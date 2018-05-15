/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   weights/GridMesh.cxx
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
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "GridMesh.h"

namespace WeightSystem
{

GridMesh::GridMesh() : 
  type(XYZ),
  RefPoint(0,0.1,0),Origin(0,0,0),
  Vec(0,0.1,0),NX(0),NY(0),NZ(0)
  /*!
    Constructor [makes XYZ mesh]
  */
{}

GridMesh::GridMesh(const GridMesh& A) : 
  tallyN(A.tallyN),type(A.type),RefPoint(A.RefPoint),
  Origin(A.Origin),Axis(A.Axis),Vec(A.Vec),X(A.X),Y(A.Y),
  Z(A.Z),XFine(A.XFine),YFine(A.YFine),ZFine(A.ZFine),
  NX(A.NX),NY(A.NY),NZ(A.NZ)
  /*!
    Copy constructor
    \param A :: GridMesh to copy
  */
{}

GridMesh&
GridMesh::operator=(const GridMesh& A)
  /*!
    Assignment operator
    \param A :: GridMesh to copy
    \return *this
  */
{
  if (this!=&A)
    {
      type=A.type;
      RefPoint=A.RefPoint;
      Origin=A.Origin;
      Axis=A.Axis;
      Vec=A.Vec;
      X=A.X;
      Y=A.Y;
      Z=A.Z;
      XFine=A.XFine;
      YFine=A.YFine;
      ZFine=A.ZFine;
      NX=A.NX;
      NY=A.NY;
      NZ=A.NZ;
    }
  return *this;
}

void
GridMesh::setMeshType(const GeomENUM& A)
  /*!
    Set the type
    \param A :: Type
   */
{
  type=A;
  return;
}


void 
GridMesh::setMesh(const std::vector<double>& XV,
		    const std::vector<size_t>& XN,
		    const std::vector<double>& YV,
		    const std::vector<size_t>& YN,
		    const std::vector<double>& ZV,
		    const std::vector<size_t>& ZN)
  /*!
    Ugly way to set the mesh from the WWG constructor
    \param XV :: X-bounaries
    \param YV :: Y-bounaries
    \param ZV :: Z-bounaries
    \param XN :: X-bounaries sizes
    \param YN :: Y-bounaries sizes
    \param ZN :: Z-bounaries sizes
  */
{
  X=XV;
  Y=YV;
  Z=ZV;
  XFine=XN;
  YFine=YN;
  ZFine=ZN;
  NX=std::accumulate(XFine.begin(),XFine.end(),0UL);
  NY=std::accumulate(YFine.begin(),YFine.end(),0UL);
  NZ=std::accumulate(ZFine.begin(),ZFine.end(),0UL);
  return;
}

std::string
GridMesh::getType() const
  /*!
    Returns the type fo the mess as a string
    \return type-string
   */
{
  switch (type)
    {
    case 1:
      return "XYZ";
    case 2:
      return "CYL";
    case 3:
      return "SPH";
    }
  throw ColErr::InContainerError<int>(type,"WeghtMesh::getType");
}

double
GridMesh::getCoordinate(const std::vector<double>& Vec,
			  const std::vector<size_t>& NF,
			  const size_t Index)
  /*!
    Get coordinate from the vector set 
    Outer Boundary test of Index MUST have been done
    on entry to this code.
    \param Vec :: Boundary conditions
    \param NF :: Number in each boundary
    \param Index :: index value
    \return positions [Origin non - offset
   */
{
  size_t offset(NF[0]);
  size_t I(0);
  while(Index>offset)
    {
      offset+=NF[I];
      I++;
    }
  offset-=NF[I];
         
  
  return Vec[I]+static_cast<double>(Index-offset)*
    (Vec[I+1]-Vec[I])/static_cast<double>(NF[I]);
}
  
  
Geometry::Vec3D
GridMesh::point(const size_t a,const size_t b,const size_t c) const
  /*!
    Determine the 3d Vector corresponding the mesh point (a,b,c)
    \param a :: x index
    \param b :: y index
    \param c :: z index
    \return Vec3D point 
  */
{
  ELog::RegMethod RegA ("GridMesh","point");

  if (a >= NX)
    throw ColErr::IndexError<size_t>(a,NX,"X-coordinate");
  if (b >= NY)
    throw ColErr::IndexError<size_t>(b,NY,"Y-coordinate");
  if (c >= NZ)
    throw ColErr::IndexError<size_t>(c,NZ,"Z-coordinate");

  const double xc=getCoordinate(X,XFine,a);
  const double yc=getCoordinate(Y,YFine,b);
  const double zc=getCoordinate(Z,ZFine,c);
  return Geometry::Vec3D(xc,yc,zc);
}
  

}   // NAMESPACE WeightSystem
