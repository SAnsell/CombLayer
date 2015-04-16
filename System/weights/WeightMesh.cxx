/********************************************************************* 
  CombLayer : MNCPX Input builder
 
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
#include "WForm.h"
#include "WeightMesh.h"

namespace WeightSystem
{

WeightMesh::WeightMesh() : 
  WForm(),type(XYZ)			   
  /*!
    Cconstructor [makes XYZ mesh]
  */
{}

WeightMesh::WeightMesh(const WeightMesh& A) : 
  WForm(),type(A.type),RefPoint(A.RefPoint),Origin(A.Origin),
  Axis(A.Axis),Vec(A.Vec),X(A.X),Y(A.Y),Z(A.Z),Mesh(A.Mesh)
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
      WForm::operator=(A);
      type=A.type;
      RefPoint=A.RefPoint;
      Origin=A.Origin;
      Axis=A.Axis;
      Vec=A.Vec;
      X=A.X;
      Y=A.Y;
      Z=A.Z;
      Mesh=A.Mesh;
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
WeightMesh::setCylinder(const Geometry::Vec3D& Base,
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
  ELog::RegMethod RegA("WeightMesh","setCylinder");
  
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

  const double dR=Radius/NR;
  for(size_t i=0;i<=NR;i++)
    X.push_back(dR*i);

  const double dL=((Top-Base).abs())/NZ;
  for(size_t i=0;i<=NZ;i++)
    Y.push_back(dL*i);

  const double dT=1.0/NT;
  for(size_t i=0;i<=NT;i++)
    Z.push_back(dT*i);
  
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
WeightMesh::write(std::ostream& OX) const
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
      for(vc=X.begin();vc!=X.end();vc++)
	cx<<*vc<<" ";
      StrFunc::writeMCNPX(cx.str(),OX);
      cx<<c[i]<<"ints";
      for(size_t index=0;index<Vec.size();index++)
	cx<<" 1";
      StrFunc::writeMCNPX(cx.str(),OX);
    }
  return;
}

}   // NAMESPACE WeightSystem
