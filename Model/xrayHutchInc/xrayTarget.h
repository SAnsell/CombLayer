/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   xrayHutchInc/xrayTarget.h
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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
#ifndef xrayHutSystem_xrayTarget_h
#define xrayHutSystem_xrayTarget_h

class Simulation;

namespace xrayHutSystem
{
/*!
  \class xrayTarget
  \author S. Ansell
  \version 1.0
  \date May 2022
  \brief Simple hutch

  Constructed hutch -- the intent is to add chicanes/openings etc
*/

class xrayTarget :
  public attachSystem::ContainedComp,
  public attachSystem::FixedRotate,
  public attachSystem::CellMap
{
 private:
  
  double length;            ///< Total length
  double height;            ///< Total height 
  double width;             ///< Total width

  int mat;              ///< wall material
  
  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  xrayTarget(const std::string&);
  xrayTarget(const xrayTarget&);
  xrayTarget& operator=(const xrayTarget&);
  ~xrayTarget();

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);
    
};

}

#endif
 
