/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   Linac/YagUnit.cxx
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
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "ExternalCut.h"
#include "FrontBackCut.h" 
#include "BaseMap.h"
#include "SurfMap.h"
#include "CellMap.h" 

#include "YagUnit.h"

namespace tdcSystem
{

YagUnit::YagUnit(const std::string& Key) :
  attachSystem::FixedOffset(Key,6),
  attachSystem::ContainedComp(),
  attachSystem::FrontBackCut(),
  attachSystem::CellMap(),
  attachSystem::SurfMap()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}


YagUnit::~YagUnit() 
  /*!
    Destructor
  */
{}

void
YagUnit::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase for variables
  */
{
  ELog::RegMethod RegA("YagUnit","populate");

  FixedOffset::populate(Control);

  radius=Control.EvalVar<double>(keyName+"Radius");
  height=Control.EvalVar<double>(keyName+"Height");
  depth=Control.EvalVar<double>(keyName+"Depth");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  flangeRadius=Control.EvalVar<double>(keyName+"FlangeRadius");
  flangeLength=Control.EvalVar<double>(keyName+"FlangeLength");
  plateThick=Control.EvalVar<double>(keyName+"PlateThick");

  viewZStep=Control.EvalVar<double>(keyName+"ViewZStep");
  viewRadius=Control.EvalVar<double>(keyName+"ViewRadius");
  viewThick=Control.EvalVar<double>(keyName+"ViewThick");
  ELog::EM<<"View Thic == "<<viewThick<<ELog::endDiag;
  viewLength=Control.EvalVar<double>(keyName+"ViewLength");
  viewFlangeRadius=Control.EvalVar<double>(keyName+"ViewFlangeRadius");
  viewFlangeLength=Control.EvalVar<double>(keyName+"ViewFlangeLength");
  viewPlateThick=Control.EvalVar<double>(keyName+"ViewPlateThick");
  
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
YagUnit::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("YagUnit","createSurfaces");

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
			   Origin-Z*(depth-flangeLength),Z);
  ModelSupport::buildPlane(SMap,buildIndex+116,
			   Origin+Z*(height-flangeLength),Z);

  // plate
  ModelSupport::buildPlane(SMap,buildIndex+155,
			   Origin-Z*(depth+plateThick),Z);
  ModelSupport::buildPlane(SMap,buildIndex+156,
			   Origin+Z*(height+plateThick),Z);


  // view
  const Geometry::Vec3D viewOrg(Origin+Z*viewZStep);
  ModelSupport::buildCylinder(SMap,buildIndex+207,viewOrg,X,viewRadius);
  ModelSupport::buildCylinder
    (SMap,buildIndex+217,viewOrg,X,viewRadius+viewThick);
  
  ModelSupport::buildPlane(SMap,buildIndex+203,Origin-X*viewLength,X);

  // view flange

  ModelSupport::buildCylinder
    (SMap,buildIndex+227,viewOrg,X,viewFlangeRadius);    

  ModelSupport::buildPlane
    (SMap,buildIndex+213,viewOrg-X*(viewLength-viewFlangeLength),X);
  // view plate
  ModelSupport::buildPlane
    (SMap,buildIndex+253,viewOrg-X*(viewLength+viewPlateThick),X);

  // plate
  ModelSupport::buildPlane(SMap,buildIndex+151,
			   Origin-Z*(depth+plateThick),Z);
  ModelSupport::buildPlane(SMap,buildIndex+152,
			   Origin+Z*(height+plateThick),Z);
    
  // Both Front/Back port [300] -- front/back at [400/500]
  ModelSupport::buildCylinder(SMap,buildIndex+307,Origin,Y,portRadius);
  ModelSupport::buildCylinder
    (SMap,buildIndex+317,Origin,Y,portRadius+portThick);
  ModelSupport::buildCylinder(SMap,buildIndex+327,Origin,Y,portFlangeRadius);

  // front 
  ModelSupport::buildPlane(SMap,buildIndex+401,Origin-Y*frontLength,Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+411,Origin-Y*(frontLength-portFlangeLength),Y);
  // back
  ModelSupport::buildPlane(SMap,buildIndex+502,Origin+Y*backLength,Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+412,Origin+Y*(backLength-portFlangeLength),Y);

  
  return;
}

void
YagUnit::createObjects(Simulation& System)
  /*!
    Builds all the objects
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("YagUnit","createObjects");

  std::string Out;
  
  const std::string frontStr=getRuleStr("front");
  const std::string backStr=getRuleStr("back");

  // inner void
  Out=ModelSupport::getComposite(SMap,buildIndex," 105 -106 -7 ");
  makeCell("Void",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," 105 -106 7 -17 307 (200:207) ");
  makeCell("Tube",System,cellIndex++,mainMat,0.0,Out);
  
  Out=ModelSupport::getComposite(SMap,buildIndex," 105 -115 17 -107 ");
  makeCell("FlangeA",System,cellIndex++,mainMat,0.0,Out);

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

  Out=ModelSupport::getComposite(SMap,buildIndex," -401 -327 317 ");
  makeCell("frontFlange",System,cellIndex++,mainMat,0.0,Out+frontStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," (217:200) 401 -100 17 -327 317 ");
  makeCell("frontOut",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 100 7 -307 ");
  makeCell("backPort",System,cellIndex++,voidMat,0.0,Out+backStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," (207:200) 100 17 307 -317 ");
  makeCell("backWall",System,cellIndex++,mainMat,0.0,Out+backStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," 502 -327 317 ");
  makeCell("backFlange",System,cellIndex++,mainMat,0.0,Out+backStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," (217:200) -502 100 17 -327 317 ");
  makeCell("backOut",System,cellIndex++,0,0.0,Out);

  // outer void box:
  Out=ModelSupport::getComposite
    (SMap,buildIndex," 155 -156 253 (-327 : -107 : (-227 -200)) ");
  addOuterSurf(Out+frontStr+backStr);
  
  return;
}

void 
YagUnit::createLinks()
  /*!
    Create the linked units
   */
{
  ELog::RegMethod RegA("YagUnit","createLinks");

  ExternalCut::createLink("front",*this,0,Origin,Y);  //front and back
  ExternalCut::createLink("back",*this,1,Origin,Y);  //front and back
      
  return;
}

void
YagUnit::createAll(Simulation& System,
	       const attachSystem::FixedComp& FC,
	       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed point track 
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("YagUnit","createAll");
  
  populate(System.getDataBase());
  createCentredUnitVector(FC,sideIndex,2.0*frontLength);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE tdcSystem
