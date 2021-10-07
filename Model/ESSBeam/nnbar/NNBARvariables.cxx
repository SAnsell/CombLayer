/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:    ESSBeam/nnbar/NNBARvariables.cxx
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
#include "essVariables.h"
#include "ShieldGenerator.h"
#include "LayerGenerator.h"
#include "FocusGenerator.h"
#include "ChopperGenerator.h"
#include "PitGenerator.h"
#include "PipeGenerator.h"
#include "JawGenerator.h"
#include "BladeGenerator.h"
#include "CryoGenerator.h"

namespace setVariable
{

 
  
void
NNBARvariables(FuncDataBase& Control)
  /*!
    Create all the beamline variables for vor
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("NNBARvariables[F]","NNBARvariables");

  setVariable::ChopperGenerator CGen;
  setVariable::CryoGenerator CryGen;
  setVariable::FocusGenerator FGen;
  setVariable::LayerGenerator LGen;
  setVariable::PitGenerator PGen;
  setVariable::PipeGenerator PipeGen;
  setVariable::BladeGenerator BGen;
  setVariable::JawGenerator JawGen;

  Control.addVariable("nnbarStartPoint",0);
  Control.addVariable("nnbarStopPoint",0);
  
  LGen.setWall(100.0,{35.0,65.0}, {4,4}, {"CastIron","Concrete"});
  LGen.setRoof(100.0,{35.0,65.0}, {4,4}, {"CastIron","Concrete"});
  LGen.setFloor(100.0,{35.0,65.0}, {4,4}, {"CastIron","Concrete"});

  FGen.setLayer(1,0.5,"Copper");
  FGen.setLayer(2,0.5,"Void");
  FGen.setYOffset(2.0);
  FGen.generateTaper(Control,"nnbarFA",350.0, 70.0,146.0, 105.0,112.0);

  PipeGen.setPipe(35.0,1.0);
  PipeGen.setWindow(-4.0,1.0);
  PipeGen.setFlange(-8.0,4.0);
  PipeGen.generatePipe(Control,"nnbarPipeB",8.0,46.0);

  FGen.setLayer(1,0.5,"Aluminium");
  FGen.clearYOffset();
  FGen.generateTaper(Control,"nnbarFB",38.0, 50.0,55.0, 20.0,25.0);

  // Section from 6m to 10m
  PipeGen.setPipe(35.0,1.0);
  PipeGen.generatePipe(Control,"nnbarPipeC",4.0,400.0);

  FGen.clearYOffset();
  FGen.generateTaper(Control,"nnbarFC",392.0, 55.0,57.5, 25.0,27.5);

  // Section from 10m to 30m
  PipeGen.setPipe(35.0,1.0);
  PipeGen.generatePipe(Control,"nnbarPipeD",4.0,2000.0);

  FGen.clearYOffset();
  FGen.generateRectangle(Control,"nnbarFD",1992.0,57.5,27.5);

  // BEAM INSERT:
  Control.addVariable("nnbarBInsertHeight",90.0);
  Control.addVariable("nnbarBInsertWidth",90.0);
  Control.addVariable("nnbarBInsertTopWall",1.0);
  Control.addVariable("nnbarBInsertLowWall",1.0);
  Control.addVariable("nnbarBInsertLeftWall",1.0);
  Control.addVariable("nnbarBInsertRightWall",1.0);
  Control.addVariable("nnbarBInsertWallMat","Stainless304");       


  LGen.generateLayer(Control,"nnbarShieldA",3500.0,1);
  // Section from 30m to 50m
  PipeGen.generatePipe(Control,"nnbarPipeOutA",4.0,2000.0);

  FGen.setYOffset(12.0);
  FGen.generateRectangle(Control,"nnbarFOutA",1996.0,57.5,27.5);

  // Extra Section from 30m [Drift Tube]
  PipeGen.generatePipe(Control,"nnbarPipeOutA",8.0,3000.0);

    // HUT:  
  Control.addVariable("nnbarCaveYStep",25.0);
  Control.addVariable("nnbarCaveXStep",0.0);
  Control.addVariable("nnbarCaveVoidFront",60.0);
  Control.addVariable("nnbarCaveVoidHeight",270.0);
  Control.addVariable("nnbarCaveVoidDepth",225.0);
  Control.addVariable("nnbarCaveVoidWidth",480.0);  // max 5.8 full
  Control.addVariable("nnbarCaveVoidLength",960.0);

  Control.addVariable("nnbarCaveFeFront",25.0);
  Control.addVariable("nnbarCaveFeLeftWall",15.0);
  Control.addVariable("nnbarCaveFeRightWall",15.0);
  Control.addVariable("nnbarCaveFeRoof",15.0);
  Control.addVariable("nnbarCaveFeFloor",15.0);
  Control.addVariable("nnbarCaveFeBack",15.0);

  Control.addVariable("nnbarCaveConcFront",35.0);
  Control.addVariable("nnbarCaveConcLeftWall",35.0);
  Control.addVariable("nnbarCaveConcRightWall",35.0);
  Control.addVariable("nnbarCaveConcRoof",35.0);
  Control.addVariable("nnbarCaveConcFloor",50.0);
  Control.addVariable("nnbarCaveConcBack",35.0);

  Control.addVariable("nnbarCaveFeMat","Stainless304");
  Control.addVariable("nnbarCaveConcMat","Concrete");

  // Beam port through front of cave
  Control.addVariable("nnbarCaveCutShape","Circle");
  Control.addVariable("nnbarCaveCutRadius",10.0);

  // Beam port through front of cave
  Control.addVariable("nnbarCaveCutShape","Circle");
  Control.addVariable("nnbarCaveCutRadius",100.0);


  //Overriding default values for monolith variables
  //Bulk flight variable settings
  
  //1. Flight path modifications in monolith bulk
  Control.addVariable("TopNNBARFlightTapSurf", "cone");
  Control.addVariable("TopNNBARFlightXStep",
	       8.9);// Step from centre due to 270XY rotation goes along X 
  Control.addVariable("TopNNBARFlightYStep", 0);// ESS-1210581.2
  Control.addVariable("TopNNBARFlightZStep", 13.7);// Step from centre

  Control.addVariable("TopNNBARFlightAngleXY1",4.0);  // Angle out
  Control.addVariable("TopNNBARFlightAngleXY2",4.0);  // Angle out
  Control.addVariable("TopNNBARFlightXYAngle",
		      270.0);//creating wrt master origin
  Control.addVariable("TopNNBARFlightZAngle",0.0);
  // Step down angle !!! 1.455 is too much -
  Control.addVariable("TopNNBARFlightAngleZTop",4.0);       // fight line cuts the Bilbao target wheel ESS-0032315.3
  Control.addVariable("TopNNBARFlightAngleZBase",4.0);   // Step up angle ESS-0032315.3 

  // Full height = TopFlyTotalHeight
  Control.addVariable("TopNNBARFlightHeight", 3.0);     //  ESS-1210581.2
  Control.addVariable("TopNNBARFlightWidth", 30.6);     //  ESS-1210581.2
  Control.addVariable("TopNNBARFlightNLiner",0); // TSV32IS
  //  Control.addVariable("TopNNBARFlightLinerThick1",0.3);      
  //  Control.addVariable("TopNNBARFlightLinerMat1","Aluminium");      




  //2. Flight path modifications in Twister top Butterfly
  //specs unknown, taken similar to flight in the bulk
  Control.addVariable("TopTwisterNNBARFlightTapSurf", "cone");
  Control.addVariable("TopTwisterNNBARFlightXStep",
	       8.9);// Step from centre due to 270XY rotation goes along X 
  Control.addVariable("TopTwisterNNBARFlightYStep", 0);// ESS-1210581.2
  Control.addVariable("TopTwisterNNBARFlightZStep", 13.7);// Step from centre

  Control.addVariable("TopTwisterNNBARFlightAngleXY1",4.0);  // Angle out
  Control.addVariable("TopTwisterNNBARFlightAngleXY2",4.0);  // Angle out
  Control.addVariable("TopTwisterNNBARFlightXYAngle",
		      270.0);//creating wrt master origin
  Control.addVariable("TopTwisterNNBARFlightZAngle",0.0);
  // Step down angle !!! 1.455 is too much -
  Control.addVariable("TopTwisterNNBARFlightAngleZTop",3.0);
  // fight line cuts the Bilbao target wheel ESS-0032315.3
  Control.addVariable("TopTwisterNNBARFlightAngleZBase",3.0);
   // Step up angle, 4.0 degrees for flight in bulk, ESS-0032315.3,
   // making 2 degrees for twister

  // Full height = TopFlyTotalHeight
  Control.addVariable("TopTwisterNNBARFlightHeight", 3.0);     //  ESS-1210581.2
  Control.addVariable("TopTwisterNNBARFlightWidth", 30.6);     //  ESS-1210581.2
  Control.addVariable("TopTwisterNNBARFlightNLiner",0); // TSV32IS
  //  Control.addVariable("TopTwisterNNBARFlightLinerThick1",0.3);      
  //  Control.addVariable("TopTwisterNNBARFlightLinerMat1","Aluminium");      



  
  // Flight NNBAR from lower moderator, bulk 
  
  Control.addVariable("LowNNBARFlightTapSurf", "cone");
  Control.addParse<double>("LowNNBARFlightXStep",
                           "TopNNBARFlightXStep");
// Step from centre. due to 270XY rotation done from X
  Control.addVariable("LowNNBARFlightYStep", 0.0);// ESS-1210581.2
  Control.addVariable("LowNNBARFlightZStep",
	       -15.2); // Step from TCS center ESS-1210581.2

 Control.addParse<double>("LowNNBARFlightAngleXY1",
			  "TopNNBARFlightAngleXY1");  // Angle out
 Control.addParse<double>("LowNNBARFlightAngleXY2",
			  "TopNNBARFlightAngleXY2");  // Angle out
 Control.addParse<double> ("LowNNBARFlightXYAngle",
			   "TopNNBARFlightXYAngle");
  Control.addVariable("LowNNBARFlightZAngle",0.0);
  // Step down angle !!! 1.455 is too much -
  Control.addVariable("LowNNBARFlightAngleZTop",4.0);    // fight line cu
  Control.addVariable("LowNNBARFlightAngleZLow",4.0);    // fight line cuts the Bilbao target wheel ESS-0032315.3
  Control.addVariable("LowNNBARFlightAngleZBase",4.0);   // Step up angle ESS-0032315.3 

  // Full height = LowFlyTotalHeight
  Control.addVariable("LowNNBARFlightHeight", 6.0);     //  ESS-1210581.2
  Control.addVariable("LowNNBARFlightWidth", 30.6);     //  ESS-1210581.2
  Control.addVariable("LowNNBARFlightNLiner",0); // TSV32IS
  //  Control.addVariable("LowNNBARFlightLinerThick1",0.3);      
  //  Control.addVariable("LowNNBARFlightLinerMat1","Aluminium");      


  // Flight NNBAR from lower moderator, bulk 
  
  Control.addVariable("LowTwisterNNBARFlightTapSurf", "cone");
  Control.addParse<double>("LowTwisterNNBARFlightXStep",
                           "TopNNBARFlightXStep");
// Step from centre. due to 270XY rotation done from X
  Control.addVariable("LowTwisterNNBARFlightYStep", 0.0);// ESS-1210581.2
  Control.addVariable("LowTwisterNNBARFlightZStep",
	       -15.2-9); // Step from TCS center ESS-1210581.2

 Control.addParse<double>("LowTwisterNNBARFlightAngleXY1",
			  "TopTwisterNNBARFlightAngleXY1");  // Angle out
 Control.addParse<double>("LowTwisterNNBARFlightAngleXY2",
			  "TopTwisterNNBARFlightAngleXY2");  // Angle out
 Control.addParse<double> ("LowTwisterNNBARFlightXYAngle",
			   "TopTwisterNNBARFlightXYAngle");
  Control.addVariable("LowTwisterNNBARFlightZAngle",0.0);
  // Step down angle !!! 1.455 is too much -
  Control.addVariable("LowTwisterNNBARFlightAngleZTop",0.0);
  // Made flat fight line cu
  Control.addVariable("LowTwisterNNBARFlightAngleZLow",4.0);    // fight line cuts the Bilbao target wheel ESS-0032315.3
  Control.addVariable("LowTwisterNNBARFlightAngleZBase",2.0);
  // Make less than in drawings

  // Full height = LowFlyTotalHeight
  Control.addVariable("LowTwisterNNBARFlightHeight", 6.0 + 18.0);
  // make to moderator height  
  Control.addVariable("LowTwisterNNBARFlightWidth", 40.6);
  //  Ballpark guess, make wider then ESS-1210581.2
  Control.addVariable("LowTwisterNNBARFlightNLiner",0); // TSV32IS
  //  Control.addVariable("LowTwisterNNBARFlightLinerThick1",0.3);      
  //  Control.addVariable("LowTwisterNNBARFlightLinerMat1","Aluminium");      



  
  
  //Guide bay variable settings
  //Narrow down and shift BLineLow10 to avoid clash with NNBAR beamline
  Control.addVariable("G1BLineLow10Width1",1.0);
  Control.addVariable("G1BLineLow10BeamXStep",-10.0);
  
  Control.addVariable("G1BLineLow11Width1",100.0);
  Control.addVariable("G1BLineLow11Width2",115.0);
  Control.addVariable("G1BLineLow11Width3",155.0);

  Control.addVariable("G1BLineLow11Depth1",40.0+10+7.5);
  Control.addVariable("G1BLineLow11Depth2",45.0+10+7.5);
  Control.addVariable("G1BLineLow11Depth3",48.0+10+7.5);
  
  Control.addVariable("G1BLineLow11Height1",40.0+30-7.5);
  Control.addVariable("G1BLineLow11Height2",44.0+30-7.5);
  Control.addVariable("G1BLineLow11Height3",48.0+30-7.5);

  Control.addVariable("G1BLineLow11BeamZStep",7.5);
  Control.addVariable("G1BLineLow11BeamXStep",-2.0);
  // BeamZStep -- applies double shift. Bug in GuideItem.cxx?

  Control.addVariable("GuideBayInnerHeight",70.0);
  Control.addVariable("GuideBayInnerDepth",70.0);
  Control.addVariable("GuideBayHeight",80.0);
  Control.addVariable("GuideBayDepth",80.0);

  return;
}
 
}  // NAMESPACE setVariable
 
