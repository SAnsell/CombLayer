/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   t1UpgradeInc/TriUnit.h
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#ifndef moderatorSystem_TriUnit_h
#define moderatorSystem_TriUnit_h

namespace Geometry
{
  class Convex2D;
}


namespace moderatorSystem
{

/*!
  \class TriUnit
  \version 1.0
  \author S. Ansell
  \date February 2014
  \brief Points associated with triangle spaces
*/

class TriUnit 
{

  const int offset;            ///< Offset number
  std::vector<int> cells;      ///< Cell numbers
  Geometry::Convex2D* CHPtr;   ///< Convex hull ptr

  static size_t findFirstPoint(const Geometry::Vec3D&,
			       const std::vector<Geometry::Vec3D>&); 
  

 public:

  size_t nCorner;         ///< number of corner points  
  std::vector<Geometry::Vec3D> Pts;  ///< Points of trianlge/shape 
  std::vector<int> absolute;         ///< Points are absolue
  std::vector<int> nonConvex;        ///< Points are non-convex

  TriUnit(const int);
  TriUnit(const TriUnit&);
  TriUnit& operator=(const TriUnit&);
  ~TriUnit();

  void constructConvex(const Geometry::Vec3D&);

  void clear();
  void renormalize(const Geometry::Vec3D&,const Geometry::Vec3D&,
		   const Geometry::Vec3D&,const Geometry::Vec3D&);
  void applyAbsShift(const double,const double,const double);

  int inHull(const Geometry::Vec3D&) const;
  void addCell(const int);
  /// access cells
  const std::vector<int>& getCells() const { return cells; }

  void addObject(const Geometry::Vec3D&,const int);
  int getOffset() const { return offset; }  ///< Access offset

  std::string getString(const size_t) const;
  std::string getExclude(const size_t) const;

};

}

#endif
 
