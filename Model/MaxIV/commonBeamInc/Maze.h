/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/Maze.h
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#ifndef xraySystem_Maze_h
#define xraySystem_Maze_h

class Simulation;

namespace xraySystem
{
  /*!
    \class Maze
    \version 1.0
    \author S. Ansell
    \date June 2018
    \brief Extra beam stop for opticshutch
  */
  
class Maze :
  public attachSystem::ContainedGroup,
  public attachSystem::FixedOffset,
  public attachSystem::CellMap,
  public attachSystem::SurfMap,
  public attachSystem::ExternalCut
{
 private:

  double height;                 ///< height of inner gap
  double width;                  ///< width of inner gap

  double mainOutLength;      ///< Y step to X-wall
  double mainThick;          ///< Thickness [both]
  double mainXWidth;         ///< Length from middle to end 

  double catchOutLength;     ///< Y step to X-wall
  double catchThick;         ///< Thickness [both]
  double catchXGap;          ///< X step to start of side wall
  double catchXWidth;        ///< Length from middle to end A
  

  int wallMat;                   ///< wall material

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

  
 public:
  
  Maze(const std::string&);
  Maze(const Maze&);
  Maze& operator=(const Maze&);
  virtual ~Maze() {}  ///< Destructor


  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);
};

}

#endif
 
