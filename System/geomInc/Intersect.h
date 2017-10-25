/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geomInc/Intersect.h
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
#ifndef Geometry_Intersect_h
#define Geometry_Intersect_h

namespace Geometry 
{

/*!
  \class Intersect
  \brief Intersect of all 3D shapes
  \version 1.0
  \date December 2010
  \author S. Ansell
  
  Base class for the 3D shapes
 */

class Intersect
{
 public:

  Intersect() {}        ///< Constructor
  Intersect(const Intersect&) {}  ///< Copy constructor
  /// Assignment operator
  Intersect& operator=(const Intersect&) { return *this; }
  virtual ~Intersect() {}  ///< Destructor

  virtual bool hasCentre() const { return false; }      ///< Has no centre

  ///\cond ABSTRACT
  virtual double area() const =0;
  virtual Vec3D centre() const =0;
  virtual Geometry::Vec3D ParamPt(const double) const =0;
  virtual void write(std::ostream&) const =0;
  ///\endcond ABSTRACT
};


std::ostream& operator<<(std::ostream& OX,const Intersect&);

}   // NAMESPACE Geometry

#endif
