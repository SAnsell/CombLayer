/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/pinInfo
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
#include "pinInfo.h"

namespace essSystem
{

pinInfo::pinInfo(const std::string& S,
		 const size_t RNum,
		 const size_t SNum,
		 const size_t OIndex,
		 const double L,const double BR,
		 const double CR,
		 const Geometry::Vec3D& CP) :
  Name(S),radN(RNum),sectN(SNum),index(OIndex),
  length(L),boltRadius(BR),clearRadius(CR),
  basePoint(CP)
  /*!
    Constructor
    \param S :: Name
    \param RNum :: Radial number
    \param SNum :: Sector number
    \param OIndex :: Offset number
    \param L :: Bolt length
    \param BR :: Bolt radius
    \param cR :: Clearance radius
    \param CP :: Base point
  */
{}

 
}  // NAMESPACE essSystem

