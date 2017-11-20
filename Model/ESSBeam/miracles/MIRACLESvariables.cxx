/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:    ESSBeam/miracles/MIRACLESvariables.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "FocusGenerator.h"
#include "ShieldGenerator.h"
#include "ChopperGenerator.h"
#include "PitGenerator.h"
#include "PipeGenerator.h"
#include "JawGenerator.h"
#include "BladeGenerator.h"
#include "TwinBaseGenerator.h"
#include "TwinGenerator.h"
#include "essVariables.h"


namespace setVariable
{
  
void
MIRACLESvariables(FuncDataBase& Control)
  /*!
    Create all the beamline variables for vor
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("MIRACLESvariables[F]","MIRACLESvariables");

  setVariable::ChopperGenerator CGen;
  setVariable::FocusGenerator FGen;
  setVariable::ShieldGenerator SGen;
  setVariable::PitGenerator PGen;
  setVariable::PipeGenerator PipeGen;
  setVariable::BladeGenerator BGen;
  setVariable::TwinGenerator TGen;

  PipeGen.setPipe(8.0,0.5);
  PipeGen.setWindow(-0.5,0.5);
  PipeGen.setFlange(-1.0,1.0);

  SGen.addWall(1,20.0,"CastIron");
  SGen.addRoof(1,20.0,"CastIron");
  SGen.addFloor(1,20.0,"CastIron");
  SGen.addFloorMat(5,"Concrete");
  SGen.addRoofMat(5,"Concrete");
  SGen.addWallMat(5,"Concrete");

  // extent of beamline
  Control.addVariable("miraclesStopPoint",0);
  Control.addVariable("miraclesAxisXYAngle",0.9); // rotation
  Control.addVariable("miraclesAxisZAngle",0.0);  // rotation
  Control.addVariable("miraclesAxisZStep",0.0);   // offset

  FGen.setLayer(1,0.8,"Copper");
  FGen.setLayer(2,0.3,"Void");  
  FGen.setYOffset(8.0);
  FGen.generateTaper(Control,"miraclesFA",350.0, 6.0,5.0 ,5.0,9.5);
  
  // Pipe in gamma shield
  PipeGen.generatePipe(Control,"miraclesPipeB",8.0,44.0);
  FGen.setLayer(1,0.8,"Aluminium");
  FGen.clearYOffset();
  FGen.generateTaper(Control,"miraclesFB",42.0, 5.0,4.857,  9.5,9.85714);

  // Pipe to collimator:
  PipeGen.generatePipe(Control,"miraclesPipeC",2.0,96.0);
  FGen.setYCentreOffset(-5.0);
  FGen.generateTaper(Control,"miraclesFC",84.0, 4.857,4.0, 9.857,12.0);

  Control.addVariable("miraclesAppAInnerWidth",4.0);
  Control.addVariable("miraclesAppAInnerHeight",4.0);
  Control.addVariable("miraclesAppAWidth",12.0);
  Control.addVariable("miraclesAppAHeight",12.0);
  Control.addVariable("miraclesAppAThick",5.0);
  Control.addVariable("miraclesAppAYStep",7.0);
  Control.addVariable("miraclesAppADefMat","Tungsten");

  TGen.generateChopper(Control,"miraclesTwinB",14.0,16.0,10.0);  

  // Single Blade chopper
  BGen.setThick({0.2});
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.generateBlades(Control,"miraclesBBladeTop",-2.0,22.5,35.0);

  // Single Blade chopper
  BGen.setThick({0.2});
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.generateBlades(Control,"miraclesBBladeLow",2.0,22.5,35.0);

  // Pipe after first chopper unit
  PipeGen.generatePipe(Control,"miraclesPipeD",2.0,34.0);
  FGen.clearYOffset();
  FGen.generateTaper(Control,"miraclesFD",32.0, 5.0,4.857,  9.5,9.85714);

  TGen.generateChopper(Control,"miraclesTwinC",14.0,16.0,10.0);  

  // Single Blade chopper
  BGen.setThick({0.2});
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.generateBlades(Control,"miraclesCBladeTop",-2.0,22.5,35.0);

  // Single Blade chopper
  BGen.setThick({0.2});
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.generateBlades(Control,"miraclesCBladeLow",2.0,22.5,35.0);

  // Pipe after second chopper unit [to 11.5m]
  PipeGen.setRectPipe(16.0,16.0,0.5);
  PipeGen.generatePipe(Control,"miraclesPipeE",2.0,359.0);
  FGen.clearYOffset();
  FGen.generateTaper(Control,"miraclesFE",355.0, 5.0,4.857,  9.5,9.85714);


  CGen.setMainRadius(38.122);
  CGen.setFrame(86.5,86.5);
  CGen.generateChopper(Control,"miraclesChopE",14.0,12.0,6.55);

  // Single Blade chopper
  BGen.setThick({0.2});
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.generateBlades(Control,"miraclesEBlade",0.0,22.5,35.0);


  Control.addVariable("miraclesShutterAYStep",3.0);
  Control.addVariable("miraclesShutterALiftZStep",-10.0);
  Control.addVariable("miraclesShutterALength",30.0);
  Control.addVariable("miraclesShutterAWidth",10.1);
  Control.addVariable("miraclesShutterAHeight",11.1);
  Control.addVariable("miraclesShutterANLayers",1);
  Control.addVariable("miraclesShutterAMat0","Aluminium");
  
  Control.addVariable("miraclesShutterASurroundThick",3.0);
  Control.addVariable("miraclesShutterASurroundMat","Stainless304");
  Control.addVariable("miraclesShutterATopVoid",8.1);

  PipeGen.setRectPipe(16.0,16.0,0.5);
  PipeGen.generatePipe(Control,"miraclesPipeF",2.0,520.0);
  FGen.generateTaper(Control,"miraclesFF",516.0, 5.0,4.857,  9.5,9.85714);

  PipeGen.setRectPipe(16.0,16.0,0.5);
  PipeGen.generatePipe(Control,"miraclesPipeG",2.0,730.0);
  FGen.generateBender(Control,"miraclesBG",724.0, 12.0,12.0,12.0,12.0,
		    500000.0,0.0 );

  
  // BEAM INSERT:
  Control.addVariable("miraclesBInsertHeight",20.0);
  Control.addVariable("miraclesBInsertWidth",28.0);
  Control.addVariable("miraclesBInsertTopWall",1.0);
  Control.addVariable("miraclesBInsertLowWall",1.0);
  Control.addVariable("miraclesBInsertLeftWall",1.0);
  Control.addVariable("miraclesBInsertRightWall",1.0);
  Control.addVariable("miraclesBInsertWallMat","Stainless304");       
  
  // Optional pipe in wall
  PipeGen.generatePipe(Control,"miraclesPipeWall",4.0,348.0);

  FGen.generateBender(Control,"miraclesFWall",344.0, 12.0,12.0,12.0,12.0,
		      500000.0,0.0);

  // OUTER shielding
  SGen.generateShield(Control,"miraclesShieldA",3000.0,40.0,40.0,40.0,4,8);  

  PipeGen.setPipe(10.0,0.5);
  PipeGen.generatePipe(Control,"miraclesPipeOutA",4.0,1495.0);  
  FGen.generateBender(Control,"miraclesBOutA",1491.0,
		      12.0,12.0,12.0,12.0,
		      500000.0,0.0);

  PipeGen.generatePipe(Control,"miraclesPipeOutB",1.0,1498.0);  
  FGen.generateBender(Control,"miraclesBOutB",1494.0,
		      12.0,12.0,12.0,12.0,
		      500000.0,0.0);

  

  return;
}
 
}  // NAMESPACE setVariable
