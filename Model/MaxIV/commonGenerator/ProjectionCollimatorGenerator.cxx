/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/commonGenerator/ProjectionCollimatorGenerator.cxx
 *
 * Copyright (c) 2026 U. Friman-Gayer
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
#include <array>
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

#include "ProjectionCollimatorGenerator.h"

namespace setVariable {

template <std::size_t NPoints, std::size_t NSegments>
ProjectionCollimatorGenerator<NPoints,
                              NSegments>::ProjectionCollimatorGenerator()
    : apertureX{}, apertureY{}, apertureZ{}, height(10.0), width(10.0),
      material("Copper"), voidMaterial("Void") {}

template <std::size_t NPoints, std::size_t NSegments>
void ProjectionCollimatorGenerator<NPoints, NSegments>::generate(
    FuncDataBase &Control, const std::string &keyName) const {

  for (size_t nSection = 0; nSection < NSegments + 1; ++nSection) {
    for (size_t nPoint = 0; nPoint < NPoints; ++nPoint) {
      Control.addVariable(keyName + "X" + std::to_string(nPoint) + "AtY" +
                              std::to_string(nSection),
                          apertureX[nSection][nPoint]);
      if (nSection > 0) {
        Control.addVariable(keyName + "Y" + std::to_string(nSection),
                            apertureY[nSection]);
      }
      Control.addVariable(keyName + "Z" + std::to_string(nPoint) + "AtY" +
                              std::to_string(nSection),
                          apertureZ[nSection][nPoint]);
    }
  }

  Control.addVariable(keyName + "Height", height);
  Control.addVariable(keyName + "Width", width);

  Control.addVariable(keyName + "Material", material);
  Control.addVariable(keyName + "VoidMaterial", voidMaterial);
}

template class ProjectionCollimatorGenerator<4, 2>;

} // NAMESPACE setVariable
