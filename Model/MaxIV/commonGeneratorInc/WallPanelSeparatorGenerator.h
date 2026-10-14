/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/commonGeneratorInc/WallPanelSeparatorGenerator.h
 *
 * Copyright (c) 2026 by S. Ansell and U. Friman-Gayer
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
#ifndef setVariable_WallPanelSeparatorGenerator_h
#define setVariable_WallPanelSeparatorGenerator_h

class FuncDataBase;

namespace setVariable {

/*!
  \class WallPanelSeparatorGenerator
  \version 0.1
  \author U. Friman-Gayer
  \date October 2026
  \brief WallPanelSeparatorGenerator for variables
*/

class WallPanelSeparatorGenerator {
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

  std::string channelMaterial;
  std::string plugMaterial;
  std::string voidMaterial;

public:
  WallPanelSeparatorGenerator();
  ~WallPanelSeparatorGenerator() = default;

  void generate(FuncDataBase &, const std::string &) const;
};

} // namespace setVariable

#endif
