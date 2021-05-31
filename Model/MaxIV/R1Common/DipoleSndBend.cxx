/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   R1Common/DipoleSndBend.cxx
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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "surfRegister.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"  
#include "FixedComp.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "ExternalCut.h" 
#include "BaseMap.h"
#include "SurfMap.h"
#include "CellMap.h"

#include "DipoleSndBend.h"

namespace xraySystem
{

DipoleSndBend::DipoleSndBend(const std::string& Key) : 
  attachSystem::FixedRotate(Key,8),
  attachSystem::ContainedGroup("Beam","Extra"),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  attachSystem::SurfMap()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

DipoleSndBend::~DipoleSndBend() 
  /*!
    Destructor
  */
{}

void
DipoleSndBend::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase for variables
  */
{
  ELog::RegMethod RegA("DipoleSndBend","populate");

  FixedRotate::populate(Control);

  arcAngle=Control.EvalVar<double>(keyName+"ArcAngle");

  curveRadius=Control.EvalVar<double>(keyName+"CurveRadius");  
  flatWidth=Control.EvalVar<double>(keyName+"FlatWidth");
  outerFlat=Control.EvalVar<double>(keyName+"OuterFlat");
  tipHeight=Control.EvalVar<double>(keyName+"TipHeight");
  height=Control.EvalVar<double>(keyName+"Height");
  outerHeight=Control.EvalVar<double>(keyName+"OuterHeight");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  
  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  outerMat=ModelSupport::EvalMat<int>(Control,keyName+"OuterMat");

  return;
}

void
DipoleSndBend::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("DipoleSndBend","createSurface");

  const double coneAngle(M_PI/4.0);
  
  if (!ExternalCut::isActive("front"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
      setCutSurf("front",SMap.realSurf(buildIndex+1));
    }

  const Geometry::Vec3D CylOrg(Origin+X*curveRadius);
  const Geometry::Quaternion QYA
    (Geometry::Quaternion::calcQRotDeg(arcAngle,-Z));
  const Geometry::Vec3D PY=QYA.makeRotate(Y);
  const Geometry::Vec3D endPoint
    (CylOrg
     -X*(curveRadius*cos(M_PI*arcAngle/180.0))
     +Y*(curveRadius*sin(M_PI*arcAngle/180.0)));

  
  ModelSupport::buildPlane(SMap,buildIndex+2,endPoint,PY);  
	       
  // tip
  ModelSupport::buildCylinder(SMap,buildIndex+7,CylOrg,Z,curveRadius);
  // second cone start
  ModelSupport::buildCylinder
    (SMap,buildIndex+27,CylOrg,Z,curveRadius+flatWidth);

  // First cut:
  const Geometry::Vec3D COrgTop =CylOrg
    +Z*(tipHeight/2.0-curveRadius/tan(coneAngle));

  const Geometry::Vec3D COrgBase =CylOrg
    -Z*(tipHeight/2.0-curveRadius/tan(coneAngle));


  ModelSupport::buildCone(SMap,buildIndex+85,COrgBase,Z,45.0);
  ModelSupport::buildCone(SMap,buildIndex+86,COrgTop,Z,45.0);

  // Second cut:
  const double SR(flatWidth+curveRadius);

  const Geometry::Vec3D SOrgTop = CylOrg
    +Z*(height/2.0-SR*tan(coneAngle));

  const Geometry::Vec3D SOrgBase = CylOrg
    -Z*(height/2.0-SR/tan(coneAngle));
  
  ModelSupport::buildCone(SMap,buildIndex+815,SOrgBase,Z,45.0);
  ModelSupport::buildCone(SMap,buildIndex+816,SOrgTop,Z,45.0);

  
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin-X*outerFlat,X);
    
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(height/2.0),Z);

  ModelSupport::buildPlane(SMap,buildIndex+15,Origin-Z*(outerHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*(outerHeight/2.0),Z);


  // WALLS
  ModelSupport::buildPlane
    (SMap,buildIndex+104,Origin-X*(outerFlat+wallThick),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+105,Origin-Z*(wallThick+height/2.0),Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+106,Origin+Z*(wallThick+height/2.0),Z);

  ModelSupport::buildPlane
    (SMap,buildIndex+115,Origin-Z*(wallThick+outerHeight/2.0),Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+116,Origin+Z*(wallThick+outerHeight/2.0),Z);

  // tip
  ModelSupport::buildCylinder
    (SMap,buildIndex+107,CylOrg,Z,curveRadius-wallThick);

 
  // First cut:
  const double WRadius=curveRadius-wallThick;
  const double SRadius(flatWidth+curveRadius-wallThick);
  
  const Geometry::Vec3D CWallTop =CylOrg
    +Z*(tipHeight/2.0-WRadius/tan(coneAngle));

  const Geometry::Vec3D CWallBase = CylOrg
    -Z*(tipHeight/2.0-WRadius/tan(coneAngle));

  const Geometry::Vec3D SWallTop =CylOrg
    +Z*(height/2.0-SRadius/tan(coneAngle));

  const Geometry::Vec3D SWallBase = CylOrg
    -Z*(height/2.0-SRadius/tan(coneAngle));

  ModelSupport::buildCone(SMap,buildIndex+185,CWallBase,Z,45.0);
  ModelSupport::buildCone(SMap,buildIndex+186,CWallTop,Z,45.0);

  ModelSupport::buildCone(SMap,buildIndex+1815,SWallBase+X*wallThick,Z,45.0);
  ModelSupport::buildCone(SMap,buildIndex+1816,SWallTop+X*wallThick,Z,45.0);

  return;
}

