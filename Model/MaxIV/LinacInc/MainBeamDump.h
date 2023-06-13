/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   LinacInc/MainBeamDump.h
 *
 * Copyright (c) 2004-2023 by Konstantin Batkov
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
#ifndef tdcSystem_MainBeamDump_h
#define tdcSystem_MainBeamDump_h

class Simulation;


namespace tdcSystem
{
/*!
  \class MainBeamDump
  \version 1.1
  \author S. Ansell / K. Batkov
  \date February 2021

  \brief Electron beam stop for MAX-IV
*/

class MainBeamDump :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedGroup,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  double width;                 ///< void radius
  double length;                ///< void length [total]
  double height;                ///< void height
  double depth;                 ///< void depth

  double wallThick;             /// main wall thickness
  double portLength;            ///< Port length

  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  MainBeamDump(const std::string&);
  MainBeamDump(const MainBeamDump&);
  MainBeamDump& operator=(const MainBeamDump&);
  virtual ~MainBeamDump();

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
