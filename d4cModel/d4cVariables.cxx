/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   d4cModel/d4cVariables.cxx
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
#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>

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
D4CModel(FuncDataBase& Control)
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

// ------------
// Cell stuff
// ------------
  Control.addVariable("cellXStep",0.0);         // Radius the beam
  Control.addVariable("cellYStep",0.0);         // Radius the beam
  Control.addVariable("cellZStep",0.0);         // Radius the beam
  Control.addVariable("cellZangle",0.0);         // Radius the beam
  Control.addVariable("cellXYangle",0.0);         // Radius the beam
  Control.addVariable("cellNLayers",2);         // Radius the beam
  Control.addVariable("cellRadius1",0.7);       //
  Control.addVariable("cellRadius2",1.1);       //
  Control.addVariable("cellHeight1",3.5);       //
  Control.addVariable("cellHeight2",4.5);       //
  Control.addVariable("cellMaterial1","Aluminium");       //
  Control.addVariable("cellMaterial2","B4C");       //


// ------------
// BellJar stuff
// ------------
  Control.addVariable("belljarXStep",0.0);         // Radius the beam
  Control.addVariable("belljarYStep",0.0);         // Radius the beam
  Control.addVariable("belljarZStep",0.0);         // Radius the beam
  Control.addVariable("belljarZangle",0.0);         // Radius the beam
  Control.addVariable("belljarXYangle",0.0);         // Radius the beam
  Control.addVariable("belljarHeight",100.0);         // Radius the beam
  Control.addVariable("belljarRadius",120.0);       //
  Control.addVariable("belljarWallThick",1.0);       //
  Control.addVariable("belljarWallMat",5);       //
  Control.addVariable("belljarColRadius",70.0);       //
  Control.addVariable("belljarColWidth",4.0);       //
  Control.addVariable("belljarColFront",12.0);       //
  Control.addVariable("belljarColBack",12.0);       //
  Control.addVariable("belljarColMat",5);       //
  Control.addVariable("belljarColAngle1",5);       //
  Control.addVariable("belljarColAngle2",25);       //
  Control.addVariable("belljarColAngle3",45);       //
  Control.addVariable("belljarColAngle4",65);       //
  Control.addVariable("belljarColAngle5",85);       //

  // ------------
  // DETECTORS
  // -----------
  Control.addVariable("detectorXStep",0.0);         // Radius the beam
  Control.addVariable("detectorYStep",0.0);         // Radius the beam
  Control.addVariable("detectorZStep",0.0);         // Radius the beam
  Control.addVariable("detectorZangle",0.0);         // Radius the beam
  Control.addVariable("detectorXYangle",0.0);         // Radius the beam
  Control.addVariable("detectorNDetector",100);       //
  Control.addVariable("detectorInitAngle",2.0);       //
  Control.addVariable("detectorFinalAngle",165.0);       //
  Control.addVariable("detectorCentRadius",90.0);       //
  Control.addVariable("detectorTubeRadius",1.0);       //
  Control.addVariable("detectorHeight",20.0);       //
  Control.addVariable("detectorWallThick",0.4);       //
  Control.addVariable("detectorWallMat","Aluminium");       //
  Control.addVariable("detectorDetMat",0);       //

  

  return;
}

}  // NAMESPACE setVariable
