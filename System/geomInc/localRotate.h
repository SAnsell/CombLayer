/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geomInc/localRotate.h
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
#ifndef Geometry_localRotate_h
#define Geometry_localRotate_h

class transComp;
namespace MonteCarlo
{
  class Object;
}
namespace Geometry
{
  class Surface;
  class Plane;
}
/*!
  \class localRotate
  \version 1.0
  \date October 2009
  \author S. Ansell
  \brief Set of axis/displacement rotations for model
*/

class localRotate
{
 private:

  typedef std::shared_ptr<transComp> TPtr;  ///< transform TYPE
  std::vector<TPtr> Transforms;               ///< Transforms
  
  void axisRotate(Geometry::Vec3D&) const;
  void axisRotateReverse(Geometry::Vec3D&) const;
  
 public:
  
  localRotate();
  localRotate(const localRotate&);
  localRotate& operator=(const localRotate&);
  /// Desctructor
  virtual ~localRotate() {} 

  void addRotation(const Geometry::Vec3D&,
		   const Geometry::Vec3D&,
		   const double);
  void addMirror(const Geometry::Plane&); 
  void addDisplace(const Geometry::Vec3D&);

  virtual Geometry::Vec3D calcRotate(const Geometry::Vec3D&) const;
  virtual Geometry::Vec3D calcAxisRotate(const Geometry::Vec3D&) const;

  virtual Geometry::Vec3D reverseRotate(const Geometry::Vec3D&) const;
  virtual Geometry::Vec3D reverseAxisRotate(const Geometry::Vec3D&) const;

  void reset();

  virtual void applyFull(MonteCarlo::Object*) const;
  virtual void applyFull(Geometry::Surface*) const;
  virtual void applyFull(Geometry::Vec3D&) const;

  virtual void applyFullAxis(Geometry::Vec3D&) const;

};

#endif
