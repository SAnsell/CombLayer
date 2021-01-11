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

  inletXStep=Control.EvalVar<double>(keyName+"InletXStep");
  inletZStep=Control.EvalVar<double>(keyName+"InletZStep");
  inletRadius=Control.EvalVar<double>(keyName+"InletRadius");

  holeXStep=Control.EvalVar<double>(keyName+"HoleXStep");
  holeZStep=Control.EvalVar<double>(keyName+"HoleZStep");
  holeRadius=Control.EvalVar<double>(keyName+"HoleRadius");

  //  beamTubeRadius=Control.EvalVar<double>(keyName+"BeamTubeRadius");

  skinMat=ModelSupport::EvalMat<int>(Control,keyName+"SkinMat");
  pbMat=ModelSupport::EvalMat<int>(Control,keyName+"PbMat");
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

  // Steel inner layer

  ModelSupport::buildPlane(SMap,buildIndex+12,
			   Origin+Y*(length+innerThick),Y);
  ModelSupport::buildPlane(SMap,buildIndex+13,
			   Origin-X*(outWidth+innerThick),X);
  ModelSupport::buildPlane(SMap,buildIndex+16,
			       Origin+Z*(height+innerThick),Z);
  
  // Lead
  ModelSupport::buildPlane(SMap,buildIndex+21,
			   Origin+Y*(innerThick+pbFrontThick),Y);
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
  
  /*
    if (outerOutVoid>Geometry::zeroTol)
    ModelSupport::buildPlane
      (SMap,buildIndex+1033,
       Origin-X*(outWidth+steelThick+pbWallThick+outerOutVoid),X);

  if (inletRadius>Geometry::zeroTol)
    ModelSupport::buildCylinder
      (SMap,buildIndex+107,Origin+X*inletXStep+Z*inletZStep,Y,inletRadius);

  if (holeRadius>Geometry::zeroTol)
    ModelSupport::buildCylinder
      (SMap,buildIndex+117,Origin+X*holeXStep+Z*holeZStep,Y,holeRadius);

  */
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
  ELog::EM<<"Origin == "<<Origin<<ELog::endDiag;
  const HeadRule sideWall=ExternalCut::getValidRule("SideWall",Origin);
  const HeadRule floor=ExternalCut::getValidRule("Floor",Origin);
  const HeadRule frontWall=
    ExternalCut::getValidRule("RingWall",Origin+Y*length);


  HeadRule HR;

  HR=ModelSupport::getSetHeadRule
    (SMap,buildIndex,"-2 3 (-4:-104) -6");
  makeCell("Void",System,cellIndex++,voidMat,0.0,HR*floor*frontWall);

  // walls:
  int HI(buildIndex);

  HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"-2 -3 13 -6 ");
  makeCell("InnerWall",System,cellIndex++,skinMat,0.0,HR*floor*frontWall);
  HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"-2 -13 23 -6 ");
  makeCell("LeadWall",System,cellIndex++,pbMat,0.0,HR*floor*frontWall);
  HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"-2 -23 33 -6 ");
  makeCell("OuterWall",System,cellIndex++,skinMat,0.0,HR*floor*frontWall);

  (SMap,buildIndex,HI,"2M -12M 33 (-34:-134) -6 117 ");
  HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"2 -12 33 -104 -6");
  makeCell("BackIWall",System,cellIndex++,skinMat,0.0,HR*floor);
  HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"12 -22 33 -104 -6");
  makeCell("BackPbWall",System,cellIndex++,pbMat,0.0,HR*floor);
  HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"22 -32 33 -104 -6");
  makeCell("BackOuterWall",System,cellIndex++,skinMat,0.0,HR*floor);


  /*
  std::list<int> matList({skinMat,pbMat,skinMat});

  for(const std::string& layer : {"Inner","Lead","Outer"})
    {
      const int mat=matList.front();
      matList.pop_front();
      HR=ModelSupport::getSetHeadRule(SMap,buildIndex,HI,"1 -2 -3M 13M -6 ");
      makeCell(layer+"Wall",System,cellIndex++,mat,0.0,HR*floor);

      // HR=ModelSupport::getSetHeadRule(SMap,buildIndex,HI,
      // 					"1 -2  4M  104M   -6 ");
      // makeCell(layer+"Wall",System,cellIndex++,mat,0.0,HR+floor);


      makeCell(layer+"FrontWall",System,cellIndex++,mat,0.0,HR+floor);

      //back wall
      HR=ModelSupport::getSetHeadRule
	(SMap,buildIndex,HI,"2M -12M 33 (-34:-134) -6 117 ");
      makeCell(layer+"BackWall",System,cellIndex++,mat,0.0,HR+floor);
      addCell("BackWall",cellIndex-1);

      // roof
      HR=ModelSupport::getSetHeadRule
	(SMap,buildIndex,HI,"11M -32 33 (-34:-134) 6M -16M ");
      if (layer=="Outer") HR+=frontWall;
      makeCell(layer+"Roof",System,cellIndex++,mat,0.0,HR);
      HI+=10;
    }


  // Outer void for pipe

  if (inletRadius>Geometry::zeroTol)
    {
      Out=ModelSupport::getSetComposite(SMap,buildIndex," -1 -107 ");
      makeCell("Inlet",System,cellIndex++,voidMat,0.0,Out+frontWall);
    }


  if (holeRadius>Geometry::zeroTol)
    {
      Out=ModelSupport::getSetComposite(SMap,buildIndex,HI," 2 -2M -117 ");
      makeCell("ExitHole",System,cellIndex++,voidMat,0.0,Out);
    }

  // Filler space :
  Out=ModelSupport::getComposite
    (SMap,buildIndex," 34 134 -36 -132");
  makeCell("Filler",System,cellIndex++,voidMat,0.0,Out+sideWall+floor+frontWall);

  //  Extension (void outside back wall)
  if (extension>Geometry::zeroTol)
    {
      Out=ModelSupport::getSetComposite(SMap,buildIndex,HI," 2M -132 33 -134 -36");
      makeCell("Extension",System,cellIndex++,voidMat,0.0,Out+floor);
    }

  // EXCLUDE:
  if (outerOutVoid>Geometry::zeroTol)
    {
      Out=ModelSupport::getComposite
	(SMap,buildIndex,HI," -132 1033 -3M -6M ");
      makeCell("OuterVoid",System,cellIndex++,voidMat,0.0,Out+floor+frontWall);

      Out=ModelSupport::getComposite
	(SMap,buildIndex,HI," -132 1033 -6M ");
      Out+=innerSideWall;
    }
  else
    Out=ModelSupport::getComposite
      (SMap,buildIndex,HI," -132 3M (-4M:-104M) -6M ");

  */

  // dont need floor ??
  HR=ModelSupport::getSetHeadRule
    (SMap,buildIndex,"-32 33 (-4:-104) -36");

  addOuterSurf(HR*frontWall);

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
  /*
  const double extraFront(innerThick+outerThick+pbFrontThick);
  const double extraBack(innerThick+outerThick+pbBackThick);
  const double extraWall(innerThick+outerThick+pbWallThick);

  setConnect(0,Origin-Y*(extraFront),-Y);
  setConnect(1,Origin+Y*(length+extraBack),Y);

  setLinkSurf(0,-SMap.realSurf(buildIndex+31));
  setLinkSurf(1,SMap.realSurf(buildIndex+132));

  // inner surf
  setConnect(2,Origin+Y*length,-Y);
  setLinkSurf(2,-SMap.realSurf(buildIndex+2));

  // outer surf
  setConnect(3,Origin-X*(extraWall+outWidth)+Y*(length/2.0),-X);
  setLinkSurf(3,-SMap.realSurf(buildIndex+33));
  nameSideIndex(3,"leftWall");
  // outer surf
  setConnect(4,Origin-X*(extraWall+ringWidth)+Y*(length/2.0),X);
  setLinkSurf(4,SMap.realSurf(buildIndex+34));
  nameSideIndex(4,"rightWall");

  setConnect(7,Origin+X*holeXStep+Z*holeZStep+Y*length,-Y);
  setLinkSurf(7,-SMap.realSurf(buildIndex+2));
  nameSideIndex(7,"exitHole");

  setConnect(8,Origin+X*holeXStep+Z*(holeRadius+holeZStep)+Y*length,-Z);
  setLinkSurf(8,SMap.realSurf(buildIndex+117));
  nameSideIndex(8,"exitHoleRadius");

  setConnect(9,Origin+X*inletXStep+Z*inletZStep+Y*length,-Y);
  setLinkSurf(9,SMap.realSurf(buildIndex+1));
  nameSideIndex(9,"inlet");

  setConnect(10,Origin+X*inletXStep+Z*(inletRadius+inletZStep),-Z);
  setLinkSurf(10,SMap.realSurf(buildIndex+107));
  nameSideIndex(10,"inletRadius");

  setConnect(11,Origin,Y);
  setConnect(12,Origin+Y*length,-Y);

  setLinkSurf(11,SMap.realSurf(buildIndex+1));
  setLinkSurf(12,-SMap.realSurf(buildIndex+2));

  nameSideIndex(11,"innerFront");
  nameSideIndex(12,"innerBack");

  // inner surf
  setConnect(13,Origin-X*outWidth+Y*(length/2.0),X);
  setLinkSurf(13,SMap.realSurf(buildIndex+3));
  nameSideIndex(13,"innerLeftWall");

  setConnect(14,Origin-X*ringWidth+Y*(length/2.0),-X);
  setLinkSurf(14,-SMap.realSurf(buildIndex+4));
  nameSideIndex(14,"innerRightWall");


  const double steelThick(innerThick+outerThick);
  HeadRule mainCut;
  //  Out=ModelSupport::getComposite(SMap,buildIndex," 4:104 15 ");
  // setConnect(15,Origin-Z*(depth+floorThick),Z);
  // setLinkSurf(15,SMap.realSurf(buildIndex+34));
  // addLinkSurf(15,SMap.realSurf(buildIndex+134));
  // addLinkComp(15,-SMap.realSurf(buildIndex+15));
  // addLinkComp(15,SMap.realSurf(buildIndex+32));

  setConnect(16,Origin+Z*(height+steelThick+pbRoofThick),Y);
  setLinkSurf(16,SMap.realSurf(buildIndex+34));
  addLinkSurf(16,SMap.realSurf(buildIndex+134));
  addLinkComp(16,SMap.realSurf(buildIndex+36));
  addLinkComp(16,SMap.realSurf(buildIndex+132));

  setConnect(17,Origin,-Y);
  setLinkSurf(17,SMap.realSurf(buildIndex+34));
  addLinkSurf(17,SMap.realSurf(buildIndex+134));
  addLinkComp(17,SMap.realSurf(buildIndex+36));
  addLinkComp(17,SMap.realSurf(buildIndex+132));
  */
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
  /*
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
  */
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
