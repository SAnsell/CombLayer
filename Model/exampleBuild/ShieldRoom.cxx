/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   exmapleBuild/BremBlock.cxx
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#include "groupRange.h"
#include "objectGroups.h"
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
#include "ExternalCut.h"

#include "ShieldRoom.h"

namespace exampleSystem
{

ShieldRoom::ShieldRoom(const std::string& Key) :
  attachSystem::FixedOffset(Key,2),
  attachSystem::ContainedComp(),
  attachSystem::CellMap()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}
  
ShieldRoom::~ShieldRoom() 
  /*!
    Destructor
  */
{}

void
ShieldRoom::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("ShieldRoom","populate");
  
  FixedOffset::populate(Control);

  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  depth=Control.EvalVar<double>(keyName+"Depth");
  length=Control.EvalVar<double>(keyName+"Length");
  sideThick=Control.EvalVar<double>(keyName+"SideThick");
  roofThick=Control.EvalVar<double>(keyName+"RoofThick");
  floorThick=Control.EvalVar<double>(keyName+"FloorThick");

  sideExtra=Control.EvalVar<double>(keyName+"SideExtra");
  earthLevel=Control.EvalVar<double>(keyName+"EarthLevel");
  earthDome=Control.EvalVar<double>(keyName+"EarthDome");
  earthDepth=Control.EvalVar<double>(keyName+"EarthDepth");
  earthRadius=Control.EvalVar<double>(keyName+"EarthRadius");



  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  innerMat=ModelSupport::EvalMat<int>(Control,keyName+"InnerMat");
  earthMat=ModelSupport::EvalMat<int>(Control,keyName+"EarthMat");
 
  return;
}


void
ShieldRoom::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("ShieldRoom","createSurfaces");

  // Do outer surfaces (vacuum ports)
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length/2.0),Y);

  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*depth,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*height,Z);

  ModelSupport::buildPlane(SMap,buildIndex+13,Origin-X*(sideThick+width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,Origin+X*(sideThick+width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+15,Origin-Z*(depth+floorThick),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*(height+roofThick),Z);

  
  ModelSupport::buildPlane(SMap,buildIndex+23,
			   Origin-X*(sideThick+sideExtra+width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+24,
			   Origin+X*(sideThick+sideExtra+width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+25,
			   Origin-Z*(earthDepth+depth+floorThick),Z);
  
  // earth level
  ModelSupport::buildPlane
      (SMap,buildIndex+26,Origin+Z*(earthLevel+height),Z);
  const Geometry::Vec3D domeOrg=
    Origin+Z*(height+roofThick+earthDome-earthRadius);

  ELog::EM<<"Dome ORg ="<<domeOrg<<" "<<earthDome<<ELog::endDiag;
  ModelSupport::buildCylinder(SMap,buildIndex+27,domeOrg,Y,earthRadius);

  return;
}

void
ShieldRoom::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("ShieldRoom","createObjects");

  std::string Out;
  // Inner void
  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 3 -4 5 -6 ");
  makeCell("Void",System,cellIndex++,voidMat,0.0,Out);

  // Concrete walls
  Out=ModelSupport::getComposite
    (SMap,buildIndex," 1 -2 13 -14 15 -16 (-3:4:-5:6) ");
  makeCell("InnerWall",System,cellIndex++,innerMat,0.0,Out);

  // SubGround
  Out=ModelSupport::getComposite
    (SMap,buildIndex," 1 -2 23 -24 25 -15 ");
  makeCell("SubGround",System,cellIndex++,earthMat,0.0,Out);

  // MidGround
  Out=ModelSupport::getComposite
    (SMap,buildIndex," 1 -2 23 -24 15 -26 (-13:14:16) ");
  makeCell("MidGround",System,cellIndex++,earthMat,0.0,Out);

  // TopGround
  Out=ModelSupport::getComposite
    (SMap,buildIndex," 1 -2 -27 26");
  makeCell("TopGround",System,cellIndex++,earthMat,0.0,Out);

  Out=ModelSupport::getSetComposite
    (SMap,buildIndex,"1 -2 23 -24 25 (-26 : -27)");
  addOuterSurf(Out);

  return;
}

void
ShieldRoom::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane.
    Port position are used for first two link points
    Note that 0/1 are the flange surfaces
  */
{
  ELog::RegMethod RegA("ShieldRoom","createLinks");

  // port centre  
  
  return;
}

void
ShieldRoom::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("ShieldRoom","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  createSurfaces();    
  createObjects(System);
  
  createLinks();
  insertObjects(System);   

  return;
}
  
}  // NAMESPACE xraySystem
