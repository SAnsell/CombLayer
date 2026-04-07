/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/CM2Crystal.h
 *
 * Copyright (c) 2026 by U. Friman-Gayer
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
#ifndef xraySystem_CM2Crystal_h
#define xraySystem_CM2Crystal_h

class Simulation;

namespace xraySystem
{
/*!
  \class CM2Crystal
  \author U. Friman-Gayer
  \version 0.1
  \date April 2026
  \brief Crystal in CM2 for catching and realigning the beam from CM1.

  This class implements the crystal and its copper holder.

  References:
  [1] JJ X-RAY, SINCRYS beamline - MAXIV, Final Design Report v2, 23087, 2025-07-11
  [2] CAD model CM2_chamber_2026-03-04.STEP /mxn/groups/rad/Beamlines/DanMAX/Simulations/CM2_chamber_2026-03-04.STEP
*/
class CM2Crystal :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedComp,
  public attachSystem::FrontBackCut,
  public attachSystem::SurfMap,
  public attachSystem::CellMap
{
 private:

  // Holder and outer-surface dimensions

  double height;
  double length;
  double width;

  // Upper part of the holder

  double topHeight;
  double topHeightAboveOpticalAxis;
  double topWidth;

  // Crystal

  double crystalHeight;
  double crystalPitch;
  double crystalPitChannelDepth;
  double crystalPitChannelWidth;
  double crystalPitDepth;
  double crystalPitHeight;
  double crystalPitWidth;
  double crystalRoll;
  double crystalThick;
  double crystalWidth;
  double crystalYaw;

  // Materials

  int holderMaterial;
  int crystalMaterial;

 protected:
  void populate(const FuncDataBase&) override;
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  
 public:

  CM2Crystal(const std::string&);

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
			 const long int) override;
};

}

#endif
 
