/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   heimdalInc/LegoBrack.h
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
  \date May 2012
  \brief LegoBrick system
*/

class LegoBrick : 
  public attachSystem::FixedRotate,
  public attachSystem::ContainedComp,
  public attachSystem::ExternalCut
{
 private:

  double width;                 ///< full width
  double height;                ///< full height
  double depth;                 ///< full depth (z)

  int mat;

  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);

 public:

  LegoBrick(const std::string&);
  LegoBrick(const LegoBrick&);
  LegoBrick& operator=(const LegoBrick&);
  ~LegoBrick();

  using FixedComp::createAll;
  void createAll(Simulation&,
		 const attachSystem::FixedComp&,
		 const long int);
};

}

#endif
 
