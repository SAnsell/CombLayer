/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/VacuumPipe.cxx
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#include <memory>
#include <array>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"  
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "FrontBackCut.h"
#include "SurfMap.h"
#include "SurInter.h"
#include "surfDivide.h"

#include "VacuumPipe.h"

namespace constructSystem
{

VacuumPipe::VacuumPipe(const std::string& Key) : 
  attachSystem::FixedOffset(Key,11),
  attachSystem::ContainedComp(),attachSystem::CellMap(),
  attachSystem::FrontBackCut(),
  vacIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(vacIndex+1),frontJoin(0),backJoin(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

VacuumPipe::VacuumPipe(const VacuumPipe& A) : 
  attachSystem::FixedOffset(A),attachSystem::ContainedComp(A),
  attachSystem::CellMap(A),attachSystem::FrontBackCut(A),
  vacIndex(A.vacIndex),cellIndex(A.cellIndex),
  frontJoin(A.frontJoin),FPt(A.FPt),FAxis(A.FAxis),
  backJoin(A.backJoin),BPt(A.BPt),BAxis(A.BAxis),
  radius(A.radius),length(A.length),feThick(A.feThick),
  flangeRadius(A.flangeRadius),flangeLength(A.flangeLength),
  activeWindow(A.activeWindow),windowThick(A.windowThick),
  windowRadius(A.windowRadius),voidMat(A.voidMat),
  feMat(A.feMat),windowMat(A.windowMat),
  nDivision(A.nDivision)
  /*!
    Copy constructor
    \param A :: VacuumPipe to copy
  */
{}

VacuumPipe&
VacuumPipe::operator=(const VacuumPipe& A)
  /*!
    Assignment operator
    \param A :: VacuumPipe to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedOffset::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::FrontBackCut::operator=(A);
      cellIndex=A.cellIndex;
      frontJoin=A.frontJoin;
      FPt=A.FPt;
      FAxis=A.FAxis;
      backJoin=A.backJoin;
      BPt=A.BPt;
      BAxis=A.BAxis;
      radius=A.radius;
      length=A.length;
      feThick=A.feThick;
      flangeRadius=A.flangeRadius;
      flangeLength=A.flangeLength;
      activeWindow=A.activeWindow;
      windowThick=A.windowThick;
      windowRadius=A.windowRadius;
      voidMat=A.voidMat;
      feMat=A.feMat;
      windowMat=A.windowMat;
      nDivision=A.nDivision;
    }
  return *this;
}

VacuumPipe::~VacuumPipe() 
  /*!
    Destructor
  */
{}

void
VacuumPipe::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("VacuumPipe","populate");
  
  FixedOffset::populate(Control);

  // Void + Fe special:
  radius=Control.EvalDefVar<double>(keyName+"Radius",-1.0);
  height=Control.EvalDefVar<double>(keyName+"Height",-1.0);
  width=Control.EvalDefVar<double>(keyName+"Width",-1.0);

  if (radius<0.0 && (width<0.0 || height<0.0))
    throw ColErr::EmptyContainer("Pipe has neither Radius or Height/Width");
  
  length=Control.EvalVar<double>(keyName+"Length");

  feThick=Control.EvalVar<double>(keyName+"FeThick");
  flangeRadius=Control.EvalVar<double>(keyName+"FlangeRadius");
  flangeLength=Control.EvalVar<double>(keyName+"FlangeLength");
  
  activeWindow=Control.EvalDefVar<int>(keyName+"WindowActive",0);
  windowThick=Control.EvalDefVar<double>(keyName+"WindowThick",0.0);
  windowRadius=Control.EvalDefVar<double>(keyName+"WindowRadius",0.0);
  windowMat=ModelSupport::EvalDefMat<int>(Control,keyName+"WindowMat",0);
  
  voidMat=ModelSupport::EvalDefMat<int>(Control,keyName+"VoidMat",0);
  feMat=ModelSupport::EvalMat<int>(Control,keyName+"FeMat");

  nDivision=Control.EvalDefVar<size_t>(keyName+"NDivision",0);
  return;
}

void
VacuumPipe::createUnitVector(const attachSystem::FixedComp& FC,
                             const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("VacuumPipe","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  // after rotation
  applyActiveFrontBack();

  return;
}


void
VacuumPipe::applyActiveFrontBack()
  /*!
    Apply the active front/back point to re-calcuate Origin
    It applies the rotation of Y to Y' to both X/Z to preserve
    orthogonality.
   */
{
  ELog::RegMethod RegA("VacuumPipe","applyActiveFrontBack");


  const Geometry::Vec3D curFP=(frontJoin) ? FPt : Origin;
  const Geometry::Vec3D curBP=(backJoin) ? BPt : Origin+Y*length;

  Origin=(curFP+curBP)/2.0;
  Geometry::Vec3D YAxis=(curBP-curFP).unit();
  const Geometry::Quaternion QR=
    Geometry::Quaternion::calcQVRot(Y,YAxis);
  QR.rotate(X);
  QR.rotate(Z);
  Y=YAxis;
  
  return;
}
  
void
VacuumPipe::getShiftedSurf(const HeadRule& HR,
			   const int index,
			   const int dFlag,
			   const double length)
  /*!
    Support function to calculate the shifted surface
    \param HR :: HeadRule to extract plane surf
    \param index :: offset index
    \param dFlag :: direction flag
    \param length :: length to shift by
   */
{
  ELog::RegMethod RegA("VacuumPipe","getShiftedSurf");
  
  std::set<int> FS=HR.getSurfSet();
  for(const int& SN : FS)
    {
      const Geometry::Surface* SPtr=SMap.realSurfPtr(SN);

      const Geometry::Plane* PPtr=
	dynamic_cast<const Geometry::Plane*>(SPtr);
      if (PPtr)
	{
	  if (SN*dFlag>0)
	    ModelSupport::buildShiftedPlane
	      (SMap,vacIndex+index,PPtr,dFlag*length);
	  else
	    ModelSupport::buildShiftedPlaneReversed
	      (SMap,vacIndex+index,PPtr,dFlag*length);
	  
	  return;
	}
      const Geometry::Cylinder* CPtr=
	dynamic_cast<const Geometry::Cylinder*>(SPtr);
      // Cylinder case:
      if (CPtr)
	{
	  if (SN>0)
	    ModelSupport::buildCylinder
	      (SMap,vacIndex+index,CPtr->getCentre()+Y*length,
	       CPtr->getNormal(),CPtr->getRadius());
	  else
	    ModelSupport::buildCylinder
	      (SMap,vacIndex+index,CPtr->getCentre()-Y*flangeLength,
	       CPtr->getNormal(),CPtr->getRadius());
	  return;
	}
    }
  throw ColErr::EmptyValue<int>("HeadRule contains no planes/cylinder");
} 

void
VacuumPipe::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("VacuumPipe","createSurfaces");
  
  // middle of the flange
  const double midFlange((length-flangeLength)/2.0);
  
  // Inner void
  if (frontActive())
    {
      getShiftedSurf(getFrontRule(),101,1,flangeLength);
      if (activeWindow & 1)
	{
	  getShiftedSurf(getFrontRule(),1001,1,
			 (flangeLength-windowThick)/2.0);
	  getShiftedSurf(getFrontRule(),1002,1,
			 (flangeLength+windowThick)/2.0);
	}
    }
  else
    {
      ModelSupport::buildPlane(SMap,vacIndex+1,Origin-Y*(length/2.0),Y);
      ModelSupport::buildPlane(SMap,vacIndex+101,
			       Origin-Y*(length/2.0-flangeLength),Y);
      if (activeWindow & 1)
	{

	  ModelSupport::buildPlane(SMap,vacIndex+1001,
				   Origin-Y*(midFlange+windowThick/2.0),Y);
	  ModelSupport::buildPlane(SMap,vacIndex+1002,
				   Origin-Y*(midFlange-windowThick/2.0),Y);
	}	    
    }
    // Inner void
  if (backActive())
    {
      getShiftedSurf(getBackRule(),102,-1,flangeLength);
      if (activeWindow & 2)
	{
	  getShiftedSurf(getBackRule(),1101,-1,
			 (flangeLength-windowThick)/2.0);
	  getShiftedSurf(getBackRule(),1102,-1,
			 (flangeLength+windowThick)/2.0);
	}
    }
  else
    {
      ModelSupport::buildPlane(SMap,vacIndex+2,Origin+Y*(length/2.0),Y);
      ModelSupport::buildPlane(SMap,vacIndex+102,
			       Origin+Y*(length/2.0-flangeLength),Y);
	    
      if (activeWindow & 2)
	{

	  ModelSupport::buildPlane(SMap,vacIndex+1101,
				   Origin+Y*(midFlange+windowThick/2.0),Y);
	  ModelSupport::buildPlane(SMap,vacIndex+1102,
				   Origin+Y*(midFlange-windowThick/2.0),Y);
	}	    

    }
  
  if (radius>0.0)
    {
      ModelSupport::buildCylinder(SMap,vacIndex+7,Origin,Y,radius);
      ModelSupport::buildCylinder(SMap,vacIndex+17,Origin,Y,radius+feThick);
    }
  else
    {
      ModelSupport::buildPlane(SMap,vacIndex+3,Origin-X*(width/2.0),X);
      ModelSupport::buildPlane(SMap,vacIndex+4,Origin+X*(width/2.0),X);
      ModelSupport::buildPlane(SMap,vacIndex+5,Origin-Z*(height/2.0),Z);
      ModelSupport::buildPlane(SMap,vacIndex+6,Origin+Z*(height/2.0),Z);

      ModelSupport::buildPlane(SMap,vacIndex+13,Origin-X*(feThick+width/2.0),X);
      ModelSupport::buildPlane(SMap,vacIndex+14,Origin+X*(feThick+width/2.0),X);
      ModelSupport::buildPlane(SMap,vacIndex+15,Origin-Z*(feThick+height/2.0),Z);
      ModelSupport::buildPlane(SMap,vacIndex+16,Origin+Z*(feThick+height/2.0),Z);
    }

  ModelSupport::buildCylinder(SMap,vacIndex+107,Origin,Y,flangeRadius);
  if (activeWindow)
    ModelSupport::buildCylinder(SMap,vacIndex+1007,Origin,Y,windowRadius);
  
  return;
}

void
VacuumPipe::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("VacuumPipe","createObjects");

