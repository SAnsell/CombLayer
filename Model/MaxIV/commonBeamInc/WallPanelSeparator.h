/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonBeamInc/WallPanelSeparator.h
 *
 * Copyright (c) 2004-2021 by Stuart Ansell and U. Friman-Gayer
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
#ifndef xraySystem_WallPanelSeparator_h
#define xraySystem_WallPanelSeparator_h

class Simulation;

namespace xraySystem {
/*!
  \class WallPanelSeparator
  \version 0.1
  \author U. Friman-Gayer
  \date October 2026
  \brief
*/

class WallPanelSeparator : public attachSystem::ContainedComp,
                           public attachSystem::FixedRotate,
                           public attachSystem::CellMap,
                           public attachSystem::SurfMap,
                           public attachSystem::ExternalCut {
private:
  double channelGapToPlug;
  double channelGapToWall;
  double channelHeight;
  double channelThickness;
  double channelWidth;

  double height;

  double plugGapInside;
  double plugGapOutside;
  double plugOffsetInside;
  double plugThicknessInside;
  double plugThicknessOutside;
  double plugWidthInside;
  double plugWidthOutside;

  int channelMaterial;
  int plugMaterial;
  int voidMaterial;

  void populate(const FuncDataBase &) override;
  void createSurfaces();
  void createObjects(Simulation &);
  void createLinks();

public:
  WallPanelSeparator(const std::string &);
  WallPanelSeparator(const WallPanelSeparator &);
  WallPanelSeparator &operator=(const WallPanelSeparator &);
  ~WallPanelSeparator() override {} ///< Destructor

  using attachSystem::FixedComp::createAll;
  void createAll(Simulation &, const attachSystem::FixedComp &,
                 const long int) override;
};

} // namespace xraySystem

#endif
