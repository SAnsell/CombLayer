/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geometry/SurInter.cxx
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
#include <complex>
#include <vector>
#include <deque>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "PolyFunction.h"
#include "PolyVar.h"
#include "Vec3D.h"
#include "Vec2D.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "M2.h"
#include "M3.h"
#include "solveValues.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Sphere.h"
#include "Cylinder.h"
#include "Cone.h"
#include "Line.h"
#include "Intersect.h"
#include "Pnt.h"
#include "SglLine.h"
#include "DblLine.h"
#include "Circle.h"
#include "Ellipse.h"
#include "HeadRule.h"
#include "interPoint.h"
#include "surfIndex.h"
#include "LineIntersectVisit.h"
#include "SurInter.h"


namespace SurInter
{

Geometry::Vec3D
getLinePoint(const Geometry::Vec3D& Origin,
	     const Geometry::Vec3D& LAxis,
	     const int SNum,
	     const Geometry::Vec3D& closePt)
  /*!
    Calculate the intersection of a line with a surface
    If there are multiple points of intersection find
    the point closest to closePT
    \param Origin :: Start of line
    \param LAxis :: Axis of line
    \param SNum :: Surface number
    \param closePt :: Point to determine closest solution
    \return Line/Surf intersection
  */
{
  ELog::RegMethod RegA("SurInter[F]","lineSurfPoint(int,close)");

  ModelSupport::surfIndex& SurI=
    ModelSupport::surfIndex::Instance();
  const Geometry::Surface* SPtr=SurI.getSurf(SNum);
  return getLinePoint(Origin,LAxis,SPtr,closePt);
}

Geometry::Vec3D
getLinePoint(const Geometry::Vec3D& Origin,
	     const Geometry::Vec3D& LAxis,
	     const int SNum)
  /*!
    Calculate the intesection of a line with a plane
    (found by index number) : Throw if not
    \param Origin :: Start of line
    \param LAxis :: Axis of line
    \param SNum :: Surface number
    \return Line/Surf intersection
  */
{
  ELog::RegMethod RegA("SurInter[F]","lineSurfPoint(int)");

  ModelSupport::surfIndex& SurI=
    ModelSupport::surfIndex::Instance();
  // must be a plane to get one point
  const Geometry::Plane* PPtr=
    SurI.realSurf<Geometry::Plane>(std::abs(SNum));
  if (!PPtr)
    throw ColErr::InContainerError<int>(SNum,"Plane Surface");
  return getLinePoint(Origin,LAxis,PPtr);
}

  
Geometry::Vec3D
getLinePoint(const Geometry::Vec3D& Origin,const Geometry::Vec3D& N,
	     const HeadRule& mainHR,const Geometry::Vec3D& closePt)
/*!
    Given a line (origin:N) find the intersects wiht MainHR that
    satisfy sndHR
    \param Origin :: Origin of line
    \param N :: Normal of line
    \param mainHR :: Main headRule    
    \param sndHR :: Secondary/ Bridge rule
   */
{
  ELog::RegMethod RegA("SurInter[F]","getLinePoint(HR,closePt)");

  std::vector<Geometry::interPoint> IPts;
  mainHR.calcSurfIntersection(Origin,N,IPts);
  if (IPts.empty())
    throw ColErr::InContainerError<std::string>
	(mainHR.display(),"HeadRule / Line does not intersect");
  
  return nearPoint(IPts,closePt);
}

Geometry::Vec3D
getLinePoint(const Geometry::Vec3D& Origin,
	     const Geometry::Vec3D& N,
	     const HeadRule& mainHR,
	     const HeadRule& sndHR)
/*!
    Given a line (origin:N) find the intersects wiht MainHR that
    satisfy sndHR
    \param Origin :: Origin of line
    \param N :: Normal of line
    \param mainHR :: Main headRule    
    \param sndHR :: Secondary/ Bridge rule
   */
{
  ELog::RegMethod RegA("SurInter[F]","getLinePoint(HR,HR)");
  
  std::vector<Geometry::interPoint> IPts;

  mainHR.calcSurfIntersection(Origin,N,IPts);
  Geometry::Vec3D out;
  bool flag(0);
  if (sndHR.hasRule())
    {
      for(const Geometry::interPoint& ipt : IPts)
        {
          if (flag && sndHR.isValid(ipt.Pt))
	    throw ColErr::MisMatch<size_t>
	      (IPts.size(),1,"Too points in intersect (sndHR)");
	  flag=1;
	  out=ipt.Pt;
        }
      if (flag) return out;
      throw ColErr::EmptyContainer("No points in intersect system");
    }

  if (IPts.size()!=1)
    throw ColErr::MisMatch<size_t>
      (IPts.size(),1,"Incorrect points in intersect");
  return IPts.front().Pt;
}

Geometry::Vec3D
getLinePoint(const Geometry::Vec3D& Origin,
             const Geometry::Vec3D& Axis,
	     const Geometry::Plane* PPtr)
  /*!
    Calculate the line though a plane
    This is a specialization of getLinePoint because
    a plane will only have one (at most) intersection points

    \param Origin :: Origin of the line
    \param Axis :: axis direction
    \param PPtr :: plane object
    \return Point on intersect
  */
{
  MonteCarlo::LineIntersectVisit trackLine(Origin,Axis);
  return trackLine.getPoint(PPtr);
}

Geometry::Vec3D
getLinePoint(const Geometry::Vec3D& Origin,
             const Geometry::Vec3D& Axis,
	     const Geometry::Surface* SPtr,
             const Geometry::Vec3D& NPoint)
  /*!
    Calculate the line though a surface
    \param Origin :: Origin of the line
    \param Axis :: axis direction
    \param SPtr :: surface point
    \param NPoint :: nearest point
    \return Point on intersect
  */
{
  MonteCarlo::LineIntersectVisit trackLine(Origin,Axis);
  return trackLine.getPoint(SPtr,NPoint);
}

Geometry::Vec3D
getForwardPoint(const Geometry::Vec3D& Origin,
		const Geometry::Vec3D& LAxis,
		const int SNum)
  /*!
    Calculate the intesection of a line with a surface
    (found by index number) which is in the forward
    (LAxis direction)
    \param Origin :: Start of line
    \param LAxis :: Axis of line
    \param SNum :: Surface number
    \return Line/Surf intersection
  */
{
  ELog::RegMethod RegA("SurInter[F]","lineSurfPoint(int)");

  ModelSupport::surfIndex& SurI=
    ModelSupport::surfIndex::Instance();
  // must be a plane to get one point
  const Geometry::Plane* PPtr=
    SurI.realSurf<Geometry::Plane>(std::abs(SNum));
  if (!PPtr)
    throw ColErr::InContainerError<int>(SNum,"Plane Surface");
  return getLinePoint(Origin,LAxis,PPtr);
}
  
double
getLineDistance(const Geometry::Vec3D& Origin,
             const Geometry::Vec3D& Axis,
	     const Geometry::Plane* PPtr)
  /*!
    Calculate the line though a plane
    This is a specialization of getLinePoint because
    a plane will only have one (at most) intersection points

    \param Origin :: Origin of the line
    \param Axis :: axis direction
    \param PPtr :: plane object
    \return Distance between Origin and intersect
  */
{
  MonteCarlo::LineIntersectVisit trackLine(Origin,Axis);
  return trackLine.getDist(PPtr);
}

double
getLineDistance(const Geometry::Vec3D& Origin,
             const Geometry::Vec3D& Axis,
	     const Geometry::Surface* SPtr,
             const Geometry::Vec3D& NPoint)
  /*!
    Calculate the line though a surface
    \param Origin :: Origin of the line
    \param Axis :: axis direction
    \param SPtr :: surface point
    \param NPoint :: nearest point
    \return Distance to point
  */
{
  MonteCarlo::LineIntersectVisit trackLine(Origin,Axis);
  Geometry::Vec3D Pt=trackLine.getPoint(SPtr,NPoint);
  const double R=Pt.Distance(Origin);
  // calc direction
  Pt-=Origin;
  return (Pt.dotProd(Axis)>0.0) ? R : -R;
}

   
template<>
Geometry::Intersect*
calcIntersect(const Geometry::Plane& PlnA,
	      const Geometry::Plane& PlnB)
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
calcIntersect(const Geometry::Sphere& SphA,
	      const Geometry::Sphere& SphB)
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

template<>
Geometry::Intersect*
calcIntersect(const Geometry::Cone& Cne,
	      const Geometry::Plane& Pln)
  /*!
    Calculate the intersection object between two objects
    This follows the geometricTools tutorial
    \param Cne :: Cone object
    \param Pln :: Plane object 
    \return Intersect object
  */
{
  ELog::RegMethod RegA("SurInter","calcIntersect<Cone,Plane>");

  // First find out if we intersect at all:

  const Geometry::Vec3D& cD=Cne.getNormal();
  const Geometry::Vec3D& cK=Cne.getCentre();
  const double cAlpha=Cne.getCosAngle();


  const Geometry::Vec3D& pNorm=Pln.getNormal(); 
  const Geometry::Vec3D pU=pNorm.crossNormal();
  const Geometry::Vec3D pV=pNorm*pU;

  // need a point on the plane :
  const Geometry::Vec3D pC=
    SurInter::getLinePoint(cK,cD,&Pln);
  ELog::EM<<"PC == "<<pC<<ELog::endDiag;
  const Geometry::Vec3D delta=pC-cK;
  const Geometry::Matrix<double> I(3,3,1);
  const Geometry::Matrix<double> M=
      cD.outerProd(cD)-I*(cAlpha*cAlpha);

  // element of the conic matrix:
  const double c1 = pU.dotProd(M * pU);
  const double c2 = pU.dotProd(M * pV);
  const double c3 = pV.dotProd(M * pV);
  const double c4 = delta.dotProd(M * pU);
  const double c5 = delta.dotProd(M * pV);
  const double c6 = delta.dotProd(M * delta);

  Geometry::M3<double> CM
    ({{c1,c2,c4},
      {c2,c3,c5},
      {c4,c5,c6}});
  //  ELog::EM<<"CM == "<<CM<<ELog::endDiag;
  // rotation matrix
  Geometry::M2<double> MR(c1,c2,c2,c3);
  Geometry::Vec2D cTrans(c4,c5);

  MR.constructEigen();
  Geometry::M2<double> R=MR.getEigVectors();

  Geometry::M2<double> Rprime=R.prime();
  Geometry::M2<double> lambda=MR.getEigValues();

  Geometry::M2<double> CRcheck=R*lambda*Rprime;
  lambda.invert();
  Geometry::Vec2D t=R*(lambda*(Rprime*cTrans));
  
  t*=-1.0;

  // hermician matrix
  const Geometry::M3<double> H
    ({{R.get(0,0),R.get(0,1),t[0]},
      {R.get(1,0),R.get(1,1),t[1]},
      {0.0,0.0,1.0}});

  const Geometry::M3<double> Hprime(H.prime());
  Geometry::M3<double> conicalM=Hprime*(CM*H);
  const double aRadius=
    std::sqrt(-conicalM.get(2,2)/conicalM.get(0,0));
  const double bRadius=
    std::sqrt(-conicalM.get(2,2)/conicalM.get(1,1));

  const Geometry::Vec3D eCentre(pU*t.X()+pV*t.Y()+pC);

  const Geometry::Vec3D aM(R.get(0,0),R.get(1,0),0.0);
  const Geometry::Vec3D bM(R.get(1,0),R.get(1,1),0.0);
  Geometry::Vec3D pUU=aM.getInBasis(pU,pV,pNorm);
  Geometry::Vec3D pVV=bM.getInBasis(pU,pV,pNorm);
  return new Geometry::Ellipse(eCentre,pUU*aRadius,pVV*bRadius,pNorm);
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
	  const Geometry::Plane* B,
	  const Geometry::Plane* C)
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
  
