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
#include "ModelSupport.h"
#include "ShapeUnit.h"
#include "PlateUnit.h"

#include "Rules.h"
#include "Quadratic.h"
#include "Plane.h"
#include "HeadRule.h"

namespace beamlineSystem
{

PlateUnit::PlateUnit(const int ON,const int LS)  :
  ShapeUnit(ON,LS),CHPtr(0),nCorner(0),rotateFlag(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param LS :: Layer separation
    \param ON :: offset number
  */
{}

PlateUnit::PlateUnit(const PlateUnit& A) : 
  ShapeUnit(A),
  CHPtr(A.CHPtr),XVec(A.XVec),YVec(A.YVec),ZVec(A.ZVec),
  nCorner(A.nCorner),rotateFlag(A.rotateFlag),
  APts(A.APts),BPts(A.BPts),
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
      rotateFlag=A.rotateFlag;
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
  rotateFlag=0;
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
   Find the first point in BVec that equals testPt
   \param testPt :: First point to find
   \param BVec :: Vector of look up
   \return position in BVec 
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
  */
{
  ELog::RegMethod RegA("PlateUnit","constructConvex");

  if (nCorner>2)
    {
      delete CHPtr;
      // construct convex for front window
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
	  rotateFlag=1;
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
  ZVec.unit();
  
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
    \param PA :: A Point
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
PlateUnit::frontPt(const size_t Index,const double T) const
  /*!
    Calculate the real point based on the offest
    \param Index :: Index point
    \param T :: Offset distance (T)
    \return real point 
   */
{
  const Geometry::Vec3D& CPT(APts[Index % nCorner]);

  const double XScale(1.0+T/fabs(CPT.X()));
  const double ZScale(1.0+T/fabs(CPT.Z()));

  return begPt+XVec*(CPT.X()*XScale)+ZVec*(CPT.Z()*ZScale);
}

Geometry::Vec3D
PlateUnit::backPt(const size_t Index,const double T) const
  /*!
    Calculate the real point based on the offest
    \param Index :: Index point
    \return real point 
   */
{
  const Geometry::Vec3D& CPT(BPts[Index % nCorner]);

  const double XScale(1.0+T/fabs(CPT.X()));
  const double ZScale(1.0+T/fabs(CPT.Z()));

  return endPt+XVec*(CPT.X()*XScale)+ZVec*(CPT.Z()*ZScale);
}
 
void
PlateUnit::createSurfaces(ModelSupport::surfRegister& SMap,
			  const std::vector<double>& Thick)
  /*!
    Build the surfaces for the track
    \param SMap :: SMap to use
    \param Thick :: Thickness for each layer
   */
{
  ELog::RegMethod RegA("PlateUnit","createSurfaces");

  
  for(size_t j=0;j<Thick.size();j++)
    {
      // Start from 1
      int SN(shapeIndex+layerSep*static_cast<int>(j)+1);

      for(size_t i=0;i<nCorner;i++)
	{
	  const Geometry::Vec3D PA=frontPt(i,Thick[j]);
	  const Geometry::Vec3D PB=frontPt(i+1,Thick[j]);
	  const Geometry::Vec3D BA=backPt(i,Thick[j]);
	  Geometry::Vec3D Norm=(BA-PA)*(PB-PA);
	  Norm.makeUnit();
	  
	  if (!rotateFlag)
	    Norm*=-1;
	  ModelSupport::buildPlane(SMap,SN,PA,PB,BA,Norm);
	  SN++;
	}
    }   
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
PlateUnit::getString(const ModelSupport::surfRegister& SMap,
		      const size_t layerN) const
  /*!
    Write string for layer number
    \param SMap :: Surface register
    \param layerN :: Layer number
    \return inward string
  */
{
  ELog::RegMethod RegA("PlateUnit","getString");

  if (!nCorner) return "";

  std::ostringstream cx;
  bool bFlag(0);
  // Start from 1
  int SN(layerSep*static_cast<int>(layerN)+1);
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
      cx<<SN++;
    }
  if (bFlag) cx<<")";

  return ModelSupport::getComposite(SMap,shapeIndex,cx.str());
}

std::string
PlateUnit::getExclude(const ModelSupport::surfRegister& SMap,
		       const size_t layerN) const
  /*!
    Write string for layer number
    \param SMap :: Surface register
    \param layerN :: Layer number
    \return inward string
  */
{
  ELog::RegMethod RegA("PlateUnit","getExclude");
  HeadRule Out(getString(SMap,layerN));
  Out.makeComplement();
  return Out.display();
}
  
}  // NAMESPACE beamlineSystem
