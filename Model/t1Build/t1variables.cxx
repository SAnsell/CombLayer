/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t1Build/t1variables.cxx
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
  TS1BlockTarget(Control);
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

  Control.addVariable("t1ReflectDefMat","Be/10D2O");      // Be 

  // Reflector boxes:
  Control.addVariable("RBoxLBaseNLayers",2);
  Control.addVariable("RBoxLBaseLayThick1",0.8); 
  Control.addVariable("RBoxLBaseLayMat1","Stainless304");
  Control.addVariable("RBoxLBaseLayThick2",0.673);
  Control.addVariable("RBoxLBaseLayMat2","TS1Boral");
  Control.addVariable("RBoxLBaseDefMat","Be/10D2O");      

  Control.addVariable("RBoxRBaseNLayers",2);
  Control.addVariable("RBoxRBaseLayThick1",0.8); 
  Control.addVariable("RBoxRBaseLayMat1","Stainless304");
  Control.addVariable("RBoxRBaseLayThick2",0.673);
  Control.addVariable("RBoxRBaseLayMat2","TS1Boral");
  Control.addVariable("RBoxRBaseDefMat","Be/10D2O");      

  Control.addVariable("RBoxWaterNLayers",2);
  Control.addVariable("RBoxWaterLayThick1",0.8); 
  Control.addVariable("RBoxWaterLayMat1","Stainless304");
  Control.addVariable("RBoxWaterLayThick2",0.673);
  Control.addVariable("RBoxWaterLayMat2","TS1Boral");
  Control.addVariable("RBoxWaterDefMat","Be/10D2O"); 

  Control.addVariable("RBoxMerlinNLayers",2);
  Control.addVariable("RBoxMerlinLayThick1",0.8); 
  Control.addVariable("RBoxMerlinLayMat1","Stainless304");
  Control.addVariable("RBoxMerlinLayThick2",0.673); // check
  Control.addVariable("RBoxMerlinLayMat2","TS1Boral");
  Control.addVariable("RBoxMerlinDefMat","Be/10D2O");

  Control.addVariable("RBoxMethaneNLayers",2);
  Control.addVariable("RBoxMethaneLayThick1",1.0); 
  Control.addVariable("RBoxMethaneLayMat1","Stainless304");
  Control.addVariable("RBoxMethaneLayThick2",0.673);
  Control.addVariable("RBoxMethaneLayMat2","TS1Boral");
  Control.addVariable("RBoxMethaneDefMat","Be/10D2O");

  Control.addVariable("RBoxLH2NLayers",2);
  Control.addVariable("RBoxLH2LayThick1",1.0); 
  Control.addVariable("RBoxLH2LayMat1","Stainless304");
  Control.addVariable("RBoxLH2LayThick2",0.673);
  Control.addVariable("RBoxLH2LayMat2","TS1Boral");
  Control.addVariable("RBoxLH2DefMat","Be/10D2O");                                     

  Control.addVariable("RBoxMerlinWrapperNLayers",2);
  Control.addVariable("RBoxMerlinWrapperLayThick1",0.8); 
  Control.addVariable("RBoxMerlinWrapperLayMat1","Stainless304");  
  Control.addVariable("RBoxMerlinWrapperLayThick2",0.673);
  Control.addVariable("RBoxMerlinWrapperLayMat2","TS1Boral");
  Control.addVariable("RBoxMerlinWrapperDefMat","InValid");

  Control.addVariable("RBoxLH2WrapperNLayers",2);
  Control.addVariable("RBoxLH2WrapperLayThick1",0.8); 
  Control.addVariable("RBoxLH2WrapperLayMat1","Stainless304");
  Control.addVariable("RBoxLH2WrapperLayThick2",0.673);
  Control.addVariable("RBoxLH2WrapperLayMat2","TS1Boral");  
  Control.addVariable("RBoxLH2WrapperDefMat","InValid");

  Control.addVariable("RBoxMerlinPlateNLayers",2);
  Control.addVariable("RBoxMerlinPlateLayThick1",0.8); 
  Control.addVariable("RBoxMerlinPlateLayMat1","Stainless304");
  Control.addVariable("RBoxMerlinPlateLayThick2",0.673);
  Control.addVariable("RBoxMerlinPlateLayMat2","TS1Boral");  
  Control.addVariable("RBoxMerlinPlateDefMat","InValid");

  Control.addVariable("RBoxLH2PlateNLayers",2);
  Control.addVariable("RBoxLH2PlateLayThick1",0.8); 
  Control.addVariable("RBoxLH2PlateLayMat1","Stainless304");
  Control.addVariable("RBoxLH2PlateLayThick2",0.673);
  Control.addVariable("RBoxLH2PlateLayMat2","TS1Boral");  
  Control.addVariable("RBoxLH2PlateDefMat","InValid");
  
  Control.addVariable("RBoxTopHexNLayers",2);
  Control.addVariable("RBoxTopHexLayThick1",0.8); 
  Control.addVariable("RBoxTopHexLayMat1","Stainless304");
  Control.addVariable("RBoxTopHexLayThick2",0.673);
  Control.addVariable("RBoxTopHexLayMat2","TS1Boral");
  Control.addVariable("RBoxTopHexDefMat","Be/10D2O");
  
  Control.addVariable("RBoxTopPenNLayers",2);
  Control.addVariable("RBoxTopPenLayThick1",0.8); 
  Control.addVariable("RBoxTopPenLayMat1","Stainless304");
  Control.addVariable("RBoxTopPenLayThick2",0.673);
  Control.addVariable("RBoxTopPenLayMat2","TS1Boral");
  Control.addVariable("RBoxTopPenDefMat","Be/10D2O"); 
  
  Control.addVariable("RBoxBotHexNLayers",2);
  Control.addVariable("RBoxBotHexLayThick1",0.8); 
  Control.addVariable("RBoxBotHexLayMat1","Stainless304");
  Control.addVariable("RBoxBotHexLayThick2",0.673);
  Control.addVariable("RBoxBotHexLayMat2","TS1Boral");
  Control.addVariable("RBoxBotHexDefMat","Be/10D2O"); 
  
  Control.addVariable("RBoxBotPenNLayers",2);
  Control.addVariable("RBoxBotPenLayThick1",0.8); 
  Control.addVariable("RBoxBotPenLayMat1","Stainless304");
  Control.addVariable("RBoxBotPenLayThick2",0.673);
  Control.addVariable("RBoxBotPenLayMat2","TS1Boral");
  Control.addVariable("RBoxBotPenDefMat","Be/10D2O");
  
  Control.addVariable("RBoxBotQuadNLayers",2);
  Control.addVariable("RBoxBotQuadLayThick1",0.8); 
  Control.addVariable("RBoxBotQuadLayMat1","Stainless304");
  Control.addVariable("RBoxBotQuadLayThick2",0.673);
  Control.addVariable("RBoxBotQuadLayMat2","TS1Boral");
  Control.addVariable("RBoxBotQuadDefMat","Be/10D2O"); 
  
  Control.addVariable("RBoxTopNLayers",2);
  Control.addVariable("RBoxTopLayThick1",1.0); 
  Control.addVariable("RBoxTopLayMat1","Stainless304");
  Control.addVariable("RBoxTopLayThick2",0.673);
  Control.addVariable("RBoxTopLayMat2","TS1Boral");
  Control.addVariable("RBoxTopDefMat","Be/10D2O");          

  Control.addVariable("RBoxLH2VoidNLayers",0);
  Control.addVariable("RBoxLH2VoidLayThick1",0.8); 
  Control.addVariable("RBoxLH2VoidLayMat1","Stainless304");
  Control.addVariable("RBoxLH2VoidLayThick2",0.673);
  Control.addVariable("RBoxLH2VoidLayMat2","TS1Boral");
  Control.addVariable("RBoxLH2VoidDefMat","Void");
  
  Control.addVariable("RBoxCH4SouthNLayers",2);
  Control.addVariable("RBoxCH4SouthLayThick1",0.8); 
  Control.addVariable("RBoxCH4SouthLayMat1","Stainless304");
  Control.addVariable("RBoxCH4SouthLayThick2",0.673);
  Control.addVariable("RBoxCH4SouthLayMat2","TS1Boral");  
  Control.addVariable("RBoxCH4SouthDefMat","InValid");  

  Control.addVariable("RBoxWatNorthNLayers",2);
  Control.addVariable("RBoxWatNorthLayThick1",0.8); 
  Control.addVariable("RBoxWatNorthLayMat1","Stainless304");
  Control.addVariable("RBoxWatNorthLayThick2",0.673);
  Control.addVariable("RBoxWatNorthLayMat2","TS1Boral");  
  Control.addVariable("RBoxWatNorthDefMat","InValid");  
                                
  Control.addVariable("RBoxWatSouthNLayers",2);
  Control.addVariable("RBoxWatSouthLayThick1",0.8); 
  Control.addVariable("RBoxWatSouthLayMat1","Stainless304");
  Control.addVariable("RBoxWatSouthLayThick2",0.673);
  Control.addVariable("RBoxWatSouthLayMat2","TS1Boral");  
  Control.addVariable("RBoxWatSouthDefMat","InValid");  

  Control.addVariable("RBoxCH4NorthNLayers",2);
  Control.addVariable("RBoxCH4NorthLayThick1",0.8); 
  Control.addVariable("RBoxCH4NorthLayMat1","Stainless304");
  Control.addVariable("RBoxCH4NorthLayThick2",0.673);
  Control.addVariable("RBoxCH4NorthLayMat2","TS1Boral");  
  Control.addVariable("RBoxCH4NorthDefMat","InValid");  

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

  Control.addVariable("H2ModAlMat","Aluminium");        // Al 
