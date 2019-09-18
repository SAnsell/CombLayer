/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   R3Common/ExperimentalHutch.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"  
#include "FixedComp.h"
#include "FixedGroup.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "ExternalCut.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "PortChicane.h"

#include "ExperimentalHutch.h"

namespace xraySystem
{

ExperimentalHutch::ExperimentalHutch(const std::string& Key) : 
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

ExperimentalHutch::~ExperimentalHutch() 
  /*!
    Destructor
  */
{}

void
ExperimentalHutch::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("ExperimentalHutch","populate");

  FixedOffset::populate(Control);

  // Void + Fe special:
  depth=Control.EvalVar<double>(keyName+"Depth");
  height=Control.EvalVar<double>(keyName+"Height");
  length=Control.EvalVar<double>(keyName+"Length");
  outWidth=Control.EvalVar<double>(keyName+"OutWidth");
  ringWidth=Control.EvalVar<double>(keyName+"RingWidth");

  innerThick=Control.EvalVar<double>(keyName+"InnerThick");
  pbThick=Control.EvalVar<double>(keyName+"PbThick");
  outerThick=Control.EvalVar<double>(keyName+"OuterThick");

  innerOutVoid=Control.EvalDefVar<double>(keyName+"InnerOutVoid",0.0);
  outerOutVoid=Control.EvalDefVar<double>(keyName+"OuterOutVoid",0.0);

  holeXStep=Control.EvalDefVar<double>(keyName+"HoleXStep",0.0);
  holeZStep=Control.EvalDefVar<double>(keyName+"HoleZStep",0.0);
  holeRadius=Control.EvalDefVar<double>(keyName+"HoleRadius",0.0);

  voidMat=ModelSupport::EvalDefMat<int>(Control,keyName+"VoidMat",0);
  skinMat=ModelSupport::EvalMat<int>(Control,keyName+"SkinMat");
  pbMat=ModelSupport::EvalMat<int>(Control,keyName+"PbMat");
  holeMat=ModelSupport::EvalDefMat<int>(Control,keyName+"HoleMat",voidMat);

  return;
}

void
ExperimentalHutch::createUnitVector(const attachSystem::FixedComp& FC,
				    const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("ExperimentalHutch","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  return;
}
 
void
ExperimentalHutch::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("ExperimentalHutch","createSurfaces");

  // Inner void
  if (!isActive("frontWall"))
    ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);

  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*length,Y);
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*outWidth,X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*ringWidth,X);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*height,Z);  

  SurfMap::setSurf("innerBack",-SMap.realSurf(buildIndex+2));

  if (innerOutVoid>Geometry::zeroTol)
    ModelSupport::buildPlane
      (SMap,buildIndex+1003,Origin-X*(outWidth-innerOutVoid),X);  


  // Walls
  double extraThick(0.0);
  int HI(buildIndex+10);
  for(const double T : {innerThick,pbThick,outerThick})
    {
      extraThick+=T;
      if (!isActive("frontWall"))
	ModelSupport::buildPlane(SMap,HI+1,
				 Origin-Y*extraThick,Y);
      ModelSupport::buildPlane(SMap,HI+2,
			       Origin+Y*(length+extraThick),Y);
      ModelSupport::buildPlane(SMap,HI+3,
			       Origin-X*(outWidth+extraThick),X);
      ModelSupport::buildPlane(SMap,HI+4,
			       Origin+X*(ringWidth+extraThick),X);
      ModelSupport::buildPlane(SMap,HI+6,
			       Origin+Z*(height+extraThick),Z);  
      HI+=10;
    }  
  HI-=10;
  SurfMap::setSurf("outerBack",SMap.realSurf(HI+2));
  if (outerOutVoid>Geometry::zeroTol)
    ModelSupport::buildPlane
      (SMap,buildIndex+1033,
       Origin-X*(outWidth+extraThick+outerOutVoid),X);  

  
  if (holeRadius>Geometry::zeroTol)
    ModelSupport::buildCylinder
      (SMap,buildIndex+107,Origin+X*holeXStep+Z*holeZStep,Y,holeRadius);
  
  return;
}

