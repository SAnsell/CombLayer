/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geomInc/geomSupport.h
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
#ifndef Geometry_geomSupport_h
#define Geometry_geomSupport_h

class HeadRule;

namespace Geometry
{

  class Vec3D;
  class Plane;
  
  std::tuple<Geometry::Vec3D,Geometry::Vec3D,Geometry::Vec3D>
    findCornerCircle(const HeadRule&,
		     const Geometry::Plane&,
		     const Geometry::Plane&,
		     const Geometry::Plane&,
		     const double);
  
  
  std::tuple<Geometry::Vec3D,Geometry::Vec3D,Geometry::Vec3D>
    findCornerCircle(const Geometry::Vec3D&,
		     const Geometry::Vec3D&,
		     const Geometry::Vec3D&,
		     const double);
  
  
}     // NAMESPACE Geometry

#endif
