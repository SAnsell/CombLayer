/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   R3Common/ExperimentalHutch.cxx
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

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "Vec3D.h"
#include "Quaternion.h"
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
  height=Control.EvalVar<double>(keyName+"Height");
  length=Control.EvalVar<double>(keyName+"Length");
  ringWidth=Control.EvalVar<double>(keyName+"RingWidth");
  outWidth=Control.EvalVar<double>(keyName+"OutWidth");

  cornerLength=Control.EvalDefVar<double>(keyName+"CornerLength",-100.0);
  cornerAngle=Control.EvalDefVar<double>(keyName+"CornerAngle",45.0);

  pbFrontThick=Control.EvalDefVar<double>(keyName+"PbFrontThick",-1.0);

  fHoleRadius=Control.EvalDefVar<double>(keyName+"FHoleRadius",1.0);
  fHoleXStep=Control.EvalDefVar<double>(keyName+"FHoleXStep",0.0);
  fHoleZStep=Control.EvalDefVar<double>(keyName+"FHoleZStep",0.0);

  innerThick=Control.EvalVar<double>(keyName+"InnerThick");
  pbWallThick=Control.EvalVar<double>(keyName+"PbWallThick");
  pbBackThick=Control.EvalVar<double>(keyName+"PbBackThick");
  pbRoofThick=Control.EvalVar<double>(keyName+"PbRoofThick");
  outerThick=Control.EvalVar<double>(keyName+"OuterThick");

  innerOutVoid=Control.EvalDefVar<double>(keyName+"InnerOutVoid",0.0);
  outerOutVoid=Control.EvalDefVar<double>(keyName+"OuterOutVoid",0.0);

  // exit hole radii
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

  const size_t N=Control.EvalDefVar<size_t>(keyName+"NForkHoles",0);
  if (N)
    {
      forkYStep=Control.EvalVar<double>(keyName+"ForkYStep");
      forkLength=Control.EvalDefVar<double>(keyName+"ForkLength",60.0);
      forkHeight=Control.EvalDefVar<double>(keyName+"ForkHeight",10.0);
      for(size_t i=0;i<N;i++)
	fZStep.push_back(Control.EvalVar<double>
			 (keyName+"ForkZStep"+std::to_string(i)));
    }
  
  voidMat=ModelSupport::EvalDefMat(Control,keyName+"VoidMat",0);
  skinMat=ModelSupport::EvalMat<int>(Control,keyName+"SkinMat");
  pbMat=ModelSupport::EvalMat<int>(Control,keyName+"PbMat");

  return;
}

