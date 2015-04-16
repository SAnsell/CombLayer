/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   geometry/Triangle.cxx
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <vector>
#include <map>
#include <string>
#include <algorithm>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "Surface.h"
#include "Triangle.h"

namespace Geometry
{

std::ostream&
operator<<(std::ostream& OX,const Triangle& A)
  /*!
    Standard output stream
    \param OX :: Output stream
    \param A :: Triangle to write
    \return Stream State
   */
{
  A.write(OX);
  return OX;
}

Triangle::Triangle(const Geometry::Vec3D& PtA,
		   const Geometry::Vec3D& PtB,
		   const Geometry::Vec3D& PtC) :
  index(0)
  /*!
    Constructor 
    \param PtA :: First point
    \param PtB :: Second point
    \param PtC :: Third point
  */
{
  setPoints(PtA,PtB,PtC);
}

Triangle::Triangle(const int I,
		   const Geometry::Vec3D& PtA,
		   const Geometry::Vec3D& PtB,
		   const Geometry::Vec3D& PtC) : 
  index(I) 
  /*!
    Constructor 
    \param I :: Index value
    \param PtA :: First point
    \param PtB :: Second point
    \param PtC :: Third point
  */
{
  setPoints(PtA,PtB,PtC);
}  

Triangle::Triangle(const Triangle& A) :
  index(A.index),NormV(A.NormV)
 /*!
    Copy Constructor 
    \param A :: Triangle object 
  */
{
  V[0]=A.V[0];
  V[1]=A.V[1];
  V[2]=A.V[2];
}

Triangle&
Triangle::operator=(const Triangle& A) 
  /*! 
    Assignment constructor
    \param A :: Triangle object 
    \return *this
  */
{
  if (this!=&A)
    {
      index=A.index;
      V[0]=A.V[0];
      V[1]=A.V[1];
      V[2]=A.V[2];
      NormV=A.NormV;
    }
  return *this;
}

const Geometry::Vec3D& 
Triangle::operator[](const int Index) const
  /*!
    Triangle accessor (const)
    \param Index :: Index point to get
    \return Vertex 
   */
{
  return V[Index % 3];
}

Geometry::Vec3D& 
Triangle::operator[](const int Index) 
  /*!
    Triangle accessor
    \param Index :: Index point to get
    \return Vertex 
   */
{
  return V[Index % 3];
}


void
Triangle::setPoints(const Geometry::Vec3D& PtA,
		    const Geometry::Vec3D& PtB,
		    const Geometry::Vec3D& PtC) 
  /*!
    Sets the triangle points
    \param PtA :: First point
    \param PtB :: Second point
    \param PtC :: Third point
  */
{
  V[0]=PtA;
  V[1]=PtB;
  V[2]=PtC;
  const Geometry::Vec3D LA=PtC-PtA;
  const Geometry::Vec3D LB=PtB-PtA;
  NormV=LA*LB;
  NormV.makeUnit();
  return;
}

double
Triangle::area() const 
  /*!
    Calculates the Area
    \return Area
  */
{
  return 0.5*((V[1]-V[0])*(V[2]-V[0])).abs();
}

Vec3D
Triangle::centre() const
  /*!
    Calculate the centre of mass
    \return bisection intersect
  */
{
  return (V[0]+V[1]+V[2])/3.0;
}

int
Triangle::valid(const Geometry::Vec3D& Pt) const
  /*!
    Carrys out an effective point in triangle
    calculate.
    Finds the projection of the sides of the triangle 
    within the point.
    \param Pt :: Point
    \retval 1 if inside the triangle
    \retval 0 if on the edge
    \retval -1 if outside the triangle
   */
{
  // Quick check as to in plane:
  const Vec3D c=Pt-V[0];
  if (NormV.dotProd(Pt)>Geometry::shiftTol)
    return 0;

  const Vec3D a=V[2]-V[0];
  const Vec3D b=V[1]-V[0];

  const double daa=a.dotProd(a);
  const double dab=a.dotProd(b);
  const double dac=a.dotProd(c);
  const double dbb=b.dotProd(b);
  const double dbc=c.dotProd(c);
  
  const double nVal = (daa*dbb-dab*dab);         
  if (nVal<Geometry::zeroTol)
    return 0;
  const double u = (dbb*dac - dab*dbc)/nVal;
  const double v = (daa*dbc - dab*dac)/nVal;
  
  if (u>Geometry::zeroTol && v>Geometry::zeroTol) 
    return 1;
  if (u<Geometry::zeroTol && v<Geometry::zeroTol) 
    return -1;
  return 0;
}

void
Triangle::write(std::ostream& OX) const
  /*!
    Debug write out1
    \param OX :: Output stream
   */
{
  OX<<"("<<index<<"):"<<V[0]<<" : "<<V[1]<<" : "<<V[2];
  return;
}


} // NAMESPACE Geometry
