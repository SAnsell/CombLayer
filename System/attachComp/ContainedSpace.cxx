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
#include "ContainedComp.h"
#include "ContainedSpace.h"


namespace attachSystem
{

ContainedSpace::ContainedSpace()  :
  ContainedComp(),nDirection(8),
  primaryCell(0),buildCell(0),LCutters(2)
  /*!
    Constructor 
  */
{}

ContainedSpace::ContainedSpace(const ContainedSpace& A) : 
  ContainedComp(A),
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
  ELog::RegMethod RegA("ContainedComp","setConnect");
  if (Index>=LCutters.size())
    throw ColErr::IndexError<size_t>(Index,LCutters.size(),"LU.size/index");

  LCutters[Index].setConnectPt(C);
  LCutters[Index].setAxis(A);
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
  ELog::RegMethod RegA("ContainedSpace","setUSLinkCopy");
  
  if (Index>=LCutters.size())
    throw ColErr::IndexError<size_t>(Index,LCutters.size(),
				     "LCutters size/index");
  LCutters[Index]=FC.getSignedLU(sideIndex);
  
  return;
}
  
void
ContainedSpace::calcBoundary(Simulation& System,const int cellN,
			    const size_t NDivide)
  /*!
    Boundary calculator
    \param System :: Simulation
    \param cellN :: Cell number
    \param NDivide :: Number to divide
  */
{
  ELog::RegMethod RegA("ContainedSpace","calcBoundary");
  
  const MonteCarlo::Qhull* outerObj=System.findQhull(cellN);
  if (!outerObj)
    throw ColErr::InContainerError<int>(cellN,"cellN on found");

  HeadRule objHR=outerObj->getHeadRule();
  std::set<int> surfN;
  for(const LinkUnit& LU : LCutters)
    {
      
      Geometry::Vec3D OPt=LU.getConnectPt();
      const Geometry::Vec3D& YY=LU.getAxis();
      const Geometry::Vec3D XX=YY.crossNormal();
      const Geometry::Vec3D ZZ=YY*XX;
      OPt-=YY*0.1;

      if (!outerObj->isValid(OPt))
	{
	  ELog::EM<<"Object == "<<*outerObj<<ELog::endDiag;
	  ELog::EM<<"Point == "<<OPt<<ELog::endDiag;
	  throw ColErr::InContainerError<Geometry::Vec3D>
	    (OPt,"Point out of object");
	}

      const double angleStep=2.0*M_PI/static_cast<double>(NDivide);
      double angle(0.0);
      for(size_t i=0;i<NDivide;i++)
	{
	  const Geometry::Vec3D Axis=XX*cos(angle)+ZZ*sin(angle);
	  double D;
	  // Outgoing sign
	  const int SN=objHR.trackSurf(OPt,Axis,D);
	  surfN.insert(-SN);
	  angle+=angleStep;
	}
    }
  // NOW eliminate all surfaces NOT in surfN
  const std::set<int> fullSurfN=objHR.getSurfSet();
  for(const int SN : fullSurfN)
    {
      if (surfN.find(SN) == surfN.end())
	objHR.removeItems(SN);
    }

  
  // Check for no negative repeats:
  BBox.reset();
  for(const int SN : surfN)
    {
      if (SN==0 || (SN<0 && surfN.find(-SN)!=surfN.end()))
	throw ColErr::InContainerError<int>(SN,"Surf repeated");
      BBox.addIntersection(SN);
    }
  return;
}

void
ContainedSpace::registerSpaceCut(const long int linkA,const long int linkB)
				
  /*!
    Register the surface space
    \param 
  */
{
  ABLink.first=linkA;
  ABLink.second=linkB;
  return;
}

void
ContainedSpace::buildWrapCell(Simulation& System,
			      const int pCell,
			      const int cCell)
  /*!
    Build the cells within the bounding space that
    contains the complex outerSurf
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


  HeadRule inwardCut;  
  HeadRule innerVacuum(outerSurf);
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

  outerSurf=inwardCut;
  return;
}

void
ContainedSpace::initialize()
  /*!
    Initializer to be run after object is built
  */
{
  if (ABLink.first && ABLink.second)
    {
      FixedComp& FC=dynamic_cast<FixedComp&>(*this);
      ContainedSpace::setLinkCopy(0,FC,ABLink.first);
      ContainedSpace::setLinkCopy(1,FC,ABLink.second);
      if (!primaryCell && !insertCells.empty())
	primaryCell=insertCells.front();
      if (!buildCell)
	buildCell=FC.nextCell();
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
      calcBoundary(System,primaryCell,nDirection);
      buildWrapCell(System,primaryCell,buildCell);
      primaryCell=0;
    }

  ContainedComp::insertObjects(System);
	
  return;
}

}  // NAMESPACE attachSystem
