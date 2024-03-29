/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   ralBuild/BlockShutter.cxx
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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
#include <algorithm>
#include <cmath>
#include <complex>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <list>
#include <map>
#include <memory>
#include <numeric>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
#include "shutterBlock.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedUnit.h"
#include "FixedGroup.h"
#include "FixedRotate.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ExternalCut.h"
#include "ContainedComp.h"
#include "GeneralShutter.h"
#include "BlockShutter.h"
#include "collInsert.h"

namespace shutterSystem
{

BlockShutter::BlockShutter(const size_t ID,
			   const std::string& K,
			   std::string  ZK) :
  GeneralShutter(ID,K),
  b4cMat(47),
  blockKey(std::move(ZK)),
  collPtr(new collInsert(blockKey+"Insert"))
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param ID :: Index number of shutter
    \param K :: Key name
    \param ZK :: zoom Key name
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  OR.addObject(collPtr);
}

BlockShutter::~BlockShutter() 
  /*!
    Destructor
  */
{}

void
BlockShutter::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Simulation to use
  */
{
  ELog::RegMethod RegA("BlockShutter","populate");

  GeneralShutter::populate(Control);

  // Modification to the general shutter populated variables:

  colletHGap=Control.EvalVar<double>(blockKey+"ColletHGap");
  colletVGap=Control.EvalVar<double>(blockKey+"ColletVGap");
  colletFGap=Control.EvalVar<double>(blockKey+"ColletFGap");
  colletMat=ModelSupport::EvalMat<int>(Control,blockKey+"ColletMat");

  return;
}

void
BlockShutter::createSurfaces()
  /*!
    Create all the surfaces that are normally created 
  */
{
  ELog::RegMethod RegA("BlockShutter","createSurfaces");

  // Inner cut [on flightline]
  ModelSupport::buildPlane
    (SMap,buildIndex+325,
     frontPt+Z*(-colletVGap+voidHeightInner/2.0+centZOffset),zSlope);
  
  // Inner cut [on flightline]
  ModelSupport::buildPlane
    (SMap,buildIndex+326,
     frontPt-Z*(-colletVGap+voidHeightInner/2.0+centZOffset),zSlope);

  // Outer cut [on flightline]
  ModelSupport::buildPlane
    (SMap,buildIndex+425,
     frontPt+Z*(-colletVGap+voidHeightOuter/2.0+centZOffset),zSlope);
  
  // Outer cut [on flightline]
  ModelSupport::buildPlane(SMap,buildIndex+426,
      frontPt-Z*(-colletVGap+voidHeightOuter/2.0-centZOffset),zSlope);

  // HORRIZONTAL
  ModelSupport::buildPlane(SMap,buildIndex+313,
	  Origin-X*(-colletHGap+voidWidthInner/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+314,
		   Origin+X*(-colletHGap+voidWidthInner/2.0),X);

  ModelSupport::buildPlane(SMap,buildIndex+413,
         Origin-X*(-colletHGap+voidWidthOuter/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+414,
	    Origin+X*(-colletHGap+voidWidthOuter/2.0),X);
  
  // Forward gap
  ModelSupport::buildPlane(SMap,buildIndex+401,
	   frontPt+Y*(voidDivide+colletFGap),Y);

  return;
}  

void
BlockShutter::createObjects(Simulation& System)
  /*!
    Construction of the main shutter
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("BlockShutter","constructObjects");

  const HeadRule& RInnerComp=ExternalCut::getComplementRule("RInner");
  const HeadRule& ROuterHR=ExternalCut::getRule("ROuter");

  // Flightline
  HeadRule HR;

  if (voidDivide>0.0)
    {
      // exclude from flight line
      MonteCarlo::Object* VObjA=CellMap::getCellObject(System,"InnerVoid");
      MonteCarlo::Object* VObjB=CellMap::getCellObject(System,"OuterVoid");

      HR=ModelSupport::getHeadRule(SMap,buildIndex,"(-313:314:325:-326)");
      VObjA->addIntersection(HR);
      
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"(-413:414:425:-426)");
      VObjB->addIntersection(HR);
      

      HR=ModelSupport::getHeadRule
	(SMap,buildIndex,"200 313 -314 -325 326 -401");
      makeCell("InnerCollet",System,cellIndex++,0,0.0,HR*RInnerComp);
      // OuterCollet

      HR=ModelSupport::getHeadRule
	(SMap,buildIndex,"413 -414 -425 426 401");
      makeCell("OuterCollet",System,cellIndex++,colletMat,0.0,HR*ROuterHR);
      // SPACER:
      HR=ModelSupport::getHeadRule
	(SMap,buildIndex,"413 -414 -425 426 100 -401 (-313:314:325:-326)");
      makeCell("Spacer",System,cellIndex++,0,0.0,HR);
    }
  return;
}


void
BlockShutter::createInsert(Simulation& System)
  /*!
    Create the insert
   */
{
  ELog::RegMethod RegA("BlockShutter","createInsert");

  const HeadRule& RInnerComp=ExternalCut::getComplementRule("RInner");
  const HeadRule& ROuterHR=ExternalCut::getRule("ROuter");

  collPtr->copyCutSurf("RInner",*this,"RInner");
  collPtr->copyCutSurf("ROuter",*this,"ROuter");
  collPtr->copyCutSurf("Divider",*this,"Divider");

  collPtr->createAll(System,this->getSecondary(),0);
  collPtr->insertInCell(System,getCell("InnerCollet"));
  collPtr->insertInCell(System,getCell("OuterCollet"));

  return;
}

void
BlockShutter::createAll(Simulation& System,
			const attachSystem::FixedComp& FC,
			const long int sideIndex)
  /*!
    Create the shutter (only creates the volume (with clearance gaps)
    the actual beam path is added later. 
    \param System :: Simulation to process
    \param FC :: Fixed pointer for shutter origin [void centre]
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("BlockShutter","createAll");

  const FuncDataBase& Control=System.getDataBase();

  populate(Control);

  collPtr->populate(Control);

  this->GeneralShutter::setZOffset(collPtr->calcDrop(innerRadius));
  GeneralShutter::createAll(System,FC,sideIndex);

  createSurfaces();
  createObjects(System);  
  createInsert(System);
  return;
}
  
}  // NAMESPACE shutterSystem
