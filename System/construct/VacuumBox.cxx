/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/VacuumBox.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#include "FrontBackCut.h"

#include "VacuumBox.h"

namespace constructSystem
{

VacuumBox::VacuumBox(const std::string& Key,
		       const bool flag) : 
  attachSystem::FixedOffset(Key,6),
  attachSystem::ContainedComp(),attachSystem::CellMap(),
  attachSystem::FrontBackCut(),
  centreOrigin(flag),
  vacIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(vacIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
    \param flag :: true if the box is centred on coordinates
  */
{}

VacuumBox::~VacuumBox() 
  /*!
    Destructor
  */
{}

void
VacuumBox::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("VacuumBox","populate");
  
  FixedOffset::populate(Control);

  // Void + Fe special:
  voidHeight=Control.EvalVar<double>(keyName+"VoidHeight");
  voidDepth=Control.EvalVar<double>(keyName+"VoidDepth");
  voidWidth=Control.EvalVar<double>(keyName+"VoidWidth");

  voidLength=Control.EvalVar<double>(keyName+"VoidLength");

  const double wallThick=Control.EvalDefVar<double>(keyName+"WallThick",0.0);
  feHeight=Control.EvalDefVar<double>(keyName+"FeHeight",wallThick);
  feDepth=Control.EvalDefVar<double>(keyName+"FeDepth",wallThick);
  feWidth=Control.EvalDefVar<double>(keyName+"FeWidth",wallThick);
  feFront=Control.EvalDefVar<double>(keyName+"FeFront",wallThick);
  feBack=Control.EvalDefVar<double>(keyName+"FeBack",wallThick);

  portWallThick=Control.EvalVar<double>(keyName+"PortWallThick");
  portTubeLength=Control.EvalVar<double>(keyName+"PortTubeLength");
  portTubeRadius=Control.EvalVar<double>(keyName+"PortTubeRadius");
  
  flangeRadius=Control.EvalVar<double>(keyName+"FlangeRadius");
  flangeLength=Control.EvalVar<double>(keyName+"FlangeLength");
  
  voidMat=ModelSupport::EvalDefMat<int>(Control,keyName+"VoidMat",0);
  feMat=ModelSupport::EvalMat<int>(Control,keyName+"FeMat");

  return;
}

void
VacuumBox::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("VacuumBox","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  // after rotation
  if (!centreOrigin)
    Origin+=Y*(portTubeLength+feFront+voidLength/2.0);
  return;
}


void
VacuumBox::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("VacuumBox","createSurfaces");

  // Do outer surfaces (vacuum ports)
  if (!frontActive())
    {
      ModelSupport::buildPlane(SMap,vacIndex+101,
			       Origin-Y*(portTubeLength+voidLength/2.0),Y);
      setFront(SMap.realSurf(vacIndex+101));
    }
  if (!backActive())
    {
      ModelSupport::buildPlane(SMap,vacIndex+102,
			       Origin+Y*(portTubeLength+voidLength/2.0),Y);
      setBack(-SMap.realSurf(vacIndex+102));
    }
  
