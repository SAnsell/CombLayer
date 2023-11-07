/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   heimdalInc/LegoBrack.h
 *
 *  Modified: Isabel Llamas-Jansa, November 2023 
 * - added comments and separation lines for self
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
#ifndef essSystem_LegoBrick_h
#define essSystem_LegoBrick_h

class Simulation;

namespace essSystem
{

/*!
  \class LegoBrick
  \version 1.0
  \author S. Ansell
  (modified I. Llamas-Jansa)
  \date Sept 2023
  \brief LegoBrick system
*/

class LegoBrick : 
  public attachSystem::FixedRotate,
  public attachSystem::ContainedComp,
  public attachSystem::ExternalCut
{
 private:

  double width;                 ///< full width (x)
  double height;                ///< full height (z)
  double depth;                 ///< full depth (y)

  int mat;

  // functions used in the .cxx file
  // I have added variables to be the same as in .cxx
  void populate(const FuncDataBase& Control);
  void createSurfaces();
  void createObjects(Simulation& System);

 public:

  LegoBrick(const std::string& key);
  LegoBrick(const LegoBrick& A);
  LegoBrick& operator=(const LegoBrick& A);
  ~LegoBrick();

  using FixedComp::createAll;
  void createAll(Simulation& System,
		 const attachSystem::FixedComp& FC,
		 const long int sideIndex);
};

}

#endif
 
