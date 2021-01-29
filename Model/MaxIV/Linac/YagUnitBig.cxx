/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Linac/YagUnitBig.cxx
 *
 * Copyright (c) 2004-2021 by Stuart Ansell / Konstantin Batkov
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

#include "YagUnitBig.h"

namespace tdcSystem
{

YagUnitBig::YagUnitBig(const std::string& Key) :
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


YagUnitBig::~YagUnitBig()
  /*!
    Destructor
  */
{}

void
YagUnitBig::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase for variables
  */
{
  ELog::RegMethod RegA("YagUnitBig","populate");

  FixedRotate::populate(Control);

  radius=Control.EvalVar<double>(keyName+"Radius");
  height=Control.EvalVar<double>(keyName+"Height");
  depth=Control.EvalVar<double>(keyName+"Depth");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  flangeRadius=Control.EvalVar<double>(keyName+"FlangeRadius");
  flangeLength=Control.EvalVar<double>(keyName+"FlangeLength");
  plateThick=Control.EvalVar<double>(keyName+"PlateThick");

  viewAZStep=Control.EvalVar<double>(keyName+"ViewAZStep");
  viewARadius=Control.EvalVar<double>(keyName+"ViewARadius");
  viewAThick=Control.EvalVar<double>(keyName+"ViewAThick");
  viewALength=Control.EvalVar<double>(keyName+"ViewALength");
  viewAFlangeRadius=Control.EvalVar<double>(keyName+"ViewAFlangeRadius");
  viewAFlangeLength=Control.EvalVar<double>(keyName+"ViewAFlangeLength");
  viewAPlateThick=Control.EvalVar<double>(keyName+"ViewAPlateThick");

  viewBYStep=Control.EvalVar<double>(keyName+"ViewBYStep");
  viewBRadius=Control.EvalVar<double>(keyName+"ViewBRadius");
  viewBThick=Control.EvalVar<double>(keyName+"ViewBThick");
  viewBLength=Control.EvalVar<double>(keyName+"ViewBLength");
  viewBFlangeRadius=Control.EvalVar<double>(keyName+"ViewBFlangeRadius");
  viewBFlangeLength=Control.EvalVar<double>(keyName+"ViewBFlangeLength");
  viewBPlateThick=Control.EvalVar<double>(keyName+"ViewBPlateThick");

  portRadius=Control.EvalVar<double>(keyName+"PortRadius");
  portThick=Control.EvalVar<double>(keyName+"PortThick");
  portFlangeRadius=Control.EvalVar<double>(keyName+"PortFlangeRadius");
  portFlangeLength=Control.EvalVar<double>(keyName+"PortFlangeLength");

  frontLength=Control.EvalVar<double>(keyName+"FrontLength");
  backLength=Control.EvalVar<double>(keyName+"BackLength");

  outerRadius=Control.EvalVar<double>(keyName+"OuterRadius");

  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  mainMat=ModelSupport::EvalMat<int>(Control,keyName+"MainMat");

  return;
}


void
YagUnitBig::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("YagUnitBig","createSurfaces");

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

  // -- divider plate X/Y
  ModelSupport::buildPlane(SMap,buildIndex+100,Origin,Y);
  ModelSupport::buildPlane(SMap,buildIndex+200,Origin,X);