//  Control.addVariable("H2ModLh2Mat","ParaH2");      // LH2 
  Control.addVariable("H2ModLh2Mat","ParaOrtho%80");      // LH2 - 80% para / 20% ortho
  Control.addVariable("H2ModVoidMat","Void");      // void 

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

  Control.addVariable("CH4ModAlMat","Aluminium");   
  Control.addVariable("CH4ModCH4Mat","CH4Liq110K");  
  Control.addVariable("CH4ModVoidMat","Void");      
  Control.addVariable("CH4ModPoisonMat","Gadolinium"); 

  Control.addVariable("CH4ModCH4Temp",110.0);     

  Control.addVariable("CH4ModViewSphere",25.0);  // sphere curvature  


  // Water Downstream Moderator:
  Control.addVariable("MerlinModXStep",0.0);      // guess
  Control.addVariable("MerlinModYStep",18.9-9.2);  //  relative to VoidVessel    
  Control.addVariable("MerlinModZStep",14.6);      // guess
  Control.addVariable("MerlinModXYangle",80.0);      // guess

  Control.addVariable("MerlinModWidth",12.5);    // guess
  Control.addVariable("MerlinModDepth",4.5);    // guess
  Control.addVariable("MerlinModHeight",12.0);    // guess
  Control.addVariable("MerlinModInnerThick",0.35);    // guess
  Control.addVariable("MerlinModVacThick",0.1);    // guess

  Control.addVariable("MerlinModVaneSide",0);      // No vanes
  Control.addVariable("MerlinModNPoison",1);       // ok
  Control.addVariable("MerlinModPoisonYStep1",0.00);      // ok
  Control.addVariable("MerlinModPoisonThick1",0.005);      //  

  Control.addVariable("MerlinModAlMat","Aluminium");       // Al 
  Control.addVariable("MerlinModWaterMat","H2O");      // water
  Control.addVariable("MerlinModPoisonMat","Gadolinium");      // Gadolinium

  // Water Upstream Moderator:
  Control.addVariable("WaterModXStep",0.0);      // guess
