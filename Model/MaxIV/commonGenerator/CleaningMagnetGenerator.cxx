/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonBeam/CleaningMagnetGenerator.cxx
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
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <cmath>
#include <complex>
#include <list>
#include <vector>
#include <stack>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <numeric>
#include <memory>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"

#include "CleaningMagnetGenerator.h"

namespace setVariable
{

CleaningMagnetGenerator::CleaningMagnetGenerator() :
  length(31.5),width(3.0),height(10.0),gap(3.0),
  yokeLength(28.5),
  yokeDepth(14.0),
  yokeThick(5.0),
  mat("Neodymium"),
  yokeMat("Stainless304"), // actually s235jr,
  voidMat("Void")
  /*!
    Constructor and defaults
  */
{}

CleaningMagnetGenerator::~CleaningMagnetGenerator()
 /*!
   Destructor
 */
{}

void
CleaningMagnetGenerator::generate(FuncDataBase& Control,
			       const std::string& keyName) const
/*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables
    \param keyName :: Head name for variable
  */
{
  ELog::RegMethod RegA("CleaningMagnetGenerator","generate");

  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"Width",width);
  Control.addVariable(keyName+"Height",height);
  Control.addVariable(keyName+"Gap",gap);
  Control.addVariable(keyName+"YokeLength",yokeLength);
  Control.addVariable(keyName+"YokeDepth",yokeDepth);
  Control.addVariable(keyName+"YokeThick",yokeThick);
  Control.addVariable(keyName+"Mat",mat);
  Control.addVariable(keyName+"YokeMat",yokeMat);
  Control.addVariable(keyName+"VoidMat",voidMat);

  return;

}


}  // namespace setVariable
