/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/VacuumTube.cxx
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

#include "VacuumTube.h"

namespace constructSystem
{

VacuumTube::VacuumTube(const std::string& Key) : 
  attachSystem::FixedOffset(Key,6),
  attachSystem::ContainedComp(),attachSystem::CellMap(),
  vacIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(vacIndex+1),activeFront(0),activeBack(0),
  activeDivide(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

VacuumTube::~VacuumTube() 
  /*!
    Destructor
  */
{}

void
VacuumTube::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("VacuumTube","populate");
  
  FixedOffset::populate(Control);

  // Void + Fe special:
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
VacuumTube::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("VacuumTube","createUnitVector");


  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  // after rotation
  Origin+=Y*(length/2.0);
  return;
}

void
VacuumTube::getShiftedSurf(const HeadRule& HR,
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
  ELog::RegMethod RegA("VacuumTube","getShiftedSurf");
  
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
VacuumTube::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("VacuumTube","createSurfaces");
  
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

  //  buildMainLayers();
  
  ModelSupport::buildCylinder(SMap,vacIndex+107,Origin,Y,flangeRadius);

  if (activeWindow)
    ModelSupport::buildCylinder(SMap,vacIndex+1007,Origin,Y,windowRadius);
  
  return;
}

void
VacuumTube::buildMainLayers()
  /*!
    Create the expanded main layers
   */
{
  ELog::RegMethod RegA("VacuumTube","buildMainLayers");

  /*
  std::set<const Geometry::Surface*> SSet=
    primSurf.getSurfaces();
  secondSurf.procString(primSurf.display());
  
  std::map<int,int> crossMapping;
  const int index(vacIndex+2001)
  for(const Geometry::Surface* SPtr : SSet)
    {
      SX=ModelSupport::surfaceCreateExpand(SPtr,feThick);
      SX->setName(index);
      SMap.registerSurf(SPtr->getName(),SX);
      index++;
    }
  
  */
  
  return;
}

  
void
VacuumTube::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
    \param FC :: FixedComp of front face
   */
{
  ELog::RegMethod RegA("VacuumTube","createObjects");

  std::string Out;

  const std::string mainStr=primSurf.display();
  const std::string secStr=secondSurf.display();
    
  const std::string frontStr
    (activeFront ? frontSurf.display() : 
     ModelSupport::getComposite(SMap,vacIndex," 1 "));
  const std::string backStr
    (activeBack ? backSurf.display() : 
     ModelSupport::getComposite(SMap,vacIndex," -2 "));
  const std::string divStr
    (activeDivide ? divideSurf.display() :  "");


  const std::string FBStr=frontStr+backStr+divStr;

  std::string windowFrontExclude;
  std::string windowBackExclude;
  if (activeWindow & 1)      // FRONT
    { 
      Out=ModelSupport::getComposite(SMap,vacIndex,"-1007 1001 -1002 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,windowMat,0.0,
				       Out+divStr));
      addCell("Window",cellIndex-1);
      HeadRule WHR(Out);
      WHR.makeComplement();
      windowFrontExclude=WHR.display();
    }
  if (activeWindow & 2)
    { 
      Out=ModelSupport::getComposite(SMap,vacIndex,"-1007 1102 -1101");
      System.addCell(MonteCarlo::Qhull(cellIndex++,windowMat,0.0,
				       Out+divStr));
      addCell("Window",cellIndex-1);
      HeadRule WHR(Out);
      WHR.makeComplement();
      windowBackExclude=WHR.display();
    }

  
  // Void 
  Out=ModelSupport::getComposite(SMap,vacIndex," -7 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,voidMat,0.0,
				   Out+FBStr+
				   windowFrontExclude+windowBackExclude));
  addCell("Void",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,vacIndex,"101 -102 -17 7");
  System.addCell(MonteCarlo::Qhull(cellIndex++,feMat,0.0,Out+divStr));
  addCell("Steel",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,vacIndex,"-101 -107 7");
  System.addCell(MonteCarlo::Qhull(cellIndex++,feMat,0.0,Out+
				   frontStr+divStr+windowFrontExclude));
  addCell("Steel",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,vacIndex,"102 -107 7");
  System.addCell(MonteCarlo::Qhull(cellIndex++,feMat,0.0,Out+
				   backStr+divStr+windowBackExclude));
  addCell("Steel",cellIndex-1);

  
  // outer void:
  Out=ModelSupport::getComposite(SMap,vacIndex,"101 -102 -107 17");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out+divStr));
  addCell("OutVoid",cellIndex-1);

  // Outer
  Out=ModelSupport::getComposite(SMap,vacIndex,"-107 ");
  addOuterSurf(Out+FBStr);

  
  
  return;
}

