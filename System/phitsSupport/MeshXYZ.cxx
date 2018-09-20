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
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Transform.h"
#include "MeshXYZ.h"

namespace phitsSystem
{

MeshXYZ::MeshXYZ() :
  logSpace({0,0,0}),NX(0),NY(0),NZ(0),TransPtr(0)
  /*!
    Constructor [makes XYZ mesh]
  */
{}

  
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
  // make for logspace
  D[0]*= 0.5+static_cast<double>(a)/static_cast<double>(NX);
  D[1]*= 0.5+static_cast<double>(b)/static_cast<double>(NY);
  D[2]*= 0.5+static_cast<double>(c)/static_cast<double>(NZ);
  
  return minPoint+D;
}

void
MeshXYZ::setSize(const size_t XP,const size_t YP,const size_t ZP)
{
  NX=XP;
  NY=YP;
  NZ=ZP;
  return;
}

void
MeshXYZ::setCoordinates(const Geometry::Vec3D& A,
			const Geometry::Vec3D& B)
  /*!
    Sets the min/max coordinates
    \param A :: First coordinate
    \param B :: Second coordinate
  */
{
  ELog::RegMethod RegA("MeshXYZ","setCoordinates");
  
  minPoint=A;
  maxPoint=B;
  // Add some checking here
  for(size_t i=0;i<3;i++)
    {
      if (std::abs(minPoint[i]-maxPoint[i])<Geometry::zeroTol)
	throw ColErr::NumericalAbort(StrFunc::makeString(minPoint)+" ::: "+
				     StrFunc::makeString(maxPoint)+
				     " Equal components");
      if (minPoint[i]>maxPoint[i])
	std::swap(minPoint[i],maxPoint[i]);
    }
  return;
}

void
MeshXYZ::write(std::ostream& OX) const
  /*!
    Write out to a mesh
    \param OX :: output stream
  */
{
  ELog::RegMethod RegA("MeshXYZ","write");

  if (NX*NY*NZ==0) return;

  OX<<"mesh = xyz\n";

  if (NX>1)
    {
      OX<<"x-type = "<<(2+logSpace[0])<<"\n";
      OX<<"nx = "<<NX<<"\n";
      OX<<"xmin = "<<minPoint[0]<<"\n";
      OX<<"xmax = "<<maxPoint[0]<<"\n";
    }
  else
    {
      OX<<"x-type = 1\n";
      OX<<"nx = 1 \n";
      OX<<"     "<<minPoint[0]<<" "<<maxPoint[0]<<"\n";
    }

  if (NY>1)
    {
      OX<<"y-type = "<<(2+logSpace[1])<<"\n";
      OX<<"ny = "<<NY<<"\n";
      OX<<"ymin = "<<minPoint[1]<<"\n";
      OX<<"ymax = "<<maxPoint[1]<<"\n";
    }
  else
    {
      OX<<"y-type = 1\n";
      OX<<"ny = 1 \n";
      OX<<"    "<<minPoint[1]<<" "<<maxPoint[1]<<"\n";
    }

  if (NZ>1)
    {
      OX<<"x-type = "<<(2+logSpace[2])<<"\n";
      OX<<"nz = "<<NZ<<"\n";
      OX<<"zmin = "<<minPoint[2]<<"\n";
      OX<<"zmax = "<<maxPoint[2]<<"\n";
    }
  else
    {
      OX<<"z-type = 1\n";
      OX<<"nz = 1 \n";
      OX<<"     "<<minPoint[2]<<" "<<maxPoint[2]<<"\n";
    }
    
  return;
}

}   // NAMESPACE phitsSystem
