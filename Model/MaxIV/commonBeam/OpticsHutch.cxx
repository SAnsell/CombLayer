/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonBeam/OpticsHutch.cxx
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
#include <array>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "objectRegister.h"
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
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "PortChicane.h"

#include "OpticsHutch.h"

namespace xraySystem
{

OpticsHutch::OpticsHutch(const std::string& Key) :
  attachSystem::FixedOffset(Key,18),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  attachSystem::SurfMap()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{
  nameSideIndex(15,"floorCut");
  nameSideIndex(16,"roofCut");
  nameSideIndex(17,"frontCut");
}


OpticsHutch::~OpticsHutch()
  /*!
    Destructor
  */
{}

void
OpticsHutch::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("OpticsHutch","populate");

  FixedOffset::populate(Control);
  height=Control.EvalVar<double>(keyName+"Height");
  length=Control.EvalVar<double>(keyName+"Length");
  outWidth=Control.EvalVar<double>(keyName+"OutWidth");
  ringExtra=Control.EvalVar<double>(keyName+"RingExtra");
  ringFlat=Control.EvalVar<double>(keyName+"RingFlat");
  
  
  innerThick=Control.EvalVar<double>(keyName+"InnerThick");
  pbWallThick=Control.EvalVar<double>(keyName+"PbWallThick");
  pbFrontThick=Control.EvalVar<double>(keyName+"PbFrontThick");
  pbBackThick=Control.EvalVar<double>(keyName+"PbBackThick");
  pbRoofThick=Control.EvalVar<double>(keyName+"PbRoofThick");
  outerThick=Control.EvalVar<double>(keyName+"OuterThick");

  innerOutVoid=Control.EvalDefVar<double>(keyName+"InnerOutVoid",0.0);
  outerOutVoid=Control.EvalDefVar<double>(keyName+"OuterOutVoid",0.0);
  extension=Control.EvalDefVar<double>(keyName+"Extension",0.0);

  holeXStep=Control.EvalVar<double>(keyName+"HoleXStep");
  holeZStep=Control.EvalVar<double>(keyName+"HoleZStep");
  holeRadius=Control.EvalVar<double>(keyName+"HoleRadius");

  //  beamTubeRadius=Control.EvalVar<double>(keyName+"BeamTubeRadius");

  skinMat=ModelSupport::EvalMat<int>(Control,keyName+"SkinMat");
  pbMat=ModelSupport::EvalMat<int>(Control,keyName+"PbMat");
  concreteMat=ModelSupport::EvalMat<int>(Control,keyName+"ConcreteMat");
  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  
  return;
}

void
OpticsHutch::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("OpticsHutch","createSurfaces");

  // getsurfaces to expand:
  
  // Inner void
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*length,Y);
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*outWidth,X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*ringFlat,X);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*height,Z);

  if (innerOutVoid>Geometry::zeroTol)
    ModelSupport::buildPlane
      (SMap,buildIndex+1003,Origin-X*(outWidth-innerOutVoid),X);

  // Steel inner layer

  ModelSupport::buildPlane(SMap,buildIndex+12,
			   Origin+Y*(length+innerThick),Y);
  ModelSupport::buildPlane(SMap,buildIndex+13,
			   Origin-X*(outWidth+innerThick),X);
  ModelSupport::buildPlane(SMap,buildIndex+16,
			       Origin+Z*(height+innerThick),Z);
  
  // Lead
  ModelSupport::buildPlane(SMap,buildIndex+22,
			   Origin+Y*(length+innerThick+pbBackThick),Y);
  ModelSupport::buildPlane(SMap,buildIndex+23,
			   Origin-X*(outWidth+innerThick+pbWallThick),X);
  ModelSupport::buildPlane(SMap,buildIndex+26,
			       Origin+Z*(height+innerThick+pbRoofThick),Z);

  const double steelThick(innerThick+outerThick);

  // OuterWall

  SurfMap::makePlane("outerWall",SMap,buildIndex+32,
		     Origin+Y*(length+steelThick+pbBackThick),Y);
  
  ModelSupport::buildPlane(SMap,buildIndex+33,
			   Origin-X*(outWidth+steelThick+pbWallThick),X);

  SurfMap::makePlane("roof",SMap,buildIndex+36,
		     Origin+Z*(height+steelThick+pbRoofThick),Z);


  // 
  // Side Cut
  //
  ExternalCut::makeShiftedSurf(SMap,"SideWall",buildIndex+104,X,-ringExtra);

  if (holeRadius>Geometry::zeroTol)
    ModelSupport::buildCylinder
      (SMap,buildIndex+107,Origin+X*holeXStep+Z*holeZStep,Y,holeRadius);

  // extra for chicanes
  if (outerOutVoid>Geometry::zeroTol)
    ModelSupport::buildPlane
      (SMap,buildIndex+1033,
       Origin-X*(outWidth+steelThick+pbWallThick+outerOutVoid),X);
  
  return;
}

