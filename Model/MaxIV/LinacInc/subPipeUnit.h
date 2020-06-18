/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   linacInc/subPipeUnit.h
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#ifndef tdcSystem_subPipeUnit_h
#define tdcSystem_subPipeUnit_h

class Simulation;

namespace tdcSystem
{

  /*!
  \class subPipeUnit
  \version 1.0
  \author S. Ansell
  \date June 2020
  \brief Sub-pipe that linkes to flang
*/

struct subPipeUnit
{
  public:

  const std::string keyName;   ///< variable keyname
   
  double xStep;            ///< Offset of x
  double zStep;            ///< Offset of y
  double xyAngle;          ///< Rotation angle
  double zAngle;           ///< Rotation angle
  double radius;           ///< void radius [inner] 
  double length;           ///< void length [total]
  double thick;            ///< wall thickness
  
  double flangeRadius;     ///< Flange outer radius
  double flangeLength;     ///< Flange length
  
  int voidMat;             ///< void material
  int wallMat;             ///< Wall material
  int flangeMat;           ///< flange material
  
  subPipeUnit(const std::string& K) : keyName(K) {};
  subPipeUnit(const subPipeUnit&);
  subPipeUnit& operator=(const subPipeUnit&);
  subPipeUnit() {}
  
  void populate(const FuncDataBase&);
};

}

#endif
