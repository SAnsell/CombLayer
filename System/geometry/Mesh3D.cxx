/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geometry/Mesh3D.cxx
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#include "Mesh3D.h"

namespace Geometry
{

Mesh3D::Mesh3D() : 
  type(XYZ),writeFlag(0),
  RefPoint(0,0.1,0),Origin(0,0,0),
  Vec(0,0.1,0),NX(0),NY(0),NZ(0)
  /*!
    Constructor [makes XYZ mesh]
  */
{}

Mesh3D::Mesh3D(const Mesh3D& A) : 
  type(A.type),writeFlag(A.writeFlag),RefPoint(A.RefPoint),
  Origin(A.Origin),Axis(A.Axis),Vec(A.Vec),X(A.X),Y(A.Y),
  Z(A.Z),XFine(A.XFine),YFine(A.YFine),ZFine(A.ZFine),
  NX(A.NX),NY(A.NY),NZ(A.NZ)
  /*!
    Copy constructor
    \param A :: Mesh3D to copy
  */
{}

Mesh3D&
Mesh3D::operator=(const Mesh3D& A)
  /*!
    Assignment operator
    \param A :: Mesh3D to copy
    \return *this
  */
{
  if (this!=&A)
    {
      type=A.type;
      writeFlag=A.writeFlag;
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
Mesh3D::setMeshType(const GeomENUM& A)
  /*!
    Set the type
    \param A :: Type
   */
{
  type=A;
  return;
}


void 
Mesh3D::setMesh(const std::vector<double>& XV,
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

void
Mesh3D::setRefPt(const Geometry::Vec3D& Pt)
  /*!
    Sets the reference point
    \param Pt :: Point to reference             
    \todo Handle master rotation
  */
{
  writeFlag ^= 1;
  RefPoint=Pt;
  return;
} 

std::string
Mesh3D::getType() const
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
Mesh3D::getCoordinate(const std::vector<double>& Vec,
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

std::vector<Geometry::Vec3D>
Mesh3D::midPoints() const
  /*!
    Calculate a linearized mid point list
    Retruns (NX-1*NY-1*NZ-1) points indexing
    on X [highest] Y[mid] Z [lowest] index
    \return vector of point
   */
{
  ELog::RegMethod RegA("Mesh3D","midPoints");

  std::vector<Geometry::Vec3D> midPt;
  if (!NX*NY*NZ) return midPt;           // origin on failure

  double xA,xB,yA,yB,zA,zB;
  xB=getXCoordinate(0);
  for(size_t i=1;i<=NX;i++)
    {
      xA=xB;
      xB=getXCoordinate(i);
      yB=getYCoordinate(0);
      for(size_t j=1;j<=NY;j++)
        {
          yA=yB;
          yB=getYCoordinate(j);
          zB=getZCoordinate(0);
          for(size_t k=1;k<=NZ;k++)
            {
              zA=zB;
              zB=getZCoordinate(k);
              midPt.push_back(Geometry::Vec3D
                              ((xB+xA)/2.0,(yB+yA)/2.0,(zB+zA)/2.0));
            }
        }
    }
  return midPt;
}
  
double
Mesh3D::getXCoordinate(const size_t I) const
  /*!
    Determine the x coordinate of the boundary
    \param I :: x index
    \return value of coordinate
  */
{
  ELog::RegMethod RegA ("Mesh3D","getXCoordinate");

  if (I > NX)
    throw ColErr::IndexError<size_t>(I,NX,"X-coordinate");

  return getCoordinate(X,XFine,I);
}
  
double
Mesh3D::getYCoordinate(const size_t I) const
  /*!
    Determine the x valud 
    \param a :: y index
    \return value
  */
{
  ELog::RegMethod RegA ("Mesh3D","getYCoordinate");

  if (I > NY)
    throw ColErr::IndexError<size_t>(I,NY,"Y-coordinate");

  return getCoordinate(Y,YFine,I);
}

double
Mesh3D::getZCoordinate(const size_t I) const
  /*!
    Determine the z coordinate
    \param I :: Z index
    \return value
  */
{
  ELog::RegMethod RegA ("Mesh3D","getZCoordinate");

  if (I > NZ)
    throw ColErr::IndexError<size_t>(I,NZ,"Z-coordinate");

  return getCoordinate(Z,ZFine,I);
}

Geometry::Vec3D
Mesh3D::point(const size_t a,const size_t b,const size_t c) const
  /*!
    Determine the 3d Vector corresponding the mesh point (a,b,c)
    \param a :: x index
    \param b :: y index
    \param c :: z index
    \return Vec3D point 
  */
{
  ELog::RegMethod RegA ("Mesh3D","point");

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
Mesh3D::writeWWINP(std::ostream& OX) const
  /*!
    Write out to a mesh to a wwinp file
    Currently ONLY works correctly with a rectangular file
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("Mesh3D","writeWWINP");

  // Write Mesh:
  size_t itemCnt(0);
  StrFunc::writeLine(OX,NX,itemCnt,6);
  StrFunc::writeLine(OX,NY,itemCnt,6);
  StrFunc::writeLine(OX,NZ,itemCnt,6);
  StrFunc::writeLine(OX,Origin,itemCnt,6);

  StrFunc::writeLine(OX,XFine.size(),itemCnt,4);
  StrFunc::writeLine(OX,YFine.size(),itemCnt,4);
  StrFunc::writeLine(OX,ZFine.size(),itemCnt,4);
  // Course bin (3) + Geometry is rectangle flag
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
Mesh3D::write(std::ostream& OX) const
  /*!
    Write out to a mesh
    \param OX :: output stream
  */
{
  ELog::RegMethod RegA("Mesh3D","write");

  std::ostringstream cx;
  cx<<"mesh  geom="<<getType()<<" origin="<<Origin;
  cx<<" ref="<<RefPoint;
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

}   // NAMESPACE Geometry
