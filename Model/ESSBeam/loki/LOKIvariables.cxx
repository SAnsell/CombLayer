/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   ESSBeam/loki/LOKIvariables.cxx
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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

  PipeGen.setPipe(2.5,2.5);     //  (12.0,0.5); // 12.0 changed to 1.45 -- flanges?
  PipeGen.setWindow(-2.0,0.3);
  PipeGen.setFlange(-4.0,1.0);

  Control.addVariable("lokiAxisXStep",0.0); //TEMP ask Clara
  Control.addVariable("lokiAxisXYAngle",0.0); // not with the bender! starting angle 1 degree from the port in the horizontal rotation
  Control.addVariable("lokiAxisZAngle",0.0);

  FGen.setGuideMat("Aluminium"); //all guides are aluminum according to Damian //used to be copper before
  FGen.setYOffset(0.0);
  FGen.generateBender(Control,"lokiBA",350.0,2.5,2.5,2.5,2.5,6125.0,90.0); //2nd, 3rd param = width of the guide//last parameter - angle for the horizontal (0 degrees) or vertical (90 degrees bender)
  //up or down bending -- -90 or +90, 0.0 = horizontal bending? //6125.0

  //FGen.generateBender(Control,"freiaBA",350.0,4.0,4.0,10.593,17.566,
   //                   7000.0,90.0);

  // Pipe
   PipeGen.generatePipe(Control,"lokiPipeB",0.2,92.15); //52 = 2 cm bigger than the guide
   Control.addVariable("lokiPipeBRadius",5);
   Control.addVariable("lokiPipeBFeThick",0.8);
   // Control.addVariable("lokiPipeBFlangeRadius",4.0);
  //  Control.addVariable("lokiPipeBFlangeLength",1.0);
   Control.addVariable("lokiPipeBMat","Stainless316");
   // Control.addVariable("lokiPipeBVoidMat","Void");



  FGen.setGuideMat("Aluminium");  //the material of LokiBB and all the guides below
  //FGen.clearYOffset();
  FGen.setYOffset(-45.0);
  //straight guide
  FGen.generateTaper(Control,"lokiBB",90.15,2.5,3.0,2.5,3.0);
  //FGen.generateBender(Control,"lokiBB",44.0, 3.0,3.0,3.0,3.0,0.0,0.0);
  Control.addVariable("lokiFMonoNShapes",1);       
  Control.addVariable("lokiFMonoNShapeLayers",1);
  Control.addVariable("lokiFMonoActiveShield",0);
  Control.addVariable("lokiFMonoLayerThick1",1.0);  //material thick


  // //first chopper //chopper = box for chopper, actual chopper is Double Blade, DDisk, etc 
  // CGen.setMainRadius(62.5); // size of housing //was 25.0 //62.5
  // CGen.setFrame(60.0,60.0); //from clara //was 60, 60 80,80 
  // CGen.generateChopper(Control,"lokiChopperA",1.0,15.7,4.55); // 1st = step between chopper and optics, 
  // //2nd = thickness of the chopper unit  // 3rd = thickness of the void  //dimensions from clara, //was (10, , 4.55) 1.0,15.7,5.3); 



  // CGen.setMainRadius(62.5);
  // CGen.setFrame(120.0,120.0); //will leave for now
  // CGen.generateChopper(Control,"lokiChopperA",9.0,15.7,6.0); //3rd was 5.3


  // // Double Blade chopper
  // BGen.setThick({1.5,1.5}); //roughly, from Erik = 2
  // BGen.setGap(0.9); // gap between the blades //from clara 
  // BGen.addPhase({95.0},{120.0});
  // BGen.addPhase({95.0},{120.0});
  // // BGen.addPhase({95},{120.0}); //chopper closed, if -5 or -10 = chopper closed //was ({95,275},{30.0,30.0}); 
  // // BGen.addPhase({95},{120.0}); //was ({95,275},{30.0,30.0}); 
  // BGen.setMaterials("Aluminium", "Aluminium"); //material of inner blade, and outer blade
  // BGen.generateBlades(Control,"lokiDBladeA", 0.0,10.0,22.50); // 0.0,10.0,70.0); //



