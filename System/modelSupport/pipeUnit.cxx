/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   process/pipeUnit.cxx
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
#include <cmath>
#include <complex> 
#include <vector>
#include <map> 
#include <list> 
#include <set>
#include <string>
#include <algorithm>
#include <functional>
#include <numeric>
#include <iterator>
#include <memory>
 
#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "stringCombine.h"
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
#include "ModelSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedUnit.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "LineTrack.h"
#include "pipeSupport.h"
#include "pipeUnit.h"

namespace ModelSupport
{

pipeUnit::pipeUnit(const std::string& Key,const size_t Index) : 
  attachSystem::FixedUnit(3,Key+std::to_string(Index)),
  attachSystem::ContainedComp(),
  attachSystem::CellMap(),
  nAngle(6),prev(0),next(0),
  activeFlag(511)
 /*!
   Constructor
   \param Key :: KeyName for full pipe
   \param Index :: Index that specialize the keyname
  */
{}

pipeUnit::pipeUnit(const pipeUnit& A) : 
  attachSystem::FixedUnit(A),attachSystem::ContainedComp(A),
  attachSystem::CellMap(A),
  nAngle(A.nAngle),
  prev(A.prev),next(A.next),APt(A.APt),BPt(A.BPt),Axis(A.Axis),
  ANorm(A.ANorm),BNorm(A.BNorm),ASurf(A.ASurf),BSurf(A.BSurf),
  activeFlag(A.activeFlag),cylVar(A.cylVar)
  /*!
    Copy constructor
    \param A :: pipeUnit to copy
  */
{}

pipeUnit&
pipeUnit::operator=(const pipeUnit& A)
  /*!
    Assignment operator
    \param A :: pipeUnit to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedComp::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::CellMap::operator=(A);
      nAngle=A.nAngle;
      APt=A.APt;
      BPt=A.BPt;
      Axis=A.Axis;
      ANorm=A.ANorm;
      BNorm=A.BNorm;
      ASurf=A.ASurf;
      BSurf=A.BSurf;
      activeFlag=A.activeFlag;
      cylVar=A.cylVar;
    }
  return *this;
}

pipeUnit::~pipeUnit()
  /*!
    Destructor
  */
{}

void
pipeUnit::setASurf(const HeadRule& AR)
  /*!
    Set the initial surface
    \param AR :: Initial surface rule
  */
{
  ASurf=AR;
  return;
}

void
pipeUnit::setBSurf(const HeadRule& BR)
  /*!
    Set the final surface
    \param BR :: Final surface rule
  */
{
  BSurf=BR;
  return;
}

void
pipeUnit::setPoints(const Geometry::Vec3D& A,
		    const Geometry::Vec3D& B)
  /*!
    Allocate the points
    \param A :: First Point
    \param B :: Second point
  */
{
  ELog::RegMethod RegA("pipeUnit","setPoints");
  
  Axis=(B-A).unit();
  APt=A;
  BPt=B;
  return;
}

void
pipeUnit::calcNorm(const int PN,
		   const Geometry::Vec3D& AAxis,
		   const Geometry::Vec3D& BAxis)
   /*!
     Calculate the surface normal value from Pt on another Axis
     \param PN :: Point direction [0==Base/1==top]
     \param AAxis :: First joining axis
     \param BAxis :: second Joining Axis
   */
{
  ELog::RegMethod RegA("pipeUnit","calcNorm");

  Geometry::Vec3D Norm=(BAxis+AAxis)/2.0;
  if (PN)
    BNorm=Norm;
  else    
    ANorm=Norm;
  return;
}
  
void 
pipeUnit::checkForward()
  /*!
    Sets the next item if necessary
   */
{
  if (next)
    next->connectFrom(this);
  return;
}

void
pipeUnit::populate(const size_t AF,const std::vector<cylValues>& CV)
  /*!
    Popuate everything assuming prev / next have been set
    along with APt and BPt 
    \param AF :: Active flag
    \param CV :: Cylindrical value
  */
{
  ELog::RegMethod RegA("pipeUnit","populate");

  activeFlag=AF;
  cylVar=CV;
  
  checkForward();
  const Geometry::Vec3D& pAxis((prev) ? prev->getAxis() : Axis);
  const Geometry::Vec3D& nAxis((next) ? -next->getAxis() : -Axis);
  calcNorm(0,Axis,pAxis);
  calcNorm(1,-Axis,nAxis);

  return;
}

const Geometry::Vec3D&
pipeUnit::getPt(const int side) const
  /*!
    Return a specific connection point
    \param side :: Side of object
    \return centre connection point
  */
{
  return (!side) ? APt : BPt;
}

void
pipeUnit::connectFrom(pipeUnit* Ptr) 
  /*!
    Set the previous connection
    \param Ptr :: Pointer to use
  */
{
  prev=Ptr;
  return;
}

void
pipeUnit::connectTo(pipeUnit* Ptr) 
  /*!
    Set the connection to the next object
    \param Ptr :: Pointer to use
  */
{
  next=Ptr;
  return;
}

double
pipeUnit::getOuterRadius() const
  /*!
    Determine the outer active cylinder
    \return Radius [outer]
   */
{
  ELog::RegMethod RegA("pipeUnit","getOuterRadius");

  if (cylVar.empty()) return 0.0;
  const size_t index=getOuterIndex();
  return cylVar[index].CRadius;
}

size_t
pipeUnit::getOuterIndex() const
  /*!
    Determine the outer index
    \return Index of outer unit 
  */
{
  ELog::RegMethod RegA("pipeUnit","getOuterIndex");

  if (cylVar.empty()) 
    throw ColErr::EmptyValue<int>("cylVar vector empty");

  size_t index=cylVar.size()-1;
  if (!activeFlag) return index;
  size_t bitIndex=1 << (index+1);
  do
    {
      bitIndex>>=1;
    } while( !(bitIndex & activeFlag) && --index>0);

  return index;
}

void
pipeUnit::createSurfaces()
  /*!
    Create Surface components
  */
{
  ELog::RegMethod RegA("pipeUnit","createSurface");

  if (!ASurf.hasRule())
    {
      ModelSupport::buildPlane(SMap,buildIndex+5,APt,ANorm);
      ASurf=HeadRule(StrFunc::makeString(SMap.realSurf(buildIndex+5)));
    }
  if (!BSurf.hasRule())
    {
      ModelSupport::buildPlane(SMap,buildIndex+6,BPt,BNorm);
      BSurf=HeadRule(StrFunc::makeString(SMap.realSurf(buildIndex+6)));
    }

  // Create cylinders
  
  size_t bitIndex(1);    // Index for flag bit [Which to build]
  for(size_t i=0;i<cylVar.size();i++)
    {
      if (!activeFlag || (activeFlag & bitIndex))
	ModelSupport::buildCylinder(SMap,buildIndex+7+static_cast<int>(i)*10,
				    APt,Axis,cylVar[i].CRadius);
      bitIndex<<=1;
    }
   
  return;
}

HeadRule
pipeUnit::createCaps() const
  /*!
    Creates the caps with the appropiate surfaces
    \return caps string [inward pointing]
  */
{
  ELog::RegMethod RegA("pipeUnit","createCaps");

  return ASurf*BSurf;
}
  

const HeadRule&
pipeUnit::getCap(const int side) const
  /*!
    Get the end cap head rule
    \param side :: side value (true => end)
    \return cap rule
   */
{
  return (side) ? BSurf : ASurf;
}

  
void
pipeUnit::createOuterObject()
  /*!
    Construct the object and the outside bracket
    for the system
  */
{
  ELog::RegMethod RegA("pipeUnit","createOuterObject");

  const size_t outerIndex=getOuterIndex();

  const int SI(buildIndex+static_cast<int>(outerIndex)*10);
  HeadRule HR=createCaps();
  HR*=ModelSupport::getHeadRule(SMap,SI,"-7");
  addOuterSurf(HR);
  return;
}

void
pipeUnit::createObjects(Simulation& System)
  /*!
    Construct the object and the outside bracket
    for the system
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("pipeUnit","createObjects");

  HeadRule HR;

  int SI(buildIndex);
  int SIprev(0);
  const HeadRule Cap=createCaps();
  size_t bitIndex(1);
  for(size_t i=0;i<cylVar.size();i++)
    {
      if (!activeFlag || (activeFlag & bitIndex))
	{
	  const std::string lName="Layer"+std::to_string(i);
	  HR=Cap*ModelSupport::getHeadRule(SMap,SI,"-7");
	  if (SIprev)
	    HR*=ModelSupport::getHeadRule(SMap,SIprev,"7");
	  makeCell(lName,System,cellIndex++,
		   cylVar[i].MatN,cylVar[i].Temp,HR);
	  SIprev=SI;
	}      
      bitIndex<<=1;
      SI+=10;
    }
  

  return;
}

void
pipeUnit::addInsertSet(const std::set<int>& S)
  /*!
    Insert the extra cell set into cellCut set
    \param S :: Set of cells to insert
  */
{
  cellCut.insert(S.begin(),S.end());
  return;
}

void
pipeUnit::clearInsertSet()
  /*!
    Clear the insert set
   */
{
  cellCut.clear();
  return;
}


void
pipeUnit::insertObjects(Simulation& System) 
  /*!
    Insert the objects into the main simulation. It is separated
    from creation since we need to determine those object that 
    need to have an exclude item added to them.
    \param System :: Simulation to add object to
  */
{
  ELog::RegMethod RegA("pipeUnit","insertObjects");

  System.populateCells();
  System.validateObjSurfMap();

  typedef std::map<int,MonteCarlo::Object*> MTYPE;
  MTYPE OMap;         // Set on index

  // calculate 3 orthonormal vectors including the primary axis
  Geometry::Vec3D Axis(BPt-APt);
  Axis.makeUnit();
  const Geometry::Vec3D AX(Axis.crossNormal());
  const Geometry::Vec3D AY(AX*Axis);
  double radius=getOuterRadius()+0.001;
  if (prev && std::abs(prev->getOuterRadius()-radius)<10.0*Geometry::zeroTol)
    radius+=Geometry::zeroTol*100;  

  const double angleStep(2*M_PI/static_cast<double>(nAngle));
  double angle(0.0);
  Geometry::Vec3D addVec(0,0,0);
  //  addVec+=Axis*0.001;
  System.populateCells();
  for(size_t i=0;i<=nAngle;angle+=angleStep,i++)
    {
      calcLineTrack(System,APt+addVec,BPt+addVec,OMap);
      // set for next angle
      addVec=AX*(cos(angle)*radius)+AY*(sin(angle)*radius);
    }

  // update 
  
  // add extra cells from insert forced list [cellCut]
  for(const int forceCellN : cellCut)
    {
      if (OMap.find(forceCellN)==OMap.end())
	{
	  MonteCarlo::Object* SObj=System.findObject(forceCellN);
	  if (SObj)
	    OMap.insert(MTYPE::value_type(forceCellN,SObj));
	}
    }
  
  excludeUnit(System,OMap);
  
  return;
}

void
pipeUnit::excludeUnit(Simulation& System,
		      const std::map<int,MonteCarlo::Object*>& OMap) const
  /*!
    Adds the exclude strings to the objects
    \param OMap :: Object Map to add
   */
{
  ELog::RegMethod RegA("pipeUnit","excludeUnit");

  // Add exclude string
  for(const auto& [CN , OPtr] : OMap)
    {
      if (CN!=cellIndex-1)
	{
	  OPtr->addIntersection(getOuterSurf().complement());
	  System.minimizeObject(CN);
	}
    }

  return;
}


void 
pipeUnit::buildUnit(Simulation& System,
		    const size_t AF,
		    const std::vector<cylValues>& CV)
/*!
    Build a pipe unit and insert it into the model
    \param System :: Simulation to 
    \param AF :: activeFlag
    \param CV :: Values for each layer
   */
{
  ELog::RegMethod RegA("pipeUnit","buildUnit");

  populate(AF,CV);
  createSurfaces();
  createOuterObject();
  insertObjects(System);
  createObjects(System);
  
  return;
}

} // NAMESPACE ModelSupport

