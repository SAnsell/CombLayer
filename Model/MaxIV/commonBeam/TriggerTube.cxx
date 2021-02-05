/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/TriggerTube.cxx
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

#include "TriggerTube.h"

namespace xraySystem
{

TriggerTube::TriggerTube(const std::string& Key) :
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


TriggerTube::~TriggerTube() 
  /*!
    Destructor
  */
{}

void
TriggerTube::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase for variables
  */
{
  ELog::RegMethod RegA("TriggerTube","populate");

  FixedRotate::populate(Control);

  radius=Control.EvalVar<double>(keyName+"Radius");
  xRadius=Control.EvalVar<double>(keyName+"XRadius");  // beam axis
  yRadius=Control.EvalVar<double>(keyName+"YRadius");  // beam axis
  wallThick=Control.EvalVar<double>(keyName+"WallThick");

  height=Control.EvalVar<double>(keyName+"Height");
  depth=Control.EvalVar<double>(keyName+"Depth");

  frontLength=Control.EvalVar<double>(keyName+"FrontLength");
  backLength=Control.EvalVar<double>(keyName+"BackLength");

  flangeXRadius=Control.EvalVar<double>(keyName+"FlangeXRadius");
  flangeYRadius=Control.EvalVar<double>(keyName+"FlangeYRadius");
  flangeZRadius=Control.EvalVar<double>(keyName+"FlangeZRadius");

  flangeXLength=Control.EvalVar<double>(keyName+"FlangeXLength");
  flangeYLength=Control.EvalVar<double>(keyName+"FlangeYLength");
  flangeZLength=Control.EvalVar<double>(keyName+"FlangeZLength");

  sideZOffset=Control.EvalVar<double>(keyName+"SideZOffset");
  sideLength=Control.EvalVar<double>(keyName+"SideLength");
  plateThick=Control.EvalVar<double>(keyName+"PlateThick");

  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  plateMat=ModelSupport::EvalMat<int>(Control,keyName+"PlateMat");

  return;
}


void
TriggerTube::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("TriggerTube","createSurfaces");

  if (!isActive("front"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*frontLength,Y);
      ExternalCut::setCutSurf("front",SMap.realSurf(buildIndex+1));
    }
  if (!isActive("back"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*backLength,Y);
      ExternalCut::setCutSurf("back",-SMap.realSurf(buildIndex+2));
    }

  // vertical/horizontal dividers
  ModelSupport::buildPlane(SMap,buildIndex+100,Origin,X);
  ModelSupport::buildPlane(SMap,buildIndex+200,Origin,Y);
  ModelSupport::buildPlane(SMap,buildIndex+300,Origin,Z);

  // front/back pipe and thickness
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,yRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,yRadius+wallThick);

  ModelSupport::buildPlane
    (SMap,buildIndex+101,Origin-Y*(frontLength-flangeYLength),Y);
  ModelSupport::buildCylinder(SMap,buildIndex+107,Origin,Y,flangeYRadius);

  ModelSupport::buildPlane
    (SMap,buildIndex+202,Origin+Y*(backLength-flangeYLength),Y);
  ModelSupport::buildCylinder(SMap,buildIndex+207,Origin,Y,flangeYRadius);


  // Main (VERTICAL) tube
  ModelSupport::buildPlane(SMap,buildIndex+405,Origin-Z*depth,Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+415,Origin-Z*(depth-flangeZLength),Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+425,Origin-Z*(depth+plateThick),Z);

  ModelSupport::buildPlane(SMap,buildIndex+406,Origin+Z*height,Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+416,Origin+Z*(height-flangeZLength),Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+426,Origin+Z*(height+plateThick),Z);
  
  ModelSupport::buildCylinder(SMap,buildIndex+407,Origin,Z,radius);
  ModelSupport::buildCylinder(SMap,buildIndex+417,Origin,Z,radius+wallThick);
  ModelSupport::buildCylinder(SMap,buildIndex+427,Origin,Z,flangeZRadius);

  // Side Tubes 2 -ve X  :: 1 +ve X
  

  ModelSupport::buildCylinder(SMap,buildIndex+507,Origin,X,xRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+517,Origin,X,xRadius+wallThick);
  ModelSupport::buildCylinder(SMap,buildIndex+527,Origin,X,flangeXRadius);

  ModelSupport::buildPlane(SMap,buildIndex+503,Origin-X*sideLength,X);
  ModelSupport::buildPlane(SMap,buildIndex+504,Origin+X*sideLength,X);

  ModelSupport::buildPlane
    (SMap,buildIndex+513,Origin-X*(sideLength-flangeXLength),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+514,Origin+X*(sideLength-flangeXLength),X);

  ModelSupport::buildPlane
    (SMap,buildIndex+523,Origin-X*(sideLength+plateThick),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+524,Origin+X*(sideLength+plateThick),X);
  
  Geometry::Vec3D sideOrg(Origin+Z*sideZOffset);
  ModelSupport::buildCylinder(SMap,buildIndex+607,sideOrg,X,xRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+617,sideOrg,X,xRadius+wallThick);
  ModelSupport::buildCylinder(SMap,buildIndex+627,sideOrg,X,flangeXRadius);

  
  return;
}

