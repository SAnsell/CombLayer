/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   LinacInc/SixPortPump.h
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
#ifndef tdcSystem_SixPortPump_h
#define tdcSystem_SixPortPump_h

class Simulation;


namespace tdcSystem
{

  /*!
  \class SixPortPump
  \version 1.0
  \author S. Ansell
  \date May 2020

  \brief SixPortPump for Max-IV
*/

class SixPortPump :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedComp,
  public attachSystem::FrontBackCut,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  double radius;               ///< void radius
  double linkRadius;           ///< void radius on 4 cross way
  double wallThick;            ///< pipe thickness

  double frontLength;            ///< full to flange length
  double backLength;             ///< full to flange length
  double sideLength;              ///< full to flange length

  double flangeARadius;         ///< Joining Flange radius
  double flangeBRadius;         ///< Joining Flange radius
  double flangeSRadius;         ///< Joining Flange radius

  double flangeALength;         ///< Joining Flange radius
  double flangeBLength;         ///< Joining Flange radius
  double flangeSLength;         ///< Joining Flange radius
  
  double plateThick;         ///< Joining Flange radius

  int voidMat;                 ///< void material
  int mainMat;                 ///< main material
  int flangeMat;               ///< flange material
  int plateMat;                ///< plate material
 
  void populate(const FuncDataBase&);  
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  SixPortPump(const std::string&);
  SixPortPump(const std::string&,const std::string&);
  SixPortPump(const SixPortPump&);
  SixPortPump& operator=(const SixPortPump&);
  virtual ~SixPortPump();

  using FixedComp::createAll;
  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
