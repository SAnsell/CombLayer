/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/BremTube.cxx
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

#include "BremTube.h"

namespace xraySystem
{

BremTube::BremTube(const std::string& Key) :
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


BremTube::~BremTube() 
  /*!
    Destructor
  */
{}

void
BremTube::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase for variables
  */
{
  ELog::RegMethod RegA("BremTube","populate");

  FixedRotate::populate(Control);

  frontRadius=Control.EvalVar<double>(keyName+"FrontRadius");
  frontLength=Control.EvalVar<double>(keyName+"FrontLength");
  frontFlangeRadius=Control.EvalVar<double>(keyName+"FrontFlangeRadius");
  frontFlangeLength=Control.EvalVar<double>(keyName+"FrontFlangeLength");

  midRadius=Control.EvalVar<double>(keyName+"MidRadius");
  midLength=Control.EvalVar<double>(keyName+"MidLength");

  tubeRadius=Control.EvalVar<double>(keyName+"TubeRadius");
  tubeHeight=Control.EvalVar<double>(keyName+"TubeHeight");
  tubeDepth=Control.EvalVar<double>(keyName+"TubeDepth");
  tubeFlangeRadius=Control.EvalVar<double>(keyName+"TubeFlangeRadius");
  tubeFlangeLength=Control.EvalVar<double>(keyName+"TubeFlangeLength");

  backRadius=Control.EvalVar<double>(keyName+"BackRadius");
  backLength=Control.EvalVar<double>(keyName+"BackLength");
  backFlangeRadius=Control.EvalVar<double>(keyName+"BackFlangeRadius");
  backFlangeLength=Control.EvalVar<double>(keyName+"BackFlangeLength");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  plateThick=Control.EvalVar<double>(keyName+"PlateThick");

  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  plateMat=ModelSupport::EvalMat<int>(Control,keyName+"PlateMat");

  return;
}


void
BremTube::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("BremTube","createSurfaces");

  const double frontDist(frontLength+midLength);
  if (!isActive("front"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*frontDist,Y);
      ExternalCut::setCutSurf("front",SMap.realSurf(buildIndex+1));
    }
  if (!isActive("back"))
    {
      ModelSupport::buildPlane
	(SMap,buildIndex+2,Origin+Y*backLength,Y);
      ExternalCut::setCutSurf("back",-SMap.realSurf(buildIndex+2));
    }

  // vertical/horizontal dividers
  ModelSupport::buildPlane(SMap,buildIndex+100,Origin,X);
  ModelSupport::buildPlane(SMap,buildIndex+200,Origin,Y);
  ModelSupport::buildPlane(SMap,buildIndex+300,Origin,Z);

  // front/back pipe and thickness
  ModelSupport::buildCylinder(SMap,buildIndex+107,
			      Origin,Y,frontRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+117,
			      Origin,Y,frontRadius+wallThick);
  ModelSupport::buildCylinder(SMap,buildIndex+127,
			      Origin,Y,frontFlangeRadius);

  ModelSupport::buildPlane
    (SMap,buildIndex+101,Origin-Y*(frontDist-frontFlangeLength),Y);

  // Mid section
  ModelSupport::buildPlane(SMap,buildIndex+202,Origin-Y*midLength,Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+212,Origin-Y*(midLength-wallThick),Y);

  ModelSupport::buildCylinder(SMap,buildIndex+207,Origin,Y,midRadius);
  ModelSupport::buildCylinder
    (SMap,buildIndex+217,Origin,Y,midRadius+wallThick);

  // Main (VERTICAL) tube
  ModelSupport::buildPlane(SMap,buildIndex+305,Origin-Z*tubeDepth,Z);
  ModelSupport::buildPlane(SMap,buildIndex+306,Origin+Z*tubeHeight,Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+315,Origin-Z*(tubeDepth-tubeFlangeLength),Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+316,Origin+Z*(tubeHeight-tubeFlangeLength),Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+325,Origin-Z*(tubeDepth+plateThick),Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+326,Origin+Z*(tubeHeight+plateThick),Z);

  makeCylinder("innerRadius",SMap,buildIndex+307,Origin,Z,tubeRadius);
  makeCylinder("outerRadius",SMap,buildIndex+317,Origin,Z,tubeRadius+wallThick);
  makeCylinder("flangeRadius",SMap,buildIndex+327,Origin,Z,tubeFlangeRadius);

  // EXIT:    
  makePlane("flangeSurf",SMap,buildIndex+412,
	    Origin+Y*(backLength-backFlangeLength),Y); 

  ModelSupport::buildCylinder(SMap,buildIndex+407,Origin,Y,backRadius);
   
