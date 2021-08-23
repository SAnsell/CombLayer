/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachComp/BlockZone.cxx
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
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <cmath>
#include <complex>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <deque>
#include <string>
#include <algorithm>
#include <iterator>
#include <memory>
#include <functional>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "BaseMap.h"
#include "CellMap.h"

#include "BlockZone.h"

namespace attachSystem
{

std::ostream&
operator<<(std::ostream& OX,const BlockZone& A)
  /*!
    Debug write out function
    \param OX :: Output stream
    \param A :: Inner zone to write
    return Stream after use.
  */
{
  A.write(OX);
  return OX;
}

BlockZone::BlockZone() :
  attachSystem::FixedComp(2,"BZtemp"),
  attachSystem::CellMap(),
  voidMat(0)
  /*!
    Simple constructor
  */
{}
  
BlockZone::BlockZone(const std::string& key) :
  attachSystem::FixedComp(key,6),
  attachSystem::CellMap(),
  voidMat(0)
  /*!
    Simple constructor
  */
{}


BlockZone::BlockZone(const BlockZone& A) : 
  attachSystem::FixedComp(A),attachSystem::CellMap(A),
  surroundHR(A.surroundHR),frontHR(A.frontHR),backHR(A.backHR),
  maxExtentHR(A.maxExtentHR),voidMat(A.voidMat),
  insertCells(A.insertCells)
  /*!
    Copy constructor
    \param A :: BlockZone to copy
  */
{}

BlockZone&
BlockZone::operator=(const BlockZone& A)
  /*!
    Assignment operator
    \param A :: BlockZone to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedComp::operator=(A);
      attachSystem::CellMap::operator=(A);
      surroundHR=A.surroundHR;
      frontHR=A.frontHR;
      backHR=A.backHR;
      maxExtentHR=A.maxExtentHR;
      voidMat=A.voidMat;
      insertCells=A.insertCells;
    }
  return *this;
}

int
BlockZone::merge(const BlockZone& BZ)
  /*!
    Merge two Block zones if BOTH have common
    surround and if both have reversed front+back
    \retval 1 if normal direction merge   [ this -- BZ]
    \retval -1 if reversed direction merge [BZ -- this ]
    \retval 0 no merge
   */
{
  ELog::RegMethod RegA("BlockZone","merge");

  if(surroundHR==BZ.surroundHR)
    {
      if (backHR==BZ.frontHR)
	{
	  backHR=BZ.backHR;
	  return 1;
	}
      if (BZ.backHR==frontHR)
	{
	  frontHR=BZ.frontHR;
	  return -1;
	}
    }
  return 0;
}
  
HeadRule
BlockZone::getVolume() const
  /*!
    Get the full excluded volume
   */
{
  HeadRule Out(surroundHR);
  Out*=frontHR;
  Out*=backHR.complement();
  return Out;
}
  

void
BlockZone::setSurround(const HeadRule& HR)
  /*!
    Set the capture/surround rule
    \param HR :: Surround rule [inward]
  */
{
  surroundHR=HR;
  return;
}
  
void
BlockZone::setFront(const HeadRule& HR)
  /*!
    Set the front facing rule AND the back rule
    \param HR :: Rule at front [inward]
  */
{
  frontHR=HR;
  backHR=HR;
  return;
}

void
BlockZone::initFront(const HeadRule& HR)
  /*!
    Set the front facing rule AND the back rule
    \param HR :: Rule at front [inward]
  */
{
  frontHR=HR;
  maxExtentHR=HR;
  backHR=HR;
  return;
}

void
BlockZone::setBack(const HeadRule& HR)
  /*!
    Set the back rule [for delayed insert]
    \param HR :: Rule at back [outward]
  */
{
  backHR=HR;
  return;
}

void
BlockZone::setMaxExtent(const HeadRule& HR)
  /*!
    Set the maximum outward rule
    \param HR :: Rule at front [inward]
  */
{
  maxExtentHR=HR;
  return;
}

void
BlockZone::addInsertCell(const int CN)
  /*!
    Insert the cell [Check needed for uniqueness?]
    \param CN :: Cell number
   */
{
  insertCells.push_back(CN);
  return;
}

void
BlockZone::addInsertCells(const std::vector<int>& CVec)
  /*!
    Insert the cell [Check needed for uniqueness?]
    \param CN :: Cell number
   */
{
  for(const int CN : CVec)
    insertCells.push_back(CN);
  return;
}
		 
void
BlockZone::insertCell(Simulation& System,const HeadRule& HR)
  /*!
    Insert the new volume into the cell(s) : 
    \todo Check HR fits in 
    \param System :: simulation for cells
    \param HR :: Volume to be exclude [not complemented yet]
   */
{
  ELog::RegMethod RegA("BlockZone","insertCell");

  
  if (HR.hasRule())
    {
      for(const int cellN : insertCells)
	{
	  MonteCarlo::Object* outerObj=System.findObject(cellN);
	  if (!outerObj)
	    throw ColErr::InContainerError<int>
	      (cellN,"Cell not in Simulation");
	  
	  if (insertHR.find(cellN)==insertHR.end())
	    {
	      insertHR.emplace(cellN,outerObj->getHeadRule());
	    }
	  outerObj->addIntersection(HR.complement());
	}
    }
  return;
}

void
BlockZone::rebuildInsertCells(Simulation& System)
  /*!
    Rebuild the cell to deal with end point
   */
{
  ELog::RegMethod RegA("BlockZone","rebuildInsertCells");

  HeadRule ItemHR=frontHR*backHR.complement()*surroundHR;
  ItemHR.makeComplement();

  for(const auto& [cellN,HR] : insertHR)
    {
      MonteCarlo::Object* outerObj=System.findObject(cellN);
      if (!outerObj)
	throw ColErr::InContainerError<int>
	  (cellN,"Cell not in Simulation");

      outerObj->procHeadRule(HR*ItemHR);
      outerObj->populate();
    }
  return;
  
}
  

int
BlockZone::createUnit(Simulation& System)
  /*!
    Construct outer void object main pipe
    \param System :: Simulation
    \return cell nubmer
  */
{
  ELog::RegMethod RegA("BlockZone","createUnit(System)");

  // alway outgoing [so use complement]

  // maxEXTENT is INWARDS FACING
  const HeadRule newBackFC=maxExtentHR;
  HeadRule Volume=surroundHR * backHR * newBackFC;
  makeCell("Unit",System,cellIndex++,voidMat,0.0,Volume);

  backHR=newBackFC.complement();
  insertCell(System,Volume);

  return cellIndex-1;
}

int
BlockZone::createUnit(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const std::string& linkName)
  /*!
    Construct outer void object main pipe
    \param System :: Simulation
    \param FC :: FixedComp
    \param linkName :: link point
    \return cell nubmer
  */
{
  return createUnit(System,FC,FC.getSideIndex(linkName));
}
  
int
BlockZone::createUnit(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int sideIndex)
  /*!
    Construct outer void object main pipe
    \param System :: Simulation
    \param FC :: FixedComp
    \param sideIndex :: link point
    \return cell nubmer
  */
{
  ELog::RegMethod RegA("BlockZone","createUnit");

  // alway outgoing [so use complement]
  const HeadRule newBackFC=FC.getFullRule(sideIndex);
  HeadRule Volume=surroundHR * backHR * newBackFC.complement();
  makeCell("Unit",System,cellIndex++,voidMat,0.0,Volume);
  backHR=newBackFC;
  
  insertCell(System,Volume);

  return cellIndex-1;
}

void
BlockZone::createLinks(const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    construct front/back points
   */
{
  ELog::RegMethod RegA("Segment27","createLinks");

  createUnitVector(FC,sideIndex);
  frontHR.populateSurf();
  backHR.populateSurf();

  const Geometry::Vec3D APt=frontHR.trackPoint(Origin,Y);
  const Geometry::Vec3D BPt=backHR.trackPoint(Origin,Y);

  FixedComp::setLinkSurf(0,frontHR.complement());
  FixedComp::setLinkSurf(1,backHR.complement());

  FixedComp::setConnect(0,APt,-Y);
  FixedComp::setConnect(1,BPt,Y);

  return;
}

void
BlockZone::write(std::ostream& OX) const
  /*!
    Debug write out function
    \param OX :: Output stream
  */
{
  OX<<"-------------"<<std::endl;
  OX<<"Front:"<<frontHR<<"\n";
  OX<<"Back :"<<backHR<<"\n";
  OX<<"Surround:"<<surroundHR<<"\n";
  OX<<"-------------"<<std::endl;
  return;
}

void
BlockZone::createAll(Simulation&,
		     const attachSystem::FixedComp& FC,
		     const long int sideIndex)
  /*!
    Simplified create all. Is an origin necessary ??
    \param System :: Simulation
    \param FC :: FixedComp
    \param sideIndex :: link point
  */
{
  createUnitVector(FC,sideIndex);
  return;
}
  
  
}  // NAMESPACE attachSystem
