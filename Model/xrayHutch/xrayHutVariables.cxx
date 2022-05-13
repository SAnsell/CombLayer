/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   xrayHutBuild/xrayHutVariables.cxx
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <iterator>
#include <memory>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "xrayHutVariables.h"

namespace setVariable
{
  
void
xrayHutVariables(FuncDataBase& Control)
  /*!
    Create all the beamline variabes
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("xrayHutVariables[F]","XrayHutVariables");
  
  Control.addVariable("HutLength",350.0);
  Control.addVariable("HutWidth",75.0);
  Control.addVariable("HutHeight",100.0);
  Control.addVariable("HutWallThick",0.4);
  Control.addVariable("HutWallMat","Stainless304");
  
  Control.addVariable("TargetLength",5.0);
  Control.addVariable("TargetWidth",1.0);
  Control.addVariable("TargetHeight",1.0);
  Control.addVariable("TargetMat","Tungsten");


  return;
}

}  // NAMESPACE setVariable
 