  ModelSupport::buildCylinder
    (SMap,buildIndex+417,Origin,Y,backRadius+wallThick);

  ModelSupport::buildCylinder(SMap,buildIndex+427,Origin,Y,backFlangeRadius);

  return;
}

void
BremTube::createObjects(Simulation& System)
  /*!
    Builds all the objects
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("BremTube","createObjects");

  std::string Out;
  
  const std::string frontStr=getRuleStr("front");
  const std::string backStr=getRuleStr("back");

  // Front
  Out=ModelSupport::getComposite(SMap,buildIndex," -107 -212");
  makeCell("FrontVoid",System,cellIndex++,voidMat,0.0,Out+frontStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," 107 -117 -202");
  makeCell("FrontTube",System,cellIndex++,wallMat,0.0,Out+frontStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," 117 -127 -101");
  makeCell("FrontFlange",System,cellIndex++,wallMat,0.0,Out+frontStr);

  // Note : using 427
  Out=ModelSupport::getComposite(SMap,buildIndex,"-427 127 -101");
  makeCell("FrontFlangeVoid",System,cellIndex++,0,0.0,Out+frontStr);

  // Note : using 427
  Out=ModelSupport::getComposite(SMap,buildIndex,"-202 -427 117 101");
  makeCell("FrontOuterVoid",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 107 -212 202 -217");
  makeCell("MidPlate",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -200 307 -207 212");
  makeCell("MidVoid",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -200 317 -217 207 212");
  makeCell("MidWall",System,cellIndex++,wallMat,0.0,Out);

  // Note : using 427
  Out=ModelSupport::getComposite(SMap,buildIndex," -427 -200 317 217 202");
  makeCell("MidOuterVoid",System,cellIndex++,0,0.0,Out);

  // main tube
  Out=ModelSupport::getComposite(SMap,buildIndex,"305 -306 -307");
  makeCell("Void",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex,"305 -306 307 -317 (207:200) (407:-200)");
  makeCell("TubeWall",System,cellIndex++,wallMat,0.0,Out);
  
  Out=ModelSupport::getComposite(SMap,buildIndex,"-315 305 317 -327");
  makeCell("TubeBaseFlange",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"316 -306 317 -327");
  makeCell("TubeTopFlange",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"325 -305 -327");
  makeCell("TubeBaseCap",System,cellIndex++,plateMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"-326 306 -327");
  makeCell("TubeTopCap",System,cellIndex++,plateMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"315 -316 -327 317 427");
  makeCell("TubeOuterVoid",System,cellIndex++,0,0.0,Out);

  // Extention
  Out=ModelSupport::getComposite(SMap,buildIndex,"200 307 -407 ");
  makeCell("BackVoid",System,cellIndex++,0,0.0,Out+backStr);

  Out=ModelSupport::getComposite(SMap,buildIndex,"200 317 407 -417");
  makeCell("BackWall",System,cellIndex++,wallMat,0.0,Out+backStr);

  Out=ModelSupport::getComposite(SMap,buildIndex,"417 -427 412 ");
  makeCell("BackFlange",System,cellIndex++,wallMat,0.0,Out+backStr);

  Out=ModelSupport::getComposite(SMap,buildIndex,"-427 417 -412 200 317");
  makeCell("BackOuterVoid",System,cellIndex++,0,0.0,Out);

  // outer void box:
  Out=ModelSupport::getComposite(SMap,buildIndex,"-427 ");
  addOuterSurf(Out+frontStr+backStr);

  Out=ModelSupport::getComposite(SMap,buildIndex,"-327 325 -326");
  addOuterUnionSurf(Out);

  return;
}

void 
BremTube::createLinks()
  /*!
    Create the linked units
  */
{
  ELog::RegMethod RegA("BremTube","createLinks");

  ExternalCut::createLink("front",*this,0,Origin,Y);  //front and back
  ExternalCut::createLink("back",*this,1,Origin,Y);  //front and back

  FixedComp::setConnect(2,Origin-X*(tubeRadius+wallThick),-X);
  FixedComp::setLinkSurf(2,SMap.realSurf(buildIndex+417));

  FixedComp::setConnect(3,Origin+X*(tubeRadius+wallThick),X);
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+417));

  FixedComp::setConnect(4,Origin-Z*(tubeDepth+plateThick),-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+425));

  FixedComp::setConnect(5,Origin+Z*(tubeHeight+plateThick),Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+426));
  
  return;
}

void
BremTube::createAll(Simulation& System,
	       const attachSystem::FixedComp& FC,
	       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed point track 
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("BremTube","createAll");
  
  populate(System.getDataBase());
  createCentredUnitVector(FC,sideIndex,frontLength+midLength);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE tdcSystem
