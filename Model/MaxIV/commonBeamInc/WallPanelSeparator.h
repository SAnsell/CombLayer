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
  \brief Separator between panels of a straight wall in Caratelli design

  Long shielding walls in the Caratelli design typically consist of multiple
  panels. To avoid radiation leakage from the gaps between the panels, a piece
  with a letter-T-shaped cross section -here referred to as a "plug"- is
  inserted.

  This class implements the T-shaped plug and a pair of so-called C channels
  that are located at the inside of a straight (separators between angled panels
  or between hutches may have a different design) wall. While the main shielding
  function is fulfilled by the plug alone, the channels may significantly
  contribute as well. Several variables are provided to model imperfections like
  gaps and offsets in the separator.

  An overview of all variables of the plug is shown in the top-view drawing
  below:

  <--------------PlugWidthOutside ------------------>
  ---------------------------------------------------  ^
  |                 PLUG OUTSIDE                    |  | PlugThicknessOutside
  ---------------------------------------------------  v
                      |     |                          ^
                      |  P  |                          | PlugGapOutside
                      |  L  |                          v
  -----------------   |  U  |   ------------------------
        WALL      |   |  G  |   |       WALL
  -----------------   |     |   ------------------------
  PlugGapInside    <->|  I  |<-> PlugGapInside     ^
  - PlugGapOffset     |  N  |    + PlugGapOffset   |
                      |  S  |                      |
                      |  I  |                      | PlugWidthInside
                      |  D  |                      |
       "Left"         |  E  |         "Right"      |
                      |     |                      |
                      -------                      v
                      <-----> PlugThicknessInside

  The drawing above also shows the definition of "left" and "right" used in this
  class. The channels provide the variables shown in the top-view drawing below:

  -----------------   |  P  |   ------------------------
        WALL      |   |  L  |   |       WALL
  -----------------   |  U  |   ------------------------
                      |  G  |      ChannelHeight      ^
                      |     |   <-------------------> |  ChannelGapToWall
                      |     |    ____________________ v
                ^     |     |   |                   | ^  ChannelThickness
                |     |     |   |   ________________| v
                |     |     |   |   |
  ChannelWidth  |     |     |   |   |
                |     |     |   |   |
                |     |     |   |   |
                |     |     |   |   |________________
                |     |     |   |                   |
                v     |     |   |___________________|
                             <-> ChannelGapToPlug

  The only parameter that is not shown in the drawings is the height.

  In order to place the separator, "innerWall" and "outerWall" rules
  must be provided as cut surfaces. An optional "roof" rule can also be supplied
  to attach the separator to the inside of a roof surface.

  The general design of the wall-panel separators was taken from Refs. [1] and
  [2] for the DanMAX beamline at MAX IV.

  References
  ----------
  [1] Caratelli, MAX IV DanMAX, 06769-00-000-C.pdf
  [2] CAD Model for DanMAX Hutches, DANMAX 2017-12-15.STEP
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

  HeadRule roofHR;

  void populate(const FuncDataBase &) override;
  void createSurfaces();
  void createObjects(Simulation &);
  void createLinks();

public:
  WallPanelSeparator(const std::string &);
  ~WallPanelSeparator() override {}

  using attachSystem::FixedComp::createAll;
  void createAll(Simulation &, const attachSystem::FixedComp &,
                 const long int) override;
};

} // namespace xraySystem

#endif
