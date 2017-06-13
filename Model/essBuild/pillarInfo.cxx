/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/pillarInfo
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

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "pillarInfo.h"

namespace essSystem
{

pillarInfo::pillarInfo(const std::string& S,
		       const int RIndex,
		       const Geometry::Vec3D& CP,
		       const Geometry::Vec3D& YA) :
  Name(S),RI(RIndex),centPoint(CP),YAxis(YA.unit())
  /*!
    Constructor
    \param S :: Name
    \param RIndex :: Offset number
    \param CP :: Central point
    \param YA :: YAxis direction
  */
{}

pillarInfo::pillarInfo(const pillarInfo& A) :
  Name(A.Name),RI(A.RI),
  centPoint(A.centPoint),YAxis(A.YAxis)
  /*!
    Copy constructor
    \param A :: pillarInfo object to construct
  */
{}

pillarInfo&
pillarInfo::operator=(const pillarInfo& A)
  /*!
    Assignment operator
    \param A :: pillarInfo object to construct
    \return *this
  */
{
  if (this!=&A)
    {
      centPoint=A.centPoint;
      YAxis=A.YAxis;
    }
  return *this;
}
  
  
}  // NAMESPACE essSystem