void
TriggerTube::createObjects(Simulation& System)
  /*!
    Builds all the objects
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("TriggerTube","createObjects");

  HeadRule HR;
  
  const HeadRule frontHR=getRule("front");
  const HeadRule backHR=getRule("back");

  // inner void
  HR=ModelSupport::getHeadRule(SMap,buildIndex," -7 ");
  makeCell("Void",System,cellIndex++,voidMat,0.0,HR*frontHR*backHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 7 -17 407 ");
  makeCell("MainTube",System,cellIndex++,wallMat,0.0,HR*frontHR*backHR);
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-101 17 -107 ");
  makeCell("FlangeA",System,cellIndex++,wallMat,0.0,HR*frontHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 202 17 -107 ");
  makeCell("FlangeB",System,cellIndex++,wallMat,0.0,HR*backHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-407 405 -406 7");
  makeCell("MainVoid",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"(607:100) 507 -417 407 405 -406 17 ");
  makeCell("MainWall",System,cellIndex++,wallMat,0.0,HR);

  // BASE part
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex," 417 -427 405 -415 ");
  makeCell("LowFlange",System,cellIndex++,wallMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," -427 425 -405");
  makeCell("LowPlate",System,cellIndex++,plateMat,0.0,HR);

  // TOP part
  HR=ModelSupport::getHeadRule(SMap,buildIndex," 417 -427 -406 416 ");
  makeCell("TopFlange",System,cellIndex++,wallMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," -427 -426 406");
  makeCell("TopPlate",System,cellIndex++,plateMat,0.0,HR);

  // vert void
  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"(617:100) 517 -427 415 -416 17 417  ");
  if (flangeZRadius-Geometry::zeroTol > frontLength-flangeYLength)
    {
      if (flangeZRadius-Geometry::zeroTol > backLength-flangeYLength)
	HR*=ModelSupport::getHeadRule(SMap,buildIndex," ((101 -202):107) ");
      else
	HR*=ModelSupport::getHeadRule(SMap,buildIndex," (101:107) ");
    }
  else if (flangeZRadius-Geometry::zeroTol > backLength-flangeYLength)
    {
      HR*=ModelSupport::getHeadRule(SMap,buildIndex," (-202:107) ");
    }

  makeCell("VertOuter",System,cellIndex++,0,0.0,HR);
  
  // front void
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-200 17 427 -107 101");
  makeCell("FrontOuter",System,cellIndex++,0,0.0,HR);

  // back void
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"200 17 427 -207 -202 ");
  makeCell("BackOuter",System,cellIndex++,0,0.0,HR);

  // Side objects
  // left side
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-100 407 -507 503 ");
  makeCell("LeftVoid",System,cellIndex++,0,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-100 417 -517 507 503 ");
  makeCell("LeftWall",System,cellIndex++,wallMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"517 -527 503 -513");
  makeCell("LeftFlange",System,cellIndex++,wallMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-527 523 -503");
  makeCell("LeftPlate",System,cellIndex++,plateMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-100 427 517 -527 513 ");
  makeCell("LeftOuter",System,cellIndex++,0,0.0,HR);
  
  // left top
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-100 407 -607 503 ");
  makeCell("LTopVoid",System,cellIndex++,0,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-100 417 -617 607 503 ");
  makeCell("LTopWall",System,cellIndex++,wallMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"617 -627 503 -513");
  makeCell("LTopFlange",System,cellIndex++,wallMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-627 523 -503");
  makeCell("LTopPlate",System,cellIndex++,plateMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-100 427 617 -627 513 ");
  makeCell("LTopOuter",System,cellIndex++,0,0.0,HR);
  
  // Right side
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"100 407 -507 -504 ");
  makeCell("RightVoid",System,cellIndex++,0,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"100 417 -517 507 -504 ");
  makeCell("RightWall",System,cellIndex++,wallMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"517 -527 -504 514");
  makeCell("RightFlange",System,cellIndex++,wallMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-527 -524 504");
  makeCell("RightPlate",System,cellIndex++,plateMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"100 427 517 -527 -514 ");
  makeCell("RightOuter",System,cellIndex++,0,0.0,HR);


  // Outer boxes:

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"523 -524 425 -426 427 527 107 (100:627)");
  makeCell("MainOuter",System,cellIndex++,0,0.0,HR*frontHR*backHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"523 -524 425 -426");
  addOuterSurf(HR*frontHR*backHR);

  return;
}

void 
TriggerTube::createLinks()
  /*!
    Create the linked units
   */
{
  ELog::RegMethod RegA("TriggerTube","createLinks");

  ExternalCut::createLink("front",*this,0,Origin,Y);  //front and back
  ExternalCut::createLink("back",*this,1,Origin,Y);  //front and back

  FixedComp::setConnect(2,Origin-X*(radius+wallThick),-X);
  FixedComp::setLinkSurf(2,SMap.realSurf(buildIndex+417));

  FixedComp::setConnect(3,Origin+X*(radius+wallThick),X);
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+417));

  FixedComp::setConnect(4,Origin-Z*(depth+plateThick),-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+425));

  FixedComp::setConnect(5,Origin+Z*(height+plateThick),Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+426));
  
  return;
}

void
TriggerTube::createAll(Simulation& System,
	       const attachSystem::FixedComp& FC,
	       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed point track 
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("TriggerTube","createAll");
  
  populate(System.getDataBase());
  createCentredUnitVector(FC,sideIndex,frontLength);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE tdcSystem
