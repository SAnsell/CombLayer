/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geometry/Vert2D.cxx
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
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <vector>
#include <map>
#include <string>
#include <algorithm>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Vert2D.h"

namespace Geometry
{

std::ostream&
operator<<(std::ostream& OX,const Vert2D& A)
  /*!
    Standard output stream
    \param OX :: Output stream
    \param A :: Vert2D to write
    \return Stream State
   */
{
  A.write(OX);
  return OX;
}

Vert2D::Vert2D() :
  done(0),onHull(1),vIndex(0),cAngle(0.0)
  ///< Constructor
{
  ELog::RegMethod RegA("Vert2D","Constructor()");
}

Vert2D::Vert2D(const size_t N,const Geometry::Vec3D& PVec) :
  done(0),onHull(1),vIndex(N),cAngle(0),V(PVec)
  /*!
    Constructor 
    \param N :: Index number
    \param PVec :: Point for hull
  */
{
  ELog::RegMethod RegA("Vert2D","Constructor(N,PVec)");
}

Vert2D::Vert2D(const Vert2D& A) :
  done(A.done),onHull(A.onHull),
  vIndex(A.vIndex),cAngle(A.cAngle),V(A.V)
  /*!
    Copy Constructor 
    \param A :: Vert2D object 
  */
{
  ELog::RegMethod RegA("Vert2D","Copy Constructor");
}

Vert2D&
Vert2D::operator=(const Vert2D& A) 
  /*! 
    Assignment constructor
    \param A :: Vert2D object 
    \return *this
  */
{
  ELog::RegMethod RegA("Vert2D","operator=");
  if (this!=&A)
    {
      done=A.done;
      onHull=A.onHull;
      vIndex=A.vIndex;
      cAngle=A.cAngle;
      V=A.V;
    }
  return *this;
}

double
Vert2D::calcAngle(const Geometry::Vec3D& ZeroPt,const Geometry::Vec3D& Norm)
  /*!
    Calculates the angle relative to the zeroPt
    \param ZeroPt :: Origin
    \param Norm :: Plane normal
    \return cos(angle)
  */
{
  ELog::RegMethod RegA("Vert2D","calcAngle");
  Geometry::Vec3D Pt(V-ZeroPt);
  if (Pt.abs()>1e-5)
    {
      Pt.makeUnit();
      cAngle=Pt.dotProd(Norm);
    }
  else
    cAngle=0.0;
  return cAngle;
}

void
Vert2D::write(std::ostream& OX) const
  /*!
    Debug write out1
    \param OX :: Output stream
   */
{
  OX<<"(IHDc) "<<vIndex<<" "<<onHull<<" "<<done<<" "<<cAngle<<" : "<<V;
  return;
}


} // NAMESPACE Geometry
