/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   beamlineInc/ShapeUnit.h
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
 protected:

  const int shapeIndex;        ///< Offset number
  const int layerSep;          ///< Layer seperation count

  std::vector<int> cells;      ///< Cell numbers

  Geometry::Vec3D begPt;   ///< Current start point
  Geometry::Vec3D endPt;   ///< Current exit point

    
 public:

  ShapeUnit(const int,const int);
  ShapeUnit(const ShapeUnit&);
  ShapeUnit& operator=(const ShapeUnit&);
  virtual ShapeUnit* clone() const =0;
  virtual ~ShapeUnit();

  void setEndPts(const Geometry::Vec3D&,const Geometry::Vec3D&);
  void addCell(const int);
  /// access cells
  const std::vector<int>& getCells() const { return cells; }

  //  int getOffset() const { return offset; }  ///< Access offset

  /// Accessor 
  const Geometry::Vec3D& getBegin() const { return begPt; }
  /// Accessor
  const Geometry::Vec3D& getEnd() const { return endPt; }
  virtual Geometry::Vec3D getBegAxis() const =0; 
  virtual Geometry::Vec3D getEndAxis() const =0;

  virtual std::string getString(const ModelSupport::surfRegister&,
				const size_t) const =0;
  virtual std::string getExclude(const ModelSupport::surfRegister&,
				 const size_t) const =0;

  virtual void createSurfaces(ModelSupport::surfRegister&,
		      const std::vector<double>&) =0;
};


}

#endif
 
