/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   geometry/DblLine.cxx
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
#include "DblLine.h"

namespace Geometry
{

DblLine::DblLine(const Geometry::Vec3D& APoint,
		 const Geometry::Vec3D& BPoint,
		 const Geometry::Vec3D& NV) :
  Intersect(),index(0),
  APt(APoint),BPt(BPoint),NormV(NV.unit())
  /*!
    Constructor 
    \param APoint :: Line start (1st)
    \param BPoint :: Line start (2nd)
    \param NV :: Normal
  */
{}  

DblLine::DblLine(const int I,
		 const Geometry::Vec3D& APoint,
		 const Geometry::Vec3D& BPoint,
		 const Geometry::Vec3D& NV) :
  Intersect(),index(I),
  APt(APoint),BPt(BPoint),NormV(NV.unit())
  /*!
    Constructor 
    \param I :: Index value
    \param APoint :: Line start (1st)
    \param BPoint :: Line start (2nd)
    \param NV :: Normal
  */
{}  

DblLine::DblLine(const DblLine& A) :
  Intersect(A),
  index(A.index),APt(A.APt),
  BPt(A.BPt),NormV(A.NormV)
 /*!
    Copy Constructor 
    \param A :: DblLine object 
  */
{}

DblLine&
DblLine::operator=(const DblLine& A) 
  /*! 
    Assignment constructor
    \param A :: DblLine object 
    \return *this
  */
{
  if (this!=&A)
    {
      index=A.index;
      APt=A.APt;
      BPt=A.BPt;
      NormV=A.NormV;
    }
  return *this;
}


DblLine&
DblLine::operator()(const Geometry::Vec3D& APoint,
		    const Geometry::Vec3D& BPoint,
		    const Geometry::Vec3D& NV)
  /*!
    Operator() cast
    \param APoint :: First point
    \param BPoint :: Second point
    \param NV :: Direction
    \return *this
  */
{
  APt=APoint;
  BPt=BPoint;
  NormV=NV.unit();
  return *this;
}  

Geometry::Vec3D
DblLine::ParamPt(const double P) const
  /*!
    Get a parametric point out the system :
    Note that only positive side of the lines
    \param P :: Parameter value [-inf : inf ]
    \return Point [
  */
{
  if (P>0)
    return APt+NormV*P;
  else
    return BPt+NormV*P;
}

void
DblLine::write(std::ostream& OX) const
  /*!
    Debug write out
    \param OX :: Output stream
  */
{
  OX<<"DblLine: ("<<index<<"):"<<APt<<" : "<<BPt<<" Dir="<<NormV;
  return;
}


} // NAMESPACE Geometry