void
ExperimentalHutch::createSurfaces()
{
  ELog::RegMethod RegA("ExperimentalHutch","createSurfaces");

  // getsurfaces to expand:

    // ROTATION for Corner

  // Inner void
  if (!isActive("frontWall"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
      ExternalCut::setCutSurf("frontWall",SMap.realSurf(buildIndex+1));
    }
	
  if (!isActive("floor"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*height,Z);
      ExternalCut::setCutSurf("floor",SMap.realSurf(buildIndex+5));
    }

  if (pbFrontThick>Geometry::zeroTol)
    {
      const Geometry::Vec3D holeOrg(Origin+X*fHoleXStep+Z*fHoleZStep);
	
      makeShiftedSurf(SMap,"frontWall",
		      buildIndex+11,Y,innerThick);
      makeShiftedSurf(SMap,"frontWall",
		      buildIndex+21,Y,pbFrontThick+innerThick);    
      makeShiftedSurf(SMap,"frontWall",
		      buildIndex+31,Y,outerThick+pbFrontThick+innerThick);
      if (fHoleRadius>Geometry::zeroTol)
	makeCylinder("frontHole",SMap,buildIndex+7,holeOrg,Y,fHoleRadius);
    }
  
  // Inner void
  SurfMap::makePlane("innerBack",SMap,buildIndex+2,Origin+Y*length,Y);
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*outWidth,X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*ringWidth,X);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*height,Z);


  if (innerOutVoid>Geometry::zeroTol)
    {
      ModelSupport::buildPlane
	(SMap,buildIndex+1003,Origin-X*(outWidth-innerOutVoid),X);  
      ModelSupport::buildPlane
	(SMap,buildIndex+1004,Origin+X*(ringWidth-innerOutVoid),X);  
    }
  

  // Steel inner layer

  ModelSupport::buildPlane(SMap,buildIndex+12,
			   Origin+Y*(length+innerThick),Y);
  ModelSupport::buildPlane(SMap,buildIndex+13,
			   Origin-X*(outWidth+innerThick),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,
			   Origin+X*(ringWidth+innerThick),X);
  ModelSupport::buildPlane(SMap,buildIndex+16,
			       Origin+Z*(height+innerThick),Z);


  // Lead

  ModelSupport::buildPlane(SMap,buildIndex+22,
			   Origin+Y*(length+innerThick+pbBackThick),Y);
  ModelSupport::buildPlane(SMap,buildIndex+23,
			   Origin-X*(outWidth+innerThick+pbWallThick),X);
  ModelSupport::buildPlane(SMap,buildIndex+24,
			   Origin+X*(ringWidth+innerThick+pbWallThick),X);
  ModelSupport::buildPlane(SMap,buildIndex+26,
			       Origin+Z*(height+innerThick+pbRoofThick),Z);
  
  const double steelThick(innerThick+outerThick);

  // OuterWall

  SurfMap::makePlane("outerBack",SMap,buildIndex+32,
		     Origin+Y*(length+steelThick+pbBackThick),Y);
  ModelSupport::buildPlane(SMap,buildIndex+33,
			   Origin-X*(outWidth+steelThick+pbWallThick),X);
  ModelSupport::buildPlane(SMap,buildIndex+34,
			   Origin+X*(ringWidth+steelThick+pbWallThick),X);
  SurfMap::makePlane("roof",SMap,buildIndex+36,
		     Origin+Z*(height+steelThick+pbRoofThick),Z);

  if (cornerLength>Geometry::zeroTol)
    {
      const Geometry::Quaternion Qxy=
	Geometry::Quaternion::calcQRotDeg(-cornerAngle,Z);
      const Geometry::Vec3D CX=Qxy.makeRotate(X);
      const Geometry::Vec3D CY=Qxy.makeRotate(Y);
      Geometry::Vec3D cornerPt(Origin+Y*cornerLength-X*outWidth);
      ModelSupport::buildPlane(SMap,buildIndex+302,cornerPt,CY);
      ModelSupport::buildPlane(SMap,buildIndex+303,cornerPt,-CX);
      cornerPt-=CX*innerThick;
      ModelSupport::buildPlane(SMap,buildIndex+313,cornerPt,-CX);
      cornerPt-=CX*pbWallThick;
      ModelSupport::buildPlane(SMap,buildIndex+323,cornerPt,-CX);
      cornerPt-=CX*steelThick;
      ModelSupport::buildPlane(SMap,buildIndex+333,cornerPt,-CX);
      cornerPt-=CX*outerOutVoid;
      if (outerOutVoid>Geometry::zeroTol)
	ModelSupport::buildPlane(SMap,buildIndex+1333,cornerPt,-CX);
    }
  
  int BI(buildIndex+1000);
  for(size_t i=0;i<holeRadius.size();i++)
    {
      const Geometry::Vec3D HPt(holeOffset[i].getInBasis(X,Y,Z));
      makeCylinder("exitHole",SMap,BI+7,Origin+HPt,Y,holeRadius[i]);
      BI+=100;
    }

  // extra for chicanes
  if (outerOutVoid>Geometry::zeroTol)
    {
      ModelSupport::buildPlane
	(SMap,buildIndex+1033,
	 Origin-X*(outWidth+steelThick+pbWallThick+outerOutVoid),X);

      ModelSupport::buildPlane
	(SMap,buildIndex+1034,
	 Origin+X*(ringWidth+steelThick+pbWallThick+outerOutVoid),X);      
    }

  if(!fZStep.empty())
    {
      ModelSupport::buildPlane
	(SMap,buildIndex+3001,Origin+Y*(forkYStep-forkLength/2),Y);
      ModelSupport::buildPlane
	(SMap,buildIndex+3002,Origin+Y*(forkYStep+forkLength/2),Y);
      int BI(buildIndex+3000);
      for(size_t i=0;i<fZStep.size();i++)
	{
	  ModelSupport::buildPlane
	    (SMap,BI+5,Origin+Z*(fZStep[i]-forkHeight/2.0),Z);
	  ModelSupport::buildPlane
	    (SMap,BI+6,Origin+Z*(fZStep[i]+forkHeight/2.0),Z);
	  BI+=10;
	}
    }

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

  HeadRule HR;

  const HeadRule sideWall=ExternalCut::getValidRule("SideWall",Origin);
  const HeadRule floor=ExternalCut::getRule("floor");
  const HeadRule frontWall=ExternalCut::getRule("frontWall");
  const HeadRule innerWall=
    (pbFrontThick>Geometry::zeroTol) ?
    ModelSupport::getHeadRule(SMap,buildIndex,"31") :
    ExternalCut::getRule("frontWall");

  HeadRule holeCut;
  int BI(buildIndex+1000);
  for(size_t i=0;i<holeRadius.size();i++)
    {
      holeCut*=HeadRule(SMap,BI,7);
      BI+=100;
    }

  HeadRule forkWall;
  if(!fZStep.empty())
    forkWall=ModelSupport::getHeadRule(SMap,buildIndex,"(-3001:3002)");
  
  if (innerOutVoid>Geometry::zeroTol)
    {
      HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"-2 1003 -1004 -6 -303");
      makeCell("Void",System,cellIndex++,voidMat,0.0,HR*floor*innerWall);

      HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"-2 3 -1003 -6 -303");
      makeCell("LeftWallVoid",System,cellIndex++,voidMat,0.0,
	       HR*floor*innerWall);
      
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"-2 -4 1004 -6 ");
      makeCell("RightWallVoid",System,cellIndex++,voidMat,0.0,
	       HR*floor*innerWall);
    }
  else
    {
      HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"-2 3 -4 -6 -303");
      makeCell("Void",System,cellIndex++,voidMat,0.0,HR*floor*innerWall);
    }


  // Inner Wall
  HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"-2 (-3:303) 13 -6 -313");
  makeCell("InnerLeftWall",System,cellIndex++,skinMat,0.0,HR*floor*innerWall);

  HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"-2 4 -14 -6");
  makeCell("InnerRingWall",System,cellIndex++,skinMat,0.0,
	   HR*floor*innerWall*forkWall);

  // alt
  HR=ModelSupport::getAltHeadRule
    (SMap,buildIndex,"2 -12 -313A 13B -14 -6");
  makeCell("InnerBackWall",System,cellIndex++,skinMat,0.0,
	   HR*floor*innerWall*holeCut);

  HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"-32 33 -333 -34 6 -16");
  makeCell("InnerRoof",System,cellIndex++,skinMat,0.0,HR*innerWall);

  // Lead
  HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"-12 (-13:313) 23 -6 -323");
  makeCell("LeadLeftWall",System,cellIndex++,pbMat,0.0,HR*floor*innerWall);

  HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"-12 14 -24 -6");
  makeCell("LeadRingWall",System,cellIndex++,pbMat,0.0,
	   HR*floor*innerWall*forkWall);
  
  HR=ModelSupport::getAltHeadRule
    (SMap,buildIndex,"12 -22 -24 -323A 23B -6");
  makeCell("LeadBackWall",System,cellIndex++,pbMat,0.0,
	   HR*floor*innerWall*holeCut);

  HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"-32 33 -333 -34 16 -26");
  makeCell("LeadRoof",System,cellIndex++,pbMat,0.0,HR*innerWall);

  // Outer Wall
  HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"-22 (-23:323) 33 -6 -333");
  makeCell("OuterLeftWall",System,cellIndex++,skinMat,0.0,HR*floor*innerWall);

  HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"-22 24 -34 -6");
  makeCell("OuterRingWall",System,cellIndex++,skinMat,0.0,
	   HR*floor*innerWall*forkWall);
  
  HR=ModelSupport::getAltHeadRule
    (SMap,buildIndex,"22 -32 -34 -333A 33B -6");
  makeCell("OuterBackWall",System,cellIndex++,skinMat,0.0,
	   HR*floor*innerWall*holeCut);

  HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"-32 33 -333 -34 26 -36");
  makeCell("OuterRoof",System,cellIndex++,skinMat,0.0,HR*innerWall);

  // front wall:

  if (pbFrontThick>Geometry::zeroTol)
    {
      HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"-11 33 -34 -36 7");
      makeCell("FrontWall",System,cellIndex++,skinMat,0.0,HR*frontWall*floor);
      HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"11 -21 33 -34 -36 7");
      makeCell("FrontWall",System,cellIndex++,pbMat,0.0,HR*floor);
      HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"21 -31 33 -34 -36 7");
      makeCell("FrontWall",System,cellIndex++,skinMat,0.0,HR*floor);
      HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"-31 -7");
      makeCell("EntranceHole",System,cellIndex++,voidMat,0.0,HR*frontWall);
    }

    // Outer void for pipe(s)
  BI=buildIndex+1000;
  for(size_t i=0;i<holeRadius.size();i++)
    {
      HR=ModelSupport::getSetHeadRule(SMap,buildIndex,BI,"2 -32 -7M");
      makeCell("ExitHole",System,cellIndex++,voidMat,0.0,HR);
      BI+=100;
    }
  
  //EXCLUDE
  if (outerOutVoid>Geometry::zeroTol)
    {
      HR=ModelSupport::getSetHeadRule
	(SMap,buildIndex,"-32 (-33:333) 1033 -36 -1333");
      makeCell("OuterLeftVoid",System,cellIndex++,voidMat,
	       0.0,HR*floor*frontWall);

      HR=ModelSupport::getHeadRule(SMap,buildIndex,"-32 34 -1034 -36");
      makeCell("OuterRightVoid",System,cellIndex++,voidMat,
	       0.0,HR*floor*frontWall);
      HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"-32 1033 -1034 -36 -1333");
    }
  else
    HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"-32 33 -34 -36 -333");

  addOuterSurf(HR*frontWall*floor);

  if(!fZStep.empty())
    {
      HeadRule cutHR;
      int BI(buildIndex+3000);
      for(size_t i=0;i<fZStep.size();i++)
	{
	  HR=ModelSupport::getHeadRule
	    (SMap,buildIndex,BI,"3001 -3002 5M -6M 4 -34");
	  makeCell("ForkHole",System,cellIndex++,voidMat,0.0,HR);
	  cutHR*=ModelSupport::getHeadRule(SMap,BI,"(-5:6)");	  
	  BI+=10;
	}
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"3001 -3002 -6 4 -14");
      makeCell("ForkInner",System,cellIndex++,skinMat,0.0,HR*floor*cutHR);
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"3001 -3002 -6 14 -24");
      makeCell("ForkLead",System,cellIndex++,pbMat,0.0,HR*floor*cutHR);
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"3001 -3002 -6 24 -34");
      makeCell("ForkOuter",System,cellIndex++,skinMat,0.0,HR*floor*cutHR);
    }
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

  const double extraWall(innerThick+outerThick+pbWallThick);

  ExternalCut::createLink("frontWall",*this,0,Origin,-Y);
  
  setConnect(1,Origin+Y*(length+extraWall),Y);
  setLinkSurf(1,SMap.realSurf(buildIndex+32));

  // outer lead wall
  setConnect(3,Origin-X*(extraWall+outWidth)+Y*(length/2.0),-X);
  setLinkSurf(3,-SMap.realSurf(buildIndex+33));
  nameSideIndex(3,"leftWall");
  // outer surf
  setConnect(4,Origin+X*(extraWall+ringWidth)+Y*(length/2.0),X);
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

  setConnect(14,Origin+X*ringWidth+Y*(length/2.0),-X);
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
      const std::string unitName(keyName+"Chicane"+NStr);
      std::shared_ptr<PortChicane> PItem=
	std::make_shared<PortChicane>(unitName);
      
      OR.addObject(PItem);
      const std::string wallName=
	Control.EvalDefVar<std::string>(unitName+"Wall","Left");
      if (wallName!="Left" && wallName!="Right")
	throw ColErr::InContainerError<std::string>(wallName,"Side not valid");
      
      // set surfaces:
      if (wallName=="Left")
	{
		  
	  PItem->addInsertCell("Inner",getCell("InnerLeftWall"));
	  PItem->addInsertCell("Inner",getCell("LeadLeftWall"));
	  PItem->addInsertCell("Inner",getCell("OuterLeftWall"));
	  PItem->addInsertCell("Main",getCell("LeftWallVoid"));
	  PItem->addInsertCell("Main",getCell("OuterLeftVoid",0));
	  PItem->setCutSurf("innerWall",*this,"innerLeftWall");
	  PItem->setCutSurf("outerWall",*this,"leftWall");      
	  PItem->createAll(System,*this,getSideIndex("leftWall"));	    
	}
      else if (wallName=="Right")
	{
	  PItem->addInsertCell("Inner",getCell("InnerRingWall"));
	  PItem->addInsertCell("Inner",getCell("LeadRingWall"));
	  PItem->addInsertCell("Inner",getCell("OuterRingWall"));
	  PItem->addInsertCell("Main",getCell("RightWallVoid"));
	  PItem->addInsertCell("Main",getCell("OuterRightVoid",0));
	  PItem->setCutSurf("innerWall",*this,"innerRightWall");
	  PItem->setCutSurf("outerWall",*this,"rightWall");
	  PItem->createAll(System,*this,getSideIndex("rightWall"));
	}
      PChicane.push_back(PItem);
      //      PItem->splitObject(System,23,getCell("WallVoid"));
      //      PItem->splitObject(System,24,getCell("SplitVoid"));      
    }
  return;
}

