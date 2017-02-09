/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geometry/CylCan.cxx
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
#include <set>
#include <map>
#include <stack>
#include <string>
#include <algorithm>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "support.h"
#include "mathSupport.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "CylCan.h"

namespace Geometry
{
 
CylCan::CylCan() : Surface(),
   OPt(0,0,0),unitD(1,0,0),length(1.0),radius(1.0)
  /*!
    Constructor: sets cylinder along Z- axis
    at (0,0,0) length 1.0
  */
{
  makeSides();
}

CylCan::CylCan(const CylCan& A) : Surface(A),
  OPt(A.OPt),unitD(A.unitD),length(A.length),radius(A.radius)
  /*!
    Copy Constructor
    \param A :: CylCan to copy
  */
{}

CylCan*
CylCan::clone() const
  /*!
    Makes a clone (implicit virtual copy constructor) 
    \return new(this)
  */
{
  return new CylCan(*this);
}

CylCan&
CylCan::operator=(const CylCan& A) 
  /*!
    Assignment operator
    \param A :: CylCan to copy
    \return *this
  */
{
  if (&A!=this)
    {
      this->Surface::operator=(A);
      OPt=A.OPt;
      unitD=A.unitD;
      length=A.length;
      radius=A.radius;
      Sides=A.Sides;
    }
  return *this;
}

CylCan::~CylCan()
  /*!
    Destructor
  */
{}


bool
CylCan::operator==(const CylCan& A) const
  /*!
    Determine if two CylCans are equal (within tolerance).
    \param A :: CylCan to compare
    \return A==This
  */
{
  if (&A==this) return 1;
  if (fabs(A.length-length)>Geometry::zeroTol ||
      fabs(A.radius-radius)>Geometry::zeroTol)
    return 0;
  // Basic origin:
  if (OPt.Distance(A.OPt)<Geometry::zeroTol)
    return (unitD.Distance(A.unitD)<Geometry::zeroTol) ? 1 : 0;

  // shifted origin:
  if (OPt.Distance(A.OPt+unitD*A.length)<Geometry::zeroTol)
    return (unitD.Distance(-A.unitD)<Geometry::zeroTol) ? 1 : 0;
  
  return 0;
}

bool
CylCan::operator!=(const CylCan& A) const
  /*!
    Determine if two CylCans are not equal (within tolerance).
    \param A :: CylCan to compare
    \return A!=This
  */
{
  return !(this->operator==(A));
}

const Geometry::Plane&
CylCan::getPlane(const size_t Index) const
  /*!
    Access the Plane vectors (Sides)
    \param Index :: Index item
    \return Sides[index]
   */
{
  if (Index>=Sides.size())
    throw ColErr::IndexError<size_t>(Index,Sides.size(),"CylCan::getSides");
  return Sides[Index];
}

int
CylCan::setCylCan(const Geometry::Vec3D& A,const Geometry::Vec3D& B,
		  const double R) 
  /*!
    Given the start point and end point create
    a cylinder
    \param A :: Start Point
    \param B :: Axis Line
    \param R :: Radius 
    \retval 0 :: success
    \retval -1 :: Failed
  */
{
  Vec3D uVec=B;
  const double L=uVec.makeUnit();
  if (L<Geometry::zeroTol || R<=0)
    return -1;
  
  OPt=A;
  unitD=uVec;
  length=L;
  radius=R;
  makeSides();
  return 0;
}

int
CylCan::setCylCan(const Geometry::Vec3D& A,const Geometry::Vec3D& D,
		  const double L,const double R)
  /*!
    Set the cylinder given a length and a radius
    \param A :: Start Point
    \param D :: Central axis line
    \param L :: Length of cylinder
    \param R :: Radius 
    \retval 0 :: success
    \retval -1 :: Failed
  */
{
  if (R<=Geometry::zeroTol || L<=Geometry::zeroTol ||
      D.abs()<=Geometry::zeroTol)
    return -1;
  
  OPt=A;
  unitD=D;
  unitD.makeUnit();
  length=L;
  radius=R;
  makeSides();
  return 0;
}

int
CylCan::setSurface(const std::string& Pstr)
  /*! 
     Processes a standard MCNPX plane string    
     - rpp : Corner : Corner 
     \param Pstr :: String to make into a rectangle
     \return 0 on success, -ve of failure
  */
{
  std::string Line=Pstr;
  
  int nx;
  if (StrFunc::section(Line,nx))
    setName(nx);                       // Assign the name


  std::string Btype;
  if (!StrFunc::section(Line,Btype))
    return -1;
  
  if (Btype!="rcc" && Btype!="RCC")
    return -2;

  Vec3D C,Axis;
  double R,L;

  if (!StrFunc::section(Line,C) || 
      !StrFunc::section(Line,Axis) || 
      !StrFunc::section(Line,R))
    return  -3;

  // return (-1 on fail)*4
  return (StrFunc::section(Line,L)) ? 
    4*(setCylCan(C,Axis,L,R)) :
    4*(setCylCan(C,Axis,R));         // return (-1 on fail)*4
}

void
CylCan::rotate(const Geometry::Matrix<double>& MA) 
  /*!
    Rotate the plane about the origin by MA 
    \param MA direct rotation matrix (3x3)
  */
{
  OPt.rotate(MA);
  unitD.rotate(MA);
  makeSides();
  return;
}

void
CylCan::displace(const Geometry::Vec3D& Sp) 
  /*!
    Displace the centre by Point Sp.  
    i.e. r+sp now on the rectable
    \param Sp :: point value of displacement
  */
{
  OPt+=Sp;
  makeSides();
  return;
}

void
CylCan::mirror(const Geometry::Plane& MP)
  /*!
    Apply a mirror plane to the ArbPoly
    This involves moving the centre and the 
    axis 
    \param MP :: Mirror point
   */
{
  MP.mirrorPt(OPt);
  MP.mirrorAxis(unitD);

  for(Geometry::Plane& sc : Sides)
    sc.mirror(MP);

  return;
}

double
CylCan::distance(const Geometry::Vec3D& Pt) const
  /*!
    Determine the distance of point Pt from the planes 
    returns a value relative to the normal
    See diagram in notes 
    \param Pt :: point to get distance from 
    \returns distance from the surface to the point
  */
{
  // First find the normal going to the point
  Vec3D Amov=Pt-OPt;
  const double lambda=Amov.dotProd(unitD);
  // Distance to infinite plane
  const Vec3D Ccut= unitD*lambda;
//  const double CylD=fabs(Ccut.Distance(Amov)-radius);

  // plane closest point 
  if (lambda>length/2)
    Amov+=unitD*length;
  const double Aabs=Amov.abs();
  const double PDvert(Amov.dotProd(unitD));
  const double PDhorr(sqrt(PDvert*PDvert+Aabs*Aabs));
  const double outR=PDhorr-radius;

  // Above half:
  if (lambda>length/2)
    {
      // sector B : within cylinder & below plane
      // sector C : outside cylinder & below plane
      if (lambda<length)
	return std::min(fabs(outR),PDvert);
      
      // sector A : within cylinder & above plane
      // sector D : outside cylinder & above plane
      return (outR<0) ?  PDvert : sqrt(outR*outR+PDvert*PDvert);
    }

  // Lower half

  // sector B : within cylinder & above plane
  // sector C : outside cylinder & above plane
  if (lambda>=0.0)
    return std::min(fabs(outR),PDvert);
      
  // sector A : within cylinder & below plane
  // sector D : outside cylinder & below plane
  return (outR<0) ?  PDvert : sqrt(outR*outR+PDvert*PDvert);
}

void
CylCan::makeSides()
  /*!
    Build the edge surface planes
   */
{
  Sides.resize(2);
  Sides[0].setPlane(OPt,-unitD);
  Sides[1].setPlane(OPt+unitD*length,unitD);
  return;
}

int
CylCan::side(const Geometry::Vec3D& Pt) const
  /*!
    Calcualates the side that the point is on a side
    \param Pt :: test point
    \retval 1 :: Inside the box
    \retval -1 :: Outside the box
    \retval 0 :: Pt is on the plane itself (within tolerence) 
  */
{
  // First find the normal going to the point
  Vec3D Amov=Pt-OPt;
  const double lambda=Amov.dotProd(unitD);
  // Distance to infinite plane
  const Vec3D Ccut= unitD*lambda;
//  const double CylD=fabs(Ccut.Distance(Amov)-radius);

  // plane closest point 
  if (lambda>length/2)
    Amov+=unitD*length;
  const double Aabs=Amov.abs();
  const double PDvert(Amov.dotProd(unitD));
  const double PDhorr(sqrt(PDvert*PDvert+Aabs*Aabs));
  const double outR=PDhorr-radius;

  // Above half:
  if (lambda>length/2)
    {
      // below Plane
      if (lambda<length-Geometry::parallelTol)
        {
	  if (outR<-Geometry::zeroTol)
	    return 1;
	  return (outR>Geometry::zeroTol) ? 1 : 0;
	}
      // On plane
      if (lambda<length+Geometry::parallelTol)    
	return (outR<=Geometry::zeroTol) ? -1 : 0;

     // above plane 
     return -1;      
    }

  // above Plane
  if (lambda>Geometry::parallelTol)
    {
      if (outR<-Geometry::zeroTol)
	return 1;
      return (outR>Geometry::zeroTol) ? 1 : 0;
    }
  // On plane
  if (lambda > -Geometry::parallelTol)    
    return (outR<=Geometry::zeroTol) ? -1 : 0;

  // below plane 
  return -1;      
}

Geometry::Vec3D 
CylCan::surfaceNormal(const Geometry::Vec3D& Pt) const
  /*!
    Calculate the normal at the point
    \param Pt :: Point to use
    \return normal at point
    \todo To be written
  */
{
  return Pt;
}

int
CylCan::onSurface(const Geometry::Vec3D& A) const
  /*! 
     Calcuate the side that the point is on
     and returns success if it is on the surface.
     - Uses CTolerance to determine the closeness
     \param A :: Point to test
     \retval 1 if on the surface 
     \retval 0 if off the surface 
     
  */
{
  return (side(A)!=0) ? 0 : 1;
}

void 
CylCan::print() const
  /*!
    Prints out the surface info and
    the CylCan info.
  */
{
  Surface::print();
  std::cout<<"Origin = "<<OPt<<" Axis="<<unitD<<std::endl;
  std::cout<<"Radius = "<<radius<<" lenght = "<<length<<std::endl;
  return;
}

void
CylCan::writeFLUKA(std::ostream&) const
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
CylCan::writePOVRay(std::ostream&) const
  /*!
    Write out the cone class in a POV-Ray format.
    \param  :: Output Stream (required for multiple std::endl)
  */
{
  ELog::EM<<"CylCan::writePOVRay Unsupported output"<<ELog::endErr;
  return;
}


void 
CylCan::write(std::ostream& OX) const
  /*! 
    Object of write is to output a MCNPX plane info 
    \param OX :: Output stream (required for multiple std::endl)  
  */
{
  std::ostringstream cx;
  cx.precision(Geometry::Nprecision);
  
  cx<<"rcc "<<OPt<<" "<<unitD*length<<" "<<radius;
  StrFunc::writeMCNPX(cx.str(),OX);
  return;
}

  
  
}  // NAMESPACE Geometry
