/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   Linac/PrismaChamber.cxx
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

#include "PrismaChamber.h"

namespace tdcSystem
{

PrismaChamber::PrismaChamber(const std::string& Key) :
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


PrismaChamber::~PrismaChamber() 
  /*!
    Destructor
  */
{}

void
PrismaChamber::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase for variables
  */
{
  ELog::RegMethod RegA("PrismaChamber","populate");

  FixedRotate::populate(Control);

  radius=Control.EvalVar<double>(keyName+"Radius");
  beamRadius=Control.EvalVar<double>(keyName+"BeamRadius");
  sideRadius=Control.EvalVar<double>(keyName+"SideRadius");

  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  height=Control.EvalVar<double>(keyName+"Height");
  depth=Control.EvalVar<double>(keyName+"Depth");

  beamXStep=Control.EvalVar<double>(keyName+"BeamXStep");

  frontLength=Control.EvalVar<double>(keyName+"FrontLength");
  backLength=Control.EvalVar<double>(keyName+"BackLength");
  sideLength=Control.EvalVar<double>(keyName+"SideLength");

  flangeRadius=Control.EvalVar<double>(keyName+"FlangeRadius");
  flangeXRadius=Control.EvalVar<double>(keyName+"FlangeXRadius");
  flangeBeamRadius=Control.EvalVar<double>(keyName+"FlangeBeamRadius");

  flangeLength=Control.EvalVar<double>(keyName+"FlangeLength");
  flangeXLength=Control.EvalVar<double>(keyName+"FlangeXLength");
  flangeBeamLength=Control.EvalVar<double>(keyName+"FlangeBeamLength");

  sideThick=Control.EvalVar<double>(keyName+"SideThick");
  plateThick=Control.EvalVar<double>(keyName+"PlateThick");

  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  plateMat=ModelSupport::EvalMat<int>(Control,keyName+"PlateMat");


  return;
}


void
PrismaChamber::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("PrismaChamber","createSurfaces");

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

  // Main cyclinder
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Z,radius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Z,radius+wallThick);
  ModelSupport::buildCylinder(SMap,buildIndex+27,Origin,Z,flangeRadius);  

  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*depth,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*height,Z);
  ModelSupport::buildPlane(SMap,buildIndex+15,Origin-Z*(depth+wallThick),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*(height+plateThick),Z);
  ModelSupport::buildPlane(SMap,buildIndex+26,Origin+Z*(height-flangeLength),Z);
  
  // front/back pipe and thickness
  const Geometry::Vec3D portCent(Origin+X*beamXStep);
  ModelSupport::buildCylinder(SMap,buildIndex+107,portCent,Y,beamRadius);
  ModelSupport::buildCylinder
    (SMap,buildIndex+117,portCent,Y,beamRadius+wallThick);
  ModelSupport::buildCylinder(SMap,buildIndex+127,portCent,Y,flangeBeamRadius);
  
  ExternalCut::makeShiftedSurf(SMap,"front",buildIndex+11,Y,flangeBeamLength);
  ExternalCut::makeShiftedSurf(SMap,"back",buildIndex+12,Y,-flangeBeamLength);

  // Cross tubes

  ModelSupport::buildPlane(SMap,buildIndex+303,Origin-X*sideLength,X);
  ModelSupport::buildPlane
    (SMap,buildIndex+313,Origin-X*(sideLength-flangeXLength),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+323,Origin-X*(sideLength+sideThick),X);

  ModelSupport::buildPlane(SMap,buildIndex+304,Origin+X*sideLength,X);
  ModelSupport::buildPlane
    (SMap,buildIndex+314,Origin+X*(sideLength-flangeXLength),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+324,Origin+X*(sideLength+sideThick),X);

  ModelSupport::buildCylinder(SMap,buildIndex+307,Origin,X,sideRadius);
  ModelSupport::buildCylinder
    (SMap,buildIndex+317,Origin,X,sideRadius+wallThick);
  ModelSupport::buildCylinder(SMap,buildIndex+327,Origin,X,flangeXRadius);

  return;
}

