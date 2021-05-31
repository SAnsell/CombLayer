/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/CrossWayTube.cxx
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

#include "CrossWayTube.h"

namespace xraySystem
{

CrossWayTube::CrossWayTube(const std::string& Key) :
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


CrossWayTube::~CrossWayTube() 
  /*!
    Destructor
  */
{}

void
CrossWayTube::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase for variables
  */
{
  ELog::RegMethod RegA("CrossWayTube","populate");

  FixedRotate::populate(Control);

  radius=Control.EvalVar<double>(keyName+"Radius");
  xRadius=Control.EvalVar<double>(keyName+"XRadius");
  yRadius=Control.EvalVar<double>(keyName+"YRadius");  // beam axis
  wallThick=Control.EvalVar<double>(keyName+"WallThick");

  height=Control.EvalVar<double>(keyName+"Height");
  depth=Control.EvalVar<double>(keyName+"Depth");
  frontLength=Control.EvalVar<double>(keyName+"FrontLength");
  backLength=Control.EvalVar<double>(keyName+"BackLength");
  sideALength=Control.EvalVar<double>(keyName+"SideALength");
  sideBLength=Control.EvalVar<double>(keyName+"SideBLength");

  flangeXRadius=Control.EvalVar<double>(keyName+"FlangeXRadius");
  flangeYRadius=Control.EvalVar<double>(keyName+"FlangeYRadius");
  flangeZRadius=Control.EvalVar<double>(keyName+"FlangeZRadius");
  flangeXLength=Control.EvalVar<double>(keyName+"FlangeXLength");
  flangeYLength=Control.EvalVar<double>(keyName+"FlangeYLength");
  flangeZLength=Control.EvalVar<double>(keyName+"FlangeZLength");

  plateThick=Control.EvalVar<double>(keyName+"PlateThick");

  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  plateMat=ModelSupport::EvalMat<int>(Control,keyName+"PlateMat");

  return;
}


void
CrossWayTube::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("CrossWayTube","createSurfaces");

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


  // Cross tubes

  ModelSupport::buildPlane(SMap,buildIndex+303,Origin-X*sideALength,X);
  ModelSupport::buildPlane
    (SMap,buildIndex+313,Origin-X*(sideALength-flangeXLength),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+323,Origin-X*(sideALength+plateThick),X);

  ModelSupport::buildPlane(SMap,buildIndex+304,Origin+X*sideBLength,X);
  ModelSupport::buildPlane
    (SMap,buildIndex+314,Origin+X*(sideBLength-flangeXLength),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+324,Origin+X*(sideBLength+plateThick),X);

  ModelSupport::buildCylinder(SMap,buildIndex+307,Origin,X,xRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+317,Origin,X,xRadius+wallThick);
  ModelSupport::buildCylinder(SMap,buildIndex+327,Origin,X,flangeXRadius);

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

   return;
}

