/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/BunkerInsert.cxx
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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
#include "SurInter.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
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
#include "BaseMap.h"
#include "CellMap.h"
#include "ExternalCut.h"
#include "BunkerInsert.h"

namespace essSystem
{

BunkerInsert::BunkerInsert(const std::string& Key)  :
  attachSystem::FixedOffset(Key,15),
  attachSystem::ContainedComp(),
  attachSystem::CellMap(),
  attachSystem::ExternalCut()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

BunkerInsert::BunkerInsert(const BunkerInsert& A) : 
  attachSystem::FixedOffset(A),
  attachSystem::ContainedComp(A),
  attachSystem::CellMap(A),
  attachSystem::ExternalCut(A),
  backStep(A.backStep),
  height(A.height),width(A.width),topWall(A.topWall),
  lowWall(A.lowWall),leftWall(A.leftWall),rightWall(A.rightWall),
  wallMat(A.wallMat),voidMat(A.voidMat)
  /*!
    Copy constructor
    \param A :: BunkerInsert to copy
  */
{}

BunkerInsert&
BunkerInsert::operator=(const BunkerInsert& A)
  /*!
    Assignment operator
    \param A :: BunkerInsert to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedOffset::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::ExternalCut::operator=(A);
      backStep=A.backStep;
      height=A.height;
      width=A.width;
      topWall=A.topWall;
      lowWall=A.lowWall;
      leftWall=A.leftWall;
      rightWall=A.rightWall;
      wallMat=A.wallMat;
      voidMat=A.voidMat;
    }
  return *this;
}

BunkerInsert::~BunkerInsert() 
  /*!
    Destructor
  */
{}

void
BunkerInsert::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("BunkerInsert","populate");
  FixedOffset::populate(Control);

  backStep=Control.EvalDefVar<double>(keyName+"BackStep",300.0);
    
  height=Control.EvalVar<double>(keyName+"Height");
  width=Control.EvalVar<double>(keyName+"Width");
  topWall=Control.EvalVar<double>(keyName+"TopWall");
  lowWall=Control.EvalVar<double>(keyName+"LowWall");
  leftWall=Control.EvalVar<double>(keyName+"LeftWall");
  rightWall=Control.EvalVar<double>(keyName+"RightWall");

  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  voidMat=ModelSupport::EvalDefMat(Control,keyName+"VoidMat",0);
  
  return;
}
  
  
void
BunkerInsert::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("BunkerInsert","createSurface");

  /// Dividing plane
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*backStep,Y);
    
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*width/2.0,X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*width/2.0,X);

  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*height/2.0,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*height/2.0,Z);

  ModelSupport::buildPlane(SMap,buildIndex+13,Origin-X*(width/2.0+leftWall),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,Origin+X*(width/2.0+rightWall),X);

  ModelSupport::buildPlane(SMap,buildIndex+15,Origin-Z*(height/2.0+lowWall),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*(height/2.0+topWall),Z);

  return;
}
  
  
void
BunkerInsert::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("BunkerInsert","createObjects");

  const HeadRule fbHR=getRule("front")*getRule("back");
  HeadRule HR;

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"3 -4 5 -6");
  makeCell("Void",System,cellIndex++,voidMat,0.0,HR*fbHR);
  setCell("Void",cellIndex-1);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,
				 "13 -14 15 -16 (-3 : 4: -5: 6)");
  System.addCell(MonteCarlo::Object(cellIndex++,wallMat,0.0,HR*fbHR));
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 13 -14 15 -16");
  addOuterSurf(HR);

  // Create cut unit:
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"13 -14 15 -16");

  

  return;
}
  
void
BunkerInsert::createLinks()
  /*!
    Create all the linkes [OutGoing]
  */
{
  ELog::RegMethod RegA("BunkerInsert","createLinks");

  
  ExternalCut::createLink("front",*this,0,Origin,-Y);
  ExternalCut::createLink("back",*this,1,Origin,Y);
  // Calc bunker edge intersectoin
  const std::vector<Geometry::Vec3D> endMidPt
    ({
      this->getLinkPt("front"),
      this->getLinkPt("back")
    });

  // need to use midPt here because we have both
  // front/back normally and Origin is often outside of the
  // object.
  const Geometry::Vec3D midPt((endMidPt[0]+endMidPt[1])/2.0);
  // Mid point [useful for guides etc]
  FixedComp::setConnect(6,midPt,Y);

  FixedComp::setConnect(2,midPt-X*(width/2.0),X);
  FixedComp::setConnect(3,midPt+X*(width/2.0),X);
  FixedComp::setConnect(4,midPt-Z*(height/2.0),Z);
  FixedComp::setConnect(5,midPt+Z*(height/2.0),Z);
  
  FixedComp::setLinkSurf(2,SMap.realSurf(buildIndex+3));
  FixedComp::setLinkSurf(3,-SMap.realSurf(buildIndex+4));
  FixedComp::setLinkSurf(4,SMap.realSurf(buildIndex+5));
  FixedComp::setLinkSurf(5,-SMap.realSurf(buildIndex+6));
  
  // add endpoint [not mid line]

  size_t index(5);
  for(const Geometry::Vec3D& EP : endMidPt)
    {
      FixedComp::setConnect(index+2,EP-X*(width/2.0),X);
      FixedComp::setConnect(index+3,EP+X*(width/2.0),X);
      FixedComp::setConnect(index+4,EP-Z*(height/2.0),Z);
      FixedComp::setConnect(index+5,EP+Z*(height/2.0),Z);
      
      FixedComp::setLinkSurf(index+2,SMap.realSurf(buildIndex+3));
      FixedComp::setLinkSurf(index+3,-SMap.realSurf(buildIndex+4));
      FixedComp::setLinkSurf(index+4,SMap.realSurf(buildIndex+5));
      FixedComp::setLinkSurf(index+5,-SMap.realSurf(buildIndex+6));
      index+=4;
    }

  
  return;
}


void
BunkerInsert::createAll(Simulation& System,
			const attachSystem::FixedComp& FC,
			const long int orgIndex)

/*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param orgIndex :: link for origin
    \param bunkerObj :: Bunker wall object
  */
{
  ELog::RegMethod RegA("BunkerInsert","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,orgIndex);
  createSurfaces();

  // Walls : [put]
  createObjects(System);
  createLinks();
  
  insertObjects(System);              

  return;
}

}  // NAMESPACE essSystem
