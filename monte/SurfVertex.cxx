/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   monte/SurfVertex.cxx
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
#include <cmath>
#include <complex>
#include <vector>
#include <deque>
#include <map>
#include <string>
#include <sstream>
#include <algorithm>
#include <functional>
#include <boost/regex.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Transform.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Plane.h"
#include "SurfVertex.h"

namespace MonteCarlo
{

std::ostream&
operator<<(std::ostream& OF,const SurfVertex& A) 
  /*!
    Stream operator (uses A.write)
    \param OF :: output stream
    \param A :: SurfVertex to output
    \returns Output stream
  */
{
  A.write(OF);
  return OF;
}

SurfVertex::SurfVertex() :
  Pt()
  /*!
    Default constructor
  */
{}

SurfVertex::SurfVertex(const SurfVertex& A) :
  Surf(A.Surf),Pt(A.Pt)
  /*!
    Copy constructor
    \param A :: Object to copy
  */
{}

SurfVertex&
SurfVertex::operator=(const SurfVertex& A) 
  /*!
    Assignment operator 
    \param A :: SurfVertex to copy
    \returns *this
  */
{
  if (this!=&A)
    {
      Surf=A.Surf;
      Pt=A.Pt;
    }
  return *this;
}

SurfVertex::~SurfVertex() 
  /*!
    Destructor : Removes all the paths
  */
{}


void
SurfVertex::rotate(const Geometry::Matrix<double>& MA) 
  /*!
    Rotates the point by matrix MA
    \param MA :: matrix to use as a rotation
  */
{
  Pt.rotate(MA);
  return;
}

void 
SurfVertex::displace(const Geometry::Vec3D& A)
  /*!
    Apply a displacement Pt 
    \param A :: Displacement to add to SurfVertex
  */ 
{
  Pt+=A;
  return;
}

void 
SurfVertex::mirror(const Geometry::Plane& MP)
  /*!
    Apply a mirror rotation
    \param MP :: Plane to apply
  */ 
{
  MP.mirrorPt(Pt);
  return;
}

void
SurfVertex::addSurface(const Geometry::Surface* A)
  /*!
    Adds a surface either as a copy or as a pointer 
    \param A :: Surface to add to a vertex
  */
{
  Surf.push_back(A);
  return;
}


void
SurfVertex::print(std::ostream& of,const Geometry::Vec3D& Dis,
	      const Geometry::Matrix<double>& MR)  const
  /*!
    Applies a transform to the Point before output
    \param of :: Output Stream to use
    \param MR :: Rotation to be applied
    \param Dis :: Distance to be added 
  */
{
  Geometry::Vec3D X=Pt+Dis;
  X.rotate(MR);
  of<<" -- "<<X;
  return; 
}

void
SurfVertex::write(std::ostream& OX) const
  /*!
    Output function
    \param OX :: output stream
  */
{
  OX<<" -- "<<Pt;
  return;
}

}  // NAMESPACE MonteCarlo
