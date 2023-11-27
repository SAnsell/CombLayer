/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/GunTestFacilityInc/RFGun.h
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
#ifndef xraySystem_RFGun_h
#define xraySystem_RFGun_h

class Simulation;

namespace xraySystem
{

/*!
  \class RFGun
  \version 1.0
  \author Konstantin Batkov
  \date October 2023
  \brief RF gun
*/

class RFGun :
    public attachSystem::ContainedComp,
    public attachSystem::FixedRotate,
    public attachSystem::CellMap,
    public attachSystem::SurfMap,
    public attachSystem::FrontBackCut
{
 private:

  double length;                ///< Total length including void
  double cavityRadius;          ///< Cavity inner radius
  double cavityLength;          ///< Cavity length
  double cavitySideWallThick;   ///< Main cavity side wall thickness
  double cavityOffset;          ///< Main cavity offset
  double wallThick;             ///< Wall thickness
  double frontFlangeThick;      ///< Front flange thickness

  int mainMat;                  ///< Main material
  int wallMat;                  ///< Wall material

  void populate(const FuncDataBase&);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  RFGun(const std::string&);
  RFGun(const RFGun&);
  RFGun& operator=(const RFGun&);
  virtual RFGun* clone() const;
  virtual ~RFGun();

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,const long int);

};

}

#endif