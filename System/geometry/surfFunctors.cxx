/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geometry/surfFunctors.cxx
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
#include <complex>
#include <cmath>
#include <vector>
#include <map>
#include <string>
#include <algorithm>

#include "Exception.h"
#include "GTKreport.h"
#include "FileReport.h"
#include "OutputLog.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Transform.h"
#include "Line.h"
#include "Surface.h"
#include "surfFunctors.h"

namespace Geometry
{

int 
surfaceCheck(const int sign,const Surface* Sptr,const Vec3D& Pt)
  /*!
    Test a point realative to the surface
    \param sign :: Sign to apply to surface 
    \param Sptr :: Surface Ptr
    \param Pt :: Point to test
    \retval 0 :: surface valid
    \retval 1 : Surface not valid  
  */
{
  if (!Sptr) return 1;
  return ((Sptr->side(Pt) * sign)>0) ? 0 : 1;
}

}  // NAMESPACE Geometry
