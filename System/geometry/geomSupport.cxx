/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   geometry/geomSupport.cxx
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#include <complex>
#include <cmath>
#include <vector>
#include <map>
#include <string>
#include <algorithm>

#include "Exception.h"
#include "GTKreport.h"
#include "FileReport.h"
#include "NameStack.h"
#include "OutputLog.h"
#include "RegMethod.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "geomSupport.h"

namespace Geometry
{

Geometry::Vec3D
cornerCircleTouch(const Geometry::Vec3D& C,
		  const Geometry::Vec3D& A,
		  const Geometry::Vec3D& B,
		  const double radius)
  /*!
    Given a cirlce of radius R, find the centre 
    relative to the two lines between A-C and B-C
    
    Lines must not be parallel.
    
    \param C :: Corner
    \param A :: Extend point (1)
    \param B :: Extend point (2)
    \param radius :: Radius of circle
    \return Centre point of circle
  */
{
  ELog::RegMethod RegA("geomSupport[F]","cornerCircleTouch");

  const Geometry::Vec3D a=(A-C).unit();
  const Geometry::Vec3D b=(B-C).unit();

  const Geometry::Vec3D midDivide=((a+b)/2.0).unit();
  
  const double aDotM=a.dotProd(midDivide);
  const double lambda=sqrt(radius*radius/(1.0-aDotM*aDotM));

  return C+midDivide*lambda;
}

std::pair<Geometry::Vec3D,Geometry::Vec3D>
cornerCircle(const Geometry::Vec3D& C,
	     const Geometry::Vec3D& A,
	     const Geometry::Vec3D& B,
	     const double radius)
  /*!
    Given a circle of radius R, find the centre 
    relative to the two lines between A-C and B-C
    
    Lines must not be parallel.
    
    \param C :: Corner
    \param A :: Extend point (1)
    \param B :: Extend point (2)
    \param radius :: Radius of circle
    \return Edge 1  + Edge 2
  */
{
  ELog::RegMethod RegA("geomSupport[F]","cornerCircleTouch");

  const Geometry::Vec3D a=(A-C).unit();
  const Geometry::Vec3D b=(B-C).unit();

  const Geometry::Vec3D midDivide=((a+b)/2.0).unit();
  
  const double aDotM=a.dotProd(midDivide);
  const double lambda=sqrt(radius*radius/(1.0-aDotM*aDotM));

  Geometry::Vec3D RCent=midDivide*lambda;   // c out by step*midDivde
  const double alpha=a.dotProd(RCent);
  const double beta=b.dotProd(RCent);

  return std::pair<Geometry::Vec3D,Geometry::Vec3D>
    (C+a*alpha,C+b*beta);
}

}  // NAMESPACE Geometry
