/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   phitsSupport/MeshXYZ.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
  nBins({0,0,0}),
  logSpace({0,0,0}),
  TransPtr(0)
  /*!
    Constructor [makes XYZ mesh]
  */
{}

MeshXYZ::MeshXYZ(const MeshXYZ& A) : 
  nBins(A.nBins),logSpace(A.logSpace),
  minPoint(A.minPoint),maxPoint(A.maxPoint),TransPtr(A.TransPtr)
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
      nBins=A.nBins;
      logSpace=A.logSpace;
      minPoint=A.minPoint;
      maxPoint=A.maxPoint;
    }
  return *this;
}

size_t
MeshXYZ::getZeroIndex() const
   /*!
     Determine if one bin in singular
     \throw if no none zero/unit bin or two such bins
     \return Index  [0-2] 
    */
{
  ELog::RegMethod RegA("MeshXYZ","getZeroIndex");
  size_t zUnit=0;
    
  for(size_t i=0;i<3;i++)
    if (nBins[i]<2)
      {
	if (zUnit) zUnit=-10;
	zUnit=i+1;
      }

  if (zUnit<1)
    throw ColErr::DimensionError<3,size_t>
      ({nBins[0],nBins[1],nBins[2]},{2,2,2},"Array need only one zero/one");

  return zUnit-1;
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

  if (a >= nBins[0])
    throw ColErr::IndexError<size_t>(a,nBins[0],"X-coordinate");
  if (b >= nBins[1])
    throw ColErr::IndexError<size_t>(b,nBins[1],"Y-coordinate");
  if (c >= nBins[2])
    throw ColErr::IndexError<size_t>(c,nBins[2],"Z-coordinate");


  Geometry::Vec3D D=maxPoint-minPoint;
  // make for logspace

  D[0]*= 0.5+static_cast<double>(a)/static_cast<double>(nBins[0]);
  D[1]*= 0.5+static_cast<double>(b)/static_cast<double>(nBins[1]);
  D[2]*= 0.5+static_cast<double>(c)/static_cast<double>(nBins[2]);
  
  return minPoint+D;
}

void
MeshXYZ::setSize(const size_t XP,const size_t YP,const size_t ZP)
  /*!
    Set the mesh size [number of points]
    \param XP :: Number of X points
    \param YP :: Number of Y points
    \param ZP :: Number of Z points
   */
{
  nBins={XP,YP,ZP};
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
MeshXYZ::write2D(std::ostream& OX) const
/*!
  Write out the mesh as if it is a 2d plane
  \param OX :: Output stream
*/
{
  ELog::RegMethod RegA("MeshXYZ","write2D");

  const std::string txyz[]={"x","y","z"};
  const std::string axyz[]={"yz","xz","xy"};
  OX<<"  mesh = xyz \n";
  const size_t nullIndex=getZeroIndex();
    
  for(size_t index=0;index<3;index++)
    {
      const std::string tx=txyz[index];
      if (nullIndex!=index)
	{
	  OX<<"  "<<tx<<"-type = 2 \n";
	  OX<<"    n"<<tx<<" = "<<nBins[index]<<"\n";
	  OX<<"  "<<tx<<"min = "<<minPoint[index]<<"\n";
	  OX<<"  "<<tx<<"max = "<<maxPoint[index]<<"\n";
	}
      else
	{
	  OX<<"  "<<tx<<"-type = 1 \n";
	  OX<<"    n"<<tx<<" = "<<1<<"\n";
	  OX<<"        "<<minPoint[index]<<" "<<maxPoint[index]<<"\n";
	}
    }
  // this must be after mesh:
  OX<<"  axis = "<<axyz[nullIndex]<<"\n";

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

  if (nBins[0]*nBins[1]*nBins[2]==0) return;

  std::string spc("  ");
  const std::string txyz[]={"x","y","z"};
  
  OX<<spc<<"mesh = xyz\n";
  spc+="  ";
  for(size_t i=0;i<3;i++)
    {
      const std::string xyzT=txyz[i];
      if (nBins[i]>1)
	{
	  OX<<spc<<xyzT<<"-type = "<<(2+logSpace[i])<<"\n";
	  OX<<spc<<"n"<<xyzT<<" = "<<nBins[i]<<"\n";
	  OX<<spc<<xyzT<<"min = "<<minPoint[i]<<"\n";
	  OX<<spc<<xyzT<<"max = "<<maxPoint[i]<<"\n";
	}
      else
	{
	  OX<<spc<<xyzT<<"-type = 1\n";
	  OX<<spc<<"n"<<xyzT<<" = 1 \n";
	  OX<<spc<<"     "<<minPoint[i]<<" "<<maxPoint[i]<<"\n";
	}
    }
  return;
}

}   // NAMESPACE phitsSystem
