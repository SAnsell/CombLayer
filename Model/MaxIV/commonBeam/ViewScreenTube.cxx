/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   Linac/ViewScreenTube.cxx
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

#include "ViewScreenTube.h"

namespace xraySystem
{

ViewScreenTube::ViewScreenTube(const std::string& Key) :
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


ViewScreenTube::~ViewScreenTube() 
  /*!
    Destructor
  */
{}

void
ViewScreenTube::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase for variables
  */
{
  ELog::RegMethod RegA("ViewScreenTube","populate");

  FixedRotate::populate(Control);

  radius=Control.EvalVar<double>(keyName+"Radius");
  height=Control.EvalVar<double>(keyName+"Height");
  depth=Control.EvalVar<double>(keyName+"Depth");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  plateThick=Control.EvalVar<double>(keyName+"PlateThick");

  flangeRadius=Control.EvalVar<double>(keyName+"FlangeRadius");
  flangeLength=Control.EvalVar<double>(keyName+"FlangeLength");

  portARadius=Control.EvalVar<double>(keyName+"PortARadius");
  portBRadius=Control.EvalVar<double>(keyName+"PortBRadius");
  portALength=Control.EvalVar<double>(keyName+"PortALength");
  portBLength=Control.EvalVar<double>(keyName+"PortBLength");

  flangeARadius=Control.EvalVar<double>(keyName+"FlangeARadius");
  flangeALength=Control.EvalVar<double>(keyName+"FlangeALength");
  flangeBRadius=Control.EvalVar<double>(keyName+"FlangeBRadius");
  flangeBLength=Control.EvalVar<double>(keyName+"FlangeBLength");

  viewRadius=Control.EvalVar<double>(keyName+"ViewRadius");
  viewLength=Control.EvalVar<double>(keyName+"ViewLength");
  viewFlangeRadius=Control.EvalVar<double>(keyName+"ViewFlangeRadius");
  viewFlangeLength=Control.EvalVar<double>(keyName+"ViewFlangeLength");
  viewPlateThick=Control.EvalVar<double>(keyName+"ViewPlateThick");

  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");


  return;
}


void
ViewScreenTube::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("ViewScreenTube","createSurfaces");

  if (!isActive("front"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*portALength,Y);
      ExternalCut::setCutSurf("front",SMap.realSurf(buildIndex+1));
    }
  if (!isActive("back"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*portBLength,Y);
      ExternalCut::setCutSurf("back",-SMap.realSurf(buildIndex+2));
    }

  // -- divider plate X/Y
  ModelSupport::buildPlane(SMap,buildIndex+100,Origin,X);
  ModelSupport::buildPlane(SMap,buildIndex+200,Origin,Y);
  ModelSupport::buildPlane(SMap,buildIndex+300,Origin,Z);

  // main pipe and thicness
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Z,radius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Z,radius+wallThick);

  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*depth,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*height,Z);

  // base plate:
  ModelSupport::buildPlane(SMap,buildIndex+15,
			   Origin-Z*(depth-wallThick),Z);

  // flange
  ModelSupport::buildCylinder(SMap,buildIndex+27,Origin,Z,flangeRadius);
    
  ModelSupport::buildPlane(SMap,buildIndex+16,
			   Origin+Z*(height-flangeLength),Z);

  ModelSupport::buildPlane(SMap,buildIndex+26,
			   Origin+Z*(height+plateThick),Z);


  // Front/back Ports:

  // 
  ModelSupport::buildCylinder(SMap,buildIndex+107,Origin,Y,portARadius);
  ModelSupport::buildCylinder
    (SMap,buildIndex+117,Origin,Y,portARadius+wallThick);
  ModelSupport::buildCylinder(SMap,buildIndex+127,Origin,Y,flangeARadius);
  ModelSupport::buildPlane
    (SMap,buildIndex+101,Origin-Y*(portALength-flangeALength),Y);
  

  // Back port
  ModelSupport::buildCylinder(SMap,buildIndex+207,Origin,Y,portBRadius);
  ModelSupport::buildCylinder
    (SMap,buildIndex+217,Origin,Y,portBRadius+wallThick);
  ModelSupport::buildCylinder(SMap,buildIndex+227,Origin,Y,flangeBRadius);
  ModelSupport::buildPlane
    (SMap,buildIndex+202,Origin+Y*(portBLength-flangeBLength),Y);

  // VIEWPORT
  const Geometry::Vec3D XY((-X-Y)/sqrt(2.0));
  
  ModelSupport::buildCylinder(SMap,buildIndex+307,Origin,XY,viewRadius);
  ModelSupport::buildCylinder
    (SMap,buildIndex+317,Origin,XY,viewRadius+wallThick);
  ModelSupport::buildCylinder
    (SMap,buildIndex+327,Origin,XY,viewFlangeRadius);
  
  ModelSupport::buildPlane(SMap,buildIndex+301,Origin+XY*viewLength,XY);
  ModelSupport::buildPlane
    (SMap,buildIndex+311,Origin+XY*(viewLength-viewFlangeLength),XY);
  ModelSupport::buildPlane
    (SMap,buildIndex+321,Origin+XY*(viewPlateThick+viewLength),XY);

   return;
}