  // main pipe and thicness
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Z,radius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Z,radius+wallThick);


  ModelSupport::buildPlane(SMap,buildIndex+105,Origin-Z*depth,Z);
  ModelSupport::buildPlane(SMap,buildIndex+106,Origin+Z*height,Z);

  // flange
  ModelSupport::buildCylinder(SMap,buildIndex+107,Origin,Z,flangeRadius);

  ModelSupport::buildPlane(SMap,buildIndex+115,
			   Origin-Z*(depth-wallThick),Z);
  ModelSupport::buildPlane(SMap,buildIndex+116,
			   Origin+Z*(height-flangeLength),Z);

  // plate
  ModelSupport::buildPlane(SMap,buildIndex+156,
			   Origin+Z*(height+plateThick),Z);

  // view A
  const Geometry::Vec3D viewAOrg(Origin+Z*viewAZStep);
  ModelSupport::buildCylinder(SMap,buildIndex+207,viewAOrg,X,viewARadius);
  ModelSupport::buildCylinder
    (SMap,buildIndex+217,viewAOrg,X,viewARadius+viewAThick);

  ModelSupport::buildPlane(SMap,buildIndex+203,Origin-X*viewALength,X);

  // viewA flange
  ModelSupport::buildCylinder
    (SMap,buildIndex+227,viewAOrg,X,viewAFlangeRadius);
  ModelSupport::buildPlane
    (SMap,buildIndex+213,viewAOrg-X*(viewALength-viewAFlangeLength),X);
  // viewA plate
  ModelSupport::buildPlane
    (SMap,buildIndex+253,viewAOrg-X*(viewALength+viewAPlateThick),X);

  // view B
  const Geometry::Vec3D viewBOrg(Origin-Y*viewBYStep);
  ModelSupport::buildCylinder(SMap,buildIndex+607,viewBOrg,Z,viewBRadius);
  ModelSupport::buildCylinder
    (SMap,buildIndex+617,viewBOrg,Z,viewBRadius+viewBThick);

  ModelSupport::buildPlane(SMap,buildIndex+603,Origin-Z*viewBLength,Z);
  // viewB flange
  ModelSupport::buildCylinder
    (SMap,buildIndex+627,viewBOrg,Z,viewBFlangeRadius);
  ModelSupport::buildPlane
    (SMap,buildIndex+613,viewBOrg-Z*(viewBLength-viewBFlangeLength),Z);
  // viewB plate
  ModelSupport::buildPlane
    (SMap,buildIndex+653,viewBOrg-Z*(viewBLength+viewBPlateThick),Z);
  //////////////////

  // Both Front/Back port [300] -- front/back at [400/500]
  ModelSupport::buildCylinder(SMap,buildIndex+307,Origin,Y,portRadius);
  ModelSupport::buildCylinder
    (SMap,buildIndex+317,Origin,Y,portRadius+portThick);
  ModelSupport::buildCylinder(SMap,buildIndex+327,Origin,Y,portFlangeRadius);

  // front
  ModelSupport::buildPlane
    (SMap,buildIndex+411,Origin-Y*(frontLength-portFlangeLength),Y);
  // back
  ModelSupport::buildPlane
    (SMap,buildIndex+512,Origin+Y*(backLength-portFlangeLength),Y);

   return;
}

void
YagUnitBig::createObjects(Simulation& System)
  /*!
    Builds all the objects
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("YagUnitBig","createObjects");

  std::string Out;

  const std::string frontStr=getRuleStr("front");
  const std::string backStr=getRuleStr("back");

  // inner void
  Out=ModelSupport::getComposite(SMap,buildIndex," 115 -106 -7 ");
  makeCell("Void",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 105 -115 607 -7 ");
  makeCell("TubeA",System,cellIndex++,mainMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," 105 -106 7 -17 307 (200:207) ");
  makeCell("Tube",System,cellIndex++,mainMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -106 116 17 -107 ");
  makeCell("Flange",System,cellIndex++,mainMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -156 106 -107 ");
  makeCell("Plate",System,cellIndex++,mainMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," 105 -116 327 17 -107 (200:227)");
  makeCell("OutTube",System,cellIndex++,0,0.0,Out);

  // viewA
  Out=ModelSupport::getComposite(SMap,buildIndex," 307 7 -200 -207 203");
  makeCell("viewAVoid",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 317 17 -200 -217 207 203");
  makeCell("viewATube",System,cellIndex++,mainMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -227 217 203 -213");
  makeCell("viewAFlange",System,cellIndex++,mainMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -227 -203 253 ");
  makeCell("viewAPlate",System,cellIndex++,mainMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 217 -227 -200 17 213 327");
  makeCell("viewAOut",System,cellIndex++,0,0.0,Out);

  // viewB
  Out=ModelSupport::getComposite(SMap,buildIndex," -607 603 -115 ");
  makeCell("viewBVoid",System,cellIndex++,voidMat,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex," 607 -617 603 -105 ");
  makeCell("viewBTube",System,cellIndex++,mainMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -627 617 603 -613 ");
  makeCell("viewBFlange",System,cellIndex++,mainMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -107 617 613 -105 ");
  makeCell("viewBOut",System,cellIndex++,voidMat,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex," -107 627 653 -613 ");
  makeCell("viewBOut",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -627 653 -603 ");
  makeCell("viewBPlate",System,cellIndex++,mainMat,0.0,Out);


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
    (SMap,buildIndex," 653 -156 253 (-327 : -107 : (-227 -200)) ");
  addOuterSurf(Out+frontStr+backStr);

  return;
}

void
YagUnitBig::createLinks()
  /*!
    Create the linked units
   */
{
  ELog::RegMethod RegA("YagUnitBig","createLinks");

  ExternalCut::createLink("front",*this,0,Origin,Y);  //front and back
  ExternalCut::createLink("back",*this,1,Origin,Y);  //front and back

  FixedComp::setConnect(2,Origin-Z*(depth),Z);
  FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+105));

  FixedComp::setConnect(3,Origin+Z*(height+plateThick),Z);
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+156));

  return;
}

void
YagUnitBig::createAll(Simulation& System,
	       const attachSystem::FixedComp& FC,
	       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed point track
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("YagUnitBig","createAll");

  populate(System.getDataBase());
  createCentredUnitVector(FC,sideIndex,frontLength);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // NAMESPACE tdcSystem
