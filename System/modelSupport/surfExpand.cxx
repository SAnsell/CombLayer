/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   process/surfExpand.cxx
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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
#include <complex>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <stack>
#include <string>
#include <algorithm>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "Surface.h"
#include "surfIndex.h"
#include "Quadratic.h"
#include "Cylinder.h"
#include "Plane.h"

typedef ModelSupport::surfIndex::STYPE SMAP;

namespace ModelSupport
{

template<typename T>
T* createExpand(const Geometry::Surface*,const double);


template<>
Geometry::Plane*
createExpand<Geometry::Plane>(const Geometry::Surface* SPtr,
			      const double Dist)
  /*!						
    This creates a surface which is shifted from the current
    point in by the appropiate a mount. Planes move -ve normal on
    +ve Dist. 
    \param SPtr :: Point to the plane
    \param Dist :: Distance to shift by
    \return Point to new surface 
  */
{
  ELog::RegMethod RegA("surfExpand","createExpand<Plane>");

  const Geometry::Plane* PPtr=
    dynamic_cast<const Geometry::Plane*>(SPtr);
  if (!PPtr)
    throw ColErr::CastError<Geometry::Surface>(SPtr,"No Plane Conversion");
  
  Geometry::Plane* OutPtr=PPtr->clone();
  OutPtr->setDistance(OutPtr->getDistance()+Dist);
  return OutPtr;
}

template<>
Geometry::Cylinder*
createExpand<Geometry::Cylinder>(const Geometry::Surface* SPtr,
				 const double Dist)
  /*!						
    This creates a surface which is shifted from the current
    point in by the appropiate a mount. Cylinders expand on +ve Dist
    \param SPtr :: Point to the cylinder
    \param Dist :: Distance to shift by
    \return Point to new surface 
  */
{				
  ELog::RegMethod RegA("surfExpand","createExpand<Cylinder>");
  ELog::EM<<"HERE"<<ELog::endDiag;
  const Geometry::Cylinder* PPtr=
    dynamic_cast<const Geometry::Cylinder*>(SPtr);
  if (!PPtr)
    throw ColErr::CastError<Geometry::Surface>(SPtr,"No Cyl Conv");

  Geometry::Cylinder* OutPtr=PPtr->clone();
  OutPtr->setRadius(OutPtr->getRadius()+Dist);
  return OutPtr;
}

// ------------------------------------------
//               OUTSIDE
// ------------------------------------------

  
Geometry::Surface*
surfaceCreateExpand(const Geometry::Surface* SPtr,const double Distance)
  /*!
    Expand/collapse a surface.  Contained surfaces contract on 
    negative distance. Planes are scaled by their sign so +ve distance 
    is -ve to the normal.
    \param SPtr :: surface pointer
    \param Distance :: Distance to expand surface by [-ve == inward]
    \return Surface pointer 
   */
{
  ELog::RegMethod RegA("surfExpand","surfaceCreateExpand");
  
  if (!SPtr)
    throw ColErr::EmptyValue<Geometry::Surface*>("SPtr");

  const Geometry::SurfKey sIndex=SPtr->classIndex();
  
  if (sIndex == Geometry::SurfKey::Cylinder)
    return createExpand<Geometry::Cylinder>(SPtr,Distance);
  if (sIndex == Geometry::SurfKey::Plane)
    return createExpand<Geometry::Plane>(SPtr,Distance);

  throw ColErr::AbsObjMethod("createExpand<Surface>");  
}

} // NAMESPACE ModelSupport
