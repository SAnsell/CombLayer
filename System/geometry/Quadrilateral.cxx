/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   geometry/Quadrilateral.cxx
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
#include "Vec3D.h"
#include "Quadrilateral.h"

namespace Geometry
{

std::ostream&
operator<<(std::ostream& OX,const Quadrilateral& A)
  /*!
    Standard output stream
    \param OX :: Output stream
    \param A :: Quadrilateral to write
    \return Stream State
   */
{
  A.write(OX);
  return OX;
}

Quadrilateral::Quadrilateral(const Geometry::Vec3D& PtA,
			     const Geometry::Vec3D& PtB,
			     const Geometry::Vec3D& PtC, 
			     const Geometry::Vec3D& PtD) :
  index(0)
  /*!
    Constructor 
    \param PtA :: First point
    \param PtB :: Second point
    \param PtC :: Third point
    \param PtD :: Fourth point
  */
{
  setPoints(PtA,PtB,PtC,PtD);
}

Quadrilateral::Quadrilateral(const int I,
			     const Geometry::Vec3D& PtA,
			     const Geometry::Vec3D& PtB,
			     const Geometry::Vec3D& PtC, 
			     const Geometry::Vec3D& PtD) : 
  index(I) 
  /*!
    Constructor 
    \param I :: Index value
    \param PtA :: First point
    \param PtB :: Second point
    \param PtC :: Third point
    \param PtD :: Fourth point
  */
{
  setPoints(PtA,PtB,PtC,PtD);
}  

Quadrilateral::Quadrilateral(const Quadrilateral& A) :
  index(A.index),NormV(A.NormV)
 /*!
    Copy Constructor 
    \param A :: Quadrilateral object 
  */
{
  for(int i=0;i<4;i++)
    V[i]=A.V[i];
}

Quadrilateral&
Quadrilateral::operator=(const Quadrilateral& A) 
  /*! 
    Assignment constructor
    \param A :: Quadrilateral object 
    \return *this
  */
{
  if (this!=&A)
    {
      index=A.index;
      for(int i=0;i<4;i++)
	V[i]=A.V[i];
      NormV=A.NormV;
    }
  return *this;
}

const Geometry::Vec3D& 
Quadrilateral::operator[](const int Index) const
  /*!
    Quadrilateral accessor (const)
    \param Index :: Index point to get
    \return Vertex 
   */
{
  return V[Index % 4];
}

Geometry::Vec3D& 
Quadrilateral::operator[](const int Index) 
  /*!
    Quadrilateral accessor
    \param Index :: Index point to get
    \return Vertex 
   */
{
  return V[Index % 4];
}


void
Quadrilateral::setPoints(const Geometry::Vec3D& PtA,
			 const Geometry::Vec3D& PtB,
			 const Geometry::Vec3D& PtC,
			 const Geometry::Vec3D& PtD) 
  /*!
    Sets the Quadrilateral points
    \param PtA :: First point
    \param PtB :: Second point
    \param PtC :: Third point
    \param PtD :: Fourth point
  */
{
  V[0]=PtA;
  V[1]=PtB;
  V[2]=PtC;
  V[3]=PtD;
  const Geometry::Vec3D LA=PtC-PtA;
  const Geometry::Vec3D LB=PtB-PtA;
  NormV=LA*LB;
  NormV.makeUnit();
  return;
}

double
Quadrilateral::area() const 
  /*!
    Calculates the Area
    \return Area
  */
{
  double area=0.5*((V[1]-V[0])*(V[2]-V[0])).abs();  
  area+=0.5*((V[1]-V[3])*(V[2]-V[3])).abs();
  return area;
}

Vec3D
Quadrilateral::centre() const
  /*!
    Calculate the centre of mass
    \return bisection intersect
  */
{
  return (V[0]+V[1]+V[2]+V[3])/4.0;
}

void
Quadrilateral::write(std::ostream& OX) const
  /*!
    Debug write out1
    \param OX :: Output stream
   */
{
  OX<<"("<<index<<"):"<<V[0]<<" : "<<V[1]<<" : "<<V[2]<<" : "<<V[3];
  return;
}


} // NAMESPACE Geometry