//parameters adopted from VOR, mixed, TEMP, to be FIXED!!!

  CGen.setMainRadius(62.5);   // diameter 70.0 emali
  CGen.setFrame(160.0,160.0);
  CGen.generateChopper(Control,"lokiChopperA",9.0,15.7,5.3);    

  // Double Blade chopper
  BGen.setMaterials("Aluminium","Aluminium");
  BGen.setThick({2.0,2.0});
  BGen.setGap(1.0);
  BGen.addPhase({-60.0},{120.0});
  BGen.addPhase({-60.0},{120.0});
  BGen.generateBlades(Control,"lokiDBladeA",0.95,10.0,35.0);


  // straight guide between first and second chopper
  PipeGen.setPipe(2.5,2.5);     //  (12.0,0.5); // 12.0 changed to 1.45 -- flanges?
  PipeGen.setWindow(-2.0,0.3);
  PipeGen.setFlange(-4.0,1.0);
  PipeGen.generatePipe(Control,"lokiPipeC",0.1,491.5); //87 = 2 cm longer then the guide
  Control.addVariable("lokiPipeCRadius",5);
  Control.addVariable("lokiPipeCFeThick",0.8);
  Control.addVariable("lokiPipeCMat","Stainless316");




  //   // Pipe
  //  PipeGen.generatePipe(Control,"lokiPipeB",0.1,92.15); //52 = 2 cm bigger than the guide
  //  Control.addVariable("lokiPipeBRadius",3.7);
  //  Control.addVariable("lokiPipeBFeThick",8);
  //  // Control.addVariable("lokiPipeBFlangeRadius",4.0);
  // //  Control.addVariable("lokiPipeBFlangeLength",1.0);
  //  Control.addVariable("lokiPipeBMat","Stainless316");
  //  // Control.addVariable("lokiPipeBVoidMat","Void");





  FGen.setYOffset(-245.0);
  FGen.generateRectangle(Control,"lokiFC",490.0,2.5,3.0);
  Control.addVariable("lokiFCMonoNShapes",1);       
  Control.addVariable("lokiFCMonoNShapeLayers",1);
  Control.addVariable("lokiFCMonoActiveShield",0);
  Control.addVariable("lokiFCMonoLayerThick1",1.0);  //material thick



  //second chopper
  CGen.setMainRadius(25.0);
  CGen.setFrame(60.0,60.0);
  CGen.generateChopper(Control,"lokiChopperAExtra",10.0,10.0,4.55);

  // Two single blades (disks)
  BGen.setThick({0.5});
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.generateBlades(Control,"lokiSBladeAEFirst",0.0,10.0,22.50);

  BGen.setThick({0.5});
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.generateBlades(Control,"lokiSBladeAESecond",0.0,10.0,22.50);


  // Bender D (straight guide between second and third chopper)
  PipeGen.generatePipe(Control,"lokiPipeD",2.0,282.0); //282 = 2 cm longer than the guide
  FGen.generateRectangle(Control,"lokiBD",280.0,2.5,3.0);

  // // Third chopper
  // CGen.setMainRadius(25.0);
  // CGen.setFrame(60.0,60.0);
  // CGen.generateChopper(Control,"lokiChopperB",10.0,10.0,4.55);

  // // Double Blade chopper
  // BGen.setThick({0.5,0.5});
  // BGen.setGap(1.0);
  // BGen.addPhase({95,275},{30.0,30.0});
  // BGen.addPhase({95,275},{30.0,30.0});
  // BGen.generateBlades(Control,"lokiDBladeB",0.0,10.0,22.50);

  // // straight guide between third and fourth chopper
  // PipeGen.generatePipe(Control,"lokiPipeE",6.0,87.0); //87 = 2 cm longer then the guides
  // FGen.clearYOffset();
  // FGen.generateRectangle(Control,"lokiFE",85.0,3.0,3.0);

  // // Fourth chopper
  // CGen.setMainRadius(25.0);
  // CGen.setFrame(60.0,60.0);
  // CGen.generateChopper(Control,"lokiChopperC",10.0,10.0,4.55);

  // // Double Blade chopper
  // BGen.setThick({0.5,0.5});
  // BGen.setGap(1.0);
  // BGen.addPhase({95,275},{30.0,30.0});
  // BGen.addPhase({95,275},{30.0,30.0});
  // BGen.generateBlades(Control,"lokiDBladeC",0.0,10.0,22.50);

  //   // Guide from fourth chopper to the bunker wall
  // PipeGen.generatePipe(Control,"lokiPipeF",6.0,24.0); //52 = 2 cm longer than the guide TEMP!!! 52 cm
  // FGen.generateRectangle(Control,"lokiFF",18.0,3.0,3.0); //TEMP 50 cm



