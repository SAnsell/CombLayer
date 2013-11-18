/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   geometry/RecTriangle.cxx
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
#include <queue>
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
#include "Triangle.h"
#include "Quadrilateral.h"
#include "RecTriangle.h"

namespace Geometry
{

template<>
RecTriangle<Triangle>::RecTriangle(const Geometry::Vec3D& CPt,
				   const Geometry::Vec3D& AxisX,
				   const Geometry::Vec3D& AxisY) :
  level(0)
  /*!
    Constructor for Triangle:
    The grid goes from Centre Point +/- Axis values
    \param CPt :: Centre point
    \param AxisX :: X axis
    \param AxisY :: Y axis
  */
{
  activeList.push(Triangle(CPt,CPt-AxisX-AxisY,CPt-AxisX+AxisY));
  activeList.push(Triangle(CPt,CPt-AxisX+AxisY,CPt+AxisX+AxisY));
  activeList.push(Triangle(CPt,CPt+AxisX+AxisY,CPt+AxisX-AxisY));
  activeList.push(Triangle(CPt,CPt+AxisX-AxisY,CPt-AxisX-AxisY));
}

template<>
RecTriangle<Quadrilateral>::RecTriangle(const Geometry::Vec3D& CPt,
					const Geometry::Vec3D& AxisX,
					const Geometry::Vec3D& AxisY) :
  level(0)
  /*!
    Constructor for Quadrilateral
    \param CPt :: Centre point
    \param AxisX :: X axis
    \param AxisY :: Y axis
  */
{
  activeList.push(Quadrilateral(CPt-AxisX-AxisY,CPt-AxisX+AxisY,
				CPt+AxisX+AxisY,CPt+AxisX-AxisY));
}

template<typename Shape>
RecTriangle<Shape>::RecTriangle(const RecTriangle<Shape>& A) :
  level(A.level),activeList(A.activeList)
 /*!
    Copy Constructor 
    \param A :: RecTriangle object 
  */
{}

template<typename Shape>
RecTriangle<Shape>&
RecTriangle<Shape>::operator=(const RecTriangle<Shape>& A) 
  /*! 
    Assignment constructor
    \param A :: RecTriangle object 
    \return *this
  */
{
  if (this!=&A)
    {
      level=A.level;
      activeList=A.activeList;
    }
  return *this;
}

template<>
Vec3D
RecTriangle<Triangle>::getNext() 
  /*!
    Calculate the new centre
    \return bisection centre
  */
{
  ELog::RegMethod RegA("RecTriangle","getNext<Triangle>");

  const Triangle& TA=activeList.front();
  const int I=TA.getIndex();
  const Vec3D Cent=TA.centre();
  for(int i=0;i<3;i++)
    activeList.push(Triangle(I+1,Cent,TA[i],TA[i+1]));

  activeList.pop();
  if (I!=activeList.front().getIndex())
    level++;
  
  return Cent;
}

template<>
Vec3D
RecTriangle<Quadrilateral>::getNext() 
  /*!
    Calculate the new centre
    \return bisection centre
  */
{
  ELog::RegMethod RegA("RecTriangle","getNext<Qauadrilateral>");

  const Quadrilateral& TA=activeList.front();
  const int I=TA.getIndex();
  const Vec3D Cent=TA.centre();

  Vec3D midPt[4];
  for(int i=0;i<4;i++)
    midPt[i]=(TA[i]+TA[i+1])/2.0;
  for(int i=0;i<4;i++)
    activeList.push(Quadrilateral(I+1,Cent,midPt[(i+3) % 4],TA[i],midPt[i]));

  activeList.pop();
  if (I!=activeList.front().getIndex())
    level++;

  return Cent;
}

template class RecTriangle<Triangle>;
template class RecTriangle<Quadrilateral>;

} // NAMESPACE Geometry
