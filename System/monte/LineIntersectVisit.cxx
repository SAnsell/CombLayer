/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   monte/LineIntersectVisit.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#include "GTKreport.h"
#include "OutputLog.h"
#include "mathSupport.h"
#include "support.h"
#include "Triple.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Surface.h"
#include "surfFunctors.h"
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
#include "Torus.h"
#include "Line.h"
#include "SurInter.h"
#include "neutron.h"
#include "Rules.h"
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
  (const MonteCarlo::neutron& N) :
    BaseVisit(),ATrack(N.Pos,N.uVec),neutIndex(N.ID)
  /*!
    Constructor
    \param N :: Neutron to track
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
LineIntersectVisit::setLine(const neutron& N)
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
  ATrack.intersect(PtOut,Surf);
  procTrack(&Surf);
  return;
}

void
LineIntersectVisit::Accept(const Geometry::Plane& Surf)
  /*!
    Process an intersect track
    \param Surf :: Surface to use int line Interesect
  */
{
  ATrack.intersect(PtOut,Surf);
  procTrack(&Surf);
  return;
}

void
LineIntersectVisit::Accept(const Geometry::ArbPoly& Surf)
  /*!
    Process an intersect track
    \param Surf :: Surface to use int line Interesect
  */
{
  ATrack.intersect(PtOut,Surf);
  procTrack(&Surf);
  return;
}

void
LineIntersectVisit::Accept(const Geometry::Cone& Surf)
  /*!
    Process an intersect track
    \param Surf :: Surface to use int line Interesect
  */
{
  ATrack.intersect(PtOut,Surf);
  procTrack(&Surf);
  return;
}

void
LineIntersectVisit::Accept(const Geometry::CylCan& Surf)
  /*!
    Process an intersect track
    \param Surf :: Surface to use int line Interesect
  */
{
  ATrack.intersect(PtOut,Surf);
  procTrack(&Surf);
  return;
}

void
LineIntersectVisit::Accept(const Geometry::Cylinder& Surf)
  /*!
    Process an intersect track
    \param Surf :: Surface to use int line Interesect
  */
{
  ATrack.intersect(PtOut,Surf);
  procTrack(&Surf);
  return;
}

void
LineIntersectVisit::Accept(const Geometry::EllipticCyl& Surf)
  /*!
    Process an intersect track
    \param Surf :: Surface to use int line Interesect
  */
{
  ATrack.intersect(PtOut,Surf);
  procTrack(&Surf);
  return;
}

void
LineIntersectVisit::Accept(const Geometry::Sphere& Surf)
  /*!
    Process an intersect track
    \param Surf :: Surface to use int line Interesect
  */
{
  ATrack.intersect(PtOut,Surf);
  procTrack(&Surf);
  return;
}

void
LineIntersectVisit::Accept(const Geometry::General& Surf)
  /*!
    Process an intersect track
    \param Surf :: Surface to use int line Interesect
  */
{
  ATrack.intersect(PtOut,Surf);
  procTrack(&Surf);
  return;
}