void
VacuumTube::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("VacuumTube","createLinks");

  FixedComp::setConnect(0,Origin-Y*(length/2.0),-Y);
  FixedComp::setConnect(1,Origin+Y*(length/2.0),Y);
  FixedComp::setConnect(2,Origin-X*((feThick)/2.0),-X);
  FixedComp::setConnect(3,Origin+X*((feThick)/2.0),X);
  FixedComp::setConnect(4,Origin-Z*((feThick)/2.0),-Z);
  FixedComp::setConnect(5,Origin+Z*((feThick)/2.0),Z);

  if (activeFront)
    FixedComp::setLinkSurf(0,frontSurf);      
  else
    FixedComp::setLinkSurf(0,-SMap.realSurf(vacIndex+1));      

  if (activeBack)
    FixedComp::setLinkSurf(1,backSurf);      
  else
    FixedComp::setLinkSurf(1,SMap.realSurf(vacIndex+2));      

  FixedComp::setLinkSurf(2,SMap.realSurf(vacIndex+7));
  FixedComp::setLinkSurf(3,SMap.realSurf(vacIndex+7));
  FixedComp::setLinkSurf(4,SMap.realSurf(vacIndex+7));
  FixedComp::setLinkSurf(5,SMap.realSurf(vacIndex+7));
  
  return;
}

void
VacuumTube::setDivider(const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Set divider surface
    \param FC :: FixedComponent 
    \param sideIndex ::  Direction to link
   */
{
  ELog::RegMethod RegA("VacuumTube","setDivider");
  
  if (sideIndex==0)
    throw ColErr::EmptyValue<long int>("SideIndex cant be zero");

  activeDivide=1;
  if (sideIndex>0)
    {
      const size_t SI(static_cast<size_t>(sideIndex-1));
      divideSurf=FC.getCommonRule(SI);
    }
  else
    {
      const size_t SI(static_cast<size_t>(-sideIndex-1));
      divideSurf=FC.getCommonRule(SI);
      divideSurf.makeComplement();
    }
  return;
}

  
  
void
VacuumTube::setFront(const attachSystem::FixedComp& FC,
		     const long int sideIndex)
  /*!
    Set front surface
    \param FC :: FixedComponent 
    \param sideIndex ::  Direction to link
   */
{
  ELog::RegMethod RegA("VacuumTube","setFront");
  
  if (sideIndex==0)
    throw ColErr::EmptyValue<long int>("SideIndex cant be zero");

  activeFront=1;
  if (sideIndex>0)
    {
      const size_t SI(static_cast<size_t>(sideIndex-1));
      frontSurf=FC.getMainRule(SI);
    }
  else
    {
      const size_t SI(static_cast<size_t>(-sideIndex-1));
      frontSurf=FC.getMainRule(SI);
      frontSurf.makeComplement();
    }
  return;
}
  
void
VacuumTube::setBack(const attachSystem::FixedComp& FC,
		     const long int sideIndex)
  /*!
    Set Back surface
    \param FC :: FixedComponent 
    \param sideIndex ::  Direction to link
   */
{
  ELog::RegMethod RegA("VacuumTube","setBack");
  
  if (sideIndex==0)
    throw ColErr::EmptyValue<long int>("SideIndex cant be zero");

  activeBack=1;
  if (sideIndex>0)
    backSurf=FC.getMainRule(static_cast<size_t>(sideIndex-1));
  else
    {
      backSurf=FC.getMainRule(static_cast<size_t>(-sideIndex-1));
      backSurf.makeComplement();
    }
  return;
}
  

  
void
VacuumTube::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int FIndex)
/*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("VacuumTube","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  createSurfaces();    
  createObjects(System);  
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE constructSystem
