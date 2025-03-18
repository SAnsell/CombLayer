/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/commonBeamInc/ProximityShielding.h
 *
 * Copyright (c) 2004-2025 by Konstantin Batkov
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
#ifndef xraySystem_ProximityShielding_h
#define xraySystem_ProximityShielding_h

class Simulation;

namespace xraySystem
{

/*!
  \class ProximityShielding
  \version 1.0
  \author Konstantin Batkov
  \date March 2025
  \brief Proximity shielding
*/

class ProximityShielding :
    public attachSystem::ContainedComp,
    public attachSystem::FixedRotate,
    public attachSystem::CellMap,
    public attachSystem::SurfMap,
    public attachSystem::FrontBackCut
{
 private:

  double length;                ///< Total length including void
  double width;                 ///< Width
  double height;                ///< Height
  double boreRadius;            ///< Pipe penetration radius

  int voidMat;                  ///< Void material
  int wallMat;                  ///< Wall material

  void populate(const FuncDataBase&);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  ProximityShielding(const std::string&);
  ProximityShielding(const ProximityShielding&);
  ProximityShielding& operator=(const ProximityShielding&);
  virtual ProximityShielding* clone() const;
  virtual ~ProximityShielding();

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,const long int);

};

}

#endif