void
ExperimentalHutch::splitChicane(Simulation& System,const size_t indexA,
				const size_t indexB)
  /*!
    Split chicane
    \param System :: simulation
    \param indexA of chicane
    \param indexB of chicane
   */
{
  ELog::RegMethod RegA("ExperimentalHutch","splitChicane");

  if (indexA<indexB &&
      indexA<PChicane.size() &&
      indexB<PChicane.size())
    {
      const FuncDataBase& Control=System.getDataBase();
      const std::string NStr(std::to_string(indexA));
      const std::string unitName(keyName+"Chicane"+NStr);
      const std::string wallName=
	Control.EvalDefVar<std::string>(unitName+"Wall","Left");
      
      if (wallName=="Left")
	{
	  const Geometry::Vec3D APt=PChicane[indexA]->getLinkPt(4);
	  const Geometry::Vec3D BPt=PChicane[indexB]->getLinkPt(3);
	  const Geometry::Vec3D Axis=PChicane[indexA]->getLinkAxis(4);
	  const Geometry::Vec3D midPt=(APt+BPt)/2.0;
	  
	  this->splitObjectAbsolute(System,5001,getCell("LeftWallVoid"),
				    midPt,Axis);
	  for(const std::string& cellName : {
	      "InnerLeftWall","LeadLeftWall","OuterLeftWall","OuterLeftVoid" })
	    {
	      this->splitObject(System,buildIndex+5001,getCell(cellName));
	    }
	}
      else if (wallName=="Right")
	{
	  const Geometry::Vec3D APt=PChicane[indexA]->getLinkPt(4);
	  const Geometry::Vec3D BPt=PChicane[indexB]->getLinkPt(3);
	  const Geometry::Vec3D Axis=PChicane[indexA]->getLinkAxis(4);
	  const Geometry::Vec3D midPt=(APt+BPt)/2.0;
	  
	  this->splitObjectAbsolute(System,5001,getCell("RightWallVoid"),
				    midPt,Axis);
	  for(const std::string& cellName : {
	      "InnerRingWall","LeadRingWall","OuterRingWall","OuterRightVoid" })
	    {
	      this->splitObject(System,buildIndex+5001,getCell(cellName));
	    }
	}
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