void
DipoleSndBend::createObjects(Simulation& System)
  /*!
    Builds all the objects
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("DipoleSndBend","createObjects");

  std::string Out;
  const std::string fStr=getRuleStr("front");
  const std::string side=getRuleStr("side");

  Out=ModelSupport::getComposite(SMap,buildIndex,"85 86  -27 7  5 -6 -2 ");
  makeCell("Void",System,cellIndex++,voidMat,0.0,Out+fStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," 815 816  27 4 15 -16 -2 ");
  makeCell("OuterVoid",System,cellIndex++,wallMat,0.0,Out+fStr+side);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," 105 -106 186 185 -27 107 (-85:-86:-7:-5:6) -2");
  makeCell("Wall",System,cellIndex++,wallMat,0.0,Out+fStr);


  Out=ModelSupport::getComposite
    (SMap,buildIndex," 1815 1816 115 -116 (-815 : -816 :-15 :16) 4 27 -2 ");
  makeCell("Wall",System,cellIndex++,wallMat,0.0,Out+fStr+side);


  // Outer Volumes
  // note extra divider that is need for cone
  Out=ModelSupport::getComposite
    (SMap,buildIndex," 107 -185 105 -106 -2");
  makeCell("Outer",System,cellIndex++,outerMat,0.0,Out+fStr);

  // Outer Volumes
  Out=ModelSupport::getComposite
    (SMap,buildIndex," 107 -186 105 -106 -2");
  makeCell("Outer",System,cellIndex++,outerMat,0.0,Out+fStr);


  Out=ModelSupport::getComposite
    (SMap,buildIndex," 27 115 -116 -1816  -2");
  makeCell("Outer",System,cellIndex++,outerMat,0.0,Out+fStr);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," 27 115 -116 -1815  -2");
  makeCell("Outer",System,cellIndex++,outerMat,0.0,Out+fStr);


  Out=ModelSupport::getComposite
    (SMap,buildIndex," -27 107 105 -106 -2");
  addOuterSurf("Beam",Out+fStr);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," 115 -116 4 27 -2 ");
  addOuterSurf("Extra",Out+fStr);

  return;
}

void 
DipoleSndBend::createLinks()
  /*!
    Create the linked units
   */
{
  ELog::RegMethod RegA("DipoleSndBend","createLinks");
  ExternalCut::createLink("front",*this,0,Origin,Y);

  const Geometry::Quaternion QYA
    (Geometry::Quaternion::calcQRotDeg(arcAngle,-Z));
  const Geometry::Vec3D PY=QYA.makeRotate(Y);
  // Beam is a set distance from Origin (xstep)
  //  xStep+
  const Geometry::Vec3D endPoint
    (Origin
     +X*(-xStep+curveRadius*(1.0-cos(M_PI*arcAngle/180.0)))
     +Y*(curveRadius*sin(M_PI*arcAngle/180.0)));

  FixedComp::setConnect(1,endPoint,PY);
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));

  // mid point
  const Geometry::Quaternion QMA
    (Geometry::Quaternion::calcQRotDeg(arcAngle/2.0,-Z));
  const Geometry::Vec3D MY=QMA.makeRotate(Y);
  
  const Geometry::Vec3D midPoint
    (Origin
     +X*(-xStep+curveRadius*(1.0-cos(M_PI*arcAngle/360.0)))
     +Y*(curveRadius*sin(M_PI*arcAngle/360.0)));

  FixedComp::setConnect(6,midPoint,MY);
  FixedComp::nameSideIndex(6,"dipoleCentre");
  return;
}


void
DipoleSndBend::createAll(Simulation& System,
			 const attachSystem::FixedComp& FC,
			 const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed point track 
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("DipoleSndBend","createAll");
  
  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE epbSystem
