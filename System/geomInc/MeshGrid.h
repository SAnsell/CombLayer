/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   geomInc/MeshGrid.h
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
#ifndef Geometry_MeshGrid_h
#define Geometry_MeshGrid_h

namespace Geometry
{
/*!
  \class MeshGrid
  \brief RecTriangle mapping of a Grid
  \version 1.0
  \date May 2011
  \author S. Ansell
 */

class MeshGrid
{
 private:

  const Geometry::Vec3D Origin;     ///< Lower base origin
  const Geometry::Vec3D XAxis;      ///< X-Axis [NOT unit]
  const Geometry::Vec3D YAxis;      ///< Y-Axis [NOT unit]

  int level;           ///< depth level
  int type;            ///< Type

  std::vector<double> fracUnits;      ///< Fraction to process
  std::vector<double> partUnits;      ///< Fraction to process


  size_t AIndex;             ///< Full index
  size_t BIndex;             ///< Part index

  void createNextLevel();
  void updateAB(const size_t,const size_t);

 public:
  
  /// Generic constructor
  MeshGrid(const Geometry::Vec3D&,
	   const Geometry::Vec3D&,
	   const Geometry::Vec3D&);
  
  MeshGrid(const MeshGrid&);
  MeshGrid& operator=(const MeshGrid&);
  ~MeshGrid() {}

  /// Access level 
  int getLevel() const { return level; }
  /// Get Active point:
  Geometry::Vec3D getNext();
  
};

} 

#endif
