/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   chip/ChipIRInsert.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. 
 *
 ****************************************************************************/
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <cmath>
#include <complex>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <numeric>
#include <iterator>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfDIter.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "surfDivide.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "LineIntersectVisit.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "shutterBlock.h"
#include "SimProcess.h"
#include "SurInter.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "SecondTrack.h"
#include "TwinComp.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "GeneralShutter.h"
#include "chipDataStore.h"
#include "BulkInsert.h"
#include "cylTrack.h"
#include "LeadPlate.h"
#include "ChipIRInsert.h"

namespace shutterSystem
{

ChipIRInsert::ChipIRInsert(const size_t ID,const std::string& BKey,
			   const std::string& IKey)  : 
  BulkInsert(ID,BKey),compName(IKey),nLayers(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param ID :: Shutter number
    \param BKey :: Bulk Key
    \param IKey :: Insert Key to use
  */
{}


ChipIRInsert::ChipIRInsert(const ChipIRInsert& A) : 
  BulkInsert(A),
  compName(A.compName),voidOrigin(A.voidOrigin),Axis(A.Axis),
  zModAngle(A.zModAngle),xyModAngle(A.xyModAngle),
  fStep(A.fStep),bStep(A.bStep),radius(A.radius),
  lowCut(A.lowCut),rXDisp(A.rXDisp),rZDisp(A.rZDisp),
  frontMat(A.frontMat),backMat(A.backMat),defMat(A.defMat),
  chipInnerVoid(A.chipInnerVoid),chipOuterVoid(A.chipOuterVoid),
  CCol(A.CCol),PbA(A.PbA),PbB(A.PbB),nLayers(A.nLayers),
  cFrac(A.cFrac),cMat(A.cMat),CDivideList(A.CDivideList)
  /*!
    Copy constructor
    \param A :: ChipIRInsert to copy
  */
{}

ChipIRInsert&
ChipIRInsert::operator=(const ChipIRInsert& A)
  /*!
    Assignment operator
    \param A :: ChipIRInsert to copy
    \return *this
  */
{
  if (this!=&A)
    {
      BulkInsert::operator=(A);
      voidOrigin=A.voidOrigin;
      Axis=A.Axis;
      zModAngle=A.zModAngle;
      xyModAngle=A.xyModAngle;
      fStep=A.fStep;
      bStep=A.bStep;
      radius=A.radius;
      lowCut=A.lowCut;
      rXDisp=A.rXDisp;
      rZDisp=A.rZDisp;
      frontMat=A.frontMat;
      backMat=A.backMat;
      defMat=A.defMat;
      chipInnerVoid=A.chipInnerVoid;
      chipOuterVoid=A.chipOuterVoid;
      CCol=A.CCol;
      PbA=A.PbA;
      PbB=A.PbB;
      nLayers=A.nLayers;
      cFrac=A.cFrac;
      cMat=A.cMat;
     CDivideList=A.CDivideList;
    }
  return *this;
}

ChipIRInsert::~ChipIRInsert() 
  /*!
    Destructor
  */
{}

void
ChipIRInsert::populate(const Simulation& System)
  /*!
    Populate all the variables
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("ChipIRInsert","populate");

  const FuncDataBase& Control=System.getDataBase();
  try
    {
      zModAngle=Control.EvalDefVar<double>(compName+"ZAngle",0.0);
      xyModAngle=Control.EvalDefVar<double>(compName+"XYAngle",0.0);
      fStep=Control.EvalDefVar<double>(compName+"FStep",0.0);
      bStep=Control.EvalDefVar<double>(compName+"BStep",0.0);      
      radius=Control.EvalVar<double>(compName+"Radius");
      lowCut=Control.EvalVar<double>(compName+"LowCut");

      rXDisp=Control.EvalDefVar<double>(compName+"RXDisp",0.0);
      rZDisp=Control.EvalDefVar<double>(compName+"RZDisp",0.0);

      clearTopGap=Control.EvalVar<double>(compName+"ClearTopGap");
      clearSideGap=Control.EvalVar<double>(compName+"ClearSideGap");
      clearBaseGap=Control.EvalVar<double>(compName+"ClearBaseGap");
      clearTopOff=Control.EvalVar<double>(compName+"ClearTopOff");
      clearSideOff=Control.EvalVar<double>(compName+"ClearSideOff");
      clearBaseOff=Control.EvalVar<double>(compName+"ClearBaseOff");


      frontMat=ModelSupport::EvalMat<int>(Control,compName+"FrontMat");
      backMat=ModelSupport::EvalMat<int>(Control,compName+"BackMat");
      defMat=ModelSupport::EvalMat<int>(Control,compName+"DefMat");

      // Layers
      nLayers=Control.EvalDefVar<size_t>(compName+"NLayers",0);
      ModelSupport::populateDivide(Control,nLayers,
				   compName+"Frac_",cFrac);
      ModelSupport::populateDivide(Control,nLayers,
				   compName+"Mat_",defMat,cMat);
      
    }
  // Exit and don't report if we are not using this scatter plate
  catch (ColErr::InContainerError<std::string>& EType)
    {
      ELog::EM<<"ChipIRInsert "<<compName<<" not in use Var:"
	      <<EType.getItem()<<ELog::endWarn;
    }
  return;
}

void
ChipIRInsert::createUnitVector()
  /*!
    Create the unit vectors
  */
{
  ELog::RegMethod RegA("ChipIRInsert","createUnitVector");

  //  const masterRotate& MR=masterRotate::Instance();
  //  chipIRDatum::chipDataStore& CS=chipIRDatum::chipDataStore::Instance();

  // ADDITIONAL ROTATIONS TO 
  const Geometry::Quaternion Qz=
    Geometry::Quaternion::calcQRotDeg(zModAngle,X);
  const Geometry::Quaternion Qxy=
    Geometry::Quaternion::calcQRotDeg(xyModAngle,Z);
  
  Axis=Y;
  Qz.rotate(Axis);
  Qxy.rotate(Axis);
  
  // Modification of ExitPoint
  const Geometry::Cylinder* OutCyl=
    SMap.realPtr<const Geometry::Cylinder>(surfIndex+27);
  voidOrigin=OutCyl->getCentre();

  MonteCarlo::LineIntersectVisit 
    AxisLine(Origin+Z*rZDisp+X*rXDisp,Axis);
  setExit(AxisLine.getPoint(OutCyl,DPlane,-1),Axis);

  return;
}

void
ChipIRInsert::createSurfaces()
  /*!
    Create all the surfaces
  */
{
  ELog::RegMethod RegA("ChipIRInsert","createSurface");
  // Front plane
  if (fStep>0.0)
    ModelSupport::buildCylinder(SMap,surfIndex+101,
				voidOrigin,Z,innerRadius+fStep);		  else
    SMap.addMatch(surfIndex+101,surfIndex+7);          // Outer top

  // Back plane
  if (bStep>0.0)
    {
      ModelSupport::buildCylinder(SMap,surfIndex+102,
				  voidOrigin,Z,outerRadius-bStep);
    }
  else
    SMap.addMatch(surfIndex+102,surfIndex+27);          // Outer top

  // Central cylinder
  ModelSupport::buildCylinder(SMap,surfIndex+107,
			      Origin+X*rXDisp+Z*rZDisp,Axis,radius);

  // Low cut
  // Measured from the lowest point of the 
  // cylinder upwards
  if (fabs(lowCut)>Geometry::zeroTol)
    {
      ModelSupport::buildPlane(SMap,surfIndex+115,
			       Origin+X*rXDisp+Z*(lowCut+rZDisp-radius),Z);
    }
  
  
  return;
}

void
ChipIRInsert::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("ChipIRInsert","createObjects");
  CDivideList.clear();
  const std::string dSurf=divideStr();
  std::string Out;
  if (fabs(lowCut)>Geometry::zeroTol)
    {
      // Low cut fragment [Inner]:
      Out=ModelSupport::getComposite(SMap,surfIndex,
				     "-115 -107 101 -17 3 -4 -5 6 ")+dSurf;
      System.addCell(MonteCarlo::Qhull(cellIndex++,defMat,0.0,Out));
      // Low cut fragment [Outer]:
      Out=ModelSupport::getComposite(SMap,surfIndex,
				     "-115 -107 17 -102 13 -14 -15 16 ")+dSurf;
      System.addCell(MonteCarlo::Qhull(cellIndex++,defMat,0.0,Out));
      // Inner void
      Out=ModelSupport::getComposite(SMap,surfIndex,
				     "115 -107 101 -17 3 -4 -5 6 ")+dSurf;
      System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
      chipInnerVoid=cellIndex-1;

      // outer Void
      Out=ModelSupport::getComposite(SMap,surfIndex,
				     "115 -107 17 -102 13 -14 -15 16 ")+dSurf;
      System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
      chipOuterVoid=cellIndex-1;

      // Set Containers:
      Out=ModelSupport::getComposite(SMap,surfIndex," 115 -107 3 -4 -5 ");
      addBoundarySurf("inner",Out);
      Out=ModelSupport::getComposite(SMap,surfIndex," 115 -107 13 -14 -15 ");
      addBoundarySurf("outer",Out);
    }
  else
    {
      // Inner void
      Out=ModelSupport::getComposite(SMap,surfIndex,
				     "-107 101 -17 3 -4 -5 6 ")+dSurf;
      System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
      chipInnerVoid=cellIndex-1;
      // Outer void
      Out=ModelSupport::getComposite(SMap,surfIndex,
				     "-107 17 -102 13 -14 -15 16 ")+dSurf;
      System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
      chipOuterVoid=cellIndex-1;

      Out=ModelSupport::getComposite(SMap,surfIndex,"-107 3 -4 -5 6 ");
      addBoundarySurf("inner",Out);
      Out=ModelSupport::getComposite(SMap,surfIndex,"-107 13 -14 -15 16 ");
      addBoundarySurf("outer",Out);

    }
  // Inner Material:
  Out=ModelSupport::getComposite(SMap,surfIndex,
				 "107 101 -17 3 -4 -5 6 ")+dSurf;
  System.addCell(MonteCarlo::Qhull(cellIndex++,defMat,0.0,Out));
  CDivideList.push_back(cellIndex-1);

  // Inner edge void
  if (fStep>0.0)
    {
      Out=ModelSupport::getComposite(SMap,surfIndex,
				     "7 -101 3 -4 -5 6 ")+dSurf;
      System.addCell(MonteCarlo::Qhull(cellIndex++,frontMat,0.0,Out));
    }

  // Outer Material:
  Out=ModelSupport::getComposite(SMap,surfIndex,
				 "107 17 -27 -102 13 -14 -15 16 ")+dSurf;
  System.addCell(MonteCarlo::Qhull(cellIndex++,defMat,0.0,Out));
  //  CDivideList.push_back(cellIndex-1);

  // Outer edge void
  if (bStep>0.0)
    {
      Out=ModelSupport::getComposite(SMap,surfIndex,
				     "102 -27 13 -14 -15 16 ")+dSurf;
      System.addCell(MonteCarlo::Qhull(cellIndex++,backMat,0.0,Out));
    }

  System.removeCell(innerVoid);
  System.removeCell(outerVoid);
  return;
}

void 
ChipIRInsert::layerProcess(Simulation& System)
  /*!
    Processes the splitting of the surfaces into a multilayer system
    This has to deal with the three layers that invade cells:
    
    \param System :: Simulation to work on
  */
{
  ELog::RegMethod RegA("ChipIRInsert","LayerProcess");
 
  if (nLayers<=0) return;
  ModelSupport::surfDivide DA;
  // Generic
  for(size_t i=0;i<static_cast<size_t>(nLayers-1);i++)
    {
      DA.addFrac(cFrac[i]);
      DA.addMaterial(cMat[i]);
    }
  DA.addMaterial(cMat[static_cast<size_t>(nLayers-1)]);
  const int CSize=static_cast<int>(CDivideList.size());

  for(int i=0;i<CSize;i++)
    {
      DA.init();
      // Cell Specific:
      DA.setCellN(CDivideList[static_cast<size_t>(i)]);
      DA.setOutNum(cellIndex,surfIndex+501+100*i);
      DA.makePair<Geometry::Cylinder,Geometry::Plane>
	(SMap.realSurf(surfIndex+107),-SMap.realSurf(surfIndex+3+10*i));
      DA.addOuterSingle(SMap.realSurf(surfIndex+4+10*i));
      DA.addOuterSingle(SMap.realSurf(surfIndex+5+10*i));
      DA.addOuterSingle(SMap.realSurf(surfIndex+6+10*i));
       
      DA.activeDivide(System);
      cellIndex=DA.getCellNum();
    }
  return;
}

void
ChipIRInsert::createDatumPoints() const
  /*!
    Create the datum points based on the current model
  */
{
  ELog::RegMethod RegA("ChipIRInsert","createDatumPoints");

  const masterRotate& MR=masterRotate::Instance();
  chipIRDatum::chipDataStore& CS=chipIRDatum::chipDataStore::Instance();

  const Geometry::Cylinder* inPlate=
    SMap.realPtr<const Geometry::Cylinder>(surfIndex+101);
  const Geometry::Cylinder* outPlate=
    SMap.realPtr<const Geometry::Cylinder>(surfIndex+102);

  const Geometry::Cylinder* fPlate=
    SMap.realPtr<const Geometry::Cylinder>(surfIndex+7);
  const Geometry::Cylinder* bPlate=
    SMap.realPtr<const Geometry::Cylinder>(surfIndex+27);
  
  MonteCarlo::LineIntersectVisit 
    AxisLine(Origin+Z*rZDisp+X*rXDisp,Axis);

  const Geometry::Vec3D APt=AxisLine.getPoint(inPlate,DPlane,-1);
  const Geometry::Vec3D BPt=AxisLine.getPoint(outPlate,DPlane,-1);
  const Geometry::Vec3D CPt=AxisLine.getPoint(fPlate,DPlane,-1);
  const Geometry::Vec3D DPt=AxisLine.getPoint(bPlate,DPlane,-1);
  
  CS.setCNum(chipIRDatum::insertStart,MR.calcRotate(APt));
  CS.setCNum(chipIRDatum::insertEnd,MR.calcRotate(BPt));
  CS.setCNum(chipIRDatum::insertAxis,MR.calcAxisRotate(Axis));
  CS.setVNum(chipIRDatum::insertRadius,radius);
  return;
}


void
ChipIRInsert::createLeadPlate(Simulation& System)
  /*!
    Create the Lead Plate collimator
    \param System :: Simulation to build into
  */
{
  ELog::RegMethod RegA("ChipIRInsert","createLeadPlate");
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();


  if (!PbA)
    PbA=std::shared_ptr<LeadPlate>(new LeadPlate("chipPb1"));

  PbA->addBoundarySurf(this->getCompContainer("inner"));
  PbA->setInsertCell(chipInnerVoid);
  PbA->createAll(System,*this,1);
  OR.addObject(PbA);

  // Outer
  if (!PbB)
    PbB=std::shared_ptr<LeadPlate>(new LeadPlate("chipPb2"));

  PbB->addBoundarySurf(this->getCompContainer("outer"));
  PbB->setInsertCell(chipOuterVoid);
  PbB->createAll(System,*this,2);
  OR.addObject(PbB);

  return;
}

void
ChipIRInsert::createAll(Simulation& System,
			const shutterSystem::GeneralShutter& GS)
  /*!
    Create the shutter
    \param System :: Simulation to process
    \param GS :: GeneralShutter object to use
   */
{
  ELog::RegMethod RegA("ChipIRInsert","createAll");
  BulkInsert::createAll(System,GS);
  populate(System);
  createUnitVector();
  createSurfaces();
  createObjects(System);
  createLeadPlate(System);
  layerProcess(System);
  createDatumPoints();
  return;
}
  
}  // NAMESPACE shutterSystem
