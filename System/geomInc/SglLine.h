/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geomInc/SglLine.h
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
#ifndef Geometry_SglLine_h
#define Geometry_SglLine_h

namespace Geometry 
{

/*!
  \class SglLine
  \brief Single line
  \version 1.0
  \date December 2010
  \author S. Ansell
 */

class SglLine : public Intersect
{
 private:

  int index;             ///< Creation index
  Vec3D APt;             ///< A-Start Point
  Vec3D NormV;           ///< Normal 
  
 public:
  
  SglLine(const Geometry::Vec3D&,const Geometry::Vec3D&);
  SglLine(const int,const Vec3D&,const Vec3D&);
  SglLine(const SglLine&);
  SglLine& operator=(const SglLine&);
  virtual ~SglLine() {}  ///< Destructor
  SglLine& operator()(const Geometry::Vec3D&,const Geometry::Vec3D&);

  /// Debug function to get index
  int getIndex() const { return index; }
  /// Get point
  const Geometry::Vec3D& getPt() const { return APt; }   
  /// Get direction
  const Geometry::Vec3D& getNorm() const { return NormV; } 

  /// Calc area [-ve is undefined 
  double area() const { return -1.0;}

  /// calculate the centre [ Mid point of lines ]
  Vec3D centre() const { return APt; }  

  Vec3D ParamPt(const double) const;

  void write(std::ostream&) const;
};

}   // NAMESPACE Geometry

#endif
