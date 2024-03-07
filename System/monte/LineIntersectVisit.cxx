/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   monte/LineIntersectVisit.cxx
 *
 * Copyright (c) 2004-2024 by Stuart Ansell
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
#include <list>
#include <stack>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <complex>
#include <memory>
#include <functional>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Triple.h"
#include "Vec3D.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Plane.h"
#include "ArbPoly.h"
#include "CylCan.h"
#include "Cylinder.h"
#include "Cone.h"
#include "EllipticCyl.h"
#include "MBrect.h"
#include "Sphere.h"
#include "General.h"
#include "Line.h"
#include "SurInter.h"
#include "particle.h"
#include "interPoint.h"
#include "HeadRule.h"
#include "LineIntersectVisit.h"

namespace MonteCarlo
{

std::ostream& 
operator<<(std::ostream& OX,const LineIntersectVisit& A)
  /*!
    Write to a strandard stream
    \param OX :: Output stream
    \param A :: LineVisit to write
    \return stream
  */
{
  A.write(OX);
  return OX;
}

LineIntersectVisit::LineIntersectVisit
  (const Geometry::Vec3D& Pt,const Geometry::Vec3D& uVec) :
    BaseVisit(),ATrack(Pt,uVec),neutIndex(0)
  /*!
    Constructor
    \param Pt :: Point to start track
    \param uVec :: Outgoing track direction
  */
{}

LineIntersectVisit::LineIntersectVisit
  (const Geometry::Line& L) :
    BaseVisit(),ATrack(L),neutIndex(0)
  /*!
    Constructor
    \param L :: Line
  */
{}

LineIntersectVisit::LineIntersectVisit
  (const MonteCarlo::particle& N) :
    BaseVisit(),ATrack(N.Pos,N.uVec),
    neutIndex(N.ID)
  /*!
    Constructor
    \param N :: Particle to track
  */
{}

void
LineIntersectVisit::setLine(const Geometry::Vec3D& Pt,
			    const Geometry::Vec3D& Axis)
  /*!
    Set the line to a new track.
    \param Pt :: Origin point
    \param Axis :: Axis direction
  */
{
  ATrack.setLine(Pt,Axis);
  neutIndex=0;
  return;
}

void
LineIntersectVisit::setLine(const particle& N)
  /*!
    Set the line to a new track.
    \param N :: Neutron
  */
{
  ATrack.setLine(N.Pos,N.uVec);
  neutIndex=N.ID;
  return;
}

void
LineIntersectVisit::Accept(const Geometry::Surface&)
  /*!
    Process an intersect track
    \param :: Surface to use int line Interesect
    \throw AbsObjMethod [always]
  */
{
  throw ColErr::AbsObjMethod("LineIntersectVisit::Accept Surface");
  return;
}

void
LineIntersectVisit::Accept(const Geometry::Quadratic& Surf)
  /*!
    Process an intersect track
    \param Surf :: Surface to use int line Interesect
  */
{
  std::vector<Geometry::Vec3D> PtVec;
  ATrack.intersect(PtVec,Surf);
  procTrack(PtVec,&Surf);
  return;
}

void
LineIntersectVisit::Accept(const Geometry::Plane& Surf)
  /*!
    Process an intersect track
    \param Surf :: Surface to use int line Interesect
  */
{
  std::vector<Geometry::Vec3D> PtVec;
  ATrack.intersect(PtVec,Surf);
  procTrack(PtVec,&Surf);
  return;
}

void
LineIntersectVisit::Accept(const Geometry::ArbPoly& Surf)
  /*!
    Process an intersect track
    \param Surf :: Surface to use in the line Interesect
  */
{
  std::vector<Geometry::Vec3D> PtVec;
  ATrack.intersect(PtVec,Surf);
  procTrack(PtVec,&Surf);
  return;
}

void
LineIntersectVisit::Accept(const Geometry::Cone& Surf)
  /*!
    Process an intersect track
    \param Surf :: Surface to use in the line Interesect
  */
{
  std::vector<Geometry::Vec3D> PtVec;
  ATrack.intersect(PtVec,Surf);
  procTrack(PtVec,&Surf);
  return;
}

void
LineIntersectVisit::Accept(const Geometry::CylCan& Surf)
  /*!
    Process an intersect track
    \param Surf :: Surface to use in the line Interesect
  */
{
  std::vector<Geometry::Vec3D> PtVec;
  ATrack.intersect(PtVec,Surf);
  procTrack(PtVec,&Surf);
  return;
}

void
LineIntersectVisit::Accept(const Geometry::Cylinder& Surf)
  /*!
    Process an intersect track
    \param Surf :: Surface to use int line Interesect
  */
{
  std::vector<Geometry::Vec3D> PtVec;
  ATrack.intersect(PtVec,Surf);
  procTrack(PtVec,&Surf);
  return;
}

void
LineIntersectVisit::Accept(const Geometry::EllipticCyl& Surf)
  /*!
    Process an intersect track
    \param Surf :: Surface to use int line Interesect
  */
{
  std::vector<Geometry::Vec3D> PtVec;
  ATrack.intersect(PtVec,Surf);
  procTrack(PtVec,&Surf);
  return;
}

void
LineIntersectVisit::Accept(const Geometry::Sphere& Surf)
  /*!
    Process an intersect track
    \param Surf :: Surface to use int line Interesect
  */
{
  std::vector<Geometry::Vec3D> PtVec;
  ATrack.intersect(PtVec,Surf);
  procTrack(PtVec,&Surf);
  return;
}

void
LineIntersectVisit::Accept(const Geometry::General& Surf)
  /*!
    Process an intersect track
    \param Surf :: Surface to use int line Interesect
  */
{
  std::vector<Geometry::Vec3D> PtVec;
  ATrack.intersect(PtVec,Surf);
  procTrack(PtVec,&Surf);
  return;
}

void
LineIntersectVisit::Accept(const Geometry::MBrect& Surf)
  /*!
    Process an intersect track
    \param Surf :: Surface to use int line Interesect
  */
{
  std::vector<Geometry::Vec3D> PtVec;
  ATrack.intersect(PtVec,Surf);
  procTrack(PtVec,&Surf);
  return;
}

void
LineIntersectVisit::Accept(const HeadRule& HR)
  /*!
    Process an intersect track
    Assumes that the point must be valid and either
    an incoming or outgoing intersection
    \param HR :: HeadRules to Interesect
  */
{
  // This sorts IPts
  HR.calcSurfIntersection(ATrack.getOrigin(),
			  ATrack.getDirect(),
			  IPts);

  return;
}

void
LineIntersectVisit::Accept(const Geometry::Torus&)
  /*!
    Process an intersect track
    \param :: Surface to use int line Interesect
  */
{
  throw ColErr::AbsObjMethod("LineIntersectVisit::Accept Torus");  
  return;
}

size_t
LineIntersectVisit::getNPoints() const
  /*!
    Get the number of points found
   */
{
  return IPts.size();
}

void
LineIntersectVisit::clearTrack() 
  /*!
    Clear all the track
   */
{
  IPts.clear();
  return;
}



void
LineIntersectVisit::procTrack(const std::vector<Geometry::Vec3D>& PtVec,
			      const Geometry::Surface* surfID) 
  /*!
    Sorts the PtVec and distances
    with a "closest first" order.
    \param surfID :: surface ID
  */
{
  // Calculate the distances to the points
  // add uncalculated distVec points
  for(const Geometry::Vec3D& Pt : PtVec)
    {
      const Geometry::Vec3D LP=Pt-ATrack.getOrigin();
      const double D=LP.dotProd(ATrack.getDirect());
      const int SN=(surfID) ? surfID->getName() : 0;
      IPts.push_back(Geometry::interPoint(LP,D,SN,surfID,0));
    }

  std::sort(IPts.begin(),IPts.end(),
	    [](const Geometry::interPoint& A,
	       const Geometry::interPoint& B)
	    {
	      return (A.D > B.D);
	    });
	    
	    
  return;
}

const Geometry::interPoint&
LineIntersectVisit::getClosest() const
  /*!
    Calculate the distance to the closest point along the line
    Note this has been pre-checked not to throw -- but is
    there just incase.
    \return point closest to line origin
  */
{  
  if (IPts.size()==1) 
    return IPts.front();

  std::vector<Geometry::interPoint>::const_iterator minIter=
    std::min_element(IPts.cbegin(),IPts.cend(),
		     [](const Geometry::interPoint& A,
			const Geometry::interPoint& B)
		     {
		       return std::abs<double>(A.D) <
			 std::abs<double>(B.D);
		     });

  if (minIter==IPts.cend())
    throw ColErr::EmptyContainer
      ("Calling uncheck LineIntersectVisit::getClosest"
       "code for empty IPts");

  return *minIter;
  
}


// ACCESSORS:
double
LineIntersectVisit::getDist(const Geometry::Surface* SPtr) 
  /*!
    Calculate the distance to the closest point along the line
    to the surface SPtr [+ve or -ve]
    \param SPtr :: surface to intersect
    \return distance
  */
{
  SPtr->acceptVisitor(*this);
  
  if (IPts.empty())
    throw ColErr::EmptyValue<void>("LineIntersecVisit::getDist<"+
				   SPtr->className()+">");

  if (IPts.size()==1)
    return IPts.front().D;
  
  double dOut(0.0);
  double absD=std::numeric_limits<double>::max();
  for(const Geometry::interPoint& ipt : IPts)
    {
      if (std::abs(ipt.D)<absD)
	{
	  absD=std::abs<double>(ipt.D);
	  dOut=ipt.D;
	}
    }
  // signed distance to closest point
  return dOut;
}

double
LineIntersectVisit::getForwardDist(const Geometry::Surface* SPtr) 
  /*!
    Calculate the distance to the closest point along the line
    to the surface SPtr. Only in the forward direction
    \param SPtr :: surface to intersect
    \return distance
  */
{
  SPtr->acceptVisitor(*this);

  bool posFlag(0);
  double dOut;

  
  for(const Geometry::interPoint& ipt : IPts)
    {
      if (ipt.D>Geometry::zeroTol &&
	  (!posFlag || ipt.D<dOut))
	{
	  dOut=ipt.D;
	  posFlag=1;
	}
    }
  
  if (!posFlag) 
    throw ColErr::EmptyValue<void>("LineIntersecVisit::getForwardDist<"+
				 SPtr->className()+">");
      
  return dOut;
}

Geometry::Vec3D
LineIntersectVisit::getPoint(const Geometry::Surface* SPtr) 
  /*!
    Calculate the point at the closest point along the line
    to the surface SPtr
    \param SPtr :: surface to intersect
    \return Points
  */
{
  clearTrack();
  SPtr->acceptVisitor(*this);
  if (IPts.empty())
    throw ColErr::EmptyContainer("LineIntersecVisit::getPoint<"+
				   SPtr->className()+"> : no points found");

  return getClosest().Pt;
}

Geometry::Vec3D
LineIntersectVisit::getPoint(const Geometry::Surface* SPtr,
			     const Geometry::Vec3D& nearPt) 
  /*!
    Calculate the point at the closest point along the line
    to the surface SPtr
    \param SPtr :: surface to intersect
    \return Points
  */
{
  clearTrack();
  SPtr->acceptVisitor(*this);
  if (IPts.empty())
    throw ColErr::EmptyContainer("LineIntersecVisit::getPoint<"+
				 SPtr->className()+">(near) no points");

  return SurInter::closestPt(IPts,nearPt).Pt;
}

Geometry::Vec3D
LineIntersectVisit::getPoint(HeadRule& HR,
			     const Geometry::Vec3D& nearPt) 
  /*!
    Calculate the point at the closest point along the line
    to the surface SPtr
    \param HR :: Rule descriptor to intersect
    \param nearPt :: Point ot use to intersect
    \return Points
  */
{
  ELog::RegMethod RegA("LineIntersect","getPoint(HR,Near)");

  IPts.clear();
  HR.populateSurf();
  HR.calcSurfIntersection(ATrack.getOrigin(),
			  ATrack.getDirect(),
			  IPts);
  

  if (IPts.empty())
    throw ColErr::EmptyContainer("Intersect with "+HR.display());
  
  return SurInter::closestPt(IPts,nearPt).Pt;
}

Geometry::Vec3D
LineIntersectVisit::getPoint(const Geometry::Surface* SPtr,
			     const Geometry::Surface* CntlPtr,
			     const int signV) 
  /*!
    Calculate the point at the closest point along the line
    to the surface SPtr
    \param SPtr :: surface to intersect
    \param CntlPtr :: control/guard surface to check intersects
    \param signV :: sign on control surface
    \throw EmptyValue if no intersections possible
    \return Point of intersection
  */
{
  ELog::RegMethod RegA("LineIntersect","getPoint(SPtr,Cntl)");

  IPts.clear();
  SPtr->acceptVisitor(*this);

  // pre-sorted:
  const Geometry::interPoint* iPtr;
  double absD=std::numeric_limits<double>::max();
  for(const Geometry::interPoint& ipt : IPts)
    {
      if ( std::abs(ipt.D)<absD &&
	   (CntlPtr->side(ipt.Pt)*signV<0) )
	{
	  absD=std::abs<double>(ipt.D);
	  iPtr= &ipt;
	}
    }
  if (!iPtr)
    throw ColErr::EmptyValue<void>("LineIntersecVisit::getPoint<"+
				   SPtr->className()+">");

  return iPtr->Pt;
}


const std::vector<Geometry::interPoint>&
LineIntersectVisit::getIntercept(const Geometry::Surface* SPtr) 
  /*!
    Calculate the Points  along the trace
    to the surface SPtr
    \param SPtr :: surface to intersect
    \return vector of points
  */
{
  IPts.clear();
  SPtr->acceptVisitor(*this);
  return IPts;
}

const std::vector<Geometry::interPoint>&
LineIntersectVisit::getIntercept(const HeadRule& HR)
  /*!
    Calculate the Points along the trace
    to the surface SPtr
    \param HR :: head rule to extra surfaces
    \return vector of points
  */
{
  IPts.clear();
  // This sorts IPts
  HR.calcSurfIntersection(ATrack.getOrigin(),
			  ATrack.getDirect(),
			  IPts);
  return IPts;
}

void
LineIntersectVisit::write(std::ostream& OX) const
  /*!
    Debug output to a stream
    \param OX :: output stream
  */
{
  OX<<"Track "<<ATrack<<"\n";
  for(const Geometry::interPoint& ipt : IPts)
    OX<<ipt<<"\n";

  return;
}

}  // NAMESPACE MonteCarlo

