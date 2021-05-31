/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/GaugeTube.cxx
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#include "ExternalCut.h"
#include "FrontBackCut.h" 
#include "BaseMap.h"
#include "SurfMap.h"
#include "CellMap.h" 

#include "GaugeTube.h"

namespace xraySystem
{

GaugeTube::GaugeTube(const std::string& Key) :
  attachSystem::FixedRotate(Key,6),
  attachSystem::ContainedComp(),
  attachSystem::FrontBackCut(),
  attachSystem::CellMap(),
  attachSystem::SurfMap()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}


GaugeTube::~GaugeTube() 
  /*!
    Destructor
  */
{}

void
GaugeTube::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase for variables
  */
{
  ELog::RegMethod RegA("GaugeTube","populate");

  FixedRotate::populate(Control);

  xRadius=Control.EvalVar<double>(keyName+"XRadius");
  yRadius=Control.EvalVar<double>(keyName+"YRadius");  // beam axis
  wallThick=Control.EvalVar<double>(keyName+"WallThick");

  length=Control.EvalVar<double>(keyName+"Length");
  
  sideLength=Control.EvalVar<double>(keyName+"SideLength");
  sideYStep=Control.EvalVar<double>(keyName+"SideYStep");
  sideAngle=Control.EvalVar<double>(keyName+"SideAngle");

  flangeXRadius=Control.EvalVar<double>(keyName+"FlangeXRadius");
  flangeYRadius=Control.EvalVar<double>(keyName+"FlangeYRadius");
  flangeXLength=Control.EvalVar<double>(keyName+"FlangeXLength");
  flangeYLength=Control.EvalVar<double>(keyName+"FlangeYLength");

  plateThick=Control.EvalVar<double>(keyName+"PlateThick");

  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  plateMat=ModelSupport::EvalMat<int>(Control,keyName+"PlateMat");

  return;
}


void
GaugeTube::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("GaugeTube","createSurfaces");

  if (!isActive("front"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(length/2.0),Y);
      ExternalCut::setCutSurf("front",SMap.realSurf(buildIndex+1));
    }
  if (!isActive("back"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length/2.0),Y);
      ExternalCut::setCutSurf("back",-SMap.realSurf(buildIndex+2));
    }

  // divider
  ModelSupport::buildPlane(SMap,buildIndex+200,Origin,Y);
  
  // front/back pipe and thickness
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,yRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,yRadius+wallThick);

  ModelSupport::buildPlane
    (SMap,buildIndex+101,Origin-Y*(length/2.0-flangeYLength),Y);
  ModelSupport::buildCylinder(SMap,buildIndex+107,Origin,Y,flangeYRadius);

  ModelSupport::buildPlane
    (SMap,buildIndex+202,Origin+Y*(length/2.0-flangeYLength),Y);
  ModelSupport::buildCylinder(SMap,buildIndex+207,Origin,Y,flangeYRadius);

  // Cross tube angle [-X ==> 0 degrees]
  Geometry::Vec3D sideX(-X);
  if (std::abs(sideAngle)<Geometry::zeroTol)
    sideX= -X;
  else if (std::abs(sideAngle-180.0)<Geometry::zeroTol)
    sideX=X;
  else if (std::abs(sideAngle-270.0)<Geometry::zeroTol)
    sideX=Z;
  else
    {
      const Geometry::Quaternion QV =
	Geometry::Quaternion::calcQRotDeg(-sideAngle,Y);
      sideX=QV.makeRotate(X);
    }

  const Geometry::Vec3D sideOrg(Origin+Y*sideYStep);
  ModelSupport::buildPlane(SMap,buildIndex+100,sideOrg,sideX);
  
  ModelSupport::buildPlane(SMap,buildIndex+303,sideOrg+sideX*sideLength,sideX);
  ModelSupport::buildPlane
    (SMap,buildIndex+313,sideOrg+sideX*(sideLength-flangeXLength),sideX);
  ModelSupport::buildPlane
    (SMap,buildIndex+323,sideOrg+sideX*(sideLength+plateThick),sideX);
  
  ModelSupport::buildCylinder(SMap,buildIndex+307,sideOrg,sideX,xRadius);
  ModelSupport::buildCylinder
    (SMap,buildIndex+317,sideOrg,sideX,xRadius+wallThick);
  ModelSupport::buildCylinder
    (SMap,buildIndex+327,sideOrg,sideX,flangeXRadius);
  
  // link point
  FixedComp::setConnect(2,Origin+sideX*(sideLength+plateThick),sideX);
  FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+323));
  
  return;
}

void
GaugeTube::createObjects(Simulation& System)
  /*!
    Builds all the objects
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("GaugeTube","createObjects");

  std::string Out;
  
  const std::string frontStr=getRuleStr("front");
  const std::string backStr=getRuleStr("back");

  // inner void
  Out=ModelSupport::getComposite(SMap,buildIndex," -7 ");
  makeCell("Void",System,cellIndex++,voidMat,0.0,Out+frontStr+backStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," 7 -17 (307:-100)");
  makeCell("MainTube",System,cellIndex++,wallMat,0.0,Out+frontStr+backStr);
  
  Out=ModelSupport::getComposite(SMap,buildIndex,"-101 17 -107 ");
  makeCell("FlangeA",System,cellIndex++,wallMat,0.0,Out+frontStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," 202 17 -107 ");
  makeCell("FlangeB",System,cellIndex++,wallMat,0.0,Out+backStr);


  Out=ModelSupport::getComposite(SMap,buildIndex," 100 -307 -303 7  ");
  makeCell("SideVoid",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 100 -317 307 -303 17  ");
  makeCell("SideWall",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 317 -327 -303 313 ");
  makeCell("LeftFlange",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -327 303 -323 ");
  makeCell("LeftPlate",System,cellIndex++,plateMat,0.0,Out);

  // cross void
  Out=ModelSupport::getComposite(SMap,buildIndex," 100 -327 -313 17  317 ");
  makeCell("HorOuter",System,cellIndex++,0,0.0,Out);
  
  // front void
  Out=ModelSupport::getComposite(SMap,buildIndex,"  17 327 -107 101 -202 100");
  makeCell("SideOuter",System,cellIndex++,0,0.0,Out);

  // back void
  Out=ModelSupport::getComposite(SMap,buildIndex," -100 17 101 -202 -107");
  makeCell("FarOuter",System,cellIndex++,0,0.0,Out);

  // outer void box:
  Out=ModelSupport::getComposite(SMap,buildIndex,"-107 ");
  addOuterSurf(Out+frontStr+backStr);

  Out=ModelSupport::getComposite
    (SMap,buildIndex,"(100 -323 -327 ) :");
  addOuterUnionSurf(Out);

  return;
}

void 
GaugeTube::createLinks()
  /*!
    Create the linked units
   */
{
  ELog::RegMethod RegA("GaugeTube","createLinks");

  ExternalCut::createLink("front",*this,0,Origin,Y);  //front and back
  ExternalCut::createLink("back",*this,1,Origin,Y);  //front and back


  FixedComp::setConnect(3,Origin+X*(yRadius+wallThick),X);
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+17));

  FixedComp::setConnect(4,Origin-Z*(yRadius+wallThick),-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+17));

  FixedComp::setConnect(5,Origin+Z*(yRadius+wallThick),Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+17));
  
  return;
}

void
GaugeTube::createAll(Simulation& System,
	       const attachSystem::FixedComp& FC,
	       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed point track 
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("GaugeTube","createAll");
  
  populate(System.getDataBase());
  createCentredUnitVector(FC,sideIndex,length/2.0);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE xraySystem
