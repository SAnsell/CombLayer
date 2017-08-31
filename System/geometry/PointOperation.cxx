/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geometry/PointOperation.cxx
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
#include <list>
#include <vector>
#include <map>
#include <string>
#include <algorithm>

#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "surfProg.h"

namespace Geometry
{

void
expandCluster(const double D,std::vector<Vec3D>& planePts)
  /*!
    Expand / contract a group of points around the centre
    point. Determined from entry points as average mid point.
    \param D :: Distance to move point by +ve is outwards)
    \param planePts :: Point that need to be moved
  */
{
  if (planePts.empty())
    return;

  Geometry::Vec3D centPoint;
  for(const Geometry::Vec3D& Pt : planePts)
    centPoint+= Pt;
  
  centPoint/=static_cast<double>(planePts.size());

  for(Geometry::Vec3D& Pt : planePts)
    {
      const Geometry::Vec3D unitVec=(Pt-centPoint).unit();
      Pt += unitVec*D;
    }
  return;
}

}  // NAMESPACE Geometry