//  Control.addVariable("WaterModYStep",0.9);        // guess
  Control.addVariable("WaterModYStep",0.9-9.2);       //  relative to VoidVessel  
  Control.addVariable("WaterModZStep",14.6);      // guess
  Control.addVariable("WaterModXYangle",-90.0+32.0);      // guess

  Control.addVariable("WaterModWidth",12.5);    // guess
  Control.addVariable("WaterModDepth",4.5);    // guess
  Control.addVariable("WaterModHeight",12.0);    // guess
  Control.addVariable("WaterModInnerThick",0.35);    // guess
  Control.addVariable("WaterModVacThick",0.1);    // guess

  Control.addVariable("WaterModVaneSide",1);      // far side
  
  Control.addVariable("WaterModVanesNBlades",0);        // Number of blades
  Control.addVariable("WaterModVanesBladeWidth",0.3);   // Thickness [total]
  Control.addVariable("WaterModVanesBladeGap",0.2);     // Space of blade
  Control.addVariable("WaterModVanesAbsThick",0.05);    // Space in blade
  Control.addVariable("WaterModVanesYLength",1.5);      // Planeare
  Control.addVariable("WaterModVanesZLength",-1.0);     // -ve : to to bottom
  Control.addVariable("WaterModVanesXOffset",0.0);       
  Control.addVariable("WaterModVanesYOffset",0.0);       
  Control.addVariable("WaterModVanesZOffset",0.0);       
  Control.addVariable("WaterModVanesModTemp",0.0);     // Temperature

  Control.addVariable("WaterModVanesModMat","H2O");  // Water
  Control.addVariable("WaterModVanesBladeMat","SiCrystal");          // Silicon
  Control.addVariable("WaterModVanesAbsMat","Gadolinium");           // GD

  Control.addVariable("WaterModNPoison",2);      // ok
  Control.addVariable("WaterModPoisonYStep1",-0.75125);      // ok
  Control.addVariable("WaterModPoisonThick1",0.005);      // ok
  Control.addVariable("WaterModPoisonYStep2",0.75125);      // ok
  Control.addVariable("WaterModPoisonThick2",0.005);      // ok

  Control.addVariable("WaterModAlMat","Aluminium");       // Al 
  Control.addVariable("WaterModWaterMat","H2O");      // water
  Control.addVariable("WaterModPoisonMat","Gadolinium");      // Gadolinium

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
  Control.addVariable("H2FlightLinerMat1","Aluminium"); 
  Control.addVariable("H2FlightLinerMat2","Cadmium"); 

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
  Control.addVariable("CH4FlightSLinerThick1",0.8);    // Thickness
  Control.addVariable("CH4FlightSLinerThick2",0.673);   // Thickness
  Control.addVariable("CH4FlightSLinerMat1","Stainless304");        //  
  Control.addVariable("CH4FlightSLinerMat2","TS1Boral");        //  
  
  
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
  Control.addVariable("CH4FlightNLinerMat1","Aluminium");       
  Control.addVariable("CH4FlightNLinerMat2","Cadmium");       

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
  Control.addVariable("MerlinFlightLinerMat1","Aluminium");
  Control.addVariable("MerlinFlightLinerMat2","Cadmium"); 

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
  Control.addVariable("WatNorthFlightLinerMat1","Aluminium");
  Control.addVariable("WatNorthFlightLinerMat2","Cadmium");

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
  Control.addVariable("WatSouthFlightLinerMat1","Aluminium");
  Control.addVariable("WatSouthFlightLinerMat2","Cadmium");

  // Beam Window (PROTON FLIGHT PATH)
  Control.addVariable("BWindowRadius",4.0);          

