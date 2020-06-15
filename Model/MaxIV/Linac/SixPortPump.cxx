/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   Linac/SixPortPump.cxx
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
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfDIter.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "Rules.h"
#include "SurInter.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "SimProcess.h"
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

#include "SixPortPump.h"

namespace tdcSystem
{

SixPortPump::SixPortPump(const std::string& Key) :
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


SixPortPump::~SixPortPump() 
  /*!
    Destructor
  */
{}

void
SixPortPump::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase for variables
  */
{
  ELog::RegMethod RegA("SixPortPump","populate");

  FixedRotate::populate(Control);

  radius=Control.EvalVar<double>(keyName+"Radius");
  linkRadius=Control.EvalVar<double>(keyName+"LinkRadius");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  
  frontLength=Control.EvalVar<double>(keyName+"FrontLength");
  backLength=Control.EvalVar<double>(keyName+"BackLength");
  sideLength=Control.EvalVar<double>(keyName+"SideLength");

  flangeARadius=Control.EvalVar<double>(keyName+"FlangeARadius");
  flangeBRadius=Control.EvalVar<double>(keyName+"FlangeBRadius");
  flangeSRadius=Control.EvalVar<double>(keyName+"FlangeSRadius");
  flangeALength=Control.EvalVar<double>(keyName+"FlangeALength");
  flangeBLength=Control.EvalVar<double>(keyName+"FlangeBLength");
  flangeSLength=Control.EvalVar<double>(keyName+"FlangeSLength");

  plateThick=Control.EvalVar<double>(keyName+"PlateThick");

  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  mainMat=ModelSupport::EvalMat<int>(Control,keyName+"MainMat");
  flangeMat=ModelSupport::EvalMat<int>(Control,keyName+"FlangeMat");
  plateMat=ModelSupport::EvalMat<int>(Control,keyName+"PlateMat");
  
  return;
}


void
SixPortPump::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("SixPortPump","createSurfaces");

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
  ModelSupport::buildPlane(SMap,buildIndex+200,Origin,Z);

  // main pipe and thicness
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,radius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,radius+wallThick);

  ModelSupport::buildPlane
    (SMap,buildIndex+101,Origin-Y*(frontLength-flangeAlLngth),Y);
  ModelSupport::buildCylinder(SMap,buildIndex+107,Origin,Y,flangeARadius);

  ModelSupport::buildPlane
    (SMap,buildIndex+202,Origin+Y*(backLength-flangeBLength),Y);
  ModelSupport::buildCylinder(SMap,buildIndex+207,Origin,Y,flangeBRadius);


  // horizontal tube

  ModelSupport::buildPlane(SMap,buildIndex+303,Origin-X*sideLength,X);
  ModelSupport::buildPlane
    (SMap,buildIndex+313,Origin-X*(sideLength-flangeSlength),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+323,Origin-X*(sideLength+plateThick),X);

  ModelSupport::buildPlane(SMap,buildIndex+304,Origin+X*sideLength,X);
  ModelSupport::buildPlane
    (SMap,buildIndex+314,Origin+X*(sideLength-flangeSlength),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+324,Origin+X*(sideLength+plateThick),X);

  ModelSupport::buildCylinder(SMap,buildIndex+307,Origin,X,radius);
  ModelSupport::buildCylinder(SMap,buildIndex+317,Origin,X,radius+wallThick);
  ModelSupport::buildCylinder(SMap,buildIndex+327,Origin,X,flangeSRadius);

  // vertical tube
  ModelSupport::buildPlane(SMap,buildIndex+405,Origin-Z*sideLength,Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+415,Origin-Z*(sideLength-flangeSlength),Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+425,Origin-Z*(sideLength+plateThick),Z);

  ModelSupport::buildPlane(SMap,buildIndex+406,Origin+Z*sideLength,Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+416,Origin+Z*(sideLength-flangeSlength),Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+426,Origin+Z*(sideLength+plateThick),Z);
  
  ModelSupport::buildCylinder(SMap,buildIndex+407,Origin,Z,radius);
  ModelSupport::buildCylinder(SMap,buildIndex+417,Origin,Z,radius+wallThick);
  ModelSupport::buildCylinder(SMap,buildIndex+427,Origin,Z,flangeSRadius);

   return;
}