void
ViewScreenTube::createObjects(Simulation& System)
  /*!
    Builds all the objects
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("ViewScreenTube","createObjects");

  HeadRule HR;
  
  const HeadRule frontHR=getRule("front");
  const HeadRule backHR=getRule("back");

  // inner void
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"15 -6 -7");
  makeCell("Void",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"5 -6 7 -17 (107:200) (207:-200) (100:307)");
  makeCell("Tube",System,cellIndex++,wallMat,0.0,HR);
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"5 -15 -7");
  makeCell("Base",System,cellIndex++,wallMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-6 16 17 -27");
  makeCell("Flange",System,cellIndex++,wallMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-26 6 -27");
  makeCell("Plate",System,cellIndex++,wallMat,0.0,HR);

  // Main outer
  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"5 -16 17 -27 127 (227:-200) (327:100)");
  makeCell("OuterVoid",System,cellIndex++,0,0.0,HR);

  // front ::
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-107 -200 7");
  makeCell("FrontVoid",System,cellIndex++,voidMat,0.0,HR*frontHR);


  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-117 107 -200 17");
  makeCell("FrontWall",System,cellIndex++,wallMat,0.0,HR*frontHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-127 117 -101");
  makeCell("FrontFlange",System,cellIndex++,wallMat,0.0,HR*frontHR);

  // add in view port
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"317 -127 117 101 -200 17");
  makeCell("FrontOuter",System,cellIndex++,0,0.0,HR*frontHR);

  // back ::
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-207 200 7 ");
  makeCell("BackVoid",System,cellIndex++,voidMat,0.0,HR*backHR);


  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-217 207 200 17");
  makeCell("BackWall",System,cellIndex++,wallMat,0.0,HR*backHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-227 217 202");
  makeCell("BackFlange",System,cellIndex++,wallMat,0.0,HR*backHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-227 217 -202 200 17 5");
  makeCell("BackOuter",System,cellIndex++,0,0.0,HR*backHR);

  //
  // view tube ::
  //
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-307 -100 7 -301");
  makeCell("ViewVoid",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"117 -317 307 -100 17 -301");
  makeCell("ViewWall",System,cellIndex++,wallMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-327 317 311 -301");
  makeCell("ViewFlange",System,cellIndex++,wallMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-327 301 -321");
  makeCell("ViewPlate",System,cellIndex++,wallMat,0.0,HR);

  // add front port in as well
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"127 -327 317 -311 -100 17");
  makeCell("ViewOuter",System,cellIndex++,0,0.0,HR);

  // outer void box:
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"5 -26 -27");
  HR+=ModelSupport::getHeadRule(SMap,buildIndex,"-127 -200")*frontHR;
  HR+=ModelSupport::getHeadRule(SMap,buildIndex,"-227 200")*backHR;
  HR+=ModelSupport::getHeadRule(SMap,buildIndex,"-321 -327 -100");

  // HR=ModelSupport::getHeadRule
  //   (SMap,buildIndex," (-227:-200) (200:-127) 15 -26 -27");
  addOuterSurf(HR);
  
  return;
}

void 
ViewScreenTube::createLinks()
  /*!
    Create the linked units
   */
{
  ELog::RegMethod RegA("ViewScreenTube","createLinks");

  ExternalCut::createLink("front",*this,0,Origin,Y);  //front and back
  ExternalCut::createLink("back",*this,1,Origin,Y);  //front and back
  
  FixedComp::setConnect(2,Origin-Z*depth,Z);
  FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+5));

  FixedComp::setConnect(3,Origin+Z*(height+plateThick),Z);
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+26));

  return;
}

void
ViewScreenTube::createAll(Simulation& System,
	       const attachSystem::FixedComp& FC,
	       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed point track 
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("ViewScreenTube","createAll");
  
  populate(System.getDataBase());
  createCentredUnitVector(FC,sideIndex,portALength);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE tdcSystem