void
PrismaChamber::createObjects(Simulation& System)
  /*!
    Builds all the objects
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("PrismaChamber","createObjects");

  std::string Out;
  
  const std::string frontStr=getRuleStr("front");
  const std::string backStr=getRuleStr("back");

  // inner void
  Out=ModelSupport::getComposite(SMap,buildIndex," 5 -6 -7 ");
  makeCell("Void",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 7 -17 5 -6 107 307");
  makeCell("MainTube",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -17 15 -5 ");
  makeCell("Base",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"-27 17 -6 26");
  makeCell("TopFlange",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"-27 6 -16");
  makeCell("TopPlate",System,cellIndex++,wallMat,0.0,Out);

  // outer need exclude:
  Out=ModelSupport::getComposite(SMap,buildIndex,"-27 17 15 -26 127 327 ");
  makeCell("OutVoid",System,cellIndex++,0,0.0,Out);


  // front / back
  Out=ModelSupport::getComposite(SMap,buildIndex," -107 -200 7 ");
  makeCell("FrontPort",System,cellIndex++,voidMat,0.0,Out+frontStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," -117 107 -200 17 ");
  makeCell("FrontWall",System,cellIndex++,wallMat,0.0,Out+frontStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," -127 117 -11 ");
  makeCell("FrontFlange",System,cellIndex++,wallMat,0.0,Out+frontStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," -200 17 117 -127 11");
  makeCell("FrontOuter",System,cellIndex++,0,0.0,Out);
  
  Out=ModelSupport::getComposite(SMap,buildIndex," -107 200 7 ");
  makeCell("BackPort",System,cellIndex++,voidMat,0.0,Out+backStr);
  
  Out=ModelSupport::getComposite(SMap,buildIndex," -117 107 200 17 ");
  makeCell("BackWall",System,cellIndex++,wallMat,0.0,Out+backStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," -127 117 12 ");
  makeCell("BackFlange",System,cellIndex++,wallMat,0.0,Out+backStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," 200 17 117 -127 -12");
  makeCell("BackOuter",System,cellIndex++,0,0.0,Out);


  // left/right
  
  Out=ModelSupport::getComposite(SMap,buildIndex," -100 -307 303 7  ");
  makeCell("LeftVoid",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -100 -317 307 303 17 ");
  makeCell("LeftWall",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 317 -327 303 -313 ");
  makeCell("LeftFlange",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -327 -303 323 ");
  makeCell("LeftPlate",System,cellIndex++,plateMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 313 -100 17 317 -327 ");
  makeCell("LeftOuter",System,cellIndex++,0,0.0,Out);

  
  // Right  
  Out=ModelSupport::getComposite(SMap,buildIndex," 100 -307 -304 7  ");
  makeCell("RightVoid",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 100 -317 307 -304 17  ");
  makeCell("RightWall",System,cellIndex++,wallMat,0.0,Out);
  
  Out=ModelSupport::getComposite(SMap,buildIndex," 317 -327 -304 314 ");
  makeCell("RightFlange",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -327 304 -324");
  makeCell("RightPlate",System,cellIndex++,plateMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -314 100 17 317 -327 ");
  makeCell("RightOuter",System,cellIndex++,0,0.0,Out);

  // outer void box:
  Out=ModelSupport::getComposite(SMap,buildIndex," -27 15 -16  ");
  addOuterSurf(Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -327 323 -324  ");
  addOuterUnionSurf(Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -127 ");
  addOuterUnionSurf(Out+frontStr+backStr);

  return;
}

void 
PrismaChamber::createLinks()
  /*!
    Create the linked units
   */
{
  ELog::RegMethod RegA("PrismaChamber","createLinks");

  const Geometry::Vec3D portCent(Origin+X*beamXStep);
  
  ExternalCut::createLink("front",*this,0,portCent,Y);  //front and back
  ExternalCut::createLink("back",*this,1,portCent,Y);  //front and back

  FixedComp::setConnect(2,Origin-X*(sideLength+plateThick),-X);
  FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+323));

  FixedComp::setConnect(3,Origin+X*(sideLength+plateThick),X);
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+343));

  FixedComp::setConnect(4,Origin-Z*(depth+wallThick),-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+15));

  FixedComp::setConnect(5,Origin+Z*(height+plateThick),Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+16));
  
  return;
}

void
PrismaChamber::createAll(Simulation& System,
			 const attachSystem::FixedComp& FC,
			 const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed point track 
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("PrismaChamber","createAll");
  
  populate(System.getDataBase());
  createCentredUnitVector
    (FC,sideIndex,Geometry::Vec3D(-beamXStep,frontLength,0.0));
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE tdcSystem