void
OpticsHutch::createObjects(Simulation& System)
  /*!
    Adds the main objects
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("OpticsHutch","createObjects");

  /* Walls going forward are : */
  
  // ring wall
  const HeadRule sideWall=ExternalCut::getValidRule("SideWall",Origin);
  const HeadRule floor=ExternalCut::getValidRule("Floor",Origin);
  const HeadRule frontWall=
    ExternalCut::getValidRule("RingWall",Origin+Y*length);

  HeadRule HR;

  HR=ModelSupport::getSetHeadRule
    (SMap,buildIndex,"-2 3 (-4:-104) -6");
  makeCell("Void",System,cellIndex++,voidMat,0.0,HR*floor*frontWall);
  // walls:

  
  HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"-2 -3 13 -6");
  makeCell("InnerWall",System,cellIndex++,skinMat,0.0,HR*floor*frontWall);
  HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"-2 -13 23 -6");
  makeCell("LeadWall",System,cellIndex++,pbMat,0.0,HR*floor*frontWall);
  HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"-2 -23 33 -6");
  makeCell("OuterWall",System,cellIndex++,skinMat,0.0,HR*floor*frontWall);

  
  HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"2 -12 33 -104 -6 107");
  makeCell("BackIWall",System,cellIndex++,skinMat,0.0,HR*floor);
  HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"12 -22 33 -104 -6 107");
  makeCell("BackPbWall",System,cellIndex++,pbMat,0.0,HR*floor);
  HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"22 -32 33 -104 -6 107");
  makeCell("BackOuterWall",System,cellIndex++,skinMat,0.0,HR*floor);


  HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"-32 33 (-104:-4) 6 -16");
  makeCell("RoofIWall",System,cellIndex++,skinMat,0.0,HR*frontWall);
  HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"-32 33 (-104:-4) 16 -26");
  makeCell("RoofPbWall",System,cellIndex++,pbMat,0.0,HR*frontWall);
  HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"-32 33 (-104:-4) 26 -36");
  makeCell("RoofOuterWall",System,cellIndex++,skinMat,0.0,HR*frontWall);

  
  HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"4 104  -32 -36 ");
  makeCell("ConcreteSide",System,cellIndex++,concreteMat,0.0,
   	   HR*frontWall*sideWall*floor);
  
  // Outer void for pipe

  if (holeRadius>Geometry::zeroTol)
    {
      HR=ModelSupport::getSetHeadRule(SMap,buildIndex," 2 -32 -107");
      makeCell("ExitHole",System,cellIndex++,voidMat,0.0,HR);
    }

  // EXCLUDE:
  if (outerOutVoid>Geometry::zeroTol)
    {
      HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"-32 1033 -33 -36");
      makeCell("OuterVoid",System,cellIndex++,voidMat,0.0,HR*floor*frontWall);
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"-32 1033 -36");
    }
  else
    HR=ModelSupport::getHeadRule(SMap,buildIndex,"-32 33 -36");


  addOuterSurf(HR*frontWall*sideWall);

  return;
}

void
OpticsHutch::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("OpticsHutch","createLinks");

  const double wallThick(pbBackThick+innerThick+outerThick);
  
  setConnect(0,Origin,Y);
  setLinkSurf(0,ExternalCut::getValidRule("RingWall",Origin+Y*length));

  setConnect(1,Origin+Y*(length+wallThick),Y);
  setLinkSurf(1,SMap.realSurf(buildIndex+32));


  setConnect(7,Origin+X*holeXStep+Z*holeZStep+Y*(length+wallThick),Y);
  setLinkSurf(7,SMap.realSurf(buildIndex+32));

  setConnect(8,Origin+X*holeXStep+Z*(holeRadius+holeZStep)+
	     Y*(length+wallThick),Z);
  setLinkSurf(8,SMap.realSurf(buildIndex+117));


  setConnect(11,Origin,Y);
  setLinkSurf(11,ExternalCut::getValidRule("RingWall",Origin+Y*length));

  setConnect(12,Origin+Y*length,-Y);
  setLinkSurf(12,-SMap.realSurf(buildIndex+2));
  
  nameSideIndex(7,"exitHole");
  nameSideIndex(8,"exitHoleRadius");
  nameSideIndex(11,"innerFront");
  nameSideIndex(12,"innerBack");

  return;
}

void
OpticsHutch::createChicane(Simulation& System)
  /*!
    Generic function to create chicanes
    \param System :: Simulation
  */
{
  ELog::RegMethod Rega("OpticsHutch","createChicane");
  return;
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  const FuncDataBase& Control=System.getDataBase();

  const size_t NChicane=
    Control.EvalDefVar<size_t>(keyName+"NChicane",0);

  for(size_t i=0;i<NChicane;i++)
    {
      const std::string NStr(std::to_string(i));
      std::shared_ptr<PortChicane> PItem=
	std::make_shared<PortChicane>(keyName+"Chicane"+NStr);

      OR.addObject(PItem);
      PItem->addInsertCell("Main",getCell("WallVoid"));
      PItem->addInsertCell("Inner",getCell("InnerWall",0));
      PItem->addInsertCell("Inner",getCell("LeadWall",0));
      PItem->addInsertCell("Inner",getCell("OuterWall",0));
      // set surfaces:

      PItem->setCutSurf("innerWall",*this,"innerLeftWall");
      PItem->setCutSurf("outerWall",*this,"leftWall");
      PItem->createAll(System,*this,getSideIndex("leftWall"));
      PItem->addInsertCell("Main",getCell("OuterVoid",0));

      PItem->insertObjects(System);
      PChicane.push_back(PItem);
      //      PItem->splitObject(System,23,getCell("WallVoid"));
      //      PItem->splitObject(System,24,getCell("SplitVoid"));
    }

  return;
}

void
OpticsHutch::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("OpticsHutch","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);

  createSurfaces();
  createObjects(System);

  createLinks();
  createChicane(System);
  insertObjects(System);

  return;
}

}  // NAMESPACE xraySystem
