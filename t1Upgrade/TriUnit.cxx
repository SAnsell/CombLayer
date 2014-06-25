/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   t1Upgrade/TriUnit.cxx 
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#include <boost/bind.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Vert2D.h"
#include "Convex2D.h"
#include "TriUnit.h"

namespace moderatorSystem
{


TriUnit::TriUnit(const int ON)  :
  offset(ON),CHPtr(0),nCorner(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param ON :: offset number
  */
{}


TriUnit::TriUnit(const TriUnit& A) : 
  offset(A.offset),cells(A.cells),
  CHPtr((A.CHPtr) ? new Geometry::Convex2D(*A.CHPtr) : 0),
  nCorner(A.nCorner),
  Pts(A.Pts),absolute(A.absolute),
  nonConvex(A.nonConvex)
  /*!
    Copy constructor
    \param A :: TriUnit to copy
  */
{}
  
TriUnit&
TriUnit::operator=(const TriUnit& A)
  /*!
    Assignment operator
    \param A :: TriUnit to copy
    \return *this
  */
{
  if (this!=&A)
    {
      cells=A.cells;
      delete CHPtr;
      CHPtr=(A.CHPtr) ? 
	new Geometry::Convex2D(*A.CHPtr) : 0;
      nCorner=A.nCorner;
      Pts=A.Pts;
      absolute=A.absolute;
      nonConvex=A.nonConvex;
    }
  return *this;
}

void
TriUnit::clear()
  /*!
    Clear all the points
   */
{
  delete CHPtr;
  CHPtr=0;
  nCorner=0;
  cells.clear();
  Pts.clear();
  absolute.clear();
  nonConvex.clear();
  return;
}

TriUnit::~TriUnit()
  /*!
    Destructor
  */
{
  delete CHPtr;
}

void
TriUnit::renormalize(const Geometry::Vec3D& Origin,
		     const Geometry::Vec3D& X,const Geometry::Vec3D& Y,
		     const Geometry::Vec3D& Z) 
  /*!
    Renormalize all the absolute points
    \param Origin :: Origin value
    \param X :: X axis 
    \param Y :: Y axis 
    \param Z :: Z axis 
  */
{
  ELog::RegMethod RegA("TriUnit","renormalize");

  for(size_t i=0;i<nCorner;i++)
    {
      if (absolute[i])
	{
	  Pts[i]-=Origin;
	  // component of Z to remove:
	  const double alpha=Pts[i].dotProd(Z);
	  Pts[i]-=Z*alpha;
	  // leaves Z without Z component:
	  
	  const double XD=X.dotProd(Pts[i]);
	  const double YD=Y.dotProd(Pts[i]);
	  Pts[i]=Geometry::Vec3D(XD,YD,0);
	  absolute[i]=0;
	}
    }
  return;
}

void
TriUnit::addCell(const int C) 
  /*!
    Add a cell to the cell list
    \param C :: Cell number
  */
{
  cells.push_back(C);
  return;
}

size_t
TriUnit::findFirstPoint(const Geometry::Vec3D& AVec,
			  const std::vector<Geometry::Vec3D>& BPts) 
/*!
    Find the first point in BPts that equals BPts
    \param AVec :: First point
    \param BPts :: Vector of look up
    \return 
  */
{
  ELog::RegMethod RegA("TriUnit","findFirstPt");
  
  std::vector<Geometry::Vec3D>::const_iterator vc=
    find_if(BPts.begin(),BPts.end(),
	    boost::bind(std::equal_to<Geometry::Vec3D>(),_1,AVec));

  return (vc!=BPts.end()) ? 
    static_cast<size_t>(vc-BPts.begin()) :  BPts.size();
}
 
void
TriUnit::constructConvex(const Geometry::Vec3D& Z)
  /*!
    Process a group of points to produce correct orientation
    -- note need non-static as we need axis information (Z)
    to remove biaze
   */
{
  ELog::RegMethod RegA("TriUnit","constructConvex");

  if (nCorner>2)
    {
      delete CHPtr;
      CHPtr=new Geometry::Convex2D;
      CHPtr->setPoints(Pts);
      CHPtr->constructHull();
      std::vector<Geometry::Vec3D> OutPt=CHPtr->getSequence();

      /// DETERMINE THOSE POINTS IN CENTROID:
      size_t i;
      size_t APt(OutPt.size());
      size_t BPt(OutPt.size());
      for(i=0;i<nCorner && APt==OutPt.size();i++)
	APt=findFirstPoint(Pts[i],OutPt);
      for(;i<nCorner && BPt==OutPt.size();i++)
	BPt=findFirstPoint(Pts[i],OutPt);
      i--;
      if (APt==OutPt.size())
	throw ColErr::InContainerError<Geometry::Vec3D>
	  (OutPt[APt],"Two outside points not found (A)");
      if (BPt==OutPt.size())
	throw ColErr::InContainerError<Geometry::Vec3D>
	  (OutPt[BPt],"Two outside points not found (B)");

      const Geometry::Vec3D AX=(OutPt[BPt]-OutPt[APt]).unit();  
      Geometry::Vec3D Cent=(OutPt[BPt]+OutPt[APt])/2.0;
      Cent-=CHPtr->getCentroid();
      Cent.makeUnit();

      const Geometry::Vec3D N(AX*Z);
      
      if (N.dotProd(Cent)>0)
	{
	  // rotate : could you c++11 construct  rotate
	  for(size_t i=1;i<(nCorner+1)/2;i++)
	    std::swap(Pts[i],Pts[nCorner-i]);
	}  
      // Having done the rotation we now can check for non-convex.      
      for(size_t i=0;i<nCorner;i++)
	{
	  APt=findFirstPoint(Pts[i],OutPt);
	  nonConvex[i] = (APt!=OutPt.size()) ? 0 : 1;
	}
    }
  return;
}

void
TriUnit::applyAbsShift(const double xStep,const double yStep,
		       const double zStep)
  /*!
    Apply a shift to the absolute points
    \param xStep :: Xstep
    \param yStep :: Ystep
    \param zStep :: Zstep
  */
{
  ELog::RegMethod RegA("TriUnit","applyAbsShift");

  const Geometry::Vec3D Shift(xStep,yStep,zStep);
  for(size_t i=0;i<nCorner;i++)
    {
      if (absolute[i])
	Pts[i]+=Shift;
    }
  return;

}


void
TriUnit::addObject(const Geometry::Vec3D& P,const int A)
  /*!
    Add extra point
    \param P :: Point
    \param A :: Absolute value
   */
{
  Pts.push_back(P);
  absolute.push_back(A);
  nonConvex.push_back(0);
  nCorner=Pts.size();
  return;
}

int
TriUnit::inHull(const Geometry::Vec3D& testPt) const
  /*!
    Determine if the point is within the hull
    \param testPt :: test Point
    \return -1 [out of hull] if hull not built
   */
{
  ELog::RegMethod RegA("TriUnit","inHull");
  return (CHPtr) ? CHPtr->inHull(testPt) : -1;
}

std::string
TriUnit::getString(const size_t layerN) const
  /*!
    Write string for layer number
    \param layerN :: Layer number
    \return inward string
  */
{
  ELog::RegMethod RegA("TriUnit","getString");

  if (!nCorner) return "";

  std::ostringstream cx;
  bool bFlag(0);
  for(size_t i=0;i<nCorner;i++)
    {
      if (nonConvex[i] || nonConvex[(i+1) % nCorner])
	{
	  cx<< ((!bFlag) ? " (" : ":");
	  bFlag=1;
	}
      else
	{
	  cx<< ((bFlag) ? ") " : " ");
	  bFlag=0;
	}
      cx<<offset+static_cast<int>(layerN*100+i+1);
    }
  if (bFlag) cx<<")";
  return cx.str();
}

std::string
TriUnit::getExclude(const size_t layerN) const
  /*!
    Write string for layer number
    \param layerN :: Layer number
    \return inward string
  */
{
  ELog::RegMethod RegA("TriUnit","getExclude");
  
  if (!nCorner) return "";
  std::ostringstream cx;
  cx<<" ( ";
  cx<<-(offset+static_cast<int>(layerN*100+1));
  for(size_t i=1;i<nCorner;i++)
    cx<<" : "<<-(offset+static_cast<int>(layerN*100+i+1));

  cx<<" ) ";
  return cx.str();
}

  
}  // NAMESPACE moderatorSystem
