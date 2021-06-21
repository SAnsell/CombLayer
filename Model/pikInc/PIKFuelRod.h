/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/pikInc/PIKFuelRod.h
 *
 * Copyright (c) 2004-2021 by Konstantin Batkov
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
#ifndef pikSystem_PIKFuelRod_h
#define pikSystem_PIKFuelRod_h

class Simulation;

namespace pikSystem
{

/*!
  \class PIKFuelRod
  \version 1.0
  \author Konstantin Batkov
  \date June 2021
  \brief PIK fuel rod (ТВЭЛ)
*/

class PIKFuelRod : public attachSystem::ContainedComp,
                    public attachSystem::FixedRotate,
                    public attachSystem::CellMap,
                    public attachSystem::SurfMap
{
 private:

  double length;                ///< Total length including void
  double width;                 ///< Width
  double height;                ///< Height

  int mainMat;                   ///< Main material

  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  PIKFuelRod(const std::string&);
  PIKFuelRod(const PIKFuelRod&);
  PIKFuelRod& operator=(const PIKFuelRod&);
  virtual PIKFuelRod* clone() const;
  virtual ~PIKFuelRod();

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,const long int);

};

}

#endif
