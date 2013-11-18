/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   geomInc/Transform.h
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
#ifndef Geometry_Transform_h
#define Geometry_Transform_h

namespace Geometry
{

/*!
  \class Transform
  \brief Holds the transform operations for surfaces
  \author S. Ansell
  \date 2004
  \version 1.0

  Holds and can supply the rotation and movements
  transform objects.
*/

class Transform
{
 private:
  
  int Nmb;                  ///< Name (index number)
  Geometry::Matrix<double> Rot;       ///< Rotation matrix 
  Geometry::Vec3D Shift;              ///< Shift vector 

 public:
  
  Transform();
  Transform(const Transform&);
  Transform& operator=(const Transform&);
  ~Transform();
  
  /// Get Rotation matrix
  const Geometry::Matrix<double>& rotMat() const { return Rot; }  
  /// Get Shift
  Geometry::Vec3D shift() const { return Shift; }
  
  /// Set the identifier
  void setName(const int N) { Nmb=N; }
  /// Get the identifier
  int getName() const { return Nmb; }
  int setTransform(const std::string&,const std::string&); 
  int setTransform(const std::string&);
  int setTransform(const Geometry::Vec3D&,const Geometry::Matrix<double>&);
  void print() const;
  void write(std::ostream&) const;
};

std::ostream&
operator<<(std::ostream&,const Transform&);

}

#endif