// NEW BEAM INSERT:
  Control.addVariable("lokiCInsertYStep",1);
  Control.addVariable("lokiCInsertNBox",3);
  Control.addVariable("lokiCInsertHeight0",28.4);
  Control.addVariable("lokiCInsertWidth0",27.6);
  Control.addVariable("lokiCInsertHeight1",38.6);
  Control.addVariable("lokiCInsertWidth1",38.0);
  Control.addVariable("lokiCInsertHeight2",28.0);
  Control.addVariable("lokiCInsertWidth2",27.3);
  Control.addVariable("lokiCInsertLength0",125.0);
  Control.addVariable("lokiCInsertLength1",100.0);
  Control.addVariable("lokiCInsertLength2",125.0);
  Control.addVariable("lokiCInsertMat0","Stainless304");
  Control.addVariable("lokiCInsertMat1","Stainless304");
  Control.addVariable("lokiCInsertMat2","Stainless304");

  Control.addVariable("lokiCInsertNWall",0);
  Control.addVariable("lokiCInsertWallThick0",1.0);
  Control.addVariable("lokiCInsertWallMat0","Nickel");
  Control.addVariable("lokiCInsertWallThick1",0.2);
  Control.addVariable("lokiCInsertWallMat1","Void");



// // OLD
//   // BEAM INSERT:
//   Control.addVariable("lokiBInsertHeight",20.0); //defined by the instrument. TEMP!!! Ask Clara
//   Control.addVariable("lokiBInsertWidth",28.0);
//   Control.addVariable("lokiBInsertTopWall",1.0);
//   Control.addVariable("lokiBInsertLowWall",1.0);
//   Control.addVariable("lokiBInsertLeftWall",1.0);
//   Control.addVariable("lokiBInsertRightWall",1.0);
//   Control.addVariable("lokiBInsertWallMat","Stainless304");  


