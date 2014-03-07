/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   t1Upgrade/t1Uvariables.cxx
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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

void TS1CylMod(FuncDataBase& Control)
  /*
    Set the variables for the lower moderators
    \param Control :: DataBase to put variables
   */
{
  Control.addVariable("H2CylModXStep",6.0);  
  Control.addVariable("H2CylModYStep",20.0);  
  Control.addVariable("H2CylModZStep",-13.2);
  Control.addVariable("H2CylModXYangle",91.5);
  Control.addVariable("H2CylModZangle",0.0);
  Control.addVariable("H2CylModRadius",6.0);
  Control.addVariable("H2CylModHeight",12.0);
  Control.addVariable("H2CylModMat","ParaH2");
  Control.addVariable("H2CylModTemp",20.0);
  Control.addVariable("H2CylModNLayers",6);
  // al layer
  Control.addVariable("H2CylModHGap1",0.3);
  Control.addVariable("H2CylModRadGap1",0.3);
  Control.addVariable("H2CylModMaterial1","Aluminium");  // Al materk
  Control.addVariable("H2CylModTemp1",20.0);  
  // Vac gap
  Control.addVariable("H2CylModHGap2",0.5);
  Control.addVariable("H2CylModRadGap2",0.5);
  Control.addVariable("H2CylModMaterial2","Void"); 
  // Next Al layer
  Control.addVariable("H2CylModHGap3",0.2);
  Control.addVariable("H2CylModRadGap3",0.2);
  Control.addVariable("H2CylModMaterial3","Aluminium"); 
  Control.addVariable("H2CylModTemp3",77.0);  
  // He Layer
  Control.addVariable("H2CylModHGap4",0.2);
  Control.addVariable("H2CylModRadGap4",0.2);
  Control.addVariable("H2CylModMaterial4","Void"); 
  // Outer Layer
  Control.addVariable("H2CylModHGap5",0.2);
  Control.addVariable("H2CylModRadGap5",0.2);
  Control.addVariable("H2CylModMaterial5","Aluminium"); 
  Control.addVariable("H2CylModTemp5",300.0); 
  // Clearance
  Control.addVariable("H2CylModHGap6",0.2);
  Control.addVariable("H2CylModRadGap6",0.2);
  Control.addVariable("H2CylModMaterial6","Void"); 

  Control.addVariable("H2CylModNConic",4);
  Control.addVariable("H2CylModConic1Cent",Geometry::Vec3D(0,1,0));
  Control.addVariable("H2CylModConic1Axis",Geometry::Vec3D(0,1,0));
  Control.addVariable("H2CylModConic1Angle",35.0);
  Control.addVariable("H2CylModConic1Mat","Void");
  Control.addVariable("H2CylModConic1WallMat","Aluminium");
  Control.addVariable("H2CylModConic1Wall",0.2);

  Control.addVariable("H2CylModConic2Cent",Geometry::Vec3D(1,0,0));
  Control.addVariable("H2CylModConic2Axis",Geometry::Vec3D(1,0,0));
  Control.addVariable("H2CylModConic2Angle",35.0);
  Control.addVariable("H2CylModConic2Mat","Void");
  Control.addVariable("H2CylModConic2WallMat","Aluminium");
  Control.addVariable("H2CylModConic2Wall",0.2);

  Control.addVariable("H2CylModConic3Cent",Geometry::Vec3D(-1,0,0));
  Control.addVariable("H2CylModConic3Axis",Geometry::Vec3D(-1,0,0));
  Control.addVariable("H2CylModConic3Angle",35.0);
  Control.addVariable("H2CylModConic3Mat","Void");
  Control.addVariable("H2CylModConic3WallMat","Aluminium");
  Control.addVariable("H2CylModConic3Wall",0.2);

  Control.addVariable("H2CylModConic4Cent",Geometry::Vec3D(0,-1,0));
  Control.addVariable("H2CylModConic4Axis",Geometry::Vec3D(0,-1,0));
  Control.addVariable("H2CylModConic4Angle",35.0);
  Control.addVariable("H2CylModConic4Mat","Void");
  Control.addVariable("H2CylModConic4WallMat","Aluminium");
  Control.addVariable("H2CylModConic4Wall",0.2);

  Control.addVariable("H2CylPreNLayers",5);  
  Control.addVariable("H2CylPreHeight1",0.3);  
  Control.addVariable("H2CylPreDepth1",0.3);  
  Control.addVariable("H2CylPreThick1",0.3);  
  Control.addVariable("H2CylPreMaterial1","Void");  
  Control.addVariable("H2CylPreHeight2",0.2);  
  Control.addVariable("H2CylPreDepth2",0.2);  
  Control.addVariable("H2CylPreThick2",0.2);  
  Control.addVariable("H2CylPreMaterial2","Aluminium");  
  Control.addVariable("H2CylPreHeight3",1.0);  
  Control.addVariable("H2CylPreDepth3",2.0);  
  Control.addVariable("H2CylPreThick3",2.0);  
  Control.addVariable("H2CylPreMaterial3","H2O");  
  Control.addVariable("H2CylPreHeight4",0.2);  
  Control.addVariable("H2CylPreDepth4",0.2);  
  Control.addVariable("H2CylPreThick4",0.2);  
  Control.addVariable("H2CylPreMaterial4","Aluminium");  
  Control.addVariable("H2CylPreHeight5",0.2);  
  Control.addVariable("H2CylPreDepth5",0.2);  
  Control.addVariable("H2CylPreThick5",0.2);  
  Control.addVariable("H2CylPreMaterial5","Void"); 

  return;
}