void
ExperimentalHutch::createObjects(Simulation& System)
  /*!
    Adds the main objects
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("ExperimentalHutch","createObjects");

  std::string Out;

  const std::string floor=ExternalCut::getRuleStr("Floor");
  const std::string innerSideWall=
    ExternalCut::getComplementStr("InnerSideWall");
  const std::string frontWall=ExternalCut::getRuleStr("frontWall");

  if (innerOutVoid>Geometry::zeroTol)
    {
      Out=ModelSupport::getSetComposite(SMap,buildIndex,"1 -2 3 -1003 -6 ");
      makeCell("WallVoid",System,cellIndex++,voidMat,0.0,Out+floor+frontWall);
      Out=ModelSupport::getSetComposite(SMap,buildIndex,"1 -2 1003 -4 -6 ");
      makeCell("Void",System,cellIndex++,voidMat,0.0,Out+floor+frontWall);
    }
  else
    {
      Out=ModelSupport::getSetComposite(SMap,buildIndex,"1 -2 3 -4 -6 ");
      makeCell("Void",System,cellIndex++,0,0.0,Out+floor+frontWall);
    }

  // walls:
  int HI(buildIndex);

  std::list<int> matList({skinMat,pbMat,skinMat});
  for(const std::string& layer : {"Inner","Lead","Outer"})
    {
      const int mat=matList.front();
      matList.pop_front();
      Out=ModelSupport::getSetComposite(SMap,buildIndex,HI,"1 -2 -3M 13M -6 ");
      makeCell(layer+"Wall",System,cellIndex++,mat,0.0,Out+floor+frontWall);

      Out=ModelSupport::getSetComposite(SMap,buildIndex,HI,
					"1 -2  4M -14M -6 ");
      makeCell(layer+"Wall",System,cellIndex++,mat,0.0,Out+floor+frontWall);
      
      //back wall
      Out=ModelSupport::getSetComposite
	(SMap,buildIndex,HI,"2M -12M 33 -34 -6 ");
      makeCell(layer+"BackWall",System,cellIndex++,mat,0.0,Out+floor+frontWall);

      //front wall
      if (!isActive("frontWall"))
	{
	  Out=ModelSupport::getSetComposite(SMap,buildIndex,HI,
					    " 11M -1M  33 -34 -6 107 ");
	  makeCell(layer+"FrontWall",System,cellIndex++,mat,0.0,Out+floor);
	}
      
      // roof
      Out=ModelSupport::getSetComposite(SMap,buildIndex,HI,
					" 31 -32 33 -34 6M -16M ");
      makeCell(layer+"Roof",System,cellIndex++,mat,0.0,Out+frontWall);
      HI+=10;
    }

  if (!isActive("frontWall") && holeRadius>Geometry::zeroTol)
    {
      Out=ModelSupport::getSetComposite(SMap,buildIndex,HI," 1M -1 -107 ");
      makeCell("EntranceHole",System,cellIndex++,holeMat,0.0,Out);
    }

  // EXCLUDE:
  if (outerOutVoid>Geometry::zeroTol)
    {
      Out=ModelSupport::getSetComposite
	(SMap,buildIndex,HI," 1M -2M 1033 -3M -6M ");
      makeCell("OuterLeftVoid",System,cellIndex++,0,0.0,Out+floor+frontWall);

      Out=ModelSupport::getSetComposite
	(SMap,buildIndex,HI," 1M -2M 1033 -4M -6M ");
    }
  else
    {
      Out=ModelSupport::getSetComposite(SMap,HI," 1 -2 3 -4 -6 ");
    }

  addOuterSurf(Out+frontWall);

  return;
}

void
ExperimentalHutch::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("ExperimentalHutch","createLinks");

  const double extraWall(innerThick+outerThick+pbThick);


  setConnect(0,Origin-Y*extraWall,-Y);
  setConnect(1,Origin+Y*(length+extraWall),Y);
  
  setLinkSurf(0,-SMap.realSurf(buildIndex+31));
  setLinkSurf(1,SMap.realSurf(buildIndex+32));

  // outer lead wall
  setConnect(3,Origin-X*(extraWall+outWidth)+Y*(length/2.0),-X);
  setLinkSurf(3,-SMap.realSurf(buildIndex+33));
  nameSideIndex(3,"leftWall");
  // outer surf
  setConnect(4,Origin-X*(extraWall+ringWidth)+Y*(length/2.0),X);
  setLinkSurf(4,SMap.realSurf(buildIndex+34));
  nameSideIndex(4,"rightWall");

  setConnect(11,Origin,Y);
  setConnect(12,Origin+Y*length,-Y);

  if (!isActive("frontWall"))
    setLinkSurf(11,SMap.realSurf(buildIndex+1));
  else
    setLinkSurf(11,getRule("frontWall"));
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

  return;
}

  
void
ExperimentalHutch::createChicane(Simulation& System)
  /*!
    Generic function to create chicanes
    \param System :: Simulation 
  */
{
  ELog::RegMethod Rega("ExperimentalHutch","createChicane");

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
      PItem->addInsertCell("Main",getCell("OuterLeftVoid",0));

      PItem->insertObjects(System);
      PChicane.push_back(PItem);
      //      PItem->splitObject(System,23,getCell("WallVoid"));
      //      PItem->splitObject(System,24,getCell("SplitVoid"));      
    }
  return;
}


void
ExperimentalHutch::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("ExperimentalHutch","createAll(FC)");

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
