/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   attachComp/PositionSupport.cxx
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
#include <deque>
#include <string>
#include <algorithm>
#include <memory>

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
#include "Surface.h"
#include "Rules.h"
#include "HeadRule.h"
#include "Object.h"
#include "surfRegister.h"

#include "LinkUnit.h"
#include "FixedComp.h"
#include "PositionSupport.h"


namespace attachSystem
{

void
applyZAxisRotate(const FixedComp& FC,const double xyAngle,
		 Geometry::Vec3D& V)
 /*!
   Apply a rotation to the vector around the z axis
   \param FC :: Object to get Z axis from 
   \param xyAngle :: Angle in degrees
   \param V :: Vector to rotate
 */
{
  ELog::RegMethod RegA("PositionSupport[F]","applyZAxisRotate");

  if (fabs(xyAngle)>Geometry::zeroTol)
    {
      const Geometry::Quaternion Qxy=
	Geometry::Quaternion::calcQRotDeg(xyAngle,FC.getZ());
      Qxy.rotate(V);
    }
  return;
}



}  // NAMESPACE attachSystem
