/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geometry/Torus.cxx
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
#include <string>
#include <sstream>
#include <cmath>
#include <list>
#include <vector>
#include <map>
#include <stack>
#include <algorithm>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Quaternion.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Torus.h"

namespace Geometry
{

Torus::Torus() : Surface(),
		 Normal(1,0,0),Iradius(0),Oradius(0)
  /*!
    Constructor with centre line along X axis 
    and centre on origin
  */
{}
 
Torus::Torus(const Torus& A) : Surface(A),
  Centre(A.Centre), RotPhase(A.RotPhase),Normal(A.Normal), 
  Iradius(A.Iradius), Oradius(A.Oradius)
  /*!
    Standard Copy Constructor
    \param A :: Torus to copy
  */
{}

Torus*
Torus::clone() const
  /*! 
    Makes a clone (implicit virtual copy constructor) 
    \return new(*this)
  */
{
  return new Torus(*this);
}

Torus&
Torus::operator=(const Torus& A)
  /*!
    Assignment operator
    \param A :: Torus to copy
    \return *this
  */
{
  if(this!=&A)
    {
      Surface::operator=(A);
      Centre=A.Centre;
      RotPhase=A.RotPhase;
      Normal=A.Normal;
      Iradius=A.Iradius;
      Oradius=A.Oradius;
    }
  return *this;
}


Torus::~Torus()
  /*!
    Destructor
  */
{} 

bool
Torus::operator==(const Torus& A) const
  /*!
    Equality operator. Checks angle,centre and 
    normal separately
    \param A :: Torus to compare
    \return A==this within Tolerance
  */
{
  if(this==&A)
    return 1;

  if ( (fabs(Iradius-A.Iradius)>Geometry::zeroTol) ||
       (fabs(Oradius-A.Oradius)>Geometry::zeroTol) )
    return 0;
  if (RotPhase!=A.RotPhase)
    return 0;

  if (Centre.Distance(A.Centre)>Geometry::zeroTol)
    return 0;
  if (Normal.Distance(A.Normal)>Geometry::zeroTol)
    return 0;

  return 1;
}

bool
Torus::operator!=(const Torus& A) const
  /*!
    Inequality operator
    \param A :: Torus to compare
    \return A!=This
   */
{
  return !(this->operator==(A));
}

void 
Torus::setCentre(const Geometry::Vec3D& A)
  /*!
    Sets the central point and the Base Equation
    \param A :: New Centre point
  */
{
  Centre=A;
  return;
}

void
Torus::setNormal(const Vec3D& N)
  /*!
    Set the normal vector for the torus
    
    The normal vector (0,0,1) in the unrotated torus.
    To obtain the rotation us an axis vector normal to 
    (0,0,1) and N.
    \param N :: Normal Vector
   */
{
  if (N.abs()>Geometry::zeroTol)
    {
      Normal=N;
      Normal.makeUnit();            // In case not normal
      Vec3D Axis(-Normal.Y(),Normal.X(),0);
      Axis.makeUnit();
      RotPhase=Quaternion(2*Normal.Z(),Axis);
    }
  return;
}

void
Torus::setIRad(const double D)
  /*!
    Set the inner radius
    \param D :: Radius
   */
{
  Iradius=D;
  return;
}

void
Torus::setORad(const double D)
  /*!
    Set the outer radius
    \param D :: Radius
   */
{
  Oradius=D;
  return;
}


int 
Torus::setSurface(const std::string& Pstr)
  /*! 
    Processes a standard MCNPX cone string    
    Recall that cones can only be specified on an axis
     Valid input is: 
     - number {transformNumber} t/x cen_x cen_y cen_z a,b,c
    \param Pstr :: String to process
    \return : 0 on success, neg of failure 
  */
{
  enum { errDesc=-1, errAxis=-2,
	 errCent=-3, errNormal=-4};

  std::string Line=Pstr;

  std::string item;
  if (!StrFunc::section(Line,item) || 
      tolower(item[0])!='t' || item.length()!=2) 
    return errDesc;

  // Torus on X/Y/Z axis
  const int ptype=static_cast<int>(tolower(item[2])-'x');
  if (ptype<0 || ptype>=3)
    return errAxis;

  Geometry::Vec3D Norm;
  Geometry::Vec3D Cent;
  Geometry::Vec3D PtVec;

  // Torus on X/Y/Z axis
  Norm[ptype]=1.0;
  if (!StrFunc::section(Line,Centre))
    return errCent;
  if (!StrFunc::section(Line,PtVec))
    return errCent;
  
  Iradius=PtVec[1];
  Oradius=PtVec[2];

  return 0;
} 

void
Torus::rotate(const Geometry::Matrix<double>& R)
  /*!
    Rotate both the centre and the normal direction 
    \param R :: Matrix for rotation. 
  */
{
  Centre.rotate(R);
  Normal.rotate(R);
  setNormal(Normal);            // Trick to get quaterion set correctly
  return;
}

void 
Torus::displace(const Geometry::Vec3D& A)
  /*!
    Displace the centre
    Only need to update the centre position 
    \param A :: Point to add
  */
{
  Centre+=A;
  return;
}


void
Torus::mirror(const Geometry::Plane& MP)
  /*!
    Apply a mirror plane to the MBrect
    This involves moving the centre and the 
    axis 
    \param MP :: Mirror point
   */
{
  MP.mirrorPt(Centre);
  MP.mirrorAxis(Normal);
  return;
}


Geometry::Vec3D
Torus::surfaceNormal(const Geometry::Vec3D& Pt) const
  /*!
    Get the normal at a point.
    \f[ \frac{df/dx}=4x(x^2+y^2+z^2-r^2-R^2) \f]
    \f[ \frac{df/dy}=4y(x^2+y^2+z^2-r^2-R^2) \f]
    \f[ \frac{df/dz}=4y(x^2+y^2+z^2-r^2-R^2)+8R^2z \f]
    \param Pt :: New Normal 
    \return Normal
    \todo Not tested
  */
{
  Geometry::Vec3D Pcent=Pt-Centre;
  // Need axis to rotated to the z axis
  
  const double cosZA=acos(Normal.dotProd(Geometry::Vec3D(0,0,1)));
  
  
  if (fabs(cosZA)>Geometry::zeroTol)
    {
      Geometry::Vec3D rotAxis=Normal*Geometry::Vec3D(0,0,1);
      rotAxis.makeUnit();
      rotAxis/=sin(cosZA/2.0);
      Quaternion QR=Quaternion(cos(cosZA/2.0),rotAxis);
      QR.rotate(Pcent);
      const double DP=Pcent.dotProd(Pcent)-
	Iradius*Iradius-Oradius*Oradius;  // x^2+y^2+z^z-R^2-r^2
      const double x=Pcent.X();
      const double y=Pcent.Y();
      const double z=Pcent.Z();
      Geometry::Vec3D Out(4*x*DP,4*y*DP,4*z*(DP+2*Oradius*Oradius));
      QR.invRotate(Out);
      return Out;
    }
  const double DP=Pcent.dotProd(Pcent)-
    Iradius*Iradius-Oradius*Oradius;  // x^2+y^2+z^z-R^2-r^2
  const double x=Pcent.X();
  const double y=Pcent.Y();
  const double z=Pcent.Z();
  Geometry::Vec3D Out(4*x*DP,4*y*DP,4*z*(DP+2*Oradius*Oradius));
  return Out;
}


double
Torus::distance(const Geometry::Vec3D& Pt) const
  /*!
    Calculates the distance from the point to the Torus
    does not calculate the point on the Torus that is closest
    \param Pt :: Point to calcuate from

    - normalise to a cone vertex at the origin
    - calculate the angle between the axis and the Point
    - Calculate the distance to P
    \return distance to Pt
  */
{
  const Geometry::Vec3D Px=Pt-Centre;
  // test is the centre to point distance is zero
  if(Px.abs()<Geometry::zeroTol)
    return Px.abs();
  return Px.abs();
}

int
Torus::side(const Geometry::Vec3D&) const

