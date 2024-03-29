/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geometry/Pnt.cxx
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
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "FileReport.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "Intersect.h"
#include "Pnt.h"

namespace Geometry
{

Pnt::Pnt() : Intersect(),
  index(0)
  /*!
    Default Constructor 
  */
{}  

Pnt::Pnt(Geometry::Vec3D  C) :
  Intersect(),index(0),Cent(std::move(C))
  /*!
    Constructor 
    \param C :: Centre
  */
{}  

Pnt::Pnt(const int I,Geometry::Vec3D  C) :
  Intersect(),index(I),Cent(std::move(C))
  /*!
    Constructor 
    \param I :: Index value
    \param C :: Centre
  */
{}  

Pnt::Pnt(const Pnt& A) :
  Intersect(A),
  index(A.index),Cent(A.Cent)
  /*!
    Copy Constructor 
    \param A :: Pnt object 
  */
{}

Pnt&
Pnt::operator=(const Pnt& A) 
  /*! 
    Assignment constructor
    \param A :: Pnt object 
    \return *this
  */
{
  if (this!=&A)
    {
      index=A.index;
      Cent=A.Cent;
    }
  return *this;
}


Pnt&
Pnt::operator()(const Geometry::Vec3D& C)
  /*!
    Operator() cast
    \param C :: Centre
    \return *this
  */
{
  Cent=C;
  return *this;
}  

Geometry::Vec3D
Pnt::ParamPt(const double) const
  /*!
    Get a parametric point out the system :
    \param :: Parameter value [Not used]
    \return Point 
  */
{
  return Cent;
}

void
Pnt::write(std::ostream& OX) const
  /*!
    Debug write out
    \param OX :: Output stream
  */
{
  OX<<"Pnt: ("<<index<<"):"<<Cent;
  return;
}

} // NAMESPACE Geometry
