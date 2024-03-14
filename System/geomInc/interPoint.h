/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geomInc/interPoint.h
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
#ifndef interPoint_h
#define interPoint_h

class Token;
class Rule;
class CompGrp;
class SurfPoint;

namespace Geometry
{
  class Surface;

  /*!
    \struct interPoint
    \brief Holds an intersection with surf/pt/direction
    \author S. Ansell
    \version 1.0
    \date February 2024
  */
  
  struct interPoint
  {
    Geometry::Vec3D Pt;     ///< Crosssing point;
    double D;               ///< Distance
    int SNum;               ///< signed surf number [true as particle moves fwd]
    const Surface* SPtr;    ///< SurfPointer
    bool outFlag;           ///< true if particle leaves
    
    explicit interPoint() :
      D(0.0),SNum(0),SPtr(nullptr),outFlag(0) {}
    explicit interPoint(Geometry::Vec3D P) :
      Pt(std::move(P)),D(0.0),SNum(0),SPtr(nullptr),outFlag(0) {}
    interPoint(const Geometry::Vec3D& P,const int SN) :
      Pt(std::move(P)),D(0.0),SNum(SN),SPtr(nullptr),outFlag(0) {}
    interPoint(Geometry::Vec3D P,const double dist,const int SN) :
      Pt(std::move(P)),D(dist),SNum(SN),SPtr(nullptr),outFlag(0) {}
    interPoint(Geometry::Vec3D P,const double dist,
	       const int SN,const Surface* ptr,const bool flag) :
    Pt(std::move(P)),D(dist),SNum(SN),SPtr(ptr),outFlag(flag) {}

  };

void sortVector(std::vector<interPoint>&);

std::ostream&
operator<<(std::ostream& OX,const interPoint& A);
 
}

#endif