  std::string Out;
  
  const std::string frontStr=
    frontActive() ? frontRule() :
    ModelSupport::getComposite(SMap,vacIndex," 1 ");
  const std::string backStr=
    backActive() ? backRule() :
     ModelSupport::getComposite(SMap,vacIndex," -2 ");

  std::string windowFrontExclude;
  std::string windowBackExclude;
  if (activeWindow & 1)      // FRONT
    { 
      Out=ModelSupport::getComposite(SMap,vacIndex,"-1007 1001 -1002 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,windowMat,0.0,
				       Out+frontBridgeRule()));
      addCell("Window",cellIndex-1);

      HeadRule WHR(Out);
      WHR.makeComplement();
      windowFrontExclude=WHR.display();
    }
  if (activeWindow & 2)
    { 
      Out=ModelSupport::getComposite(SMap,vacIndex,"-1007 1102 -1101");
      System.addCell(MonteCarlo::Qhull(cellIndex++,windowMat,0.0,
				       Out+backBridgeRule()));
      addCell("Window",cellIndex-1);
      HeadRule WHR(Out);
      WHR.makeComplement();
      windowBackExclude=WHR.display();
    }

  
  // Void
  Out=ModelSupport::getSetComposite(SMap,vacIndex," -7 3 -4 5 -6");
  HeadRule InnerVoid(Out);
  InnerVoid.makeComplement();
  System.addCell(MonteCarlo::Qhull(cellIndex++,voidMat,0.0,
				   Out+frontStr+backStr+
				   windowFrontExclude+windowBackExclude));
  addCell("Void",cellIndex-1);

  Out=ModelSupport::getSetComposite(SMap,vacIndex," -17 13 -14 15 -16");
  HeadRule WallLayer(Out);
  
  Out=ModelSupport::getComposite(SMap,vacIndex,"101 -102 ");
  Out+=WallLayer.display()+InnerVoid.display();
  System.addCell(MonteCarlo::Qhull(cellIndex++,feMat,0.0,Out));
  addCell("Steel",cellIndex-1);
  addCell("MainSteel",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,vacIndex,"-101 -107 ");
  Out+=InnerVoid.display();
  System.addCell(MonteCarlo::Qhull(cellIndex++,feMat,0.0,Out+
				   frontStr+windowFrontExclude));
  addCell("Steel",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,vacIndex,"102 -107 ");
  Out+=InnerVoid.display()+backStr+windowBackExclude;
  System.addCell(MonteCarlo::Qhull(cellIndex++,feMat,0.0,Out));
  addCell("Steel",cellIndex-1);

  
  // outer void:
  WallLayer.makeComplement();
  Out=ModelSupport::getComposite(SMap,vacIndex,"101 -102 -107 ");
  Out+=WallLayer.display();
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  addCell("OutVoid",cellIndex-1);

  // Outer
  Out=ModelSupport::getComposite(SMap,vacIndex,"-107 ");
  addOuterSurf(Out+frontStr+backStr);
  
  return;
}


