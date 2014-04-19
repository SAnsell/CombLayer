/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   beamlineInc/ShapeUnit.h
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
#ifndef beamlineSystem_ShapeUnit_h
#define beamlineSystem_ShapeUnit_h

namespace Geometry
{
  class Convex2D;
}


namespace beamlineSystem
{

/*!
  \class ShapeUnit
  \version 1.0
  \author S. Ansell
  \date February 2014
  \brief Points associated with tracked beamline projections
*/

class ShapeUnit 
{
 private:

  const int offset;            ///< Offset number
  const int layerSep;          ///< Layer seperation count

  std::vector<int> cells;      ///< Cell numbers
  Geometry::Convex2D* CHPtr;   ///< Convex hull ptr

  Geometry::Vec3D begPt;   ///< Current start point
  Geometry::Vec3D endPt;   ///< Current exit point
  Geometry::Vec3D XVec;    ///< Current XVector
  Geometry::Vec3D YVec;    ///< Current YVector
  Geometry::Vec3D ZVec;    ///< Current ZVector

  size_t nCorner;                     ///< number of corner points  
  std::vector<Geometry::Vec3D> APts;  ///< Points of front shape
  std::vector<Geometry::Vec3D> BPts;  ///< Points of Tail shape
  std::vector<int> nonConvex;         ///< Points are non-convex

  static size_t findFirstPoint(const Geometry::Vec3D&,
			       const std::vector<Geometry::Vec3D>&); 

  Geometry::Vec3D frontPt(const size_t) const;
  Geometry::Vec3D backPt(const size_t) const;
  std::pair<Geometry::Vec3D,Geometry::Vec3D>
    frontPair(const size_t,const size_t,const double) const;
  std::pair<Geometry::Vec3D,Geometry::Vec3D>
    backPair(const size_t,const size_t,const double) const;

  std::pair<Geometry::Vec3D,Geometry::Vec3D>
    scaledPair(const Geometry::Vec3D&,const Geometry::Vec3D&,
	       const double) const;
  Geometry::Vec3D sideNorm(const std::pair<Geometry::Vec3D,
			    Geometry::Vec3D>&) const;
  
  
 public:

  ShapeUnit(const int,const int);
  ShapeUnit(const ShapeUnit&);
  ShapeUnit& operator=(const ShapeUnit&);
  virtual ShapeUnit* clone() const;
  virtual ~ShapeUnit();

  void setTrack(const Geometry::Vec3D*,const Geometry::Vec3D&,
		const Geometry::Vec3D&,const double,const double,
		const double);

  void setXAxis(const Geometry::Vec3D&,const Geometry::Vec3D&);
  void setEndPts(const Geometry::Vec3D&,const Geometry::Vec3D&);

  void constructConvex();

  void clear();

  int inHull(const Geometry::Vec3D&) const;
  void addCell(const int);
  /// access cells
  const std::vector<int>& getCells() const { return cells; }

  void addPrimaryPoint(const Geometry::Vec3D&);
  void addPairPoint(const Geometry::Vec3D&,const Geometry::Vec3D&);
  int getOffset() const { return offset; }  ///< Access offset

  /// Accessor 
  const Geometry::Vec3D& getBegin() const { return begPt; }
  /// Accessor
  const Geometry::Vec3D& getEnd() const { return endPt; }
  /// Accessor
  const Geometry::Vec3D& getAxis() const { return YVec; }

  std::string getString(const size_t) const;
  std::string getExclude(const size_t) const;

  void createSurfaces(ModelSupport::surfRegister&,const int,
		      const std::vector<double>&);
};

}

#endif
 
