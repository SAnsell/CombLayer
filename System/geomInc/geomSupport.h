/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   geomInc/geomSupport.h
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
#ifndef Geometry_geomSupport_h
#define Geometry_geomSupport_h

namespace Geometry
{

  Geometry::Vec3D
    cornerCircleTouch(const Geometry::Vec3D&,
		      const Geometry::Vec3D&,
		      const Geometry::Vec3D&,
		      const double);
  std::pair<Geometry::Vec3D,Geometry::Vec3D>
    cornerCircle(const Geometry::Vec3D&,
		 const Geometry::Vec3D&,
		 const Geometry::Vec3D&,
		 const double);
  
}     // NAMESPACE Geometry

#endif
