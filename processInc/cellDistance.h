/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   processInc/cellDistance.h
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#ifndef ModelSupport_cellDistance_h
#define ModelSupport_cellDistance_h

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

class cellDistance
{
 private:
  
  const Simulation* ASim;              ///< Simulation
  MonteCarlo::Object* initObj;   ///< Initial object
  int aRange;                          ///< Cells that constitute a hit
  int bRange;                          ///< Cells that constistute a hit
  
  void setRange(const std::string&);


 public:

  
  cellDistance(const Simulation&);
  cellDistance(const cellDistance&);
  cellDistance& operator=(const cellDistance&);
  ~cellDistance() {}  ///< Destrcutro

  Geometry::Vec3D calcPoint(const Geometry::Vec3D&,
			    const Geometry::Vec3D&,
			    const std::string&);
  
};

}

#endif
