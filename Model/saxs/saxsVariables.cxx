/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   saxsModel/saxsVariables.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
SAXSModel(FuncDataBase& Control)
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

  Control.addVariable("sdefEnergy",3000.0);
// ------------
// Cell stuff
// ------------
  
  // wall thickness 10 micron! 
  Control.addVariable("SampleCapRadius",(0.17-0.002)/2.0);    // Radius (outer-wall)
  Control.addVariable("SampleCapWallThick",0.001);       
  Control.addVariable("SampleCapInnerMat","H2O");
  Control.addVariable("SampleCapWallMat","SiO2");       
  Control.addVariable("SampleCapLength",10.0);

  // wall thickness 10 micron! 
  Control.addVariable("WaterCapZStep",-1.0);    // Radius (outer-wall)
  Control.addVariable("WaterCapRadius",(0.17-0.002)/2.0);    // Radius (outer-wall)
  Control.addVariable("WaterCapWallThick",0.001);       
  Control.addVariable("WaterCapInnerMat","H2O");
  Control.addVariable("WaterCapWallMat","SiO2");       
  Control.addVariable("WaterCapLength",10.0);

  // wall thickness 10 micron!
  Control.addVariable("EnergyCapZStep",-1.0);    // Radius (outer-wall)
  Control.addVariable("EnergyCapRadius",(0.17-0.002)/2.0);    // Radius (outer-wall)
  Control.addVariable("EnergyCapWallThick",0.001);       
  Control.addVariable("EnergyCapInnerMat","H2O");
  Control.addVariable("EnergyCapWallMat","Void");       
  Control.addVariable("EnergyCapLength",10.0);



// ------------
// BellJar stuff
// ------------
  Control.addVariable("belljarXStep",0.0);         // Radius the beam
  Control.addVariable("belljarYStep",0.0);         // Radius the beam
  Control.addVariable("belljarZStep",0.0);         // Radius the beam
  Control.addVariable("belljarZAngle",0.0);         // Radius the beam
  Control.addVariable("belljarXYAngle",0.0);         // Radius the beam
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
  Control.addVariable("detectorZAngle",0.0);         // Radius the beam
  Control.addVariable("detectorXYAngle",0.0);         // Radius the beam
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

  Control.addVariable("AreaPlateYStep",100.0);       //
  Control.addVariable("AreaPlateNWBin",40);       // 
  Control.addVariable("AreaPlateNHBin",40);       //
  Control.addVariable("AreaPlateWidth",40.0);     // 10 cm (real)
  Control.addVariable("AreaPlateHeight",40.0);    // 10 cm (real)
  Control.addVariable("AreaPlateThick",0.5);       //

  Control.addVariable("AreaPlateMainMat","Void");       //

  return;
}

}  // NAMESPACE setVariable
