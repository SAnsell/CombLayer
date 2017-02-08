/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geometry/EllipticCyl.cxx
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
#include <complex>
#include <list>
#include <vector>
#include <map>
#include <stack>
#include <string>
#include <algorithm>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "support.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Line.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Plane.h"
#include "EllipticCyl.h"

#include "Debug.h"

namespace Geometry
{

std::ostream&
operator<<(std::ostream& OX,const EllipticCyl& A)
  /*!
    Output stream
    \param OX :: output stream
    \param A :: EllipticCly to write
    \return Output stream
  */
{
  A.write(OX);
  return OX;
}

EllipticCyl::EllipticCyl() : 
  Quadratic(),Centre(),Normal(0,0,1),LAxis(1,0,0),
  CAxis(0,1,0),ARadius(1.0),BRadius(1.0)
  /*!
    Standard Constructor creats a cylinder (radius 0)
    along the x axis
  */
{
  // Called after it has been sized by Quadratic
  EllipticCyl::setBaseEqn();
}

EllipticCyl::EllipticCyl(const int N,const int T) : 
  Quadratic(N,T),Centre(),Normal(0,0,1),LAxis(1,0,0),
  CAxis(0,1,0),ARadius(1.0),BRadius(1.0)
  /*!
    Standard Constructor creates a cylinder (radius 0)
    along the x axis
    \param N :: Name
    \param T :: Transform
  */
{
  // Called after it has been sized by Quadratic
  EllipticCyl::setBaseEqn();
}

EllipticCyl::EllipticCyl(const EllipticCyl& A) : 
  Quadratic(A),
  Centre(A.Centre),Normal(A.Normal),LAxis(A.LAxis),
  CAxis(A.CAxis),ARadius(A.ARadius),BRadius(A.BRadius)
  /*!
    Copy constructor
    \param A :: EllipticCyl to copy
  */
{}

EllipticCyl&
EllipticCyl::operator=(const EllipticCyl& A)
  /*!
    Assignment operator
    \param A :: EllipticCyl to copy
    \return *this
  */
{
  if (this!=&A)
    {
      Quadratic::operator=(A);
      Centre=A.Centre;
      Normal=A.Normal;
      LAxis=A.LAxis;
      CAxis=A.CAxis;
      ARadius=A.ARadius;
      BRadius=A.BRadius;
    }
  return *this;
}

EllipticCyl*
EllipticCyl::clone() const
  /*!
    Makes a clone (implicit virtual copy constructor) 
    \return Copy(*this)
  */
{
  return new EllipticCyl(*this);
}

EllipticCyl::~EllipticCyl()
  /*!
    Standard Destructor
  */
{}

bool
EllipticCyl::operator!=(const EllipticCyl& A) const
  /*!
    Determine if two cylinders are equal (within tolerance).
    This is complicated by the fact that both centres can be different
    but it is the same cylinder. Or that the axis can be 180 degrees.
    \param A :: EllipticCyl to compare
    \return this!=A
  */
{
  return !(this->operator==(A));
}

bool
EllipticCyl::operator==(const EllipticCyl& A) const
  /*!
    Determine if two cylinders are equal (within tolerance).
    This is complicated by the fact that both centres can be different
    but it is the same cylinder. Or that the axis can be 180 degrees.
    \param A :: EllipticCyl to compare
    \return this==A
  */
{
  ELog::RegMethod RegA("EllipticCyl","operator==");

  if (&A==this) return 1;
  return Quadratic::operator==(A);
}

int
EllipticCyl::setSurface(const std::string& Pstr)
  /*! 
    Processes a standard MCNPX cone string    
    Recall that elliptic cyls can only be specified on an axis
     Valid input is: 
     - e/x cen_y cen_z radiusY radiusZ
     - ex radiusY radiusZ
     \param Pstr :: Input string
    \retval : 0 on success
    \retval : -1 :: Description (Hard fail)
    \retval : -2 :: Axis (Hard fail)
    \retval : -3 :: Centre (Hard fail)
    \retval : -4 :: Radius (Hard fail)
    \retval : -101/2 :: radius negative/zero (Hard fail)
  */
{
  ELog::RegMethod RegA("EllipticCyl","setSurface");

  enum { errDesc=-1, errAxis=-2,
	 errCent=-3, errRadius=-4};

  std::string Line=Pstr;
  std::string item;
  processSetHead(Line);
    
  if (!StrFunc::section(Line,item) || 
      tolower(item[0])!='e' || item.length()<2 || 
      item.length()>3)
    return errDesc;

  // EllipticCyls on X/Y/Z axis
  const size_t itemPt((item[1]=='/' && item.length()==3) ? 2 : 1);
  const size_t ptype=static_cast<size_t>(tolower(item[itemPt])-'x');
  if (ptype>=3) return errAxis;

  double norm[3]={0.0,0.0,0.0};
  double cross[3]={0.0,0.0,0.0};
  double cent[3]={0.0,0.0,0.0};
  norm[ptype]=1.0;
  if (ptype)
    cross[0]=1.0;
  else
    cross[1]=1.0; 

  if (itemPt!=1)
    {
      for(size_t index=0;index<3;index++)
	{
	  if (index!=ptype && !StrFunc::section(Line,cent[index]))
	    return errCent;
	}
    }
  // Now get radii
  double RA,RB;
  if (!StrFunc::section(Line,RA) || !StrFunc::section(Line,RB))
    return errRadius;
  if (RA<=0.0 || RB<=0.0)
    return -100+errRadius;

  Centre=Geometry::Vec3D(cent);
  setNorm(Geometry::Vec3D(norm),Geometry::Vec3D(cross));
  ARadius=RA;
  BRadius=RB;

  setBaseEqn();
  return 0;
} 

int
EllipticCyl::setEllipticCyl(const Geometry::Vec3D& Cpoint,
			    const Geometry::Vec3D& Axis,
			    const Geometry::Vec3D& CrossAxis,
			    const double RA,const double RB) 
  /*!
    Given a point and axid direction set the cylinder
    \param Cpoint :: Centre point
    \param Axis :: Axis direction
    \param CrossAxis :: Cross.Axis direction
    \param RA :: radius of LAxis direction
    \param RB :: radius of Norm*LAxis direction
    \retval 0 :: success / -ve on failure
  */
{
  if (RA<=0 || RB<=0.0)
    return -1;
  
  Centre=Cpoint;

  ARadius=RA;
  BRadius=RB;
  return setNorm(Axis,CrossAxis);
}

int 
EllipticCyl::side(const Geometry::Vec3D& Pt) const 
  /*!
    Calculate if the point PT within the middle
    of the cylinder 
    \param Pt :: Point to check
    \retval -1 :: within cylinder 
    \retval 1 :: outside the cylinder
    \retval 0 :: on the surface 
  */
{
  return Quadratic::side(Pt);
}

int 
EllipticCyl::onSurface(const Geometry::Vec3D& Pt) const 
  /*!
    Calculate if the point PT on the cylinder 
    \param Pt :: Geometry::Vec3D to test
    \retval 1 :: on the surface 
    \retval 0 :: not on the surface
  */
{
  ELog::RegMethod RegA("EllipticCyl","onSurface");
  return Quadratic::onSurface(Pt);
}


void
EllipticCyl::rotate(const Geometry::Matrix<double>& MA)
/*!
  Apply a rotation to the cylinder and re-check the
  status of the main axis.
  \param MA :: Rotation Matrix (not inverted)
*/
{
  Centre.rotate(MA);
  Normal.rotate(MA);
  Normal.makeUnit();
  LAxis.rotate(MA);
  LAxis.makeUnit();

  Quadratic::rotate(MA);
  return;
}

void
EllipticCyl::rotate(const Geometry::Quaternion& QA)
  /*!
    Apply a rotation to the cylinder and re-check the
    status of the main axis.
    \param QA :: Quaternion to rotate by
  */
{
  QA.rotate(Centre);
  QA.rotate(Normal);  
  QA.rotate(LAxis);
  Normal.makeUnit();
  LAxis.makeUnit();
  Quadratic::rotate(QA);
  return;
}

void 
EllipticCyl::displace(const Geometry::Vec3D& Pt)
  /*!
    Apply a displacement Pt 
    \param Pt :: Displacement to add to the centre
  */ 
{
  Centre+=Pt;
  Quadratic::displace(Pt);
  return;
}

void
EllipticCyl::setCentre(const Geometry::Vec3D& A)
  /*!
    Sets the centre Geometry::Vec3D
    \param A :: centre point 
  */
{
  Centre=A;
  setBaseEqn();
  return;
}

int
EllipticCyl::setNorm(const Geometry::Vec3D& A,const Geometry::Vec3D& B)
  /*! 
    Sets the centre line unit vector 
    A does not need to be a unit vector
    \param A :: Vector along the centre line 
    \param B :: Vector along off axis
    \return Normal value
  */
{

  Normal=A.unit();
  LAxis=B.unit();

  // calc vector in the plane that is normal
  const double cosA=LAxis.dotProd(Normal);

  if (fabs(cosA)>(1.0-Geometry::zeroTol))
    return -1;
  LAxis=LAxis-(Normal*cosA);
  CAxis=LAxis*Normal;

  setBaseEqn();
  return 0;
}

void
EllipticCyl::setRadius(const double RA,const double RB)
  /*! 
    Sets the radius
    \param RA :: New radius (forced +ve)
    \param RB :: New radius (forced +ve)
  */
{
  ARadius=fabs(RA);
  BRadius=fabs(RB);
  setBaseEqn();
  return;
}

void
EllipticCyl::setBaseEqn()
  /*!
    Sets an equation of type (elliptic cylinder)
    \f[ Ax^2+By^2+Cz^2+Dxy+Exz+Fyz+Gx+Hy+Jz+K=0 \f]
  */
{
  ELog::RegMethod RegA("EllipticCyl","setBaseEqn");

  Geometry::Matrix<double> MA(3,3);
  for(size_t i=0;i<3;i++)
    {
      MA[i][0]=LAxis[i];
      MA[i][1]=CAxis[i];
      MA[i][2]=Normal[i];
    }
  for(size_t i=0;i<10;i++)
    BaseEqn[i]=0.0;

  BaseEqn[0]= 1.0/(ARadius*ARadius);     
  BaseEqn[1]= 1.0/(BRadius*BRadius);     
  BaseEqn[9]= -1.0;

  Quadratic::rotate(MA);
  Quadratic::displace(Centre);
  
  return;
}

void
EllipticCyl::mirror(const Geometry::Plane& MP)
  /*!
    Apply a mirror plane to the cylinder
    This involves moving the centre and the 
    axis 
    \param MP :: Mirror point
   */
{
  
  MP.mirrorAxis(Normal);
  MP.mirrorAxis(LAxis);
  MP.mirrorAxis(CAxis);
  MP.mirrorPt(Centre);
  setBaseEqn();
  return;
}

double
EllipticCyl::firstQuadrant(const double e[2],const double y[2])
  /*!
    Internal function to work out distnace of a point from an ellipse
    in the first quadrant by bisection.
    Note that this code can be used to compute the impact point
    \param e :: normalized radius 
    \param y :: reduced coordinate
    \return distance
  */
{
  ELog::RegMethod RegA("EllipticCyl","firstQuadrant");

  double distance =0.0;
  if (y[1] > Geometry::zeroTol)
    {
      if (y[0] > Geometry::zeroTol)
	{
	  // Bisect to compute the root of F(t) for t >= -e1*e1.
	  double esqr[2] = { e[0]*e[0], e[1]*e[1] };
	  double ey[2] = { e[0]*y[0], e[1]*y[1] };
	  double t0 = -esqr[1] + ey[1];
	  double t1 = -esqr[1] + sqrt(ey[0]*ey[0] + ey[1]*ey[1]);
	  double t = t0;
	  const int imax = 2*std::numeric_limits<double>::max_exponent;
	  for (int i=0;i<imax;i++)
	    {
	      t = (0.5)*(t0 + t1);
	      if (t == t0 || t == t1)    // can this happen?
		break;
	      
	      const double r[2] = { ey[0]/(t+esqr[0]), ey[1]/(t+esqr[1]) };
	      double f = r[0]*r[0]+r[1]*r[1]-1.0;
	      if (f > Geometry::zeroTol)
		t0 = t;
	      else if (f < Geometry::zeroTol)
		t1 = t;
	      else
		break;
	    }
	  const double xA = esqr[0]*y[0]/(t+esqr[0]);
	  const double xB = esqr[1]*y[1]/(t+esqr[1]);
	  const double d[2] = { xA - y[0], xB - y[1] };
	  distance = sqrt(d[0]*d[0] + d[1]*d[1]);
	}
      else // y0 == 0
	{
	  // xA = 0.0; xB = e[1];
	  distance = fabs(y[1] - e[1]);
	}
    }
  else // y1 == 0
    {
      const double denom0 = e[0]*e[0] - e[1]*e[1];
      const double e0y0 = e[0]*y[0];
      if (e0y0 < denom0)
	{
	  // y0 is inside the subinterval.
	  const double x0de0 = e0y0/denom0;
	  const double xA = e[0]*x0de0;
	  const double xB = e[1]*sqrt(fabs(1.0-x0de0*x0de0));
	  const double d0 = xA-y[0];
	  distance = sqrt(d0*d0+xB*xB);
	}
      else
	{
	  // y0 is outside the subinterval. The closest ellipse point has
	  // x1 == 0 and is on the domain-boundary interval (x0/e0)^2 = 1.
	  // const doubel xA = e[0]; const double  x[1] = (Real)0;
	  distance = fabs(y[0]-e[0]);
	}
    }
  return distance;
}


double
EllipticCyl::distance(const Geometry::Vec3D& A) const
  /*!
    Calculates the distance of point A from 
    the surface of the  Elliptic cylinder.
    \param A :: Point to calculate distance from
    \return :: +ve distance to the surface.
  */
{
  // First remove the component along the vector: 
  // and remove center;
  Geometry::Vec3D Pt=A-Normal*(Normal.dotProd(A));
  Pt-=Centre;
  
  double y[2],e[2];
  y[0]=fabs(LAxis.dotProd(Pt));
  y[1]=fabs(CAxis.dotProd(Pt));
  e[0]=ARadius;
  e[1]=BRadius;
  if (BRadius>ARadius)
    {
      std::swap(y[0],y[1]);
      std::swap(e[0],e[1]);
    }
  //----------------------------------------------------------------------------
// The ellipse is (x0/e0)^2 + (x1/e1)^2 = 1. The query point is (y0,y1).
// The function returns the distance from the query point to the ellipse.
// It also computes the ellipse point (x0,x1) that is closest to (y0,y1).
//----------------------------------------------------------------------------

// Determine reflections for y to the first quadrant.
  for (int i=0;i<2;i++)
    {
      //      reflect[i] = (y[i] < -Geometry::zeroTol);
      if (y[i] < -Geometry::zeroTol)
	y[i]*=-1.0;
    }
  return firstQuadrant(e,y);
}

void
EllipticCyl::writePOVRay(std::ostream& OX) const
  /*! 
    Write out the cylinder for POVRay
    \param OX :: output stream
  */
{
  Quadratic::writePOVRay(OX);
  return;
}

  
void
EllipticCyl::write(std::ostream& OX) const
  /*! 
    Write out the cylinder for MCNPX
    \param OX :: output stream
  */
{
  //               -3 -2 -1 0 1 2 3        
  Quadratic::write(OX);
  return;
}

void
EllipticCyl::print() const
 /*!
   Debug routine to print out basic information 
 */
{
  Quadratic::print();
  std::cout<<"Axis =="<<Normal<<" ";
  std::cout<<"LAxis =="<<LAxis<<" ";
  std::cout<<"CAxis =="<<CAxis<<" ";
  std::cout<<"Centre == "<<Centre<<" ";
  std::cout<<"Radius == "<<ARadius<<" "<<BRadius<<std::endl;
  return;
}


}   // NAMESPACE Geometry

