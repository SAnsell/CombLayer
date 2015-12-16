/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   beamlineInc/PlateUnit.h
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
#ifndef beamlineSystem_PlateUnit_h
#define beamlineSystem_PlateUnit_h

namespace Geometry
{
  class Convex2D;
}


namespace beamlineSystem
{

/*!
  \class PlateUnit
  \version 1.0
  \author S. Ansell
  \date February 2014
  \brief Points associated with tracked beamline projections
*/

class PlateUnit : public ShapeUnit 
{
 private:

  Geometry::Convex2D* CHPtr;   ///< Convex hull ptr

  Geometry::Vec3D XVec;    ///< Current XVector
  Geometry::Vec3D YVec;    ///< Current YVector
  Geometry::Vec3D ZVec;    ///< Current ZVector

  size_t nCorner;                     ///< number of corner points
  bool rotateFlag;                    ///< Rotation on points
  std::vector<Geometry::Vec3D> APts;  ///< Points of front shape
  std::vector<Geometry::Vec3D> BPts;  ///< Points of Tail shape
  std::vector<int> nonConvex;         ///< Points are non-convex

  static size_t findFirstPoint(const Geometry::Vec3D&,
			       const std::vector<Geometry::Vec3D>&); 

  Geometry::Vec3D frontPt(const size_t,const double) const;
  Geometry::Vec3D backPt(const size_t,const double) const;

 public:

  PlateUnit(const int,const int);
  PlateUnit(const PlateUnit&);
  PlateUnit& operator=(const PlateUnit&);
  virtual PlateUnit* clone() const;
  virtual ~PlateUnit();

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

  Geometry::Vec3D getBegAxis() const { return -YVec; }
  Geometry::Vec3D getEndAxis() const { return YVec; }

  virtual std::string getString(const ModelSupport::surfRegister&,
				const size_t) const;
  virtual std::string getExclude(const ModelSupport::surfRegister&,
				 const size_t) const;

  virtual void createSurfaces(ModelSupport::surfRegister&,
			      const std::vector<double>&);
};


}

#endif
 
