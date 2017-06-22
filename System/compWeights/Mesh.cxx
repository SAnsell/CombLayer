/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   weights/Mesh.cxx
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
#include <memory>
#include <boost/multi_array.hpp>

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
#include "Mesh.h"

namespace compSystem
{

Mesh::Mesh() : 
  type(XYZ)			   
  /*!
    Cconstructor [makes XYZ mesh]
  */
{}

Mesh::Mesh(const Mesh& A) : 
  type(A.type),RefPoint(A.RefPoint),Origin(A.Origin),
  Axis(A.Axis),Vec(A.Vec),X(A.X),Y(A.Y),Z(A.Z),MData(A.MData)
  /*!
    Copy constructor
    \param A :: Mesh to copy
  */
{}

Mesh&
Mesh::operator=(const Mesh& A)
  /*!
    Assignment operator
    \param A :: Mesh to copy
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
      MData=A.MData;
    }
  return *this;
}

void
Mesh::setMeshType(const GeomENUM& A)
  /*!
    Set the type
    \param A :: Type
   */
{
  type=A;
  return;
}

void 
Mesh::setCylinder(const Geometry::Vec3D& Base,
		  const Geometry::Vec3D& Top,
		  const Geometry::Vec3D& uRVec,
		  const double Radius,const size_t NR,
		  const size_t NZ,const size_t NT)
  /*!
    Sets up a cyclindrical mesh
    \param Base :: Base point in Cylinder
    \param Top :: Top Point in Cylinder
    \param uRVec :: Vector along radius to call 0 angle 
    \param Radius :: Radius of cylinder
    \param NR :: number of radii components
    \param NZ :: number of radii components
    \param NT :: number of radii components
   */
{
  ELog::RegMethod RegA("Mesh","setCylinder");
  
  type=Cyl;
  Origin=Base;
  Axis=(Top-Base).unit();
  Vec=uRVec.unit();

  if (!(NR*NZ*NT))
    {
      ELog::EM<<"Incorrect values in Input ranges : "
	      <<NR<<" "<<NZ<<" "<<NT<<ELog::endErr;
      throw ColErr::ExitAbort("NR/NZ/NT failure");
    }

  const double dR=Radius/static_cast<double>(NR);
  for(size_t i=0;i<=NR;i++)
    X.push_back(static_cast<double>(i)*dR);

  const double dL=((Top-Base).abs())/static_cast<double>(NZ);
  for(size_t i=0;i<=NZ;i++)
    Y.push_back(dL*static_cast<double>(i));

  const double dT=1.0/static_cast<double>(NT);
  for(size_t i=0;i<=NT;i++)
    Z.push_back(dT*static_cast<double>(i));
  
  return;
}

std::string
Mesh::getType() const
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

Geometry::Vec3D
Mesh::point(const size_t a,const size_t b,const size_t c) const
  /*!
    Determine the 3d Vector corresponding the mesh point (a,b,c)
    \param a :: x index
    \param b :: y index
    \param c :: z index
    \return Vec3D point 
  */
{
  ELog::RegMethod RegA ("Mesh","point");

  if (a >= X.size())
    throw ColErr::IndexError<size_t>(a,X.size(),"X-coordinate");
  if (b >= Y.size())
    throw ColErr::IndexError<size_t>(b,Y.size(),"Y-coordinate");
  if (c >= Z.size())
    throw ColErr::IndexError<size_t>(c,Z.size(),"Z-coordinate");

  const double xc=X[a];
  const double yc=Y[b];
  const double zc=Z[c];
  switch (type)
    {
    case XYZ:
      return Geometry::Vec3D(xc,yc,zc);
    case Cyl:
      // Stuff with axis... etc...
      return Geometry::Vec3D(xc,yc,zc);
    case Sph:
      // Stuff with axis... etc...
      return Geometry::Vec3D(xc,yc,zc);
    }
  return Geometry::Vec3D(xc,yc,zc);
}

void
Mesh::write(std::ostream& OX) const
  /*!
    Write out to a mesh
    \param OX :: output stream
  */
{
  std::ostringstream cx;
  cx<<"mesh "<<getType()<<" origin "<<Origin
    <<" axs "<<Axis<<" vec "<<Vec<<" ref "<<RefPoint;
  StrFunc::writeMCNPX(cx.str(),OX);

  // imesh :
  char c[3]={'i','j','k'};
  std::vector<double>::const_iterator vc;
  for(int i=0;i<3;i++)
    {
      const std::vector<double>& Vec=
	((!i) ? X : (i==1) ? Y : Z);
      
      cx.str("");
      cx<<c[i]<<"mesh ";
      for(const double val : X)
	cx<<val<<" ";
      StrFunc::writeMCNPX(cx.str(),OX);
      cx<<c[i]<<"ints";
      for(size_t index=0;index<Vec.size();index++)
	cx<<" 1";
      StrFunc::writeMCNPX(cx.str(),OX);
    }
  return;
}

}   // NAMESPACE compSystem
