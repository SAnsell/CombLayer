/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   cuBlock/cuVariables.cxx
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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

namespace setVariable
{

void
CuVariables(FuncDataBase& Control)
  /*!
    Function to set the control variables and constants
    -- This version is for TS1 (real version : non-model)
    \param Control :: Function data base to add constants too
  */
{
// -----------
// GLOBAL stuff
// -----------

  Control.addVariable("zero",0.0);      // Zero
  Control.addVariable("one",1.0);      // one

  // Copper block
  Control.addVariable("CuTargetXStep",0.0);  
  Control.addVariable("CuTargetYStep",0.0);  
  Control.addVariable("CuTargetZStep",0.0);  
  Control.addVariable("CuTargetRadius",5.0);
  Control.addVariable("CuTargetHoleRadius",2.50);
  Control.addVariable("CuTargetCuRadius",8.0);  
  Control.addVariable("CuTargetCuGap",5.0);  

  Control.addVariable("CuTargetCeramicThick",2.0);  
  Control.addVariable("CuTargetSteelThick",1.0);  
  Control.addVariable("CuTargetCuThick",2.0);  

  Control.addVariable("CuTargetCeramicMat",40);  // Ceramics [saphire]
  Control.addVariable("CuTargetSteelMat",3);     // Stainless steel
  Control.addVariable("CuTargetCuMat",73);       // Pure copper

  return;
}

}  // NAMESPACE setVariable
