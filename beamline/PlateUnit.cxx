/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   beamline/PlateUnit.cxx 
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#include <functional>

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
#include "Quaternion.h"
#include "Vert2D.h"
#include "Convex2D.h"
#include "surfRegister.h"
#include "Surface.h"
#include "generateSurf.h"
#include "ShapeUnit.h"
#include "PlateUnit.h"

namespace beamlineSystem
{

PlateUnit::PlateUnit(const int ON,const int LS)  :
  ShapeUnit(ON,LS),CHPtr(0),nCorner(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param LS :: Layer separation
    \param ON :: offset number
  */
{}

PlateUnit::PlateUnit(const PlateUnit& A) : 
  ShapeUnit(A),
  CHPtr(A.CHPtr),XVec(A.XVec),YVec(A.YVec),ZVec(A.ZVec),
  nCorner(A.nCorner),APts(A.APts),BPts(A.BPts),
  nonConvex(A.nonConvex)
  /*!
    Copy constructor
    \param A :: PlateUnit to copy
  */
{}

PlateUnit&
PlateUnit::operator=(const PlateUnit& A)
  /*!
    Assignment operator
    \param A :: PlateUnit to copy
    \return *this
  */
{
  if (this!=&A)
    {
      ShapeUnit::operator=(A);
      XVec=A.XVec;
      YVec=A.YVec;
      ZVec=A.ZVec;
      nCorner=A.nCorner;
      APts=A.APts;
      BPts=A.BPts;
      nonConvex=A.nonConvex;
    }
  return *this;
}


PlateUnit::~PlateUnit() 
  /*!
    Destructor
   */
{
  delete CHPtr;
}

PlateUnit*
PlateUnit::clone() const 
  /*!
    Clone funciton
    \return *this
   */
{
  return new PlateUnit(*this);
}

void
PlateUnit::clear()
  /*!
    Clear all the points
   */
{
  delete CHPtr;
  CHPtr=0;
  nCorner=0;
  cells.clear();
  APts.clear();
  BPts.clear();
  nonConvex.clear();
  return;
}

void
PlateUnit::addCell(const int C) 
  /*!
    Add a cell to the cell list
    \param C :: Cell number
  */
{
  cells.push_back(C);
  return;
}

size_t
PlateUnit::findFirstPoint(const Geometry::Vec3D& testPt,
			  const std::vector<Geometry::Vec3D>& BVec) 
/*!
    Find the first point in BPts that equals BPts
    \param testPt :: First point to find
    \param BVec :: Vector of look up
    \return 
  */
{
  ELog::RegMethod RegA("PlateUnit","findFirstPt");
  
  std::vector<Geometry::Vec3D>::const_iterator vc=
    find_if(BVec.begin(),BVec.end(),
	    std::bind(std::equal_to<Geometry::Vec3D>(),
		      std::placeholders::_1,testPt));

  return (vc!=BVec.end()) ? 
    static_cast<size_t>(vc-BVec.begin()) :  BVec.size();
}
 
void
PlateUnit::constructConvex()
  /*!
    Process a group of points to produce correct orientation
    -- note need non-static as we need axis information (Y)
    to remove bias
    \param Y :: Principle direction
   */
{
  ELog::RegMethod RegA("PlateUnit","constructConvex");

  if (nCorner>2)
    {
      delete CHPtr;
      CHPtr=new Geometry::Convex2D;
      CHPtr->setPoints(APts);
      CHPtr->constructHull();

      std::vector<Geometry::Vec3D> OutPt=
	CHPtr->getSequence();

      /// DETERMINE THOSE POINTS IN CENTROID:
      size_t i;
      size_t APt(OutPt.size());
      size_t BPt(OutPt.size());
      for(i=0;i<nCorner && APt==OutPt.size();i++)
	APt=findFirstPoint(APts[i],OutPt);
      for(;i<nCorner && BPt==OutPt.size();i++)
	BPt=findFirstPoint(APts[i],OutPt);
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

      const Geometry::Vec3D N(AX*YVec);
      
      if (N.dotProd(Cent)>0)
	{
	  // rotate : could you c++11 construct rotate
	  for(size_t i=1;i<(nCorner+1)/2;i++)
	    {
	      std::swap(APts[i],APts[nCorner-i]);
	      std::swap(BPts[i],BPts[nCorner-i]);
	    }
	}  
      // Having done the rotation we now can check for non-convex.      
      for(size_t i=0;i<nCorner;i++)
	{
	  APt=findFirstPoint(APts[i],OutPt);
	  nonConvex[i] = (APt!=OutPt.size()) ? 0 : 1;
	}
    }
  return;
}

void
PlateUnit::setEndPts(const Geometry::Vec3D& A,
		     const Geometry::Vec3D& B)
  /*!
    Set the end points
    \param A :: First point
    \param B :: Second point
   */
{
  begPt=A;
  endPt=B;
  return;
}


void
PlateUnit::setXAxis(const Geometry::Vec3D& X,
		    const Geometry::Vec3D& Z)
 /*!
   Set the main axis
   \param X :: XAxis direction
   \param Z :: Z axis sense [i.e to ZVec is in its direction]
  */
{
  ELog::RegMethod RegA("PlateUnit","setXAxis");
  
  if (begPt.Distance(endPt)<Geometry::zeroTol)
    throw ColErr::NumericalAbort("Vector beg/end unset");
  
  XVec=X.unit();
  YVec=(endPt-begPt).unit();
  ZVec=XVec*YVec;
  if (ZVec.dotProd(Z)<0)
    ZVec*=-1.0;
  return;
}

void
PlateUnit::addPrimaryPoint(const Geometry::Vec3D& PA)
  /*!
    Add extra point
    \param PA :: Point that is mirror to both ends
   */
{
  APts.push_back(PA);
  BPts.push_back(PA);
  nonConvex.push_back(0);
  nCorner=APts.size();
  return;
}

void
PlateUnit::addPairPoint(const Geometry::Vec3D& PA,
			const Geometry::Vec3D& PB)
  /*!
    Add extra point
    \param PA :: A  Point
    \param PB :: B Point
   */
{
  APts.push_back(PA);
  BPts.push_back(PB);
  nonConvex.push_back(0);
  nCorner=APts.size();
  return;
}

Geometry::Vec3D
PlateUnit::frontPt(const size_t Index) const
  /*!
    Calculate the real point based on the offest
    \param Index :: Index poitn
    \return real point 
   */
{
  const Geometry::Vec3D& CPT(APts[(Index % nCorner)]);
  return begPt+XVec*CPT.X()+ZVec*CPT.Z();
}

Geometry::Vec3D
PlateUnit::backPt(const size_t Index) const
  /*!
    Calculate the real point based on the offest
    \param Index :: Index point
    \return real point 
   */
{
  const Geometry::Vec3D& CPT(BPts[(Index % nCorner)]);
  return endPt+XVec*CPT.X()+ZVec*CPT.Z();
}
  

std::pair<Geometry::Vec3D,Geometry::Vec3D>
PlateUnit::frontPair(const size_t IndexA,const size_t IndexB,
		     const double scale) const
  /*!
    Given two corners which define a normal to line.
    Step out by the distance scale.
    -- Converts both points into normal origin/X/Y
    \param IndexA :: First corner
    \param IndexB :: Second corner
    \param scale :: Distance out from the surface
    \return shifted points
  */
{
  ELog::RegMethod RegA("PlateUnit","frontPair");

  Geometry::Vec3D APoint=frontPt(IndexA);
  Geometry::Vec3D BPoint=frontPt(IndexB);
  return scaledPair(APoint,BPoint,scale);
}

std::pair<Geometry::Vec3D,Geometry::Vec3D>
PlateUnit::backPair(const size_t IndexA,const size_t IndexB,
		    const double scale) const
/*!
    Given two corners which define a normal to line.
    Step out by the distance scale.
    -- Converts both points into normal origin/X/Y
    \param IndexA :: First corner
    \param IndexB :: Second corner
    \param scale :: Distance out from the surface
    \return shifted points
  */
{
  ELog::RegMethod RegA("PlateUnit","backPair");

  Geometry::Vec3D APoint=backPt(IndexA);
  Geometry::Vec3D BPoint=backPt(IndexB);
  return scaledPair(APoint,BPoint,scale);
}

std::pair<Geometry::Vec3D,Geometry::Vec3D>
PlateUnit::scaledPair(const Geometry::Vec3D& APoint,
		      const Geometry::Vec3D& BPoint,
		      const double scale) const
  /*!
    Determine a scaled pair
    \param APoint :: First point 
    \param BPoint :: Second point 
    \return pair unit
  */
{
  ELog::RegMethod RegA("PlateUnit","scaledPair");
  std::pair<Geometry::Vec3D,Geometry::Vec3D> Out(APoint,BPoint);
  // Note points go inward
      
  if (fabs(scale)>Geometry::zeroTol)
    {
      Geometry::Vec3D ASide=(BPoint-APoint).unit();
      ASide=YVec*ASide;
      Out.first-=ASide*scale;

      Out.second-=ASide*scale;
    }
  return Out;
}

Geometry::Vec3D 
PlateUnit::sideNorm(const std::pair<Geometry::Vec3D,
		    Geometry::Vec3D>& touchPair) const
  /*!
    Calculate the normal leaving the side of the triangle
    \param touchPair :: Pair of points making up surface (with +Z)
    \return Normal
  */
{
  ELog::RegMethod RegA("PlateUnit","sideNorm");

  // Now get cross vector
  const Geometry::Vec3D ASide=(touchPair.second-touchPair.first).unit();
  return YVec*ASide;
}

void
PlateUnit::createSurfaces(ModelSupport::surfRegister& SMap,
			  const int indexOffset,
			  const std::vector<double>& Thick)
  /*!
    Build the surfaces for the track
    \param SMap :: SMap to use
    \param indexOffset :: Index offset
    \param Thick :: Thickness for each layer
   */
{
  ELog::RegMethod RegA("PlateUnit","createSurfaces");

  for(size_t j=0;j<Thick.size();j++)
    {
      // Start from 1
      int SN(indexOffset+offset+
	     static_cast<int>(j)*layerSep+1);

      for(size_t i=0;i<nCorner;i++)
	{
	  const std::pair<Geometry::Vec3D,Geometry::Vec3D> PX=
	    frontPair(i,i+1,Thick[j]);
	  ModelSupport::buildPlane(SMap,SN,
				   PX.first,PX.second,
				   backPt(i),
				   sideNorm(PX));
	  SN++;
	}
    }

  const std::pair<Geometry::Vec3D,Geometry::Vec3D> PXX=
    frontPair(0,1,0.0);
   
  return;
}


int
PlateUnit::inHull(const Geometry::Vec3D& testPt) const
  /*!
    Determine if the point is within the hull
    \param testPt :: test Point
    \return -1 [out of hull] if hull not built
   */
{
  ELog::RegMethod RegA("PlateUnit","inHull");
  return (CHPtr) ? CHPtr->inHull(testPt) : -1;
}

std::string
PlateUnit::getString(const size_t layerN) const
  /*!
    Write string for layer number
    \param layerN :: Layer number
    \return inward string
  */
{
  ELog::RegMethod RegA("PlateUnit","getString");

  if (!nCorner) return "";

  std::ostringstream cx;
  bool bFlag(0);
  const int lValue(offset+static_cast<int>(layerN)*layerSep);
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
      cx<<lValue+static_cast<int>(i+1);
    }
  if (bFlag) cx<<")";
  return cx.str();
}

std::string
PlateUnit::getExclude(const size_t layerN) const
  /*!
    Write string for layer number
    \param layerN :: Layer number
    \return inward string
  */
{
  ELog::RegMethod RegA("PlateUnit","getExclude");
  
  if (!nCorner) return "";
  std::ostringstream cx;
  cx<<" ( ";
  cx<<-(offset+static_cast<int>(layerN*layerSep+1));
  for(size_t i=1;i<nCorner;i++)
    cx<<" : "<<-(offset+static_cast<int>(layerN*layerSep+i+1));

  cx<<" ) ";
  return cx.str();
}

  
}  // NAMESPACE beamlineSystem
