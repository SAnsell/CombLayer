/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   bunker/BunkerFeed.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "SimProcess.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "boxValues.h"
#include "boxUnit.h"
#include "BoxLine.h"
#include "SurInter.h"
#include "Bunker.h"
#include "BunkerFeed.h"

#include "debugMethod.h"

namespace essSystem
{

BunkerFeed::BunkerFeed(const std::string& Key,
                       const size_t Index)  :
  attachSystem::FixedComp(Key+StrFunc::makeString(Index),2),
  ID(Index),baseName(Key),
  pipeIndex(ModelSupport::objectRegister::Instance().cell(keyName)),
  cellIndex(pipeIndex+1),
  voidTrack(new ModelSupport::BoxLine(keyName))
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

BunkerFeed::BunkerFeed(const BunkerFeed& A) : 
  attachSystem::FixedComp(A),attachSystem::CellMap(A),
  ID(A.ID),baseName(A.baseName),pipeIndex(A.pipeIndex),
  cellIndex(A.cellIndex),voidTrack(new ModelSupport::BoxLine(*A.voidTrack)),
  height(A.height),width(A.width),Offset(A.Offset),
  CPts(A.CPts)
  /*!
    Copy constructor
    \param A :: BunkerFeed to copy
  */
{}

BunkerFeed&
BunkerFeed::operator=(const BunkerFeed& A)
  /*!
    Assignment operator
    \param A :: BunkerFeed to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedComp::operator=(A);
      attachSystem::CellMap::operator=(A);
      cellIndex=A.cellIndex;
      *voidTrack = *A.voidTrack;
      height=A.height;
      width=A.width;
      Offset=A.Offset;
      CPts=A.CPts;
    }
  return *this;
}

  
BunkerFeed::~BunkerFeed() 
  /*!
    Destructor
  */
{
  delete voidTrack;
}
  
void
BunkerFeed::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase for variables
  */
{
  ELog::RegMethod RegA("BunkerFeed","populate");
  
  Offset=Control.EvalPair<Geometry::Vec3D>(keyName+"Offset",
                                           baseName+"Offset");  
  height=Control.EvalPair<double>(keyName+"Height",baseName+"Height"); 
  width=Control.EvalPair<double>(keyName+"Width",baseName+"Width"); 

  if (Control.hasVariable(keyName+"Track0"))
    CPts=SimProcess::getVarVec<Geometry::Vec3D>(Control,keyName+"Track");
  else 
    CPts=SimProcess::getVarVec<Geometry::Vec3D>(Control,baseName+"Track");

  if (CPts.empty())
    ColErr::EmptyContainer("CPTs::TrackPts");

  return;
}
  
void
BunkerFeed::createUnitVector(const Bunker& BUnit,
                             const size_t segIndex)
  /*!
    Create the unit vectors
    \param BUnit :: Bunker unit
    \param segIndex :: Segment number
  */
{
  ELog::RegMethod RegA("BunkerFeed","createUnitVector");

  Geometry::Vec3D DPos,DX,DY,DZ;
  BUnit.calcSegPosition(segIndex,DPos,DX,DY,DZ);
  attachSystem::FixedComp::createUnitVector(DPos,DX,DY,DZ);
  return;
}

void
BunkerFeed::moveToLayer(const Bunker& BUnit,
                        const std::string& PosName)
  /*!
    Move the Origin to the layer name
    \param BUnit :: Bunker unit
    \param PosName :: Position name
   */
{
  ELog::RegMethod RegA("BunkerFeed","moveToLayer");
  if (PosName=="Floor")
    {
      const int SN=BUnit.getSurf("floorInner");
      const Geometry::Surface* SPtr=
        SMap.realSurfPtr(SN);
      Origin=SurInter::getLinePoint(Origin,Z,SPtr,Origin);
    }
  return;
}
  

void 
BunkerFeed::insertColl(Simulation& System)
  /*!
    Add a feed pipe to the collimator area
    \param System :: Simulation to add pipe to
  */
{
  ELog::RegMethod RegA("BunkerFeed","insertColl");
  ELog::debugMethod DegA;

  Geometry::Vec3D LineOrg=Origin+
    X*Offset[0]+Y*Offset[1]+Z*Offset[2];

  //  const Geometry::Vec3D OP=B.calcSideIntercept(3,LineOrg,X);
  //  const Geometry::Vec3D XDir=Hut.calcSurfNormal(3);
  //  const Geometry::Vec3D YDir=XDir*Z;
  //  vodTrack->addPoint(Offset-XDir);

  for(const Geometry::Vec3D& trackPt : CPts)
    {
      Geometry::Vec3D realPoint=
	X*trackPt[0]+Y*trackPt[1]+Z*trackPt[2];
      realPoint+=Origin;
      voidTrack->addPoint(realPoint);
      ELog::EM<<"Adding Point == "<<realPoint<<ELog::endDiag;
    }
        
  // make void
  ELog::EM<<"WH == "<<width<<" "<<height<<ELog::endDiag;
  voidTrack->addSection(width,height,0,0.0);
  voidTrack->setInitZAxis(Z);
  DegA.activate();
  voidTrack->createAll(System);

  return;
}
  
void
BunkerFeed::createAll(Simulation& System,
                      const Bunker& bunkerObj,
                      const size_t segNumber,
                      const std::string& feedName)
  /*!
    Generic function to create everything
    \param System :: Simulation to create objects in
    \param bunkerObj :: Bunker Object
    \param segNumber :: segment number
    \param feedName :: Feed direction name
  */
{
  ELog::RegMethod RegA("BunkerFeed","createAll");
  
  populate(System.getDataBase());
  createUnitVector(bunkerObj,segNumber);
  moveToLayer(bunkerObj,feedName);
  insertColl(System); 

  //  insertPipes(System);       
  
  return;
}
  
}  // NAMESPACE essSystem
