/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:    ESSBeam/vor/VORvariables.cxx
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
#include "BladeGenerator.h"
#include "essVariables.h"

namespace setVariable
{

void
VORvariables(FuncDataBase& Control)
  /*!
    Create all the beamline variables for vor
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("VORvariables[F]","VORvariables");

  setVariable::ChopperGenerator CGen;
  setVariable::FocusGenerator FGen;
  setVariable::ShieldGenerator SGen;
  setVariable::PitGenerator PGen;
  setVariable::PipeGenerator PipeGen;
  setVariable::BladeGenerator BGen;

  Control.addVariable("vorStopPoint",0);
  Control.addVariable("vorStartPoint",0);
  Control.addVariable("vorAxisXYAngle",0.0);   // rotation
  Control.addVariable("vorAxisZAngle",0.0);   // rotation 

  
  SGen.addWall(1,15.0,"CastIron");
  SGen.addRoof(1,15.0,"CastIron");
  SGen.addFloor(1,15.0,"CastIron");
  SGen.addFloorMat(5,"Concrete");
  SGen.addRoofMat(5,"Concrete");
  SGen.addWallMat(5,"Concrete");

  FGen.setLayer(1,0.5,"Copper");
  FGen.setLayer(2,0.5,"Void");
  FGen.setYOffset(2.0);

  PipeGen.setPipe(12.0,0.5);
  PipeGen.setWindow(-2.0,0.3);
  PipeGen.setFlange(-4.0,1.0);

    
  
  //  Control.addVariable("vorGABeamXYAngle",1.0);
  FGen.setYOffset(.20);
  FGen.generateTaper(Control,"vorFA",350.0,2.114,3.2417,3.16,3.9228);

  // VACUUM PIPE in Gamma shield
  PipeGen.generatePipe(Control,"vorPipeB",8.0,46.0);
  FGen.setLayer(1,0.5,"Aluminium");
  FGen.clearYOffset();
  FGen.generateTaper(Control,"vorFB",44.0,3.30,3.4028,4.0,4.2);

  // VACUUM PIPE in Gamma shield
  PipeGen.generatePipe(Control,"vorPipeC",2.0,326.0);
  FGen.generateTaper(Control,"vorFC",322.0,3.4028,3.87,4.2,5.906);

  CGen.setMainRadius(26.0);
  CGen.setFrame(60.0,60.0);
  CGen.generateChopper(Control,"vorChopperA",8.0,10.0,4.55);    

  // Double Blade chopper
  BGen.setMaterials("Inconnel","Aluminium");
  BGen.setThick({0.3,0.3});
  BGen.setGap(1.0);
  BGen.addPhase({-15,165},{30.0,30.0});
  BGen.addPhase({-15,165},{30.0,30.0});
  BGen.generateBlades(Control,"vorDBlade",0.0,10.0,22.50);

  // VACUUM PIPE in Gamma shield
  PipeGen.generatePipe(Control,"vorPipeD",2.0,204.0);
  FGen.generateTaper(Control,"vorFD",200.0,3.4028,3.87,4.2,5.906);

  
  // BEAM INSERT:
  Control.addVariable("vorBInsertHeight",20.0);
  Control.addVariable("vorBInsertWidth",28.0);
  Control.addVariable("vorBInsertTopWall",1.0);
  Control.addVariable("vorBInsertLowWall",1.0);
  Control.addVariable("vorBInsertLeftWall",1.0);
  Control.addVariable("vorBInsertRightWall",1.0);
  Control.addVariable("vorBInsertWallMat","Stainless304");       

  // VACUUM PIPE: in bunker wall
  PipeGen.setPipe(6.0,0.5);
  PipeGen.setWindow(-2.0,0.5);
  PipeGen.setFlange(-4.0,1.0);
  PipeGen.generatePipe(Control,"vorPipeWall",1.0,348.0);

  // Guide in wall
  FGen.clearYOffset();
  FGen.generateTaper(Control,"vorFWall",346.0,6.0,6.0,6.0,6.0);

  PGen.setFeLayer(6.0);
  PGen.setConcLayer(10.0);
  PGen.generatePit(Control,"vorOutPitA",0.0,25.0,120.0,110.0,40.0);

  CGen.setMainRadius(33.0);
  CGen.setFrame(80.0,80.0);
  CGen.generateChopper(Control,"vorChopperOutA",20.0,15.0,9.55);

  Control.addVariable("vorFOCExitPortShape","Circle");
  Control.addVariable("vorFOCExitPortRadius",5.0);

  // FOC DISK A
  BGen.setThick({0.2});
  BGen.setInnerThick({0.4});
  BGen.setMaterials("Inconnel","B4C");
  BGen.addPhase({-15,165},{30.0,30.0});   // chopper open
  BGen.generateBlades(Control,"vorFOCDisk",2.0,20.0,30.0);

  // Shield/Pipe/Guide after bunker wall
  SGen.setRFLayers(3,8);
  SGen.generateShield(Control,"vorShieldA",1430.0,40.0,40.0,40.0,3,8);

  PipeGen.generatePipe(Control,"vorPipeOutA",2.0,1176.0);
  FGen.generateTaper(Control,"vorFOutA",1170.0,3.4028,3.87,4.2,5.906);

  PGen.setFeLayer(6.0);
  PGen.setConcLayer(10.0);
  PGen.generatePit(Control,"vorOutPitB",1200.0,25.0,120.0,110.0,40.0);

  Control.addVariable("vorFOCEntryPortBShape","Circle");
  Control.addVariable("vorFOCEntryPortBRadius",5.0);

  Control.addVariable("vorFOCExitPortBShape","Circle");
  Control.addVariable("vorFOCExitPortBRadius",5.0);

  CGen.setMainRadius(26.0);
  CGen.setFrame(60.0,60.0);
  CGen.generateChopper(Control,"vorChopperOutB",22.0,10.0,4.55);    

  // FOCB single disk chopper
  BGen.setThick({0.2});
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.generateBlades(Control,"vorFOCBBlade",0.0,70.0,90.0);

  SGen.setRFLayers(3,8);
  SGen.generateShield(Control,"vorShieldB",900.0,40.0,40.0,40.0,3,8);

  PipeGen.generatePipe(Control,"vorPipeOutC",2.0,814.0);
  FGen.generateTaper(Control,"vorFOutC",808.0,4.0,4.0,20.0,16.0);
  

      // HUT:
  Control.addVariable("vorCaveXStep",80.0);
  Control.addVariable("vorCaveYStep",800.0);
  Control.addVariable("vorCaveVoidHeight",200.0);
  Control.addVariable("vorCaveVoidDepth",183.0);
  Control.addVariable("vorCaveVoidWidth",640.0);
  Control.addVariable("vorCaveVoidLength",600.0);
  Control.addVariable("vorCaveVoidFrontCut",160.0);
  Control.addVariable("vorCaveVoidBackCut",90.0);
  Control.addVariable("vorCaveVoidFrontStep",60.0);
  Control.addVariable("vorCaveVoidBackStep",60.0);

  Control.addVariable("vorCaveFeThick",25.0);
  Control.addVariable("vorCaveConcThick",35.0);

  Control.addVariable("vorCaveFeMat","Stainless304");
  Control.addVariable("vorCaveConcMat","Concrete");


  Control.addVariable("vorCavePortShape","Circle");
  Control.addVariable("vorCavePortRadius",5.0);

    // DetectorTank
  Control.addVariable("vorTankYStep",300.0);

  Control.addVariable("vorTankNLayers",2.0);
  Control.addVariable("vorTankInnerRadius",10.0);
  Control.addVariable("vorTankOuterRadius",300.0);
  Control.addVariable("vorTankHeight",100.0);
  Control.addVariable("vorTankMidAngle",-40.0);
  Control.addVariable("vorTankFrontThick",2.0);
  Control.addVariable("vorTankInnerThick",2.0);
  Control.addVariable("vorTankRoofThick",1.0);
  Control.addVariable("vorTankBackThick",3.0);
  Control.addVariable("vorTankWallMat","Stainless304");

  
  // SAMPLE
  Control.addVariable("vorSampleXStep",0.0);
  Control.addVariable("vorSampleYStep",0.0);
  Control.addVariable("vorSampleZStep",0.0);
  Control.addVariable("vorSampleXYangle",0.0);
  Control.addVariable("vorSampleZangle",0.0);
  Control.addVariable("vorSampleNLayers",2.0);
  Control.addVariable("vorSampleRadius1",0.5);
  Control.addVariable("vorSampleRadius2",0.6);
  Control.addVariable("vorSampleHeight1",2.0);
  Control.addVariable("vorSampleHeight2",2.1);
  Control.addVariable("vorSampleMaterial1","H2O");
  Control.addVariable("vorSampleMaterial2","Aluminium");

  return;
}
 
}  // NAMESPACE setVariable
