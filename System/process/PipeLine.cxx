/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   process/PipeLine.cxx
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
#include <numeric>
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
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "surfDIter.h"
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
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "pipeUnit.h"
#include "PipeLine.h"

#include "debugMethod.h"

namespace ModelSupport
{

PipeLine::PipeLine(const std::string& Key)  :
  keyName(Key),nCylinder(0),nAngle(6)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

PipeLine::PipeLine(const PipeLine& A) : 
  keyName(A.keyName),nCylinder(A.nCylinder),CV(A.CV),
  Pts(A.Pts),layerSurf(A.layerSurf),commonSurf(A.commonSurf),
  activeFlags(A.activeFlags),nAngle(A.nAngle),
  startSurf(A.startSurf)
  /*!
    Copy constructor
    \param A :: PipeLine to copy
  */
{
}

PipeLine&
PipeLine::operator=(const PipeLine& A)
  /*!
    Assignment operator
    \param A :: PipeLine to copy
    \return *this
  */
{
  if (this!=&A)
    {
      nCylinder=A.nCylinder;
      CV=A.CV;
      Pts=A.Pts;
      layerSurf=A.layerSurf;
      commonSurf=A.commonSurf;
      activeFlags=A.activeFlags;
      nAngle=A.nAngle;
      startSurf=A.startSurf;
      clearPUnits();
    }
  return *this;
}

PipeLine::~PipeLine() 
 /*!
   Destructor
 */
{
  clearPUnits();
}

void
PipeLine::clearPUnits()
  /*!
    Tidy up the PUnit vector
   */
{
  std::vector<pipeUnit*>::iterator vc;
  for(vc=PUnits.begin();vc!=PUnits.end();vc++)
    delete *vc;
  PUnits.clear();
  return;
}

void 
PipeLine::setPoints(const std::vector<Geometry::Vec3D>& V)
  /*!
    Set the sequence of points
    \param V :: Vector of points to used
   */
{
  clearPUnits();
  Pts=V;
  activeFlags.resize(Pts.size()-1);
  fill(activeFlags.begin(),activeFlags.end(),0);
  return;
}


void 
PipeLine::addPoint(const Geometry::Vec3D& newPt)
  /*!
    Add an additional point
    \param newPt :: Point to add
   */
{
  ELog::RegMethod RegA("PipeLine","addPoint");

  if (Pts.empty() || Pts.back()!=newPt)
    {
      const size_t Index(Pts.size());
      if (Index>1)
	{
          const Geometry::Vec3D AAxis=(Pts[Index-2]-Pts[Index-1]).unit();
	  const Geometry::Vec3D BAxis=(newPt-Pts[Index-1]).unit();

          // This is to test if the axis reverses on itself
	  if (AAxis.dotProd(BAxis)>1.0-Geometry::zeroTol)
	    {
	      ELog::EM<<"["<<keyName<<"] Points reversed at index "
		      <<Index<<ELog::endCrit;
	      ELog::EM<<"PtA "<<Pts[Index-2]<<ELog::endCrit;
	      ELog::EM<<"PtB "<<Pts[Index-1]<<ELog::endCrit;
	      ELog::EM<<"PtNew "<<newPt<<ELog::endErr;
	      return;
	    }
	}
      Pts.push_back(newPt);
      if (Pts.size()>1)
	activeFlags.push_back(0);
    }
  else
    {
      ELog::EM<<"Adding Singular Point "<<Pts.size()<<":"
	      <<newPt<<ELog::endErr;
    }
  return;
}

void 
PipeLine::addSurfPoint(const Geometry::Vec3D& Pt,const std::string& surfStr)
  /*!
    Add an additional point
    \param Pt :: Point to add
    \param surfStr :: Outgoing surface string
  */
{ 
  ELog::RegMethod RegA("PipeLine","addSurfPoint");
  addSurfPoint(Pt,surfStr,std::string());
  return;
}

void 
PipeLine::addSurfPoint(const Geometry::Vec3D& Pt,
		       const std::string& surfStr,
		       const std::string& commonStr)
  /*!
    Add an additional point
    \param Pt :: Point to add
    \param surfStr :: Outgoing surface string
    \param commonStr :: common surface
   */
{ 
  ELog::RegMethod RegA("PipeLine","addSurfPoint");

  HeadRule LSurf,CSurf;

  if (LSurf.procString(surfStr)!=1)
    throw ColErr::InvalidLine("surfStr",surfStr,0);

  layerSurf.insert(std::map<size_t,HeadRule>::value_type(Pts.size(),LSurf));
  if (!commonStr.empty())
    {
      if (CSurf.procString(commonStr)!=1)
	throw ColErr::InvalidLine("commonStr",commonStr,0);
    }
  // insert empty rule if needed
  commonSurf.insert(std::map<size_t,HeadRule>::value_type(Pts.size(),CSurf));

  Pts.push_back(Pt);
  if (Pts.size()>1)
    activeFlags.push_back(0);

  return;
}

void
PipeLine::setActive(const size_t uIndex,const size_t flag)
  /*!
    Set the activity flag: 
    Goes from inner to outer 
    \param uIndex :: point layer (between uIndex and uIndex+1)
    \param flag :: 0 -- all, bits in order from inner point to outer
   */
{
  ELog::RegMethod RegA("PipeLine","setActive");

  if (activeFlags.size()<=uIndex)
    throw ColErr::IndexError<size_t>(uIndex,activeFlags.size(),"uIndex");
  activeFlags[uIndex]=flag;
  return;
}


void 
PipeLine::addRadius(const double R,const int M,const double T)
  /*!
    Add an additional radius
    \param R :: Radius
    \param M :: Material number
    \param T :: Temperature [kelvin]
   */
{
  ELog::RegMethod RegA("PipeLine","addRadius");
  nCylinder++;
  if (!CV.empty() && CV.back().CRadius>R)
    {
      ELog::EM<<"CV == "<<CV.size()<<ELog::endDiag;
      ELog::EM<<"Radii for pipeline["<<keyName
	      <<"] must be in increasing order"<<ELog::endErr;
    }
   
  CV.push_back(cylValues(R,M,T));

  return;
}

void
PipeLine::addInsertCell(const size_t segment,const int CellN)
  /*!
    Add a cell number that is definatively going to be cut by the 
    pipe. This is a method for dealing with ultra-thin cells etc.
    \param segment :: Index of pipe segment+1 [use 0 for all]
    \param CellN :: Cell to exclude
  */
{
  ELog::RegMethod RegA("PipeLine","addInsertCell");
  
  typedef std::map<size_t,std::set<int> > MTYPE;
  MTYPE::iterator mc=segExtra.find(segment);
  if (mc==segExtra.end())
    {
      segExtra.insert(MTYPE::value_type(segment,std::set<int>()));
      mc=segExtra.find(segment);
    }
  MTYPE::mapped_type& CSet=mc->second;
  
  CSet.insert(CellN);
  
  return; 
}

void
PipeLine::forcedInsertCells(const size_t Index)
  /*!
    Force the segment extra cell numbers to the pipeUnits
    so that we are guarranteed to insert into those cells
    \param Index :: PipeUnits [unchecked]
  */
{
  ELog::RegMethod RegA("PipeLine","forcedInsertCells");

  typedef std::map<size_t,std::set<int> > MTYPE;
  
  MTYPE::const_iterator mc=segExtra.find(Index+1);
  if (mc!=segExtra.end())
    PUnits[Index]->addInsertSet(mc->second);
  
  // Universal
  mc=segExtra.find(0);
  if (mc!=segExtra.end())
    PUnits[Index]->addInsertSet(mc->second);
  
  return;
}

  
int
PipeLine::createUnits(Simulation& System)
  /*!
    Create the individual pipeUnits
    \param System :: Simulation to add pipes to
    \return 0 on success / -ve on error
   */
{
  ELog::RegMethod RegA("PipeLine","createUnits");

  if (Pts.size()<2)
    {
      ELog::EM<<"No points to create pipeLine"<<ELog::endCrit;
      return -1;
    }
  
  // Set the points
  HeadRule PtRule;
  for(size_t i=1;i<Pts.size();i++)
    {
      pipeUnit* PU=new pipeUnit(keyName,i);
      PU->setPoints(Pts[i-1],Pts[i]);
      if (layerSurf.find(i-1)!=layerSurf.end())
	{
	  PtRule=layerSurf[i-1];
      	  PtRule.addIntersection(commonSurf[i-1]);
	  PU->setASurf(PtRule);
	}
      // Complementary object only for modified surface
      if (layerSurf.find(i)!=layerSurf.end())
	{
	  PtRule=layerSurf[i];
	  PtRule.makeComplement();
	  PtRule.addIntersection(commonSurf[i]);
	  PU->setBSurf(PtRule);
	}
      PUnits.push_back(PU);
    } 
  for(size_t i=0;i<PUnits.size();i++)
    {
      if (i>0)
	PUnits[i]->connectFrom(PUnits[i-1]);
      if (i<PUnits.size()-1)
	PUnits[i]->connectTo(PUnits[i+1]);
    }

  
  // Actually build the units
  if (!startSurf.empty())
    {
      HeadRule ARule(startSurf);
      PUnits[0]->setASurf(ARule);
    }
  for(size_t i=0;i<PUnits.size();i++)
    {
      forcedInsertCells(i);
      PUnits[i]->setNAngle(nAngle);
      PUnits[i]->buildUnit(System,activeFlags[i],CV);
    }
  return 0;
}

void
PipeLine::setNAngle(const size_t NA)
  /*!
    Set the angles in the pipeline
    \param NA :: Number of angle segments
   */
{
  nAngle=NA;
  return;
}

const HeadRule&
PipeLine::getCap(const size_t index,const int side) const
  /*!
    Access the end cap rules
    \param index :: index to point
    \param side :: 0 / 1 for front/back
    \return HeadRule
  */  
{
  ELog::RegMethod RegA("PipeLine","getCap");
  if (index>=PUnits.size())
    throw ColErr::IndexError<size_t>(index,PUnits.size(),
				     "index >= PUnits.size()");
  return PUnits[index]->getCap(side);
}

const Geometry::Vec3D&
PipeLine::getAxis(const size_t index) const
  /*!
    Access the axis of a give pipe unit
    \param index :: index to point
    \return Axis unit vector
  */
{
  ELog::RegMethod RegA("PipeLine","getAxis");

  if (index>=PUnits.size())
    throw ColErr::IndexError<size_t>(index,PUnits.size(),
				     "index >= PUnits.size()");
  return PUnits[index]->getAxis();
}

const pipeUnit&
PipeLine::first() const
  /*!
    Access the first pipe unit
    \return first PUnit
  */
{
  ELog::RegMethod RegA("PipeLine","first");
  if (PUnits.empty())
    throw ColErr::InContainerError<size_t>(0,"PUnits.empty()");
  return *(PUnits.front());
}

const pipeUnit&
PipeLine::last() const
  /*!
    Access the last pipe unit
    \return last PUnit
   */
{
  ELog::RegMethod RegA("PipeLine","last");
  if (PUnits.empty())
    throw ColErr::InContainerError<size_t>(0,"PUnits.empty()");
  return *(PUnits.back());
}

void
PipeLine::setStartSurf(const std::string& startS)
  /*!
    Simple setter for start surf
    \param startS :: Start surface
  */
{
  ELog::RegMethod RegA("PipeLine","setStartSurf");

  startSurf=startS;
  return;
}
  
void
PipeLine::build(Simulation& System)
  /*!
    Global creation of pipe
    \param System :: Simulation to add vessel to
  */
{
  ELog::RegMethod RegA("PipeLine","build");
  createUnits(System);

  return;
}

  
}  // NAMESPACE ModelSystem
