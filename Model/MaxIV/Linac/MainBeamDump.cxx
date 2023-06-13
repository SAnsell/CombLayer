/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Linac/MainBeamDump.cxx
 *
 * Copyright (c) 2004-2023 by Konstantin Batkov
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
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
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
#include "ContainedGroup.h"
#include "ExternalCut.h"
#include "BaseMap.h"
#include "SurfMap.h"
#include "CellMap.h"

#include "MainBeamDump.h"

namespace tdcSystem
{

MainBeamDump::MainBeamDump(const std::string& Key) :
  attachSystem::FixedRotate(Key,7),
  attachSystem::ContainedGroup("Main","FlangeA"),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  attachSystem::SurfMap()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}


MainBeamDump::~MainBeamDump()
  /*!
    Destructor
  */
{}

void
MainBeamDump::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase for variables
  */
{
  ELog::RegMethod RegA("MainBeamDump","populate");

  FixedRotate::populate(Control);

  width=Control.EvalVar<double>(keyName+"Width");
  length=Control.EvalVar<double>(keyName+"Length");
  height=Control.EvalVar<double>(keyName+"Height");
  depth=Control.EvalVar<double>(keyName+"Depth");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  portLength=Control.EvalVar<double>(keyName+"PortLength");


  return;
}


void
MainBeamDump::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("MainBeamDump","createSurfaces");

  if (!isActive("front"))
    {
      ModelSupport::buildPlane
	(SMap,buildIndex+1,Origin-Y*(portLength+length/2.0),Y);
      ExternalCut::setCutSurf("front",SMap.realSurf(buildIndex+1));
    }
  if (!isActive("back"))
    {
      ModelSupport::buildPlane
	(SMap,buildIndex+2,Origin+Y*(portLength+length/2.0),Y);
      ExternalCut::setCutSurf("back",-SMap.realSurf(buildIndex+2));
    }

  ModelSupport::buildPlane(SMap,buildIndex+101,Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+102,Origin+Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+103,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+104,Origin+X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+105,Origin-Z*depth,Z);
  ModelSupport::buildPlane(SMap,buildIndex+106,Origin+Z*height,Z);


  return;
}

void
MainBeamDump::createObjects(Simulation& System)
  /*!
    Builds all the objects
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("MainBeamDump","createObjects");

  HeadRule HR;

  const HeadRule& frontHR=getRule("front");
  const HeadRule& backHR=getRule("back");

  // // inner void : excluding main BS
  // HR=ModelSupport::getHeadRule
  //   (SMap,buildIndex,"101 -102 103 -104 105 -106"
  //   "(600: -605: 607 ) (-501 : 502 : 507)");
  // if (!closedFlag)
  //   HR*=ModelSupport::getHeadRule(SMap,buildIndex,"(617:605)");

  // makeCell("Void",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex," 101 -102 103 -104 105 -106");
  makeCell("SideWall",System,cellIndex++,0,0.0,HR);

  addOuterSurf("Main",HR);

  // HR=ModelSupport::getHeadRule(SMap,buildIndex,"-121 -227");
  // addOuterSurf("FlangeA",HR*frontHR);

  // HR=ModelSupport::getHeadRule(SMap,buildIndex,"122 -227");
  // addOuterSurf("FlangeB",HR*backHR);

  return;
}

void
MainBeamDump::createLinks()
  /*!
    Create the linked units
   */
{
  ELog::RegMethod RegA("MainBeamDump","createLinks");

  ExternalCut::createLink("front",*this,0,Origin,Y);  //front and back
  ExternalCut::createLink("back",*this,1,Origin,Y);  //front and back

  // FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+121));
  // FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+122));

  // FixedComp::setConnect(2,Origin-Y*(baseFlangeExtra+wallThick+length/2.0),-Y);
  // FixedComp::setConnect(3,Origin+Y*(baseFlangeExtra+wallThick+length/2.0),Y);

  // nameSideIndex(2,"BoxFront");
  // nameSideIndex(3,"BoxBack");

  return;
}

void
MainBeamDump::createAll(Simulation& System,
	       const attachSystem::FixedComp& FC,
	       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed point track
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("MainBeamDump","createAll");

  populate(System.getDataBase());
  createCentredUnitVector(FC,sideIndex,portLength+length/2.0);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // NAMESPACE tdcSystem
