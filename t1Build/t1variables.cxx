/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   t1Build/t1variables.cxx
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
TS1real(FuncDataBase& Control)
  /*!
    Function to set the control variables and constants
    -- This version is for TS1 (real version : non-model)
    \param Control :: Function data base to add constants too
  */
{
  TS1base(Control);
  TS1CylTarget(Control);
  TS1PlateTarget(Control);
  TS1InnerTarget(Control);
  TS1Cannelloni(Control);
  TS1CylFluxTrap(Control);
  TS1EllipticCylTarget(Control);

  // Reflector:
  Control.addVariable("t1ReflectXStep",0.0);      // guess
  Control.addVariable("t1ReflectYStep",12.0-9.2);      // guess
  Control.addVariable("t1ReflectZStep",-0.875);      // guess
//  Control.addVariable("t1ReflectZStep",1.0);      // guess
  Control.addVariable("t1ReflectXSize",52.0);      // guess
  Control.addVariable("t1ReflectYSize",70.0);      // guess
  Control.addVariable("t1ReflectYSizeColdCut",5.0);      // guess  
//  Control.addVariable("t1ReflectZSize",76.0);      // guess
  Control.addVariable("t1ReflectZSize",83.0);      // guess
  Control.addVariable("t1ReflectXYangle",0.0);      // guess
//  Control.addVariable("t1ReflectCutLen",8.0);      // guess
  Control.addVariable("t1ReflectCutLen",12.00);      // guess

  Control.addVariable("t1ReflectBaseZCut",22.4);      // from centre

  Control.addVariable("t1ReflectDefMat",10);      // Be 

  // Reflector boxes:
  Control.addVariable("RBoxLBaseNLayers",2);
  Control.addVariable("RBoxLBaseLayThick1",1.0); 
  Control.addVariable("RBoxLBaseLayMat1",3);
  Control.addVariable("RBoxLBaseLayThick2",0.65);
  Control.addVariable("RBoxLBaseLayMat2",66);
  Control.addVariable("RBoxLBaseDefMat",10);      

  Control.addVariable("RBoxRBaseNLayers",2);
  Control.addVariable("RBoxRBaseLayThick1",1.0); 
  Control.addVariable("RBoxRBaseLayMat1",3);
  Control.addVariable("RBoxRBaseLayThick2",0.65);
  Control.addVariable("RBoxRBaseLayMat2",66);
  Control.addVariable("RBoxRBaseDefMat",10);      

  Control.addVariable("RBoxWaterNLayers",2);
  Control.addVariable("RBoxWaterLayThick1",1.0); 
  Control.addVariable("RBoxWaterLayMat1",3);
  Control.addVariable("RBoxWaterLayThick2",0.65);
  Control.addVariable("RBoxWaterLayMat2",66);
  Control.addVariable("RBoxWaterDefMat",10); 

  Control.addVariable("RBoxMerlinNLayers",2);
  Control.addVariable("RBoxMerlinLayThick1",1.0); 
  Control.addVariable("RBoxMerlinLayMat1",3);
  Control.addVariable("RBoxMerlinLayThick2",0.65); // check
  Control.addVariable("RBoxMerlinLayMat2",66);
  Control.addVariable("RBoxMerlinDefMat",10);

  Control.addVariable("RBoxMethaneNLayers",2);
  Control.addVariable("RBoxMethaneLayThick1",1.0); 
  Control.addVariable("RBoxMethaneLayMat1",3);
  Control.addVariable("RBoxMethaneLayThick2",0.65);
  Control.addVariable("RBoxMethaneLayMat2",66);
  Control.addVariable("RBoxMethaneDefMat",10);

  Control.addVariable("RBoxLH2NLayers",2);
  Control.addVariable("RBoxLH2LayThick1",1.0); 
  Control.addVariable("RBoxLH2LayMat1",3);
  Control.addVariable("RBoxLH2LayThick2",0.65);
  Control.addVariable("RBoxLH2LayMat2",66);
  Control.addVariable("RBoxLH2DefMat",10);                                     

  Control.addVariable("RBoxMerlinWrapperNLayers",2);
  Control.addVariable("RBoxMerlinWrapperLayThick1",1.0); 
  Control.addVariable("RBoxMerlinWrapperLayMat1",3);  
  Control.addVariable("RBoxMerlinWrapperLayThick2",0.65);
  Control.addVariable("RBoxMerlinWrapperLayMat2",66);
  Control.addVariable("RBoxMerlinWrapperDefMat",-1);

  Control.addVariable("RBoxLH2WrapperNLayers",2);
  Control.addVariable("RBoxLH2WrapperLayThick1",1.0); 
  Control.addVariable("RBoxLH2WrapperLayMat1",3);
  Control.addVariable("RBoxLH2WrapperLayThick2",0.65);
  Control.addVariable("RBoxLH2WrapperLayMat2",66);  
  Control.addVariable("RBoxLH2WrapperDefMat",-1);

  Control.addVariable("RBoxMerlinPlateNLayers",2);
  Control.addVariable("RBoxMerlinPlateLayThick1",1.0); 
  Control.addVariable("RBoxMerlinPlateLayMat1",3);
  Control.addVariable("RBoxMerlinPlateLayThick2",0.65);
  Control.addVariable("RBoxMerlinPlateLayMat2",66);  
  Control.addVariable("RBoxMerlinPlateDefMat",-1);

  Control.addVariable("RBoxLH2PlateNLayers",2);
  Control.addVariable("RBoxLH2PlateLayThick1",1.0); 
  Control.addVariable("RBoxLH2PlateLayMat1",3);
  Control.addVariable("RBoxLH2PlateLayThick2",0.65);
  Control.addVariable("RBoxLH2PlateLayMat2",66);  
  Control.addVariable("RBoxLH2PlateDefMat",-1);
  
  Control.addVariable("RBoxTopHexNLayers",2);
  Control.addVariable("RBoxTopHexLayThick1",1.0); 
  Control.addVariable("RBoxTopHexLayMat1",3);
  Control.addVariable("RBoxTopHexLayThick2",0.65);
  Control.addVariable("RBoxTopHexLayMat2",66);
  Control.addVariable("RBoxTopHexDefMat",10);
  
  Control.addVariable("RBoxTopPenNLayers",2);
  Control.addVariable("RBoxTopPenLayThick1",1.0); 
  Control.addVariable("RBoxTopPenLayMat1",3);
  Control.addVariable("RBoxTopPenLayThick2",0.65);
  Control.addVariable("RBoxTopPenLayMat2",66);
  Control.addVariable("RBoxTopPenDefMat",10); 
  
  Control.addVariable("RBoxBotHexNLayers",2);
  Control.addVariable("RBoxBotHexLayThick1",1.0); 
  Control.addVariable("RBoxBotHexLayMat1",3);
  Control.addVariable("RBoxBotHexLayThick2",0.65);
  Control.addVariable("RBoxBotHexLayMat2",66);
  Control.addVariable("RBoxBotHexDefMat",10); 
  
  Control.addVariable("RBoxBotPenNLayers",2);
  Control.addVariable("RBoxBotPenLayThick1",1.0); 
  Control.addVariable("RBoxBotPenLayMat1",3);
  Control.addVariable("RBoxBotPenLayThick2",0.65);
  Control.addVariable("RBoxBotPenLayMat2",66);
  Control.addVariable("RBoxBotPenDefMat",10);
  
  Control.addVariable("RBoxBotQuadNLayers",2);
  Control.addVariable("RBoxBotQuadLayThick1",1.0); 
  Control.addVariable("RBoxBotQuadLayMat1",3);
  Control.addVariable("RBoxBotQuadLayThick2",0.65);
  Control.addVariable("RBoxBotQuadLayMat2",66);
  Control.addVariable("RBoxBotQuadDefMat",10); 
  
  Control.addVariable("RBoxTopNLayers",2);
  Control.addVariable("RBoxTopLayThick1",1.0); 
  Control.addVariable("RBoxTopLayMat1",3);
  Control.addVariable("RBoxTopLayThick2",0.65);
  Control.addVariable("RBoxTopLayMat2",66);
  Control.addVariable("RBoxTopDefMat",10);          

  Control.addVariable("RBoxLH2VoidNLayers",0);
  Control.addVariable("RBoxLH2VoidLayThick1",1.0); 
  Control.addVariable("RBoxLH2VoidLayMat1",3);
  Control.addVariable("RBoxLH2VoidLayThick2",0.65);
  Control.addVariable("RBoxLH2VoidLayMat2",66);
  Control.addVariable("RBoxLH2VoidDefMat",0);                                     
  // LH2 Moderator:
  Control.addVariable("H2ModXStep",0.98);      
//  Control.addVariable("H2ModYStep",18.37); 
  Control.addVariable("H2ModYStep",18.37-9.2);       //  relative to VoidVessel 
  Control.addVariable("H2ModZStep",-14.6);      // Dave Bellenger
  Control.addVariable("H2ModXYAngle",-81.0);      // Dave Bellenger

  Control.addVariable("H2ModHeight",12.0);  
  Control.addVariable("H2ModDepth",8.0);    
  Control.addVariable("H2ModWidth",11.0);   

  Control.addVariable("H2ModInnerThick",0.5);     // ok
  Control.addVariable("H2ModVacThick",0.4);     // ok
  Control.addVariable("H2ModMidThick",0.3);     // ok
  Control.addVariable("H2ModTerThick",0.1);     // ok
  Control.addVariable("H2ModOuterThick",0.2);   // ok
  Control.addVariable("H2ModClearThick",0.3);   // ok

  Control.addVariable("H2ModOuterView",0.1);   // ok
  Control.addVariable("H2ModVacTop",0.1);     // ok
  Control.addVariable("H2ModClearTop",0.05);   // guess

  Control.addVariable("H2ModAlMat",5);        // Al 
  Control.addVariable("H2ModLh2Mat",16);      // LH2 
  Control.addVariable("H2ModVoidMat",0);      // void 

  Control.addVariable("H2ModLh2Temp",20.0);      // LH2 

  Control.addVariable("H2ModViewSphere",25.0);  // sphere curvature

  // CH4 Moderator new:
  Control.addVariable("CH4ModXStep",0.00);      // ok
//  Control.addVariable("CH4ModYStep",0.90);       //
  Control.addVariable("CH4ModYStep",0.9-9.2);       //  relative to VoidVessel
  Control.addVariable("CH4ModZStep",-14.6);      // Dave Bellenger   
//  Control.addVariable("CH4ModZStep",-15.2);      // 
  Control.addVariable("CH4ModXYAngle",-90.0-33.0);      // Dave Bellenger

    
  Control.addVariable("CH4ModHeight",11.5);      // ok 
  Control.addVariable("CH4ModDepth",4.5);       // ok  
  Control.addVariable("CH4ModWidth",12.0);      // ok  

  Control.addVariable("CH4ModInnerThick",0.32);     // ok
  Control.addVariable("CH4ModVacThick",0.28);     // ok
  Control.addVariable("CH4ModOuterThick",0.25);   // ok
  Control.addVariable("CH4ModClearThick",0.3);   // ok

  Control.addVariable("CH4ModOuterView",0.);   // ok - don't needed!
  Control.addVariable("CH4ModVacTop",0.0);     // ok - don't needed!
  Control.addVariable("CH4ModClearTop",0.0);   // ok - don't needed!

  // Poison !!!!  
  Control.addVariable("CH4ModPoisonXStep",0.00);      // ok
  Control.addVariable("CH4ModPoisonYStep",0.00);       // 
  Control.addVariable("CH4ModPoisonZStep",0.00);      //
  Control.addVariable("CH4ModPoisonGdThick",0.005);      //
  Control.addVariable("CH4ModPoisonAlThick",0.1);      //      

  Control.addVariable("CH4ModAlMat",5);        // Al 
  Control.addVariable("CH4ModCH4Mat",29);      // liquid methane at 100K 
  Control.addVariable("CH4ModVoidMat",0);      // void 
  Control.addVariable("CH4ModPoisonMat",6);      // Gadolinium  

  Control.addVariable("CH4ModCH4Temp",100.0);      // liquid methane at 100K 

  Control.addVariable("CH4ModViewSphere",25.0);  // sphere curvature  


  // Water Downstream Moderator:
  Control.addVariable("MerlinModXStep",0.0);      // guess
//  Control.addVariable("MerlinModYStep",18.9);        // 
  Control.addVariable("MerlinModYStep",18.9-9.2);       //  relative to VoidVessel    
  Control.addVariable("MerlinModZStep",14.6);      // guess
  Control.addVariable("MerlinModXYangle",80.0);      // guess

  Control.addVariable("MerlinModWidth",12.5);    // guess
  Control.addVariable("MerlinModDepth",4.5);    // guess
  Control.addVariable("MerlinModHeight",12.0);    // guess
  Control.addVariable("MerlinModInnerThick",0.35);    // guess
  Control.addVariable("MerlinModVacThick",0.1);    // guess

  Control.addVariable("MerlinModNPoison",1);      // ok
  Control.addVariable("MerlinModPoisonYStep1",0.00);      // ok
  Control.addVariable("MerlinModPoisonThick1",0.005);      //  

  Control.addVariable("MerlinModAlMat",5);       // Al 
  Control.addVariable("MerlinModWaterMat",11);      // water
  Control.addVariable("MerlinModPoisonMat",6);      // Gadolinium

  // Water Upstream Moderator:
  Control.addVariable("WaterModXStep",0.0);      // guess
//  Control.addVariable("WaterModYStep",0.9);        // guess
  Control.addVariable("WaterModYStep",0.9-9.2);       //  relative to VoidVessel  
  Control.addVariable("WaterModZStep",14.6);      // guess
  Control.addVariable("WaterModXYangle",-90+32.0);      // guess

  Control.addVariable("WaterModWidth",12.5);    // guess
  Control.addVariable("WaterModDepth",4.5);    // guess
  Control.addVariable("WaterModHeight",12.0);    // guess
  Control.addVariable("WaterModInnerThick",0.35);    // guess
  Control.addVariable("WaterModVacThick",0.1);    // guess

  Control.addVariable("WaterModNPoison",2);      // ok
  Control.addVariable("WaterModPoisonYStep1",-0.75125);      // ok
  Control.addVariable("WaterModPoisonThick1",0.005);      // ok
  Control.addVariable("WaterModPoisonYStep2",0.75125);      // ok
  Control.addVariable("WaterModPoisonThick2",0.005);      // ok

  Control.addVariable("WaterModAlMat",5);       // Al 
  Control.addVariable("WaterModWaterMat",11);      // water
  Control.addVariable("WaterModPoisonMat",6);      // Gadolinium

  // Flightline LH2 Moderator:

  Control.addVariable("H2FlightXStep",0.0);      // Step from centre
  Control.addVariable("H2FlightZStep",0.0);      // Step from centre
  Control.addVariable("H2FlightAngleXY1",0.0);  // Angle out
  Control.addVariable("H2FlightAngleXY2",37.0);  // Angle out
  Control.addVariable("H2FlightAngleZTop",0.0);  // Step down angle
  Control.addVariable("H2FlightAngleZBase",0.0); // Step up angle
  Control.addVariable("H2FlightHeight",10.0);     // Full height
  Control.addVariable("H2FlightWidth",14.0);     // Full width

  Control.addVariable("H2FlightNLiner",0);           // Number of layers
  Control.addVariable("H2FlightLinerThick1",0.5);    // Thickness
  Control.addVariable("H2FlightLinerThick2",0.12);   // Thickness
  Control.addVariable("H2FlightLinerMat1",5);        //  Cadnium
  Control.addVariable("H2FlightLinerMat2",7);        //  Cadnium

  // Flightline #1 CH4 Moderator:

  Control.addVariable("CH4FlightSXStep",0.0);      // Step from centre
  Control.addVariable("CH4FlightSZStep",0.0);      // Step from centre
  Control.addVariable("CH4FlightSAngleXY1",26.5);  // Angle out
  Control.addVariable("CH4FlightSAngleXY2",26.5);  // Angle out
  Control.addVariable("CH4FlightSAngleZTop",0.0);  // Step down angle
  Control.addVariable("CH4FlightSAngleZBase",0.0); // Step up angle
//  Control.addVariable("CH4FlightSHeight",13.1);     // Full height
  Control.addVariable("CH4FlightSHeight",10.0);     // Full height
  Control.addVariable("CH4FlightSWidth",14.3);     // Full width
  
  Control.addVariable("CH4FlightSNLiner",0);           // Number of layers
  Control.addVariable("CH4FlightSLinerThick1",0.5);    // Thickness
  Control.addVariable("CH4FlightSLinerThick2",0.12);   // Thickness
  Control.addVariable("CH4FlightSLinerMat1",5);        //  Cadnium
  Control.addVariable("CH4FlightSLinerMat2",7);        //  Cadnium

  // Flightline #2 CH4 Moderator:

  Control.addVariable("CH4FlightNXStep",0.0);      // Step from centre
  Control.addVariable("CH4FlightNZStep",0.0);      // Step from centre
//  Control.addVariable("CH4FlightNAngleXY1",20.0);  // Angle out
  Control.addVariable("CH4FlightNAngleXY1",13.0);  // Angle out
//  Control.addVariable("CH4FlightNAngleXY2",20.0);  // Angle out
  Control.addVariable("CH4FlightNAngleXY2",13.0);  // Angle out
  Control.addVariable("CH4FlightNAngleZTop",0.0);  // Step down angle
  Control.addVariable("CH4FlightNAngleZBase",0.0); // Step up angle
//  Control.addVariable("CH4FlightNHeight",13.1);     // Full height
  Control.addVariable("CH4FlightNHeight",10.0);     // Full height
  Control.addVariable("CH4FlightNWidth",14.3);     // Full width

  Control.addVariable("CH4FlightNNLiner",0);           // Number of layers
  Control.addVariable("CH4FlightNLinerThick1",0.5);    // Thickness
  Control.addVariable("CH4FlightNLinerThick2",0.12);   // Thickness
  Control.addVariable("CH4FlightNLinerMat1",5);        //  Cadnium
  Control.addVariable("CH4FlightNLinerMat2",7);        //  Cadnium

  // Flightline Water Downstream Moderator:

  Control.addVariable("MerlinFlightXStep",0.0);      // Step from centre
  Control.addVariable("MerlinFlightZStep",0.0);      // Step from centre
//  Control.addVariable("MerlinFlightAngleXY1",5.0);  // Angle out
  Control.addVariable("MerlinFlightAngleXY1",10.0);  // Angle out
  Control.addVariable("MerlinFlightAngleXY2",5.0);  // Angle out
  Control.addVariable("MerlinFlightAngleZTop",0.0);  // Step down angle
  Control.addVariable("MerlinFlightAngleZBase",0.0); // Step up angle
//  Control.addVariable("MerlinFlightHeight",10.0);     // Full height
  Control.addVariable("MerlinFlightHeight",9.6);     // Full height  !!! If 10 cm -> red lines (top of RBoxMerlinPlate)
  Control.addVariable("MerlinFlightWidth",13.4);     // Full width

  Control.addVariable("MerlinFlightNLiner",0);           // Number of layers
  Control.addVariable("MerlinFlightLinerThick1",0.5);    // Thickness
  Control.addVariable("MerlinFlightLinerThick2",0.12);   // Thickness
  Control.addVariable("MerlinFlightLinerMat1",5);        //  Cadnium
  Control.addVariable("MerlinFlightLinerMat2",7);        //  Cadnium

  // Flightline #1 Water Upstream Moderator:
  Control.addVariable("WatNorthFlightXStep",0.0);      // Step from centre
  Control.addVariable("WatNorthFlightZStep",0.0);      // Step from centre
//  Control.addVariable("WatNorthFlightAngleXY1",26.5);  // Angle out
  Control.addVariable("WatNorthFlightAngleXY1",25.0);  // Angle out
//  Control.addVariable("WatNorthFlightAngleXY2",26.5);  // Angle out
  Control.addVariable("WatNorthFlightAngleXY2",0.0);  // Angle out
  Control.addVariable("WatNorthFlightAngleZTop",0.0);  // Step down angle
  Control.addVariable("WatNorthFlightAngleZBase",0.0); // Step up angle
//  Control.addVariable("WatNorthFlightHeight",13.2);     // Full height
  Control.addVariable("WatNorthFlightHeight",10.0);     // Full height
  Control.addVariable("WatNorthFlightWidth",13.4);     // Full width

  Control.addVariable("WatNorthFlightNLiner",0);           // Number of layers
  Control.addVariable("WatNorthFlightLinerThick1",0.5);    // Thickness
  Control.addVariable("WatNorthFlightLinerThick2",0.12);   // Thickness
  Control.addVariable("WatNorthFlightLinerMat1",5);        //  Cadnium
  Control.addVariable("WatNorthFlightLinerMat2",7);        //  Cadnium

  // Flightline #2 Water Upstream Moderator:

  Control.addVariable("WatSouthFlightXStep",0.0);      // Step from centre
  Control.addVariable("WatSouthFlightZStep",0.0);      // Step from centre
//  Control.addVariable("WatSouthFlightAngleXY1",20.0);  // Angle out
  Control.addVariable("WatSouthFlightAngleXY1",15.0);  // Angle out
//  Control.addVariable("WatSouthFlightAngleXY2",20.0);  // Angle out
  Control.addVariable("WatSouthFlightAngleXY2",15.0);  // Angle out
  Control.addVariable("WatSouthFlightAngleZTop",0.0);  // Step down angle
  Control.addVariable("WatSouthFlightAngleZBase",0.0); // Step up angle
//  Control.addVariable("WatSouthFlightHeight",13.2);     // Full height
  Control.addVariable("WatSouthFlightHeight",10.0);     // Full height
  Control.addVariable("WatSouthFlightWidth",13.4);     // Full width

  Control.addVariable("WatSouthFlightNLiner",0);           // Number of layers
  Control.addVariable("WatSouthFlightLinerThick1",0.5);    // Thickness
  Control.addVariable("WatSouthFlightLinerThick2",0.12);   // Thickness
  Control.addVariable("WatSouthFlightLinerMat1",5);        //  Cadnium
  Control.addVariable("WatSouthFlightLinerMat2",7);        //  Cadnium

  // Beam Window (PROTON FLIGHT PATH)
  Control.addVariable("BWindowRadius",4.0);          

//  Control.addVariable("BWindowHeThick",11.725);   // He Thickness
  Control.addVariable("BWindowYStep",15.0);   // Step from front target
  Control.addVariable("BWindowIncThick1",0.3);   // Inconel Thickness 1
  Control.addVariable("BWindowWaterThick",0.1);   // Light water Thickness
  Control.addVariable("BWindowIncThick2",0.3);   // Inconel Thickness 2
  Control.addVariable("BWindowInconelMat",8);           // Inconel
  Control.addVariable("BWindowWaterMat",11);           // Light water

  

  // PIPEWORK Merlin General
  Control.addVariable("MerlinPipeWaterRadius",1.2);           
  Control.addVariable("MerlinPipeOuterRadius",1.7);           
  Control.addVariable("MerlinPipeVoidRadius",2.3);           
  Control.addVariable("MerlinPipeSteelRadius",2.8);           

  // PIPEWORK Merlin WaterIN
  Control.addVariable("MerlinPipeInXStep",-3.8); 
  Control.addVariable("MerlinPipeInYStep",0.0);             
  Control.addVariable("MerlinPipeInFullLen",31.25);        
  
  // PIPEWORK Merlin WaterOUT
  Control.addVariable("MerlinPipeOutXStep",3.8);        
  Control.addVariable("MerlinPipeOutYStep",0.0);        
  Control.addVariable("MerlinPipeOutFullLen",31.25);        
  Control.addVariable("MerlinPipeOutOuterRadius",1.7);           
  Control.addVariable("MerlinPipeOutWaterRadius",1.2);         
  Control.addVariable("MerlinPipeOutVoidRadius",2.3);            
  Control.addVariable("MerlinPipeOutSteelRadius",2.8);            

  Control.addVariable("MerlinPipeOuterMat",5);        
  Control.addVariable("MerlinPipeWaterMat",11);
  Control.addVariable("MerlinPipeSteelMat",3);


  // PIPEWORK
  Control.addVariable("H2PipeXOffset",0.0);        
  Control.addVariable("H2PipeYOffset",0.0);        
  Control.addVariable("H2PipeFullLen",30.0);        
  Control.addVariable("H2PipeDepthLen",10.0);        

  Control.addVariable("H2PipeH2InnerRadius",0.63);        
  Control.addVariable("H2PipeAlInnerRadius",0.80);        
  Control.addVariable("H2PipeH2OuterRadius",0.9);        
  Control.addVariable("H2PipeAlOuterRadius",1.10);   // Outer        
  Control.addVariable("H2PipeVacRadius",1.60);   // Outer        
  Control.addVariable("H2PipeMidRadius",1.80);   // Outer        
  Control.addVariable("H2PipeTerRadius",1.90);   // Outer        
  Control.addVariable("H2PipeAlTerRadius",2.10);   // Outer        
  Control.addVariable("H2PipeClearRadius",2.60);   // Outer        
  Control.addVariable("H2PipeSteelRadius",2.90);   // steel in Reflector       

  Control.addVariable("H2PipeAlMat",5);        
  Control.addVariable("H2PipeSteelMat",3);                  
  Control.addVariable("H2PipeH2Mat",16);     
  Control.addVariable("H2PipeH2Temp",20.0);     

  // PIPEWORK Water General
  Control.addVariable("WaterPipeWaterRadius",1.2);           
  Control.addVariable("WaterPipeOuterRadius",1.7);           
  Control.addVariable("WaterPipeVoidRadius",2.3);           
  Control.addVariable("WaterPipeSteelRadius",2.8);           

  // PIPEWORK water WaterIN
  Control.addVariable("WaterPipeInXStep",3.8); 
  Control.addVariable("WaterPipeInYStep",0.0);             
  Control.addVariable("WaterPipeInFullLen",31.25);        
  
  // PIPEWORK water WaterOUT
  Control.addVariable("WaterPipeOutXStep",-3.8);        
  Control.addVariable("WaterPipeOutYStep",0.0);        
  Control.addVariable("WaterPipeOutFullLen",31.25);        
  Control.addVariable("WaterPipeOutOuterRadius",1.7);           
  Control.addVariable("WaterPipeOutWaterRadius",1.2);         
  Control.addVariable("WaterPipeOutVoidRadius",2.3);            
  Control.addVariable("WaterPipeOutSteelRadius",2.8);            

  Control.addVariable("WaterPipeOuterMat",5);        
  Control.addVariable("WaterPipeWaterMat",11);
  Control.addVariable("WaterPipeSteelMat",3);
  
   // PIPEWORK CH4
  Control.addVariable("CH4PipeXOffset",0.0);        
  Control.addVariable("CH4PipeYOffset",0.0);        
  Control.addVariable("CH4PipeFullLen",42.2);        
  Control.addVariable("CH4PipeDepthLen",10.55);        

  Control.addVariable("CH4PipeCH4InnerRadius",0.6);        
  Control.addVariable("CH4PipeAlInnerRadius",0.7);        
  Control.addVariable("CH4PipeCH4OuterRadius",1.43);       
  Control.addVariable("CH4PipeAlOuterRadius",1.59);   // Outer        
  Control.addVariable("CH4PipeVacRadius",1.865);   // Outer        
  Control.addVariable("CH4PipeAlTerRadius",1.905);   // Outer        
  Control.addVariable("CH4PipeClearRadius",2.38);   // Outer        

  Control.addVariable("CH4PipeAlMat",5);        
  Control.addVariable("CH4PipeCH4Mat",29);      // liquid methane at 100K      
  Control.addVariable("CH4PipeCH4Temp",100.0);            
  
  // RODS:
  Control.addVariable("RodsZAngle",0.0);
  Control.addVariable("RodsXYAngle",0.0);
  Control.addVariable("RodsCentSpace",2.8);
  Control.addVariable("RodsRadius",1.0);
  Control.addVariable("RodsLinerThick",0.3);
  Control.addVariable("RodsDefMat",0);
  Control.addVariable("RodsInnerMat",29);
  Control.addVariable("RodsLinerMat",3);


  return;
}



}  // NAMESPACE setVariable
