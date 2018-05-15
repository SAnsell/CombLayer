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

  SGen.addWall(1,20.0,"Steel71");
  SGen.addRoof(1,20.0,"Steel71");
  SGen.addFloor(1,20.0,"Steel71");
  SGen.addFloorMat(5,"Concrete");
  SGen.addRoofMat(5,"Concrete");
  SGen.addWallMat(5,"Concrete");

  // extent of beamline
  Control.addVariable("dreamStopPoint",0);
  Control.addVariable("dreamAxisXYAngle",0.0);   // rotation
  Control.addVariable("dreamAxisZAngle",0.0);   // rotation 

  FGen.setLayer(1,0.8,"Copper");
  FGen.setLayer(2,0.5,"Void");
  FGen.setYOffset(0.0);
  FGen.generateTaper(Control,"dreamFA",350.0, 7.630,2.96, 4.0,4.0);

  PipeGen.setWindow(-2.0,0);
  PipeGen.setMat("Copper");
  PipeGen.generatePipe(Control,"dreamPipeB",6.5,26.0);
  FGen.clearYOffset();
    //  FGen.generateTaper(Control,"dreamFB",50.0,2.88,2.11,3.81,3.95);   
  FGen.generateTaper(Control,"dreamFB",26.0,2.63,2.14,3.75,3.76);   
  
  // VACBOX A : 6.10m target centre
  //  Length 100.7 + Width [87.0] + Height [39.0] void Depth/2 + front
  CGen.setMainRadius(38.122);
  CGen.setFrame(86.5,86.5);
  CGen.generateChopper(Control,"dreamChopperA",32.5,12.0,6.55);

  // Double Blade chopper
  BGen.setThick({0.2,0.2});
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.generateBlades(Control,"dreamDBlade",-2.0,22.5,33.5);

  BGen.setThick({0.2});
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.generateBlades(Control,"dreamSBlade",1.0,28.0,33.0);
   
    // VACUUM PIPE: SDisk to T0 (A)
  PipeGen.setMat("Aluminium");
  PipeGen.generatePipe(Control,"dreamPipeC0",0.5,28.5);
  PipeGen.setWindow(-2.0,0.5);
  PipeGen.generatePipe(Control,"dreamPipeC",0.5,320.0);

  FGen.setLayer(1,0.8,"Aluminium");
  FGen.setLayer(2,0.5,"Void");
  FGen.clearYOffset();
  FGen.generateTaper(Control,"dreamFC",318.0,1.19,3.53,4.06,4.74);   

  Control.addVariable("dreamCollimAYStep",26.0);
  Control.addVariable("dreamCollimALength",50.0);
  Control.addVariable("dreamCollimAMat","Copper"); 

  // Band chopoper
  CGen.setMainRadius(33.00);
  CGen.setFrame(83.5,83.5);
  CGen.generateChopper(Control,"dreamChopperB",7.5,14.0,6.0);

  BGen.setMaterials("Inconnel","Void");
  BGen.setThick({0.5});
  BGen.addPhase({95,275},{35.0,25.0});
  BGen.generateBlades(Control,"dreamBandADisk",0.5,22.0,30.0);

  PipeGen.generatePipe(Control,"dreamPipeD",0.5,77.5);
  FGen.generateTaper(Control,"dreamFD",75.5,3.6,3.90,4.77,4.9); 

  // T0 chopper:
  CGen.generateChopper(Control,"dreamChopperC",19.5,38.0,34.0);
  // T0 Chopper disk A
  BGen.setMaterials("Inconnel","Void");
  BGen.setThick({30.0});
  //  BGen.setInnerThick({5.4});
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.generateBlades(Control,"dreamT0DiskA",0.0,20.0,30.0);

  PipeGen.generatePipe(Control,"dreamPipeE1",0.5,536.0);
  FGen.generateTaper(Control,"dreamFE1",534.0,4.05,5.32,4.96,5.57);
  Control.addVariable("dreamCollimBYStep",310.5);
  Control.addVariable("dreamCollimBLength",50.0);
  Control.addVariable("dreamCollimBMat","Copper"); 

  PipeGen.generatePipe(Control,"dreamPipeE2",0.5,536.0);
  FGen.generateTaper(Control,"dreamFE2",534.0,5.32,5.82,5.58,5.84);

  Control.addVariable("dreamCollimCYStep",26.0);
  Control.addVariable("dreamCollimCLength",50.0);
  Control.addVariable("dreamCollimCMat","Copper"); 

  //Heavy Shutter
  PipeGen.generatePipe(Control,"dreamPipeF",0.5,200.0);
  FGen.generateTaper(Control,"dreamFF",198.0,5.82,5.86,5.84,5.86); 

  PipeGen.generatePipe(Control,"dreamPipeG",0.5,62.5);
  FGen.generateRectangle(Control,"dreamFG",60.5,5.86,5.86); 

  // BEAM INSERT:
  Control.addVariable("dreamBInsertAYStep",2.0);
  Control.addVariable("dreamBInsertANBox",1);
  Control.addVariable("dreamBInsertAHeight",18.0);
  Control.addVariable("dreamBInsertAWidth",18.0);
  Control.addVariable("dreamBInsertALength",188.5);
  Control.addVariable("dreamBInsertAMat","Stainless304");
  Control.addVariable("dreamBInsertANWall",1);
  Control.addVariable("dreamBInsertAWallThick",2.85);
  Control.addVariable("dreamBInsertAWallMat","Stainless304");       

  Control.addVariable("dreamBInsertBNBox",1);
  Control.addVariable("dreamBInsertBHeight",32.0);
  Control.addVariable("dreamBInsertBWidth",30.0);
  Control.addVariable("dreamBInsertBLength",191.5);
  Control.addVariable("dreamBInsertBMat","Stainless304");
  Control.addVariable("dreamBInsertBNWall",1);
  Control.addVariable("dreamBInsertBWallThick",3.85);
  Control.addVariable("dreamBInsertBWallMat","Stainless304"); 

  // Guides in wall
  FGen.setYOffset(2.0);
  FGen.generateRectangle(Control,"dreamFWallA",380.0,5.86,5.86);  

  SGen.setRFLayers(3,8);
  SGen.generateShield(Control,"dreamShieldA",1335.0,40.0,40.0,40.0,8,8);
  Control.addVariable("dreamShieldAYStep",171.5);

  // Guide Section 2 (after wall) [+17.6m]  
  PipeGen.setPipe(6.0,0.5);
  PipeGen.generatePipe(Control,"dreamPipeOutA",0.5,1314.5);  //
  FGen.clearYOffset();
  FGen.setLayer(1,1.0,"Borosilicate");
  FGen.setLayer(2,0.5,"Void");
  FGen.generateRectangle(Control,"dreamFOutA",1312.5,5.86,5.86); // end of second part

  // Guide Section 3 [+17.6m]
  SGen.generateShield(Control,"dreamShieldB",1760.0,32.0,32.0,32.0,8,8);  
  PipeGen.generatePipe(Control,"dreamPipeOutB",0.0,1760.0);
  FGen.generateRectangle(Control,"dreamFOutB",1758.0,5.86,5.86); // end of third part

  // Part of Guide Section 4 [+11.85m]
  SGen.generateShield(Control,"dreamShieldC",1185.0,32.0,32.0,32.0,8,8);  
  PipeGen.generatePipe(Control,"dreamPipeOutC",0.0,1185);
  FGen.generateOctagon(Control,"dreamFOutC",1183.0,5.86,4.34); 

  PipeGen.setWindowMat("Void"); 
  PipeGen.setWindow(-2.0,0.0);
  // Part of Guide Section 4 inside Cave
  PipeGen.generatePipe(Control,"dreamPipeCaveA",0.0,516.36);
  FGen.generateOctagon(Control,"dreamFCaveA",516.36,4.33,1.5);

  // Boron Nose
  FGen.setLayer(1,1.0,"Boron");
  FGen.generateOctagon(Control,"dreamFCaveB",27.3,1.8,1.1);
  Control.addVariable("dreamFCaveBYStep",0.0);

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
