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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Plane.h"


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
  portRadius=Control.EvalVar<double>(keyName+"PortRadius");
  targetWidth=Control.EvalVar<double>(keyName+"TargetWidth");
  targetHeight=Control.EvalVar<double>(keyName+"TargetHeight");
  targetLength=Control.EvalVar<double>(keyName+"TargetLength");
  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");
  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  targetMat=ModelSupport::EvalMat<int>(Control,keyName+"TargetMat");


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
	(SMap,buildIndex+1,Origin-Y*(wallThick+length/2.0),Y);
      ExternalCut::setCutSurf("front",SMap.realSurf(buildIndex+1));
    }
  // if (!isActive("back"))
  //   {
  //     ModelSupport::buildPlane
  // 	(SMap,buildIndex+2,Origin+Y*(portLength+length/2.0),Y);
  //     ExternalCut::setCutSurf("back",-SMap.realSurf(buildIndex+2));
  //   }

  // Wall
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(wallThick+length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(wallThick+width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(wallThick+width/2.0),X);
  const auto p = ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(wallThick+depth),Z);

  ELog::EM << "plane 5: " << acos(p->getNormal().Z())*180/3.1415 << " deg" << ELog::endDiag;


  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(wallThick+height),Z);

  // Main body
  ModelSupport::buildPlane(SMap,buildIndex+11,Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+12,Origin+Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+13,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,Origin+X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+15,Origin-Z*depth,Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*height,Z);

  // Pipe
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,portRadius);
  ModelSupport::buildPlane(SMap,buildIndex+21,Origin-Y*(wallThick+length/2.0-portLength+targetLength*2),Y);
  ModelSupport::buildPlane(SMap,buildIndex+22,Origin-Y*(wallThick+length/2.0-portLength),Y);

  ModelSupport::buildPlane(SMap,buildIndex+23,Origin-X*(targetWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+24,Origin+X*(targetWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+25,Origin-Z*(targetHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+26,Origin+Z*(targetHeight/2.0),Z);

  ModelSupport::buildPlane(SMap,buildIndex+31,Origin-Y*(wallThick+length/2.0-portLength+targetLength),Y);

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
    (SMap,buildIndex,"11 -21 13 -14 15 -16 7");
  makeCell("MainBody",System,cellIndex++,mat,0.0,HR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"-21 -7");
  makeCell("Pipe",System,cellIndex++,voidMat,0.0,HR*frontHR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"21 -31 23 -24 25 -26");
  makeCell("TargetBox",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"31 -22 23 -24 25 -26");
  makeCell("Target",System,cellIndex++,targetMat,0.0,HR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"21 -22 13 -23 15 -16");
  makeCell("MainBodyTargetLeft",System,cellIndex++,mat,0.0,HR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"21 -22 24 -14 15 -16");
  makeCell("MainBodyTargetRight",System,cellIndex++,mat,0.0,HR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"21 -22 23 -24 15 -25");
  makeCell("MainBodyTargetBottom",System,cellIndex++,mat,0.0,HR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"21 -22 23 -24 26 -16");
  makeCell("MainBodyTargetTop",System,cellIndex++,mat,0.0,HR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"22 -12 13 -14 15 -16");
  makeCell("MainBodyAfterTarget",System,cellIndex++,mat,0.0,HR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"-11 3 -4 5 -6 7");
  makeCell("SideWallFront",System,cellIndex++,wallMat,0.0,HR*frontHR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"12 -2 3 -4 5 -6");
  makeCell("SideWallBack",System,cellIndex++,wallMat,0.0,HR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"11 -12 14 -4 5 -6");
  makeCell("SideWallLeft",System,cellIndex++,wallMat,0.0,HR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"11 -12 3 -13 5 -6");
  makeCell("SideWallRight",System,cellIndex++,wallMat,0.0,HR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"11 -12 13 -14 5 -15");
  makeCell("SideWallBottom",System,cellIndex++,wallMat,0.0,HR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"11 -12 13 -14 16 -6");
  makeCell("SideWallTop",System,cellIndex++,wallMat,0.0,HR);



  HR=ModelSupport::getHeadRule
    (SMap,buildIndex," -2 3 -4 5 -6");
  addOuterSurf("Main",HR*frontHR);

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

  ExternalCut::createLink("front",*this,0,Origin,Y);  //front
  //  ExternalCut::createLink("back",*this,1,Origin,Y);  //front and back

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
  createCentredUnitVector(FC,sideIndex,wallThick+length/2.0);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // NAMESPACE tdcSystem
