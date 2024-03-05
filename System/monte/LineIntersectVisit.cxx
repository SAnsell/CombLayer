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
  std::vector<Geomtery::Vec3D> PtVec;
  ATrack.intersect(PtVec,Surf);
  procTrack(PVec,&Surf);
  return;
}

void
LineIntersectVisit::Accept(const Geometry::Plane& Surf)
  /*!
    Process an intersect track
    \param Surf :: Surface to use int line Interesect
  */
{
  std::vector<Geomtery::Vec3D> PtVec;
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
  std::vector<Geomtery::Vec3D> PtVec;
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
  std::vector<Geomtery::Vec3D> PtVec;
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
  std::vector<Geomtery::Vec3D> PtVec;
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
  std::vector<Geomtery::Vec3D> PtVec;
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
  std::vector<Geomtery::Vec3D> PtVec;
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
  std::vector<Geomtery::Vec3D> PtVec;
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
  std::vector<Geomtery::Vec3D> PtVec;
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
  std::vector<Geomtery::Vec3D> PtVec;
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
  const std::set<const Geometry::Surface*> SurfSet=HR.getSurfaces();
  for(const Geometry::Surface* SPtr : SurfSet)
    {
      SPtr->acceptVisitor(*this);      
      // construct local LineIntersect
      //      LineIntersectVisit LI(ATrack);

      LI.getPoints(SPtr);

      const int NS=SPtr->getName();	    
      for(size_t index=0;index<LI.PtVec.size();index++)
	{
	  const Geometry::Vec3D& Pt(LI.PtVec[index]);
	  
	  const int pAB=HR.isValid(Pt,NS);
	  const int mAB=HR.isValid(Pt,-NS);

	  if (pAB!=mAB)  // out going positive surface
	    {
	      PtVec.push_back(Pt);
	      distVec.push_back(LI.distVec[index]);
	      surfVec.push_back(LI.surfVec[index]);
	    }
	}
    }
  // remove non-unique ?? / sort
  
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
  for(const Geometry::Vec3d& Pt : PtVec)
    {
      const Geometry::Vec3D LP=Pt-ATrack.getOrigin();
      const double D=LP.dotProd(ATrack.getDirect());
      const int SN=(surfID) ? surfID->getName() : 0;
      IPts.push_back(Geometry::interPoint(LP,D,SN,surfID,0));
    }  
  return;
}


// ACCESSORS:
double
LineIntersectVisit::getDist(const Geometry::Surface* SPtr) 
  /*!
    Calculate the distance to the closest point along the line
    to the surface SPtr
    \param SPtr :: surface to intersect
    \return distance
  */
{
  SPtr->acceptVisitor(*this);
  
  if (distVec.empty())
    throw ColErr::EmptyValue<void>("LineIntersecVisit::getDist<"+
				   SPtr->className()+">");

  return distVec[0];
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
  
  for(const double& D : distVec)
    if (D>Geometry::zeroTol)
      return D;
  
  throw ColErr::EmptyValue<void>("LineIntersecVisit::getDist<"+
				 SPtr->className()+">");
  
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
  if (PtVec.empty())
    throw ColErr::EmptyValue<void>("LineIntersecVisit::getPoint<"+
				   SPtr->className()+">");

  return PtVec.front();
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
  if (PtVec.empty())
    throw ColErr::EmptyValue<void>("LineIntersecVisit::getPoint<"+
				   SPtr->className()+">(near)");
  return SurInter::nearPoint(PtVec,nearPt);
}

Geometry::Vec3D
LineIntersectVisit::getPoint(const std::string& RuleStr,
			     const Geometry::Vec3D& nearPt) 
  /*!
    Calculate the point at the closest point along the line
    to the surface SPtr
    \param RuleStr :: Rule descriptor to intersect
    \param nearPt :: Point ot use to intersect
    \return Points
  */
{
  ELog::RegMethod RegA("LineIntersect","getPoint(String,Near)");

  HeadRule HRule;
  if (!HRule.procString(RuleStr))
    throw ColErr::InvalidLine("RuleStr:",RuleStr,0);

  return getPoint(HRule,nearPt);
}

Geometry::Vec3D
LineIntersectVisit::getPoint(HeadRule& HRule,
			     const Geometry::Vec3D& nearPt) 
  /*!
    Calculate the point at the closest point along the line
    to the surface SPtr
    \param RuleStr :: Rule descriptor to intersect
    \param nearPt :: Point ot use to intersect
    \return Points
  */
{
  ELog::RegMethod RegA("LineIntersect","getPoint(HR,Near)");
  
  clearTrack();
  HRule.populateSurf();
  const std::set<const Geometry::Surface*> SSet=
    HRule.getSurfaces();

  // Check all surfaces
  for(const Geometry::Surface* SPtr : SSet)
    SPtr->acceptVisitor(*this);

  if (PtVec.empty())
    throw ColErr::EmptyValue<void>("Intersect with "+HRule.display());
  return SurInter::nearPoint(PtVec,nearPt);
}

Geometry::Vec3D
LineIntersectVisit::getPoint(const Geometry::Surface* SPtr,
			     const Geometry::Surface* CntlPtr,
			     const int signV) 
  /*!
    Calculate the pont at the closest point along the line
    to the surface SPtr
    \param SPtr :: surface to intersect
    \param CntlPtr :: control/guard surface to check intersects
    \param signV :: sign on control surface
    \throw EmptyValue if no intersections possible
    \return Point of intersection
  */
{
  clearTrack();
  SPtr->acceptVisitor(*this);

  for(const Geometry::Vec3D& Pt : PtVec)
    if (CntlPtr->side(Pt)*signV>0) return Pt;

  throw ColErr::EmptyValue<void>("LineIntersecVisit::getPoint<"+
				 SPtr->className()+">");
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

const std::vector<Geometry::Vec3D>&
LineIntersectVisit::getPoints(const Geometry::Surface* SPtr,
			      const Geometry::Surface* CntlPtr,
			      const int signV)
  /*!
    Calculate the Points along the trace
    to the surface SPtr
    \param SPtr :: surface to intersect
    \param CntlPtr :: surface to test against 
    \param signV :: sign on surface CntlPtr
    \return vector of points
  */
{
  SPtr->acceptVisitor(*this);

  std::vector<Geometry::Vec3D>::iterator vc=
    remove_if(PtVec.begin(),PtVec.end(),
	      [&](const Geometry::Vec3D& Pt) -> bool
	      {
		return CntlPtr->side(Pt)*signV<0;
	      });
  PtVec.erase(vc,PtVec.end());
  return PtVec;
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
  this->Accept(HR);
  return IPts;
}

void
LineIntersectVisit::write(std::ostream& OX) const
  /*!
    Debug output to a stream
    \param OX :: output stream
  */
{
  OX<<"Track "<<ATrack<<std::endl;
  for(size_t i=0;i<PtVec.size();i++)
    { 
      OX<<"Pts/Surf["<<i<<"]="<<PtVec[i]
	<<" :: "<<surfVec[i]->getName()<<std::endl;
    }
  return;
}

}  // NAMESPACE MonteCarlo