void
SixPortPump::createObjects(Simulation& System)
  /*!
    Builds all the objects
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("SixPortPump","createObjects");

  std::string Out;
  
  const std::string frontStr=getRuleStr("front");
  const std::string backStr=getRuleStr("back");

  // inner void
  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -2 -7 ");
  makeCell("Void",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -2 7 -17 407 307");
  makeCell("MainTube",System,cellIndex++,mainMat,0.0,Out);
  
  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -101 17 -107 ");
  makeCell("FlangeA",System,cellIndex++,mainMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -2 202 17 -107 ");
  makeCell("FlangeB",System,cellIndex++,mainMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -100 -307 303 17 ");
  makeCell("LeftVoid",System,cellIndex++,mainMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -100 -317 307 303 17 ");
  makeCell("LeftWall",System,cellIndex++,mainMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 317 -327 303 -313 ");
  makeCell("LeftFlange",System,cellIndex++,mainMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 317 -327 303 -313 ");
  makeCell("LeftPlate",System,cellIndex++,mainMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 100 -307 304 17 ");
  makeCell("RightVoid",System,cellIndex++,mainMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 100 -317 307 304 17 ");
  makeCell("RightWall",System,cellIndex++,mainMat,0.0,Out);
  

  
  Out=ModelSupport::getComposite(SMap,buildIndex," 155 -105 -107 ");
  makeCell("PlateA",System,cellIndex++,mainMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -106 116 17 -107 ");
  makeCell("FlangeB",System,cellIndex++,mainMat,0.0,Out);
  
  Out=ModelSupport::getComposite(SMap,buildIndex," -156 106 -107 ");
  makeCell("PlateB",System,cellIndex++,mainMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," 115 -116 327 17 -107 (200:227)");
  makeCell("OutTube",System,cellIndex++,0,0.0,Out);

  // view
  Out=ModelSupport::getComposite(SMap,buildIndex," 307 7 -200 -207 203");
  makeCell("viewVoid",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 317 17 -200 -217 207 203");
  makeCell("viewTube",System,cellIndex++,mainMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -227 217 203 -213");
  makeCell("viewFlange",System,cellIndex++,mainMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -227 -203 253 ");
  makeCell("viewPlate",System,cellIndex++,mainMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 217 -227 -200 17 213 327");
  makeCell("viewOut",System,cellIndex++,0,0.0,Out);


  // view 
  Out=ModelSupport::getComposite(SMap,buildIndex," -100 7 -307 ");
  makeCell("frontPort",System,cellIndex++,voidMat,0.0,Out+frontStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," (207:200) -100 17 307 -317 ");
  makeCell("frontWall",System,cellIndex++,mainMat,0.0,Out+frontStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," -411 -327 317 ");
  makeCell("frontFlange",System,cellIndex++,mainMat,0.0,Out+frontStr);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," (217:200) 411 -100 17 -327 317 ");
  makeCell("frontOut",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 100 7 -307 ");
  makeCell("backPort",System,cellIndex++,voidMat,0.0,Out+backStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," (207:200) 100 17 307 -317 ");
  makeCell("backWall",System,cellIndex++,mainMat,0.0,Out+backStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," 512 -327 317 ");
  makeCell("backFlange",System,cellIndex++,mainMat,0.0,Out+backStr);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," (217:200) -512 100 17 -327 317 ");
  makeCell("backOut",System,cellIndex++,0,0.0,Out);

  // outer void box:
  Out=ModelSupport::getComposite
    (SMap,buildIndex," 155 -156 253 (-327 : -107 : (-227 -200)) ");
  addOuterSurf(Out+frontStr+backStr);
  
  return;
}

void 
SixPortPump::createLinks()
  /*!
    Create the linked units
   */
{
  ELog::RegMethod RegA("SixPortPump","createLinks");

  ExternalCut::createLink("front",*this,0,Origin,Y);  //front and back
  ExternalCut::createLink("back",*this,1,Origin,Y);  //front and back
  
  FixedComp::setConnect(2,Origin-Z*(depth+plateThick),Z);
  FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+155));

  FixedComp::setConnect(3,Origin+Z*(height+plateThick),Z);
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+156));

  return;
}

void
SixPortPump::createAll(Simulation& System,
	       const attachSystem::FixedComp& FC,
	       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed point track 
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("SixPortPump","createAll");
  
  populate(System.getDataBase());
  createCentredUnitVector(FC,sideIndex,2.0*frontLength);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE tdcSystem
