/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   t1Engineer/t1EngVariables.cxx
 *
 * Copyright (c) 2004-2015 by Stuart Ansell/Goran Skoro
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
TS1engineer(FuncDataBase& Control)
  /*!
    Function to set the control variables and constants
    -- This version is for TS1 (engineering version : non-model)
    \param Control :: Function data base to add constants too
  */
{
  ELog::RegMethod RegA("t1Uvariables[F]","TS1upgrade");

  TS1base(Control);
  TS1BulletTarget(Control);
  TS1CylTarget(Control);
  TS1CylFluxTrap(Control);
  
  TS1CylMod(Control);

  Control.addVariable("t1CylFluxTrapZOffset",0.0);    // Offset ref centre
  Control.addVariable("t1CylFluxTrapNCone",0);
  Control.addVariable("t1CylTargetFlangeRadius",6.0);
  
  Control.addVariable("sdefZOffset",0.0);            //Offset of source

  Control.addVariable("CylReflXStep",0.0);    
  Control.addVariable("CylReflYStep",10.0);    
  Control.addVariable("CylReflZStep",1.5);      
  Control.addVariable("CylReflXYAngle",0.0);    
  Control.addVariable("CylReflZAngle",0.0);    

  Control.addVariable("CylReflNLayer",1);    
  Control.addVariable("CylReflHeight1",35.0);    
  Control.addVariable("CylReflDepth1",35.0);    
  Control.addVariable("CylReflRadius1",45.0);    
  Control.addVariable("CylReflMat1","Be300K");    
  Control.addVariable("CylReflHeight2",35.0);    
  Control.addVariable("CylReflDepth2",35.0);    
  Control.addVariable("CylReflRadius2",45.0);    
  Control.addVariable("CylReflMat2","Lead");
  Control.addVariable("CylReflHeight3",35.0);    
  Control.addVariable("CylReflDepth3",35.0);    
  Control.addVariable("CylReflRadius3",45.0);    
  Control.addVariable("CylReflMat3","Be300K");          

     // Reflector (Eng new)
  Control.addVariable("EngReflXStep",0.0);    
  Control.addVariable("EngReflYStep",10.0);    
  Control.addVariable("EngReflZStep",1.5);      
  Control.addVariable("EngReflXYAngle",0.0);    
  Control.addVariable("EngReflZAngle",0.0);    
  Control.addVariable("EngReflRadius",45.0);
  Control.addVariable("EngReflWidth",80.0);          
  Control.addVariable("EngReflHeight",70.0); 
  Control.addVariable("EngReflTopCutHeight",36.0);         
  Control.addVariable("EngReflBotCutHeight",36.0);         
  Control.addVariable("EngReflCutLen",20.555);  
  Control.addVariable("EngReflCutAngle",50.0);  
  Control.addVariable("EngReflTopCCHeight",30.0);         
  Control.addVariable("EngReflCCoff",37.8);
  Control.addVariable("EngReflCutRadius",15.0);    
  Control.addVariable("EngReflReflMat","Be300K");         
  Control.addVariable("EngReflReflTemp",0.0);    

  Control.addVariable("EngReflLiftHeight",2.54);    
  Control.addVariable("EngReflLiftMat","Aluminium6061");   // - new version in !      
  Control.addVariable("EngReflLiftTemp",0.0);    

  Control.addVariable("EngReflCoolPadThick",2.5);   // 1.6 (original value) / cos(CutAngle)
  Control.addVariable("EngReflCoolPadOffset",7.55);  // 4.856 (original value) / cos(CutAngle)       
  Control.addVariable("EngReflCoolPadWidth",39.2);  // 25.2 (original value) / cos(CutAngle)
  Control.addVariable("EngReflCoolPadHeight",34.4);    
  Control.addVariable("EngReflCoolPadMat","Al5083Water");   // - new version in ! 
  Control.addVariable("EngReflCoolPadTemp",0.0);    

  Control.addVariable("EngReflInCutYOffset",2.00);
  Control.addVariable("EngReflInCutXOffset",-2.0); 
  Control.addVariable("EngReflInCutZOffset",-17.00);         
  Control.addVariable("EngReflInCutThick",30.0); 
  Control.addVariable("EngReflInCutWidth",5.0);
  Control.addVariable("EngReflInCutAngle",-8.0);  
  Control.addVariable("EngReflInCutHeight",14.0);    
  Control.addVariable("EngReflInCutMat","Void");
  Control.addVariable("EngReflInCutTemp",0.0);    

  Control.addVariable("EngReflInBWatCutYOffset",-5.00);
  Control.addVariable("EngReflInBWatCutXOffset",10.3); 
  Control.addVariable("EngReflInBWatCutZOffset",11.6);         
  Control.addVariable("EngReflInBWatCutHeight",13.2);    
  Control.addVariable("EngReflInBWatCutRadius",7.0);    
  Control.addVariable("EngReflInBWatCutMat","Void");
  Control.addVariable("EngReflInBWatCutTemp",0.0);      

  Control.addVariable("EngReflInSWatCutYOffset",-12.5);
  Control.addVariable("EngReflInSWatCutXOffset",17.3); 
  Control.addVariable("EngReflInSWatCutRadius",3.0);    

  // Beam Window (PROTON FLIGHT PATH)
  Control.addVariable("ProtonVoidViewRadius",4.8);          
  Control.addVariable("BWindowRadius",4.0);          
  Control.addVariable("BWindowHeThick",21.725);  // He Thickness
  Control.addVariable("BWindowIncThick1",0.3);  // Inconel Thickness 1
  Control.addVariable("BWindowWaterThick",0.1); // Light water Thickness
  Control.addVariable("BWindowIncThick2",0.3); // Inconel Thickness 2
  Control.addVariable("BWindowHeMat","helium");  // Helium at atmospheric pressure
  Control.addVariable("BWindowInconelMat","Inconnel");  
  Control.addVariable("BWindowWaterMat","H2O");   // Light water

  

  //
  // TRIMOD LAYER
  //
  Control.addVariable("TriModInnerNLayers",1); 
  //  Control.addVariable("TriModInnerThick1",0.5); 
  Control.addVariable("TriModInnerMat1","Void");
  Control.addVariable("TriModInnerTemp1",0.0);


  Control.addVariable("TriModLayerXStep",3.0);
  Control.addVariable("TriModLayerYStep",6.0);
  Control.addVariable("TriModLayerZStep",13.0);
  Control.addVariable("TriModLayerXYangle",-60.0);
  Control.addVariable("TriModLayerZangle",0.0);

  Control.addVariable("TriModLayerNLayer",6);   
  Control.addVariable("TriModLayerNPoison",1); 
  Control.addVariable("TriModLayerPYStep1",0.0);       // for 1 poison layer
  Control.addVariable("TriModLayerPYStep2",0.8);       // for 2 poison layer
  Control.addVariable("TriModLayerPGdThick",0.005);      //
  Control.addVariable("TriModLayerPCladThick",0.1);      //             
  Control.addVariable("TriModLayerPoisonMat","Gadolinium");      
  
  Control.addVariable("TriModLayerHeight1",12.6);   
  Control.addVariable("TriModLayerWidth1",30.0);   
  Control.addVariable("TriModLayerFront1",2.5);   
  Control.addVariable("TriModLayerBack1",-0.5);   

  // Void
  Control.addVariable("TriModLayerHeight1",12.6);   
  Control.addVariable("TriModLayerWidth1",30.0);   
  Control.addVariable("TriModLayerFront1",2.5);   
  Control.addVariable("TriModLayerBack1",-0.5);   
  Control.addVariable("TriModLayerMat1","Void");   

  // Al inner
  Control.addVariable("TriModLayerLayer2",0.3); 
  Control.addVariable("TriModLayerHeight2",0.0);        
  Control.addVariable("TriModLayerWidth2",0.6);
  Control.addVariable("TriModLayerFront2",0.3);   
  Control.addVariable("TriModLayerBack2",0.3);  
  Control.addVariable("TriModLayerTop2",0.0);   
  Control.addVariable("TriModLayerBase2",0.0);
  Control.addVariable("TriModLayerLeft2",0.3);   
  Control.addVariable("TriModLayerRight2",0.3);  
  Control.addVariable("TriModLayerMat2","Aluminium");   

  // Water
  Control.addVariable("TriModLayerLayer3",0.005);
  Control.addVariable("TriModLayerHeight3",0.0);   
  Control.addVariable("TriModLayerWidth3",0.0);    
  Control.addVariable("TriModLayerFront3",0.005);   
  Control.addVariable("TriModLayerBack3",0.0);   
  Control.addVariable("TriModLayerMat3","Gadolinium");   


  // Al layer
  Control.addVariable("TriModLayerLayer4",2.6);   
  Control.addVariable("TriModLayerHeight4",0.0);   
  Control.addVariable("TriModLayerWidth4",4.0);
  Control.addVariable("TriModLayerFront4",1.495);   
  Control.addVariable("TriModLayerBack4",4.5);        
  Control.addVariable("TriModLayerMat4","H2O");     

  // Void
  Control.addVariable("TriModLayerLayer5",0.3);
  Control.addVariable("TriModLayerHeight5",0.6);   
  Control.addVariable("TriModLayerWidth5",0.6);
  Control.addVariable("TriModLayerFront5",0.3);   
  Control.addVariable("TriModLayerBack5",0.3);  
  Control.addVariable("TriModLayerTop5",0.3);   
  Control.addVariable("TriModLayerBase5",0.3);
  Control.addVariable("TriModLayerLeft5",0.3);   
  Control.addVariable("TriModLayerRight5",0.3);  
  Control.addVariable("TriModLayerMat5","Aluminium");     

  // Decoupler Al
  Control.addVariable("TriModLayerLayer6",0.6);
  Control.addVariable("TriModLayerHeight6",1.2);   
  Control.addVariable("TriModLayerWidth6",1.2);     
  Control.addVariable("TriModLayerFront6",0.6);   
  Control.addVariable("TriModLayerBack6",0.6);
  Control.addVariable("TriModLayerTop6",0.6);   
  Control.addVariable("TriModLayerBase6",0.6);
  Control.addVariable("TriModLayerLeft6",0.6);   
  Control.addVariable("TriModLayerRight6",0.6);       
  Control.addVariable("TriModLayerMat6","Void");    
  
  // Decoupler Cd 
  Control.addVariable("TriModLayerLayer7",0.12);
  Control.addVariable("TriModLayerHeight7",0.24);   
  Control.addVariable("TriModLayerWidth7",0.24);     
  Control.addVariable("TriModLayerFront7",0.0);   
  Control.addVariable("TriModLayerBack7",0.0);
  Control.addVariable("TriModLayerTop7",0.12);   
  Control.addVariable("TriModLayerBase7",0.0);
  Control.addVariable("TriModLayerLeft7",0.12);   
  Control.addVariable("TriModLayerRight7",0.12);          
  Control.addVariable("TriModLayerMat7","Cadmium");     

  // Outer vac layer 
  Control.addVariable("TriModLayerLayer8",0.6);   
  Control.addVariable("TriModLayerMat8","Void");     

  Control.addVariable("TriModLayerNFlight",2);
  Control.addVariable("TriModLayerFlightSide0",1);
  Control.addVariable("TriModLayerFlightSide1",2);
  
  // FLIGHTLINES : 
  Control.addVariable("TriFlightASideIndex",8);     
  Control.addVariable("TriFlightAXStep",0.0);      // Step from centre  
  Control.addVariable("TriFlightAZStep",0.7);      // Step from centre  
  Control.addVariable("TriFlightAAngleXY1",25.0);  // Angle out
  Control.addVariable("TriFlightAAngleXY2",0.0);  // Angle out
  Control.addVariable("TriFlightAAngleZTop",0.0);  // Step down angle
  Control.addVariable("TriFlightAAngleZBase",0.0); // Step up angle
  Control.addVariable("TriFlightAHeight",10.0);     // Full height
  Control.addVariable("TriFlightAWidth",33.5);     // Full width
  Control.addVariable("TriFlightANLiner",2);           // Number of layers
  Control.addVariable("TriFlightALinerThick1",0.5);    // Thickness
  Control.addVariable("TriFlightALinerThick2",0.12);   // Thickness
  Control.addVariable("TriFlightALinerMat1","Aluminium");
  Control.addVariable("TriFlightALinerMat2","Cadmium");  

  Control.addVariable("TriFlightBSideIndex",3);      // Surface
  Control.addVariable("TriFlightBXStep",0.0);        // Step from centre  
  Control.addVariable("TriFlightBZStep",0.5);        // Step from centre  
  Control.addVariable("TriFlightBAngleXY1",10.0);     // Angle out [lower]
  Control.addVariable("TriFlightBAngleXY2",35.0);   // Angle out [top]
  Control.addVariable("TriFlightBAngleZTop",0.0);    // Step down angle
  Control.addVariable("TriFlightBAngleZBase",0.0);   // Step up angle
  Control.addVariable("TriFlightBHeight",10.0);      // Full height
  Control.addVariable("TriFlightBWidth",33.5);       // Full width  
  Control.addVariable("TriFlightBNLiner",2);           // Number of layers
  Control.addVariable("TriFlightBLinerThick1",0.5);    // Thickness
  Control.addVariable("TriFlightBLinerThick2",0.12);    // Thickness
  Control.addVariable("TriFlightBLinerMat1","Aluminium");        //  Alluminut 
  Control.addVariable("TriFlightBLinerMat2","Cadmium");        //  Cadmium

  // COLD CENTRAL OBJECT
  Control.addVariable("ColdCentXStep",0.0);  
  Control.addVariable("ColdCentYStep",-1.5);  // interim default      
  Control.addVariable("ColdCentZStep",-1.85);            
  Control.addVariable("ColdCentXYAngle",0.0);     
  Control.addVariable("ColdCentZAngle",0.0);     

  //
  // METHANE
  // 
  Control.addVariable("CH4ModXStep",0.5);      
  Control.addVariable("CH4ModYStep",4.3);  // +9.2    
  Control.addVariable("CH4ModZStep",-13.4);     
  Control.addVariable("CH4ModXYangle",60.0);     
  Control.addVariable("CH4ModZangle",0.0);     

 
  Control.addVariable("CH4ModNLayer",7);     
  Control.addVariable("CH4ModDepth1",4.0);     
  Control.addVariable("CH4ModWidth1",14.4);   
  Control.addVariable("CH4ModHeight1",10.86); // was 11.0 in Mark6b - CHECK   

  Control.addVariable("CH4ModFrontRadius1",1000.0);  // 2 x value in the drawings - Ok for now - CHECK 
  Control.addVariable("CH4ModFrontRadius2",1000.0);  
  Control.addVariable("CH4ModFrontRadius3",79.6);  
  Control.addVariable("CH4ModFrontRadius4",79.6);  
  Control.addVariable("CH4ModFrontRadius5",80.2);  
  Control.addVariable("CH4ModFrontRadius6",80.2);  
  Control.addVariable("CH4ModFrontRadius7",80.2); // 
 
  Control.addVariable("CH4ModBackRadius1",1000.0);  
  Control.addVariable("CH4ModBackRadius2",1000.0);  
  Control.addVariable("CH4ModBackRadius3",79.6);  
  Control.addVariable("CH4ModBackRadius4",79.6);  
  Control.addVariable("CH4ModBackRadius5",80.2);  
  Control.addVariable("CH4ModBackRadius6",80.2);  
  Control.addVariable("CH4ModBackRadius7",80.2); //
   
  Control.addVariable("CH4ModRound1",1.2);    
  Control.addVariable("CH4ModRound2",1.6);  
  Control.addVariable("CH4ModRound3",1.85);  
  Control.addVariable("CH4ModRound4",2.1);    
  Control.addVariable("CH4ModRound5",2.2);  
  Control.addVariable("CH4ModRound6",2.4);  
  
  Control.addVariable("CH4ModLayer2",0.37);       // CH4 can

  Control.addVariable("CH4ModHeight3",0.35);   
  Control.addVariable("CH4ModLayer3",0.25);       // VacGap

  Control.addVariable("CH4ModLayer4",0.2);   // Mid Al

  Control.addVariable("CH4ModHeight5",0.25);   
  Control.addVariable("CH4ModLayer5",0.1);      // Tert Gap

  Control.addVariable("CH4ModLayer6",0.2);    // Outer Al

  Control.addVariable("CH4ModTop7",0.5);
  Control.addVariable("CH4ModLayer7",0.4);       // outer vac - clearance gap -- CHECK

  Control.addVariable("CH4ModMat1","CH4Liq110K");      
  Control.addVariable("CH4ModMat2","Aluminium");  
  Control.addVariable("CH4ModMat3","Void");       
  Control.addVariable("CH4ModMat4","Aluminium");   
  Control.addVariable("CH4ModMat5","Void");        
  Control.addVariable("CH4ModMat6","Aluminium");   
  Control.addVariable("CH4ModMat7","Void");        


  Control.addVariable("CH4ModTemp1",110.0);            // liquid CH4
  Control.addVariable("CH4ModTemp2",110.0);            // liquid CH4

  // Poison !!!!  
  Control.addVariable("CH4ModInnerNPoison",1);      // 
  Control.addVariable("CH4ModInnerPYStep1",0.0);       // for 1 poison layer
  Control.addVariable("CH4ModInnerPYStep2",0.8);       // for 2 poison layer
  Control.addVariable("CH4ModInnerPGdThick",0.005);      //
  Control.addVariable("CH4ModInnerPCladThick",0.048);      //      
     
  Control.addVariable("CH4ModInnerPoisonMat","Gadolinium");      // Gadolinium
  Control.addVariable("CH4ModInnerPCladMat","Aluminium");   // Al    

  // CH4 Premod Flat 
  Control.addVariable("CH4PreFlatXStep",0.15);      
  Control.addVariable("CH4PreFlatYStep",0.0);  
  Control.addVariable("CH4PreFlatZStep",0.3);     
  Control.addVariable("CH4PreFlatXYangle",0.0);    
  Control.addVariable("CH4PreFlatZangle",0.0);  

  Control.addVariable("CH4PreFlatDepth",11.3);   
  Control.addVariable("CH4PreFlatWidth",11.3);   
  Control.addVariable("CH4PreFlatHeight",1.0);   
  Control.addVariable("CH4PreFlatAlThick",0.3);   
  Control.addVariable("CH4PreFlatVacThick",0.4);   
  Control.addVariable("CH4PreFlatAlMat","Aluminium");            // Al 
  Control.addVariable("CH4PreFlatModMat","H2O");            // Al 
  Control.addVariable("CH4PreFlatModTemp",300);            // Al 

  // FLIGHTLINES : 
  Control.addVariable("CH4FlightASideIndex",1);      // Full width
  Control.addVariable("CH4FlightAXStep",0.0);       // Step from centre  
  Control.addVariable("CH4FlightAZStep",0.55);        // Step from centre  
  Control.addVariable("CH4FlightAAngleXY1",23.0);   // Angle out
  Control.addVariable("CH4FlightAAngleXY2",40.0);     // Angle out
  Control.addVariable("CH4FlightAAngleZTop",0.0);    // Step down angle
  Control.addVariable("CH4FlightAAngleZBase",0.0);   // Step up angle
  Control.addVariable("CH4FlightAHeight",10.0);      // Full height
  Control.addVariable("CH4FlightAWidth",15.5);        // Full width
  Control.addVariable("CH4FlightANLiner",2);         // Number of layers
  Control.addVariable("CH4FlightALinerThick1",0.5);  // Thickness
  Control.addVariable("CH4FlightALinerThick2",0.12); // Thickness
  Control.addVariable("CH4FlightALinerMat1","Aluminium");
  Control.addVariable("CH4FlightALinerMat2","Cadmium");  
// FLIGHTLINES : 
  Control.addVariable("CH4FlightBSideIndex",2);      // Full width
  Control.addVariable("CH4FlightBXStep",-1.0);       // Step from centre
  Control.addVariable("CH4FlightBZStep",0.55);        // Step from centre  
  Control.addVariable("CH4FlightBAngleXY1",23.0);   // Angle out
  Control.addVariable("CH4FlightBAngleXY2",20.0);     // Angle out
  Control.addVariable("CH4FlightBAngleZTop",0.0);    // Step down angle
  Control.addVariable("CH4FlightBAngleZBase",0.0);   // Step up angle
  Control.addVariable("CH4FlightBHeight",10.0);      // Full height
  Control.addVariable("CH4FlightBWidth",13.5);        // Full width  
  Control.addVariable("CH4FlightBNLiner",2);         // Number of layers
  Control.addVariable("CH4FlightBLinerThick1",0.5);  // Thickness
  Control.addVariable("CH4FlightBLinerThick2",0.12); // Thickness
  Control.addVariable("CH4FlightBLinerMat1","Aluminium");
  Control.addVariable("CH4FlightBLinerMat2","Cadmium");  

  //
  // HYDROGEN LAYER
  //
  Control.addVariable("H2LayerXStep",2.77);
  Control.addVariable("H2LayerYStep",20.74);  // +9.2
  Control.addVariable("H2LayerZStep",-12.2);
  Control.addVariable("H2LayerXYangle",100.0);
  Control.addVariable("H2LayerZangle",0.0);


  Control.addVariable("H2LayerNPoison",0);
  Control.addVariable("H2LayerPYStep1",0.0);       // for 1 poison layer
  Control.addVariable("H2LayerPYStep2",0.8);       // for 2 poison layer
  Control.addVariable("H2LayerPGdThick",0.005);      //
  Control.addVariable("H2LayerPCladThick",0.1);      //
  Control.addVariable("H2LayerPCladMat","Aluminium");
  Control.addVariable("H2LayerPoisonMat","Gadolinium");


  Control.addVariable("H2LayerNLayer",7);  
  // H2
  Control.addVariable("H2LayerDepth1",4.0);
  Control.addVariable("H2LayerWidth1",13.35);
  Control.addVariable("H2LayerHeight1",12.6);
  Control.addVariable("H2LayerMat1","ParaOrtho%80");
  Control.addVariable("H2LayerTemp1",20.0);   
  
  // Al Can
  Control.addVariable("H2LayerLayer2",0.6);
  Control.addVariable("H2LayerMat2","Aluminium");
  Control.addVariable("H2LayerTemp2",20.0);   
  
  // vac
  Control.addVariable("H2LayerHeight3",0.35);
  Control.addVariable("H2LayerLayer3",0.25);
  Control.addVariable("H2LayerMat3","Void");

  // Al layer
  Control.addVariable("H2LayerLayer4",0.2);
  Control.addVariable("H2LayerMat4","Aluminium");

  // Tert gap
  Control.addVariable("H2LayerHeight5",0.25);
  Control.addVariable("H2LayerLayer5",0.1);
  Control.addVariable("H2LayerMat5","Void");

  // Tert can Al
  Control.addVariable("H2LayerLayer6",0.2);
  Control.addVariable("H2LayerMat6","Aluminium");

  // clearance gap
  Control.addVariable("H2LayerLayer7",0.4); /// check
  Control.addVariable("H2LayerMat7","Void");

  Control.addVariable("H2InnerNPoison",0);      // 
  
  Control.addVariable("H2LayerRadius1",1000.0);  
  Control.addVariable("H2LayerRadius2",1000.0);  
  Control.addVariable("H2LayerRadius3",40.4);  // value in the drawings - Ok for now - CHECK  
  Control.addVariable("H2LayerRadius4",40.4);  
  Control.addVariable("H2LayerRadius5",40.8); 
  Control.addVariable("H2LayerRadius6",40.8);
  Control.addVariable("H2LayerRadius7",60.0);
       
  Control.addVariable("H2LayerRound1",1.0);    
  Control.addVariable("H2LayerRound2",1.5);  
  Control.addVariable("H2LayerRound3",2.05);  
  Control.addVariable("H2LayerRound4",2.3);  
  Control.addVariable("H2LayerRound5",2.4);    
  Control.addVariable("H2LayerRound6",2.6);    
  
  // FLIGHTLINES : 
  Control.addVariable("H2FlightSideIndex",2);      // Full width
  Control.addVariable("H2FlightXStep",0.0);       // Step from centre
  Control.addVariable("H2FlightZStep",-0.65);        // Step from centre  
  Control.addVariable("H2FlightAngleXY1",20.0);   // Angle out  
  Control.addVariable("H2FlightAngleXY2",23.0);     // Angle out
  Control.addVariable("H2FlightAngleZTop",0.0);    // Step down angle
  Control.addVariable("H2FlightAngleZBase",0.0);   // Step up angle
  Control.addVariable("H2FlightHeight",10.0);      // Full height
  Control.addVariable("H2FlightWidth",13.5);        // Full width
  Control.addVariable("H2FlightNLiner",2);         // Number of layers
  Control.addVariable("H2FlightLinerThick1",0.5);  // Thickness
  Control.addVariable("H2FlightLinerThick2",0.12); // Thickness
  Control.addVariable("H2FlightLinerMat1","Aluminium");    
  Control.addVariable("H2FlightLinerMat2","Cadmium");      

  // H2 Premod Flat 
  Control.addVariable("HPreModXStep",0.0);      
  Control.addVariable("HPreModYStep",-5.65);  
  Control.addVariable("HPreModZStep",0.0);     
  Control.addVariable("HPreModXYangle",0.0);    
  Control.addVariable("HPreModZangle",0.0);  

  Control.addVariable("HPreModDepth",1.5);   
  Control.addVariable("HPreModWidth",15.55);   
  Control.addVariable("HPreModHeight",15.2);   
  Control.addVariable("HPreModAlThick",0.3);   
  Control.addVariable("HPreModVacThick",0.4);   // clearance gap
  Control.addVariable("HPreModAlMat","Aluminium");            // Al 
  Control.addVariable("HPreModModMat","H2O");            // Al 
  Control.addVariable("HPreModModTemp",300);            // Al 

  //
  // PIPES
  //
  Control.addVariable("H2PipeEngNSegIn",1);
  // Central point:
  Control.addVariable("H2PipeEngPPt0",Geometry::Vec3D(0,-10.8,0.0));
  Control.addVariable("H2PipeEngPPt1",Geometry::Vec3D(0,19.25,0.0));
  // Central point [Top]:

  Control.addVariable("H2PipeEngNRadii",8);
  Control.addVariable("H2PipeEngRadius0",0.6);
  Control.addVariable("H2PipeEngRadius1",0.7);
  Control.addVariable("H2PipeEngRadius2",1.25);
  Control.addVariable("H2PipeEngRadius3",1.45);
  Control.addVariable("H2PipeEngRadius4",2.25);
  Control.addVariable("H2PipeEngRadius5",2.415);
  Control.addVariable("H2PipeEngRadius6",2.85);
  Control.addVariable("H2PipeEngRadius7",3.015);
 
  Control.addVariable("H2PipeEngMat0","ParaOrtho%80");
  Control.addVariable("H2PipeEngMat1","Aluminium");
  Control.addVariable("H2PipeEngMat2","ParaOrtho%80");
  Control.addVariable("H2PipeEngMat3","Aluminium");
  Control.addVariable("H2PipeEngMat4","Void");
  Control.addVariable("H2PipeEngMat5","Aluminium");
  Control.addVariable("H2PipeEngMat6","Void");
  Control.addVariable("H2PipeEngMat7","Aluminium");

  Control.addVariable("H2PipeEngTemp0",20.0);
  Control.addVariable("H2PipeEngTemp1",20.0);
  Control.addVariable("H2PipeEngTemp2",20.0);
  Control.addVariable("H2PipeEngTemp3",0.0);

  Control.addVariable("H2PipeEngActive0",3);
  Control.addVariable("H2PipeEngActive1",7);
  Control.addVariable("H2PipeEngActive2",15);
  Control.addVariable("H2PipeEngActive3",31);
  Control.addVariable("H2PipeEngActive4",63);
  Control.addVariable("H2PipeEngActive5",127);
  Control.addVariable("H2PipeEngActive6",255);

  Control.addVariable("WaterPipeNSegIn",1);
  // Central point:
  Control.addVariable("WaterPipePPt0",Geometry::Vec3D(0.0,-1.0,0.0));
  Control.addVariable("WaterPipePPt1",Geometry::Vec3D(0.0,9.25,0.0));
  // Central point [Top]:

  Control.addVariable("WaterPipeNRadii",1);
  Control.addVariable("WaterPipeRadius0",1.5);
  Control.addVariable("WaterPipeRadius1",1.7);
  Control.addVariable("WaterPipeRadius2",1.8);

  Control.addVariable("WaterPipeMat0","H2O");
  Control.addVariable("WaterPipeMat1","Aluminium");
  Control.addVariable("WaterPipeMat2","Void");

  Control.addVariable("WaterPipeTemp0",0.0);
  Control.addVariable("WaterPipeTemp1",0.0);
  Control.addVariable("WaterPipeTemp2",0.0);
  

  Control.addVariable("WaterReturnNSegIn",1);
  // Central point:
  Control.addVariable("WaterReturnPPt0",Geometry::Vec3D(-12.0,-1.0,-4.0));
  Control.addVariable("WaterReturnPPt1",Geometry::Vec3D(-12.0,29.25,-4.0));
  // Central point [Top]:

  Control.addVariable("WaterReturnNRadii",3);
  Control.addVariable("WaterReturnRadius0",1.5);
  Control.addVariable("WaterReturnRadius1",1.7);
  Control.addVariable("WaterReturnRadius2",2.0);

  Control.addVariable("WaterReturnMat0","H2O");
  Control.addVariable("WaterReturnMat1","Aluminium");
  Control.addVariable("WaterReturnMat2","Void");

  Control.addVariable("WaterReturnTemp0",0.0);
  Control.addVariable("WaterReturnTemp1",0.0);
  Control.addVariable("WaterReturnTemp2",0.0);

  Control.addVariable("WaterReturnActive0",3);
  Control.addVariable("WaterReturnActive1",3);



  Control.addVariable("CH4PipeEngNSegIn",1);
  // Central point:
  Control.addVariable("CH4PipeEngPPt0",Geometry::Vec3D(0.0,-2.2,0.0));
  Control.addVariable("CH4PipeEngPPt1",Geometry::Vec3D(0.0,-30.0,0.0));
  // Central point [Top]:

  // NUMBER FROM TS1 baseline model
  Control.addVariable("CH4PipeEngNRadii",8);
  Control.addVariable("CH4PipeEngRadius0",0.6);     // inner
  Control.addVariable("CH4PipeEngRadius1",0.7);     // sep al
  Control.addVariable("CH4PipeEngRadius2",1.25);     // return
  Control.addVariable("CH4PipeEngRadius3",1.45);     // Al outer
  Control.addVariable("CH4PipeEngRadius4",2.25);     // vac
  Control.addVariable("CH4PipeEngRadius5",2.415);     // vac al [1.905 orig]
  Control.addVariable("CH4PipeEngRadius6",2.85);     // clearacne
  Control.addVariable("CH4PipeEngRadius7",3.015);     // clearacne
  
  Control.addVariable("CH4PipeEngMat0","CH4Liq110K");
  Control.addVariable("CH4PipeEngMat1","Aluminium");
  Control.addVariable("CH4PipeEngMat2","CH4Liq110K");
  Control.addVariable("CH4PipeEngMat3","Aluminium");
  Control.addVariable("CH4PipeEngMat4","Void");
  Control.addVariable("CH4PipeEngMat5","Aluminium");
  Control.addVariable("CH4PipeEngMat6","Void");
  Control.addVariable("CH4PipeEngMat7","Aluminium");
  
  Control.addVariable("CH4PipeEngTemp0",110.0);
  Control.addVariable("CH4PipeEngTemp1",110.0);
  Control.addVariable("CH4PipeEngTemp2",110.0);
  Control.addVariable("CH4PipeEngTemp3",110.0);

  Control.addVariable("CH4PipeEngActive0",3);
  Control.addVariable("CH4PipeEngActive1",7);
  Control.addVariable("CH4PipeEngActive2",15);
  Control.addVariable("CH4PipeEngActive3",31);
  Control.addVariable("CH4PipeEngActive4",63);
  Control.addVariable("CH4PipeEngActive5",127);
  Control.addVariable("CH4PipeEngActive6",255);

  
  return;
}

}  // NAMESPACE setVariable
