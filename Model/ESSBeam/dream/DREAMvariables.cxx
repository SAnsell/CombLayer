/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:    ESSBeam/dream/DREAMvariables.cxx
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
#include "essVariables.h"


namespace setVariable
{

    
void
DREAMvariables(FuncDataBase& Control)
  /*!
    Create all the beamline variables for vor
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("DREAMvariables[F]","DREAMvariables");

  setVariable::ChopperGenerator CGen;
  setVariable::FocusGenerator FGen;
  setVariable::ShieldGenerator SGen;
  setVariable::PitGenerator PGen;
  setVariable::PipeGenerator PipeGen;
  setVariable::BladeGenerator BGen;

  PipeGen.setPipe(8.0,0.5);
  PipeGen.setWindow(-2.0,0.5);
  PipeGen.setFlange(-4.0,1.0);

  SGen.addWall(1,20.0,"CastIron");
  SGen.addRoof(1,20.0,"CastIron");
  SGen.addFloor(1,20.0,"CastIron");
  SGen.addFloorMat(5,"Concrete");
  SGen.addRoofMat(5,"Concrete");
  SGen.addWallMat(5,"Concrete");

  // extent of beamline
  Control.addVariable("dreamStopPoint",0);
  Control.addVariable("dreamAxisXYAngle",0.0);   // rotation
  Control.addVariable("dreamAxisZAngle",0.0);   // rotation 

  FGen.setGuideMat("Copper");
  FGen.setYOffset(0.0);
  FGen.generateTaper(Control,"dreamFA",350.0,8.0,2.0,2.5,4.5);
  
  PipeGen.generatePipe(Control,"dreamPipeB",8.0,40.0);
  FGen.clearYOffset();
  FGen.generateTaper(Control,"dreamFB",38.0,2.5,2.5,4.5,4.5);   
  //  Control.addVariable("dreamFBBeamYStep",4.0);
 
  // VACBOX A : 6.10m target centre
  //  Length 100.7 + Width [87.0] + Height [39.0] void Depth/2 + front
  CGen.setMainRadius(38.122);
  CGen.setFrame(86.5,86.5);
  CGen.generateChopper(Control,"dreamChopperA",55.0,12.0,6.55);

  // Double Blade chopper
  BGen.setThick({0.2,0.2});
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.generateBlades(Control,"dreamDBlade",-2.0,22.5,33.5);

  BGen.setThick({0.2});
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.generateBlades(Control,"dreamSBlade",1.0,28.0,33.0);
   
    // VACUUM PIPE: SDisk to T0 (A)
  PipeGen.generatePipe(Control,"dreamPipeC",2.0,260.0);

  FGen.clearYOffset();
  FGen.generateTaper(Control,"dreamFC",252.0,2.06,2.36,4.6,4.5);   
  //  Control.addVariable("dreamFCBeamYStep",1.10); 

  // Band chopoper
  CGen.generateChopper(Control,"dreamChopperB",8.0,14.0,6.0);

  BGen.setMaterials("Inconnel","Void");
  BGen.setThick({0.5});
  BGen.addPhase({95,275},{35.0,25.0});
  BGen.generateBlades(Control,"dreamBandADisk",0.5,22.0,30.0);

  // T0 chopper:
  CGen.generateChopper(Control,"dreamChopperC",20.0,36.0,32.0);
  // T0 Chopper disk A/B
  BGen.setMaterials("Inconnel","Void");
  BGen.setThick({5.0});
  BGen.setInnerThick({5.4});
  BGen.addPhase({95,275},{30.0,30.0});

  BGen.generateBlades(Control,"dreamT0DiskA",-12.0,20.0,30.0);
  BGen.generateBlades(Control,"dreamT0DiskB",12.0,20.0,30.0);  

  // TMid guide  [Expected to be removed]
  FGen.setThickness(0.4,1.5);
  FGen.clearYOffset();  
  FGen.generateTaper(Control,"dreamFT0Mid",16.0,2.05,2.36,4.6,4.5);
  
  // VACUUM PIPE: SDisk to T0 (A)
  PipeGen.generatePipe(Control,"dreamPipeD",2.0,430.0);

  
  FGen.generateTaper(Control,"dreamFD",422.0,2.06,2.36,4.6,4.5); 

  CGen.generateChopper(Control,"dreamChopperD",10.0,9.0,3.55);

  BGen.setMaterials("Inconnel","Void");
  BGen.setThick({0.5});
  BGen.addPhase({95,275},{35.0,25.0});

  BGen.generateBlades(Control,"dreamBandBDisk",0.5,22.0,30.0);
  
  CGen.generateChopper(Control,"dreamChopperE",20.0,36.0,32.0);


    // T0 chopper:
  CGen.generateChopper(Control,"dreamChopperE",20.0,36.0,32.0);
  // T0 Chopper disk A/B
  BGen.setMaterials("Inconnel","Void");
  BGen.setThick({5.0});
  BGen.setInnerThick({5.4});
  BGen.addPhase({95,275},{125.0,125.0});

  BGen.generateBlades(Control,"dreamT1DiskA",-12.0,20.0,30.0);
  BGen.generateBlades(Control,"dreamT1DiskB",12.0,20.0,30.0);  

  // TMid guide  [Expected to be removed]
  FGen.setThickness(0.4,1.5);
  FGen.clearYOffset();  
  FGen.generateTaper(Control,"dreamFT1Mid",16.0,2.05,2.36,4.6,4.5);


  // VACUUM PIPE: SDisk to T0 (A)
  PipeGen.generatePipe(Control,"dreamPipeE",2.0,500.0);
  FGen.generateTaper(Control,"dreamFE",492.0,2.06,2.36,4.6,4.5); 

  // VACUUM PIPE: SDisk to T0 (A)
  PipeGen.generatePipe(Control,"dreamPipeF",2.0,500.0);
  FGen.generateTaper(Control,"dreamFF",492.0,2.06,2.36,4.6,4.5); 


  // VACUUM PIPE: SDisk to T0 (A)
  PipeGen.generatePipe(Control,"dreamPipeG",2.0,800.0);
  FGen.generateTaper(Control,"dreamFG",796.0,2.06,2.36,4.6,4.5);

  // VACUUM PIPE: SDisk to T0 (A)
  PipeGen.generatePipe(Control,"dreamPipeH",2.0,600.0);
  FGen.generateTaper(Control,"dreamFH",566.0,2.06,2.36,4.6,4.5);

  // BEAM INSERT:
  Control.addVariable("dreamBInsertHeight",20.0);
  Control.addVariable("dreamBInsertWidth",28.0);
  Control.addVariable("dreamBInsertTopWall",1.0);
  Control.addVariable("dreamBInsertLowWall",1.0);
  Control.addVariable("dreamBInsertLeftWall",1.0);
  Control.addVariable("dreamBInsertRightWall",1.0);
  Control.addVariable("dreamBInsertWallMat","Stainless304");       

  // Guide in wall
  FGen.clearYOffset();  
  FGen.generateTaper(Control,"dreamFWall",346.0,6.0,6.0,6.0,6.0);

  SGen.setRFLayers(3,8);
  SGen.generateShield(Control,"dreamShieldA",1430.0,40.0,40.0,40.0,8,8);

  // Guide after wall [17.5m - 3.20] for wall

  PipeGen.setPipe(6.0,0.5);
  PipeGen.generatePipe(Control,"dreamPipeOutA",2.5,1422.0);  //

  FGen.clearYOffset();
  FGen.generateTaper(Control,"dreamFOutA",1418.0,4.5,4.97,2.24,3.05);

  // Guide after wall [+17.5m] after section 1  
  PipeGen.generatePipe(Control,"dreamPipeOutB",2.0,1750.0);

  FGen.setGuideMat("Glass");
  FGen.generateTaper(Control,"dreamFOutB",2836,4.0,4.0,5.0,5.0);

  SGen.generateShield(Control,"dreamShieldB",2850.0,32.0,32.0,32.0,8,8);  

  // Guide after wall [+17.5m] after section 1  
  PipeGen.generatePipe(Control,"dreamPipeCaveA",0.0,500.0);
  FGen.generateTaper(Control,"dreamFCaveA",496,4.0,4.0,5.0,5.0);

  // HUT:
  Control.addVariable("dreamCaveYStep",0.0);
  Control.addVariable("dreamCaveVoidFront",60.0);
  Control.addVariable("dreamCaveVoidHeight",300.0);
  Control.addVariable("dreamCaveVoidDepth",183.0);
  Control.addVariable("dreamCaveVoidWidth",400.0);
  Control.addVariable("dreamCaveVoidLength",1600.0);


  Control.addVariable("dreamCaveFeFront",25.0);
  Control.addVariable("dreamCaveFeLeftWall",15.0);
  Control.addVariable("dreamCaveFeRightWall",15.0);
  Control.addVariable("dreamCaveFeRoof",15.0);
  Control.addVariable("dreamCaveFeFloor",15.0);
  Control.addVariable("dreamCaveFeBack",15.0);

  Control.addVariable("dreamCaveConcFront",35.0);
  Control.addVariable("dreamCaveConcLeftWall",35.0);
  Control.addVariable("dreamCaveConcRightWall",35.0);
  Control.addVariable("dreamCaveConcRoof",35.0);
  Control.addVariable("dreamCaveConcFloor",50.0);
  Control.addVariable("dreamCaveConcBack",35.0);

  Control.addVariable("dreamCaveFeMat","Stainless304");
  Control.addVariable("dreamCaveConcMat","Concrete");

  
  return;
}
 
}  // NAMESPACE setVariable
