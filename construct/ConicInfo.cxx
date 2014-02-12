/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   t1Upgrade/ConicInfo.cxx
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
#include <string>
#include <algorithm>
#include <numeric>
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"

#include "ConicInfo.h"

namespace constructSystem
{

ConicInfo::ConicInfo(const Geometry::Vec3D& C,const Geometry::Vec3D& A,
		     const double ang,const int M,const double WT,
		     const int WM) :
  cylFlag(0),Cent(C),Axis(A),angle(ang),wall(WT),
  mat(M),wallMat(WM)
/*!
  Ugly constructor for Cent / Axis
  \param C :: Centre
  \param A :: Axis 
  \param ang :: Angle
  \param M :: Material in conic
  \param WT :: wall thickness
  \param WM :: wall Material
*/
{} 

ConicInfo::ConicInfo(const Geometry::Vec3D& C,const Geometry::Vec3D& A,
		     const double R,const int M,
		     const double WT,const int WM,
		     const int CF) :
  cylFlag(CF),Cent(C),Axis(A),angle(R),wall(WT),
  mat(M),wallMat(WM)
 /*!
   Ugly constructor for Cent / Axis
  \param C :: Centre
  \param A :: Axis 
  \param Radius :: Angle
  \param M :: Fill Material
  \param WT :: wall thickness
  \param WM :: wall Material
  \param CF :: cylinder flag 
*/
{} 

ConicInfo::ConicInfo(const ConicInfo& A) : 
  cylFlag(A.cylFlag),Cent(A.Cent),Axis(A.Axis),angle(A.angle),
  wall(A.wall),mat(A.mat),wallMat(A.wallMat)
  /*!
    Copy constructor
    \param A :: ConicInfo to copy
  */
{}

ConicInfo&
ConicInfo::operator=(const ConicInfo& A)
  /*!
    Assignment operator
    \param A :: ConicInfo to copy
    \return *this
  */
{
  if (this!=&A)
    {
      cylFlag=A.cylFlag;
      Cent=A.Cent;
      Axis=A.Axis;
      angle=A.angle;
      wall=A.wall;
      mat=A.mat;
      wallMat=A.wallMat;
    }
  return *this;
}

Geometry::Vec3D
ConicInfo::getCent(const Geometry::Vec3D& X,
		   const Geometry::Vec3D& Y,
		   const Geometry::Vec3D& Z) const
  /*!
    Get a re-based centre point
    \param X :: New X Basis vector    
    \param Y :: New Y Basis vector
    \param Z :: New Z Basis vector
    \return Re-based centre point
   */
{
  return X*Cent.X()+Y*Cent.Y()+Z*Cent.Z();
}

Geometry::Vec3D
ConicInfo::getAxis(const Geometry::Vec3D& X,
		   const Geometry::Vec3D& Y,
		   const Geometry::Vec3D& Z) const
  /*!
    Get a re-normalized unit vector for the axis
    \param X :: New X Basis vector    
    \param Y :: New Y Basis vector
    \param Z :: New Z Basis vector
    \return unit vector of axis
   */
{
  const Geometry::Vec3D R=X*Axis.X()+Y*Axis.Y()+Z*Axis.Z();
  return R.unit();
}



  
}  // NAMESPACE ts1System
