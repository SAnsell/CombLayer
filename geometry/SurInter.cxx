/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   geometry/SurInter.cxx
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
#include <vector>
#include <deque>
#include <map>
#include <string>
#include <algorithm>
#include <boost/shared_ptr.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "support.h"
#include "mathSupport.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "PolyFunction.h"
#include "PolyVar.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "solveValues.h"
#include "vecOrder.h"
#include "Transform.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Sphere.h"
#include "Cylinder.h"
#include "Cone.h"
#include "General.h"
#include "Line.h"
#include "Intersect.h"
#include "Pnt.h"
#include "SglLine.h"
#include "DblLine.h"
#include "Circle.h"
#include "Ellipse.h"
#include "SurInter.h"

#include "Debug.h"

namespace SurInter
{

template<>
Geometry::Intersect*
calcIntersect(const Geometry::Plane& PlnA,const Geometry::Plane& PlnB)
  /*!
    Calculate the intersection object between two planes
    \param PlnA :: First Plane object
    \param PlnB :: Second Plane object 
    \return Intersect object [SglLine/0]
  */
{
  ELog::RegMethod RegA("SurInter","calcIntersect<Plane,Plane>");
  Geometry::Line LP;
  if (!LP.setLine(PlnA,PlnB))
    return 0;
  
  return new Geometry::SglLine(LP.getOrigin(),LP.getDirect());
}


template<>
Geometry::Intersect*
calcIntersect(const Geometry::Sphere& SphA,const Geometry::Sphere& SphB)
  /*!
    Calculate the intersection object between two planes
    \param SphA :: First Sphere object
    \param SphB :: Second Sphere object 
    \return Intersect object [ Circle/0 ]
  */
{
  ELog::RegMethod RegA("SurInter","calcIntersect<Sphere,Sphere>");

  const double RA=SphA.getRadius();
  const double RB=SphB.getRadius();
  const Geometry::Vec3D& pntA=SphA.getCentre();
  const Geometry::Vec3D& pntB=SphB.getCentre();
  const double D=pntA.Distance(pntB);
  if (D>RA+RB)
    return 0;

  const Geometry::Vec3D normL=(pntB-pntA).unit();
  
  // Single point
  if (fabs(D-(RA+RB))<Geometry::zeroTol)
    return new Geometry::Pnt(pntA+normL*RA);
  
  const double xfrac=(RA*RA-RB*RB+D*D)/(2*D);
  return new Geometry::Circle(pntA+normL*xfrac,normL,sqrt(RA*RA-xfrac*xfrac));
}

template<>
Geometry::Intersect*
calcIntersect(const Geometry::Sphere& Sph,const Geometry::Plane& Pln)
  /*!
    Calculate the intersection object  between two objects
    \param Sph :: Sphere object
    \param Pln :: Plane object 
    \return Intersect object
  */
{
  // First find out if we intersect at all:
  const Geometry::Vec3D& Centre=Sph.getCentre();
  const double Radius=Sph.getRadius();
  const double D=Pln.distance(Centre);       // Unsigned distance
  if (D>Radius)         // out of sphere
    return 0;
    
  const Geometry::Vec3D& N=Pln.getNormal();
  Geometry::Vec3D CirCen=Centre+N*D;        // Centre of sphere
  if (Pln.distance(CirCen)>D)
    CirCen=Centre-N*D;

  // This can go VERY slightly negative:
  const double cRad=sqrt(fabs(Radius*Radius-D*D));
  if (cRad>Geometry::zeroTol)
    return new Geometry::Circle(CirCen,N,cRad);
    
  // So it is a point:
  return new Geometry::Pnt(CirCen);
}

template<>
Geometry::Intersect*
calcIntersect(const Geometry::Cylinder& Cyl,const Geometry::Plane& Pln)
  /*!
    Calculate the intersection object  between two objects
    This follows the geometricTools tutorial
    \param Cyl :: Cylinder object
    \param Pln :: Plane object 
    \return Intersect object
  */
{
  ELog::RegMethod RegA("SurInter","calcIntersect<Cyl,Plane>");

  // First find out if we intersect at all:
  const Geometry::Vec3D& N=Pln.getNormal();
  const Geometry::Vec3D& D=Cyl.getNormal();
  const Geometry::Vec3D& C=Cyl.getCentre();
  
  const double r=Cyl.getRadius();
  const double cosTheta=N.dotProd(D);

  // NO INTERSECTION:
  const double sDist=Pln.distance(C);
  // LINE INTERSECTION:
  if (fabs(cosTheta)<Geometry::zeroTol)
    {
      // Consider parallel plane:
      if (fabs(sDist)>r) return 0;
      const Geometry::Vec3D lNorm=N*D;
      // Get closest point to the plane:
      const Geometry::Vec3D IPt=C-N*sDist;
      const double mD=sqrt(r*r-sDist*sDist);
      // Double line:
      if (mD>Geometry::zeroTol)
	return new Geometry::DblLine(IPt+lNorm*mD,IPt-lNorm*mD,D);
      else // Single Line:
	return new Geometry::SglLine(IPt,D);
    }
  
  // CIRCLE INTERSECTION:
  if (fabs(1.0-fabs(cosTheta))<Geometry::zeroTol)
    return new Geometry::Circle(C-N*sDist,N,r);

  // ELLIPSE INTERSECTION:
  // angle between plane norm and cylinder
  Geometry::Vec3D major=D-N*cosTheta;
  Geometry::Vec3D minor=N*major;
  major.makeUnit();
  minor.makeUnit();
  major*= r/fabs(cosTheta);          
  minor*= r;                 //  r : since all out of plane
      
  return new Geometry::Ellipse(C-D*(sDist/cosTheta),minor,major,N);
}

std::vector<Geometry::Vec3D> 
processPoint(const Geometry::Surface* ASPtr,
	     const Geometry::Surface* BSPtr,
	     const Geometry::Surface* CSPtr)
  /*! 
     Since Surface is abstract so the vector is 
     of derived classes. This determines the derived class
     name and case the surface (dynamic_cast) and uses this
     object to dispatch to the intersection.
     \param ASPtr :: Surface to use
     \param BSPtr :: Surface to use
     \param CSPtr :: Surface to use
     \returns vector of Intersections
  */
{
  ELog::RegMethod RegA("SurInter","processPoint");

  std::vector<Geometry::Vec3D> Out;
  const Geometry::Surface* SVec[3]={ASPtr,BSPtr,CSPtr};
  const Geometry::Quadratic* QVec[3]={0,0,0};
  const Geometry::Plane* PVec[3]={0,0,0};

  if (ASPtr==BSPtr || CSPtr==BSPtr || ASPtr==CSPtr)
    return Out;

  int planeN(0);       
  int nonPlane(0);
  for(int i=0;i<3;i++)
    { 
      QVec[i]=dynamic_cast<const Geometry::Quadratic*>(SVec[i]);
      if (!QVec[i]) 
        {
	  if (SVec[i])
	    ELog::EM<<"No special for [yet] "
		    <<SVec[i]->className()<<ELog::endErr;
	  else
	    ELog::EM<<"Null surface passed Index:"<<i<<ELog::endErr;
	  return Out;
	}
      PVec[planeN]=dynamic_cast<const Geometry::Plane*>(SVec[i]);
      if (PVec[planeN]) 
	planeN++;
      else
	nonPlane=i;
    }
  
  if (planeN==3)          // All Plane:
    {
      return makePoint(PVec[0],PVec[1],PVec[2]);
    }
  if (planeN==2)        // Make line + intersect:
    {
      Geometry::Line Lx;
      if (Lx.setLine(*PVec[0],*PVec[1]))
	Lx.intersect(Out,*QVec[nonPlane]);
      return Out;
    }
  Out=makePoint(QVec[0],QVec[1],QVec[2]);
  return Out;
}

std::vector<Geometry::Vec3D>
makePoint(const Geometry::Plane* A,
	  const Geometry::Plane* B,const Geometry::Plane* C)
  /*!
    Calculate the intersection between three planes
    \param A :: Plane pointer 
    \param B :: Plane pointer 
    \param C :: Plane pointer 
    \return Vector of intersection points [0/1]
  */
{
  ELog::RegMethod RegA("SurInter","makePoint(P,P,P)");
  std::vector<Geometry::Vec3D> Out;
  if (!A || !B || !C)
    {
      ELog::EM<<"Failed on Points "<<ELog::endErr;
      return Out;
    }

  Geometry::Line Lx;
  if (Lx.setLine(*A,*B))
    {
      Lx.intersect(Out,*C);
    }
  return Out;  
}


std::vector<Geometry::Vec3D>
makePoint(const Geometry::Plane* A,
	  const Geometry::Plane* B,
	  const Geometry::Cylinder* C)
  /*!
    Calculate the intersection between three planes
    \param A :: Plane pointer 
    \param B :: Plane pointer 
    \param C :: Cylinder pointer 
    \return Vector of intersection points [0/1/2]
  */
{
  ELog::RegMethod RegA("SurInter","makePoint(P,P,C)");

  std::vector<Geometry::Vec3D> Out;
  Geometry::Line Lx;
  if (Lx.setLine(*A,*B))
    {
      Lx.intersect(Out,*C);  // this could be tested 
    }
  return Out;  
}

std::vector<Geometry::Vec3D>
makePoint(const Geometry::Plane* A,const Geometry::Plane* B,
		    const Geometry::Quadratic* C)
  /*!
    Calculate the intersection between three planes
    \param A :: Plane pointer 
    \param B :: Plane pointer 
    \param C :: Quadratic
    \return Vector of intersection points [0/1/2]
  */
{
  ELog::RegMethod RegA("SurInter","makePoint(P,P,Q)");

  std::vector<Geometry::Vec3D> Out;
  Geometry::Line Lx;
  if (Lx.setLine(*A,*B))
    {
      Lx.intersect(Out,*C);  // this could be tested 
    }
  return Out;  
}

std::vector<Geometry::Vec3D>
makePoint(const Geometry::Quadratic* A,const Geometry::Quadratic* B,
	  const Geometry::Quadratic* C)
  /*!
    Calculate the intersection between three quadratic surfaces
    \param A :: Quadratic pointer
    \param B :: Quadratic pointer
    \param C :: Quadratic pointer 
    \return the points found
  */
{
  ELog::RegMethod RegA("SurInter","makePoint(Q,Q,Q)");
  
  std::vector<Geometry::Vec3D> Out;
  if (!A || !B || !C || A==B || C==B || A==C)
    {
      ELog::EM<<"Quadratics not valid"<<ELog::endErr;
      return Out;
    }

  mathLevel::PolyVar<3> FXYZ;
  mathLevel::PolyVar<3> GXYZ;
  mathLevel::PolyVar<3> HXYZ;
  // Addition to avoid missing varaibles 
  FXYZ.makeTriplet(A->copyBaseEqn());
  GXYZ.makeTriplet(B->copyBaseEqn());
  HXYZ.makeTriplet(C->copyBaseEqn());

  mathLevel::solveValues SV;
  SV.setEquations(FXYZ,GXYZ,HXYZ);
  SV.getSolution();

  return SV.getAnswers();
}


int
getMidPoint(const Geometry::Surface* ASPtr,const Geometry::Surface* BSPtr,
	    const Geometry::Surface* CSPtr,Geometry::Vec3D& OutPt)
  /*! 
    Since Surface is abstract so the vector is 
    of derived classes. This determines the derived class
    name and case the surface (dynamic_cast) and uses this
    object to dispatch to the mid points
    
    It effectively uses a set of special rules for cases like
    two parallel planes and a cylinder.
    
    \param ASPtr :: Surface to use
    \param BSPtr :: Surface to use
    \param CSPtr :: Surface to use
    \param OutPt :: Points found between surfaces
    \returns Number of vectors that contribute to OutPt
  */
{
  const Geometry::Surface* SVec[3]={ASPtr,BSPtr,CSPtr};
  const Geometry::Quadratic* QVec[3]={0,0,0};
  const Geometry::Plane* PVec[3]={0,0,0};
  const Geometry::Sphere* SpVec[3]={0,0,0};
  const Geometry::Cylinder* CVec[3]={0,0,0};
  int planeN(0);       
  int sphereN(0);
  int cylN(0);
  for(int i=0;i<3;i++)
    { 
      QVec[i]=dynamic_cast<const Geometry::Quadratic*>(SVec[i]);
      if (!QVec[i]) 
        {
	  if (SVec[i])
	    ELog::EM<<"No special for [yet] " 
		    <<SVec[i]->className()<<ELog::endErr;
	  else
	    ELog::EM<<"Null surface for index :"<<i
		    <<ELog::endErr;

	  return 0;
	}

      if ( (PVec[planeN]=dynamic_cast<const Geometry::Plane* > (SVec[i])) )
	planeN++;
      
      if ( (SpVec[sphereN]=dynamic_cast<const Geometry::Sphere* > (SVec[i])) )
	sphereN++;
      
      if ( (CVec[cylN]=dynamic_cast<const Geometry::Cylinder* > (SVec[i])) )
	cylN++;
    }
  
  typedef boost::shared_ptr<const Geometry::Intersect> ipTYPE;
  if (planeN==2 && sphereN==1)        // Make line + intersect:
    {
      ipTYPE CpA(calcIntersect(*SpVec[0],*PVec[0]));
      ipTYPE CpB(calcIntersect(*SpVec[0],*PVec[1]));
      if (CpA && CpB && CpA->hasCentre() &&
	  CpB->hasCentre())
	{
	  OutPt=(CpA->centre()+CpB->centre())/2.0;
	  return 1;
	}
      return 0; 
    }      

  if (planeN==2 && cylN==1)        // Make intersect
    {
      ipTYPE CpA(calcIntersect(*CVec[0],*PVec[0]));
      ipTYPE CpB(calcIntersect(*CVec[0],*PVec[1]));
      if (CpA && CpB && CpA->hasCentre() &&
	  CpB->hasCentre())
	{
	  OutPt=(CpA->centre()+CpB->centre())/2.0;
	  return 1;
	}
      return 0; 
    }      
	
  return 0;
}

Geometry::Vec3D
calcCentroid(const Geometry::Surface* pA,const Geometry::Surface* pB,
	     const Geometry::Surface* pC,const Geometry::Surface* pX,
	     const Geometry::Surface* pY,const Geometry::Surface* pZ)
  /*!
    Calculate the centroid between the two intersecting corners
    \param pA :: Plane on first corner
    \param pB :: Plane on first corner
    \param pC :: Plane on first corner
    \param pX :: Plane on second corner
    \param pY :: Plane on second corner
    \param pZ :: Plane on second corner
    \return Centroid
   */
{
  ELog::RegMethod RegA("SurInter","calcCentroid");

  std::vector<Geometry::Vec3D> OutVec;
  OutVec=SurInter::processPoint(pA,pB,pC);

  if (OutVec.empty())
    {
      ELog::EM<<"Failed to calculate first corner: Check planes intersect"<<ELog::endErr;
      return Geometry::Vec3D(0,0,0);
    }
	
  Geometry::Vec3D OutPt=OutVec.front();
  OutVec=SurInter::processPoint(pX,pY,pZ);
  if (OutVec.empty())
    {
      ELog::EM<<"Failed to calculate second corner: Check planes intersect"<<ELog::endErr;
      return Geometry::Vec3D(0,0,0);
    }
  OutPt+=OutVec.front();
  OutPt/=2.0;
  return OutPt;
}

Geometry::Vec3D
getPoint(const Geometry::Surface* A,
	 const Geometry::Surface* B,
	 const Geometry::Surface* C)
  /*! 
    Calculate the single intersection point of a set of surfaces [assuming only one]
    \param A :: Surface to use
    \param B :: Surface to use
    \param C :: Surface to use
    \returns Intersection point
  */
{
  ELog::RegMethod RegA("SurInter","getPoint");
  std::vector<Geometry::Vec3D> Out=processPoint(A,B,C);
  if (Out.size()!=1)
    throw ColErr::MisMatch<size_t>(Out.size(),1,"Out size");
  
  return Out.back();

}

Geometry::Vec3D
getPoint(const Geometry::Surface* A,
	 const Geometry::Surface* B,
	 const Geometry::Surface* C,
	 const Geometry::Vec3D& nearPt)
  /*! 
    Calculate the single intersection point of a set of surfaces [assuming only one]
    \param A :: Surface to use
    \param B :: Surface to use
    \param C :: Surface to use
    \param nearPt :: Closes point to the set
    \returns Intersection point
  */
{
  ELog::RegMethod RegA("SurInter","getPoint(near)");

  std::vector<Geometry::Vec3D> Out=processPoint(A,B,C);
  if (Out.empty())
    throw ColErr::IndexError<size_t>(Out.size(),1,"Out size");
  
  return nearPoint(Out,nearPt);

}

Geometry::Vec3D
nearPoint(const std::vector<Geometry::Vec3D>& Pts,
	  const Geometry::Vec3D& Target)
  /*!
    Calculate the nearest point (from a find set)
    \param Pts :: List of points
    \param Target :: Target point
    \return Point
   */
{
  ELog::RegMethod RegA("SurInter","nearPoint");
  
  if (Pts.empty())
    throw ColErr::EmptyValue<void>("Points vector");
  
  std::vector<Geometry::Vec3D>::const_iterator vc,mVal;
  double Dist(1e38);
  for(vc=Pts.begin();vc!=Pts.end();vc++)
    {
      const double D=vc->Distance(Target);
      if (D<Dist)
	{
	  Dist=D;
	  mVal=vc;
	}
    }
  return *mVal;
}



}  // NAMESPACE SurInter


