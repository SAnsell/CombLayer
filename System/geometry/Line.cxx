/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geometry/Line.cxx
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
#include <vector>
#include <map>
#include <stack>
#include <string>
#include <algorithm>
#include <complex>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h" 
#include "OutputLog.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "mathSupport.h"
#include "polySupport.h"
#include "support.h"
#include "Triple.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Transform.h"
#include "Surface.h"
#include "Quadratic.h"
#include "ArbPoly.h"
#include "CylCan.h"
#include "Cylinder.h"
#include "Cone.h"
#include "EllipticCyl.h"
#include "MBrect.h"
#include "Plane.h"
#include "Sphere.h"
#include "Torus.h"
#include "Line.h"
#include "LineIntersectVisit.h"

namespace Geometry
{

std::ostream&
operator<<(std::ostream& OX,const Line& Lref)
  /*!
    Write the line to a stream
    \param OX :: Output stream
    \param Lref :: Line to write
    \return ostream
  */
{
  Lref.write(OX);
  return OX;
}

  Line::Line() : Origin(),Direct(0.0,1.0,0.0)
  /*!
    Constructor
  */
{}

Line::Line(const Geometry::Vec3D& O,const Geometry::Vec3D& D) 
  : Origin(O),Direct(D)
  /*!
    Constructor
    \param O :: Origin Vector		       
    \param D :: Direction (make into a unit vector)
  */
{
  Direct.makeUnit();
  if (Direct.nullVector())
    throw ColErr::NumericalAbort("Line Constructor Direction");
}

Line::Line(const Line& A) : 
  Origin(A.Origin),Direct(A.Direct)
  /*!
    Copy Constructor
    \param A :: Line to copy
   */
{}

Line&
Line::operator=(const Line& A)
  /*!
    Assignment operator
    \param A :: Line to copy
    \return *this
  */
{
  if (this!=&A)
    {
      Origin=A.Origin;
      Direct=A.Direct;
    }
  return *this;
}

Line::~Line()
  /*!
    Destructor
  */
{}

Geometry::Vec3D
Line::getPoint(const double lambda) const
  /*!
    Return the point on the line given lambda*direction
    \param lambda :: line position scalar
    \returns \f$ \vec{O}+ \lambda \vec{D} \f$
  */
{
  return Origin+Direct*lambda;
}

Geometry::Vec3D
Line::closestPoint(const Geometry::Vec3D& Pt) const
  /*!
    Get the closest point on the line to point Pt
    \param Pt :: test Point
    \returns Point on line
  */
{
  const double lambda=Direct.dotProd(Pt-Origin);
  return getPoint(lambda);
}

std::pair<Geometry::Vec3D,Geometry::Vec3D>
Line::closestPoints(const Line& A) const
  /*!
    Get the closest point on this line to another line
    Solved with A.direct and direct are perpendicular to
    connecting vector (w_c). P(s) is point on this, Q(t)
    is point on A.
    \param A :: line to use
    
    \returns Points on this line / line A
  */
{
  const Geometry::Vec3D W0=Origin-A.Origin;

  const double uv=Direct.dotProd(A.Direct);
  const double uw=W0.dotProd(Direct);
  const double vw=W0.dotProd(A.Direct);

  // s := (uv.vw - uw) / (1-uv.uv)
  // t := (vw - uv.uw) / (1-uv.uv) 

  // always > 0.0
  const double divFactor=1.0-uv*uv;
  if (divFactor<Geometry::zeroTol)
    return std::pair<Geometry::Vec3D,Geometry::Vec3D>(Origin,A.Origin);

  const double s=(uv*vw-uw)/divFactor;
  const double t=(vw-uw*uv)/divFactor;
  
  return std::pair<Geometry::Vec3D,Geometry::Vec3D>
    (getPoint(s),A.getPoint(t));
}

double
Line::distance(const Geometry::Vec3D& A) const
  /*!
    Distance of a point from the line
    \param A :: test Point
    \returns absolute distance (not signed)
  */
{
  const double lambda=Direct.dotProd(A-Origin);
  Geometry::Vec3D L=getPoint(lambda);
  L-=A;
  return L.abs();
}

int 
Line::isValid(const Geometry::Vec3D& A) const
  /*! 
     Calculate is point is on line by using distance to determine
     if the point is within Ltolerance of the line
     \param A :: Point to test
     \retval 1 : the point is on the line
     \retval 0 : Point is not on the line
  */ 
{
  return (distance(A)>Geometry::zeroTol) ? 0 : 1;
}

void
Line::setDirect(const Geometry::Vec3D& Pt) 
  /*!
    Set the direction of the line
    \param Pt :: New direction
  */
{
  if (Pt.nullVector())
    throw ColErr::NumericalAbort("Line::setDirect Direction");
  
  Direct=Pt.unit();
  return;
}

void
Line::rotate(const Geometry::Matrix<double>& MA) 
  /*!
    Applies the rotation matrix to the 
    object.
    \param MA :: Rotation Matrix [not checked]
  */
{
  Origin.rotate(MA);
  Direct.rotate(MA);
  if (Direct.nullVector())
    throw ColErr::NumericalAbort("Line::rotate Direction");
  Direct.makeUnit();
  return;
}

void 
Line::displace(const Geometry::Vec3D& Pt)
  /*! 
    Apply a displacement Pt 
    \param Pt :: Point value of the displacement
  */ 
{
  Origin+=Pt;
  return;
}

size_t
Line::lambdaPair(const size_t ix,
		 const std::pair<std::complex<double>,
		 std::complex<double> >& SQ,
		 std::vector<Geometry::Vec3D>& PntOut) const
  /*! 
    Helper function to decide which roots to take.
    The assumption is that lambda has been solved by quadratic
    equation and we require the points that correspond to these
    values. 
    \param ix : number of solutions in SQ (0,1,2)
    \param SQ : solutions to lambda (the distance along the line
    \param PntOut : Output vector of points (added to)
    \return Number of real unique points found.
  */
{ 
  // check results
  if (ix<1)
    return 0;

  int nCnt(0);          // number of good points
  double lambdaA,lambdaB;
  if (fabs(SQ.first.imag())<1e-38)
    {
      lambdaA=SQ.first.real();
      nCnt=1;
    }
  if (ix==2 && fabs(SQ.second.imag())<1e-38)
    {
      lambdaB=SQ.second.real();
      nCnt+=2;   // This is a bin flag [1+2 ==11]
    }
  
  if (!nCnt) 
    return 0;

  if (nCnt==1)
    {
      PntOut.push_back(getPoint(lambdaA));
      return 1;
    }
  if (nCnt==2)
    {
      PntOut.push_back(getPoint(lambdaB));
      return 1;
    }
  if (lambdaA>lambdaB)
    std::swap(lambdaA,lambdaB);
  PntOut.push_back(getPoint(lambdaA));
  PntOut.push_back(getPoint(lambdaB));
  return 2;
}


template<typename T>
Geometry::Vec3D
Line::interPoint(const T& Sur)  const
  /*!
    Carries out the process of determining an intersect point
    \tparam T :: Surface type 
    \param Sur :: Surface point to find
    \return Point of intersection
  */
{
  std::vector<Geometry::Vec3D> VecOut;
  this->intersect(VecOut,Sur);
  if (VecOut.empty())
    throw ColErr::EmptyValue<void>("Line::interpoint<"+Sur.className()+">");
  return VecOut.front();
}

size_t
Line::intersect(std::vector<Geometry::Vec3D>& VecOut,
		const Quadratic& Sur) const
  /*!
     For the line that intersects the surfaces 
     add the point(s) to the VecOut, return number of points
     added. It does not check the points for validity.
     \param VecOut :: intersection points of the line and surface
     \param Sur :: Surface to intersect with a line
     \return Number of points found. 
  */
{
  const std::vector<double> BN=Sur.copyBaseEqn();
  // Debug
  //  copy(BN.begin(),BN.end(),std::ostream_iterator<double>(std::cout," :: "));
  //  std::cout<<std::endl;
  // end Debubg
  const double a(Origin[0]),b(Origin[1]),c(Origin[2]);
  const double d(Direct[0]),e(Direct[1]),f(Direct[2]);
  double Coef[3];
  Coef[0] = BN[0]*d*d+BN[1]*e*e+BN[2]*f*f+
    BN[3]*d*e+BN[4]*d*f+BN[5]*e*f;
  Coef[1] = 2*BN[0]*a*d+2*BN[1]*b*e+2*BN[2]*c*f+
    BN[3]*(a*e+b*d)+BN[4]*(a*f+c*d)+BN[5]*(b*f+c*e)+
    BN[6]*d+BN[7]*e+BN[8]*f;
  Coef[2] = BN[0]*a*a+BN[1]*b*b+BN[2]*c*c+
    BN[3]*a*b+BN[4]*a*c+BN[5]*b*c+BN[6]*a+BN[7]*b+
    BN[8]*c+BN[9];
  
  std::pair<std::complex<double>,std::complex<double> > SQ;

  const size_t ix=solveQuadratic(Coef,SQ);
  return lambdaPair(ix,SQ,VecOut);
}  

size_t 
Line::intersect(std::vector<Geometry::Vec3D>& PntOut,
		const Plane& Pln) const
  /*! 
    For the line that intersects the plane generate 
    add the point to the VecOut, return number of points
    added. It does not check the points for validity. 
    
    \param PntOut :: Vector of points found by the line/plane intersection
    \param Pln :: Plane for intersect
    \return Number of points found by intersection
  */
{
  const double OdotN=Origin.dotProd(Pln.getNormal());
  const double DdotN=Direct.dotProd(Pln.getNormal());
  if (fabs(DdotN)<Geometry::parallelTol)        // Plane and line parallel
    return 0;
  const double u=(Pln.getDistance()-OdotN)/DdotN;
  PntOut.push_back(getPoint(u));
  return 1;
}

size_t 
Line::intersect(std::vector<Geometry::Vec3D>& PntOut,const ArbPoly& APoly) const
 /*!
   Calculate all the intersections in the surfaces of the 
   general polyhedron
   \param PntOut :: Vector of points found by the line/cylinder intersection
   \param APoly :: Poly-surface shape to intersect line with
   \return Number of points found
 */
{
  size_t cnt(0);
  std::vector<Geometry::Vec3D>::const_iterator vc;

  for(size_t i=0;i<APoly.getNSurf();i++)
    {
      std::vector<Geometry::Vec3D> PlanePts;
      const Geometry::Plane& TP=APoly.getPlane(i);

      if (intersect(PlanePts,TP) && APoly.side(PlanePts.front()))
	{
	  PntOut.push_back(PlanePts.front());
	  cnt++;
	}
    }
  return cnt;
}

size_t 
Line::intersect(std::vector<Geometry::Vec3D>& PntOut,
		const Cone& CObj) const
  /*! 
     For the line that intersects the cone
     add the point to the VecOut, return number of points
     added. It does not check the points for validity. 

     This follows the Intersection-cone from geometrymagic

     \param PntOut :: Vector of points found by the line/cylinder intersection
     \param CObj :: Cone to intersect line with
     \return Number of points found by intersection
  */
{
  ELog::RegMethod RegA("Line","intersect(cone)");

  const Geometry::Vec3D V=CObj.getCentre();  // cone vertex
  const Geometry::Vec3D A=CObj.getNormal();  // cone normal

  // reduced origin
  const Geometry::Vec3D b=Origin-V;
  const double AdotN=A.dotProd(Direct);
  const double AdotB=A.dotProd(b);
  const double BdotB=b.dotProd(b);
  const double BdotN=b.dotProd(Direct);
  const double gamma2=CObj.getCosAngle()*CObj.getCosAngle();

  double C[3];  
  C[0]=AdotN*AdotN-gamma2;
  C[1]=2.0*(AdotB*AdotN-gamma2*BdotN);
  C[2]=AdotB*AdotB-gamma2*BdotB;

  std::pair<std::complex<double>,std::complex<double> > SQ;
  const size_t ix = solveQuadratic(C,SQ);

  // This takes the centre displacement into account:
  std::vector<Geometry::Vec3D> testOut;
  const size_t nP=lambdaPair(ix,SQ,testOut);
  if (!nP) return 0;
  
  const int cF=CObj.getCutFlag();
  if (!cF)
    {
      for(const Geometry::Vec3D& Pnt : testOut)
        PntOut.push_back(Pnt);
      return nP;
    }

  size_t nOut(0);
  for(const Geometry::Vec3D& Pnt : testOut)
    {
      const Geometry::Vec3D bPt=Pnt-V;
      const double AdotB=A.dotProd(bPt);
      if (cF*AdotB>0.0)
        {
          PntOut.push_back(Pnt);
          nOut++;
        }
    }    
  return nOut;
}

size_t
Line::intersect(std::vector<Geometry::Vec3D>& PntOut,const CylCan& CCan) const
  /*! 
     For the line that intersects the right cylinder 
     add the point to the PntOut, return number of points
     added. It does not check the points for validity. 

     \param PntOut :: Vector of points found by the line/cylinder intersection
     \param CCan :: Right Cylinder to intersect line with
     \return Number of points found by intersection
  */
{
  ELog::RegMethod RegA("Line","intersect(CylCan)");

  const Geometry::Vec3D Cent=CCan.getCentre();
  const Geometry::Vec3D Ax=Origin-Cent;
  const Geometry::Vec3D N= CCan.getNormal();
  const double L=CCan.getLength();
  const double R=CCan.getRadius();
  const double vDn = N.dotProd(Direct);
  const double vDA = N.dotProd(Ax);
  // First solve the equation of intersection
  double C[3];
  C[0]= 1.0-(vDn*vDn);
  C[1]= 2.0*(Ax.dotProd(Direct)-vDA*vDn);
  C[2]= Ax.dotProd(Ax)-(R*R+vDA*vDA);
  std::pair<std::complex<double>,std::complex<double> > SQ;
  const size_t ix = solveQuadratic(C,SQ);
  // Now can check both soluitons:
  std::vector<Geometry::Vec3D> TOut;
  const size_t tnum=lambdaPair(ix,SQ,TOut);
  size_t cnt(0);
  for(size_t i=0;i<tnum;i++)
    {
      Geometry::Vec3D XPnt=TOut[i]-Cent;
      const double DX=XPnt.dotProd(N)/L;
      if (DX>=0 && DX<=1.0)
        {
	  PntOut.push_back(TOut[i]);
	  cnt++;
	}
    }
  // cheap return : - two good point 
  //              : - zero point and not parallel
  if (cnt==2 || fabs(vDn)<1.0-Geometry::parallelTol)
    return cnt; 

  for(int i=0;i<2;i++)
    {
      if (intersect(PntOut,CCan.getPlane(0)))
        {
	  if (PntOut.back().Distance(Cent+N*(i*L))>R)
	    PntOut.pop_back();
	  else
	    cnt++;
	}
    }
  return cnt;
}

size_t 
Line::intersect(std::vector<Geometry::Vec3D>& PntOut,
		const Cylinder& Cyl) const
  /*! 
     For the line that intersects the cylinder generate 
     add the point to the VecOut, return number of points
     added. It does not check the points for validity. 

     \param PntOut :: Vector of points found by the line/cylinder intersection
     \param Cyl :: Cylinder to intersect line with
     \return Number of points found by intersection
  */
{
  ELog::RegMethod RegA("Line","intersect(Cyl)");

  const Geometry::Vec3D Cent=Cyl.getCentre();
  const Geometry::Vec3D Ax=Origin-Cent;
  const Geometry::Vec3D N= Cyl.getNormal();
  const double R=Cyl.getRadius();
  const double vDn = N.dotProd(Direct);
  const double vDA = N.dotProd(Ax);

  // First solve the equation of intersection
  double C[3];
  C[0]= 1.0-(vDn*vDn);
  C[1]= 2.0*(Ax.dotProd(Direct)-vDA*vDn);
  C[2]= Ax.dotProd(Ax)-(R*R+vDA*vDA);
  std::pair<std::complex<double>,std::complex<double> > SQ;
  const size_t ix = solveQuadratic(C,SQ);
  // This takes the centre displacement into account:
  return lambdaPair(ix,SQ,PntOut);  
}

size_t 
Line::intersect(std::vector<Geometry::Vec3D>& PntOut,
		const EllipticCyl& Cyl) const
  /*! 
    Solve for a line intersection in an elliptic cylinder
    -- This is solved by noting that R= u*cos(theta) + v*sin(theta) 
      and r = O + mu D  resolve in the u / v directions 
     \param PntOut :: Vector of points found by the line/cylinder intersection
     \param Cyl :: EllipticCylinder to intersect line with
     \return Number of points found by intersection
  */
{
  ELog::RegMethod RegA("Line","intersect<EllipticCyl>");

  const Geometry::Vec3D Cent=Cyl.getCentre();
  const Geometry::Vec3D Ax=Origin-Cent;
  const Geometry::Vec3D N= Cyl.getNormal();
  const double AR=Cyl.getARadius();
  const double BR=Cyl.getBRadius();
  // First remove the component in the normal direction of the 
  // cylinder
  const double vDn = N.dotProd(Direct);
  // Line intersect
  if (fabs(vDn)>1.0-Geometry::zeroTol)
    return 0;
  // Cut normal
  const Geometry::Vec3D lineN=Direct.cutComponent(N);

  // Projection of vector into plane is : n * (X * n)
  const Geometry::Vec3D lineOrig=N*( Ax * N);

  // Now solve simultaneous equ.
  // Divide since cos(theta) has AR^2 term / sin(theta) has BR^2 term
  const double Oa= lineOrig.dotProd(Cyl.getAAxis())/AR;
  const double Ob= lineOrig.dotProd(Cyl.getBAxis())/BR;
  const double Na= lineN.dotProd(Cyl.getAAxis())/AR;
  const double Nb= lineN.dotProd(Cyl.getBAxis())/BR;

  // Solve the equation of intersection  
  const double cA=Nb*Nb+Na*Na;        // x^2 comp
  const double cB=2.0*(Na*Oa+Nb*Ob);
  const double cC=Oa*Oa+Ob*Ob-1.0;

  std::pair<std::complex<double>,std::complex<double> > SQ;
  const size_t ix = solveQuadratic(cA,cB,cC,SQ);
  // This takes the centre displacement into account:
  return lambdaPair(ix,SQ,PntOut);  
}

size_t 
Line::intersect(std::vector<Geometry::Vec3D>& PntOut ,const MBrect& MR) const
  /*! 
    For the line that intersects the rectangular object
    add the point to the VecOut, return number of points
    added. It does not check the points for validity. 

    \param PntOut :: Vector of points found by the line/planes intersection
    \param MR :: Macro-object for intersect
    \return Number of points found by intersection
  */
{
  size_t cnt(0);
  // a rcc has 6 sides but this could be generalised to all macro-bodies ?
  for(size_t i=0;i<6;i++)
    cnt+=intersect(PntOut,MR.getPlane(i));
  return cnt;
}

size_t
Line::intersect(std::vector<Geometry::Vec3D>& PntOut ,const Sphere& Sph) const
  /*! 
    For the line that intersects the sphere generate 
    add the point to the PntOut, return number of points
    added. It does not check the points for validity. 
    
    \param PntOut :: Vector of points found by the line/sphere intersection
    \param Sph :: Sphere to intersect line with
    \returns Number of points found by intersection
  */
{
  // Nasty stripping of useful stuff from spher
  const Geometry::Vec3D Ax=Origin-Sph.getCentre();
  const double R=Sph.getRadius();
  // First solve the equation of intersection
  double C[3];
  C[0]=1;
  C[1]=2.0*Ax.dotProd(Direct);
  C[2]=Ax.dotProd(Ax)-R*R;
  std::pair<std::complex<double>,std::complex<double> > SQ;
  const size_t ix = solveQuadratic(C,SQ);
  return lambdaPair(ix,SQ,PntOut); 
}

// TORUS
size_t 
Line::intersect(std::vector<Geometry::Vec3D>&,const Torus&) const
  /*! 
     For the line that intersects the torus,
     add the point to the PntOut, return number of points added. 
     It does not check the points for validity. 

     This uses the system of reconstruction. The torus is always
     on the xy plane and the line is rotated. 

     \param  :: Vector of points found by the line/cylinder intersection
     \param  :: Torus to intersect line with
     \return Number of points found by intersection
  */
{
  /*
  // IF X=(x,y,z)  is the intersection
  p=(x,y,0);
  l=veclength(p);
  p=MRadius*P/l;
  n=x-p;
  n.makeUnit();  // Normal:
  */

  return 0;
  /*
  // This struct is syntactic sugar so that a.b,
  // (the dot product) looks just right (:-)
  struct { float a,b; } a;
  a.b = ax*bx + ay*by + az*bz;
  a.a = ax*ax + ay*ay + az*az;

  // Set up quartic in t:
  // t^4+At^3+Bt^2+Ct+D=0

  const double Rmajor=Tor.getMajRadius();
  const double Rminor=Tor.getMinRadius();
  const double R2=Rmajor*Rmajor
  float K = a.a - Rminor*Rminor - R2;
  float A = 4*a.b;
  float B = 2*(2*a.b*a.b + K + 2*R2*bz*bz);
  float C = 4*(K*a.b + 2*R2*az*bz);
  float D = K*K + 4*R2*(az*az - r*r);

  // Solve quartic...
  double roots[4];
  int nroots = solve_quartic(A,B,C,D,roots,SOLVE_ALL);

  *num_intersections = 0;
  while(nroots--)
    {
      float t = roots[nroots];
      float x = ax + t*bx;
      float y = ay + t*by;
      float l = R*(M_PI/2 - atan2(y,x));
      if (l <= vlength && l >= 0)
        intersections[(*num_intersections)++] = t;
    }
  */
}


//SETING

int 
Line::setLine(const Geometry::Vec3D& O,const Geometry::Vec3D& D) 
  /*!  Line LX(Apt,Bpt);

    sets the line given the Origne and direction
    \param O :: origin
    \param D :: direction
    \retval  0 ::  Direction == 0 ie no line
    \retval 1 :: on success
  */
{
  if (D.nullVector())
    return 0;
  Origin=O;
  Direct=D;
  Direct.makeUnit();
  return 1;
}

int 
Line::setLine(const Geometry::Plane& A,const Geometry::Plane& B) 
  /*!
    Given two planes finds the line between them 
    \param A :: First plane
    \param B :: First plane
    \retval  0 :: Planes are parallel
    \retval 1 :: on success
  */
{
  const double Nab = A.dotProd(B);
  const double det= 1.0-Nab*Nab;
  if (fabs(det)<Geometry::parallelTol)        //parallel planes
    return 0;
  const double c1=(A.getDistance()-B.getDistance()*Nab)/det;
  const double c2=(B.getDistance()-A.getDistance()*Nab)/det;
  Direct=A.crossProd(B);
  Direct.makeUnit();
  Origin=A.getNormal()*c1+B.getNormal()*c2;
  return 1;
}

void
Line::print() const
  /*!
    Print statement for debugging
  */
{
  std::cout<<"Line == "<<Origin<<" :: "<<Direct<<std::endl;
  return;
}

void
Line::write(std::ostream& OX) const
  /*!
    Write the line to the stream
    \param OX :: Output stream
  */
{
  OX<<Origin<<" ("<<Direct<<")";
  return;
}

///\cond TEMPLATE

template Geometry::Vec3D Line::interPoint(const Quadratic&) const;
template Geometry::Vec3D Line::interPoint(const Geometry::Cylinder&) const;
template Geometry::Vec3D Line::interPoint(const Geometry::Plane&) const;

///\endcond TEMPLATE 

}   // NAMESPACE Geometry