void
VacuumPipe::createDivision(Simulation& System)
  /*!
    Divide the vacuum pipe into sections if needed
    \param System :: Simulation
  */
{
  ELog::RegMethod RegA("VacuumPipe","createDivision");
  if (nDivision>1)
    {
      ModelSupport::surfDivide DA;
      DA.setBasicSplit(nDivision,feMat);
      
      DA.init();
      DA.setCellN(getCell("MainSteel"));
      DA.setOutNum(cellIndex,vacIndex+1000);
      DA.makePair<Geometry::Plane>(SMap.realSurf(vacIndex+101),
				   SMap.realSurf(vacIndex+102));

      DA.activeDivide(System);
      cellIndex=DA.getCellNum();
      removeCell("MainSteel");
      addCells("MainSteel",DA.getCells());
    }
   
  return;
}
  
void
VacuumPipe::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("VacuumPipe","createLinks");

  //stufff for intersection

  FixedComp::setConnect(2,Origin-X*radius,-X);
  FixedComp::setConnect(3,Origin+X*radius,X);
  FixedComp::setConnect(4,Origin-Z*radius,-Z);
  FixedComp::setConnect(5,Origin+Z*radius,Z);

  FrontBackCut::createLinks(*this,Origin,Y);  //front and back
  if (!backActive())
    {
      FixedComp::setLinkSurf(0,-SMap.realSurf(vacIndex+1));
      FixedComp::setConnect(0,Origin-Y*(length/2.0),Y);
    }

  if (!backActive())
    {
      FixedComp::setLinkSurf(1,SMap.realSurf(vacIndex+2));
      FixedComp::setConnect(1,Origin+Y*(length/2.0),Y);
    }

  
  FixedComp::setLinkSurf(2,SMap.realSurf(vacIndex+7));
  FixedComp::setLinkSurf(3,SMap.realSurf(vacIndex+7));
  FixedComp::setLinkSurf(4,SMap.realSurf(vacIndex+7));
  FixedComp::setLinkSurf(5,SMap.realSurf(vacIndex+7));

  // MID Point: [NO SURF]
  const Geometry::Vec3D midPt=
    (getSignedLinkPt(1)+getSignedLinkPt(2))/2.0;
  FixedComp::setConnect(6,midPt,Y);
  
  FixedComp::setConnect(7,Origin-Z*(radius+feThick),-Z);
  FixedComp::setConnect(8,Origin+Z*(radius+feThick),Z);
  FixedComp::setConnect(9,Origin-Z*flangeRadius,-Z);
  FixedComp::setConnect(10,Origin+Z*flangeRadius,Z);

  FixedComp::setLinkSurf(7,SMap.realSurf(vacIndex+17));
  FixedComp::setLinkSurf(8,SMap.realSurf(vacIndex+17));
  FixedComp::setLinkSurf(9,SMap.realSurf(vacIndex+107));
  FixedComp::setLinkSurf(10,SMap.realSurf(vacIndex+107));

  return;
}
  
