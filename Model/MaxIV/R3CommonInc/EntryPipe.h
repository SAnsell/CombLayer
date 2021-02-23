/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   R3CommonInc/EntryPipe.h
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *v
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ****************************************************************************/
#ifndef constructSystem_EntryPipe_h
#define constructSystem_EntryPipe_h

class Simulation;

namespace xraySystem
{

/*!
  \class EntryPipe
  \version 1.0
  \author S. Ansell
  \date July 2015
  \brief EntryPipe unit
*/

class EntryPipe :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedGroup,
  public attachSystem::CellMap,
  public attachSystem::SurfMap,
  public attachSystem::FrontBackCut
{
 private:


  double radius;                ///< void radius [inner]
  double length;                ///< void length [total]
  double wallThick;               ///< pipe thickness

  double flangeRadius;          ///< Joining Flange radius [-ve for rect]
  double flangeLength;          ///< Joining Flange length

  int voidMat;                  ///< Void material
  int wallMat;                  ///< Pipe material
  int flangeMat;                ///< Flange material

  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:
  
  EntryPipe(const std::string&);
  EntryPipe(const EntryPipe&);
  EntryPipe& operator=(const EntryPipe&);
  virtual ~EntryPipe();

  using FixedComp::createAll;
  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
			 const long int);

};

}

#endif
