/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/commonGenerator/WallPanelSeparatorGenerator.cxx
 *
 * Copyright (c) 2026 S. Ansell and U. Friman-Gayer
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
#include <cmath>
#include <fstream>
#include <list>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "CFFlanges.h"

#include "WallPanelSeparatorGenerator.h"

namespace setVariable {

WallPanelSeparatorGenerator::WallPanelSeparatorGenerator()
    : channelGapToPlug(0.05), channelGapToWall(0.05), channelHeight(5.0),
      channelThickness(0.6), channelWidth(10.0), height(300.0),
      plugGapInside(0.05), plugGapOutside(0.05), plugOffsetInside(0.0),
      plugThicknessInside(0.8), plugThicknessOutside(0.8), plugWidthInside(8.5),
      plugWidthOutside(9.0), channelMaterial("SteelUnknownGrade"),
      plugMaterial("Lead"), voidMaterial("Void") {}

void WallPanelSeparatorGenerator::generate(FuncDataBase &Control,
                                           const std::string &keyName) const {

  Control.addVariable(keyName + "ChannelGapToPlug", channelGapToPlug);
  Control.addVariable(keyName + "ChannelGapToWall", channelGapToWall);
  Control.addVariable(keyName + "ChannelHeight", channelHeight);
  Control.addVariable(keyName + "ChannelThickness", channelThickness);
  Control.addVariable(keyName + "ChannelWidth", channelWidth);

  Control.addVariable(keyName + "Height", height);

  Control.addVariable(keyName + "PlugGapInside", plugGapInside);
  Control.addVariable(keyName + "PlugGapOutside", plugGapOutside);
  Control.addVariable(keyName + "PlugOffsetInside", plugOffsetInside);
  Control.addVariable(keyName + "PlugThicknessInside", plugThicknessInside);
  Control.addVariable(keyName + "PlugThicknessOutside", plugThicknessOutside);
  Control.addVariable(keyName + "PlugWidthInside", plugWidthInside);
  Control.addVariable(keyName + "PlugWidthOutside", plugWidthOutside);

  Control.addVariable(keyName + "ChannelMaterial", channelMaterial);
  Control.addVariable(keyName + "PlugMaterial", plugMaterial);
  Control.addVariable(keyName + "VoidMaterial", voidMaterial);
}

} // NAMESPACE setVariable
