/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   include/masterRotate.h
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
#ifndef masterRotate_h
#define masterRotate_h

class transComp;

/*!
  \class masterRotate
  \version 2.0
  \date November 2011
  \author S. Ansell
  \brief Total axis rotations for model 

  Extended from localRotate to be the one global rotate
*/

class masterRotate : public localRotate
{
 private:

  int globalApplied;
  
  masterRotate();

  ///\cond SINGLETON
  masterRotate(const masterRotate&);
  masterRotate& operator=(const masterRotate&);
  ///\endcond SINGLETON
  
 public:
  
  /// Desctructor
  virtual ~masterRotate() {} 
  
  static masterRotate& Instance();

  virtual Geometry::Vec3D calcRotate(const Geometry::Vec3D&) const;
  virtual Geometry::Vec3D calcAxisRotate(const Geometry::Vec3D&) const;

  virtual Geometry::Vec3D forceCalcRotate(const Geometry::Vec3D&) const;
  virtual Geometry::Vec3D forceCalcAxisRotate(const Geometry::Vec3D&) const;

  virtual Geometry::Vec3D reverseRotate(const Geometry::Vec3D&) const;
  virtual Geometry::Vec3D reverseAxisRotate(const Geometry::Vec3D&) const;

  virtual Geometry::Vec3D forceReverseRotate(const Geometry::Vec3D&) const;
  virtual Geometry::Vec3D forceReverseAxisRotate(const Geometry::Vec3D&) const;

  virtual void applyFull(MonteCarlo::Object*) const;
  virtual void applyFull(Geometry::Surface*) const;
  virtual void applyFull(Geometry::Vec3D&) const;
  virtual void applyFullAxis(Geometry::Vec3D&) const;


  /// Rotation applied
  void setGlobal() { globalApplied=1; }
  /// Rotation not-applied
  void clearGlobal() { globalApplied=0; }
  

};

#endif
