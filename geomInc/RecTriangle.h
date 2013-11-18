/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   geomInc/RecTriangle.h
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
#ifndef Geometry_RecTriangle_h
#define Geometry_RecTriangle_h

namespace Geometry
{
/*!
  \class RecTriangle
  \brief RecTriangle mapping of Trianglation
  \version 1.0
  \date November 2010
  \author S. Ansell
 */

template<typename Shape>
class RecTriangle
{
 private:

  int level;           ///< depth level

  /// Active stack list of triangles undivided:
  std::queue<Shape> activeList;

 public:
  
  /// Generic constructor
  RecTriangle(const Geometry::Vec3D&,
	      const Geometry::Vec3D&,
	      const Geometry::Vec3D&);

  RecTriangle(const RecTriangle<Shape>&);
  RecTriangle<Shape>& operator=(const RecTriangle<Shape>&);
  ~RecTriangle() {}

  /// Access level 
  int getLevel() const { return level; }
  /// Get Active point:
  Geometry::Vec3D getNext();
  
};

} 

#endif
