/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t1Upgrade/t1Uvariables.cxx
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

  Control.addVariable("H2CylModNWedge",0);

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
  Control.addVariable("t1CylTargetMainLength",35.2);    // Offset ref centre
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
  Control.addVariable("BWindowYStep",5.0);
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

  Control.addVariable("TriModNCorner",6);  
  Control.addVariable("TriModAbsolute1",1);
  Control.addVariable("TriModAbsolute2",1);
  Control.addVariable("TriModAbsolute3",1);
  Control.addVariable("TriModAbsolute4",1);
  Control.addVariable("TriModAbsolute5",1);
  Control.addVariable("TriModAbsolute6",1);
  Control.addVariable("TriModAbsXStep",0.0);
  Control.addVariable("TriModAbsYStep",0.0);
  Control.addVariable("TriModAbsZStep",0.0);
  Control.addVariable("TriModCorner1",Geometry::Vec3D(-9.50,31.14,0.0)); 
  Control.addVariable("TriModCorner2",Geometry::Vec3D(-0.22,13.92,0)); 
  Control.addVariable("TriModCorner3",Geometry::Vec3D(12.5,2.75,0.0)); 
  Control.addVariable("TriModCorner4",Geometry::Vec3D(-0.63,-2.88,0.0)); 
  Control.addVariable("TriModCorner5",Geometry::Vec3D(-2.49,12.96,0.0)); 
  Control.addVariable("TriModCorner6",Geometry::Vec3D(-15.97,26.70,0.0)); 
 
  Control.addVariable("TriModNInnerLayers",2); 
  Control.addVariable("TriModInnerThick0",0.5); 
  Control.addVariable("TriModInnerMat0","Aluminium");
  Control.addVariable("TriModInnerMat1","Void");

  Control.addVariable("TriModNInnerUnits",2); 
  Control.addVariable("TriModInner1NCorner",3); 
  Control.addVariable("TriModInner1Corner1",Geometry::Vec3D(-13.22,25.44,0.0)); 
  Control.addVariable("TriModInner1Corner2",Geometry::Vec3D(-8.53,28.67,0.0)); 
  Control.addVariable("TriModInner1Corner3",Geometry::Vec3D(-2.08,14.0,0.0)); 
  Control.addVariable("TriModInner1Absolute1",1); 
  Control.addVariable("TriModInner1Absolute2",1); 
  Control.addVariable("TriModInner1Absolute3",1); 

  Control.addVariable("TriModInner2NCorner",3); 
  Control.addVariable("TriModInner2Corner1",Geometry::Vec3D(12.31,0.65,0.0)); 
  Control.addVariable("TriModInner2Corner2",Geometry::Vec3D(0.79,-4.32,0.0)); 
  Control.addVariable("TriModInner2Corner3",Geometry::Vec3D(-1.41,13.97,0.0)); 
  Control.addVariable("TriModInner2Absolute1",1); 
  Control.addVariable("TriModInner2Absolute2",1); 
  Control.addVariable("TriModInner2Absolute3",1); 

  Control.addVariable("TriModHeight",12.0);
  Control.addVariable("TriModWallThick",0.5);
  Control.addVariable("TriModFlatClearance",0.6);
  Control.addVariable("TriModBaseClearance",0.3);
  Control.addVariable("TriModTopClearance",0.6);
  // Poison !!!!  
  Control.addVariable("TriModPoisonFlag",1);      // Activate poison layer
  Control.addVariable("TriModPoisonStep",7.0);  
  Control.addVariable("TriModPoisonThick",0.005);
  Control.addVariable("TriModPCladThick",0.1);    

  Control.addVariable("TriModPCladMat","Aluminium");        // Al 
  Control.addVariable("TriModPoisonMat","Gadolinium");      // Gadolinium  
  Control.addVariable("TriModInnerMat","Void");
  Control.addVariable("TriModModMat","H2O");
  Control.addVariable("TriModWallMat","Aluminium");
  Control.addVariable("TriModModTemp",300.0);

  Control.addVariable("TriModNFlight",2);
  Control.addVariable("TriModFlightSide0",3);
  Control.addVariable("TriModFlightSide1",5);

  //
  // TRIMOD LAYER
  //
  Control.addVariable("TriModLayerXStep",3.0);
  Control.addVariable("TriModLayerYStep",6.0);
  Control.addVariable("TriModLayerZStep",13.0);
  Control.addVariable("TriModLayerXYangle",-60.0);
  Control.addVariable("TriModLayerZangle",0.0);

  Control.addVariable("TriModLayerNPoison",0); 
  Control.addVariable("TriModLayerPYStep1",0.0);       // for 1 poison layer
  Control.addVariable("TriModLayerPYStep2",0.8);       // for 2 poison layer
  Control.addVariable("TriModLayerPGdThick",0.005);      //
  Control.addVariable("TriModLayerPCladThick",0.1);      //             
  Control.addVariable("TriModLayerPoisonMat","Gadolinium");      
  
  Control.addVariable("TriModLayerNLayer",6);   
  Control.addVariable("TriModLayerNPoison",0);   

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
  Control.addVariable("TriFlightASideIndex",7);     
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
  Control.addVariable("H2ModNLayers",3);  
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

 
  Control.addVariable("CH4ModNLayer",7);     
  Control.addVariable("CH4ModDepth1",6.0);     
  Control.addVariable("CH4ModWidth1",12.0);   
  Control.addVariable("CH4ModHeight1",12.0);   

  Control.addVariable("CH4ModFrontRadius1",30.0);  
  Control.addVariable("CH4ModFrontRadius2",30.0);  
  Control.addVariable("CH4ModFrontRadius3",30.0);  
  Control.addVariable("CH4ModFrontRadius4",30.0);  
  Control.addVariable("CH4ModFrontRadius5",30.0);  
  Control.addVariable("CH4ModFrontRadius6",30.0);  
  Control.addVariable("CH4ModFrontRadius7",30.0);  
 
  Control.addVariable("CH4ModBackRadius1",30.0);  
  Control.addVariable("CH4ModBackRadius2",30.0);  
  Control.addVariable("CH4ModBackRadius3",30.0);  
  Control.addVariable("CH4ModBackRadius4",30.0);  
  Control.addVariable("CH4ModBackRadius5",30.0);  
  Control.addVariable("CH4ModBackRadius6",30.0);  
  Control.addVariable("CH4ModBackRadius7",30.0);  
  Control.addVariable("CH4ModRound1",0.3);    
  Control.addVariable("CH4ModRound2",0.4);  
  Control.addVariable("CH4ModRound3",0.7);  

  Control.addVariable("CH4ModFront2",0.5);       // Front wall
  Control.addVariable("CH4ModBack2",0.5);        // Back wall
  Control.addVariable("CH4ModLayer2",0.5);       // Other wall

  Control.addVariable("CH4ModLayer3",0.4);       // VacGap
  Control.addVariable("CH4ModLayer4",0.2);   // Mid al
  Control.addVariable("CH4ModLayer5",0.2);      // He Layer
  Control.addVariable("CH4ModLayer6",0.2);    // Outer Al
  Control.addVariable("CH4ModLayer7",0.5);       // outer vac

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
  Control.addVariable("CH4ModInnerNPoison",2);      // 
  Control.addVariable("CH4ModInnerPYStep1",0.0);       // for 1 poison layer
  Control.addVariable("CH4ModInnerPYStep2",0.8);       // for 2 poison layer
  Control.addVariable("CH4ModInnerPGdThick",0.005);      //
  Control.addVariable("CH4ModInnerPCladThick",0.1);      //      
     
  Control.addVariable("CH4ModInnerPoisonMat","Gadolinium");      // Gadolinium
  Control.addVariable("CH4ModInnerPCladMat","Aluminium");   // Al    


  Control.addVariable("CH4PreFlatXStep",0.0);      
  Control.addVariable("CH4PreFlatYStep",0.0);  
  Control.addVariable("CH4PreFlatZStep",0.0);     
  Control.addVariable("CH4PreFlatXYangle",0.0);    
  Control.addVariable("CH4PreFlatZangle",0.0);  

  Control.addVariable("CH4PreFlatDepth",12.0);   
  Control.addVariable("CH4PreFlatWidth",12.0);   
  Control.addVariable("CH4PreFlatHeight",1.0);   
  Control.addVariable("CH4PreFlatAlThick",0.3);   
  Control.addVariable("CH4PreFlatVacThick",0.3);   
  Control.addVariable("CH4PreFlatAlMat","Aluminium");            // Al 
  Control.addVariable("CH4PreFlatModMat","H2O");            // Al 
  Control.addVariable("CH4PreFlatModTemp",300);            // Al 

  // WRAPPER
  Control.addVariable("CH4PreModSideThick",1.8);     
  Control.addVariable("CH4PreModTopThick",1.5);     
  Control.addVariable("CH4PreModBaseThick",1.3);
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
  // METHANE LAYER TEST
  // 
  Control.addVariable("CH4LayerXStep",1.7);      
  Control.addVariable("CH4LayerYStep",2.3);  // +9.8    
  Control.addVariable("CH4LayerZStep",-13.4);     
  Control.addVariable("CH4LayerXYangle",60.0);  
  Control.addVariable("CH4LayerZangle",0.0);  

  Control.addVariable("CH4LayerZangle",0.0);  

  
  Control.addVariable("CH4LayerNPoison",0);    
  Control.addVariable("CH4LayerPYStep1",0.0);       // for 1 poison layer
  Control.addVariable("CH4LayerPYStep2",0.8);       // for 2 poison layer
  Control.addVariable("CH4LayerPGdThick",0.005);      //
  Control.addVariable("CH4LayerPCladThick",0.1);      //               
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

  //
  // HYDROGEN LAYER
  //
  Control.addVariable("H2LayerXStep",6.7);
  Control.addVariable("H2LayerYStep",20.1);  // +9.8
  Control.addVariable("H2LayerZStep",-16.1);
  Control.addVariable("H2LayerXYangle",100.0);
  Control.addVariable("H2LayerZangle",0.0);


  Control.addVariable("H2LayerNPoison",0);
  Control.addVariable("H2LayerPYStep1",0.0);       // for 1 poison layer
  Control.addVariable("H2LayerPYStep2",0.8);       // for 2 poison layer
  Control.addVariable("H2LayerPGdThick",0.005);      //
  Control.addVariable("H2LayerPCladThick",0.1);      //
  Control.addVariable("H2LayerPoisonMat","Gadolinium");

  Control.addVariable("H2LayerPCladMat","Aluminium");
  Control.addVariable("H2LayerPoisonMat","Gadolinium");


  Control.addVariable("H2LayerNLayer",5);

  Control.addVariable("H2LayerHeight1",16.8);
  Control.addVariable("H2LayerWidth1",10.2);
  Control.addVariable("H2LayerFront1",3.5);
  Control.addVariable("H2LayerBack1",3.5);
  Control.addVariable("H2LayerMat1","H2O");

  // al inner
  Control.addVariable("H2LayerLayer2",0.3);
  Control.addVariable("H2LayerMat2","Aluminium");

  // vac
  Control.addVariable("H2LayerLayer3",0.5);
  Control.addVariable("H2LayerMat3","Void");

  // H2 - Al layer
  Control.addVariable("H2LayerLayer4",0.3);
  Control.addVariable("H2LayerMat4","Aluminium");

  // vac
  Control.addVariable("H2LayerLayer5",0.5);
  Control.addVariable("H2LayerMat5","Void");

  // H2 - Al layer
  Control.addVariable("H2LayerLayer4",0.3);
  Control.addVariable("H2LayerMat4","Aluminium");

  // vac
  Control.addVariable("H2LayerLayer5",0.5);
  Control.addVariable("H2LayerMat5","Void");


  Control.addVariable("H2InnerNLayers",5);
  Control.addVariable("H2InnerThick1",2.8);
  Control.addVariable("H2InnerThick2",0.3);
  Control.addVariable("H2InnerThick3",0.3);
  Control.addVariable("H2InnerThick4",0.3);

  Control.addVariable("H2InnerMat1","H2O");
  Control.addVariable("H2InnerMat2","Aluminium");
  Control.addVariable("H2InnerMat3","Void");
  Control.addVariable("H2InnerMat4","Aluminium");
  Control.addVariable("H2InnerMat5","ParaOrtho%80");

  Control.addVariable("H2InnerTemp1",300.0);
  Control.addVariable("H2InnerTemp2",300.0);
  Control.addVariable("H2InnerTemp3",300.0);
  Control.addVariable("H2InnerTemp4",20.0);
  Control.addVariable("H2InnerTemp5",20.0);
  
  Control.addVariable("H2LayerRadius1",30.0);  
  Control.addVariable("H2LayerRadius2",30.0);  
  Control.addVariable("H2LayerRadius3",30.0);  
  Control.addVariable("H2LayerRadius4",30.0);  
  Control.addVariable("H2LayerRadius5",30.0);  
  Control.addVariable("H2LayerRound1",0.3);    
  Control.addVariable("H2LayerRound2",0.4);  
  Control.addVariable("H2LayerRound3",0.7);  


  Control.addVariable("H2PipeNSegIn",1);
  // Central point:
  Control.addVariable("H2PipePPt0",Geometry::Vec3D(0,-4.0,2.8));
  Control.addVariable("H2PipePPt1",Geometry::Vec3D(0,19.25,2.8));
  // Central point [Top]:

  Control.addVariable("H2PipeNRadii",4);
  Control.addVariable("H2PipeRadius0",1.2);
  Control.addVariable("H2PipeRadius1",1.3);
  Control.addVariable("H2PipeRadius2",1.6);
  Control.addVariable("H2PipeRadius3",2.3);
  Control.addVariable("H2PipeRadius4",2.5);
  Control.addVariable("H2PipeRadius5",2.7);
  Control.addVariable("H2PipeRadius6",2.9);
  Control.addVariable("H2PipeRadius7",3.5);
  Control.addVariable("H2PipeRadius8",3.7);

  Control.addVariable("H2PipeMat0","ParaH2");
  Control.addVariable("H2PipeMat1","Aluminium");
  Control.addVariable("H2PipeMat2","Aluminium");
  Control.addVariable("H2PipeMat3","Void");
  Control.addVariable("H2PipeMat4","Aluminium");
  Control.addVariable("H2PipeMat5","Void");
  Control.addVariable("H2PipeMat6","Aluminium");
  Control.addVariable("H2PipeMat7","Void");
  Control.addVariable("H2PipeMat8","Aluminium");

  Control.addVariable("H2PipeTemp0",25.0);
  Control.addVariable("H2PipeTemp1",25.0);
  Control.addVariable("H2PipeTemp2",25.0);
  Control.addVariable("H2PipeTemp3",0.0);

  Control.addVariable("H2PipeActive0",3);
  // Control.addVariable("H2PipeActive1",7);
  // Control.addVariable("H2PipeActive2",31);
  //  Control.addVariable("H2PipeActive3",127);
  // Control.addVariable("H2PipeActive4",511);
  // Control.addVariable("H2PipeActive5",255);
  // Control.addVariable("H2PipeActive6",255);

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



  Control.addVariable("CH4PipeNSegIn",1);
  // Central point:
  Control.addVariable("CH4PipePPt0",Geometry::Vec3D(-0.346,0.0,0.2));
  Control.addVariable("CH4PipePPt1",Geometry::Vec3D(-0.346,-30.0,0.2));
  // Central point [Top]:

  // NUMBER FROM TS1 baseline model
  Control.addVariable("CH4PipeNRadii",7);
  Control.addVariable("CH4PipeRadius0",0.6);     // inner
  Control.addVariable("CH4PipeRadius1",0.7);     // sep al
  Control.addVariable("CH4PipeRadius2",1.43);     // return
  Control.addVariable("CH4PipeRadius3",1.59);     // Al outer
  Control.addVariable("CH4PipeRadius4",1.865);     // vac
  Control.addVariable("CH4PipeRadius5",2.005);     // vac al [1.905 orig]
  Control.addVariable("CH4PipeRadius6",2.38);     // clearacne

  Control.addVariable("CH4PipeMat0","CH4Liq110K");
  Control.addVariable("CH4PipeMat1","Aluminium");
  Control.addVariable("CH4PipeMat2","CH4Liq110K");
  Control.addVariable("CH4PipeMat3","Aluminium");
  Control.addVariable("CH4PipeMat4","Void");
  Control.addVariable("CH4PipeMat5","Aluminium");
  Control.addVariable("CH4PipeMat6","Void");

  Control.addVariable("CH4PipeTemp0",110.0);
  Control.addVariable("CH4PipeTemp1",110.0);
  Control.addVariable("CH4PipeTemp2",110.0);
  Control.addVariable("CH4PipeTemp3",110.0);

  Control.addVariable("CH4PipeActive0",3);
  Control.addVariable("CH4PipeActive1",3);
  Control.addVariable("CH4PipeActive2",7);
  Control.addVariable("CH4PipeActive3",15);
  Control.addVariable("CH4PipeActive4",31);
  Control.addVariable("CH4PipeActive5",63);
  // Control.addVariable("CH4PipeActive1",7);
  // Control.addVariable("CH4PipeActive2",31);
  //  Control.addVariable("CH4PipeActive3",127);
  // Control.addVariable("CH4PipeActive4",511);
  // Control.addVariable("CH4PipeActive5",255);
  // Control.addVariable("CH4PipeActive6",255);

  Control.addVariable("t1CylFluxTrapNCone",1);
  Control.addVariable("t1CylFluxTrapCone1AngleA",-45.0);
  Control.addVariable("t1CylFluxTrapCone1AngleB",30.0);
  Control.addVariable("t1CylFluxTrapCone1Dist",-4.0);
  Control.addVariable("t1CylFluxTrapCone1Cent",Geometry::Vec3D(0,12,0));
  Control.addVariable("t1CylFluxTrapCone1Axis",Geometry::Vec3D(0,1,0));
  Control.addVariable("t1CylFluxTrapCone1Mat","H2O");
  Control.addVariable("t1CylFluxTrapCone1LayerMat","Tantalum");
  Control.addVariable("t1CylFluxTrapCone1LayerThick",0.3);

  Control.addVariable("t1CylFluxTrapCone2AngleA",-45.0);
  Control.addVariable("t1CylFluxTrapCone2AngleB",-45.0);
  Control.addVariable("t1CylFluxTrapCone2Dist",2.0);
  Control.addVariable("t1CylFluxTrapCone2Cent",Geometry::Vec3D(0,18,0));
  Control.addVariable("t1CylFluxTrapCone2Axis",Geometry::Vec3D(0,1,0));
  Control.addVariable("t1CylFluxTrapCone2Mat","H2O");
  Control.addVariable("t1CylFluxTrapCone2LayerMat","Tantalum");
  Control.addVariable("t1CylFluxTrapCone2LayerThick",0.3);


  return;
}

}  // NAMESPACE setVariable