  // Inner void
  ModelSupport::buildPlane(SMap,vacIndex+1,Origin-Y*(voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,vacIndex+2,Origin+Y*(voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,vacIndex+3,Origin-X*(voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,vacIndex+4,Origin+X*(voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,vacIndex+5,Origin-Z*voidDepth,Z);
  ModelSupport::buildPlane(SMap,vacIndex+6,Origin+Z*voidHeight,Z);  

  // Fe system [front face is link surf]
  ModelSupport::buildPlane(SMap,vacIndex+11,
   			   Origin-Y*(feFront+voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,vacIndex+12,
			   Origin+Y*(feBack+voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,vacIndex+13,
			   Origin-X*(feWidth+voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,vacIndex+14,
			   Origin+X*(feWidth+voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,vacIndex+15,
			   Origin-Z*(voidDepth+feDepth),Z);
  ModelSupport::buildPlane(SMap,vacIndex+16,
			   Origin+Z*(voidHeight+feHeight),Z);

  // Ports 
  ModelSupport::buildCylinder(SMap,vacIndex+107,Origin,Y,portTubeRadius);
  ModelSupport::buildCylinder(SMap,vacIndex+117,Origin,Y,
			      portTubeRadius+portWallThick);
  ModelSupport::buildCylinder(SMap,vacIndex+127,Origin,Y,
			      portTubeRadius+portWallThick+flangeRadius);


  // Flange cut
  FrontBackCut::getShiftedFront(SMap,vacIndex+111,1,Y,flangeLength);
  //  FrontBackCut::getShiftedBack(SMap,vacIndex+12,1,Y,feBack);

  // ModelSupport::buildPlane(SMap,vacIndex+111,
  //    Origin-Y*(portTubeLength+voidLength/2.0-flangeLength),Y);

  ModelSupport::buildPlane(SMap,vacIndex+112,
     Origin+Y*(portTubeLength+voidLength/2.0-flangeLength),Y);
  
  return;
}

void
VacuumBox::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("VacuumBox","createObjects");

  std::string Out;

  const std::string FPortStr(frontRule());
  const std::string BPortStr(backRule());
  
  // Main Void 
  Out=ModelSupport::getComposite(SMap,vacIndex,"1 -2 3 -4 5 -6");
  CellMap::makeCell("Void",System,cellIndex++,voidMat,0.0,Out);

  // PortVoids
  Out=ModelSupport::getComposite(SMap,vacIndex," -1 -107 ");
  CellMap::makeCell("PortVoid",System,cellIndex++,voidMat,0.0,Out+FPortStr);

  Out=ModelSupport::getComposite(SMap,vacIndex," 2 -107 ");
  CellMap::makeCell("PortVoid",System,cellIndex++,voidMat,0.0,Out+BPortStr);

  // Main metal
  Out=ModelSupport::getComposite
    (SMap,vacIndex," 11 -12 13 -14 15 -16 (-1:2:-3:4:-5:6) 107 ");
  CellMap::makeCell("MainWall",System,cellIndex++,feMat,0.0,Out);

  // Port metal
  Out=ModelSupport::getComposite(SMap,vacIndex," -11 107 -117 ");
  CellMap::makeCell("PortWall",System,cellIndex++,feMat,0.0,Out+FPortStr);

  Out=ModelSupport::getComposite(SMap,vacIndex," 12 107 -117 ");
  CellMap::makeCell("PortWall",System,cellIndex++,feMat,0.0,Out+BPortStr);

  // Flange
  Out=ModelSupport::getComposite(SMap,vacIndex," -111 117 -127 ");
  CellMap::makeCell("Flange",System,cellIndex++,feMat,0.0,Out+FPortStr);

  Out=ModelSupport::getComposite(SMap,vacIndex," 112 117 -127 ");
  CellMap::makeCell("Flange",System,cellIndex++,feMat,0.0,Out+BPortStr);

  // Flange Voids
  Out=ModelSupport::getComposite(SMap,vacIndex," 111 -11 117 -127 ");
  CellMap::makeCell("FlangeVoid",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,vacIndex," 12 -112 117 -127 ");
  CellMap::makeCell("FlangeVoid",System,cellIndex++,0,0.0,Out);
  

  Out=ModelSupport::getComposite(SMap,vacIndex," 11 -12 13 -14 15 -16 ");
  addOuterSurf(Out);

  Out=ModelSupport::getComposite(SMap,vacIndex," -127 ");
  addOuterUnionSurf(Out+FPortStr+BPortStr);      
  return;
}

void
VacuumBox::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
    Note that 0/1 are the flange surfaces
  */
{
  ELog::RegMethod RegA("VacuumBox","createLinks");

  FrontBackCut::createLinks(*this,Origin,Y);  //front and back
  FixedComp::setConnect(2,Origin-X*((feWidth+voidWidth)/2.0),-X);
  FixedComp::setConnect(3,Origin+X*((feWidth+voidWidth)/2.0),X);
  FixedComp::setConnect(4,Origin-Z*(feDepth+voidDepth),-Z);
  FixedComp::setConnect(5,Origin+Z*(feHeight+voidHeight),Z);  

  FixedComp::setLinkSurf(2,-SMap.realSurf(vacIndex+13));
  FixedComp::setLinkSurf(3,SMap.realSurf(vacIndex+14));
  FixedComp::setLinkSurf(4,-SMap.realSurf(vacIndex+15));
  FixedComp::setLinkSurf(5,SMap.realSurf(vacIndex+16));
  
  return;
}

void
VacuumBox::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("VacuumBox","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  createSurfaces();    
  createObjects(System);
  
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE constructSystem
