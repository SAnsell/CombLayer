/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   phitsSupport/MeshXYZ.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#include <iomanip>
#include <iostream>
#include <cmath>
#include <fstream>
#include <list>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <sstream>
#include <iterator>
#include <functional>
#include <algorithm>
#include <numeric>
#include <memory>
#include <boost/format.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h" 
#include "support.h"
#include "writeSupport.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Transform.h"
#include "MeshXYZ.h"

namespace phitsSystem
{

MeshXYZ::MeshXYZ() :
  NX(0),NY(0),NZ(0),TransPtr(0)
  /*!
    Constructor [makes XYZ mesh]
  */
{}

MeshXYZ::MeshXYZ(const MeshXYZ& A) : 
  minPoint(A.minPoint),maxPoint(A.maxPoint),NX(A.NX),
  NY(A.NY),NZ(A.NZ),TransPtr(A.TransPtr)
  /*!
    Copy constructor
    \param A :: MeshXYZ to copy
  */
{}

MeshXYZ&
MeshXYZ::operator=(const MeshXYZ& A)
  /*!
    Assignment operator
    \param A :: MeshXYZ to copy
    \return *this
  */
{
  if (this!=&A)
    {
      minPoint=A.minPoint;
      maxPoint=A.maxPoint;
      NX=A.NX;
      NY=A.NY;
      NZ=A.NZ;
    }
  return *this;
}
  
Geometry::Vec3D
MeshXYZ::point(const size_t a,const size_t b,const size_t c) const
  /*!
    Determine the 3d Vector corresponding the mesh point (a,b,c)
    \param a :: x index
    \param b :: y index
    \param c :: z index
    \return Vec3D point 
  */
{
  ELog::RegMethod RegA ("MeshXYZ","point");

  if (a >= NX)
    throw ColErr::IndexError<size_t>(a,NX,"X-coordinate");
  if (b >= NY)
    throw ColErr::IndexError<size_t>(b,NY,"Y-coordinate");
  if (c >= NZ)
    throw ColErr::IndexError<size_t>(c,NZ,"Z-coordinate");
  if (NX*NY*NZ==0) return minPoint; 

  Geometry::Vec3D D=maxPoint-minPoint;
  D[0]*= static_cast<double>(a)/NX;
  D[1]*= static_cast<double>(b)/NY;
  D[2]*= static_cast<double>(c)/NZ;
  
  return minPoint+D;
}
  
void
MeshXYZ::write(std::ostream& OX) const
  /*!
    Write out to a mesh
    \param OX :: output stream
  */
{
  ELog::RegMethod RegA("MeshXYZ","write");

  std::ostringstream cx;
  for
  return;
}

}   // NAMESPACE phitsSystem
