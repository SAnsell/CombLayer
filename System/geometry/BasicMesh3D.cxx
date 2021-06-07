/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geometry/BasicMesh3D.cxx
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "support.h"
#include "writeSupport.h"
#include "phitsWriteSupport.h"
#include "Vec3D.h"
#include "BasicMesh3D.h"

namespace Geometry
{

BasicMesh3D::BasicMesh3D() :
  ID(0),writeFlag(0),
  RefPoint(0,0.1,0),Origin(0,0,0),
  OuterPoint(1,1,1),
  xSize(1.0),ySize(1.0),zSize(1.0),
  NX(0),NY(0),NZ(0)
  /*!
    Constructor [makes XYZ mesh]
  */
{}

BasicMesh3D::BasicMesh3D(const size_t I) :
  BasicMesh3D()
  /*!
    Constructor [makes XYZ mesh]
  */
{
  ID=I;
}

BasicMesh3D::BasicMesh3D(const size_t I,
			 const Geometry::Vec3D& APt,
			 const Geometry::Vec3D& BPt, 
			 const std::array<size_t,3>& Nxyz) :
  ID(I),
  RefPoint((APt+BPt)/2.0)
  /*!
    Constructor based on simple grid
    \param I :: ID number
    \param APt :: Low Point
    \param BPt :: High Point
    \param Nxyz :: Number of bins in each orthogonal direction.
   */
{
  setMesh(APt,BPt,Nxyz);
}
  
BasicMesh3D::BasicMesh3D(const BasicMesh3D& A) : 
  ID(A.ID),writeFlag(A.writeFlag),
  RefPoint(A.RefPoint),Origin(A.Origin),
  OuterPoint(A.OuterPoint),
  xSize(A.xSize),ySize(A.ySize),zSize(A.zSize),
  NX(A.NX),NY(A.NY),NZ(A.NZ)
  /*!
    Copy constructor
    \param A :: BasicMesh3D to copy
  */
{}

BasicMesh3D&
BasicMesh3D::operator=(const BasicMesh3D& A)
  /*!
    Assignment operator
    \param A :: BasicMesh3D to copy
    \return *this
  */
{
  if (this!=&A)
    {
      ID=A.ID;
      writeFlag=A.writeFlag;
      RefPoint=A.RefPoint;
      Origin=A.Origin;
      OuterPoint=A.OuterPoint;
      xSize=A.xSize;
      ySize=A.ySize;
      zSize=A.zSize;
      NX=A.NX;
      NY=A.NY;
      NZ=A.NZ;
    }
  return *this;
}

double
BasicMesh3D::getXCoordinate(const size_t index) const
  /*!
    Get a bin center point
    \param index :: Index to use 
    \return Centre point
   */
{
  if (index>=NX)
    throw ColErr::IndexError<size_t>(index,NX,"BasicMesh3D:getXCoord");
  const double xStep=xSize/static_cast<double>(NX);
  return Origin.X()+xStep*(static_cast<double>(index)+0.5);
}

double
BasicMesh3D::getYCoordinate(const size_t index) const
  /*!
    Get a bin center point
    \param index :: Index to use 
    \return Centre point
   */
{
  if (index>=NY)
    throw ColErr::IndexError<size_t>(index,NY,"BasicMesh3D:getYCoord");
  const double yStep=ySize/static_cast<double>(NY);
  return Origin.Y()+yStep*(static_cast<double>(index)+0.5);
}
  
double
BasicMesh3D::getZCoordinate(const size_t index) const
  /*!
    Get a bin center point
    \param index :: Index to use 
    \return Centre point
   */
{
  if (index>=NZ)
    throw ColErr::IndexError<size_t>(index,NZ,"BasicMesh3D:getZCoord");
  const double zStep=zSize/static_cast<double>(NZ);
  return Origin.Z()+zStep*(static_cast<double>(index)+0.5);
}

void 
BasicMesh3D::setMesh(const Geometry::Vec3D& LowPt,
		     const Geometry::Vec3D& HighPt,
		     const std::array<size_t,3>& NPts)
/*!
  Ugly way to set the mesh from the WWG constructor
  \param LowPt : Corner point [assuming in local XYZ space]
  \param HighPt : Corner point [assuming in local XYZ space]
  \param NPts : Number of bins
*/
{
  NX=NPts[0];
  NY=NPts[1];
  NZ=NPts[2];

  Origin=LowPt;
  OuterPoint=HighPt;
  const Geometry::Vec3D D=HighPt-LowPt;
  xSize=D.X();
  ySize=D.Y();
  zSize=D.Z();
  
  return;
}

void
BasicMesh3D::setRefPt(const Geometry::Vec3D& Pt)
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

std::vector<Geometry::Vec3D>
BasicMesh3D::midPoints() const
  /*!
    Calculate a linearized mid point list
    Retruns (NX-1*NY-1*NZ-1) points indexing
    on X [highest] Y[mid] Z [lowest] index
    \return vector of points XYZ order
   */
{
  ELog::RegMethod RegA("BasicMesh3D","midPoints");

  std::vector<Geometry::Vec3D> midPt;
  if (NX*NY*NZ==0) return midPt;           // origin on failure

  const double xStep=xSize/static_cast<double>(NX);
  const double yStep=ySize/static_cast<double>(NY);
  const double zStep=zSize/static_cast<double>(NZ);

  // half a step PAST corner
  const Geometry::Vec3D cornerPt=Origin-
    Geometry::Vec3D(xStep/2.0,yStep/2.0,zStep/2.0);

  Geometry::Vec3D CPt(cornerPt);
	
  for(size_t i=1;i<=NX;i++)
    for(size_t j=1;j<=NY;j++)
      {
	CPt=cornerPt+Geometry::Vec3D(xStep*static_cast<double>(i),
				     yStep*static_cast<double>(j),0.0);
	for(size_t k=1;k<=NZ;k++)
	  {
	    CPt+=Geometry::Vec3D(0,0,zStep);
	    midPt.push_back(CPt);
	  }
      }

  return midPt;
}

Geometry::Vec3D
BasicMesh3D::point(const long int a,
		   const long int b,
		   const long int c) const
  /*!
    Determine the 3d Vector corresponding the mesh point (a,b,c)
    \param a :: x index
    \param b :: y index
    \param c :: z index
    \return Vec3D point 
  */
{
  return point(static_cast<size_t>(a),
	       static_cast<size_t>(b),
	       static_cast<size_t>(c));
}

Geometry::Vec3D
BasicMesh3D::point(const size_t a,
		   const size_t b,
		   const size_t c) const
  /*!
    Determine the 3d Vector corresponding the mesh point (a,b,c)
    \param a :: x index
    \param b :: y index
    \param c :: z index
    \return Vec3D point 
  */
{
  ELog::RegMethod RegA ("BasicMesh3D","point");

  // note this always throws if NX/NY/NZ == 0
  if (a >= NX)
    throw ColErr::IndexError<size_t>(a,NX,"X-coordinate");
  if (b >= NY)
    throw ColErr::IndexError<size_t>(b,NY,"Y-coordinate");
  if (c >= NZ)
    throw ColErr::IndexError<size_t>(c,NZ,"Z-coordinate");
  
  const double xStep=xSize/static_cast<double>(NX);
  const double yStep=ySize/static_cast<double>(NY);
  const double zStep=zSize/static_cast<double>(NZ);
  
  // half a step PAST corner
  const Geometry::Vec3D cornerPt=Origin-
    Geometry::Vec3D(xStep/2.0,yStep/2.0,zStep/2.0);

  return cornerPt+
    Geometry::Vec3D(xStep*static_cast<double>(a),
		    yStep*static_cast<double>(b),
		    zStep*static_cast<double>(c));  
}
  
void
BasicMesh3D::writeWWINP(std::ostream& OX) const
  /*!
    Write out to a mesh to a wwinp file
    Currently ONLY works correctly with a rectangular file
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("BasicMesh3D","writeWWINP");

  // Write Mesh:
  size_t itemCnt(0);

  // half a step PAST corner
  // note that wwinp needs to be orthogonal to xyz

  StrFunc::writeLine(OX,NX,itemCnt,6);  // full count of bins
  StrFunc::writeLine(OX,NY,itemCnt,6);
  StrFunc::writeLine(OX,NZ,itemCnt,6);
  StrFunc::writeLine(OX,Origin,itemCnt,6);  

  // course bins == fine bins 
  StrFunc::writeLine(OX,NX,itemCnt,4);  // full count of bins
  StrFunc::writeLine(OX,NY,itemCnt,4);
  StrFunc::writeLine(OX,NZ,itemCnt,4);
  StrFunc::writeLine(OX,1.0,itemCnt,4);   // 1.0 ==> xyz

  // calc absolute bounaries
  double XP=Origin.X();
  const double xStep=xSize/static_cast<double>(NX);
  StrFunc::writeLine(OX,XP,itemCnt,6);
  for(size_t i=0;i<NX;i++)
    {
      XP+=xStep;
      StrFunc::writeLine(OX,1.0,itemCnt,6);
      StrFunc::writeLine(OX,XP,itemCnt,6);
      StrFunc::writeLine(OX,1.0,itemCnt,6);
    }
  if (itemCnt) OX<<std::endl;

  double YP=Origin.Y();
  const double yStep=ySize/static_cast<double>(NX);
  StrFunc::writeLine(OX,XP,itemCnt,6);
  for(size_t i=0;i<NY;i++)
    {
      YP+=yStep;
      StrFunc::writeLine(OX,1.0,itemCnt,6);
      StrFunc::writeLine(OX,YP,itemCnt,6);
      StrFunc::writeLine(OX,1.0,itemCnt,6);
    }
  if (itemCnt) OX<<std::endl;

  double ZP=Origin.Z();
  const double zStep=ySize/static_cast<double>(NZ);
  StrFunc::writeLine(OX,XP,itemCnt,6);
  for(size_t i=0;i<NZ;i++)
    {
      ZP+=zStep;
      StrFunc::writeLine(OX,1.0,itemCnt,6);
      StrFunc::writeLine(OX,ZP,itemCnt,6);
      StrFunc::writeLine(OX,1.0,itemCnt,6);
    }
  if (itemCnt) OX<<std::endl;

  
  return;
}


  
void
BasicMesh3D::write(std::ostream& OX) const
  /*!
    Write out to a mesh
    \param OX :: output stream
  */
{
  ELog::RegMethod RegA("BasicMesh3D","write");

  std::ostringstream cx;
  cx<<"mesh  geom=xyz ref="<<RefPoint;
  StrFunc::writeMCNPX(cx.str(),OX);
  cx.str("");

  cx<<"     origin= "<<Origin;
  StrFunc::writeMCNPX(cx.str(),OX);
  cx.str("");

  // imesh :
  double XP=Origin.Y();
  const double xStep=xSize/static_cast<double>(NX);
  cx.str("");
  cx<<"imesh ";
  for(size_t i=0;i<NX;i++)
    {
      XP+=xStep;
      cx<<XP<<" ";
    }
  StrFunc::writeMCNPXcont(cx.str(),OX);

  // imesh :
  double YP=Origin.Y();
  const double yStep=ySize/static_cast<double>(NY);
  cx.str("");
  cx<<"jmesh ";
  for(size_t i=0;i<NY;i++)
    {
      YP+=yStep;
      cx<<YP<<" ";
    }
  StrFunc::writeMCNPXcont(cx.str(),OX);

  // kmesh :
  double ZP=Origin.Z();
  const double zStep=zSize/static_cast<double>(NZ);
  cx.str("");
  cx<<"kmesh ";
  for(size_t i=0;i<NZ;i++)
    {
      ZP+=zStep;
      cx<<ZP<<" ";
    }
  StrFunc::writeMCNPXcont(cx.str(),OX);

  return;
}


void
BasicMesh3D::writePHITS(std::ostream& OX) const
  /*!
    Write out to a mesh for phits 
    \param OX :: output stream
  */
{
  ELog::RegMethod RegA("BasicMesh3D","writePHITS");

  std::ostringstream cx;
  
  StrFunc::writePHITS(OX,1,"mesh","xyz","Mesh type is xyz");
  StrFunc::writePHITS(OX,1,"x-type",2,"min/max nx");
  StrFunc::writePHITS(OX,1,"xmin",Origin.X());
  StrFunc::writePHITS(OX,1,"xmax",Origin.X()+xSize);
  StrFunc::writePHITS(OX,1,"nx",NX);

  StrFunc::writePHITS(OX,1,"y-type",2,"min/max ny");
  StrFunc::writePHITS(OX,1,"ymin",Origin.Y());
  StrFunc::writePHITS(OX,1,"ymax",Origin.Y()+ySize);
  StrFunc::writePHITS(OX,1,"ny",NY);

  StrFunc::writePHITS(OX,1,"z-type",2,"min/max nz");
  StrFunc::writePHITS(OX,1,"zmin",Origin.Z());
  StrFunc::writePHITS(OX,1,"zmax",Origin.Z()+zSize);
  StrFunc::writePHITS(OX,1,"nz",NZ);
        
  return;
}

void
BasicMesh3D::writeVTK(std::ostream& OX) const
  /*!
    Write out to a mesh for phits 
    \param OX :: output stream
  */
{
  ELog::RegMethod RegA("BasicMesh3D","writePHITS");

  
  boost::format fFMT("%1$11.6g%|14t|");  
  OX<<"DATASET RECTILINEAR_GRID"<<std::endl;

  OX<<"DIMENSIONS "<<NX<<" "<<NY<<" "<<NZ<<std::endl;
  OX<<"X_COORDINATES "<<NX<<" float"<<std::endl;
  for(size_t i=0;i<static_cast<size_t>(NX);i++)
    OX<<(fFMT % getXCoordinate(i));
  OX<<std::endl;
  
  OX<<"Y_COORDINATES "<<NY<<" float"<<std::endl;
  for(size_t i=0;i<static_cast<size_t>(NY);i++)
    OX<<(fFMT % getYCoordinate(i));
  OX<<std::endl;

  OX<<"Z_COORDINATES "<<NZ<<" float"<<std::endl;
  for(size_t i=0;i<static_cast<size_t>(NZ);i++)
    OX<<(fFMT % getZCoordinate(i));
  OX<<std::endl;
  

  
  return;
}

}   // NAMESPACE Geometry