  typedef std::shared_ptr<const Geometry::Intersect> ipTYPE;
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
	 const int signV,const Geometry::Surface* Control)
  /*! 
    Calculate the single intersection point of a set of surfaces [assuming only one]
    \param A :: Surface to use
    \param B :: Surface to use
    \param C :: Surface to use
    \param signV :: Sign of control surface
    \param Control :: Control surface
    \returns Intersection point
  */
{
  ELog::RegMethod RegA("SurInter","getPoint(Control)");

  if (!Control) return getPoint(A,B,C);

  const std::vector<Geometry::Vec3D> Out=
    processPoint(A,B,C);
  for(const Geometry::Vec3D& Pt : Out)
    if (Control->side(Pt)*signV>0) return Pt;

  throw ColErr::MisMatch<size_t>(Out.size(),1,"No matching points in Out");
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
  if (Pts.size()==1)
    return Pts.front();

  double Dist(1e38);
  Geometry::Vec3D Out;
  for(const Geometry::Vec3D& Pt : Pts)
    {
      const double D=Pt.Distance(Target);
      if (D<Dist)
	{
	  Dist=D;
	  Out=Pt;
	}
    }
  return Out;
}

Geometry::Vec3D
nearPoint(const std::vector<Geometry::interPoint>& IPts,
	  const Geometry::Vec3D& Target)
  /*!
    Calculate the nearest point (from a find set)
    \param IPts :: List of points
    \param Target :: Target point
    \return Point
   */
{
  ELog::RegMethod RegA("SurInter","nearPoint");

  // throw is no intercept / IPts empty
  return closestPt(IPts,Target).Pt;
}


