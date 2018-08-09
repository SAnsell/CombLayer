/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   R1Ring/R1RingVariables.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "variableSetup.h"
#include "maxivVariables.h"

#include "CFFlanges.h"

namespace setVariable
{

namespace R1RingVar
{
 
}  // NAMESPACE R1RingVar
  
void
R1RingVariables(FuncDataBase& Control)
  /*!
    Function to set the control variables and constants
    -- This version is for Photon Moderator
    \param Control :: Function data base to add constants too
  */
{
  ELog::RegMethod RegA("R1RingVariables[F]","R1RingVariables");

  const std::string preName("R1Ring");
  Control.addVariable(preName+"HexRadius",1000.0);
  Control.addVariable(preName+"HexWallThick",60.0);
  Control.addVariable(preName+"Height",160.0);
  Control.addVariable(preName+"Depth",180.0);
  Control.addVariable(preName+"RoofThick",60.0);
  Control.addVariable(preName+"FloorThick",100.0);

  Control.addVariable(preName+"WallMat","Concrete");  
  Control.addVariable(preName+"FloorMat","Concrete");
  Control.addVariable(preName+"RoofMat","Concrete");

  return;
}

}  // NAMESPACE setVariable
