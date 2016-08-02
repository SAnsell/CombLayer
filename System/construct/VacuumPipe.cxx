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
#include "SurInter.h"

#include "VacuumPipe.h"

namespace constructSystem
{

VacuumPipe::VacuumPipe(const std::string& Key) : 
  attachSystem::FixedOffset(Key,7),
  attachSystem::ContainedComp(),attachSystem::CellMap(),
  vacIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(vacIndex+1),activeFront(0),activeBack(0),
  frontJoin(0),backJoin(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

VacuumPipe::VacuumPipe(const VacuumPipe& A) : 
  attachSystem::FixedOffset(A),attachSystem::ContainedComp(A),
  attachSystem::CellMap(A),
  vacIndex(A.vacIndex),cellIndex(A.cellIndex),activeFront(A.activeFront),
  activeBack(A.activeBack),
  frontSurf(A.frontSurf),frontCut(A.frontCut),
  backSurf(A.backSurf),backCut(A.backCut),
  radius(A.radius),length(A.length),
  feThick(A.feThick),flangeRadius(A.flangeRadius),
  flangeLength(A.flangeLength),voidMat(A.voidMat),
  feMat(A.feMat)
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
      cellIndex=A.cellIndex;
      activeFront=A.activeFront;
      activeBack=A.activeBack;
      frontSurf=A.frontSurf;
      frontCut=A.frontCut;
      backSurf=A.backSurf;
      backCut=A.backCut;
      radius=A.radius;
      length=A.length;
      feThick=A.feThick;
      flangeRadius=A.flangeRadius;
      flangeLength=A.flangeLength;
      feMat=A.feMat;
      voidMat=A.voidMat;
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
  radius=Control.EvalVar<double>(keyName+"Radius");
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
  if (activeFront)
    {
      getShiftedSurf(frontSurf,101,1,flangeLength);
      if (activeWindow & 1)
	{
	  getShiftedSurf(frontSurf,1001,1,
			 (flangeLength-windowThick)/2.0);
	  getShiftedSurf(frontSurf,1002,1,
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
  if (activeBack)
    {
      getShiftedSurf(backSurf,102,-1,flangeLength);
      if (activeWindow & 2)
	{
	  getShiftedSurf(backSurf,1101,-1,
			 (flangeLength-windowThick)/2.0);
	  getShiftedSurf(backSurf,1102,-1,
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
  
  ModelSupport::buildCylinder(SMap,vacIndex+7,Origin,Y,radius);

  ModelSupport::buildCylinder(SMap,vacIndex+17,Origin,Y,radius+feThick);

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
  
  const std::string frontStr
    (activeFront ? frontSurf.display()+frontCut.display() : 
     ModelSupport::getComposite(SMap,vacIndex," 1 "));
  const std::string backStr
    (activeBack ? backSurf.display()+backCut.display() : 
     ModelSupport::getComposite(SMap,vacIndex," -2 "));

  std::string windowFrontExclude;
  std::string windowBackExclude;
  if (activeWindow & 1)      // FRONT
    { 
      Out=ModelSupport::getComposite(SMap,vacIndex,"-1007 1001 -1002 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,windowMat,0.0,
				       Out+frontCut.display()));
      addCell("Window",cellIndex-1);
      HeadRule WHR(Out);
      WHR.makeComplement();
      windowFrontExclude=WHR.display();
    }
  if (activeWindow & 2)
    { 
      Out=ModelSupport::getComposite(SMap,vacIndex,"-1007 1102 -1101");
      System.addCell(MonteCarlo::Qhull(cellIndex++,windowMat,0.0,
				       Out+backCut.display()));
      addCell("Window",cellIndex-1);
      HeadRule WHR(Out);
      WHR.makeComplement();
      windowBackExclude=WHR.display();
    }

  
  // Void 
  Out=ModelSupport::getComposite(SMap,vacIndex," -7 ");

  System.addCell(MonteCarlo::Qhull(cellIndex++,voidMat,0.0,
				   Out+frontStr+backStr+
				   windowFrontExclude+windowBackExclude));
  addCell("Void",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,vacIndex,"101 -102 -17 7");
  System.addCell(MonteCarlo::Qhull(cellIndex++,feMat,0.0,Out));
  addCell("Steel",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,vacIndex,"-101 -107 7");
  System.addCell(MonteCarlo::Qhull(cellIndex++,feMat,0.0,Out+
				   frontStr+windowFrontExclude));
  addCell("Steel",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,vacIndex,"102 -107 7");
  System.addCell(MonteCarlo::Qhull(cellIndex++,feMat,0.0,Out+
				   backStr+windowBackExclude));
  addCell("Steel",cellIndex-1);

  
  // outer void:
  Out=ModelSupport::getComposite(SMap,vacIndex,"101 -102 -107 17");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  addCell("OutVoid",cellIndex-1);

  // Outer
  Out=ModelSupport::getComposite(SMap,vacIndex,"-107 ");
  addOuterSurf(Out+frontStr+backStr);

  
  
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
  Geometry::Vec3D endPoints[2];
  std::vector<Geometry::Vec3D> Pts;
  std::vector<int> SNum;

  FixedComp::setConnect(2,Origin-X*((radius+feThick)/2.0),-X);
  FixedComp::setConnect(3,Origin+X*((radius+feThick)/2.0),X);
  FixedComp::setConnect(4,Origin-Z*((radius+feThick)/2.0),-Z);
  FixedComp::setConnect(5,Origin+Z*((radius+feThick)/2.0),Z);

  if (activeFront)
    {
      FixedComp::setLinkSurf(0,frontSurf,1,frontCut,0);
      endPoints[0]=
	SurInter::getLinePoint(Origin,Y,frontSurf,frontCut);
    }
  else
    {
      FixedComp::setLinkSurf(0,-SMap.realSurf(vacIndex+1));
      endPoints[0]=Origin-Y*(length/2.0);
    }

  if (activeBack)
    {
      FixedComp::setLinkSurf(1,backSurf,1,backCut,0);
      endPoints[1]=
	SurInter::getLinePoint(Origin,Y,backSurf,backCut);
    }
  else
    {
      FixedComp::setLinkSurf(1,SMap.realSurf(vacIndex+2));
      endPoints[1]=Origin+Y*(length/2.0);
    }

  FixedComp::setConnect(0,endPoints[0],-Y);
  FixedComp::setConnect(1,endPoints[1],Y);
  
  FixedComp::setLinkSurf(2,SMap.realSurf(vacIndex+7));
  FixedComp::setLinkSurf(3,SMap.realSurf(vacIndex+7));
  FixedComp::setLinkSurf(4,SMap.realSurf(vacIndex+7));
  FixedComp::setLinkSurf(5,SMap.realSurf(vacIndex+7));

  // MID Point: [NO SURF]
  FixedComp::setConnect(6,(endPoints[1]+endPoints[0])/2.0,Y);

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
 
  if (sideIndex==0)
    throw ColErr::EmptyValue<long int>("SideIndex cant be zero");
  
  activeFront=1;
  frontSurf=FC.getSignedMainRule(sideIndex);
  frontCut=FC.getSignedCommonRule(sideIndex);
  frontSurf.populateSurf();
  frontCut.populateSurf();

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
  
  if (sideIndex==0)
    throw ColErr::EmptyValue<long int>("SideIndex cant be zero");
  
  activeBack=1;
  backSurf=FC.getSignedMainRule(sideIndex);
  backCut=FC.getSignedCommonRule(sideIndex);
  backSurf.populateSurf();
  backCut.populateSurf();

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
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE constructSystem
