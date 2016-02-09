/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   weights/WeightMesh.cxx
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
#include "WeightMesh.h"

namespace WeightSystem
{

WeightMesh::WeightMesh() : 
  tallyN(1),type(XYZ),
  RefPoint(0,0.1,0),Origin(0,0,0),
  Vec(0,0.1,0),NX(0),NY(0),NZ(0)
  /*!
    Constructor [makes XYZ mesh]
  */
{}

WeightMesh::WeightMesh(const WeightMesh& A) : 
  tallyN(A.tallyN),type(A.type),RefPoint(A.RefPoint),
  Origin(A.Origin),Axis(A.Axis),Vec(A.Vec),X(A.X),Y(A.Y),
  Z(A.Z),XFine(A.XFine),YFine(A.YFine),ZFine(A.ZFine),
  NX(A.NX),NY(A.NY),NZ(A.NZ)
  /*!
    Copy constructor
    \param A :: WeightMesh to copy
  */
{}

WeightMesh&
WeightMesh::operator=(const WeightMesh& A)
  /*!
    Assignment operator
    \param A :: WeightMesh to copy
    \return *this
  */
{
  if (this!=&A)
    {
      tallyN=A.tallyN;
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
WeightMesh::setMeshType(const GeomENUM& A)
  /*!
    Set the type
    \param A :: Type
   */
{
  type=A;
  return;
}


void 
WeightMesh::setMesh(const std::vector<double>& XV,
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
  Origin=Geometry::Vec3D(X.front(),Y.front(),Z.front());
  return;
}

std::string
WeightMesh::getType() const
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
WeightMesh::getCoordinate(const std::vector<double>& Vec,
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
    (Vec[I+1]-Vec[I])/NF[I];
}
  
  
Geometry::Vec3D
WeightMesh::point(const size_t a,const size_t b,const size_t c) const
  /*!
    Determine the 3d Vector corresponding the mesh point (a,b,c)
    \param a :: x index
    \param b :: y index
    \param c :: z index
    \return Vec3D point 
  */
{
  ELog::RegMethod RegA ("WeightMesh","point");

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

void
WeightMesh::writeWWINP(std::ostream& OX,const size_t NEBin) const
  /*!
    Write out to a mesh to a wwinp file
    Currently ONLY works correctly with a rectangular file
    \param OX :: Output stream
    \param NEBin :: Number of energy bins
  */
{
  ELog::RegMethod RegA("WeightMesh","writeWWINP");

  boost::format TopFMT("%10i%10i%10i%10i%28s\n");
  const std::string date("10/07/15 15:37:51");
  OX<<(TopFMT % tallyN % 1 % 1 % 10 % date);

  // Energy bins
  OX<<std::setw(10)<<NEBin<<std::endl;

  // Write Mesh:
  size_t itemCnt(0);
  StrFunc::writeLine(OX,NX,itemCnt,6);
  StrFunc::writeLine(OX,NY,itemCnt,6);
  StrFunc::writeLine(OX,NZ,itemCnt,6);
  StrFunc::writeLine(OX,Origin,itemCnt,6);

  StrFunc::writeLine(OX,XFine.size(),itemCnt,4);
  StrFunc::writeLine(OX,YFine.size(),itemCnt,4);
  StrFunc::writeLine(OX,ZFine.size(),itemCnt,4);
  StrFunc::writeLine(OX,1.0,itemCnt,4);

  // loop over X/Y/Z
  for(size_t index=0;index<3;index++)
    {
      const std::vector<double>& Vec=
	((!index) ? X : (index==1) ? Y : Z);
      const std::vector<size_t>& iVec=
	((!index) ? XFine : (index==1) ? YFine : ZFine);

      itemCnt=0;
      StrFunc::writeLine(OX,Vec[0],itemCnt,6);

      for(size_t i=0;i<iVec.size();i++)
	{
	  StrFunc::writeLine(OX,iVec[i],itemCnt,6);
	  StrFunc::writeLine(OX,Vec[i+1],itemCnt,6);
	  StrFunc::writeLine(OX,1.0,itemCnt,6);
		  
	}
      if (itemCnt) OX<<std::endl;
    }
  
  return;
}

  
void
WeightMesh::write(std::ostream& OX) const
  /*!
    Write out to a mesh
    \param OX :: output stream
  */
{
  ELog::RegMethod RegA("WeightMesh","write");

  std::ostringstream cx;
  cx<<"mesh   geom="<<getType()<<" origin="<<Origin
    <<" ref="<<RefPoint;
  StrFunc::writeMCNPX(cx.str(),OX);

  // imesh :
  char c[3]={'i','j','k'};
  std::vector<double>::const_iterator vc;
  for(size_t i=0;i<3;i++)
    {
      const std::vector<double>& Vec=
	((!i) ? X : (i==1) ? Y : Z);
      const std::vector<size_t>& iVec=
	((!i) ? XFine : (i==1) ? YFine : ZFine);
      
      cx.str("");
      cx<<c[i]<<"mesh ";
      for(size_t i=1;i<Vec.size();i++)
	cx<<Vec[i]<<" ";
      cx<<c[i]<<"ints";
      for(const size_t& iVal : iVec)
	cx<<" "<<iVal;
      
      StrFunc::writeMCNPXcont(cx.str(),OX);
    }
  return;
}

}   // NAMESPACE WeightSystem