void
LineIntersectVisit::Accept(const Geometry::MBrect& Surf)
  /*!
    Process an intersect track
    \param Surf :: Surface to use int line Interesect
  */
{
  ATrack.intersect(PtOut,Surf);
  procTrack(&Surf);
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

void
LineIntersectVisit::procTrack(const Geometry::Surface* surfID) 
  /*!
    Sorts the PtOut and distances
    with a "closest first" order.
    \param surfID :: surface ID
  */
{
  // Calculate the distances to the points
  // add uncalculated DOut pointes
  for(size_t i=DOut.size();i<PtOut.size();i++)
    {
      const Geometry::Vec3D LP=PtOut[i]-ATrack.getOrigin();
      DOut.push_back(LP.dotProd(ATrack.getDirect()));
    }
  while(SurfIndex.size()!=DOut.size())
    SurfIndex.push_back(surfID);
  // Add stuff from neutReg
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
  if (DOut.empty())
    throw ColErr::EmptyValue<void>("LineIntersecVisit::getDist<"+
				   SPtr->className()+">");

  return DOut[0];
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
  
  for(const double& D : DOut)
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
  if (PtOut.empty())
    throw ColErr::EmptyValue<void>("LineIntersecVisit::getPoint<"+
				   SPtr->className()+">");

  return PtOut.front();
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
  if (PtOut.empty())
    throw ColErr::EmptyValue<void>("LineIntersecVisit::getPoint<"+
				   SPtr->className()+">(near)");
  return SurInter::nearPoint(PtOut,nearPt);
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

  clearTrack();
  HeadRule HRule;
  if (!HRule.procString(RuleStr))
    throw ColErr::InvalidLine("RuleStr:",RuleStr,0);

  HRule.populateSurf();
  //  const bool cFlag=HRule.isValid(ATrack.getOrigin());
  const std::vector<const Geometry::Surface*> SVec=
    HRule.getSurfaces();

  // Check all surfaces
  for(const Geometry::Surface* SPtr : SVec)
    SPtr->acceptVisitor(*this);
  // remove if bit
  //  remove_if(

  if (PtOut.empty())
    throw ColErr::EmptyValue<void>("Intersect with "+RuleStr);
  return SurInter::nearPoint(PtOut,nearPt);
}

Geometry::Vec3D
LineIntersectVisit::getPoint(const Geometry::Surface* SPtr,
			     const Geometry::Surface* CntlPtr,
			     const int sign) 
  /*!
    Calculate the pont at the closest point along the line
    to the surface SPtr
    \param SPtr :: surface to intersect
    \param CntlPtr :: control/guard surface to check intersects
    \param sign :: sign on control surface
    \throw EmptyValue if no intersections possible
    \return Point of intersection
  */
{
  clearTrack();
  SPtr->acceptVisitor(*this);
  std::vector<Geometry::Vec3D>::iterator vc=
    remove_if(PtOut.begin(),PtOut.end(),
	      std::bind(&Geometry::surfaceCheck,sign,CntlPtr,
			std::placeholders::_1));
  PtOut.erase(vc,PtOut.end());

  if (PtOut.empty())
    throw ColErr::EmptyValue<void>("LineIntersecVisit::getPoint<"+
				   SPtr->className()+">");

  return PtOut[0];
}


const std::vector<Geometry::Vec3D>&
LineIntersectVisit::getPoints(const Geometry::Surface* SPtr) 
  /*!
    Calculate the Points  along the trace
    to the surface SPtr
    \param SPtr :: surface to intersect
    \return distance
  */
{
  SPtr->acceptVisitor(*this);
  return PtOut;
}

const std::vector<Geometry::Vec3D>&
LineIntersectVisit::getPoints(const Geometry::Surface* SPtr,
			      const Geometry::Surface* CntlPtr,
			      const int sign)
  /*!
    Calculate the Points along the trace
    to the surface SPtr
    \param SPtr :: surface to intersect
    \param CntlPtr :: surface to test against 
    \param sign :: sign on surface CntlPtr
    \return distance
  */
{
  SPtr->acceptVisitor(*this);
  std::vector<Geometry::Vec3D>::iterator vc=
    remove_if(PtOut.begin(),PtOut.end(),
	      std::bind(&Geometry::surfaceCheck,sign,CntlPtr,
			std::placeholders::_1));

  PtOut.erase(vc,PtOut.end());
  return PtOut;
}


void
LineIntersectVisit::write(std::ostream& OX) const
  /*!
    Debug output to a stream
    \param OX :: output stream
  */
{
  OX<<"Track "<<ATrack<<std::endl;
  for(size_t i=0;i<PtOut.size();i++)
    { 
      OX<<"Pts/Surf["<<i<<"]="<<PtOut[i]
	<<" :: "<<SurfIndex[i]->getName()<<std::endl;
    }
  return;
}

}  // NAMESPACE MonteCarlo

