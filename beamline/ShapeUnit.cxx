/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   beamline/ShapeUnit.cxx 
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
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <cmath>
#include <complex>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <algorithm>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Vert2D.h"
#include "Convex2D.h"
#include "surfRegister.h"
#include "Surface.h"
#include "generateSurf.h"
#include "ShapeUnit.h"

namespace beamlineSystem
{

ShapeUnit::ShapeUnit(const int ON,const int LS)  :
  shapeIndex(ON),layerSep(LS)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param ON :: offset number [full objectRegsiter number]
    \param LS :: Layer separation

  */
{}

ShapeUnit::ShapeUnit(const ShapeUnit& A) : 
  shapeIndex(A.shapeIndex),layerSep(A.layerSep),cells(A.cells),
  begPt(A.begPt),endPt(A.endPt)
  /*!
    Copy constructor
    \param A :: ShapeUnit to copy
  */
{}

ShapeUnit&
ShapeUnit::operator=(const ShapeUnit& A)
  /*!
    Assignment operator
    \param A :: ShapeUnit to copy
    \return *this
  */
{
  if (this!=&A)
    {
      cells=A.cells;
      begPt=A.begPt;
      endPt=A.endPt;
    }
  return *this;
}

ShapeUnit::~ShapeUnit() 
  /*!
    Destructor
   */
{}

void
ShapeUnit::addCell(const int C) 
  /*!
    Add a cell to the cell list
    \param C :: Cell number
  */
{
  cells.push_back(C);
  return;
}


void
ShapeUnit::setEndPts(const Geometry::Vec3D& A,
		     const Geometry::Vec3D& B)
  /*!
    Set the end points
    \param A :: First point
    \param B :: Second point
   */
{
  begPt=A;
  endPt=B;
  return;
}

 
}  // NAMESPACE beamlineSystem
