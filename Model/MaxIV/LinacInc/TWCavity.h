/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/LinacInc/TWCavity.h
 *
 * Copyright (c) 2004-2020 by Konstantin Batkov
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
#ifndef tdcSystem_TWCavity_h
#define tdcSystem_TWCavity_h

class Simulation;

namespace tdcSystem
{

/*!
  \class TWCavity
  \version 1.0
  \author Konstantin Batkov
  \date June 2020
  \brief Traveling wave cavity
*/

class TWCavity :
    public attachSystem::ContainedComp,
    public attachSystem::FixedRotate,
    public attachSystem::CellMap,
    public attachSystem::SurfMap,
    public attachSystem::FrontBackCut
{
 private:

  int    nCells;                ///< Number of regular cells
  double cellLength;            ///< Normal cell total length (void+iris)
  double cellRadius;            ///< Normal cell inner radius
  double irisLength;            ///< Iris length
  double irisRadius;            ///< Iris inner radius
  double couplerLength;         ///< coupler cell length
  double couplerWidth;          ///< coupler cell width
  double wallThick;             ///< Wall thickness
  int    wallMat;               ///< Wall material

  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  TWCavity(const std::string&);
  TWCavity(const TWCavity&);
  TWCavity& operator=(const TWCavity&);
  virtual TWCavity* clone() const;
  virtual ~TWCavity();

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,const long int);

};

}

#endif
