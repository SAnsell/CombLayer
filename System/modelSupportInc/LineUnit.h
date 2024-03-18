/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   modelSupportInc/LineUnit.h
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
#ifndef ModelSupport_LineUnit_h
#define ModelSupport_LineUnit_h

class Simulation;

namespace Geometry
{
  class Surface;
}

namespace MonteCarlo
{
  class Object;
}

namespace ModelSupport
{

/*!
  \class LineUnit
  \version 1.0
  \author S. Ansell
  \date July 2011
  \brief Track from A to B 
*/

struct LineUnit
{

  int cellNumber;                         ///< Cell number
  Geometry::Vec3D exitPoint;              ///< exit point [or end point]
  MonteCarlo::Object* objVec;             ///< Object for segment
  const Geometry::Surface* sPtr;          ///< Exiting surf point
  int surfNumber;                         ///< Exiting surf number [0 for end]
  double segmentLength;                   ///< Length of segment -1 for infity

  LineUnit(const LineUnit&) =default;
  LineUnit(LineUnit&&) =default;
  LineUnit& operator=(const LineUnit&) =default;
  LineUnit& operator=(LineUnit&&) =default;
  ~LineUnit() = default;
  
  void write(std::ostream&) const;
};

std::ostream& operator<<(std::ostream&,const LineUnit&);

}

#endif
