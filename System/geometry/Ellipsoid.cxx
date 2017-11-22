/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geometry/Ellipsoid.cxx
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
#include "Ellipsoid.h"

#include "Debug.h"

namespace Geometry
{

std::ostream&
operator<<(std::ostream& OX,const Ellipsoid& A)
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

Ellipsoid::Ellipsoid() : 
  Quadratic(),Centre(),AAxis(0,0,1),BAxis(1,0,0),
  CAxis(0,1,0),ARadius(1.0),BRadius(1.0),CRadius(1.0)
  /*!
    Standard Constructor creats a cylinder (radius 0)
    along the x axis
  */
{
  // Called after it has been sized by Quadratic
  Ellipsoid::setBaseEqn();
}

  
Ellipsoid::Ellipsoid(const int N,const int T) : 
  Quadratic(N,T),Centre(),AAxis(0,0,1),BAxis(1,0,0),
  CAxis(0,1,0),ARadius(1.0),BRadius(1.0),CRadius(1.0)
  /*!
    Standard Constructor creats a cylinder (radius 0)
    along the x axis
    \param N :: Name
    \param T :: Transform
  */
{
  // Called after it has been sized by Quadratic
  Ellipsoid::setBaseEqn();
}

Ellipsoid::Ellipsoid(const Ellipsoid& A) : 
  Quadratic(A),
  Centre(A.Centre),AAxis(A.AAxis),BAxis(A.BAxis),
  CAxis(A.CAxis),ARadius(A.ARadius),BRadius(A.BRadius),
  CRadius(A.CRadius)
  /*!
    Copy constructor
    \param A :: Ellipsoid to copy
  */
{}

Ellipsoid&
Ellipsoid::operator=(const Ellipsoid& A)
  /*!
    Assignment operator
    \param A :: Ellipsoid to copy
    \return *this
  */
{
  if (this!=&A)
    {
      Quadratic::operator=(A);
      Centre=A.Centre;
      AAxis=A.AAxis;
      BAxis=A.BAxis;
      CAxis=A.CAxis;
      ARadius=A.ARadius;
      BRadius=A.BRadius;
      CRadius=A.CRadius;
    }
  return *this;
}

Ellipsoid*
Ellipsoid::clone() const
  /*!
    Makes a clone (implicit virtual copy constructor) 
    \return Copy(*this)
  */
{
  return new Ellipsoid(*this);
}

Ellipsoid::~Ellipsoid()
  /*!
    Standard Destructor
  */
{}

bool
Ellipsoid::operator!=(const Ellipsoid& A) const
  /*!
    Determine if two cylinders are equal (within tolerance).
    This is complicated by the fact that both centres can be different
    but it is the same cylinder. Or that the axis can be 180 degrees.
    \param A :: Ellipsoid to compare
    \return this!=A
  */
{
  return !(this->operator==(A));
}

bool
Ellipsoid::operator==(const Ellipsoid& A) const
  /*!
    Determine if two cylinders are equal (within tolerance).
    This is complicated by the fact that both centres can be different
    but it is the same cylinder. Or that the axis can be 180 degrees.
    \param A :: Ellipsoid to compare
    \return this==A
  */
{
  ELog::RegMethod RegA("Ellipsoid","operator==");

  if (&A==this) return 1;
  return Quadratic::operator==(A);
}

int
Ellipsoid::setSurface(const std::string& Pstr)
  /*! 
    Processes a standard MCNPX cone string    
    Recall that elliptiod can only be specified on an axis
     Valid input is: 
     - e cen_x cen_y cen_z radiusX radiusY radiusZ
     - e radiusX radiusY radiusZ
     \param Pstr :: Input string
    \retval : 0 on success
    \retval : -1 :: Description (Hard fail)
    \retval : -2 :: Axis (Hard fail)
    \retval : -3 :: Centre (Hard fail)
    \retval : -4 :: Radius (Hard fail)
    \retval : -101/2 :: radius negative/zero (Hard fail)
  */
{
  ELog::RegMethod RegA("Ellipsoid","setSurface");

  enum { errDesc=-1, errAxis=-2,
	 errCent=-3, errRadius=-4};

  std::string Line=Pstr;
  std::string item;
  processSetHead(Line);
    
  if (!StrFunc::section(Line,item) || 
      tolower(item[0])!='e' || item.length()>1)
    return errDesc;

  double cent[3];
  for(size_t index=0;index<3;index++)
    {
      if (!StrFunc::section(Line,cent[index]))
	return errCent;
    }

  // Now get radii
  double RA,RB,RC;
  if (!StrFunc::section(Line,RA) ||
      !StrFunc::section(Line,RB) ||
      !StrFunc::section(Line,RC))
    return errRadius;
  
  if (RA<=Geometry::zeroTol || RB<=Geometry::zeroTol ||
      RC<=Geometry::zeroTol)
    return -100+errRadius;

  setEllipsoid(Geometry::Vec3D(cent),Geometry::Vec3D(1,0,0),
	       Geometry::Vec3D(0,1,0),RA,RB,RC);

  setBaseEqn();
  return 0;
} 

void
Ellipsoid::orderRadii(Geometry::Vec3D* AVec,double* R) 
  /*!
    Order the radii [STATIC]
    \param AVec :: Vectors [3 of]
    \param Radii :: Vectors [3 of]
  */
{
  // Simple bubble sort on three items:
  for(size_t i=0;i<3;i++)
    for(size_t j=1;j<3;j++)
      {
	if (R[i]<R[j])
	  {
	    std::swap(AVec[i],AVec[j]);
	    std::swap(R[i],R[j]);
	  }
      }
  return;
}
  
  
int
Ellipsoid::setEllipsoid(const Geometry::Vec3D& Cpoint,
			const Geometry::Vec3D& RAAxis,
			const Geometry::Vec3D& RBAxis,
			const double RA,const double RB,
			const double RC) 
  /*!
    Given a point and axid direction set the cylinder
    \todo add check for parallel etc.
    \param Cpoint :: Centre point
    \param RAAxis :: Axis direction
    \param RBAxis :: Cross.Axis direction
    \param RA :: radius 
    \param RB :: radius 
    \param RC :: small radius
    \retval 0 :: success / -ve on failure
  */
{
  ELog::RegMethod RegA("Ellipsoid","setEllipsoid");

  Geometry::Vec3D VecRef[3];
  double R[3]={RA,RB,RC};

  VecRef[0]= RAAxis.unit();
  VecRef[1]= RBAxis.unit();
  VecRef[2]= (VecRef[0]*VecRef[1]).unit(); // unit for numerical accuracy

  orderRadii(VecRef,R);

  ARadius=RA;
  BRadius=RB;
  CRadius=RC;
  AAxis=VecRef[0];
  BAxis=VecRef[1];
  CAxis=VecRef[2];
  
  Centre=Cpoint;
  Ellipsoid::setBaseEqn();
  return 0;
}

int 
Ellipsoid::side(const Geometry::Vec3D& Pt) const 
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
Ellipsoid::onSurface(const Geometry::Vec3D& Pt) const 
  /*!
    Calculate if the point PT on the cylinder 
    \param Pt :: Geometry::Vec3D to test
    \retval 1 :: on the surface 
    \retval 0 :: not on the surface
  */
{
  ELog::RegMethod RegA("Ellipsoid","onSurface");
  return Quadratic::onSurface(Pt);
}


void
Ellipsoid::rotate(const Geometry::Matrix<double>& MA)
/*!
  Apply a rotation to the cylinder and re-check the
  status of the main axis.
  \param MA :: Rotation Matrix (not inverted)
*/
{
  Centre.rotate(MA);
  AAxis.rotate(MA);
  AAxis.makeUnit();
  BAxis.rotate(MA);
  BAxis.makeUnit();
  CAxis.rotate(MA);
  CAxis.makeUnit();

  Quadratic::rotate(MA);
  return;
}

void
Ellipsoid::rotate(const Geometry::Quaternion& QA)
  /*!
    Apply a rotation to the cylinder and re-check the
    status of the main axis.
    \param QA :: Quaternion to rotate by
  */
{
  QA.rotate(Centre);
  QA.rotate(AAxis);  
  QA.rotate(BAxis);
  QA.rotate(CAxis);
  Quadratic::rotate(QA);
  return;
}

void 
Ellipsoid::displace(const Geometry::Vec3D& Pt)
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
Ellipsoid::setCentre(const Geometry::Vec3D& A)
  /*!
    Sets the centre Geometry::Vec3D
    \param A :: centre point 
  */
{
  Centre=A;
  setBaseEqn();
  return;
}


void
Ellipsoid::setRadii(const double RA,const double RB,const double RC)
  /*! 
    Sets the radius
    \param RA :: New radius (forced +ve)
    \param RB :: New radius (forced +ve)
    \param RC :: New radius (forced +ve)
  */
{
  ELog::RegMethod RegA("Ellipsoid","setRadii");

  // base equation set in setEllipsoid
  setEllipsoid(AAxis,BAxis,CAxis,fabs(RA),fabs(RB),fabs(RC));
  return;
}

void
Ellipsoid::setBaseEqn()
  /*!
    Sets an equation of type (elliptic cylinder)
    \f[ Ax^2+By^2+Cz^2+Dxy+Exz+Fyz+Gx+Hy+Jz+K=0 \f]
  */
{
  ELog::RegMethod RegA("Ellipsoid","setBaseEqn");

  Geometry::Matrix<double> MA(3,3);
  for(size_t i=0;i<3;i++)
    {
      MA[i][0]=AAxis[i];
      MA[i][1]=BAxis[i];
      MA[i][2]=CAxis[i];
    }
  for(size_t i=0;i<10;i++)
    BaseEqn[i]=0.0;

  BaseEqn[0]= 1.0/(ARadius*ARadius);     
  BaseEqn[1]= 1.0/(BRadius*BRadius);
  BaseEqn[2]= 1.0/(CRadius*CRadius);     
  BaseEqn[9]= -1.0;

  Quadratic::rotate(MA);
  Quadratic::displace(Centre);
  
  return;
}

void
Ellipsoid::mirror(const Geometry::Plane& MP)
  /*!
    Apply a mirror plane to the cylinder
    This involves moving the centre and the 
    axis 
    \param MP :: Mirror point
   */
{
  MP.mirrorAxis(AAxis);
  MP.mirrorAxis(BAxis);
  MP.mirrorAxis(CAxis);
  MP.mirrorPt(Centre);
  setBaseEqn();
  return;
}

double
Ellipsoid::solveTBar(const double e[3],const double y[3]) 
  /*!
    Solve the equation :
    \f[ F(t) := + \left (\frac{e_0 y_0}{t+e^2_0}\right)^2 +... = 1\f]
    for the explicit domain \f[ t= (-e^2_2 : -1) \f]  
  */
{
  return -1.0;          //  Junk solution
}
  
double
Ellipsoid::firstOctant(const double e[3],const double y[3])
  /*!
    Internal function to work out distance of a point from an ellipse
    in the first quadrant by bisection.
    Note that this code can be used to compute the impact point
    \param e :: normalized radius 
    \param y :: reduced coordinate
    \return distance
  */
{
  ELog::RegMethod RegA("Ellipsoid","firstOctant");

  double distance =0.0;
  const double tBar=solveTBar(e,y);
  if (y[2] > Geometry::zeroTol)
    {
      if (y[1] > Geometry::zeroTol)
	{
	  if (y[0] > Geometry::zeroTol)
	    {
	      double x;
	      for(size_t i=0;i<3;i++)
		{
		  x=e[i]*e[i]*y[i] / (tBar + e[i]*e[i]);
		  distance+=(x-y[i])*(x-y[i]);
		}
	      return sqrt(distance);
	    }
	}
      // OTHER SPECIAL CASED [y == 0 ] 
    }
	
  return distance;
}


double
Ellipsoid::distance(const Geometry::Vec3D& A) const
  /*!
    Calculates the distance of point A from 
    the surface of the  Elliptic cylinder.
    \param A :: Point to calculate distance from
    \return :: +ve distance to the surface.
  */
{
  // First remove theand remove center;
  Geometry::Vec3D Pt=A-Centre;;
  
  double y[3],e[3];
  y[0]=fabs(AAxis.dotProd(Pt));
  y[1]=fabs(BAxis.dotProd(Pt));
  y[2]=fabs(CAxis.dotProd(Pt));
  e[0]=ARadius;
  e[1]=BRadius;
  e[2]=CRadius;
//----------------------------------------------------------------------------
// The ellipse is (x0/e0)^2 + (x1/e1)^2 = 1. The query point is (y0,y1).
// The function returns the distance from the query point to the ellipse.
// It also computes the ellipse point (x0,x1) that is closest to (y0,y1).
//----------------------------------------------------------------------------
  return firstOctant(e,y);
}



void
Ellipsoid::writePOVRay(std::ostream& OX) const
  /*! 
    Write out the cylinder for POV-Ray
    \param OX :: output stream
  */
{
  Quadratic::writePOVRay(OX);
  return;
}

  
  
void
Ellipsoid::write(std::ostream& OX) const
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
Ellipsoid::print() const
 /*!
   Debug routine to print out basic information 
 */
{
  Quadratic::print();
  return;
}


}   // NAMESPACE Geometry

