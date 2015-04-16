/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   chipInc/cylTrack.h
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
#ifndef ModelSupport_cylTrack_h
#define ModelSupport_cylTrack_h

namespace ModelSupport
{
 
class surfRegister;

  /*!
    \struct cylTrack 
    \version 1.0
    \author S. Ansell
    \date September 2012
    \brief Holds simple things about the cylinder
  */

struct cylTrack
{
 public:
  
  Geometry::Vec3D APt;  ///< Start point
  Geometry::Vec3D BPt;  ///< End point

  double IRadius;   ///< Inner Radius 
  double ORadius;   ///< Outer Radius 
  int MatN;         ///< Material number
  double Temp;      ///< Radius 
  
  /// Basic constructor
  cylTrack(const double& iR,const double& oR,
	    const int& M,const double& T) :
  IRadius(iR),ORadius(oR),MatN(M),Temp(T) {}  
  void write(std::ostream& OX) const
  { OX<<"M("<<MatN<<") R: "<<IRadius<<" ++ "<<ORadius; }
};

/// Output stream operator
std::ostream& operator<<(std::ostream&,const cylTrack&);

}

#endif
