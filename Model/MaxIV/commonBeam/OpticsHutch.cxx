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
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "objectRegister.h"
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
{}


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

  innerThick=Control.EvalVar<double>(keyName+"InnerThick");
  pbWallThick=Control.EvalVar<double>(keyName+"PbWallThick");
  pbBackThick=Control.EvalVar<double>(keyName+"PbBackThick");
  pbRoofThick=Control.EvalVar<double>(keyName+"PbRoofThick");
  outerThick=Control.EvalVar<double>(keyName+"OuterThick");
  
  innerOutVoid=Control.EvalDefVar<double>(keyName+"InnerOutVoid",0.0);
  outerOutVoid=Control.EvalDefVar<double>(keyName+"OuterOutVoid",0.0);
  backVoid=Control.EvalDefVar<double>(keyName+"BackVoid",0.0);

  double holeRad(0.0);
  size_t holeIndex(0);
  do
    {
      const std::string iStr("Hole"+std::to_string(holeIndex));
      const double holeXStep=
	Control.EvalDefVar<double>(keyName+iStr+"XStep",0.0);
      const double holeZStep=
	Control.EvalDefVar<double>(keyName+iStr+"ZStep",0.0);
      holeRad=
	Control.EvalDefVar<double>(keyName+iStr+"Radius",-1.0);

      if (holeRad>Geometry::zeroTol)
	{
	  holeOffset.push_back(Geometry::Vec3D(holeXStep,0.0,holeZStep));
	  holeRadius.push_back(holeRad);
	  holeIndex++;
	}
    } while(holeRad>Geometry::zeroTol);

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
  SurfMap::makePlane("innerWall",SMap,buildIndex+3,Origin-X*outWidth,X);
  
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
  SurfMap::makePlane("outerBack",SMap,buildIndex+32,
		     Origin+Y*(length+steelThick+pbBackThick),Y);
  
  SurfMap::makePlane("outerWall",SMap,buildIndex+33,
			   Origin-X*(outWidth+steelThick+pbWallThick),X);

  SurfMap::makePlane("roof",SMap,buildIndex+36,
		     Origin+Z*(height+steelThick+pbRoofThick),Z);

  if (backVoid>Geometry::zeroTol)
    SurfMap::makePlane("backVoid",SMap,buildIndex+42,
		       Origin+Y*(length+steelThick+pbBackThick+backVoid),Y);

  int BI(buildIndex);
  for(size_t i=0;i<holeRadius.size();i++)
    {
      const Geometry::Vec3D HPt(holeOffset[i].getInBasis(X,Y,Z));
      ModelSupport::buildCylinder(SMap,BI+107,Origin+HPt,Y,holeRadius[i]);
      BI+=100;
    }

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
  const HeadRule sideCut=ExternalCut::getValidRule("SideWallCut",Origin);
  
  const HeadRule floor=ExternalCut::getValidRule("Floor",Origin);
  const HeadRule frontWall=
    ExternalCut::getValidRule("RingWall",Origin+Y*length);

  HeadRule holeCut;
  int BI(buildIndex);
  for(size_t i=0;i<holeRadius.size();i++)
    {
      holeCut*=ModelSupport::getHeadRule(SMap,BI,"107");
      BI+=100;
    }
  
  HeadRule HR;
  if (innerOutVoid>Geometry::zeroTol)
    {  
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"-2 3 -1003 -6");
      makeCell("WallVoid",System,cellIndex++,voidMat,0.0,HR*floor*frontWall);
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"-2 1003 -6");
    }
  else
    {
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"-2 3 -6");
    }
  makeCell("Void",System,cellIndex++,voidMat,0.0,HR*floor*frontWall*sideCut);

  // walls:
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-2 -3 13 -6");
  makeCell("InnerWall",System,cellIndex++,skinMat,0.0,HR*floor*frontWall);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-12 -13 23 -16");
  makeCell("LeadWall",System,cellIndex++,pbMat,0.0,HR*floor*frontWall);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-22 -23 33 -26");
  makeCell("OuterWall",System,cellIndex++,skinMat,0.0,HR*floor*frontWall);

  
  HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"2 -12 13 -6");
  makeCell("BackIWall",System,cellIndex++,skinMat,0.0,
	   HR*floor*sideWall*holeCut);
  HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"12 -22 23 -16");
  makeCell("BackPbWall",System,cellIndex++,pbMat,0.0,
	   HR*floor*sideWall*holeCut);
  HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"22 -32 33 -26");
  makeCell("BackOuterWall",System,cellIndex++,skinMat,0.0,
	   HR*floor*sideWall*holeCut);

  if (backVoid>Geometry::zeroTol)
    {
      HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"32 -42 33 -26 ");
      makeCell("BackVoid",System,cellIndex++,0,0.0,HR*floor*sideWall);
    }

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-12 13 6 -16");
  makeCell("RoofIWall",System,cellIndex++,skinMat,0.0,HR*frontWall*sideCut);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-22 23 16 -26");
  makeCell("RoofPbWall",System,cellIndex++,pbMat,0.0,HR*frontWall*sideCut);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-32 33  26 -36");
  makeCell("RoofOuterWall",System,cellIndex++,skinMat,0.0,HR*frontWall*sideCut);

   
  // Outer void for pipe(s)
  BI=buildIndex;
  for(size_t i=0;i<holeRadius.size();i++)
    {
      HR=ModelSupport::getSetHeadRule(SMap,buildIndex,BI," 2 -32 -107M");
      makeCell("ExitHole",System,cellIndex++,voidMat,0.0,HR);
      BI+=100;
    }

  // EXCLUDE:
  if (outerOutVoid>Geometry::zeroTol)
    {
      HR=ModelSupport::getAltHeadRule(SMap,buildIndex,"-42A -32B 1033 -33 -36");
      makeCell("OuterVoid",System,cellIndex++,voidMat,0.0,HR*floor*frontWall);
      HR=ModelSupport::getAltHeadRule(SMap,buildIndex,"-42A -32B 1033 -36");
    }
  else
    HR=ModelSupport::getHeadRule(SMap,buildIndex,"-32 33 -36");


  addOuterSurf(HR*frontWall*sideCut);

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

  // outer lead wall
  const double steelThick(innerThick+outerThick);
  setConnect(3,Origin-X*(outWidth+steelThick+pbWallThick)+Y*(length/2.0),-X);
  setLinkSurf(3,-SMap.realSurf(buildIndex+33));
  nameSideIndex(3,"outerWall");

  for(size_t i=0;i<holeRadius.size();i++)
    {
      const Geometry::Vec3D HO(holeOffset[i].getInBasis(X,Y,Z));
      setConnect(7+2*i,Origin+HO+Y*(length+wallThick),Y);
      setLinkSurf(7+2*i,SMap.realSurf(buildIndex+32));
      setConnect(8+i*2,Origin+HO+Z*holeRadius[i]+Y*(length+wallThick),Z);
      setLinkSurf(8+2*i,SMap.realSurf(buildIndex+117));
      nameSideIndex(7+2*i,"exitHole"+std::to_string(i));
      nameSideIndex(8+2*i,"exitHole"+std::to_string(i)+"Radius");
    }



  setConnect(11,Origin,Y);
  setLinkSurf(11,ExternalCut::getValidRule("RingWall",Origin+Y*length));

  setConnect(12,Origin+Y*length,-Y);
  setLinkSurf(12,-SMap.realSurf(buildIndex+2));

  setConnect(13,Origin-X*outWidth+Y*(length/2.0),X);
  setLinkSurf(13,SMap.realSurf(buildIndex+3));
  nameSideIndex(13,"innerLeftWall");

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
      PItem->addInsertCell("Main",getCell("OuterVoid"));
      PItem->addInsertCell("Inner",getCell("InnerWall"));
      PItem->addInsertCell("Inner",getCell("LeadWall"));
      PItem->addInsertCell("Inner",getCell("OuterWall"));
      // set surfaces:

      PItem->setCutSurf("innerWall",this->getSurfRule("innerWall"));
      PItem->setCutSurf("outerWall",this->getSurfRule("#outerWall"));
      PItem->createAll(System,*this,getSideIndex("outerWall"));


      PItem->insertObjects(System);
      PChicane.push_back(PItem);
      ELog::EM<<"PChicange == "<<PItem->getCentre()<<ELog::endDiag;
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
