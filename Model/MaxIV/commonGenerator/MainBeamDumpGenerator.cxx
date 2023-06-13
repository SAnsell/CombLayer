/*********************************************************************
  CombLayer : MCNP(X) Input builder

  * File:   commonGenerator/MainBeamDumpGenerator.cxx
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
#include "CFFlanges.h"

#include "MainBeamDumpGenerator.h"

namespace setVariable
{
  MainBeamDumpGenerator::MainBeamDumpGenerator() :
    width(50.0),length(100.0),height(25.0),depth(25.0),
    wallThick(10.0),portLength(30.0),
    portRadius(2.0),
    mat("Stainless304L"),
    wallMat("Poly")
    /*!
      Constructor and defaults
    */
  {}

  MainBeamDumpGenerator::~MainBeamDumpGenerator()
  /*!
    Destructor
  */
  {}


  void
  MainBeamDumpGenerator::generate(FuncDataBase& Control,
				  const std::string& keyName)  const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables
    \param keyName :: head name for variable
    \param yStep :: Step along beam centre
  */
  {
    ELog::RegMethod RegA("MainBeamDumpGenerator","generateMainBeamDump");

    Control.addVariable(keyName+"Width",width);
    Control.addVariable(keyName+"Length",length);
    Control.addVariable(keyName+"Height",height);
    Control.addVariable(keyName+"Depth",depth);
    Control.addVariable(keyName+"WallThick",wallThick);
    Control.addVariable(keyName+"PortLength",portLength);
  Control.addVariable(keyName+"PortRadius",portRadius);
  Control.addVariable(keyName+"Mat",mat);
  Control.addVariable(keyName+"WallMat",wallMat);

    return;

  }


}  // NAMESPACE setVariable
