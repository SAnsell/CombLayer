/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   geomInc/Circle.h
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
#ifndef Geometry_Circle_h
#define Geometry_Circle_h

namespace Geometry 
{

/*!
  \class Circle
  \brief Circle in a 3D system
  \version 1.0
  \date December 2010
  \author S. Ansell
 */

class Circle : public Intersect
{
 private:

  int index;              ///< Creation index
  Vec3D Cent;             ///< Current points 
  Vec3D NormV;            ///< Normal
  double Radius;          ///< Radius
  
 public:
  
  Circle();
  Circle(const Vec3D&,const Vec3D&,const double);
  Circle(const int,const Vec3D&,const Vec3D&,const double);
  Circle(const Circle&);
  Circle& operator=(const Circle&);
  virtual ~Circle() {}  ///< Destructor
  Circle& operator()(const Vec3D&,const Vec3D&,const double);

  /// Debug function to get index
  int getIndex() const { return index; }
  /// Get direction
  const Geometry::Vec3D& getNorm() const { return NormV; } 
  /// Get Centre
  const Geometry::Vec3D& getCentre() const { return Cent; }  
  /// Get Radius
  double getRadius() const { return Radius; }   
  /// Calc area
  double area() const { return Radius*Radius*M_PI;}

  
  /// Calc centre
  bool hasCentre() const { return true; }

  /// calculate the centre
  Vec3D centre() const { return Cent; }  
  Vec3D ParamPt(const double) const;

  void write(std::ostream&) const;
};

}   // NAMESPACE Geometry

#endif
