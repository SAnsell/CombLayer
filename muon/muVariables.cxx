/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   muon/muVariables.cxx
 *
 * Copyright (c) 2004-2014 by Stuart Ansell/Goran Skoro
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
#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>

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
MuonVariables(FuncDataBase& Control)
  /*!
    Function to set the control variables and constants
    -- This version is for Muon Beamline 
    \param Control :: Function data base to add constants too
  */
{
  ELog::RegMethod RegA("muVariables[F]","MuonVariables");
// -----------
// GLOBAL stuff
// -----------

  Control.addVariable("zero",0.0);      // Zero
  Control.addVariable("one",1.0);      // one


  // Target Vessel Shield
  Control.addVariable("TargShieldXStep",0.0);  
  Control.addVariable("TargShieldYStep",13.7);  
  Control.addVariable("TargShieldZStep",0.0);
  Control.addVariable("TargShieldXangle",0.0);
  Control.addVariable("TargShieldYangle",0.0);
  Control.addVariable("TargShieldZangle",0.0);    
  Control.addVariable("TargShieldHeight",160.5);  
  Control.addVariable("TargShieldDepth",215.2);  
  Control.addVariable("TargShieldWidth",80.0);         
  Control.addVariable("TargShieldBackThick",60.0);      
  Control.addVariable("TargShieldForwThick",90.0);          
  Control.addVariable("TargShieldLeftThick",20.0);
  Control.addVariable("TargShieldRightThick",0.0);       
  Control.addVariable("TargShieldBaseThick",20.0);
  Control.addVariable("TargShieldTopThick",0.0);              

  // Mild Steel - XC-06 is mild enough!       
  Control.addVariable("TargShieldMat","SteelXC06");
  
  // Target Vessel Box
  Control.addVariable("VesselBoxXStep",0.0);  
  Control.addVariable("VesselBoxYStep",0.0);  
  Control.addVariable("VesselBoxZStep",72.5);
  Control.addVariable("VesselBoxXYAngle",0.0);
  
  Control.addVariable("VesselBoxHeight",245.0);  
  Control.addVariable("VesselBoxDepth",32.0);  
  Control.addVariable("VesselBoxWidth",32.0);        
  Control.addVariable("VesselBoxSteelThick",1.0);        

  Control.addVariable("VesselBoxSteelMat","Stainless304");

  // Carbon Target
  Control.addVariable("CarbonTrgtXStep",0.0);  
  Control.addVariable("CarbonTrgtYStep",0.0);  
  Control.addVariable("CarbonTrgtZStep",0.0);
  Control.addVariable("CarbonTrgtXYAngle",45.0);
  
  Control.addVariable("CarbonTrgtHeight",5.0);  
  Control.addVariable("CarbonTrgtDepth",0.7);  
  Control.addVariable("CarbonTrgtWidth",5.0);        

  // Carbon, Graphite (Reactor Grade):
  Control.addVariable("CarbonTrgtMat","Graphite"); 

  // EPB Tube - In
  Control.addVariable("EpbInTubeXStep",0.0);  
  Control.addVariable("EpbInTubeYStep",-123.25);  
  Control.addVariable("EpbInTubeZStep",0.0);    
  Control.addVariable("EpbInTubeXangle",0.0);
  Control.addVariable("EpbInTubeYangle",0.0);
  Control.addVariable("EpbInTubeZangle",0.0);          
  Control.addVariable("EpbInTubeRadius",12.1);
  Control.addVariable("EpbInTubeThick",0.5);
  Control.addVariable("EpbInTubeLength",123.25-15.0);      
  Control.addVariable("EpbInTubeMat","Stainless316L"); 

//   EPB beamline

  // EPB Tube - Out
  Control.addVariable("EpbOutTubeXStep",0.0);  
  Control.addVariable("EpbOutTubeYStep",15.0);  
  Control.addVariable("EpbOutTubeZStep",0.0);
  Control.addVariable("EpbOutTubeXangle",0.0);
  Control.addVariable("EpbOutTubeYangle",0.0);
  Control.addVariable("EpbOutTubeZangle",0.0);              
  Control.addVariable("EpbOutTubeRadius",12.1);
  Control.addVariable("EpbOutTubeThick",0.5);
  Control.addVariable("EpbOutTubeLength",90.0-15.0);      
  Control.addVariable("EpbOutTubeMat","Stainless316L");

  // 1st Collimator
  Control.addVariable("FirstCollXStep",0.0);  
  Control.addVariable("FirstCollYStep",90.0);  
  Control.addVariable("FirstCollZStep",0.0);  
  Control.addVariable("FirstCollOutRadius",12.1);  
  Control.addVariable("FirstCollInRadius",12.05);
  Control.addVariable("FirstCollRadiusStartCone",3.74);  
  Control.addVariable("FirstCollRadiusStopCone",6.85);  
  Control.addVariable("FirstCollLength",65.0);      
  Control.addVariable("FirstCollTubeMat","Copper");     // Copper
  Control.addVariable("FirstCollInnerMat","Copper");      // Copper

  // 3rd Collimator - simple
  Control.addVariable("ThirdCollXStep",0.0);  
  Control.addVariable("ThirdCollYStep",199.8);  
  Control.addVariable("ThirdCollZStep",0.0);  
  Control.addVariable("ThirdCollOutRadius",12.1);  
  Control.addVariable("ThirdCollInRadius",12.05);
  Control.addVariable("ThirdCollRadiusStartCone",4.96);  
  Control.addVariable("ThirdCollRadiusStopCone",5.91);  
  Control.addVariable("ThirdCollLength",61.0);      
  Control.addVariable("ThirdCollTubeMat","Copper");     // Copper
  Control.addVariable("ThirdCollInnerMat","Copper");     // Copper

  // Collimator shield 1 (Layered if needed)
  Control.addVariable("FirstShieldXStep",0.0);  
  Control.addVariable("FirstShieldYStep",189.8);  
  Control.addVariable("FirstShieldZStep",0.0);
  Control.addVariable("FirstShieldXangle",0.0);
  Control.addVariable("FirstShieldYangle",0.0);
  Control.addVariable("FirstShieldZangle",0.0);    
  Control.addVariable("FirstShieldHeight",160.5);  
  Control.addVariable("FirstShieldDepth",100.0);  
  Control.addVariable("FirstShieldWidth",140.0);          
  Control.addVariable("FirstShieldSteelMat","SteelXC06");      // Mild Steel - XC-06 is mild enough!
  Control.addVariable("FirstShieldNLayers",0);      // Number of layers  

  // Tube in 1st shield
  Control.addVariable("FirstTubeXStep",0.0);  
  Control.addVariable("FirstTubeYStep",155.0);  
  Control.addVariable("FirstTubeZStep",0.0);
  Control.addVariable("FirstTubeXangle",0.0);
  Control.addVariable("FirstTubeYangle",0.0);
  Control.addVariable("FirstTubeZangle",0.0);           
  Control.addVariable("FirstTubeRadius",12.1);
  Control.addVariable("FirstTubeThick",0.5);
  Control.addVariable("FirstTubeLength",44.8);      
  Control.addVariable("FirstTubeMat","Stainless316L");

  
  // Collimator shield 3 - Layered
  Control.addVariable("ThirdShieldXStep",0.0);  
  Control.addVariable("ThirdShieldYStep",298.3);  
  Control.addVariable("ThirdShieldZStep",-24.2);
  Control.addVariable("ThirdShieldXangle",0.0);
  Control.addVariable("ThirdShieldYangle",0.0);
  Control.addVariable("ThirdShieldZangle",0.0);  
  Control.addVariable("ThirdShieldHeight",320.0);  
  Control.addVariable("ThirdShieldDepth",76.0);  
  Control.addVariable("ThirdShieldWidth",166.0);        
  Control.addVariable("ThirdShieldSteelMat","SteelXC06");      // Mild Steel - XC-06 is mild enough!
  Control.addVariable("ThirdShieldNLayers",17);      // Number of layers

  // Tube in layered shield
  Control.addVariable("ThirdTubeXStep",0.0);  
  Control.addVariable("ThirdTubeYStep",260.8);  
  Control.addVariable("ThirdTubeZStep",0.0);
  Control.addVariable("ThirdTubeXangle",0.0);
  Control.addVariable("ThirdTubeYangle",0.0);
  Control.addVariable("ThirdTubeZangle",0.0);           
  Control.addVariable("ThirdTubeRadius",11.15);
  Control.addVariable("ThirdTubeThick",0.5);
  Control.addVariable("ThirdTubeLength",78.0);      
  Control.addVariable("ThirdTubeMat","Stainless316L");

   // Profile monitor
  Control.addVariable("ProfileMonitorXStep",0.0);  
  Control.addVariable("ProfileMonitorYStep",344.2);  
  Control.addVariable("ProfileMonitorZStep",0.0);
  Control.addVariable("ProfileMonitorXYAngle",0.0);  
  Control.addVariable("ProfileMonitorHeight",35.0);  
  Control.addVariable("ProfileMonitorDepth",12.0);  
  Control.addVariable("ProfileMonitorWidth",35.0);        
  Control.addVariable("ProfileMonitorSteelMat","Stainless304");
  
  // Q44 quadrupole (Q-11/13 or Q-2 type) chech this! conflicting drawings...
  Control.addVariable("Q44quadXStep",0.0);  
  Control.addVariable("Q44quadYStep",392.7);  
  Control.addVariable("Q44quadZStep",0.0);
  Control.addVariable("Q44quadXSize",144.0);      //
  Control.addVariable("Q44quadYSize",62.0);      //
  Control.addVariable("Q44quadZSize",144.0);      //
  Control.addVariable("Q44quadXangle",0.0);      //
  Control.addVariable("Q44quadYangle",45.0);      //  
  Control.addVariable("Q44quadZangle",0.0);      //  
  Control.addVariable("Q44quadCutLenOut",30.0);      //
  Control.addVariable("Q44quadCutLenMid",20.0);      //

  Control.addVariable("Q44quadSteelThick",20.0);      //
  Control.addVariable("Q44quadCopperThick",25.0);      //
  Control.addVariable("Q44quadCopperYSize",81.0);      //  
  
  Control.addVariable("Q44quadInsertSize",30.0);      //
  Control.addVariable("Q44quadInsertThick",10.0);      //
  
  Control.addVariable("Q44quadSteelMat","SteelXC06");      // Steel XC-06: Soft magnetic (yoke) steel
  Control.addVariable("Q44quadCopperMat","Copper");     // Copper
  Control.addVariable("Q44quadInsertMat","SteelXC06");      // Steel XC-06: Soft magnetic (yoke) steel

  // Final Tube 
  Control.addVariable("FinalTubeXStep",0.0);  
  Control.addVariable("FinalTubeYStep",338.8);  
  Control.addVariable("FinalTubeZStep",0.0);
  Control.addVariable("FinalTubeXangle",0.0);
  Control.addVariable("FinalTubeYangle",0.0);
  Control.addVariable("FinalTubeZangle",0.0);           
  Control.addVariable("FinalTubeRadius",16.0);
  Control.addVariable("FinalTubeThick",0.5);
  Control.addVariable("FinalTubeLength",100.0);      
  Control.addVariable("FinalTubeMat","Stainless316L");



//   Muon beamline

  // Room
  Control.addVariable("MuRoomXStep",168.45);  
  Control.addVariable("MuRoomYStep",0.0);  
  Control.addVariable("MuRoomZStep",0.0);
  Control.addVariable("MuRoomXangle",0.0);
  Control.addVariable("MuRoomYangle",0.0);
  Control.addVariable("MuRoomZangle",90.0);    
  Control.addVariable("MuRoomHeight",240.0);  
  Control.addVariable("MuRoomDepth",256.9);  
  Control.addVariable("MuRoomWidth",240.0);         
  Control.addVariable("MuRoomBackThick",0.0);      
  Control.addVariable("MuRoomForwThick",0.0);          
  Control.addVariable("MuRoomLeftThick",40.0);
  Control.addVariable("MuRoomRightThick",40.0);       
  Control.addVariable("MuRoomBaseThick",55.0);
  Control.addVariable("MuRoomTopThick",40.0);
  // Mild Steel - XC-06 is mild enough!       
  Control.addVariable("MuRoomMat","SteelXC06");


  // Q1 quadrupole (Q-137 type)
  Control.addVariable("Q1quadXStep",87.05);  
  Control.addVariable("Q1quadYStep",0.0);  
  Control.addVariable("Q1quadZStep",0.0);
  Control.addVariable("Q1quadXSize",81.0);      //
  Control.addVariable("Q1quadYSize",60.9);      //
  Control.addVariable("Q1quadZSize",81.0);      //
  Control.addVariable("Q1quadXangle",0.0);      //
  Control.addVariable("Q1quadYangle",45.0);      //  
  Control.addVariable("Q1quadZangle",90.0);      //  
  Control.addVariable("Q1quadCutLenOut",20.0);      //
  Control.addVariable("Q1quadCutLenMid",12.0);      //

  Control.addVariable("Q1quadSteelThick",9.10);      //
  Control.addVariable("Q1quadCopperThick",12.00);      //
  Control.addVariable("Q1quadCopperYSize",80.3);      //  
  
  Control.addVariable("Q1quadInsertSize",18.2);      //
  Control.addVariable("Q1quadInsertThick",4.0);      //
  
  Control.addVariable("Q1quadSteelMat","SteelXC06");      // Steel XC-06: Soft magnetic (yoke) steel
  Control.addVariable("Q1quadCopperMat","Copper");     // Copper
  Control.addVariable("Q1quadInsertMat","SteelXC06");      // Steel XC-06: Soft magnetic (yoke) steel

  // Q2 quadrupole (Q-35 type - CERN QUAD MAGNET - SI-1100-005-00-A) 
  Control.addVariable("Q2quadXStep",208.1);  
  Control.addVariable("Q2quadYStep",0.0);  
  Control.addVariable("Q2quadZStep",0.0);
  Control.addVariable("Q2quadXSize",120.0);      //
  Control.addVariable("Q2quadYSize",27.2);      // 1.2 x (136mm x 2); 1.2 to account for front and back 'plate'
  Control.addVariable("Q2quadZSize",120.0);      //
  Control.addVariable("Q2quadXangle",0.0);      //
  Control.addVariable("Q2quadYangle",45.0);      //  
  Control.addVariable("Q2quadZangle",90.0);      //  
  Control.addVariable("Q2quadCutLenOut",30.0);      //
  Control.addVariable("Q2quadCutLenMid",25.05);      //

  Control.addVariable("Q2quadSteelThick",16.25);      //
  Control.addVariable("Q2quadCopperThick",22.5);      //
  Control.addVariable("Q2quadCopperYSize",52.4);      //  
  
  Control.addVariable("Q2quadInsertSize",6.0);      //
  Control.addVariable("Q2quadInsertThick",3.0);      //
  
  Control.addVariable("Q2quadSteelMat","SteelXC06");      // Steel XC-06: Soft magnetic (yoke) steel
  Control.addVariable("Q2quadCopperMat","Copper");
  Control.addVariable("Q2quadInsertMat","SteelXC06");    // Steel XC-06: Soft magnetic (yoke) steel

  // CERN QUAD MAGNET front "plate"
  Control.addVariable("CernFrontXStep",177.9);  
  Control.addVariable("CernFrontYStep",0.0);  
  Control.addVariable("CernFrontZStep",0.0);
  Control.addVariable("CernFrontXangle",0.0);
  Control.addVariable("CernFrontYangle",0.0);
  Control.addVariable("CernFrontZangle",90.0);    
  Control.addVariable("CernFrontHeight",130.0);  
  Control.addVariable("CernFrontDepth",2.72);  
  Control.addVariable("CernFrontWidth",130.0);         
  Control.addVariable("CernFrontBackThick",0.0);      
  Control.addVariable("CernFrontForwThick",0.0);          
  Control.addVariable("CernFrontLeftThick",40.0);
  Control.addVariable("CernFrontRightThick",40.0);       
  Control.addVariable("CernFrontBaseThick",40.0);
  Control.addVariable("CernFrontTopThick",40.0);
  // Mild Steel - XC-06 is mild enough!       
  Control.addVariable("CernFrontMat","SteelXC06");

  // CERN QUAD MAGNET back "plate"
  Control.addVariable("CernBackXStep",238.3);  
  Control.addVariable("CernBackYStep",0.0);  
  Control.addVariable("CernBackZStep",0.0);
  Control.addVariable("CernBackXangle",0.0);
  Control.addVariable("CernBackYangle",0.0);
  Control.addVariable("CernBackZangle",90.0);    
  Control.addVariable("CernBackHeight",130.0);  
  Control.addVariable("CernBackDepth",2.72);  
  Control.addVariable("CernBackWidth",130.0);         
  Control.addVariable("CernBackBackThick",0.0);      
  Control.addVariable("CernBackForwThick",0.0);          
  Control.addVariable("CernBackLeftThick",40.0);
  Control.addVariable("CernBackRightThick",40.0);       
  Control.addVariable("CernBackBaseThick",40.0);
  Control.addVariable("CernBackTopThick",40.0);
  // Mild Steel - XC-06 is mild enough!       
  Control.addVariable("CernBackMat","SteelXC06");

  
  // Beam window - 1 
  Control.addVariable("BWin1XStep",12.7);  
  Control.addVariable("BWin1YStep",0.0);  
  Control.addVariable("BWin1ZStep",0.0);
  Control.addVariable("BWin1Xangle",0.0);      //
  Control.addVariable("BWin1Yangle",0.0);      //  
  Control.addVariable("BWin1Zangle",-90.0);      // 

  Control.addVariable("BWin1FlCylOutRadius",7.35);      // 
//  Control.addVariable("BWin1FlCylInRadius",5.5);      // 
  Control.addVariable("BWin1FlCylInRadius",4.5);      // 
  Control.addVariable("BWin1FlCylThick",1.2);      // 

  Control.addVariable("BWin1WindowThick",0.006);      // 

  Control.addVariable("BWin1SmCylOutRadius",7.35);      // 
  Control.addVariable("BWin1SmCylThick",8.1);      // 

//  Control.addVariable("BWin1SmRadStartCone",6.0815);      // 
  Control.addVariable("BWin1SmRadStartCone",4.5);      // 
  Control.addVariable("BWin1SmRadStopCone",6.835);      // 
  Control.addVariable("BWin1SmLengthCone",8.1);      // 
  
  Control.addVariable("BWin1BgOutRadStartCone",7.35);      // 
  Control.addVariable("BWin1BgOutRadStopCone",10.165);      // 
  Control.addVariable("BWin1BgOutLengthCone",10.6);      //   
  
  Control.addVariable("BWin1BgInRadStartCone",6.835);      // 
  Control.addVariable("BWin1BgInRadStopCone",9.65);      // 
  Control.addVariable("BWin1BgInLengthCone",10.6);      //   
  
  Control.addVariable("BWin1EndRadCyl",13.25);      // 
  Control.addVariable("BWin1EndRadStartCone",9.65);      // 
  Control.addVariable("BWin1EndRadStopCone",9.80);      //   
  Control.addVariable("BWin1EndLength",0.8);      //   
  
  Control.addVariable("BWin1BigCylOutRadius",13.75);      // 
  Control.addVariable("BWin1BigCylInRadius",11.75);      // 
  Control.addVariable("BWin1BigCylThick",5.5);      //   
      
  Control.addVariable("BWin1VessMat","Stainless304");      // Steel 304
  Control.addVariable("BWin1WindowMat","Aluminium");     // Aluminium

  // Beam window - 2 
  Control.addVariable("BWin2XStep",-12.7);  
  Control.addVariable("BWin2YStep",0.0);  
  Control.addVariable("BWin2ZStep",0.0);
  Control.addVariable("BWin2Xangle",0.0);      //
  Control.addVariable("BWin2Yangle",0.0);      //  
  Control.addVariable("BWin2Zangle",90.0);      // 

  Control.addVariable("BWin2FlCylOutRadius",7.35);      // 
  Control.addVariable("BWin2FlCylInRadius",4.5);      // 
  Control.addVariable("BWin2FlCylThick",1.2);      // 

  Control.addVariable("BWin2WindowThick",0.006);      // 

  Control.addVariable("BWin2SmCylOutRadius",7.35);      // 
  Control.addVariable("BWin2SmCylThick",8.1);      // 

  Control.addVariable("BWin2SmRadStartCone",4.5);      // 
  Control.addVariable("BWin2SmRadStopCone",6.835);      // 
  Control.addVariable("BWin2SmLengthCone",8.1);      // 
  
  Control.addVariable("BWin2BgOutRadStartCone",7.35);      // 
  Control.addVariable("BWin2BgOutRadStopCone",10.165);      // 
  Control.addVariable("BWin2BgOutLengthCone",10.6);      //   
  
  Control.addVariable("BWin2BgInRadStartCone",6.835);      // 
  Control.addVariable("BWin2BgInRadStopCone",9.65);      // 
  Control.addVariable("BWin2BgInLengthCone",10.6);      //   
  
  Control.addVariable("BWin2EndRadCyl",13.25);      // 
  Control.addVariable("BWin2EndRadStartCone",9.65);      // 
  Control.addVariable("BWin2EndRadStopCone",9.80);      //   
  Control.addVariable("BWin2EndLength",0.8);      //   
  
  Control.addVariable("BWin2BigCylOutRadius",13.75);      // 
  Control.addVariable("BWin2BigCylInRadius",11.75);      // 
  Control.addVariable("BWin2BigCylThick",5.5);      //   
      
  Control.addVariable("BWin2VessMat","Stainless304");      // Steel 304
  Control.addVariable("BWin2WindowMat","Aluminium");     // Aluminium

  // Position ring 1 
  Control.addVariable("PosRing1XStep",38.9);  
  Control.addVariable("PosRing1YStep",0.0);  
  Control.addVariable("PosRing1ZStep",0.0);
  Control.addVariable("PosRing1Xangle",0.0);
  Control.addVariable("PosRing1Yangle",0.0);
  Control.addVariable("PosRing1Zangle",-90.0);           
  Control.addVariable("PosRing1Radius",13.5);
  Control.addVariable("PosRing1Thick",1.25);
  Control.addVariable("PosRing1Length",7.4);      
  Control.addVariable("PosRing1Mat","Stainless304");

  // Muon vacuum tube 1
  Control.addVariable("MuTube1XStep",46.3);  
  Control.addVariable("MuTube1YStep",0.0);  
  Control.addVariable("MuTube1ZStep",0.0);
  Control.addVariable("MuTube1Xangle",0.0);
  Control.addVariable("MuTube1Yangle",0.0);
  Control.addVariable("MuTube1Zangle",-90.0);           
  Control.addVariable("MuTube1Radius",13.5);
  Control.addVariable("MuTube1Thick",0.3);
  Control.addVariable("MuTube1Length",73.0);      
  Control.addVariable("MuTube1Mat","Stainless304");

  // Muon vacuum tube 2
  Control.addVariable("MuTube2XStep",119.3);  
  Control.addVariable("MuTube2YStep",0.0);  
  Control.addVariable("MuTube2ZStep",0.0);
  Control.addVariable("MuTube2Xangle",0.0);
  Control.addVariable("MuTube2Yangle",0.0);
  Control.addVariable("MuTube2Zangle",-90.0);           
  Control.addVariable("MuTube2Radius",17.5);
  Control.addVariable("MuTube2Thick",0.3);
  Control.addVariable("MuTube2Length",163.0);      
  Control.addVariable("MuTube2Mat","Stainless304");

  // Position ring 2 
  Control.addVariable("PosRing2XStep",282.3);  
  Control.addVariable("PosRing2YStep",0.0);  
  Control.addVariable("PosRing2ZStep",0.0);
  Control.addVariable("PosRing2Xangle",0.0);
  Control.addVariable("PosRing2Yangle",0.0);
  Control.addVariable("PosRing2Zangle",-90.0);           
  Control.addVariable("PosRing2Radius",18.0);
  Control.addVariable("PosRing2Thick",2.0);
  Control.addVariable("PosRing2Length",2.7);      
  Control.addVariable("PosRing2Mat","Stainless304");

  // Muon vacuum tube 3
  Control.addVariable("MuTube3XStep",285);  
  Control.addVariable("MuTube3YStep",0.0);  
  Control.addVariable("MuTube3ZStep",0.0);
  Control.addVariable("MuTube3Xangle",0.0);
  Control.addVariable("MuTube3Yangle",0.0);
  Control.addVariable("MuTube3Zangle",-90.0);           
  Control.addVariable("MuTube3Radius",17.5);
  Control.addVariable("MuTube3Thick",0.3);
  Control.addVariable("MuTube3Length",9.2);      
  Control.addVariable("MuTube3Mat","Stainless304");
 
  // Position ring 3 
  Control.addVariable("PosRing3XStep",294.2);  
  Control.addVariable("PosRing3YStep",0.0);  
  Control.addVariable("PosRing3ZStep",0.0);
  Control.addVariable("PosRing3Xangle",0.0);
  Control.addVariable("PosRing3Yangle",0.0);
  Control.addVariable("PosRing3Zangle",-90.0);           
  Control.addVariable("PosRing3Radius",18.0);
  Control.addVariable("PosRing3Thick",2.0);
  Control.addVariable("PosRing3Length",2.7);      
  Control.addVariable("PosRing3Mat","Stainless304");

  // Muon vacuum tube 4
  Control.addVariable("MuTube4XStep",296.9);  
  Control.addVariable("MuTube4YStep",0.0);  
  Control.addVariable("MuTube4ZStep",0.0);
  Control.addVariable("MuTube4Xangle",0.0);
  Control.addVariable("MuTube4Yangle",0.0);
  Control.addVariable("MuTube4Zangle",-90.0);           
  Control.addVariable("MuTube4Radius",17.5);
  Control.addVariable("MuTube4Thick",0.3);
  Control.addVariable("MuTube4Length",132.3);      
  Control.addVariable("MuTube4Mat","Stainless304");

  // Muon vacuum tube 5
  Control.addVariable("MuTube5XStep",429.2);  
  Control.addVariable("MuTube5YStep",0.0);  
  Control.addVariable("MuTube5ZStep",0.0);
  Control.addVariable("MuTube5Xangle",0.0);
  Control.addVariable("MuTube5Yangle",0.0);
  Control.addVariable("MuTube5Zangle",-90.0);           
  Control.addVariable("MuTube5Radius",13.5);
  Control.addVariable("MuTube5Thick",0.3);
  Control.addVariable("MuTube5Length",59.25);      
  Control.addVariable("MuTube5Mat","Stainless304");

  // Position ring 4 
  Control.addVariable("PosRing4XStep",488.45);  
  Control.addVariable("PosRing4YStep",0.0);  
  Control.addVariable("PosRing4ZStep",0.0);
  Control.addVariable("PosRing4Xangle",0.0);
  Control.addVariable("PosRing4Yangle",0.0);
  Control.addVariable("PosRing4Zangle",-90.0);           
  Control.addVariable("PosRing4Radius",13.5);
  Control.addVariable("PosRing4Thick",1.25);
  Control.addVariable("PosRing4Length",7.4);      
  Control.addVariable("PosRing4Mat","Stainless304");

  // Shield Plug - Layered
  Control.addVariable("ShieldPlugXStep",392.675);  
  Control.addVariable("ShieldPlugYStep",0.0);  
  Control.addVariable("ShieldPlugZStep",0.0);
  Control.addVariable("ShieldPlugXangle",0.0);
  Control.addVariable("ShieldPlugYangle",0.0);
  Control.addVariable("ShieldPlugZangle",90.0);  
  Control.addVariable("ShieldPlugHeight",240.0);  
  Control.addVariable("ShieldPlugDepth",191.55);  
  Control.addVariable("ShieldPlugWidth",240.0);        
  Control.addVariable("ShieldPlugSteelMat","SteelXC06");      // Mild Steel - XC-06 is mild enough!
  Control.addVariable("ShieldPlugNLayers",0);      // Number of layers
   
  return;
}

}  // NAMESPACE setVariable
