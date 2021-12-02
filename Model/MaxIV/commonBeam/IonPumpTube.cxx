/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/IonPumpTube.cxx
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

#include "IonPumpTube.h"

namespace xraySystem
{

IonPumpTube::IonPumpTube(const std::string& Key) :
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


IonPumpTube::~IonPumpTube() 
  /*!
    Destructor
  */
{}

void
IonPumpTube::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase for variables
  */
{
  ELog::RegMethod RegA("IonPumpTube","populate");

  FixedRotate::populate(Control);

  radius=Control.EvalVar<double>(keyName+"Radius");
  yRadius=Control.EvalVar<double>(keyName+"YRadius");  // beam axis
  wallThick=Control.EvalVar<double>(keyName+"WallThick");

  height=Control.EvalVar<double>(keyName+"Height");
  depth=Control.EvalVar<double>(keyName+"Depth");
  frontLength=Control.EvalVar<double>(keyName+"FrontLength");
  backLength=Control.EvalVar<double>(keyName+"BackLength");

  flangeYRadius=Control.EvalVar<double>(keyName+"FlangeYRadius");
  flangeZRadius=Control.EvalVar<double>(keyName+"FlangeZRadius");

  flangeYLength=Control.EvalVar<double>(keyName+"FlangeYLength");
  flangeZLength=Control.EvalVar<double>(keyName+"FlangeZLength");

  plateThick=Control.EvalVar<double>(keyName+"PlateThick");

  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  plateMat=ModelSupport::EvalMat<int>(Control,keyName+"PlateMat");

  return;
}


void
IonPumpTube::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("IonPumpTube","createSurfaces");

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
  ModelSupport::buildPlane(SMap,buildIndex+416,Origin+Z*(height+wallThick),Z);
  
  ModelSupport::buildCylinder(SMap,buildIndex+407,Origin,Z,radius);
  ModelSupport::buildCylinder(SMap,buildIndex+417,Origin,Z,radius+wallThick);
  ModelSupport::buildCylinder(SMap,buildIndex+427,Origin,Z,flangeZRadius);

   return;
}

void
IonPumpTube::createObjects(Simulation& System)
  /*!
    Builds all the objects
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("IonPumpTube","createObjects");

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

  // base part

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-300 -407 405 7");
  makeCell("LowVoid",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-300 -417 407 405 17");
  makeCell("LowWall",System,cellIndex++,wallMat,0.0,HR);
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"417 -427 405 -415");
  makeCell("LowFlange",System,cellIndex++,wallMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-427 425 -405");
  makeCell("LowPlate",System,cellIndex++,plateMat,0.0,HR);

  // TOP
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"300 -407 -406 7");
  makeCell("TopVoid",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"300 -417 407 -406 17");
  makeCell("TopWall",System,cellIndex++,wallMat,0.0,HR);  

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-417 -416 406");
  makeCell("TopBlank",System,cellIndex++,wallMat,0.0,HR);


  // vert void
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-427 415 -416 17 417");
  if (flangeZRadius-Geometry::zeroTol > frontLength-flangeYLength)
    {
      if (flangeZRadius-Geometry::zeroTol > backLength-flangeYLength)
	HR*=ModelSupport::getHeadRule(SMap,buildIndex,"((101 -202):107)");
      else
	HR*=ModelSupport::getHeadRule(SMap,buildIndex,"(101:107)");
    }
  else if (flangeZRadius-Geometry::zeroTol > backLength-flangeYLength)
    HR*=ModelSupport::getHeadRule(SMap,buildIndex,"(-202:107)");

  makeCell("VertOuter",System,cellIndex++,0,0.0,HR);
  
  // front void
  HR=ModelSupport::getHeadRule(SMap,buildIndex," -200 17  427 -107 101");
  makeCell("FrontOuter",System,cellIndex++,0,0.0,HR);

  // back void
  HR=ModelSupport::getHeadRule(SMap,buildIndex," 200 17  427 -207 -202 ");
  makeCell("BackOuter",System,cellIndex++,0,0.0,HR);

  // outer void box:
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-107");
  addOuterSurf(HR*frontHR*backHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"(425 -416 -427)");
  addOuterUnionSurf(HR);

  return;
}

void 
IonPumpTube::createLinks()
  /*!
    Create the linked units
   */
{
  ELog::RegMethod RegA("IonPumpTube","createLinks");

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
IonPumpTube::createAll(Simulation& System,
	       const attachSystem::FixedComp& FC,
	       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed point track 
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("IonPumpTube","createAll");
  
  populate(System.getDataBase());
  createCentredUnitVector(FC,sideIndex,frontLength);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE tdcSystem
