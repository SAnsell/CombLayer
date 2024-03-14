/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   geometry/interPoint.cxx
 *
 * Copyright (c) 2004-2024 by Stuart Ansell
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

#include "FileReport.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "interPoint.h"

namespace Geometry
{
void
sortVector(std::vector<interPoint>& IPts)
  /*!
    Sort a standard vector (convinience funtion)
    \parma IPts :: Sort of vector
   */
{
    std::sort(IPts.begin(),IPts.end(),
	    [](const Geometry::interPoint& A,
	       const Geometry::interPoint& B)
	    {
	      return (A.D > B.D);
	    });
    return;
}

std::ostream&
operator<<(std::ostream& OX,const interPoint& A)
  /*!
    Standard output stream
    \param OX :: Output stream
    \param A :: interPoint to write
    \return Stream State
   */
{
  OX<<A.Pt<<" : "<<A.D<<" SN["<<A.outFlag<<"]="<<A.SNum;
  return OX;
}


} // NAMESPACE Geometry
