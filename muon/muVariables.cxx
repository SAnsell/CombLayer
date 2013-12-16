/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   muon/muVariables.cxx
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
  Control.addVariable("TargShieldXYAngle",0.0);
  Control.addVariable("TargShieldHeight",160.5);  
  Control.addVariable("TargShieldDepth",215.2);  
  Control.addVariable("TargShieldWidth",80.0);         
  Control.addVariable("TargShieldBaseThick",20.0);        
  Control.addVariable("TargShieldForwThick",90.0);       
  Control.addVariable("TargShieldBackThick",60.0);         
  Control.addVariable("TargShieldMuonThick",20.0);       
  Control.addVariable("TargShieldJapThick",20.0);  

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
  Control.addVariable("EpbInTubeRadius",12.1);
  Control.addVariable("EpbInTubeThick",0.5);
  Control.addVariable("EpbInTubeLength",123.25-15.0);      
  Control.addVariable("EpbInTubeMat","Stainless316L"); 

//   EPB beamline

  // EPB Tube - Out
  Control.addVariable("EpbOutTubeXStep",0.0);  
  Control.addVariable("EpbOutTubeYStep",15.0);  
  Control.addVariable("EpbOutTubeZStep",0.0);   
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
  Control.addVariable("FirstShieldXYAngle",0.0);
  Control.addVariable("FirstShieldHeight",160.5);  
  Control.addVariable("FirstShieldDepth",100.0);  
  Control.addVariable("FirstShieldWidth",140.0);          
  Control.addVariable("FirstShieldSteelMat","SteelXC06");      // Mild Steel - XC-06 is mild enough!
  Control.addVariable("FirstShieldNLayers",0);      // Number of layers  

  // Tube in 1st shield
  Control.addVariable("FirstTubeXStep",0.0);  
  Control.addVariable("FirstTubeYStep",155.0);  
  Control.addVariable("FirstTubeZStep",0.0);    
  Control.addVariable("FirstTubeRadius",12.1);
  Control.addVariable("FirstTubeThick",0.5);
  Control.addVariable("FirstTubeLength",44.8);      
  Control.addVariable("FirstTubeMat","Stainless316L");

  
  // Collimator shield 3 - Layered
  Control.addVariable("ThirdShieldXStep",0.0);  
  Control.addVariable("ThirdShieldYStep",298.3);  
  Control.addVariable("ThirdShieldZStep",-24.2);
  Control.addVariable("ThirdShieldXYAngle",0.0); 
  Control.addVariable("ThirdShieldHeight",320.0);  
  Control.addVariable("ThirdShieldDepth",76.0);  
  Control.addVariable("ThirdShieldWidth",166.0);        
  Control.addVariable("ThirdShieldSteelMat","SteelXC06");      // Mild Steel - XC-06 is mild enough!
  Control.addVariable("ThirdShieldNLayers",17);      // Number of layers

  // Tube in layered shield
  Control.addVariable("ThirdTubeXStep",0.0);  
  Control.addVariable("ThirdTubeYStep",260.8);  
  Control.addVariable("ThirdTubeZStep",0.0);    
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
  Control.addVariable("FinalTubeRadius",16.0);
  Control.addVariable("FinalTubeThick",0.5);
  Control.addVariable("FinalTubeLength",100.0);      
  Control.addVariable("FinalTubeMat","Stainless316L");



//   Muon beamline

  // Q1 quadrupole (Q-137 type) --- check this
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

  // Q2 quadrupole (Q-35 type) 
  Control.addVariable("Q2quadXStep",208.10);  
  Control.addVariable("Q2quadYStep",0.0);  
  Control.addVariable("Q2quadZStep",0.0);
  Control.addVariable("Q2quadXSize",67.4);      //
  Control.addVariable("Q2quadYSize",42.0);      //
  Control.addVariable("Q2quadZSize",67.4);      //
  Control.addVariable("Q2quadXangle",0.0);      //
  Control.addVariable("Q2quadYangle",45.0);      //  
  Control.addVariable("Q2quadZangle",90.0);      //  
  Control.addVariable("Q2quadCutLenOut",10.2);      //
  Control.addVariable("Q2quadCutLenMid",5.05);      //

  Control.addVariable("Q2quadSteelThick",8.35);      //
  Control.addVariable("Q2quadCopperThick",11.95);      //
  Control.addVariable("Q2quadCopperYSize",72.0);      //  
  
  Control.addVariable("Q2quadInsertSize",16.0);      //
  Control.addVariable("Q2quadInsertThick",3.0);      //
  
  Control.addVariable("Q2quadSteelMat","SteelXC06");      // Steel XC-06: Soft magnetic (yoke) steel
  Control.addVariable("Q2quadCopperMat","Copper");
  Control.addVariable("Q2quadInsertMat","SteelXC06");    // Steel XC-06: Soft magnetic (yoke) steel
  



  return;
}

}  // NAMESPACE setVariable
