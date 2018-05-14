/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   ESSBeam/trex/TREXvariables.cxx
 *
 * Copyright (c) 2004-2017 by Tsitohaina Randriamalala/Stuart Ansell
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
#include "ShieldGenerator.h"
#include "FocusGenerator.h"
#include "ChopperGenerator.h"
#include "PitGenerator.h"
#include "PipeGenerator.h"
#include "BladeGenerator.h"
#include "essVariables.h"


namespace setVariable
{

void TREXvariables(FuncDataBase& Control)
  /*!
    Variable definitions for TREX
    \param Control :: Function database to use
  */
{
  ELog::RegMethod RegA("TREXvariables[F]","TREXvariables");

  
  setVariable::ChopperGenerator CGen;
  setVariable::FocusGenerator FGen;
  setVariable::ShieldGenerator SGen;
  setVariable::PitGenerator PGen;
  setVariable::PipeGenerator PipeGen;
  setVariable::BladeGenerator BGen;

  PipeGen.setPipe(10.0,0.5);
  PipeGen.setWindow(-2.0,0.5);
  PipeGen.setFlange(-4.0,1.0);

  // extent of beamline
  Control.addVariable("trexStopPoint",0);
  Control.addVariable("trexAxisXYAngle",0.0);   // rotation
  Control.addVariable("trexAxisZAngle",0.0);   // rotation
  Control.addVariable("trexAxisZStep",0.0);   // +/- height
  Control.addVariable("trexAxisYStep",175.0);   // +/- 

  FGen.setLayer(1,0.8,"Aluminium");
  FGen.setLayer(2,0.5,"CastIron");
  FGen.setLayer(3,0.5,"Void");  
  FGen.generateTaper(Control,"trexFMono",350.0,9.0,6.38,3.0,4.43);

  //  light shutter 
  PipeGen.generatePipe(Control,"trexPipeBridge",6.5,42);
  FGen.generateTaper(Control,"trexFBridge",40.0,6.38,6.0,4.43,4.59);

  //   Vac pipe +  horizontal bender (Inside Bunker)
  PipeGen.generatePipe(Control,"trexPipeInA",0.5,805.5);
  FGen.generateBender(Control,"trexBInA",803.5,6.0,6.0,4.59,6.43,
                        1900000.0,0.0);

  Control.addVariable("trexCollimAYStep",26.0);
  Control.addVariable("trexCollimALength",50.0);
  Control.addVariable("trexCollimAMat","Copper"); 
  
  PipeGen.generatePipe(Control,"trexPipeInB",1.5,805.5);
  FGen.generateBender(Control,"trexBInB",803.5,6.0,6.0,6.43,7.52,
	              1900000.0,0.0);

  Control.addVariable("trexCollimBYStep",73.0);
  Control.addVariable("trexCollimBLength",50.0);
  Control.addVariable("trexCollimBMat","Copper");

  
  PipeGen.generatePipe(Control,"trexPipeInC",1.5,200.0);
  FGen.generateBender(Control,"trexBInC",198.0,6.0,6.0,7.52,7.72,
  		      1900000.0,0.0);

  Control.addVariable("trexCollimCYStep",26.0);
  Control.addVariable("trexCollimCLength",50.0);
  Control.addVariable("trexCollimCMat","Copper");

  // BEAM INSERT
  Control.addVariable("trexBInsertANBox",1);
  Control.addVariable("trexBInsertAHeight",27.0);
  Control.addVariable("trexBInsertAWidth",25.7);
  Control.addVariable("trexBInsertALength",204.34);
  Control.addVariable("trexBInsertAMat","Void");
  Control.addVariable("trexBInsertANWall",1);
  Control.addVariable("trexBInsertAWallThick",0.35);
  Control.addVariable("trexBInsertAWallMat","Stainless304");
  Control.addVariable("trexBInsertAYStep",1.5);

  FGen.setYOffset(0.0);
  FGen.generateBender(Control,"trexFWallA",204.34,6.0,6.0,7.72,7.89,
		      1900000.0,0.0);

  Control.setVariable("trexFWallANShapeLayers",5); 
  Control.addVariable("trexFWallALayerThick1",0.8);
  Control.addVariable("trexFWallALayerThick2",0.2);
  Control.addVariable("trexFWallALayerThick3",5.0);
  Control.addVariable("trexFWallALayerThick4",3.5); 
  Control.addVariable("trexFWallALayerMat1","Copper");
  Control.addVariable("trexFWallALayerMat2","Void");
  Control.addVariable("trexFWallALayerMat3","Steel71");
  Control.addVariable("trexFWallALayerMat4","Stainless304");

  Control.addVariable("trexBInsertBNBox",1);
  Control.addVariable("trexBInsertBHeight",44.0);
  Control.addVariable("trexBInsertBWidth",42.4);
  Control.addVariable("trexBInsertBLength",195.66);
  Control.addVariable("trexBInsertBMat","Void");
  Control.addVariable("trexBInsertBNWall",1);
  Control.addVariable("trexBInsertBWallThick",0.35);
  Control.addVariable("trexBInsertBWallMat","Stainless304");
    
  FGen.generateBender(Control,"trexFWallB",195.66,6.0,6.0,7.89,8.04,
		      1900000.0,0.0);
  Control.setVariable("trexFWallBNShapeLayers",5); 
  Control.addVariable("trexFWallBLayerThick1",3.8);
  Control.addVariable("trexFWallBLayerThick2",0.2);
  Control.addVariable("trexFWallBLayerThick3",10.35);
  Control.addVariable("trexFWallBLayerThick4",3.5); 
  Control.addVariable("trexFWallBLayerMat1","Copper");
  Control.addVariable("trexFWallBLayerMat2","Void");
  Control.addVariable("trexFWallBLayerMat3","Steel71");
  Control.addVariable("trexFWallBLayerMat4","Stainless304");

  //Out of bunker
  PGen.setFeLayer(6.0);
  PGen.setConcLayer(10.0);
  PGen.generatePit(Control,"trexPitA",2583.0,25.0,160.0,100.0,66.0);
  Control.addVariable("trexPitAXStep",1.779);
  Control.addVariable("trexPitAXYAngle",-0.078);
  Control.addVariable("trexPitACutFrontShape","Square");
  Control.addVariable("trexPitACutFrontRadius",5.0);
  Control.addVariable("trexPitACutBackShape","Square");
  Control.addVariable("trexPitACutBackRadius",5.0);

  CGen.setMainRadius(38.122);
  CGen.setFrame(86.5,86.5);
  CGen.generateChopper(Control,"trexChopperA",0.0,12.0,6.55);

  BGen.setMaterials("Copper","B4C");
  BGen.setThick({0.2});
  BGen.addPhase({95},{60});
  BGen.generateBlades(Control,"trexADisk",0.0,20.0,35.0);

  const double shieldMatThick(60.0);
  const double shieldVoidThick(25.0);
  const double shieldTotalThick = shieldMatThick+shieldVoidThick;
  SGen.addWall(1,shieldVoidThick,"CastIron");
  SGen.addRoof(1,shieldVoidThick,"CastIron");
  SGen.addFloor(1,shieldVoidThick,"Concrete");
  SGen.addFloorMat(5,"Concrete");
  SGen.addRoofMat(5,"Concrete");
  SGen.addWallMat(5,"Concrete");
  SGen.setRFLayers(3,8);

  SGen.generateShield(Control,"trexShieldA",381.25,
		      shieldTotalThick,shieldTotalThick,shieldTotalThick,1,8);
  Control.addVariable("trexShieldAYStep",184.25);

  PipeGen.generatePipe(Control,"trexPipeOutA",0.25,369.5);
  Control.addVariable("trexPipeOutAXYAngle",-0.0086);
  
  FGen.setLayer(1,0.5,"Borosilicate");
  FGen.setLayer(2,0.5,"CastIron");
  FGen.setLayer(3,0.5,"Void");  
  FGen.clearYOffset();
  FGen.generateBender(Control,"trexBOutA",367.5,6.0,6.0,8.04,8.27,
		      1900000.0,0.0);

  PGen.generatePit(Control,"trexPitB",3383.0,25.0,160.0,100.0,66.0);
  Control.addVariable("trexPitBXStep",3.042);
  Control.addVariable("trexPitBXYAngle",-0.102);
  Control.addVariable("trexPitBCutFrontShape","Square");
  Control.addVariable("trexPitBCutFrontRadius",5.0);
  Control.addVariable("trexPitBCutBackShape","Square");
  Control.addVariable("trexPitBCutBackRadius",5.0);
  CGen.generateChopper(Control,"trexChopperB",0.0,12.0,6.55);
  BGen.generateBlades(Control,"trexBDisk",0.0,25.0,35.0);

  SGen.generateShield(Control,"trexShieldB",763.0,
		      shieldTotalThick,shieldTotalThick,shieldTotalThick,
		      1,8);

  PipeGen.generatePipe(Control,"trexPipeOutB",0.25,762.5);
  FGen.generateBender(Control,"trexBOutB",760.5,6.0,6.0,8.27,8.40,
 		      1900000.0,0.0);

  /// After BW2 Chopper pit
  PGen.generatePit(Control,"trexPitC",8378.0,35.0,160.0,100.0,66.0);
  Control.addVariable("trexPitCXStep",18.57);
  Control.addVariable("trexPitCXYAngle",-0.254);
  Control.addVariable("trexPitCCutFrontShape","Square");
  Control.addVariable("trexPitCCutFrontRadius",5.0);
  Control.addVariable("trexPitCCutBackShape","Square");
  Control.addVariable("trexPitCCutBackRadius",5.0);

  CGen.generateChopper(Control,"trexChopperC",0.0,20.0,10.0);
  setVariable::BladeGenerator BGen1;
  BGen1.setMaterials("Copper","B4C");
  BGen1.setThick({7.0});
  BGen1.addPhase({95},{60});
  BGen1.generateBlades(Control,"trexCDisk",3.5,20.0,30.0);
  
  /// array section
  FGen.clearYOffset();

  SGen.generateShield(Control,"trexShieldC",4958.2,
		      shieldTotalThick,shieldTotalThick,shieldTotalThick,1,8);

  
  for(size_t i=0;i<7;i++)
    {
      const std::string strNum(StrFunc::makeString(i));
      PipeGen.generatePipe(Control,"trexPipeOutC"+strNum,0.25,707.75);
      FGen.generateBender(Control,"trexBOutC"+strNum,705.75,6.0,6.0,8.50,
			  8.50,1900000.0,0.0);
    }
  SGen.generateShield(Control,"trexShieldD",576.5,shieldTotalThick,
                      shieldTotalThick,shieldTotalThick,4,8);
  
  PipeGen.generatePipe(Control,"trexPipeOutD",0.25,576.0);
  FGen.generateBender(Control,"trexBOutD",574.0,6.0,6.0,8.50,8.50,
		      1900000.0,0.0);

  // End LOS
  
  PGen.generatePit(Control,"trexPitE",1367.75,45.0,160.0,100.0,66.0);
  Control.addVariable("trexPitECutFrontShape","Square");
  Control.addVariable("trexPitECutFrontRadius",5.0);
  Control.addVariable("trexPitECutBackShape","Square");
  Control.addVariable("trexPitECutBackRadius",5.0);
  CGen.generateChopper(Control,"trexChopperE",0.0,35.0,25.0);
  BGen.generateBlades(Control,"trexE1Disk",-10.0,20.0,30.0);
  BGen.generateBlades(Control,"trexE2Disk",10.0,20.0,30.0);

  SGen.generateShield(Control,"trexShieldE",1366.5,shieldTotalThick,
                      shieldTotalThick,shieldTotalThick,1,8);
  PipeGen.generatePipe(Control,"trexPipeOutE",0.25,1366.0);
  FGen.generateRectangle(Control,"trexGOutE",1364.0,6.0,8.50);

  SGen.generateShield(Control,"trexShieldF",5106.0,shieldTotalThick,
                      shieldTotalThick,shieldTotalThick,1,8);
  
  PipeGen.generatePipe(Control,"trexPipeOutF0",0.25,638.0);
  FGen.generateTaper(Control,"trexGOutF0",636.0,6.0,6.23,8.5,8.5);
  
  double FVS[7]={8.50,8.50,8.48,8.29,7.92,7.27,6.28};
  double FVE[7]={8.50,8.48,8.29,7.92,7.27,6.28,4.84};
  double FHS[7]={6.23,6.78,7.01,6.98,6.74,6.19,5.28};
  double FHE[7]={6.78,7.01,6.98,6.74,6.19,5.28,3.81};
  for(size_t i=1;i<7;i++)
    {
      const std::string strNum(StrFunc::makeString(i));
      PipeGen.generatePipe(Control,"trexPipeOutF"+strNum,0.25,638.0);
      FGen.generateTaper(Control,"trexGOutF"+strNum,636.0,FHS[i-1],FHE[i-1],
			 FVS[i-1],FVE[i-1]);
    }
  
  PipeGen.generatePipe(Control,"trexPipeOutF7",0.25,638.0);
  FGen.generateTaper(Control,"trexGOutF7",636.0,FHS[6],FHE[6],
			 FVS[6],FVE[6]);

  FGen.setYOffset(0.25);
  FGen.generateTaper(Control,"trexGOutG",60.0,3.81,3.75,4.84,4.79);
  
  CGen.generateChopper(Control,"trexChopperG",6.0,12.0,6.55);
  Control.addVariable("trexChopperGYStep",6.25);
  BGen.generateBlades(Control,"trexGDisk",0.0,25.0,35.0);

  FGen.clearYOffset();
  PipeGen.generatePipe(Control,"trexPipeOutH",0.25,289.5);
  FGen.generateTaper(Control,"trexGOutH",287.5,3.57,2.52,4.61,3.71);
  CGen.generateChopper(Control,"trexChopperH",8.0,16.0,8.0);
  Control.addVariable("trexChopperHYStep",310.0);
  BGen.generateBlades(Control,"trexH1Disk",-1.0,25.0,35.0);
  BGen.generateBlades(Control,"trexH2Disk",1.0,25.0,35.0);

  FGen.setYOffset(0.25);
  FGen.generateTaper(Control,"trexGOutI",98.5,2.52,1.5,3.71,3.0);

  // TREXHUT:
  
  Control.addVariable("trexCaveYStep",5666.5);
  Control.addVariable("trexCaveXStep",0.0);
  Control.addVariable("trexCaveVoidFront",60.0);
  Control.addVariable("trexCaveVoidHeight",600.0);
  Control.addVariable("trexCaveVoidDepth",150.0);
  Control.addVariable("trexCaveVoidWidth",1000.0);
  Control.addVariable("trexCaveVoidLength",1000.0);

  Control.addVariable("trexCaveL1Front",0.5);
  Control.addVariable("trexCaveL1LeftWall",0.5);
  Control.addVariable("trexCaveL1RightWall",0.5);
  Control.addVariable("trexCaveL1Roof",0.5);
  Control.addVariable("trexCaveL1Floor",0.5);
  Control.addVariable("trexCaveL1Back",0.5);

  Control.addVariable("trexCaveL2Front",60.5);
  Control.addVariable("trexCaveL2LeftWall",60.5);
  Control.addVariable("trexCaveL2RightWall",60.5);
  Control.addVariable("trexCaveL2Roof",60.5);
  Control.addVariable("trexCaveL2Floor",60.5);
  Control.addVariable("trexCaveL2Back",60.5);

  Control.addVariable("trexCaveL3Front",70.5);
  Control.addVariable("trexCaveL3LeftWall",70.5);
  Control.addVariable("trexCaveL3RightWall",70.5);
  Control.addVariable("trexCaveL3Roof",70.5);
  Control.addVariable("trexCaveL3Floor",70.5);
  Control.addVariable("trexCaveL3Back",70.5);

  Control.addVariable("trexCaveL3Mat","Concrete");
  Control.addVariable("trexCaveL2Mat","Concrete");
  Control.addVariable("trexCaveL1Mat","Void");

  Control.addVariable("trexCaveFrontCutShape","Circle");
  Control.addVariable("trexCaveFrontCutRadius",10.0);

  return;
}
  
}//NAMESPACE setVariable
