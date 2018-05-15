/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   ESSBeam/loki/LOKIvariables.cxx
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
#include "RotaryHoleGenerator.h"
#include "essVariables.h"

namespace setVariable
{

void
LOKIvariables(FuncDataBase& Control)
  /*!
    Create all the beamline variables for loki
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("LOKIvariables[F]","LOKIvariables");

  setVariable::ChopperGenerator CGen;
  setVariable::FocusGenerator FGen;
  setVariable::ShieldGenerator SGen;
  setVariable::PitGenerator PGen;
  setVariable::PipeGenerator PipeGen;
  setVariable::JawGenerator JawGen;
  setVariable::BladeGenerator BGen;
  setVariable::RotaryHoleGenerator RotGen;
  
  SGen.addWall(1,30.0,"CastIron");
  SGen.addRoof(1,30.0,"CastIron");
  SGen.addFloor(1,30.0,"CastIron");
  SGen.addFloorMat(5,"Concrete");
  SGen.addRoofMat(5,"Concrete");
  SGen.addWallMat(5,"Concrete");

  PipeGen.setPipe(4.5,0.5);
  PipeGen.setWindow(-2.0,0.3);
  PipeGen.setFlange(-4.0,1.0);

  Control.addVariable("lokiStopPoint",0);  
  Control.addVariable("lokiStartPoint",0);  
  Control.addVariable("lokiAxisXStep",0.0);   // TEMP : ask Clara
  Control.addVariable("lokiAxisXYAngle",0.0);   // TEMP : ask Clara
  Control.addVariable("lokiAxisZAngle",0.0);

  FGen.setLayer(1,0.5,"Aluminium");
  FGen.setLayer(2,0.5,"Void");
  FGen.setYOffset(0.0);
  FGen.generateBender(Control,"lokiBA",350.0,3.0,3.0,2.5,2.5,6125.0,90.0);

  Control.addVariable("lokiBlockShutterHeight",66.3);
  Control.addVariable("lokiBlockShutterWidth",38.8);
  Control.addVariable("lokiBlockShutterDepth",40.3);  // cant extend out of shutter region
  Control.addParse<double>("lokiBlockShutterYStep","lokiBlockShutterDepth/2.0");
  Control.addVariable("lokiBlockShutterDefMat","Stainless304");

  // Pipe in gamma shield
  PipeGen.generatePipe(Control,"lokiPipeB",7.0,43.0);
  FGen.setLayer(1,0.5,"Aluminium");
  FGen.clearYOffset();
  FGen.generateBender(Control,"lokiBB",41.0, 3.0,3.0,3.0,3.0,5700.0,0.0);

  // Pipe in gamma shield
  PipeGen.generatePipe(Control,"lokiPipeBLink",2.0,44.0);
  FGen.clearYOffset();
  FGen.generateBender(Control,"lokiBBLink",42.0, 3.0,3.0,3.0,3.0,5700.0,0.0);
  
  CGen.setMainRadius(35.5);   // diameter 70.0 emali
  CGen.setFrame(80.0,80.0);
  CGen.generateChopper(Control,"lokiChopperA",9.5,15.7,5.3);  

  
  // Double Blade chopper
  BGen.setMaterials("Aluminium","Aluminium");
  BGen.setThick({2.0,2.0});
  BGen.setGap(1.0);
  BGen.addPhase({120},{120.0});
  BGen.addPhase({120},{120.0});
  BGen.generateBlades(Control,"lokiDBladeA",0.95,10.0,35.0);


  // Guide from First chopper to Wall
  PipeGen.setPipe(5.0,0.8);  // Rad / thick
  PipeGen.setWindow(-2.0,0.8);  // window offset/ thick
  PipeGen.setFlange(-4.0,1.0);
  PipeGen.generatePipe(Control,"lokiPipeC",1.0,488.0);
  FGen.generateRectangle(Control,"lokiFC",485.0,2.5,3.0);


// NEW BEAM INSERT:
  Control.addVariable("lokiCInsertYStep",0.8);
  Control.addVariable("lokiCInsertNBox", 2); 
  Control.addVariable("lokiCInsertHeight0",17.8);
  Control.addVariable("lokiCInsertWidth0", 23.8);
  Control.addVariable("lokiCInsertHeight1",33.2);
  Control.addVariable("lokiCInsertWidth1",49.2);
  Control.addVariable("lokiCInsertLength0",175); // + 1.5 cm to compensate for mising parts
  Control.addVariable("lokiCInsertLength1",177.6); // + 1.5 cm to compensate for mising parts
  Control.addVariable("lokiCInsertMat0","Stainless304");
  Control.addVariable("lokiCInsertMat1","Stainless304");

  Control.addVariable("lokiCInsertNWall",3);
  Control.addVariable("lokiCInsertWallThick0",0.6);
  Control.addVariable("lokiCInsertWallMat0","Void"); 
  Control.addVariable("lokiCInsertWallThick1",3.0);
  Control.addVariable("lokiCInsertWallMat1","Stainless304"); 
  Control.addVariable("lokiCInsertWallThick2",1.4);
  Control.addVariable("lokiCInsertWallMat2","Void"); 


  FGen.generateBender(Control,"lokiFWall",355.0,2.5,2.5,2.5,2.5,6125.0,-90.0); 
  // Optional pipe in wall
  PipeGen.generatePipe(Control,"lokiPipeWall",4.0,348.0);

  PGen.setFeLayer(20.0);
  PGen.setConcLayer(30.0);
  PGen.generatePit(Control,"lokiOutPitA",0.0, 22.0, 166.0, 167.0,36.0);

  
  //hole in chopper pit D to accomodate the insert
  Control.addVariable("lokiPitACutShape","Square");
  Control.addVariable("lokiPitACutRadius",36.0); //should not be radius

  CGen.setMainRadius(40.5);   // diameter 70.0 emali
  CGen.setFrame(90.0,90.0);
  CGen.generateChopper(Control,"lokiChopperOutA",9.3,15.7,5.3);  

  // Double Blade chopper
  BGen.setMaterials("Aluminium","Aluminium");
  BGen.setThick({2.0,2.0});
  BGen.addPhase({120.0},{120.0});
  BGen.addPhase({120.0},{120.0});
  BGen.generateBlades(Control,"lokiDBladeOutA",0.95,10.0,35.0);

 //Shielding around collimator drum
  SGen.setRFLayers(1,8);
  SGen.generateShield(Control,"lokiShieldA",270.0,100.0,100.0,150.0,8,8);

 // straight after fifth chopper inside the collimator drum
  PipeGen.generatePipe(Control,"lokiPipeOutA",0.5,270.0);
  
  //  FGen.setYOffset(-134.0);
  FGen.clearYOffset();
  FGen.generateRectangle(Control,"lokiFOutA",268.0,2.5,3.0); 

  Control.addVariable("lokiAppAInnerWidth",5.0);
  Control.addVariable("lokiAppAInnerHeight",5.0);
  Control.addVariable("lokiAppAWidth",100.0);
  Control.addVariable("lokiAppAHeight",20.0);
  Control.addVariable("lokiAppAThick",20.0);
  Control.addVariable("lokiAppAYStep",5.0); 
  Control.addVariable("lokiAppADefMat","Tungsten");

  Control.addVariable("lokiCollAYStep",165.35);
  Control.addVariable("lokiCollALength",7.0);
  Control.addVariable("lokiCollAMat","Copper"); 

  //Shielding around second collimator drum
  SGen.setRFLayers(1,8);
  SGen.generateShield(Control,"lokiShieldB",280.0,100.0,100.0,200.0,8,8);


  PipeGen.generatePipe(Control,"lokiPipeOutB",1.0,270.0); 
  FGen.generateRectangle(Control,"lokiFOutB",268.0,2.5,3.0); 
  

  Control.addVariable("lokiCollBYStep",134.0);
  Control.addVariable("lokiCollBLength",7.0);
  Control.addVariable("lokiCollBMat","Copper"); 

  Control.addVariable("lokiAppBInnerWidth",4.0);
  Control.addVariable("lokiAppBInnerHeight",4.0);
  Control.addVariable("lokiAppBWidth",10.0);
  Control.addVariable("lokiAppBHeight",10.0);
  Control.addVariable("lokiAppBThick",5.0);
  Control.addVariable("lokiAppBYStep",5.0); 
  Control.addVariable("lokiAppBDefMat","Tungsten");

  // HUT:
  Control.addVariable("lokiCaveYStep",25.0);
  Control.addVariable("lokiCaveVoidFront",60.0);
  Control.addVariable("lokiCaveVoidHeight",300.0);
  Control.addVariable("lokiCaveVoidDepth",183.0);
  Control.addVariable("lokiCaveVoidWidth",400.0);
  Control.addVariable("lokiCaveVoidLength",1600.0);


  Control.addVariable("lokiCaveFeFront",25.0);
  Control.addVariable("lokiCaveFeLeftWall",15.0);
  Control.addVariable("lokiCaveFeRightWall",15.0);
  Control.addVariable("lokiCaveFeRoof",15.0);
  Control.addVariable("lokiCaveFeFloor",15.0);
  Control.addVariable("lokiCaveFeBack",15.0);

  Control.addVariable("lokiCaveConcFront",35.0);
  Control.addVariable("lokiCaveConcLeftWall",35.0);
  Control.addVariable("lokiCaveConcRightWall",35.0);
  Control.addVariable("lokiCaveConcRoof",35.0);
  Control.addVariable("lokiCaveConcFloor",50.0);
  Control.addVariable("lokiCaveConcBack",35.0);

  Control.addVariable("lokiCaveFeMat","Stainless304");
  Control.addVariable("lokiCaveConcMat","Concrete");

  PipeGen.generatePipe(Control,"lokiPipeOutC",1.0,150.0);
  FGen.generateRectangle(Control,"lokiFOutC",146.0,2.5,3.0);
  
    // Vacumm tank
  Control.addVariable("lokiVTankXStep",0.0);       
  Control.addVariable("lokiVTankYStep",25.0);
  Control.addVariable("lokiVTankZStep",0.0);       
  Control.addVariable("lokiVTankXYAngle",0.0);       
  Control.addVariable("lokiVTankZAngle",0.0);
  
  Control.addVariable("lokiVTankRadius",144.0);
  Control.addVariable("lokiVTankLength",1048.0);
  Control.addVariable("lokiVTankSideThick",5.0);   // NOT CORRECT
  Control.addVariable("lokiVTankBackThick",5.0);   // NOT CORRECT
  Control.addVariable("lokiVTankFrontThick",1.0);  // NOT CORRECT

  Control.addVariable("lokiVTankWindowInsetLen",106.0);  // NOT CORRECT
  Control.addVariable("lokiVTankWindowThick",1.0);  // NOT CORRECT
  Control.addVariable("lokiVTankWindowRadius",8.0);  // NOT CORRECT

  Control.addVariable("lokiVTankWindowMat","SiCrystal");  // NOT CORRECT
  Control.addVariable("lokiVTankWallMat","Stainless304");

  return;
}

}  // NAMESPACE setVariable