const Geometry::interPoint&
closestPt(const std::vector<Geometry::interPoint>& IPVec,
	  const Geometry::Vec3D& AimPt)
  /*!
    Detemine the point that is closest 
    \param AimPt :: Aiming point
    \param IPvec :: Vector of intesections
    \return interPoint 
  */
{
  ELog::RegMethod RegA("SurInter","closestPt");

  if (IPVec.empty())
    throw ColErr::EmptyContainer("No points found");

  double DMax(std::numeric_limits<double>::max());
  const Geometry::interPoint* iPtr(nullptr);
  for(const Geometry::interPoint& IP : IPVec)
    {
      const double testD=AimPt.Distance(IP.Pt);
      if (testD<DMax)
	{
	  iPtr= &IP;
	  DMax=testD;
	}
    }
  return *iPtr;
}

std::pair<Geometry::Vec3D,int>
interceptRuleConst(const HeadRule& HR,
		   const Geometry::Vec3D& Origin,
		   const Geometry::Vec3D& N)
  /*!
    Determine the closes point to the headRule intercept
    \param HR :: HeadRule
    \param Origin :: Origin of line
    \param N :: Direction of the line
    \retval Origin / 0 :: Empty intercept
    \retval ClosePoint / SideDirection :: intercept
  */
{
  ELog::RegMethod RegA("SurInter[F]","interceptRuleConst");

  MonteCarlo::LineIntersectVisit LI(Origin,N);
  const std::vector<Geometry::interPoint>& Pts=
    LI.getIntercept(HR);

  // EMPTY return
  if (Pts.empty())
    return std::pair<Geometry::Vec3D,int>(Origin,0);

  const Geometry::interPoint& IP=
    SurInter::closestPt(Pts,Origin);

  return std::pair<Geometry::Vec3D,int>
    (IP.Pt,IP.SNum);
}

std::pair<Geometry::Vec3D,int>
interceptRule(HeadRule& HR,const
	      Geometry::Vec3D& Origin,
	      const Geometry::Vec3D& N)
  /*!
    Determine the closes point to the headRule intercept
    \param HR :: HeadRule
    \param Origin :: Origin of line
    \param N :: Direction of the line
    \return pair of position and surface.
  */
{
  ELog::RegMethod RegA("SurInter[F]","interceptRule(HeadRule&)");

  HR.populateSurf();
  return interceptRuleConst(HR,Origin,N);
}

  
}  // NAMESPACE SurInter


