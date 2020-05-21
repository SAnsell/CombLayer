/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/pikInc/PIKPool.h
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
#ifndef pikSystem_PIKPool_h
#define pikSystem_PIKPool_h

class Simulation;

namespace pikSystem
{

/*!
  \class PIKPool
  \version 1.0
  \author Konstantin Batkov
  \date May 2020
  \brief PIK reactor pool
*/

class PIKPool :
    public attachSystem::ContainedComp,
    public attachSystem::FixedRotate,
    public attachSystem::CellMap
{
 private:

  double waterRadius;           ///< water radius
  double height;                ///< water and container height
  double depth;                 ///< water and shield depth
  double shieldRadius;          ///< water container radius
  double shieldWidth;           ///< container width

  int waterMat;                 ///< water material
  int shieldMat;                ///< container material

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  PIKPool(const std::string&);
  PIKPool(const PIKPool&);
  PIKPool& operator=(const PIKPool&);
  virtual PIKPool* clone() const;
  virtual ~PIKPool();

  void createAll(Simulation&,const attachSystem::FixedComp&,const long int);

};

}

#endif
