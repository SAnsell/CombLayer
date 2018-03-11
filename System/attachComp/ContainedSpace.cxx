/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachComp/ContainedSpace.cxx
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
#include "writeSupport.h"
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
#include "Simulation.h"
#include "AttachSupport.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ContainedComp.h"
#include "ContainedSpace.h"


namespace attachSystem
{

ContainedSpace::ContainedSpace()  :
  ContainedComp(),active(0),noPrimaryInsert(0),
  nDirection(8),primaryCell(0),buildCell(0),LCutters(2)
  /*!
    Constructor 
  */
{}

ContainedSpace::ContainedSpace(const ContainedSpace& A) : 
  ContainedComp(A),FCName(A.FCName),active(A.active),
  noPrimaryInsert(A.noPrimaryInsert),
  nDirection(A.nDirection),primaryCell(A.primaryCell),
  buildCell(0),BBox(A.BBox),LCutters(A.LCutters)
  /*!
    Copy constructor
    \param A :: ContainedSpace to copy
  */
{}

ContainedSpace&
ContainedSpace::operator=(const ContainedSpace& A)
  /*!
    Assignment operator
    \param A :: ContainedSpace to copy
    \return *this
  */
{
  if (this!=&A)
    {
      ContainedComp::operator=(A);
      FCName=A.FCName;
      active=A.active;
      noPrimaryInsert=A.noPrimaryInsert;
      nDirection=A.nDirection;
      primaryCell=A.primaryCell;
      BBox=A.BBox;
      LCutters=A.LCutters;
    }
  return *this;
}

ContainedSpace::~ContainedSpace()
  /*!
    Deletion operator
  */
{}
  
void
ContainedSpace::setConnect(const size_t Index,
			   const Geometry::Vec3D& C,
			   const Geometry::Vec3D& A)
 /*!
   Set the axis of the linked component
   \param Index :: Link number
   \param C :: Centre coordinate
   \param A :: Axis direciton
 */
{
  ELog::RegMethod RegA("ContainedSpace","setConnect");
  if (Index>=LCutters.size())
    throw ColErr::IndexError<size_t>(Index,LCutters.size(),"LU.size/index");

  LCutters[Index].setConnectPt(C);
  LCutters[Index].setAxis(A);
  active=1;
  return;
}

void
ContainedSpace::setLinkSurf(const size_t Index,
			    const HeadRule& HR) 
  /*!
    Set a surface to output
    \param Index :: Link number
    \param HR :: HeadRule to add
  */
{
  ELog::RegMethod RegA("ContainedSpace","setLinkSurf(HR)");
  if (Index>=LCutters.size())
    throw ColErr::IndexError<size_t>(Index,LCutters.size(),"LU size/Index");

  LCutters[Index].setLinkSurf(HR);
  active=1;
  return;
}

void
ContainedSpace::setLinkSurf(const size_t Index,
			    const int SN) 
  /*!
    Set a surface to output
    \param Index :: Link number
    \param SN :: surf to add
  */
{
  ELog::RegMethod RegA("ContainedSpace","setLinkSurf(int)");
  if (Index>=LCutters.size())
    throw ColErr::IndexError<size_t>(Index,LCutters.size(),"LU size/Index");

  LCutters[Index].setLinkSurf(SN);
  active=1;
  return;
}

void
ContainedSpace::setLinkCopy(const size_t Index,
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
  ELog::RegMethod RegA("ContainedSpace","setLinkCopy");
  
  if (Index>=LCutters.size())
    throw ColErr::IndexError<size_t>(Index,LCutters.size(),
				     "LCutters size/index");
  
  LCutters[Index]=FC.getSignedLU(sideIndex);
  active=1;
  return;
}


HeadRule
ContainedSpace::calcBoundary(Simulation& System,
			     const int cellN,
			     const size_t NDivide,
			     const LinkUnit& ALink,
			     const LinkUnit& BLink)
  /*!
    Construct a bounding box in a cell based on the 
    link surfaces
    \param System :: Simulation to use
    \param cellN :: Cell number
    \param NDivide :: division in link point
    \param ALink :: first link point
    \param bLink :: second link point
    \return HeadRule of bounding box [- link surf]
  */
{
  ELog::RegMethod RegA("ContainedSpace","calcBoundary(System)");
  
  const MonteCarlo::Qhull* outerObj=System.findQhull(cellN);
  if (!outerObj)
    throw ColErr::InContainerError<int>(cellN,"cellN on found");

  const HeadRule objHR=outerObj->getHeadRule();

  return calcBoundary(objHR,NDivide,ALink,BLink);
}

HeadRule
ContainedSpace::calcBoundary(const HeadRule& objHR,
			     const size_t NDivide,
			     const LinkUnit& ALink,
			     const LinkUnit& BLink)
  /*!
    Construct a bounding box in a cell based on the 
    link surfaces
    \param objHR :: Headrule of boundary
    \param NDivide :: division in link point
    \param ALink :: first link point
    \param bLink :: second link point
    \return HeadRule of bounding box [- link surf]
  */
{
  ELog::RegMethod RegA("ContainedSpace","calcBoundary");

  std::set<int> linkSN;
  for(const LinkUnit& LU : {ALink,BLink})
    {
      const int SN=LU.getLinkSurf();
      linkSN.insert(SN);
    }

  std::set<int> surfN;
  // mid points moved in by 10% of distance
  const Geometry::Vec3D CPoint=
    (ALink.getConnectPt()+BLink.getConnectPt())/2.0;
  const Geometry::Vec3D& YA=ALink.getAxis();
  const Geometry::Vec3D& YB=BLink.getAxis();
  const Geometry::Vec3D YY= (YA.dotProd(YB)>0.0) ?
    (YA+YB).unit() : (YB-YA).unit();
  const Geometry::Vec3D XX=YY.crossNormal();
  const Geometry::Vec3D ZZ=YY*XX;


  if (!objHR.isValid(CPoint))
    {
      ELog::EM<<"Object == "<<objHR<<ELog::endDiag;
      ELog::EM<<"Point == "<<CPoint<<ELog::endDiag;
      throw ColErr::InContainerError<Geometry::Vec3D>
	(CPoint,"CPoint out of object");
    }
  
  const std::vector<Geometry::Vec3D> CP=
    {
      CPoint,
      ALink.getConnectPt()*0.95+CPoint*0.05,
      BLink.getConnectPt()*0.95+CPoint*0.05
    };
  for(const Geometry::Vec3D& Org : CP)
    {
      const double angleStep=2.0*M_PI/static_cast<double>(NDivide);
      double angle(0.0);
      for(size_t i=0;i<NDivide;i++)
	{
	  const Geometry::Vec3D Axis=XX*cos(angle)+ZZ*sin(angle);
	  double D;
	  const int SN=objHR.trackSurf(Org,Axis,D,linkSN);
	  if (SN)
	    surfN.insert(-SN);
	  angle+=angleStep;
	}
    }
  // NOW eliminate all surfaces NOT in surfN
  const std::set<int> fullSurfN=objHR.getSurfSet();
  HeadRule outBox=objHR;
  for(const int SN : fullSurfN)
    {
      if (surfN.find(SN) == surfN.end())
	outBox.removeItems(SN);
    }
  
  // Check for no negative repeats:
  for(const int SN : surfN)
    {
      if (SN==0 || (SN<0 && surfN.find(-SN)!=surfN.end()))
	throw ColErr::InContainerError<int>(SN,"Surf repeated");
    }
  return outBox;
}
  
void
ContainedSpace::calcBoundaryBox(Simulation& System)
  /*!
    Boundary calculator
    \param System :: Simulation
  */
{
  ELog::RegMethod RegA("ContainedSpace","calcBoundaryBox");
  BBox=calcBoundary(System,primaryCell,nDirection,LCutters[0],LCutters[1]);
  return;
}

void
ContainedSpace::registerSpaceCut(const long int linkA,const long int linkB)
				
