/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   t1UpgradeInc/voidCylVolume.h
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#ifndef constructSystem_voidCylVolume_h
#define constructSystem_voidCylVolume_h

class Simulation;

namespace constructSystem
{

  class WedgeInsert;

/*!
  \class voidCylVolume
  \author S. Ansell
  \version 1.0
  \date November 2013
  \brief Specialized for a cylinder moderator
*/

class voidCylVolume : public attachSystem::ContainedComp,
  public attachSystem::FixedComp
{
 private:

  const int voidIndex;            ///< Index of surface offset
  int cellIndex;                  ///< Cell index

  size_t nSegment;                ///< Number of layers
  double xStep;                   ///< X step
  double yStep;                   ///< Y step
  double zStep;                   ///< Z step
  double xyAngle;                 ///< xy rotation angle
  double zAngle;                  ///< z rotation angle

  double radius;                  ///< Radius
  double thick;                   ///< Thickness
  double height;                  ///< Height

  // Functions:

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

  
 public:

  voidCylVolume(const std::string&);
  voidCylVolume(const voidCylVolume&);
  voidCylVolume& operator=(const voidCylVolume&);
  virtual ~voidCylVolume();

  void createAll(Simulation&,const attachSystem::FixedComp&);
  
};

}

#endif
 
