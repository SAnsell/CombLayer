/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   geometry/Ellipse.cxx
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
#include "Intersect.h"
#include "Ellipse.h"

namespace Geometry
{

Ellipse::Ellipse() : Intersect(),
  index(0),minorAxis(1,0,0),majorAxis(0,1,0),NormV(0,0,1)
  /*!
    Default Constructor 
  */
{}  

Ellipse::Ellipse(const Geometry::Vec3D& C,
		 const Geometry::Vec3D& mnA,
		 const Geometry::Vec3D& mxA,
		 const Geometry::Vec3D& nV) :
  Intersect(),index(0),Cent(C),minorAxis(mnA),
  majorAxis(mxA),NormV(nV.unit())
  /*!
    Constructor 
    \param C :: Centre
    \param mnA :: minimum Axis
    \param mxA :: maximum Axis
    \param nV :: Normal Axis
  */
{}  

Ellipse::Ellipse(const int I,
		 const Geometry::Vec3D& C,
		 const Geometry::Vec3D& mnA,
		 const Geometry::Vec3D& mxA,
		 const Geometry::Vec3D& nV) :
  Intersect(),index(I),Cent(C),minorAxis(mnA),
  majorAxis(mxA),NormV(nV.unit())
  /*!
    Constructor 
    \param I :: Index value
    \param C :: Centre
    \param mnA :: minimum Axis
    \param mxA :: maximum Axis
    \param nV :: Normal Axis
  */
{}  

Ellipse::Ellipse(const Ellipse& A) :
  Intersect(A),
  index(A.index),Cent(A.Cent),
  minorAxis(A.minorAxis),majorAxis(A.majorAxis),
  NormV(A.NormV)
 /*!
    Copy Constructor 
    \param A :: Ellipse object 
  */
{}

Ellipse&
Ellipse::operator=(const Ellipse& A) 
  /*! 
    Assignment constructor
    \param A :: Ellipse object 
    \return *this
  */
{
  if (this!=&A)
    {
      index=A.index;
      Cent=A.Cent;
      minorAxis=A.minorAxis;
      majorAxis=A.majorAxis;
      NormV=A.NormV;
    }
  return *this;
}


Ellipse&
Ellipse::operator()(const Geometry::Vec3D& C,
		    const Geometry::Vec3D& mnA,
		    const Geometry::Vec3D& mxA,
		    const Geometry::Vec3D& nV) 
  /*!
    Operator() cast
    \param C :: Centre
    \param mnA :: minimum Axis
    \param mxA :: maximum Axis
    \param nV :: Normal Axis
    \return *this
  */
{
  Cent=C;
  minorAxis=mnA;
  majorAxis=mxA;
  NormV=nV.unit();
  return *this;
}

Geometry::Vec3D
Ellipse::ParamPt(const double P) const
  /*!
    Get a parametric point out the system :
    \param P :: Parameter value [0 : 2pi ]
    \return Point 
  */
{
  return Cent+minorAxis*cos(P)+majorAxis*sin(P);
}


void
Ellipse::write(std::ostream& OX) const
  /*!
    Debug write out1
    \param OX :: Output stream
  */
{
  OX<<"Ellipse: ("<<index<<"):"<<Cent<<" : "
    <<minorAxis<<" == "<<majorAxis<<" N="<<NormV;
  return;
}


} // NAMESPACE Geometry
