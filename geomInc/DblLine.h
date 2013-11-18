/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   geomInc/DblLine.h
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
#ifndef Geometry_DblLine_h
#define Geometry_DblLine_h

namespace Geometry 
{

/*!
  \class DblLine
  \brief Twin Parallel line
  \version 1.0
  \date December 2010
  \author S. Ansell
 */

class DblLine : public Intersect
{
 private:

  int index;             ///< Creation index
  Vec3D APt;             ///< A-Start Point
  Vec3D BPt;             ///< B-Start points 
  Vec3D NormV;           ///< Normal [Paralle
  
 public:
  
  DblLine();
  DblLine(const Vec3D&,const Vec3D&,const Vec3D&);
  DblLine(const int,const Vec3D&,const Vec3D&,const Vec3D&);
  DblLine(const DblLine&);
  DblLine& operator=(const DblLine&);
  virtual ~DblLine() {}  ///< Destructor
  DblLine& operator()(const Vec3D&,const Vec3D&,const Vec3D&);

  /// Debug function to get index
  int getIndex() const { return index; }
  const Geometry::Vec3D& getPtA() const { return APt; }   ///< Get point
  const Geometry::Vec3D& getPtB() const { return BPt; }   ///< Get point
  const Geometry::Vec3D& getNorm() const { return NormV; } ///< Get direction

  /// Calc area [-ve is undefined 
  double area() const { return -1.0;}

  /// calculate the centre [ Mid point of lines ]
  Vec3D centre() const { return (APt+BPt)/2.0; }  
  Vec3D ParamPt(const double) const;

  void write(std::ostream&) const;
};

}   // NAMESPACE Geometry

#endif