  /*!
    Register the surface space
    \param linkA :: Signed link point
    \param linkB :: Signed link point
  */
{
  ABLink.first=linkA;
  ABLink.second=linkB;
  active=1;
  return;
}

void
ContainedSpace::registerSpaceIsolation
  (const long int linkA,const long int linkB)
  /*!
    Register the surface space and dont place primary
    \param linkA :: Signed link point
    \param linkB :: Signed link point
  */
{
  active=1;
  noPrimaryInsert=1;
  registerSpaceCut(linkA,linkB);
  return;
}

void
ContainedSpace::buildWrapCell(Simulation& System,
			      const int pCell,
			      const int cCell)
  /*!
    Build the cells within the bounding space that
    contains the complex outerSurf
    \param System :: Simulation
    \param pCell :: Primary cell
    \param cCell :: Contained cell
  */
{
  ELog::RegMethod RegA("ContainedSpace","buildWrapCell");

  const MonteCarlo::Qhull* outerObj=System.findQhull(pCell);
  if (!outerObj)
    throw ColErr::InContainerError<int>(pCell,"Primary cell does not exist");

  const int matN=outerObj->getMat();
  const double matTemp=outerObj->getTemp();

  // First make inner vacuum
  // removeing front/back surfaces

  outerCut.reset();
  
  HeadRule innerVacuum(outerSurf);
  for(const LinkUnit& LU : LCutters)
    {
      const int SN=LU.getLinkSurf();
      innerVacuum.removeItems(-SN);
      outerCut.addIntersection(-SN);
    }

  // Make new outer void
  HeadRule newOuterVoid(BBox);
  for(const LinkUnit& LU : LCutters)
    newOuterVoid.addIntersection(-LU.getLinkSurf());
  
  newOuterVoid.addIntersection(innerVacuum.complement());
  System.addCell(cCell,matN,matTemp,newOuterVoid.display());

  CellMap* CMapPtr=dynamic_cast<CellMap*>(this);
  if (CMapPtr)
    CMapPtr->addCell("OuterSpace",cCell);

  
  return;
}

void
ContainedSpace::initialize()
  /*!
    Initializer to be run after object is built
  */
{
  ELog::RegMethod RegA("ContainedSpace","initialize");

  FixedComp* FC=dynamic_cast<FixedComp*>(this);
  if (!FC) return;

  FCName=FC->getKeyName();
  if (active)
    {
      if (ABLink.first)
	ContainedSpace::setLinkCopy(0,*FC,ABLink.first);
      if (ABLink.second)
	ContainedSpace::setLinkCopy(1,*FC,ABLink.second);
      
      if (!primaryCell && !insertCells.empty())
	primaryCell=insertCells.front();
      if (!buildCell)
	buildCell=FC->nextCell();
    }

  return;
}
  
void
ContainedSpace::insertObjects(Simulation& System)
  /*!
    Assumption that we can put this object into a space
    Makes the object bigger 
    Then constructs the outer boundary as if that object is 
    the outer boundary!
    \param System :: simulation system
  */
{
  ELog::RegMethod RegA("ContainedSpace","insertObjects");
      
  System.populateCells();
  initialize();

  if (primaryCell && buildCell)
    {
      calcBoundaryBox(System);
      buildWrapCell(System,primaryCell,buildCell);
    }

  std::vector<int> IHold(insertCells);
  StrFunc::removeItem(insertCells,primaryCell);
  ContainedComp::insertObjects(System);
  
  if (!noPrimaryInsert && primaryCell && buildCell)
    {
      MonteCarlo::Qhull* outerObj=System.findQhull(primaryCell);
      if (outerObj)
	outerObj->addSurfString(outerCut.complement().display());
      else
	throw ColErr::InContainerError<int>(primaryCell,
					    "Cell not in Simulation");
    }
      

  return;
}

void
ContainedSpace::insertPair(Simulation& System,
			   const std::vector<int>& insertCells,
			   const attachSystem::FixedComp& FCA,
			   const long int linkA,
			   const attachSystem::FixedComp& FCB,
			   const long int linkB)
  /*!
    Build the cells within the bounding space that
    contains the complex outerSurf
    \param System :: Simulation 
    \param insertCells :: insert cells
    \param FCA :: FixedComp A
    \param linkA :: Link point [outward]
    \param FCB :: FixedComp B
    \param linkB :: Link point [outward]
  */
{
  ELog::RegMethod RegA("ContainedSpace","insertPair");

  HeadRule BBox;
  BBox.addUnion(FCA.getFullRule(linkA));
  BBox.addUnion(FCB.getFullRule(linkB));

  for(const int pCell : insertCells)
    {
      MonteCarlo::Qhull* outerObj=System.findQhull(pCell);
      if (!outerObj)
	throw ColErr::InContainerError<int>
	  (pCell,"Primary cell does not exist");
      outerObj->addSurfString(BBox.display());
    }      
  return;
}

  
}  // NAMESPACE attachSystem
