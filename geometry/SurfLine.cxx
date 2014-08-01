/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   geometry/SurfLine.cxx
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
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <cmath>
#include <complex>
#include <vector>
#include <deque>
#include <map>
#include <string>
#include <algorithm>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "support.h"
#include "mathSupport.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "PolyFunction.h"
#include "PolyVar.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "solveValues.h"
#include "vecOrder.h"
#include "Transform.h"
#include "Surface.h"
#include "Quadratic.h"
#include "surfIndex.h"
#include "Line.h"
#include "LineIntersectVisit.h"


namespace SurInter
{


Geometry::Vec3D
lineSurfPoint(const Geometry::Vec3D& Origin,
	      const Geometry::Vec3D& LAxis,
	      const int SNum,
	      const Geometry::Vec3D& closePt)
  /*!
    Calculate the intersection object between two planes
    \param Origin :: Start of line
    \param LAxis :: Axis of line
    \param SNum :: Surface number
    \param closePt :: Point to determine closest solution
    \return Line/Surf intersection
  */
{
  ELog::RegMethod RegA("SurfLine[F]","lineSurfPoint");

  ModelSupport::surfIndex& SurI=
    ModelSupport::surfIndex::Instance();
  const Geometry::Surface* SPtr=SurI.getSurf(SNum);
  MonteCarlo::LineIntersectVisit LI(Origin,LAxis);
  return LI.getPoint(SPtr,closePt);
}

}  // NAMESPACE SurInter