void
VacuumPipe::setFront(const attachSystem::FixedComp& FC,
		     const long int sideIndex,
		     const bool joinFlag)
  /*!
    Set front surface
    \param FC :: FixedComponent 
    \param sideIndex ::  Direction to link
    \param joinFlag :: joint front to link object 
   */
{
  ELog::RegMethod RegA("VacuumPipe","setFront");

  
  FrontBackCut::setFront(FC,sideIndex);
  if (joinFlag)
    {
      frontJoin=1;
      FPt=FC.getSignedLinkPt(sideIndex);
      FAxis=FC.getSignedLinkAxis(sideIndex);
    }
    
  return;
}
  
void
VacuumPipe::setBack(const attachSystem::FixedComp& FC,
		    const long int sideIndex,
		    const bool joinFlag)
  /*!
    Set Back surface
    \param FC :: FixedComponent 
    \param sideIndex ::  Direction to link
    \param joinFlag :: joint front to link object 
   */
{
  ELog::RegMethod RegA("VacuumPipe","setBack");
  
  FrontBackCut::setBack(FC,sideIndex);
  if (joinFlag)
    {
      backJoin=1;
      BPt=FC.getSignedLinkPt(sideIndex);
      BAxis=FC.getSignedLinkAxis(sideIndex);
    }
  return;
}
  
void
VacuumPipe::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int FIndex)
 /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("VacuumPipe","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  createSurfaces();    
  createObjects(System);
  createLinks();
  createDivision(System);
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE constructSystem
