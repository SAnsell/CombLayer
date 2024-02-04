/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geomInc/Ellipse.h
 *
 * Copyright (c) 2004-2024 by Stuart Ansell
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
#ifndef Geometry_Ellipse_h
#define Geometry_Ellipse_h

namespace Geometry 
{

/*!
  \class Ellipse
  \brief Ellipse in a 3D system
  \version 1.0
  \date December 2010
  \author S. Ansell
 */

class Ellipse : public Intersect
{
 private:

  int index;              ///< Creation index
  Vec3D Cent;             ///< Current points 
  Vec3D minorAxis;        ///< Minor axis [ Half ellipse size]
  Vec3D majorAxis;        ///< Major axis [ Half ellipse size]
  Vec3D NormV;            ///< Normal
  
 public:
  
  Ellipse();
  Ellipse(Vec3D ,Vec3D ,Vec3D ,const Vec3D&);
  Ellipse(const int,Vec3D ,Vec3D ,Vec3D ,const Vec3D&);
  Ellipse(const Ellipse&);
  Ellipse& operator=(const Ellipse&);
  ~Ellipse() override {}  ///< Destructor
  Ellipse& operator()(const Vec3D&,const Vec3D&,const Vec3D&,const Vec3D&);

  /// Debug function to get index
  int getIndex() const { return index; }

  /// Calc area
  double area() const override { return -1.0;}
  /// Calc centre
  bool hasCentre() const override { return true; }
  /// calculate the centre
  Vec3D centre() const override { return Cent; }  

  Vec3D ParamPt(const double) const override;

  void write(std::ostream&) const override;
};

}   // NAMESPACE Geometry

#endif