  /*!
    Calculate if the point R is within
    the torus (return -1) or outside, 
    (return 1)
    \todo NEEDS ON SURFACE CALCULATING::
    waiting for a point to cone distance function
    \return Side of R
  */
{
  return -1;
}

int
Torus::onSurface(const Geometry::Vec3D&) const
  /*! 
     Calculate if the point R is on
     the cone (Note: have to be careful here
     since angle calcuation calcuates an angle.
     We need a distance for tolerance!)
     \param Pt :: Point to check
     \return 1 if on surface and -1 if not no surface
  */
{
  
  return -1;
}

void
Torus::write(std::ostream& OX) const
  /*!
    Write out the cone class in an mcnpx
    format.
    \param OX :: Output Stream (required for multiple std::endl)
  */
{
  //               -3 -2 -1 0 1 2 3        
  const char Tailends[]="zyx xyz";
  const int Ndir=Normal.masterDir(Geometry::zeroTol);
  if (Ndir==0)
    {
      OX<<"Torus failed"<<std::endl;
      return;
    }
  std::ostringstream cx;
  Surface::writeHeader(cx);
  cx<<" t"<<Tailends[Ndir+3]<<" ";
  cx.precision(Geometry::Nprecision);
  // Name and transform 
   
  cx<<Centre<<" "<<" "<<Oradius<<" "<<Iradius<<std::endl;
  StrFunc::writeMCNPX(cx.str(),OX);
  return;
}

void
Torus::writeFLUKA(std::ostream&) const
  /*!
    Write out the cone class in an mcnpx
    format.
    \param  :: Output Stream (required for multiple std::endl)
  */
{
  ELog::EM<<"Unsupported output"<<ELog::endErr;
  return;
}

void
Torus::writePOVRay(std::ostream&) const
  /*!
    Write out the cone class in a POV-Ray
    format.
    \param  :: Output Stream (required for multiple std::endl)
  */
{
  ELog::EM<<"Torus::writePOVRay: Unsupported output"<<ELog::endErr;
  return;
}

}  // NAMESPACE Geometry
