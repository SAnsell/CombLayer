/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geomInc/SurInter.h
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
#ifndef SurInter_h
#define SurInter_h

class HeadRule;

namespace Geometry
{
  class Intersect;
  
  class Surface;
  class Quadratic;
  class Cone;
  class Plane;
  class Cylinder;
  class General;
  class Sphere;
}

/*!
  \namespace SurInter
  \brief Holds intersection process between Surfaces
  \version 1.0
  \date August 2005
  \author S. Ansell

*/
namespace SurInter
{

Geometry::Vec3D
getLinePoint(const Geometry::Vec3D&,const Geometry::Vec3D&,
	     const Geometry::Surface*,const Geometry::Vec3D&);

Geometry::Vec3D
getLinePoint(const Geometry::Vec3D&,const Geometry::Vec3D&,
	     const Geometry::Plane*);

Geometry::Vec3D
getLinePoint(const Geometry::Vec3D&,const Geometry::Vec3D&,
	     const HeadRule&,const HeadRule&);
	     
  
Geometry::Vec3D
nearPoint(const std::vector<Geometry::Vec3D>&,
	  const Geometry::Vec3D&);

size_t
closestPt(const std::vector<Geometry::Vec3D>&,
	  const Geometry::Vec3D&);

std::pair<Geometry::Vec3D,int>
interceptRuleConst(const HeadRule&,const Geometry::Vec3D&,
	      const Geometry::Vec3D&);

std::pair<Geometry::Vec3D,int>
interceptRule(HeadRule&,const Geometry::Vec3D&,
	      const Geometry::Vec3D&);

// Generic point intersection
std::vector<Geometry::Vec3D> 
processPoint(const Geometry::Surface*,const Geometry::Surface*,
	     const Geometry::Surface*);

Geometry::Vec3D
getPoint(const Geometry::Surface*,const Geometry::Surface*,
	 const Geometry::Surface*);

Geometry::Vec3D
getPoint(const Geometry::Surface*,const Geometry::Surface*,
	 const Geometry::Surface*,const Geometry::Vec3D&);
 
// Point Stuff::
std::vector<Geometry::Vec3D> 
makePoint(const Geometry::Plane*,const Geometry::Plane*,
	  const Geometry::Plane*);

std::vector<Geometry::Vec3D> 
makePoint(const Geometry::Plane*,const Geometry::Plane*,
	  const Geometry::Cylinder*);

std::vector<Geometry::Vec3D>
makePoint(const Geometry::Quadratic*,const Geometry::Quadratic*,
	  const Geometry::Quadratic*);

int 
getMidPoint(const Geometry::Surface*,const Geometry::Surface*, 
	    const Geometry::Surface*,Geometry::Vec3D&);

Geometry::Vec3D
calcCentroid(const Geometry::Surface*,const Geometry::Surface*,
	     const Geometry::Surface*,const Geometry::Surface*,
	     const Geometry::Surface*,const Geometry::Surface*);

 
/// Intersections:
template<typename ATYPE,typename BTYPE>
Geometry::Intersect*
calcIntersect(const ATYPE&,const BTYPE&);


}

#endif 