void
TS1upgrade(FuncDataBase& Control)
  /*!
    Function to set the control variables and constants
    -- This version is for TS1 (real version : non-model)
    \param Control :: Function data base to add constants too
  */
{
  ELog::RegMethod RegA("t1Uvariables[F]","TS1upgrade");

  TS1base(Control);
  TS1CylTarget(Control);
  TS1CylFluxTrap(Control);
  TS1PlateTarget(Control);
  TS1InnerTarget(Control);
  TS1Cannelloni(Control);
  TS1BlockTarget(Control);
  TS1EllipticCylTarget(Control);
  
  TS1CylMod(Control);

  Control.addVariable("t1CylFluxTrapZOffset",0.0);    // Offset ref centre
  Control.addVariable("t1CylTargetZOffset",0.0);    // Offset ref centre  
  Control.addVariable("t1EllCylTargetZStep",0.0);    // Offset ref centre
  Control.addVariable("t1CylTargetMainLength",42.5);    // Offset ref centre
  Control.addVariable("t1InnerZStep",1.85);    // Offset ref centre
  Control.addVariable("t1CannelloniZStep",1.85);    // Offset ref centre
  Control.addVariable("sdefZOffset",1.85);            //Offset of source

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

  // Beam Window (PROTON FLIGHT PATH)
  Control.addVariable("ProtonVoidViewRadius",4.0);          
  Control.addVariable("BWindowRadius",4.0);          
  Control.addVariable("BWindowHeThick",21.725);  // He Thickness
  Control.addVariable("BWindowIncThick1",0.3);  // Inconel Thickness 1
  Control.addVariable("BWindowWaterThick",0.1); // Light water Thickness
  Control.addVariable("BWindowIncThick2",0.3); // Inconel Thickness 2
  Control.addVariable("BWindowHeMat","helium");  // Helium at atmospheric pressure
  Control.addVariable("BWindowInconelMat","Inconnel");  
  Control.addVariable("BWindowWaterMat","H2O");   // Light water


  // WATER MODERATOR
  Control.addVariable("TriModXStep",5.0);
  Control.addVariable("TriModYStep",17.0);
  Control.addVariable("TriModZStep",12.70);
  Control.addVariable("TriModXYangle",75.0);
  Control.addVariable("TriModZangle",0.0);

  Control.addVariable("TriModNCorner",4); 
  Control.addVariable("TriModCorner1",Geometry::Vec3D(12.8,12.8,0.0)); 
  Control.addVariable("TriModCorner4",Geometry::Vec3D(-8.0,8.0,0.0)); 
  Control.addVariable("TriModCorner2",Geometry::Vec3D(-12.8,-12.8,0.0)); 
  Control.addVariable("TriModCorner3",Geometry::Vec3D(-24.0,0.0,0.0)); 

  /*
  Control.addVariable("TriModNCorner",4); 
  Control.addVariable("TriModCorner1",Geometry::Vec3D(8.0,8.0,0.0)); 
  Control.addVariable("TriModCorner2",Geometry::Vec3D(8.0,-8.0,0.0)); 
  Control.addVariable("TriModCorner3",Geometry::Vec3D(-8.0,-8.0,0.0)); 
  Control.addVariable("TriModCorner4",Geometry::Vec3D(-8.0,8.0,0.0)); 
  */
  Control.addVariable("TriModHeight",12.0);
  Control.addVariable("TriModWallThick",0.5);
  Control.addVariable("TriModInnerWall",0.5);
  Control.addVariable("TriModFlatClearance",0.6);
  Control.addVariable("TriModBaseClearance",0.3);
  Control.addVariable("TriModTopClearance",0.6);
  Control.addVariable("TriModInnerStep",-1.2);  // from 1.2
  // Poison !!!!  
  Control.addVariable("TriModPoisonFlag",1);      // Activate poison layer
  Control.addVariable("TriModPoisonStep",7.0);  
  Control.addVariable("TriModPoisonThick",0.005);
  Control.addVariable("TriModPCladThick",0.1);    

  Control.addVariable("TriModPCladMat","Aluminium");        // Al 
  Control.addVariable("TriModPoisonMat","Gadolinium");      // Gadolinium  
  Control.addVariable("TriModInnerMat","H2O");
  Control.addVariable("TriModModMat","H2O");
  Control.addVariable("TriModWallMat","Aluminium");
  Control.addVariable("TriModModTemp",300.0);
  
  // FLIGHTLINES : 
  Control.addVariable("TriFlightASideIndex",3);     // Full width
  Control.addVariable("TriFlightAXStep",-5.0);      // Step from centre  
  Control.addVariable("TriFlightAZStep",0.5);      // Step from centre  
  Control.addVariable("TriFlightAAngleXY1",72.0);  // Angle out
  Control.addVariable("TriFlightAAngleXY2",-27.0);  // Angle out
  Control.addVariable("TriFlightAAngleZTop",0.0);  // Step down angle
  Control.addVariable("TriFlightAAngleZBase",0.0); // Step up angle
  Control.addVariable("TriFlightAHeight",10.0);     // Full height
  Control.addVariable("TriFlightAWidth",28.0);     // Full width
  Control.addVariable("TriFlightANLiner",2);           // Number of layers
  Control.addVariable("TriFlightALinerThick1",0.5);    // Thickness
  Control.addVariable("TriFlightALinerThick2",0.12);   // Thickness
  Control.addVariable("TriFlightALinerMat1","Aluminium");
  Control.addVariable("TriFlightALinerMat2","Cadmium");  

  Control.addVariable("TriFlightBSideIndex",1);      // Surface
  Control.addVariable("TriFlightBXStep",5.3);        // Step from centre  
  Control.addVariable("TriFlightBZStep",0.5);        // Step from centre  
  Control.addVariable("TriFlightBAngleXY1",10.0);     // Angle out [lower]
  Control.addVariable("TriFlightBAngleXY2",65.0);   // Angle out [top]
  Control.addVariable("TriFlightBAngleZTop",0.0);    // Step down angle
  Control.addVariable("TriFlightBAngleZBase",0.0);   // Step up angle
  Control.addVariable("TriFlightBHeight",10.0);      // Full height
  Control.addVariable("TriFlightBWidth",22.0);       // Full width  
  Control.addVariable("TriFlightBNLiner",2);           // Number of layers
  Control.addVariable("TriFlightBLinerThick1",0.5);    // Thickness
  Control.addVariable("TriFlightBLinerThick2",0.12);    // Thickness
  Control.addVariable("TriFlightBLinerMat1","Aluminium");        //  Alluminut 
  Control.addVariable("TriFlightBLinerMat2","Cadmium");        //  Cadmium

  // COLD CENTRAL OBJECT
  Control.addVariable("ColdCentXStep",3.0);  
  Control.addVariable("ColdCentYStep",-1.5);  // interim default      
  Control.addVariable("ColdCentZStep",-1.85);            
  Control.addVariable("ColdCentXYAngle",0.0);     
  Control.addVariable("ColdCentZAngle",0.0);     
  //
  // HYDROGEN 
  // 
  Control.addVariable("H2ModXStep",6.0);               
  Control.addVariable("H2ModYStep",20.1);             
  Control.addVariable("H2ModZStep",-13.2);     
  Control.addVariable("H2ModXYangle",91.5);  
  Control.addVariable("H2ModZangle",0.0);  
  Control.addVariable("H2ModDepth",10.0);      
  Control.addVariable("H2ModWidth",12.0);   
  Control.addVariable("H2ModHeight",12.0);   

  Control.addVariable("H2ModVacGap",0.4);     
  Control.addVariable("H2ModHeGap",0.2);     
  Control.addVariable("H2ModOutGap",0.5);
  Control.addVariable("H2ModMidAlThick",0.2);
  Control.addVariable("H2ModOutAlThick",0.2);

  Control.addVariable("H2ModFrontWall",0.5);
  Control.addVariable("H2ModBackWall",0.5);
  Control.addVariable("H2ModMainWall",0.5);
     
  Control.addVariable("H2ModSiThick",0.005);         // Poison
  Control.addVariable("H2ModNSiLayers",1);         // Poison 
//  Control.addVariable("H2ModSiThick",0.3);         // Silicon     
//  Control.addVariable("H2ModNSiLayers",8);         // Silicon
  Control.addVariable("H2ModAlMat","Aluminium");           // Al 
  Control.addVariable("H2ModLh2Mat","ParaH2");         // LH2 
  Control.addVariable("H2ModSiMat","Gadolinium");         // Poison; Gadolinium
//  Control.addVariable("H2ModSiMat","Silicon);         // Silicon  
  Control.addVariable("H2ModLh2Temp",20.0);      // LH2  Temp

  Control.addVariable("HPreModRSideThick",1.0);     
  Control.addVariable("HPreModLSideThick",1.0);     
  Control.addVariable("HPreModTopThick",0.0);     
  Control.addVariable("HPreModBaseThick",1.0);     
  Control.addVariable("HPreModBackThick",1.0);     
  Control.addVariable("HPreModAlThick",0.3);     
  Control.addVariable("HPreModVacThick",0.31);     
  Control.addVariable("HPreModModTemp",300.0);     
  Control.addVariable("HPreModModMat","H2O");     
  Control.addVariable("HPreModAlMat","Aluminium");     
  //
  // METHANE
  // 
  Control.addVariable("CH4ModXStep",0.0);      
  Control.addVariable("CH4ModYStep",0.90);  // +9.8    
  Control.addVariable("CH4ModZStep",-14.2);     
  Control.addVariable("CH4ModXYangle",54.5);     
  Control.addVariable("CH4ModZangle",0.0);     

  Control.addVariable("CH4ModNLayer",6);     
  Control.addVariable("CH4ModDepth1",6.0);     
  Control.addVariable("CH4ModWidth1",12.0);   
  Control.addVariable("CH4ModHeight1",12.0);   

  Control.addVariable("CH4ModFront2",0.5);       // FRont wall
  Control.addVariable("CH4ModBack2",0.5);       // Back wall
  Control.addVariable("CH4ModLayer2",0.5);       // Other wall

  Control.addVariable("CH4ModLayer3",0.4);       // VacGap
  Control.addVariable("CH4ModLayer4",0.2);   // Mid al
  Control.addVariable("CH4ModLayer5",0.2);      // He Layer
  Control.addVariable("CH4ModLayer6",0.2);    // Outer Al
  Control.addVariable("CH4ModLayer7",0.5);       // outer vac

  Control.addVariable("CH4ModMat1","CH4Liq");      
  Control.addVariable("CH4ModMat2","Aluminium");  
  Control.addVariable("CH4ModMat3","Void");       
  Control.addVariable("CH4ModMat4","Aluminium");   
  Control.addVariable("CH4ModMat5","Void");        
  Control.addVariable("CH4ModMat6","Aluminium");   
  Control.addVariable("CH4ModMat7","Void");        

  Control.addVariable("CH4ModTemp1",100.0);            // liquid CH4
  Control.addVariable("CH4ModTemp2",100.0);            // liquid CH4


  // Poison !!!!  
  Control.addVariable("CH4ModNPoison",2);      // 
  Control.addVariable("CH4ModPYStep1",0.0);       // for 1 poison layer
  Control.addVariable("CH4ModPYStep2",0.8);       // for 2 poison layer
  Control.addVariable("CH4ModPGdThick",0.005);      //
  Control.addVariable("CH4ModPCladThick",0.1);      //      
     
  Control.addVariable("CH4ModPoisonMat","Gadolinium");      // Gadolinium
  Control.addVariable("CH4ModPCladMat","Aluminium");   // Al    

  Control.addVariable("CH4PreFlatXStep",0.0);      
  Control.addVariable("CH4PreFlatYStep",0.0);  
  Control.addVariable("CH4PreFlatZStep",0.0);     
  Control.addVariable("CH4PreFlatXYangle",0.0);    
  Control.addVariable("CH4PreFlatZangle",0.0);  

  Control.addVariable("CH4PreFlatDepth",12.0);   
  Control.addVariable("CH4PreFlatWidth",12.0);   
  Control.addVariable("CH4PreFlatHeight",1.0);   
  Control.addVariable("CH4PreFlatAlThick",0.3);   
  Control.addVariable("CH4PreFlatVacThick",0.5);   
  Control.addVariable("CH4PreFlatAlMat","Aluminium");            // Al 
  Control.addVariable("CH4PreFlatModMat","H2O");            // Al 
  Control.addVariable("CH4PreFlatModTemp",300);            // Al 

  // WRAPPER
  Control.addVariable("CH4PreModSideThick",1.8);     
  Control.addVariable("CH4PreModTopThick",2.0);     
  Control.addVariable("CH4PreModBaseThick",1.5);
  Control.addVariable("CH4PreModFrontExt",0.7);
  Control.addVariable("CH4PreModBackExt",0.7);
  Control.addVariable("CH4PreModAlThick",0.31);     
  Control.addVariable("CH4PreModVacThick",0.6);     

  Control.addVariable("CH4PreModModTemp",300.0);     
  Control.addVariable("CH4PreModModMat","H2O");     
  Control.addVariable("CH4PreModAlMat","Aluminium");     

  // FLIGHTLINES : 
  Control.addVariable("H2FlightSideIndex",2);      // Full width
  Control.addVariable("H2FlightXStep",0.0);       // Step from centre
  Control.addVariable("H2FlightZStep",-0.15);        // Step from centre  
  Control.addVariable("H2FlightAngleXY1",28.0);   // Angle out  
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

  // FLIGHTLINES : 
  Control.addVariable("CH4FlightASideIndex",1);      // Full width
  Control.addVariable("CH4FlightAXStep",-1.0);       // Step from centre  
  Control.addVariable("CH4FlightAZStep",0.85);        // Step from centre  
  Control.addVariable("CH4FlightAAngleXY1",23.0);   // Angle out
  Control.addVariable("CH4FlightAAngleXY2",28.0);     // Angle out
  Control.addVariable("CH4FlightAAngleZTop",0.0);    // Step down angle
  Control.addVariable("CH4FlightAAngleZBase",0.0);   // Step up angle
  Control.addVariable("CH4FlightAHeight",10.0);      // Full height
  Control.addVariable("CH4FlightAWidth",13.5);        // Full width
  Control.addVariable("CH4FlightANLiner",2);         // Number of layers
  Control.addVariable("CH4FlightALinerThick1",0.5);  // Thickness
  Control.addVariable("CH4FlightALinerThick2",0.12); // Thickness
  Control.addVariable("CH4FlightALinerMat1","Aluminium");
  Control.addVariable("CH4FlightALinerMat2","Cadmium");  
// FLIGHTLINES : 
  Control.addVariable("CH4FlightBSideIndex",2);      // Full width
  Control.addVariable("CH4FlightBXStep",0.0);       // Step from centre
  Control.addVariable("CH4FlightBZStep",0.85);        // Step from centre  
  Control.addVariable("CH4FlightBAngleXY1",23.0);   // Angle out
  Control.addVariable("CH4FlightBAngleXY2",23.0);     // Angle out
  Control.addVariable("CH4FlightBAngleZTop",0.0);    // Step down angle
  Control.addVariable("CH4FlightBAngleZBase",0.0);   // Step up angle
  Control.addVariable("CH4FlightBHeight",10.0);      // Full height
  Control.addVariable("CH4FlightBWidth",11.5);        // Full width  
  Control.addVariable("CH4FlightBNLiner",2);         // Number of layers
  Control.addVariable("CH4FlightBLinerThick1",0.5);  // Thickness
  Control.addVariable("CH4FlightBLinerThick2",0.12); // Thickness
  Control.addVariable("CH4FlightBLinerMat1","Aluminium");
  Control.addVariable("CH4FlightBLinerMat2","Cadmium");  

  //
  // METHANE LAYER TEST
  // 
  Control.addVariable("CH4LayerXStep",0.0);      
  Control.addVariable("CH4LayerYStep",0.90);  // +9.8    
  Control.addVariable("CH4LayerZStep",-15.2);     
  Control.addVariable("CH4LayerXYangle",57.0);  
  Control.addVariable("CH4LayerZangle",0.0);  

  
  Control.addVariable("CH4LayerNPoison",0);      
  Control.addVariable("CH4LayerPoisonMat","Gadolinium");      
  
  Control.addVariable("CH4LayerPCladMat","Aluminium");      
  Control.addVariable("CH4LayerPoisonMat","Gadolinium");      


  Control.addVariable("CH4LayerNLayer",8);   

  Control.addVariable("CH4LayerHeight1",9.0);   
  Control.addVariable("CH4LayerWidth1",8.0);   
  Control.addVariable("CH4LayerFront1",1.5);   
  Control.addVariable("CH4LayerBack1",1.5);   
  Control.addVariable("CH4LayerMat1","H2O");   

  // al inner
  Control.addVariable("CH4LayerLayer2",0.3);   
  Control.addVariable("CH4LayerMat2","Aluminium");   

  // vac
  Control.addVariable("CH4LayerLayer3",0.5);   
  Control.addVariable("CH4LayerMat3","Void");   

  // Carbon - Al layer
  Control.addVariable("CH4LayerLayer4",0.6);   
  Control.addVariable("CH4LayerWidth4",0.0);   
  Control.addVariable("CH4LayerMat4","Graphite");     

  // H2 - Al layer
  Control.addVariable("CH4LayerLayer5",0.3);   
  Control.addVariable("CH4LayerMat5","Aluminium");     

  // H2 
  Control.addVariable("CH4LayerLayer6",1.2);   
  Control.addVariable("CH4LayerFront6",0.8);   
  Control.addVariable("CH4LayerBack6",1.0);   
  Control.addVariable("CH4LayerMat6","ParaH2");      // H2 
  
  // outer Al layer 
  Control.addVariable("CH4LayerLayer7",0.3);   
  Control.addVariable("CH4LayerMat7","Aluminium");     

  // Outer vac layer 
  Control.addVariable("CH4LayerLayer8",0.6);   
  Control.addVariable("CH4LayerMat8","Void");     


  return;
}

}  // NAMESPACE setVariable