// Guide inside the bunker wall
 FGen.setYOffset(-175.0); //to make it fit the bunker insert. number guessed, how it can be estimated/calculated
 FGen.generateBender(Control,"lokiFFBunker",350.0,2.5,2.5,2.5,2.5,6125.0,-90.0); 
 //TEMP NUMBERS FOR GUIDE LENGTH, OFFSET ETC NEED TO BE CHANGED AND ADJUSTED TO THE OFFICIAL BUNKER WALL LENGTH


  // Pipe around the guide ouside of the bunker and first part of the guide ouside of the bunker
  PipeGen.generatePipe(Control,"lokiPipeFExtra",6.0,20.0); //TEMP!!! Random numbers
  FGen.clearYOffset(); 
  FGen.generateRectangle(Control,"lokiFExtra",18.0,3.0,3.0); //TEMP!!! Random numbers

 //fifth chopper
  CGen.setMainRadius(25.0);
  CGen.setFrame(60.0,60.0);
  CGen.generateChopper(Control,"lokiChopperD",10.0,10.0,4.55);

  // Two single blades (disks)
  BGen.setThick({0.5});
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.generateBlades(Control,"lokiSBladeDFirst",0.0,10.0,22.50);

  BGen.setThick({0.5});
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.generateBlades(Control,"lokiSBladeDSecond",0.0,10.0,22.50);

  //hole in chopper pit D to accomodate the guide
  Control.addVariable("lokiCutGShape","Circle");
  Control.addVariable("lokiCutGRadius",18.0);

     // Chopper pit D, outside of bunker wall

  Control.addVariable("lokiPitDVoidHeight",167.0);
  Control.addVariable("lokiPitDVoidDepth",36.0);
  Control.addVariable("lokiPitDVoidWidth",166.0);
  Control.addVariable("lokiPitDVoidLength",22.0);
  
  Control.addVariable("lokiPitDFeHeight",20.0);
  Control.addVariable("lokiPitDFeDepth",20.0);
  Control.addVariable("lokiPitDFeWidth",15.0);
  Control.addVariable("lokiPitDFeFront",25.0);
  Control.addVariable("lokiPitDFeBack",20.0);
  Control.addVariable("lokiPitDFeMat","Stainless304");

  Control.addVariable("lokiPitDConcHeight",30.0);
  Control.addVariable("lokiPitDConcDepth",30.0);
  Control.addVariable("lokiPitDConcWidth",30.0);
  Control.addVariable("lokiPitDConcFront",30.0);
  Control.addVariable("lokiPitDConcBack",30.0);
  Control.addVariable("lokiPitDConcMat","Concrete");

  Control.addVariable("lokiPitDColletHeight",15.0);
  Control.addVariable("lokiPitDColletDepth",15.0);
  Control.addVariable("lokiPitDColletWidth",40.0);
  Control.addVariable("lokiPitDColletLength",5.0);
  Control.addVariable("lokiPitDColletMat","Tungsten");


 //Shielding around collimator drum
  SGen.setRFLayers(1,8);
  SGen.generateShield(Control,"lokiShieldG",280.0,100.0,100.0,150.0,8,8);

 // straight after fifth chopper inside the collimator drum
  PipeGen.generatePipe(Control,"lokiPipeG",6.0,295.0); ///TEMP random numbers
  FGen.setYOffset(-150.0);
  FGen.generateRectangle(Control,"lokiFG",295.0,3.0,3.0); // TEMP random numbers

  Control.addVariable("lokiCollGYStep",160.0);
  Control.addVariable("lokiCollGLength",270.0);
  Control.addVariable("lokiCollGMat","Copper"); 

  Control.addVariable("lokiAppAInnerWidth",4.0);
  Control.addVariable("lokiAppAInnerHeight",50.0);
  Control.addVariable("lokiAppAWidth",100.0);
  Control.addVariable("lokiAppAHeight",5.0);
  Control.addVariable("lokiAppADepth",5.0);
  Control.addVariable("lokiAppAYStep",5.0); 
  Control.addVariable("lokiAppADefMat","Tungsten");



