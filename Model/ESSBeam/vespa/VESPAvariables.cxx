/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:    ESSBeam/vespa/VESPAvariables.cxx
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>. 
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
#include "essVariables.h"
#include "LightShutterGenerator.h"
#include "HeavyShutterGenerator.h"
#include "ShieldGenerator.h"
#include "FocusGenerator.h"
#include "TwinBaseGenerator.h"
#include "TwinFlatGenerator.h"
#include "ChopperGenerator.h"
#include "PitGenerator.h"
#include "PipeGenerator.h"
#include "RectPipeGenerator.h"
#include "JawGenerator.h"
#include "BladeGenerator.h"
#include "CryoGenerator.h"

namespace setVariable
{

// horizontal / vertical focus section is given by:
// y = (x - 1150)*(3.5-5.0)/(5850-1150) + 5
double y (double x)   //! there must be a more elegant way to code this
{
  return -(3*x/9400) + (1009/188);
}

void
VESPAvariables(FuncDataBase& Control)
  /*!
    Create all the beamline variables for vor
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("VESPAvariables[F]","VESPAvariables");
  
  double time(0); // seconds
  double PSCFrequency(154); // Hz
  double FOCFrequency(28);  // Hz
  double T0Frequency(0);    // Hz
  double sFOCFrequency(42); // Hz
  
  setVariable::LightShutterGenerator LSGen;
  setVariable::TwinFlatGenerator TFGen;
  setVariable::RectPipeGenerator RPGen;
  setVariable::ChopperGenerator CGen;
  setVariable::CryoGenerator CryGen;
  setVariable::FocusGenerator FGen;
  setVariable::HeavyShutterGenerator HSGen;
  setVariable::ShieldGenerator SGen;
  setVariable::PitGenerator PGen;
  setVariable::PipeGenerator PipeGen;
  setVariable::BladeGenerator BGen;
  setVariable::JawGenerator JawGen;
  
  Control.addVariable("vespaStartPoint",0);
  Control.addVariable("vespaStopPoint",0);
  Control.addVariable("vespaAxisXStep",1.523);
  Control.addVariable("vespaAxisZStep",0.0);
//  Control.addVariable("vespaAxisXYAngle",0.9);  // clockwise around Z axis
//  Control.addVariable("vespaAxisZAngle",0.9);  // clockwise around Y axis
  
  PipeGen.setPipe(8.0,0.5);
  PipeGen.setWindow(-2.0,0.5); 
  PipeGen.setFlange(-4.0,1.0);
  PipeGen.setCladding(0.5,"B4C");
  
  RPGen.setFlange(76.0,24.0,1.0);
  RPGen.setPipe(70.0,16.0,0.5);
  
  TFGen.setMotorLength(15.0);
  
  SGen.addWall(1,20.0,"CastIron");
  SGen.addRoof(1,20.0,"CastIron");
  SGen.addFloor(1,20.0,"CastIron");
  SGen.addFloorMat(3,"Concrete");
  SGen.addRoofMat(3,"Concrete");
  SGen.addWallMat(3,"Concrete");
  
  // FocusA > Monolith guide
  FGen.setLayer(1,0.5,"Copper");
  FGen.setLayer(2,0.5,"Void");
  FGen.setYOffset(2.0);
  FGen.generateTaper(Control,"vespaFA",344.5,3.5,4.0,3.5,4.0);

  FGen.clearYOffset();
  
  // LShutter > Light shutter
//  LSGen.setOpenPercentage(4.0,1.0);
  LSGen.generateLightShutter(Control,"vespaLShutter",6.796);
  Control.addVariable("vespaLShutterZStep",50.0);
  
  // VPipeLS + FocusLS > Pipe inside light shutter
  PipeGen.generatePipe(Control,"vespaPipeLS",6.796,50.0);
  FGen.generateRectangle(Control,"vespaFLS",48.0,4.0,4.0);
  Control.addVariable("vespaPipeLSZStep",0.0);
  
  // VPipeB + FocusB > Pipe from light shutter to the triple chopper-system (PSCs) vacuum box
  PipeGen.generatePipe(Control,"vespaPipeB",57.796,31.5);
  FGen.generateRectangle(Control,"vespaFB",29.5,4.0,4.0);
  FGen.setLayer(1,0.5,"Aluminium");

  
  // JPipeAIn > First element of the common vacuum box (before TwinChopperA)
  RPGen.generatePipe(Control,"vespaJoinPipeAIn",2.0,14.0);
  FGen.generateRectangle(Control,"vespaFC",12.0,4.0,4.0);
  
  // Accomodate bigger disk (diameter: 80 cm)
//  TFGen.setMainRadius(48);  //39.122
//  TFGen.setInnerRadius(44); //36.0
//  fixes needed JS!
  
  // TwinChopperA > First Twin Blade Chopper (PSC-A)
  TFGen.generateChopper(Control,"vespaTwinChopperA",9.0,16.0,10.0);
  BGen.setThick({0.2});
//  BGen.addPhase(time,PSCFrequency,{0.0,122.3,242.1},{24.3,40.8,57.0});
  BGen.addPhase(time,PSCFrequency,{0.0},{360});
  BGen.generateBlades(Control,"vespaPSCTopBladeA",-2.0,26.0,35.0);
  BGen.generateBlades(Control,"vespaPSCBottomBladeA",2.0,26.0,35.0);
  
  // JPipeAB > Intermediate common vacuum box element between TwinChopperA and TwinChopperB
  RPGen.generatePipe(Control,"vespaJoinPipeAB",0.0,14.0);
  FGen.generateRectangle(Control,"vespaFD",12.0,4.0,4.0);
  
  // TwinChopperB > Second Twin Blade Chopper (PSC-B)
  TFGen.setReverseMotors(1,1);
  TFGen.generateChopper(Control,"vespaTwinChopperB",9.0,16.0,10.0);
  BGen.setThick({0.2});
//  BGen.addPhase(time,PSCFrequency,{0.0,138.8,274.8},{24.3,40.8,57.0});
  BGen.addPhase(time,PSCFrequency,{0.0},{360});
  BGen.generateBlades(Control,"vespaPSCTopBladeB",2.0,26.0,35.0);
  BGen.generateBlades(Control,"vespaPSCBottomBladeB",-2.0,26.0,35.0);
  
  // JPipeBC - Intermediate common vacuum box element between TwinChopperB and TwinChopperC
  RPGen.generatePipe(Control,"vespaJoinPipeBC",0.0,48.0);
  FGen.generateRectangle(Control,"vespaFE",46.0,4.0,4.0);
  
  // TwinChopperC > Third Twin Blade Chopper (PSC-C)
  TFGen.generateChopper(Control,"vespaTwinChopperC",9.0,16.0,10.0);
  BGen.setThick({0.2});
//  BGen.addPhase(time,PSCFrequency,{0.0,138.8,274.8},{24.3,40.8,57.0});
  BGen.addPhase(time,PSCFrequency,{0.0},{360});
  BGen.generateBlades(Control,"vespaPSCTopBladeC",2.0,26.0,35.0);
  BGen.generateBlades(Control,"vespaPSCBottomBladeC",-2.0,26.0,35.0);
  
  // JPipeCOut - Last common vacuum box element (after TwinChopperB)
  RPGen.generatePipe(Control,"vespaJoinPipeCOut",0.0,14.0);
  FGen.generateRectangle(Control,"vespaFF",12.0,4.0,4.0);
  
  
  // VPipeG + FocusG > Pipe&guide between the triple chopper system and the frame overlap chopper (ChopperFOC)
  PipeGen.generatePipe(Control,"vespaPipeG",2.0,224.464);
  FGen.generateTaper(Control,"vespaFG",222.464,4.0,4.56,4.0,4.56); //JS
  
  
  // ChopperFOC > Frame Overlap Chopper (FOC)
  CGen.setReverseMotor(1);
  CGen.setMotorRadius(10.0);
  CGen.setMotorLength(15.0);
  CGen.generateChopper(Control,"vespaChopperFOC",8.0,12.0,6.0);
  BGen.setThick({0.2});
//  BGen.addPhase(time,FOCFrequency,{0.0},{88.0});
  BGen.addPhase(time,FOCFrequency,{0.0},{360});
  BGen.generateBlades(Control,"vespaFOCBlade",0.0,20.0,35.0);
  
  
  // VPipeH + FocusH > Pipe&guide to heavy shutter
  PipeGen.generatePipe(Control,"vespaPipeH",1.0,50.24004); //44.24004
  FGen.generateTaper(Control,"vespaFH",48.24004,4.6,4.7,4.6,4.7);

  
  // HShutter > Heavy Shutter
  HSGen.setWallThick(0);
  HSGen.setSeparatorThick(2);
  HSGen.setSlabThick({10,10,10,30,10});
  HSGen.generateHeavyShutter(Control,"vespaHShutter",HSGen.getLength()/2+2,14.0,18.0);
  Control.addVariable("vespaHShutterZStep",0.0);
  
  
  // VPipeHS + FocusHS > Pipe replacing heavy shutter
  PipeGen.generatePipe(Control,"vespaPipeHS",1.0,HSGen.getLength());
  FGen.generateTaper(Control,"vespaFHS",HSGen.getLength()-2,4.0,5.0,4.0,5.0);
  
  
  // BEAM INSERT:
  Control.addVariable("vespaBInsertHeight",20.0);
  Control.addVariable("vespaBInsertWidth",28.0);
  Control.addVariable("vespaBInsertTopWall",1.0);
  Control.addVariable("vespaBInsertLowWall",1.0);
  Control.addVariable("vespaBInsertLeftWall",1.0);
  Control.addVariable("vespaBInsertRightWall",1.0);
  Control.addVariable("vespaBInsertWallMat","Stainless304");
  
  
  // VPipeWall + FocusWall > Optional Pipe in Bunker Wall
  PipeGen.generatePipe(Control,"vespaPipeWall",4.0,348.0);
  FGen.clearYOffset();
  FGen.generateRectangle(Control,"vespaFWall",348.0,5.0,5.0);
  
  
  // ChopperT0Pit > T0 Chopper Pit
  PGen.setFeLayer(15.0);
  PGen.setConcLayer(40.0);
  PGen.generatePit(Control,"vespaChopperT0Pit",0.0,28.0,120.0,88.0,40.0);
  
  // ChopperT0 > T0 Chopper
  CGen.setMainRadius(33.0);
  CGen.setFrame(80.0,80.0);
  CGen.setMotorLength(15.0);
  CGen.generateChopper(Control,"vespaChopperT0",15.0,15.0,9.55);
  BGen.setThick({3.4});
  BGen.setInnerThick({5.4});
  BGen.setMaterials("Inconnel","Tungsten");
//  BGen.addPhase(time,T0Frequency,{95.0,275.0},{30.0,30.0});
//  BGen.addPhase(time,T0Frequency,{-15.0,165.0},{30.0,30.0});  // chopper open
  BGen.addPhase(time,T0Frequency,{0.0},{360});
  BGen.addPhase(time,T0Frequency,{0.0},{360});
  BGen.generateBlades(Control,"vespaT0Blade",2.0,20.0,30.0);
  
  Control.addVariable("vespaT0ExitPortShape","Circle");
  Control.addVariable("vespaT0ExitPortRadius",5.0);
  
  
  // ShieldA > First triangular shielding
  SGen.setAngle(3.0,3.0);
  SGen.setEndWall(20.0,20.0);
  SGen.generateTriShield(Control,"vespaShieldA",339.26,80.0,80.0,80.0,1,9);
  
  // VPipeI + FocusI > Pipe&guide inside the first triangular shielding
  PipeGen.setPipe(9.0,0.5);
  PipeGen.generatePipe(Control,"vespaPipeI",58.0,441.76);
  FGen.clearYOffset();
<<<<<<< HEAD
  FGen.generateTaper(Control,"vespaFI",439.76,y(1556.99998),y(1556.99998+439.76),y(1556.99998),y(1556.99998+439.76));
  
  
  // ChoppersFOCPit > sFOC Chopper Pit
  PGen.setFeLayer(15.0);
  PGen.setConcLayer(40.0);
  PGen.generatePit(Control,"vespaChoppersFOCPit",484.76,40.0,130.0,110.0,40.0);
  
  // ChoppersFOC + SFOCDisk > Sub Frame Overlap Chopper (sFOC) and its disk
=======
  FGen.generateTaper(Control,"vespaFOutA",380.0,9.0,11.0,8.5,10.0);

  Control.addVariable("vespaT0ExitPortShape","Circle");
  Control.addVariable("vespaT0ExitPortRadius",5.0);

  CGen.setReverseMotor(1);
>>>>>>> 1d8fa07b2ef58bb768c63c44f40d9542a34aec1b
  CGen.setMainRadius(38.0);
  CGen.setFrame(110.0,110.0);
  CGen.setMotorRadius(10.0);
  CGen.setMotorLength(15.0);
  CGen.generateChopper(Control,"vespaChoppersFOC",14.0,12.0,5.55);
  BGen.setThick({0.2,0.2});
  BGen.setGap(3.0);
<<<<<<< HEAD
//  BGen.addPhase(time,sFOCFrequency,{0.0},{320.0});
//  BGen.addPhase(time,sFOCFrequency,{30.0},{320.0});
  BGen.addPhase(time,sFOCFrequency,{0.0},{360});
  BGen.addPhase(time,sFOCFrequency,{0.0},{360});
  BGen.generateBlades(Control,"vespasFOCBlade",0.0,25.0,35.5);
  
  
  // VPipeJ + FocusJ > Pipe&guide from chopper FOC to next pipe&guide
  PipeGen.generatePipe(Control,"vespaPipeJ",0.0,748.5);
  Control.addVariable("vespaPipeJRadius",9.0);
  FGen.generateTaper(Control,"vespaFJ",746.5,y(2038.75998),y(2038.75998+746.5),y(2038.75998),y(2038.75998+746.5));
  
  // ShieldB > Second triangular shielding
=======
  BGen.addPhase({0.0},{320.0});
  BGen.addPhase({30.0},{320.0});
  BGen.generateBlades(Control,"vespaFOCBladeB",0.0,25.0,35.5);

  // Guide after wall [17.5m - 3.20] for wall
  PipeGen.generatePipe(Control,"vespaPipeOutB",2.0,765.0);  //
  Control.addVariable("vespaPipeOutBRadius",9.0);
  FGen.setYOffset(2.0);
  FGen.generateTaper(Control,"vespaFOutB",750.0,9.0,11.0,8.5,10.0);

>>>>>>> 1d8fa07b2ef58bb768c63c44f40d9542a34aec1b
  SGen.clearLayers();
  SGen.addWall(1,20.0,"CastIron");
  SGen.addRoof(1,20.0,"CastIron");
  SGen.addFloor(1,20.0,"CastIron");
  SGen.addFloorMat(3,"Concrete");
  SGen.addRoofMat(3,"Concrete");
  SGen.addWallMat(3,"Concrete");
  SGen.generateTriShield(Control,"vespaShieldB",770.0,60.0,60.0,60.0,1,8);
  
  
  // ShieldArray[0-3] > 4 triangular shieldings
  const double slen(595.0);
  for(size_t i=0;i<4;i++)
  {
    const std::string shieldName("vespaShieldArray"+StrFunc::makeString(i));
    const std::string vacName("vespaVPipeArray"+StrFunc::makeString(i));
    const std::string focusName("vespaFocusArray"+StrFunc::makeString(i));
    
    SGen.generateTriShield(Control,shieldName,slen,40.0,40.0,40.0,1,8);
    PipeGen.generatePipe(Control,vacName,0.5,slen-1);
    FGen.clearYOffset();
    FGen.generateTaper(Control,focusName,slen-3.0,y(2038.75998+(slen-1)*static_cast<double>(i)),
                                                  y(2038.75998+(slen-1)*static_cast<double>(i+1)),
                                                  y(2038.75998+(slen-1)*static_cast<double>(i)),
                                                  y(2038.75998+(slen-1)*static_cast<double>(i+1)));
  }
  
  // ShieldC > Third triangular shielding
  SGen.generateShield(Control,"vespaShieldC",593.9,40.0,40.0,40.0,1,8);
  
  
  // VPipeK + FocusK > Pipe&guide from last triangular shielding to the cave
  PipeGen.generatePipe(Control,"vespaPipeK",0.5,616.9);
  Control.addVariable("vespaPipeKRadius",9.0);
  FGen.clearYOffset();
  FGen.generateTaper(Control,"vespaFK",614.9,y(2038.75998+(594.0)*static_cast<double>(4)),
                                             y(2038.75998+(594.0)*static_cast<double>(4)+616.9),
                                             y(2038.75998+(594.0)*static_cast<double>(4)),
                                             y(2038.75998+(594.0)*static_cast<double>(4)+616.9));
  
  // Cave
  Control.addVariable("vespaCaveYStep",25.0);
  Control.addVariable("vespaCaveXStep",0.0);
  Control.addVariable("vespaCaveVoidHeight",270.0);
  Control.addVariable("vespaCaveVoidDepth",225.0);
  Control.addVariable("vespaCaveVoidWidth",450.0);
  Control.addVariable("vespaCaveVoidLength",450.0);
  
  Control.addVariable("vespaCaveFeFront",25.0);
  Control.addVariable("vespaCaveFeLeftWall",15.0);
  Control.addVariable("vespaCaveFeRightWall",15.0);
  Control.addVariable("vespaCaveFeRoof",15.0);
  Control.addVariable("vespaCaveFeFloor",15.0);
  Control.addVariable("vespaCaveFeBack",15.0);
  
  Control.addVariable("vespaCaveConcFront",35.0);
  Control.addVariable("vespaCaveConcLeftWall",35.0);
  Control.addVariable("vespaCaveConcRightWall",35.0);
  Control.addVariable("vespaCaveConcRoof",35.0);
  Control.addVariable("vespaCaveConcFloor",50.0);
  Control.addVariable("vespaCaveConcBack",35.0);
  
  Control.addVariable("vespaCaveFeMat","Stainless304");
  Control.addVariable("vespaCaveConcMat","Concrete");
  
  
  // INNER Comp:
  Control.addVariable("vespaInnerVoidHeight",100.0);
  Control.addVariable("vespaInnerVoidDepth",105.0);
  Control.addVariable("vespaInnerVoidWidth",100.0);
  Control.addVariable("vespaInnerVoidLength",120.0);
  Control.addVariable("vespaInnerBackAngle",45.0);
  Control.addVariable("vespaInnerBackCutStep",30.0);
  
  Control.addVariable("vespaInnerFeFront",5.0);
  Control.addVariable("vespaInnerFeLeftWall",10.0);
  Control.addVariable("vespaInnerFeRightWall",10.0);
  Control.addVariable("vespaInnerFeRoof",10.0);
  Control.addVariable("vespaInnerFeFloor",10.0);
  Control.addVariable("vespaInnerConcFront",10.0);
  Control.addVariable("vespaInnerConcLeftWall",10.0);
  Control.addVariable("vespaInnerConcRightWall",10.0);
  Control.addVariable("vespaInnerConcRoof",10.0);
  Control.addVariable("vespaInnerConcFloor",50.0);
  
  Control.addVariable("vespaInnerFeMat","Stainless304");
  Control.addVariable("vespaInnerConcMat","Concrete");
  
  
  // Beam port through front of inner cave
  Control.addVariable("vespaInnerExitShape","Circle");
  Control.addVariable("vespaInnerExitRadius",10.0);
  
  // Jaws > Vertical jaws
  JawGen.generateJaws(Control,"vespaVJaws",75.0);
  
  // Sample
  Control.addVariable("vespaSampleYStep",0.0);
  Control.addVariable("vespaSampleNLayers",2);
  Control.addVariable("vespaSampleRadius1",1.0);
  Control.addVariable("vespaSampleRadius2",1.5);
  Control.addVariable("vespaSampleHeight1",5.0);
  Control.addVariable("vespaSampleHeight2",6.0);
  Control.addVariable("vespaSampleMaterial1","H2O");
  Control.addVariable("vespaSampleMaterial2","Aluminium");
  Control.addVariable("vespaSampleTemp2",20.0);
  
  // Detectors
  Control.addVariable("vespaNDet",32);
  
  Control.addVariable("vespaXStalWidth",12.0);
  Control.addVariable("vespaXStalThick",1.0);
  Control.addVariable("vespaXStalLength",10.0);
  Control.addVariable("vespaXStalGap",0.3);
  Control.addVariable("vespaXStalWallThick",0.5);
  Control.addVariable("vespaXStalBaseThick",0.5);
  
  Control.addVariable("vespaXStalXtalMat","Silicon300K");
  Control.addVariable("vespaXStalWallMat","Aluminium");
  
  Control.addVariable("vespaDBoxNDetectors",20);
  Control.addVariable("vespaDBoxCentRadius",0.635);
  Control.addVariable("vespaDBoxTubeRadius",0.425);
  Control.addVariable("vespaDBoxWallThick",0.1);
  Control.addVariable("vespaDBoxHeight",20.0);
  Control.addVariable("vespaDBoxWallMat","Aluminium");
  Control.addVariable("vespaDBoxDetMat","He3_10Bar");
  Control.addVariable("vespaXStal5Active",0);
  Control.addVariable("vespaXStal6Active",0);
  Control.addVariable("vespaXStal7Active",0);
  Control.addVariable("vespaXStal21Active",0);
  Control.addVariable("vespaXStal22Active",0);
  Control.addVariable("vespaXStal23Active",0);
  
  Control.addVariable("vespaDBox5Active",0);
  Control.addVariable("vespaDBox6Active",0);
  Control.addVariable("vespaDBox7Active",0);
  
  Control.addVariable("vespaDBox21Active",0);
  Control.addVariable("vespaDBox22Active",0);
  Control.addVariable("vespaDBox23Active",0);
  
  const double braggAngle[]={-50,-30,-50,-30};
  const double braggStep[]={32.63,64.157,32.63,64.157};
  const double detStep[]={24.5,30.0,24.5,30.0};
  
  int detCnt(0);
  for(size_t i=0;i<4;i++)
  {
    double aZ=0.0;
    const double aZStep=45.0;
    for(size_t j=0;j<8;j++)
    {
      const std::string xKey="vespaXStal"+StrFunc::makeString(detCnt);
      const std::string dKey="vespaDBox"+StrFunc::makeString(detCnt);
      Control.addVariable(xKey+"YStep",braggStep[i]);
      Control.addVariable(xKey+"PreXYAngle",braggAngle[i]);
      Control.addVariable(xKey+"PreZAngle",0.0);
      Control.addVariable(xKey+"XYAngle",-braggAngle[i]);
      Control.addVariable(xKey+"ZAngle",0.0);
      Control.addVariable(dKey+"YStep",detStep[i]);
      Control.addVariable(dKey+"XYAngle",braggAngle[i]);
      Control.addVariable(xKey+"YRotation",aZ);
      Control.addVariable(xKey+"ZRotation",(i>1 ? 180.0 : 0.0));
      aZ+=aZStep;
      detCnt++;
    }
  }
  
  // Cryo > Cryostat
  CryGen.generateFridge(Control,"vespaCryo",150.0,-10,4.5);
  return;
}
 
}  // NAMESPACE setVariable
