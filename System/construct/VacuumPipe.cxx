/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/VacuumPipe.cxx
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#include "CellMap.h"

#include "VacuumPipe.h"

namespace constructSystem
{

VacuumPipe::VacuumPipe(const std::string& Key) : 
  attachSystem::FixedOffset(Key,6),
  attachSystem::ContainedComp(),attachSystem::CellMap(),
  vacIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(vacIndex+1),activeFront(0),activeBack(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

VacuumPipe::VacuumPipe(const VacuumPipe& A) : 
  attachSystem::FixedOffset(A),attachSystem::ContainedComp(A),
  attachSystem::CellMap(A),
  vacIndex(A.vacIndex),cellIndex(A.cellIndex),activeFront(A.activeFront),
  activeBack(A.activeBack),frontSurf(A.frontSurf),
  backSurf(A.backSurf),radius(A.radius),length(A.length),
  feThick(A.feThick),flangeRadius(A.flangeRadius),
  flangeLength(A.flangeLength),feMat(A.feMat)
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
      backSurf=A.backSurf;
      radius=A.radius;
      length=A.length;
      feThick=A.feThick;
      flangeRadius=A.flangeRadius;
      flangeLength=A.flangeLength;
      feMat=A.feMat;
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
  Origin+=Y*(length/2.0);
  return;
}

void
VacuumPipe::getShiftedSurf(const HeadRule& HR,
			   const int index,
			   const int dFlag)
  /*!
    Support function to calculate the shifted surface
    \param HR :: HeadRule to extract plane surf
    \param index :: offset index
    \param dFlag :: direction flag
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
	      (SMap,vacIndex+index,PPtr,dFlag*flangeLength);
	  else
	    ModelSupport::buildShiftedPlaneReversed
	      (SMap,vacIndex+index,PPtr,dFlag*flangeLength);
	  
	  return;
	}
      const Geometry::Cylinder* CPtr=
	dynamic_cast<const Geometry::Cylinder*>(SPtr);
      // Cylinder case:
      if (CPtr)
	{
	  if (SN*dFlag>0)
	    ModelSupport::buildCylinder
	      (SMap,vacIndex+index,CPtr->getCentre()-Y*flangeLength,
	       CPtr->getNormal(),CPtr->getRadius());
	  else
	    ModelSupport::buildCylinder
	      (SMap,vacIndex+index,CPtr->getCentre()+Y*flangeLength,
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

  // Inner void
  if (activeFront)
    getShiftedSurf(frontSurf,101,1);
  else
    {
      ModelSupport::buildPlane(SMap,vacIndex+1,Origin-Y*(length/2.0),Y);
      ModelSupport::buildPlane(SMap,vacIndex+101,
			       Origin-Y*(length/2.0-flangeLength),Y);
    }

    // Inner void
  if (activeBack)
    getShiftedSurf(backSurf,102,-1);
  else
    {
      ModelSupport::buildPlane(SMap,vacIndex+2,Origin+Y*(length/2.0),Y);
      ModelSupport::buildPlane(SMap,vacIndex+102,
			       Origin+Y*(length/2.0-flangeLength),Y);
    }
  
  ModelSupport::buildCylinder(SMap,vacIndex+7,Origin,Y,radius);

  ModelSupport::buildCylinder(SMap,vacIndex+17,Origin,Y,radius+feThick);

  ModelSupport::buildCylinder(SMap,vacIndex+107,Origin,Y,flangeRadius);

  
  return;
}

void
VacuumPipe::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
    \param FC :: FixedComp of front face
   */
{
  ELog::RegMethod RegA("VacuumPipe","createObjects");

  std::string Out;
  
  const std::string frontStr
    (activeFront ? frontSurf.display() : 
     ModelSupport::getComposite(SMap,vacIndex," 1 "));
  const std::string backStr
    (activeBack ? backSurf.display() : 
     ModelSupport::getComposite(SMap,vacIndex," -2 "));
  const std::string FBStr=frontStr+backStr;


  // Void 
  Out=ModelSupport::getComposite(SMap,vacIndex," -7 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out+FBStr));
  addCell("Void",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,vacIndex,"101 -102 -17 7");
  System.addCell(MonteCarlo::Qhull(cellIndex++,feMat,0.0,Out));
  addCell("Steel",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,vacIndex,"-101 -107 7");
  System.addCell(MonteCarlo::Qhull(cellIndex++,feMat,0.0,Out+frontStr));
  addCell("Steel",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,vacIndex,"102 -107 7");
  System.addCell(MonteCarlo::Qhull(cellIndex++,feMat,0.0,Out+backStr));
  addCell("Steel",cellIndex-1);

  // outer void:
  Out=ModelSupport::getComposite(SMap,vacIndex,"101 -102 -107 17");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  addCell("OutVoid",cellIndex-1);

  // Outer
  Out=ModelSupport::getComposite(SMap,vacIndex,"-107 ");
  addOuterSurf(Out+FBStr);

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

  FixedComp::setConnect(0,Origin-Y*(length/2.0),-Y);
  FixedComp::setConnect(1,Origin-Y*(length/2.0),-Y);
  FixedComp::setConnect(2,Origin-X*((radius+feThick)/2.0),-X);
  FixedComp::setConnect(3,Origin+X*((radius+feThick)/2.0),X);
  FixedComp::setConnect(4,Origin-Z*((radius+feThick)/2.0),-Z);
  FixedComp::setConnect(5,Origin+Z*((radius+feThick)/2.0),Z);

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
VacuumPipe::setFront(const attachSystem::FixedComp& FC,
		     const long int sideIndex)
  /*!
    Set front surface
    \param FC :: FixedComponent 
    \param sideIndex ::  Direction to link
   */
{
  ELog::RegMethod RegA("VacuumPipe","setFront");
  
  if (sideIndex==0)
    throw ColErr::EmptyValue<long int>("SideIndex cant be zero");

  activeFront=1;
  if (sideIndex>0)
    frontSurf=FC.getMainRule(static_cast<size_t>(sideIndex-1));
  else
    {
      frontSurf=FC.getMainRule(static_cast<size_t>(-sideIndex-1));
      frontSurf.makeComplement();
    }
  return;
}
  
void
VacuumPipe::setBack(const attachSystem::FixedComp& FC,
		     const long int sideIndex)
  /*!
    Set Back surface
    \param FC :: FixedComponent 
    \param sideIndex ::  Direction to link
   */
{
  ELog::RegMethod RegA("VacuumPipe","setBack");
  
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