//Shielding around second collimator drum
  SGen.setRFLayers(1,8);
  SGen.generateShield(Control,"lokiShieldH",280.0,100.0,100.0,200.0,8,8);

 // straight after fifth chopper inside the collimator drum
  PipeGen.generatePipe(Control,"lokiPipeH",1.0,270.0); ///TEMP random numbers
  FGen.setYOffset(-134.0);
  FGen.generateRectangle(Control,"lokiFH",268.0,3.0,3.0); // TEMP random numbers

  Control.addVariable("lokiCollHYStep",134.0);
  Control.addVariable("lokiCollHLength",270.0);
  Control.addVariable("lokiCollHMat","Copper"); 

  Control.addVariable("lokiAppBInnerWidth",4.0);
  Control.addVariable("lokiAppBInnerHeight",50.0);
  Control.addVariable("lokiAppBWidth",60.0);
  Control.addVariable("lokiAppBHeight",5.0);
  Control.addVariable("lokiAppBDepth",5.0);
  Control.addVariable("lokiAppBYStep",5.0); 
  Control.addVariable("lokiAppBDefMat","Tungsten");


  //guide inside the cave
  FGen.setYOffset(0.0);
  FGen.generateRectangle(Control,"lokiFK",150.0,3.0,3.0); //TEMP!!! Random numbers


  // HUT:
  Control.addVariable("lokiCaveYStep",75.0);
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



  //// DetectorTank
  Control.addVariable("lokiTankXStep",0.0);
  Control.addVariable("lokiTankYStep",35.0);
  Control.addVariable("lokiTankZStep",0.0);
  Control.addVariable("lokiTankXYAngle",0.0);
  Control.addVariable("lokiTankZAngle",0.0);

  Control.addVariable("lokiTankNLayers",2.0);
  Control.addVariable("lokiTankInnerRadius",10.0);
  Control.addVariable("lokiTankOuterRadius",300.0);
  Control.addVariable("lokiTankHeight",100.0);
  Control.addVariable("lokiTankMidAngle",-40.0);
  Control.addVariable("lokiTankFrontThick",2.0);
  Control.addVariable("lokiTankInnerThick",2.0);
  Control.addVariable("lokiTankRoofThick",1.0);
  Control.addVariable("lokiTankBackThick",3.0);
  Control.addVariable("lokiTankWallMat","Stainless304");
  //random numbers for the cave below
  Control.addVariable("lokiTankRadius",300.0);
  Control.addVariable("lokiTankLength",200.0);
  Control.addVariable("lokiTankSideThick",3.0);
  Control.addVariable("lokiTankWindowThick",3.0);
  Control.addVariable("lokiTankWindowRadius",300.0);
  Control.addVariable("lokiTankWindowInsetLen",300.0);
  Control.addVariable("lokiTankWindowMat","Stainless304");

  // // Small :: Grid Collimator A:
  // RotGen.setMain(25.0,1.0);
  // RotGen.setWall(1.0,"Aluminium");
  // RotGen.addHole("Circle",   3.0,0.0,    0.0, 15.0);
  // RotGen.addHole("Rectangle",5.0,5.0,  120.0, 15.0);
  // RotGen.addHole("Circle",   4.0,0.0,  240.0, 15.0);
  // RotGen.generatePinHole(Control,"lokiGridA",5.0,-15.0,180.0);
  // RotGen.generatePinHole(Control,"lokiGridB",2.0,-15.0,180.0);
  // RotGen.generatePinHole(Control,"lokiGridC",2.0,-15.0,180.0);
  // RotGen.generatePinHole(Control,"lokiGridD",2.0,-15.0,180.0);

  // RotGen.setMain(30.0,300.0);
  // RotGen.resetHoles();
  // RotGen.addHole("Rectangle",   4.0,4.0,    0.0, 17.50);
  // RotGen.addHole("Rectangle",   6.0,6.0,  120.0, 17.50);
  // RotGen.addHole("Rectangle",   5.0,5.0,  240.0, 17.50);
  // RotGen.generatePinHole(Control,"lokiCollA",1.0,-17.5,180.0);
  // RotGen.generatePinHole(Control,"lokiCollB",5.0,-17.5,180.0);
  // RotGen.generatePinHole(Control,"lokiCollC",5.0,-17.5,180.0);

  // Control.addVariable("lokiCBoxBYStep",2.0);
  // Control.addVariable("lokiCBoxBHeight",80.0);
  // Control.addVariable("lokiCBoxBWidth",80.0);
  // Control.addVariable("lokiCBoxBDepth",12.0);   // THIS is half BYStep 
  // Control.addVariable("lokiCBoxBDefMat","Void");

  // // first guide in loki collimator
  // FGen.generateRectangle(Control,"lokiFCA0",296.0,3.0,3.0);
  
  return;
}

}  // NAMESPACE setVariable
