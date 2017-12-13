/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geometry/Sphere.cxx
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
#include <boost/format.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Surface.h"
#include "masterWrite.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Sphere.h"

namespace Geometry
{

Sphere::Sphere() : Quadratic(),
  Centre(0,0,0),Radius(0.0)
  /*!
    Default constructor 
    make sphere at the origin radius zero 
  */
{
  setBaseEqn();
}
 
Sphere::Sphere(const int N,const int T) : 
  Quadratic(N,T),Centre(0,0,0),Radius(0.0)
  /*!
    Constructor: make sphere at origin radius zero
    \param N :: Name
    \param T :: Transform number
  */
{
  setBaseEqn();
}
 
Sphere::Sphere(const Sphere &A) : 
  Quadratic(A),Centre(A.Centre),Radius(A.Radius)
  /*!
    Default Copy constructor 
    \param A :: Sphere to copy
  */
{ }

Sphere*
Sphere::clone() const
  /*!
    Makes a clone (implicit virtual copy constructor) 
    \return new (*this)
  */
{
  return new Sphere(*this);
}

Sphere&
Sphere::operator=(const Sphere &A) 
  /*!
    Default Assignment operator
    \param A :: Sphere to copy
    \return *this
  */
{
  if (this!=&A)
    {
      Quadratic::operator=(A);
      Centre=A.Centre;
      Radius=A.Radius;
    }
  return *this;
}

Sphere::~Sphere()
  /*!
    Destructor
  */
{}

bool
Sphere::operator==(const Sphere& A) const
  /*!
    Determine if two spheres are equal (within tolerance).
    \param A :: Sphere to compare
    \return A==This
  */
{
  if (&A==this) return 1;
  if (fabs(Radius-A.Radius)>Geometry::zeroTol)
    return 0;
  return (Centre.Distance(A.Centre)>Geometry::zeroTol) ? 0 : 1;
}

bool
Sphere::operator!=(const Sphere& A) const
  /*!
    Determine if two Spheres are not equal (within tolerance).
    \param A :: Sphere to compare
    \return A!=This
  */
{
  return !(this->operator==(A));
}


int
Sphere::setSurface(const std::string& Pstr)
  /*! 
    Processes a standard MCNPX cone string    
    Recall that cones can only be specified on an axis
     Valid input is: 
     - so radius 
     - s cen_x cen_y cen_z radius
     - sx - cen_x radius
    \return : 0 on success, neg of failure 
  */
{
  std::string Line=Pstr;
  std::string item;
  if (!StrFunc::section(Line,item) || 
      tolower(item[0])!='s' || item.length()>2)
    return -1;

  double cent[3]={0,0,0};
  double R;
  if (item.length()==2)       // sx/sy/sz
    {
      if (tolower(item[1])!='o')
        {
	  const int pType=static_cast<int>(tolower(item[1])-'x');
	  if (pType<0 || pType>2)
	    return -3;
	  if (!StrFunc::section(Line,cent[pType]))
	    return -4;
	}
    }
  else if (item.length()==1)
    {
      int index;
      for(index=0;index<3 && 
	    StrFunc::section(Line,cent[index]);index++) ;
      if (index!=3)
	return -5;
    }
  else
    return -6;
  if (!StrFunc::section(Line,R))
    return -7;

  Centre=Geometry::Vec3D(cent);
  Radius=R;
  setBaseEqn();
  return 0;
} 

int
Sphere::setSphere(const Geometry::Vec3D& Cpoint,const double R) 
  /*!
    Given a point and radius set the sphere
    \param Cpoint :: Centre point
    \param R :: radius of cylinder
    \retval 0 :: success / -ve on failure
  */
{
  if (R<=0)
    return -1;
  
  Centre=Cpoint;
  Radius=R;
  setBaseEqn();
  return 0;
}

int
Sphere::side(const Geometry::Vec3D& Pt) const
  /*!
     Calculate where the point Pt is relative to the 
     sphere.
     \param Pt :: Point to test
     \retval -1 :: Pt within sphere
     \retval 0 :: point on the surface (within CTolerance)
     \retval 1 :: Pt outside the sphere 
  */
{
  const Geometry::Vec3D Xv=Pt-Centre;
  return (Xv.dotProd(Xv)>Radius*Radius) ? 1 : -1;
}

int
Sphere::onSurface(const Geometry::Vec3D& Pt) const
  /*!
    Calculate if the point Pt on the surface of the sphere
    (within tolerance CTolerance)
    \param Pt :: Point to check
    \return 1 :: on the surfacae or 0 if not.
  */
{
  const Geometry::Vec3D Xv=Pt-Centre;
  return (fabs(Xv.dotProd(Xv)-Radius*Radius)>Geometry::zeroTol*Geometry::zeroTol) ? 0 : 1;
}

double
Sphere::distance(const Geometry::Vec3D& Pt) const
  /*! 
    Determine the shortest distance from the Surface 
    to the Point. 
    \param Pt :: Point to calculate distance from
    \return distance (Positive only)
  */
{
  const Geometry::Vec3D Amov=Pt-Centre;
  return fabs(Amov.abs()-Radius);
}


void
Sphere::displace(const Geometry::Vec3D& Pt) 
  /*!
    Apply a shift of the centre
    \param Pt :: distance to add to the centre
  */
{
  Centre+=Pt;
  Quadratic::displace(Pt);
  return;
}

void
Sphere::rotate(const Geometry::Matrix<double>& MA) 
  /*!
    Apply a Rotation matrix
    \param MA :: matrix to rotate by
  */
{
  Centre.rotate(MA);
  Quadratic::rotate(MA);
  return;
}

void
Sphere::mirror(const Geometry::Plane& MP)
  /*!
    Apply a mirror plane to the sphere
    This involves moving the centre and the 
    axis 
    \param MP :: Mirror point
   */
{
  MP.mirrorPt(Centre);
  setBaseEqn();
  return;
}

void 
Sphere::setCentre(const Geometry::Vec3D& A)
  /*!
    Set the centre point
    \param A :: New Centre Point
  */
{
  Centre=A;
  setBaseEqn();
  return;
}

void 
Sphere::setRadius(const double R)
  /*!
    Set the radius
    \param R :: New Radius
  */
{
  Radius=R;
  setBaseEqn();
  return;
}

void 
Sphere::setBaseEqn()
  /*!
    Sets an equation of type (general sphere)
    \f[ x^2+y^2+z^2+Gx+Hy+Jz+K=0 \f]
  */
{
  BaseEqn[0]=1.0;     // A x^2
  BaseEqn[1]=1.0;     // B y^2
  BaseEqn[2]=1.0;     // C z^2 
  BaseEqn[3]=0.0;     // D xy
  BaseEqn[4]=0.0;     // E xz
  BaseEqn[5]=0.0;     // F yz
  BaseEqn[6]= -2.0*Centre[0];     // G x
  BaseEqn[7]= -2.0*Centre[1];     // H y
  BaseEqn[8]= -2.0*Centre[2];     // J z
  BaseEqn[9]= Centre.dotProd(Centre)-Radius*Radius;        // K const
  return;
}

void 
Sphere::writeFLUKA(std::ostream& OX) const
  /*! 
    Object of write is to output a Fluka file [free format]
    \param OX :: Output stream (required for multiple std::endl)  
  */
{
  ELog::RegMethod RegA("Sphere","writeFLUKA");
  
  masterWrite& MW=masterWrite::Instance();
  std::ostringstream cx;
  cx<<"SPH s"<<getName()<<" "
    <<MW.Num(Centre)<<" "
    <<MW.Num(Radius);
  StrFunc::writeMCNPX(cx.str(),OX);
  return;
}

void 
Sphere::writePOVRay(std::ostream& OX) const
  /*! 
    Object of write is to output a POV-Ray file
    \param OX :: Output stream (required for multiple std::endl)  
  */
{
  ELog::RegMethod RegA("Sphere","writePOVRay");
  
  masterWrite& MW=masterWrite::Instance();
  
  OX << "#declare s"<<getName()
     <<" = sphere { <"<<MW.NumComma(Centre)<< ">, "
     << MW.Num(Radius)<<"}"<<std::endl;
  return;
}
  
void 
Sphere::write(std::ostream& OX) const
  /*! 
    Object of write is to output a MCNPX plane info 
    \param OX :: Output stream (required for multiple std::endl)  
    \todo (Needs precision) 
  */
{
  const char xyz[]="xyz";
  
  std::ostringstream cx;
  Quadratic::writeHeader(cx);
  cx.precision(Geometry::Nprecision);
  if (Centre.Distance(Geometry::Vec3D(0,0,0))<Geometry::zeroTol)
    cx<<"so "<<Radius;
  else
    {
      const size_t index(Centre.principleDir());
      if (Centre.abs()-std::abs(Centre[index])<Geometry::zeroTol)
	  cx<<"s"<<xyz[index]<<" "<<Centre[index]<<" "<<Radius;
      else
	cx<<"s "<<Centre<<" "<<Radius;
    }
  
  StrFunc::writeMCNPX(cx.str(),OX);
  return;
}

}  // NAMESPACE Geometry