void
CrossWayTube::createObjects(Simulation& System)
  /*!
    Builds all the objects
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("CrossWayTube","createObjects");

  std::string Out;
  
  const std::string frontStr=getRuleStr("front");
  const std::string backStr=getRuleStr("back");

  // inner void
  Out=ModelSupport::getComposite(SMap,buildIndex," -7 ");
  makeCell("Void",System,cellIndex++,voidMat,0.0,Out+frontStr+backStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," 7 -17 407 307");
  makeCell("MainTube",System,cellIndex++,wallMat,0.0,Out+frontStr+backStr);
  
  Out=ModelSupport::getComposite(SMap,buildIndex,"-101 17 -107 ");
  makeCell("FlangeA",System,cellIndex++,wallMat,0.0,Out+frontStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," 202 17 -107 ");
  makeCell("FlangeB",System,cellIndex++,wallMat,0.0,Out+backStr);


  Out=ModelSupport::getComposite(SMap,buildIndex," -100 -307 303 7  ");
  makeCell("LeftVoid",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -100 -317 307 303 17 407 ");
  makeCell("LeftWall",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 317 -327 303 -313 ");
  makeCell("LeftFlange",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -327 -303 323 ");
  makeCell("LeftPlate",System,cellIndex++,plateMat,0.0,Out);

  // Right
  
  Out=ModelSupport::getComposite(SMap,buildIndex," 100 -307 -304 7  ");
  makeCell("RightVoid",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 100 -317 307 -304 17 407 ");
  makeCell("RightWall",System,cellIndex++,wallMat,0.0,Out);
  

  Out=ModelSupport::getComposite(SMap,buildIndex," 317 -327 -304 314 ");
  makeCell("RightFlange",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -327 304 -324");
  makeCell("RightPlate",System,cellIndex++,plateMat,0.0,Out);


  Out=ModelSupport::getComposite(SMap,buildIndex," -300 -407 405 7 307 ");
  makeCell("LowVoid",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -300 -417 407 405 17 317");
  makeCell("LowWall",System,cellIndex++,wallMat,0.0,Out);
  
  Out=ModelSupport::getComposite(SMap,buildIndex," 417 -427 405 -415 ");
  makeCell("LowFlange",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -427 425 -405");
  makeCell("LowPlate",System,cellIndex++,plateMat,0.0,Out);

  // TOP
  Out=ModelSupport::getComposite(SMap,buildIndex," 300 -407 -406 7 307 ");
  makeCell("TopVoid",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 300 -417 407 -406 17 317");
  makeCell("TopWall",System,cellIndex++,wallMat,0.0,Out);  

  Out=ModelSupport::getComposite(SMap,buildIndex," 417 -427 -406 416 ");
  makeCell("TopFlange",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -427 -426 406");
  makeCell("TopPlate",System,cellIndex++,plateMat,0.0,Out);

  // cross void
  Out=ModelSupport::getComposite(SMap,buildIndex," -327 313 -314 17 417 317 ");
  makeCell("HorOuter",System,cellIndex++,0,0.0,Out);
  // vert void
  Out=ModelSupport::getComposite(SMap,buildIndex," -427 415 -416 17 417 327 ");
  if (flangeZRadius-Geometry::zeroTol > frontLength-flangeYLength)
    {
      if (flangeZRadius-Geometry::zeroTol > backLength-flangeYLength)
	Out+=ModelSupport::getComposite(SMap,buildIndex," ((101 -202):107) ");
      else
	Out+=ModelSupport::getComposite(SMap,buildIndex," (101:107) ");
    }
  else if (flangeZRadius-Geometry::zeroTol > backLength-flangeYLength)
    Out+=ModelSupport::getComposite(SMap,buildIndex," (-202:107) ");

  makeCell("VertOuter",System,cellIndex++,0,0.0,Out);
  
  // front void
  Out=ModelSupport::getComposite(SMap,buildIndex," -200 17 327 427 -107 101");
  makeCell("FrontOuter",System,cellIndex++,0,0.0,Out);

  // back void
  Out=ModelSupport::getComposite(SMap,buildIndex," 200 17 327 427 -207 -202 ");
  makeCell("BackOuter",System,cellIndex++,0,0.0,Out);

  // outer void box:
  Out=ModelSupport::getComposite(SMap,buildIndex,"-107 ");
  addOuterSurf(Out+frontStr+backStr);

  Out=ModelSupport::getComposite
    (SMap,buildIndex,"(323 -327 -324) : (425 -426 -427)");
  addOuterUnionSurf(Out+frontStr+backStr);

  return;
}

void 
CrossWayTube::createLinks()
  /*!
    Create the linked units
   */
{
  ELog::RegMethod RegA("CrossWayTube","createLinks");

  ExternalCut::createLink("front",*this,0,Origin,Y);  //front and back
  ExternalCut::createLink("back",*this,1,Origin,Y);  //front and back

  FixedComp::setConnect(2,Origin-X*(sideALength+plateThick),-X);
  FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+323));

  FixedComp::setConnect(3,Origin+X*(sideBLength+plateThick),X);
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+343));

  FixedComp::setConnect(4,Origin-Z*(depth+plateThick),-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+425));

  FixedComp::setConnect(5,Origin+Z*(height+plateThick),Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+426));
  
  return;
}

void
CrossWayTube::createAll(Simulation& System,
	       const attachSystem::FixedComp& FC,
	       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed point track 
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("CrossWayTube","createAll");
  
  populate(System.getDataBase());
  createCentredUnitVector(FC,sideIndex,frontLength);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE xraySystem