//  Control.addVariable("BWindowHeThick",11.725);   // He Thickness
  Control.addVariable("BWindowYStep",-15.0);   // Step from front target
  Control.addVariable("BWindowIncThick1",0.3);   // Inconel Thickness 1
  Control.addVariable("BWindowWaterThick",0.1);   // Light water Thickness
  Control.addVariable("BWindowIncThick2",0.3);   // Inconel Thickness 2
  Control.addVariable("BWindowInconelMat","Inconnel");           // Inconel
  Control.addVariable("BWindowWaterMat","H2O");           // Light water

  

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

  Control.addVariable("MerlinPipeOuterMat","Aluminium");        
  Control.addVariable("MerlinPipeWaterMat","H2O");
  Control.addVariable("MerlinPipeSteelMat","Stainless304");


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

  Control.addVariable("H2PipeAlMat","Aluminium");        
  Control.addVariable("H2PipeSteelMat","Stainless304");                  
  Control.addVariable("H2PipeH2Mat","ParaOrtho%80");     
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

  Control.addVariable("WaterPipeOuterMat","Aluminium");        
  Control.addVariable("WaterPipeWaterMat","H2O");
  Control.addVariable("WaterPipeSteelMat","Stainless304");
  
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

  Control.addVariable("CH4PipeAlMat","Aluminium");        
  Control.addVariable("CH4PipeCH4Mat","CH4Liq110K");   // liquid methane at 110K
  Control.addVariable("CH4PipeCH4Temp",110.0);     // liquid methane at 110K            
  
  // RODS:
  Control.addVariable("RodsZAngle",0.0);
  Control.addVariable("RodsXYAngle",0.0);
  Control.addVariable("RodsCentSpace",3.0);
  Control.addVariable("RodsRadius",1.35);
  Control.addVariable("RodsLinerThick",0.15);
  Control.addVariable("RodsOuterMat","D2O%H2O%80.0");
  Control.addVariable("RodsInnerMat","Nickel");
  Control.addVariable("RodsLinerMat","Stainless304");


  return;
}



}  // NAMESPACE setVariable
