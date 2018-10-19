/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachComp/CSGroup.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#include <deque>
#include <string>
#include <algorithm>
#include <iterator>
#include <memory>
#include <functional>

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
#include "SurInter.h"
#include "Rules.h"
#include "HeadRule.h"
#include "Object.h"
#include "Line.h"
#include "LineIntersectVisit.h"
#include "Qhull.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "AttachSupport.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "SpaceCut.h"

#include "CSGroup.h"


namespace attachSystem
{

CSGroup::CSGroup()  :
  nDirection(8),primaryCell(0),buildCell(0),
  LCutters(2)
  /*!
    Constructor 
  */
{}

CSGroup::CSGroup(const FCTYPE& FA)  :
  FCobjects({{FA->getKeyName(),FA}}),
  nDirection(8),primaryCell(0),buildCell(0),
  LCutters(2)
  /*!
    Constructor 
    \param FA :: Single Fixed Component to cut on
  */
{}

CSGroup::CSGroup(const FCTYPE& FA,const FCTYPE& FB)  :
  FCobjects({{FA->getKeyName(),FA},
	{FB->getKeyName(),FB}}),
  nDirection(8),primaryCell(0),buildCell(0),
  LCutters(2)
  /*!
    Constructor 
  */
{}

CSGroup::CSGroup(const FCTYPE& FA,const FCTYPE& FB,
		 const FCTYPE& FC)  :
  FCobjects({
      {FA->getKeyName(),FA},
      {FB->getKeyName(),FB},
      {FC->getKeyName(),FC}}),
  nDirection(8),primaryCell(0),buildCell(0),
  LCutters(2)
  /*!
    Constructor 
  */
{}


CSGroup::~CSGroup()
  /*!
    Deletion operator
  */
{}
  
void
CSGroup::setConnect(const size_t Index,
		    const Geometry::Vec3D& C,
		    const Geometry::Vec3D& A)
 /*!
   Set the axis of the linked component
   \param Index :: Link number
   \param C :: Centre coordinate
   \param A :: Axis direciton
 */
{
  ELog::RegMethod RegA("CSGroup","setConnect");
  if (Index>=LCutters.size())
    throw ColErr::IndexError<size_t>(Index,LCutters.size(),"LU.size/index");

  LCutters[Index].setConnectPt(C);
  LCutters[Index].setAxis(A);
  return;
}

void
CSGroup::setLinkSurf(const size_t Index,const HeadRule& HR) 
  /*!
    Set a surface to output
    \param Index :: Link number
    \param HR :: HeadRule to add
  */
{
  ELog::RegMethod RegA("CSGroup","setLinkSurf(HR)");
  if (Index>=LCutters.size())
    throw ColErr::IndexError<size_t>(Index,LCutters.size(),"LU size/Index");

  LCutters[Index].setLinkSurf(HR);
  return;
}

void
CSGroup::setLinkSurf(const size_t Index,const int SN) 
  /*!
    Set a surface to output
    \param Index :: Link number
    \param SN :: surf to add
  */
{
  ELog::RegMethod RegA("CSGroup","setLinkSurf(int)");
  if (Index>=LCutters.size())
    throw ColErr::IndexError<size_t>(Index,LCutters.size(),"LU size/Index");

  LCutters[Index].setLinkSurf(SN);
  return;
}

void
CSGroup::setLinkCopy(const size_t Index,
		     const FixedComp& FC,
		     const long int sideIndex)
  /*!
    Copy the opposite (as if joined) link surface 
    Note that the surfaces are complemented
    \param Index :: Link number
    \param FC :: Other Fixed component to copy object from
    \param sideIndex :: link unit of other object
  */
{
  ELog::RegMethod RegA("CSGroup","setLinkCopy");
  
  if (Index>=LCutters.size())
    throw ColErr::IndexError<size_t>(Index,LCutters.size(),
				     "LCutters size/index");
  LCutters[Index]=FC.getSignedLU(sideIndex);
  
  return;
}

void
CSGroup::registerFC(const std::shared_ptr<attachSystem::FixedComp>& FCPtr)
  /*!
    Register a shared FixedComp
    \param FCPtr :: Fixed pointer
  */
{
  ELog::RegMethod RegA("CSGroup","registerFC");


  const std::string keyName=FCPtr->getKeyName();
  FCobjects.emplace(keyName,FCPtr);
  if (!buildCell)
    buildCell=FCPtr->nextCell();
  return;
}


void
CSGroup::buildWrapCell(Simulation& System,
		       const int pCell,
		       const int cCell)
  /*!
    Build the cells within the bounding space that
    contains the complex outerSurf
    \param pCell :: Primary cell
    \param cCell :: Contained cell
  */
{
  ELog::RegMethod RegA("CSGroup","buildWrapCell");

  const MonteCarlo::Qhull* outerObj=System.findQhull(pCell);
  if (!outerObj)
    throw ColErr::InContainerError<int>(pCell,"Primary cell does not exist");

  const int matN=outerObj->getMat();
  const double matTemp=outerObj->getTemp();

  // First make inner vacuum
  // removeing front/back surfaces

  
  inwardCut.reset();
  HeadRule innerVacuum;
  for(const FCMAP::value_type& mc : FCobjects)
    {
      const FCTYPE fcPtr= mc.second;
      const ContainedComp* ccPtr=dynamic_cast<const ContainedComp*>
	(fcPtr.get());
      if (ccPtr)
	innerVacuum.addUnion(ccPtr->getOuterSurf());
    }
      
  for(const LinkUnit& LU : LCutters)
    {
      const int SN=LU.getLinkSurf();
      innerVacuum.removeItems(-SN);
      inwardCut.addIntersection(-SN);
    }
  
  // Make new outer void
  HeadRule newOuterVoid(BBox);
  for(const LinkUnit& LU : LCutters)
    newOuterVoid.addIntersection(-LU.getLinkSurf());
  
  newOuterVoid.addIntersection(innerVacuum.complement());
  System.addCell(cCell,matN,matTemp,newOuterVoid.display());


  return;
}
  
void
CSGroup::insertAllObjects(Simulation& System)
  /*!
    Assumption that we can put this object into a space
    Makes the object bigger 
    Then constructs the outer boundary as if that object is 
    the outer boundary!
    \param System :: simulation system
  */
{
  ELog::RegMethod RegA("CSGroup","insertAllObjects");

  System.populateCells();

  if (primaryCell && buildCell)
    {
      BBox=SpaceCut::calcBoundary
	(System,primaryCell,nDirection,LCutters[0],LCutters[1]);
      buildWrapCell(System,primaryCell,buildCell);
    }

  if (primaryCell)
    {
      MonteCarlo::Object* outerObj=System.findQhull(primaryCell);
      if (outerObj)
	{
	  outerObj->addSurfString(inwardCut.complement().display());
	  primaryCell=0;
	}
    }
  return;
}

}  // NAMESPACE attachSystem
