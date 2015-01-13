/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   d4cModel/d4cVariables.cxx
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
  Control.addVariable("cellRadius1",0.25);       //
  Control.addVariable("cellRadius2",0.325);       //
  Control.addVariable("cellHeight1",3.5);       //
  Control.addVariable("cellHeight2",4.5);       //
  Control.addVariable("cellMaterial1","Li7ClD2O6Mol");       //
  Control.addVariable("cellMaterial2","TiZr");       //


// ------------
// BellJar stuff
// ------------
  Control.addVariable("belljarXStep",0.0);         // Radius the beam
  Control.addVariable("belljarYStep",0.0);         // Radius the beam
  Control.addVariable("belljarZStep",0.0);         // Radius the beam
  Control.addVariable("belljarZangle",0.0);         // Radius the beam
  Control.addVariable("belljarXYangle",0.0);         // Radius the beam
  Control.addVariable("belljarHeight",100.0);         // Radius the beam
  Control.addVariable("belljarRadius",145.0);       //
  Control.addVariable("belljarWallThick",1.0);       //
  Control.addVariable("belljarWallMat","Aluminium");       //
  Control.addVariable("belljarColRadius",30.0);      // BEGIN OF CELL
  Control.addVariable("belljarColWidth",3.96);       // Width a max
  Control.addVariable("belljarColFront",35.0);       // Length of front
  Control.addVariable("belljarColBack",2.0);        // Back cut
  Control.addVariable("belljarSubColWidth",1.2);    // Width a max
  Control.addVariable("belljarSubColFront",65.0);    // Length of front
  Control.addVariable("belljarSubColBack",2.0);      // Back cut
  Control.addVariable("belljarColMat","B4C");       //
  Control.addVariable("belljarColAngle1",0.0);       //
  Control.addVariable("belljarColAngle2",15.0);       //
  Control.addVariable("belljarColAngle3",30.0);       //
  Control.addVariable("belljarColAngle4",45.0);       //
  Control.addVariable("belljarColAngle5",60.0);       //
  Control.addVariable("belljarColAngle6",75.0);       //
  Control.addVariable("belljarColAngle7",90.0);       //
  Control.addVariable("belljarColAngle8",105.0);       //
  Control.addVariable("belljarColAngle9",120.0);       //
  Control.addVariable("belljarColAngle10",135.0);       //

  // ------------
  // DETECTORS ARRAY [not used currently]
  // -----------
  Control.addVariable("detectorXStep",0.0);         // Radius the beam
  Control.addVariable("detectorYStep",0.0);         // Radius the beam
  Control.addVariable("detectorZStep",0.0);         // Radius the beam
  Control.addVariable("detectorZangle",0.0);         // Radius the beam
  Control.addVariable("detectorXYangle",0.0);         // Radius the beam
  Control.addVariable("detectorNDetector",100);       //
  Control.addVariable("detectorInitAngle",2.0);       //
  Control.addVariable("detectorFinalAngle",165.0);       //
  Control.addVariable("detectorCentRadius",114.6);       //
  Control.addVariable("detectorTubeRadius",1.0);       //
  Control.addVariable("detectorHeight",20.0);       //
  Control.addVariable("detectorWallThick",0.2);       //
  Control.addVariable("detectorWallMat","Aluminium");       //
  Control.addVariable("detectorDetMat",0);       //  

  // ------------
  // DETECTORS
  // -----------
  Control.addVariable("DetBankXStep",0.0);         // Radius the beam
  Control.addVariable("DetBankYStep",0.0);         // Radius the beam
  Control.addVariable("DetBankZStep",0.0);         // Radius the beam
  Control.addVariable("DetBankZangle",0.0);         // Radius the beam
  Control.addVariable("DetBankXYangle",0.0);         // Radius the beam
  Control.addVariable("DetBankNDet",8);       //
  Control.addVariable("DetBankCentreOffset",114.6);       //
  Control.addVariable("DetBankDetHeight",10.0);       //
  Control.addVariable("DetBankDetDepth",3.0);       //
  Control.addVariable("DetBankDetLength",15.9);       
  Control.addVariable("DetBankWallThick",0.2);       //
  Control.addVariable("DetBankWallMat","Aluminium");       //
  Control.addVariable("DetBankDetMat","Void");       //  

  Control.addVariable("DetBank0CentreAngle",7.5);       //
  Control.addVariable("DetBank1CentreAngle",22.5);       //
  Control.addVariable("DetBank2CentreAngle",37.5);       //
  Control.addVariable("DetBank3CentreAngle",52.5);       //
  Control.addVariable("DetBank4CentreAngle",67.5);       //
  Control.addVariable("DetBank5CentreAngle",82.5);       //
  Control.addVariable("DetBank6CentreAngle",97.5);       //
  Control.addVariable("DetBank7CentreAngle",112.5);       //
  Control.addVariable("DetBank8CentreAngle",127.5);       //

  return;
}

}  // NAMESPACE setVariable
