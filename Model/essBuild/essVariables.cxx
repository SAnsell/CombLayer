/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/essVariables.cxx
 *
 * Copyright (c) 2004-2015 by Stuart Ansell/Konstantin Batkov
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
#include "variableSetup.h"

namespace setVariable
{
  void EssBeamLinesVariables(FuncDataBase&);
  void EssBunkerVariables(FuncDataBase&);
  void EssReflectorVariables(FuncDataBase&);
  void EssSANSVariables(FuncDataBase&);
  void EssButterflyModerator(FuncDataBase&);
  void ESSWheel(FuncDataBase&);
  void F5Variables(FuncDataBase&);

void
EssWheel(FuncDataBase& Control)
  /*!
    Variables that are used for the segmented wheel
    \param Control :: Segment variables
   */
{
  Control.addVariable("BilbaoWheelEngineeringActive", 1);
  Control.addVariable("BilbaoWheelShaftHeight",435.0);

  Control.addVariable("BilbaoWheelNShaftLayers",5);
  Control.addVariable("BilbaoWheelShaftRadius1",14.0);
  Control.addVariable("BilbaoWheelShaftMat1","Iron");
  Control.addVariable("BilbaoWheelShaftRadius2",15.0);
  Control.addVariable("BilbaoWheelShaftMat2","Iron");
  Control.addVariable("BilbaoWheelShaftRadius3",17.0);
  Control.addVariable("BilbaoWheelShaftMat3",0);
  Control.addVariable("BilbaoWheelShaftRadius4",20.0);
  Control.addVariable("BilbaoWheelShaftMat4","Iron");
  Control.addVariable("BilbaoWheelShaftRadius5",22.0);
  Control.addVariable("BilbaoWheelShaftMat5",0);



  Control.addVariable("BilbaoWheelXStep",0.0);
  Control.addVariable("BilbaoWheelYStep",112.0);
  Control.addVariable("BilbaoWheelZStep",0.0);
  Control.addVariable("BilbaoWheelXYangle",5.0);
  Control.addVariable("BilbaoWheelZangle",0.0);
  Control.addVariable("BilbaoWheelTargetHeight",8.0);
  Control.addVariable("BilbaoWheelVoidTungstenThick", 0.1);
  Control.addVariable("BilbaoWheelSteelTungstenThick", 0.2);
  Control.addVariable("BilbaoWheelTemp",300.0);
  Control.addVariable("BilbaoWheelCoolantThick",0.5);
  Control.addVariable("BilbaoWheelCaseThick",1.0);
  Control.addVariable("BilbaoWheelCaseThickIn",1.5);
  Control.addVariable("BilbaoWheelVoidThick",0.7);

  Control.addVariable("BilbaoWheelInnerRadius",45);
  Control.addVariable("BilbaoWheelCoolantRadiusIn",64.07);
  Control.addVariable("BilbaoWheelCoolantRadiusOut",128.95);
  Control.addVariable("BilbaoWheelCaseRadius",129.15);
  Control.addVariable("BilbaoWheelVoidRadius",131.15);
  Control.addVariable("BilbaoWheelAspectRatio", 0.00138);
  Control.addVariable("BilbaoWheelNSectors", 36);
  Control.addVariable("BilbaoWheelSectorSepThick", 1.0);
  Control.addVariable("BilbaoWheelSectorSepMat", "SS316L785");

  Control.addVariable("BilbaoWheelWMat","Tungsten151");
  Control.addVariable("BilbaoWheelSteelMat","SS316L785");
  Control.addVariable("BilbaoWheelHeMat","Helium");
  Control.addVariable("BilbaoWheelSS316LVoidMat","M2644"); // !!! use appropriate name
  Control.addVariable("BilbaoWheelInnerMat","SS316L785");

  Control.addVariable("BilbaoWheelNLayers",3);

  Control.addVariable("BilbaoWheelRadius1",48);
  Control.addVariable("BilbaoWheelMatTYPE1",1); // SS316L

  Control.addVariable("BilbaoWheelRadius2",85.0);
  Control.addVariable("BilbaoWheelMatTYPE2",1);

  Control.addVariable("BilbaoWheelRadius3",125.0);
  Control.addVariable("BilbaoWheelMatTYPE3",3);

  return;
}

void
EssVariables(FuncDataBase& Control)
  /*!
    Function to set the control variables and constants
    -- This version is for ESS ()
    \param Control :: Function data base to add constants too
  */
{
// -----------
// GLOBAL stuff
// -----------

  Control.addVariable("zero",0.0);     // Zero
  Control.addVariable("one",1.0);      // one

  Control.addVariable("EngineeringActive",0);      // NO engineering
  Control.addVariable("F5Radius", 1000);      // Radial location of all F5 tallies (valid with -f5-collimators argument)

  Control.addVariable("LSupplyNSegIn",2);
  // Central point:
  Control.addVariable("LSupplyPPt0",Geometry::Vec3D(0,-1.0,0.0));
  Control.addVariable("LSupplyPPt1",Geometry::Vec3D(0,-19.25,0.0));
  Control.addVariable("LSupplyPPt2",Geometry::Vec3D(3.005,-19.25,64.930));
  // Central point [Top]:
  Control.addVariable("LSupplyTopPPt0",Geometry::Vec3D(0,-1.0,5.0));
  Control.addVariable("LSupplyTopPPt1",Geometry::Vec3D(0,-19.25,5.0));
  Control.addVariable("LSupplyTopPPt2",Geometry::Vec3D(3.005,-19.25,64.930));

  Control.addVariable("LSupplyNRadii",9);
  Control.addVariable("LSupplyRadius0",1.5);
  Control.addVariable("LSupplyRadius1",1.7);
  Control.addVariable("LSupplyRadius2",1.8);
  Control.addVariable("LSupplyRadius3",2.3);
  Control.addVariable("LSupplyRadius4",2.5);
  Control.addVariable("LSupplyRadius5",2.7);
  Control.addVariable("LSupplyRadius6",2.9);
  Control.addVariable("LSupplyRadius7",3.5);
  Control.addVariable("LSupplyRadius8",3.7);

  Control.addVariable("LSupplyMat0","ParaH2");
  Control.addVariable("LSupplyMat1","Aluminium");
  Control.addVariable("LSupplyMat2","Aluminium");
  Control.addVariable("LSupplyMat3","Void");
  Control.addVariable("LSupplyMat4","Aluminium");
  Control.addVariable("LSupplyMat5","Void");
  Control.addVariable("LSupplyMat6","Aluminium");
  Control.addVariable("LSupplyMat7","Void");
  Control.addVariable("LSupplyMat8","Aluminium");

  Control.addVariable("LSupplyTemp0",25.0);
  Control.addVariable("LSupplyTemp1",25.0);
  Control.addVariable("LSupplyTemp2",25.0);
  Control.addVariable("LSupplyTemp3",0.0);

  Control.addVariable("LSupplyActive0",3);
  Control.addVariable("LSupplyActive1",7);
  Control.addVariable("LSupplyActive2",31);
  Control.addVariable("LSupplyActive3",127);
  Control.addVariable("LSupplyActive4",511);
  Control.addVariable("LSupplyActive5",255);
  Control.addVariable("LSupplyActive6",255);

  // low mod return pipe

  Control.addVariable("LReturnNSegIn",1);
  Control.addVariable("LReturnPPt0",Geometry::Vec3D(0,0,0));
  Control.addVariable("LReturnPPt1",Geometry::Vec3D(0,30,0));

  Control.addVariable("LReturnTopNSegIn",1);
  Control.addVariable("LReturnTopPPt0",Geometry::Vec3D(0,0,-5));
  Control.addVariable("LReturnTopPPt1",Geometry::Vec3D(0,30,-5));

  Control.addVariable("LReturnNRadii",8);
  Control.addVariable("LReturnRadius0",1.7);
  Control.addVariable("LReturnRadius1",1.8);
  Control.addVariable("LReturnRadius2",2.3);
  Control.addVariable("LReturnRadius3",2.5);
  Control.addVariable("LReturnRadius4",2.7);
  Control.addVariable("LReturnRadius5",2.9);
  Control.addVariable("LReturnRadius6",3.5);
  Control.addVariable("LReturnRadius7",3.7);

  Control.addVariable("LReturnMat0","ParaH2");
  Control.addVariable("LReturnMat1","Aluminium");
  Control.addVariable("LReturnMat2","Void");
  Control.addVariable("LReturnMat3","Aluminium");
  Control.addVariable("LReturnMat4","Void");
  Control.addVariable("LReturnMat5","Aluminium");
  Control.addVariable("LReturnMat6","Void");
  Control.addVariable("LReturnMat7","Aluminium");

  Control.addVariable("LReturnTemp0",25.0);
  Control.addVariable("LReturnTemp1",25.0);

  Control.addVariable("LReturnActive0",3);
  Control.addVariable("LReturnActive1",15);
  Control.addVariable("LReturnActive2",63);
  Control.addVariable("LReturnActive3",255);
  Control.addVariable("LReturnActive4",127);
  //
  Control.addVariable("LowModXStep",0.0);  
  Control.addVariable("LowModYStep",0.0);  
  Control.addVariable("LowModZStep",-18.0);
  Control.addVariable("LowModXYangle",125.15); 
  Control.addVariable("LowModZangle",0.0);
  Control.addVariable("LowModRadius",8.0);
  Control.addVariable("LowModHeight",10.0);
  Control.addVariable("LowModMat","ParaH2");
  Control.addVariable("LowModTemp",20.0);
  Control.addVariable("LowModNLayers",7);   // Inner counts as 1 + 6
  // al layer
  Control.addVariable("LowModHGap1",0.3);
  Control.addVariable("LowModRadGap1",0.3);
  Control.addVariable("LowModMaterial1","Aluminium");  // Al materk
  Control.addVariable("LowModTemp1",20.0);  
  // Vac gap
  Control.addVariable("LowModHGap2",0.5);
  Control.addVariable("LowModRadGap2",0.5);
  Control.addVariable("LowModMaterial2","Void"); 
  // Next Al layer
  Control.addVariable("LowModHGap3",0.2);
  Control.addVariable("LowModRadGap3",0.2);
  Control.addVariable("LowModMaterial3","Aluminium"); 
  Control.addVariable("LowModTemp3",77.0);  
  // He Layer
  Control.addVariable("LowModHGap4",0.2);
  Control.addVariable("LowModRadGap4",0.2);
  Control.addVariable("LowModMaterial4","Void"); 
  // Outer Layer
  Control.addVariable("LowModHGap5",0.2);
  Control.addVariable("LowModRadGap5",0.2);
  Control.addVariable("LowModMaterial5","Aluminium"); 
  Control.addVariable("LowModTemp5",300.0); 
  // Clearance
  Control.addVariable("LowModHGap6",0.2);
  Control.addVariable("LowModRadGap6",0.2);
  Control.addVariable("LowModMaterial6","Void"); 
  Control.addVariable("LowModNConic",0);
  Control.addVariable("LowModNWedge",1);

  Control.addVariable("LowModWedge1Cent",Geometry::Vec3D(0,0,1));
  Control.addVariable("LowModWedge1XYangle",0.0);
  Control.addVariable("LowModWedge1Zangle",0.0);
  Control.addVariable("LowModWedge1ViewWidth",6.0);
  Control.addVariable("LowModWedge1ViewHeight",3.0);
  Control.addVariable("LowModWedge1ViewXY",30.0);
  Control.addVariable("LowModWedge1ViewZ",0.0);
  Control.addVariable("LowModWedge1Mat","Void");
  Control.addVariable("LowModWedge1WallMat","Aluminium");
  Control.addVariable("LowModWedge1Wall",0.2);
  Control.addVariable("LowModWedge1Temp",20.0);

  Control.addVariable("LowModConic1Cent",Geometry::Vec3D(0,0,1));
  Control.addVariable("LowModConic1Axis",Geometry::Vec3D(0,0,1));
  Control.addVariable("LowModConic1Angle",65.0);
  Control.addVariable("LowModConic1Mat","Void");
  Control.addVariable("LowModConic1WallMat","Aluminium");
  Control.addVariable("LowModConic1Wall",0.2);

  Control.addVariable("LowModConic2Cent",Geometry::Vec3D(0,0,-1));
  Control.addVariable("LowModConic2Axis",Geometry::Vec3D(0,0,-1));
  Control.addVariable("LowModConic2Angle",65.0);
  Control.addVariable("LowModConic2Mat","Void");
  Control.addVariable("LowModConic2WallMat","Aluminium");
  Control.addVariable("LowModConic2Wall",0.2);

  Control.addVariable("LowModConic4Cent",Geometry::Vec3D(-1,0,0));
  Control.addVariable("LowModConic4Axis",Geometry::Vec3D(-1,0,0));
  Control.addVariable("LowModConic4Angle",25.0);
  Control.addVariable("LowModConic4Mat","Void");
  Control.addVariable("LowModConic4WallMat","Aluminium");
  Control.addVariable("LowModConic4Wall",0.2);

  Control.addVariable("LowModConic3Cent",Geometry::Vec3D(1,0,0));
  Control.addVariable("LowModConic3Axis",Geometry::Vec3D(1,0,0));
  Control.addVariable("LowModConic3Angle",25.0);
  Control.addVariable("LowModConic3Mat","Void");
  Control.addVariable("LowModConic3WallMat","Aluminium");
  Control.addVariable("LowModConic3Wall",0.2);

  // FLIGHT LINE

  Control.addVariable("LowAFlightXStep",0.0); // Step from centre
  Control.addVariable("LowAFlightZStep",0.0);      // Step from centre
  Control.addVariable("LowAFlightXYangle",0.0);  // Angle out
  Control.addVariable("LowAFlightZangle",0.0);  // Angle out
  Control.addVariable("LowAFlightAngleXY1",60.0);  // Angle out
  Control.addVariable("LowAFlightAngleXY2",60.0);  // Angle out
  Control.addVariable("LowAFlightAngleZTop",0.95); // Step up angle
  Control.addVariable("LowAFlightAngleZBase",0.95);  // Step down angle
  Control.addVariable("LowAFlightHeight",5.44);     // Full height = LowFlyTotalHeight
  Control.addVariable("LowAFlightWidth",10.7);     // Full width
  Control.addVariable("LowAFlightNLiner",1);      // Liner
  Control.addVariable("LowAFlightLinerThick1",0.4);      // Liner
  Control.addVariable("LowAFlightLinerMat1","Aluminium");      // Liner
  Control.addVariable("LowAFlightLinerThick2",1.5);      // Liner
  Control.addVariable("LowAFlightLinerMat2","H2O");      // Liner
  Control.addVariable("LowAFlightLinerThick3",0.2);      // Liner
  Control.addVariable("LowAFlightLinerMat3","Aluminium");      // Liner
  Control.addVariable("LowAFlightLinerThick4",0.3);      // Liner
  Control.addVariable("LowAFlightLinerMat4","Void");      // Liner

  Control.addVariable("LowBFlightXStep",0.0);     // Angle
  Control.addVariable("LowBFlightZStep",0.0);      // Step from centre
  Control.addVariable("LowBFlightXYangle",180.0);  // Angle out
  Control.addVariable("LowBFlightZangle",0.0);  // Angle out
  Control.addVariable("LowBFlightAngleXY1",60.0);  // Angle out
  Control.addVariable("LowBFlightAngleXY2",60.0);  // Angle out
  Control.Parse("LowAFlightAngleZTop");
  Control.addVariable("LowBFlightAngleZTop");  // Step down angle
  Control.Parse("LowAFlightAngleZBase");
  Control.addVariable("LowBFlightAngleZBase"); // Step up angle
  Control.Parse("LowAFlightHeight");
  Control.addVariable("LowBFlightHeight");     // Full height = LowFlyTotalHeight
  Control.addVariable("LowBFlightWidth",10.7);     // Full width
  Control.addVariable("LowBFlightNLiner",1);      // Liner
  Control.addVariable("LowBFlightLinerThick1",0.5);   
  Control.addVariable("LowBFlightLinerMat1","Aluminium");      
  // 
  Control.addVariable("LowPreNLayers",4);  
  Control.addVariable("LowPreHeight1",0.2);  
  Control.addVariable("LowPreDepth1",0.2);  
  Control.addVariable("LowPreThick1",0.2);  
  Control.addVariable("LowPreMaterial1","Aluminium");  
  Control.addVariable("LowPreHeight2",2.0);  
  Control.addVariable("LowPreDepth2",2.0);  
  Control.addVariable("LowPreThick2",2.0);  
  Control.addVariable("LowPreMaterial2","H2O");  
  Control.addVariable("LowPreHeight3",0.2);  
  Control.addVariable("LowPreDepth3",0.2);  
  Control.addVariable("LowPreThick3",0.2);  
  Control.addVariable("LowPreMaterial3","Aluminium");  
  Control.addVariable("LowPreHeight4",0.2);  
  Control.addVariable("LowPreDepth4",0.2);  
  Control.addVariable("LowPreThick4",0.2);  
  Control.addVariable("LowPreMaterial4","Void"); 
 
  Control.addVariable("LowPreNView",2);  
  Control.addVariable("LowPreViewHeight1",10.0);  
  Control.addVariable("LowPreViewWidth1",15.0);  
  Control.addVariable("LowPreViewAngle1",0.0);  
  Control.addVariable("LowPreViewOpenAngle1",30.0);  

  Control.addVariable("LowPreViewHeight2",10.0);  
  Control.addVariable("LowPreViewWidth2",15.0);  
  Control.addVariable("LowPreViewAngle2",180.0);  
  Control.addVariable("LowPreViewOpenAngle2",30.0);  

  Control.addVariable("LowPreABlockActive",1);  
  Control.addVariable("LowPreABlockSide",0);  
  Control.addVariable("LowPreABlockWidth",4.0);  
  Control.addVariable("LowPreABlockHeight",12.0);  
  Control.addVariable("LowPreABlockLength",10.4);
  Control.addVariable("LowPreABlockWaterMat","H2O");  
  Control.addVariable("LowPreABlockTemp",300.0);
  Control.addVariable("LowPreABlockNLayers",3);  
  Control.addVariable("LowPreABlockWallThick1",0.2);   
  Control.addVariable("LowPreABlockWallMat1","Aluminium");  
  Control.addVariable("LowPreABlockWallThick2",0.3);   
  Control.addVariable("LowPreABlockWallMat2","Void");  
  Control.addVariable("LowPreABlockWallTemp1",300.0);  

  // Other block
  Control.addVariable("LowPreBBlockActive",1);  
  Control.addVariable("LowPreBBlockSide",0);  
  Control.addVariable("LowPreBBlockWidth",4.0);  
  Control.addVariable("LowPreBBlockHeight",12.0);  
  Control.addVariable("LowPreBBlockLength",10.4);  
  Control.addVariable("LowPreBBlockWaterMat","H2O");  
  Control.addVariable("LowPreBBlockTemp",300.0);
  Control.addVariable("LowPreBBlockNLayers",3);  
  Control.addVariable("LowPreBBlockWallThick1",0.2);   
  Control.addVariable("LowPreBBlockWallMat1","Aluminium");  
  Control.addVariable("LowPreBBlockWallThick2",0.3);   
  Control.addVariable("LowPreBBlockWallMat2","Void");  
  Control.addVariable("LowPreBBlockWallTemp1",300.0);  

  // TOP MODERATOR PRE:
  Control.addVariable("TopPreNLayers",5);  
  Control.addVariable("TopPreHeight1",0.2);  
  Control.addVariable("TopPreDepth1",0.2);  
  Control.addVariable("TopPreThick1",0.2);  
  Control.addVariable("TopPreMaterial1","Void");  
  Control.addVariable("TopPreHeight2",0.2);  
  Control.addVariable("TopPreDepth2",0.2);  
  Control.addVariable("TopPreThick2",0.2);  
  Control.addVariable("TopPreMaterial2","Aluminium");  
  Control.addVariable("TopPreHeight3",2.0);  
  Control.addVariable("TopPreDepth3",2.0);  
  Control.addVariable("TopPreThick3",2.0);  
  Control.addVariable("TopPreMaterial3","H2O");  
  Control.addVariable("TopPreHeight4",0.2);  
  Control.addVariable("TopPreDepth4",0.2);  
  Control.addVariable("TopPreThick4",0.2);  
  Control.addVariable("TopPreMaterial4","Aluminium");  
  Control.addVariable("TopPreHeight5",0.2);  
  Control.addVariable("TopPreDepth5",0.2);  
  Control.addVariable("TopPreThick5",0.2);  
  Control.addVariable("TopPreMaterial5","Void"); 
 
  Control.addVariable("TopPreNView",2);  
  Control.addVariable("TopPreViewHeight1",10.0);  
  Control.addVariable("TopPreViewWidth1",8.0);  
  Control.addVariable("TopPreViewAngle1",0.0);  
  Control.addVariable("TopPreViewOpenAngle1",0.0);  

  Control.addVariable("TopPreViewHeight2",10.0);  
  Control.addVariable("TopPreViewWidth2",8.0);  
  Control.addVariable("TopPreViewAngle2",180.0);  
  Control.addVariable("TopPreViewOpenAngle2",30.0);  

  Control.addVariable("TopPreABlockActive",1);  
  Control.addVariable("TopPreABlockSide",0);  
  Control.addVariable("TopPreABlockWidth",4.0);  
  Control.addVariable("TopPreABlockHeight",12.0);  
  Control.addVariable("TopPreABlockLength",10.4);  
  Control.addVariable("TopPreABlockWaterMat","H2O");
  Control.addVariable("TopPreABlockTemp",20);
  Control.addVariable("TopPreABlockNLayers",2);  
  Control.addVariable("TopPreABlockWallThick1",0.2);   
  Control.addVariable("TopPreABlockWallMat1","Aluminium");  
  Control.addVariable("TopPreABlockWallTemp1",20.0);  

  // Other block
  Control.addVariable("TopPreBBlockActive",1);  
  Control.addVariable("TopPreBBlockSide",0);  
  Control.addVariable("TopPreBBlockWidth",4.0);  
  Control.addVariable("TopPreBBlockHeight",12.0);  
  Control.addVariable("TopPreBBlockLength",10.8);  
  Control.addVariable("TopPreBBlockWaterMat","H2O");
  Control.addVariable("TopPreBBlockTemp",20);
  Control.addVariable("TopPreBBlockNLayers",2);  
  Control.addVariable("TopPreBBlockWallThick1",0.2);   
  Control.addVariable("TopPreBBlockWallMat1","Aluminium");  
  Control.addVariable("TopPreBBlockWallTemp1",20.0);  
  //
  Control.addVariable("TopModXStep",0.0);  
  Control.addVariable("TopModYStep",0.0);  
  Control.addVariable("TopModZStep",18.0);
  Control.addVariable("TopModXYangle",54.850); 
  Control.addVariable("TopModZangle",0.0);
  Control.addVariable("TopModRadius",8.0);
  Control.addVariable("TopModHeight",8.0);
  Control.addVariable("TopModMat","ParaH2");
  Control.addVariable("TopModTemp",20.0);
  Control.addVariable("TopModNLayers",7);
  // al layer
  Control.addVariable("TopModHGap1",0.3);
  Control.addVariable("TopModRadGap1",0.3);
  Control.addVariable("TopModMaterial1","Aluminium");  // Al materk
  Control.addVariable("TopModTemp1",20.0);  
  // Vac gap
  Control.addVariable("TopModHGap2",0.5);
  Control.addVariable("TopModRadGap2",0.5);
  Control.addVariable("TopModMaterial2","Void"); 
  // Next Al layer
  Control.addVariable("TopModHGap3",0.2);
  Control.addVariable("TopModRadGap3",0.5);
  Control.addVariable("TopModMaterial3","Aluminium"); 
  Control.addVariable("TopModTemp3",77.0);  
  // He Layer
  Control.addVariable("TopModHGap4",0.2);
  Control.addVariable("TopModRadGap4",0.2);
  Control.addVariable("TopModMaterial4","Void"); 
  // Outer Layer
  Control.addVariable("TopModHGap5",0.2);
  Control.addVariable("TopModRadGap5",0.2);
  Control.addVariable("TopModMaterial5","Aluminium"); 
  Control.addVariable("TopModTemp5",300.0); 
  // Clearance
  Control.addVariable("TopModHGap6",0.2);
  Control.addVariable("TopModRadGap6",0.2);
  Control.addVariable("TopModMaterial6","Void"); 

  Control.addVariable("TopModNConic",0);
  Control.addVariable("TopModNWedge",0);

  // TOP A FLIGHT
  Control.addVariable("TopAFlightXStep", 0.0);      // Step from centre
  Control.addVariable("TopAFlightZStep", 0.0);      // Step from centre
  Control.addVariable("TopAFlightAngleXY1", 60.0);  // Angle out
  Control.addVariable("TopAFlightAngleXY2", 60.0);  // Angle out
  Control.addVariable("TopAFlightXYangle", 180.0);
  Control.addVariable("TopAFlightZangle", 0.0);
  Control.addVariable("TopAFlightAngleZTop", 1.0);  // Step down angle !!! 1.455 is too much - fight line cuts the Bilbao target wheel
  Control.addVariable("TopAFlightAngleZBase", 1.0); // Step up angle
  //  Control.Parse("TopFlyTotalHeight");
  Control.addVariable("TopAFlightHeight", 2);     // Full height = TopFlyTotalHeight
  Control.addVariable("TopAFlightWidth", 10.7);     // Full width
  Control.addVariable("TopAFlightNLiner", 1);      // Liner
  Control.addVariable("TopAFlightLinerThick1", 0.3);      // Liner
  Control.addVariable("TopAFlightLinerMat1","Aluminium");      // Liner

  Control.addVariable("TopBFlightXStep", 0.0);      // Step from centre
  Control.addVariable("TopBFlightZStep", 0.0);      // Step from centre
  Control.addVariable("TopBFlightAngleXY1", 60.0);  // Angle out
  Control.addVariable("TopBFlightAngleXY2", 60.0);  // Angle out
  Control.addVariable("TopBFlightXYangle", 0.0);
  Control.addVariable("TopBFlightZangle", 0.0);
  Control.Parse("TopAFlightAngleZTop");
  Control.addVariable("TopBFlightAngleZTop");  // Step down angle
  Control.Parse("TopAFlightAngleZBase");
  Control.addVariable("TopBFlightAngleZBase"); // Step up angle
  Control.Parse("TopAFlightHeight");
  Control.addVariable("TopBFlightHeight");
  Control.addVariable("TopBFlightWidth", 10.7);     // Full width
  Control.addVariable("TopBFlightNLiner", 1);     
  Control.Parse("TopAFlightLinerThick1");
  Control.addVariable("TopBFlightLinerThick1"); 
  Control.addVariable("TopBFlightLinerMat1","Aluminium");

  
  Control.addVariable("ProtonTubeXStep",0.0);  
  Control.addVariable("ProtonTubeYStep",0.0);  
  Control.addVariable("ProtonTubeZStep",0.0);
  Control.addVariable("ProtonTubeXYangle",0.0); 
  Control.addVariable("ProtonTubeZangle",0.0);

  Control.addVariable("ProtonTubeNSection",4);

  Control.addVariable("ProtonTubeRadius1",11.5);
  Control.addVariable("ProtonTubeLength1",120.0); //from mod centre leftside
  Control.addVariable("ProtonTubeZcut1",5.35); //cut Z planes
  Control.addVariable("ProtonTubeWallThick1",0.0);
  Control.addVariable("ProtonTubeInnerMat1","helium");  // mat : 2000
  Control.addVariable("ProtonTubeWallMat1","CastIron"); // mat : 26316

  Control.addVariable("ProtonTubeRadius2",10.5);
  Control.addVariable("ProtonTubeLength2",200.0);
  Control.addVariable("ProtonTubeZcut2",0.0); 
  Control.addVariable("ProtonTubeWallThick2",1.0);
  Control.addVariable("ProtonTubeInnerMat2","helium");  // mat : 2000
  Control.addVariable("ProtonTubeWallMat2","CastIron"); // mat : 26316

  Control.addVariable("ProtonTubeRadius3",10.5);
  Control.addVariable("ProtonTubeLength3",127.5);
  Control.addVariable("ProtonTubeZcut3",0.0);
  Control.addVariable("ProtonTubeWallThick3",1.0);
  Control.addVariable("ProtonTubeInnerMat3","helium");   // mat : 2000
  Control.addVariable("ProtonTubeWallMat3","CastIron");  // mat : 26316

  Control.addVariable("ProtonTubeRadius4",10.5);
  // Control.addVariable("ProtonTubeLength4",147.5);
  Control.addVariable("ProtonTubeLength4",152.5);
  Control.addVariable("ProtonTubeZcut4",0.0);
  Control.addVariable("ProtonTubeWallThick4",1.0);
  Control.addVariable("ProtonTubeInnerMat4","Void");
  Control.addVariable("ProtonTubeWallMat4","CastIron");  // mat: 26316


  Control.addVariable("BeamMonitorXStep",0.0);
  Control.addVariable("BeamMonitorYStep",450.0);
  Control.addVariable("BeamMonitorZStep",0.0);
  Control.addVariable("BeamMonitorXYangle",0.0);
  Control.addVariable("BeamMonitorZangle",0.0);
  
  Control.addVariable("BeamMonitorBoxSide",70.0);
  Control.addVariable("BeamMonitorBoxNSections",5); //other are symmetric

  Control.addVariable("BeamMonitorBoxRadius1",11.5);
  Control.addVariable("BeamMonitorBoxThick1",2.5);
  Control.addVariable("BeamMonitorBoxMat1","helium");  // mat:2000
  Control.addVariable("BeamMonitorBoxRadius2",32.5);
  Control.addVariable("BeamMonitorBoxThick2",10.0);
  Control.addVariable("BeamMonitorBoxMat2","CastIron");  // mat: 26000
  Control.addVariable("BeamMonitorBoxRadius3",20.0);
  Control.addVariable("BeamMonitorBoxThick3",2.5);
  Control.addVariable("BeamMonitorBoxMat3","CastIron");  // mat 26316
  Control.addVariable("BeamMonitorBoxRadius4",17.5);
  Control.addVariable("BeamMonitorBoxThick4",17.5);
  Control.addVariable("BeamMonitorBoxMat4","CastIron");  // mat 26316
  Control.addVariable("BeamMonitorBoxRadius5",25.0);
  Control.addVariable("BeamMonitorBoxThick5",5.0);
  Control.addVariable("BeamMonitorBoxMat5","Aluminium");  // mat 13060

  Control.addVariable("BeamMonitorBoxSide5",50.0);
  Control.addVariable("BeamMonitorBoxHeightA5",14.60);
  Control.addVariable("BeamMonitorBoxHeightB5",8.60);
  Control.addVariable("BeamMonitorBoxHeightC5",7.5);
  Control.addVariable("BeamMonitorBoxHeightD5",7.0);

  Control.addVariable("BeamMonitorBoxWidthA5",21.40);
  Control.addVariable("BeamMonitorBoxWidthB5",20.30);
  Control.addVariable("BeamMonitorBoxWidthC5",19.80);

  Control.addVariable("BeamMonitorBoxThickA5",3.2);
  Control.addVariable("BeamMonitorBoxThickB5",0.5);
  Control.addVariable("BeamMonitorBoxThickC5",0.2);

  Control.addVariable("BeamMonitorBoxTubeN",33);
  Control.addVariable("BeamMonitorBoxTubeRadius",0.27);
  Control.addVariable("BeamMonitorBoxTubeThick",0.03);

  // Control.addVariable("BeamMonitorBoxTubeThick",0.03);
  Control.addVariable("BeamMonitorBoxTubeHeMat","helium");
  Control.addVariable("BeamMonitorBoxTubeAlMat","Aluminium");
  Control.addVariable("BeamMonitorBoxExtHeMat","helium");


  Control.addVariable("ProtonBeamViewRadius",4.0);  


 
  Control.addVariable("WheelShaftNLayers",3);
  Control.addVariable("WheelShaftHeight",435.0);
  Control.addVariable("WheelShaftRadius",32.0);
  Control.addVariable("WheelShaftCoolThick",1.0);
  Control.addVariable("WheelShaftCladThick",0.5);
  Control.addVariable("WheelShaftVoidThick",0.8);

  Control.addVariable("WheelCladShaftMat","Iron");
  Control.addVariable("WheelMainShaftMat","Iron");

  Control.addVariable("WheelXStep",0.0);  
  Control.addVariable("WheelYStep",113.0);  
  Control.addVariable("WheelZStep",0.0);
  Control.addVariable("WheelXYangle",0.0); 
  Control.addVariable("WheelZangle",0.0);
  Control.addVariable("WheelTargetHeight",8.0);
  Control.addVariable("WheelTemp",600.0);
  Control.addVariable("WheelCoolantThickIn",0.65);
  Control.addVariable("WheelCoolantThickOut",0.15);
  Control.addVariable("WheelCaseThick",0.5);
  Control.addVariable("WheelVoidThick",1.0);

  Control.addVariable("WheelInnerRadius",84.27);
  Control.addVariable("WheelCoolantRadiusIn",114.5);
  Control.addVariable("WheelCoolantRadiusOut",124.8);
  Control.addVariable("WheelCaseRadius",125.0);
  Control.addVariable("WheelVoidRadius",126.0);

  Control.addVariable("WheelWMat","Tungsten");
  Control.addVariable("WheelSteelMat","SS316L");
  Control.addVariable("WheelHeMat",0);
  Control.addVariable("WheelInnerMat","SS316L");

  Control.addVariable("WheelNLayers",24);

  Control.addVariable("WheelRadius1",85.65);
  Control.addVariable("WheelMatTYPE1",2);

  Control.addVariable("WheelRadius2",97.02);
  Control.addVariable("WheelMatTYPE2",3);

  Control.addVariable("WheelRadius3",97.52);
  Control.addVariable("WheelMatTYPE3",2);

  Control.addVariable("WheelRadius4",102.37);
  Control.addVariable("WheelMatTYPE4",3);

  Control.addVariable("WheelRadius5",102.77);    // WRONG
  Control.addVariable("WheelMatTYPE5",2);

  Control.addVariable("WheelRadius6",106.97);
  Control.addVariable("WheelMatTYPE6",3);

  Control.addVariable("WheelRadius7",107.29);
  Control.addVariable("WheelMatTYPE7",2);

  Control.addVariable("WheelRadius8",110.49);
  Control.addVariable("WheelMatTYPE8",3);

  Control.addVariable("WheelRadius9",110.78);
  Control.addVariable("WheelMatTYPE9",2);

  Control.addVariable("WheelRadius10",112.78);
  Control.addVariable("WheelMatTYPE10",3);

  Control.addVariable("WheelRadius11",113.05);
  Control.addVariable("WheelMatTYPE11",2);

  Control.addVariable("WheelRadius12",114.65);
  Control.addVariable("WheelMatTYPE12",3); 

  Control.addVariable("WheelRadius13",114.9);
  Control.addVariable("WheelMatTYPE13",2); 

  Control.addVariable("WheelRadius14",116.5);
  Control.addVariable("WheelMatTYPE14",3); 

  Control.addVariable("WheelRadius15",116.75);
  Control.addVariable("WheelMatTYPE15",2); 

  Control.addVariable("WheelRadius16",118.35);
  Control.addVariable("WheelMatTYPE16",3); 

  Control.addVariable("WheelRadius17",118.6);
  Control.addVariable("WheelMatTYPE17",2); 

  Control.addVariable("WheelRadius18",120);
  Control.addVariable("WheelMatTYPE18",3); 

  Control.addVariable("WheelRadius19",120.25);
  Control.addVariable("WheelMatTYPE19",2); 

  Control.addVariable("WheelRadius20",121.65);
  Control.addVariable("WheelMatTYPE20",3); 

  Control.addVariable("WheelRadius21",121.9);
  Control.addVariable("WheelMatTYPE21",2); 

  Control.addVariable("WheelRadius22",123.1);
  Control.addVariable("WheelMatTYPE22",3); 

  Control.addVariable("WheelRadius23",123.35);
  Control.addVariable("WheelMatTYPE23",2); 

  Control.addVariable("WheelRadius24",124.55);
  Control.addVariable("WheelMatTYPE24",3); 

  Control.addVariable("BeRefXStep",0.0);  
  Control.addVariable("BeRefYStep",0.0);  
  Control.addVariable("BeRefZStep",0.0);
  Control.addVariable("BeRefXYangle",0.0); 
  Control.addVariable("BeRefZangle",0.0);
  Control.addVariable("BeRefRadius",34.3);
  Control.addVariable("BeRefHeight",74.2);
  Control.addVariable("BeRefWallThick",0.3);
  Control.addVariable("BeRefWallThickLow",0.0);
  Control.addVariable("BeRefTargetSepThick",13.0);
  Control.addVariable("BeRefLowVoidThick",2.3);
  Control.addVariable("BeRefTopVoidThick",2.3);
  Control.addVariable("BeRefRefMat","Be5H2O");
  //  Control.addVariable("BeRefRefMat","Be300K");


  Control.addVariable("BeRefWallMat","Aluminium");
  Control.addVariable("BeRefTargSepMat","Void");

  ///< TODO : Fix double variable dependency !!!
  
  Control.addVariable("BeRefInnerStructureWaterDiscThick", 0.6);
  Control.addVariable("BeRefInnerStructureWaterDiscMat", "H2O");
  Control.addVariable("BeRefInnerStructureWaterDiscWallThick", 0.3);
  Control.addVariable("BeRefInnerStructureWaterDiscWallMat", "Aluminium");

  Control.addVariable("BeRefInnerStructureBeRadius", 12.5);
  Control.addVariable("BeRefInnerStructureBeMat", "Be10H2O");
  Control.addVariable("BeRefInnerStructureBeWallThick", 0.3);
  Control.addVariable("BeRefInnerStructureBeWallMat", "Aluminium");

  Control.addVariable("TopBeRefWaterDiscNLayers",2);
  Control.addVariable("TopBeRefWaterDiscHeight0",0.3);
  Control.addVariable("TopBeRefWaterDiscDepth0",0.3);
  Control.Parse("BeRefRadius");
  Control.addVariable("TopBeRefWaterDiscRadius0");
  Control.addVariable("TopBeRefWaterDiscMat0","H2O");
  Control.addVariable("TopBeRefWaterDiscHeight1",0.4);
  Control.addVariable("TopBeRefWaterDiscDepth1",0.0);
  Control.Parse("BeRefWallThick");
  Control.addVariable("TopBeRefWaterDiscRadius1");
  Control.addVariable("TopBeRefWaterDiscMat1", "Aluminium");

  Control.addVariable("LowBeRefWaterDiscNLayers",2);
  Control.addVariable("LowBeRefWaterDiscHeight0",0.3);
  Control.addVariable("LowBeRefWaterDiscDepth0",0.3);
  Control.Parse("BeRefRadius");
  Control.addVariable("LowBeRefWaterDiscRadius0");
  Control.addVariable("LowBeRefWaterDiscMat0","H2O");
  Control.addVariable("LowBeRefWaterDiscHeight1",0.4);
  Control.addVariable("LowBeRefWaterDiscDepth1",0.0);
  Control.Parse("BeRefWallThick");
  Control.addVariable("LowBeRefWaterDiscRadius1");
  Control.addVariable("LowBeRefWaterDiscMat1", "Aluminium");

  Control.addVariable("BulkXStep",0.0);
  Control.addVariable("BulkYStep",0.0);
  Control.addVariable("BulkZStep",0.0);
  Control.addVariable("BulkXYangle",0.0);
  Control.addVariable("BulkZangle",0.0);
  Control.addVariable("BulkNLayer",3);

  Control.Parse("BeRefRadius+BeRefWallThick+0.2");
  Control.addVariable("BulkRadius1");
  /*!
    \todo : This is ugly conterintuative
    and going to break if anyone make a change
  */

  Control.Parse("BeRefHeight/2.0+BeRefWallThick+"
                "TopBeRefWaterDiscHeight0+TopBeRefWaterDiscDepth0+"
		"TopBeRefWaterDiscHeight1+0.2");
  Control.addVariable("BulkHeight1");
  Control.Parse("BeRefHeight/2.0+BeRefWallThick+LowBeRefWaterDiscHeight0"
		"+LowBeRefWaterDiscDepth0+LowBeRefWaterDiscHeight1+0.2");
  Control.addVariable("BulkDepth1");
  Control.addVariable("BulkMat1","Void");

  Control.addVariable("BulkRadius2",65.0);
  Control.addVariable("BulkHeight2",75.0);
  Control.addVariable("BulkDepth2",75.0);
  Control.addVariable("BulkMat2","Iron10H2O");        // SA: using hand-made mixture because CL can't generate volume fractions

  // Bulk steel layer [No individual guides]
  Control.addVariable("BulkRadius3",200.0);
  Control.addVariable("BulkHeight3",200.0);
  Control.addVariable("BulkDepth3",200.0);
  Control.addVariable("BulkMat3","Iron10H2O");        // SA: using hand-made mixture because CL can't generate volume fractions


  // BULK FLIGHT VOID
  Control.addVariable("BulkLAFlightSideIndex",-2);   // Index
  Control.addVariable("BulkLAFlightXStep",0.0);      // Step from centre
  Control.addVariable("BulkLAFlightZStep",0.0);      // Step from centre
  Control.addVariable("BulkLAFlightAngleXY1",30.0);  // Angle out
  Control.addVariable("BulkLAFlightAngleXY2",30.0);  // Angle out
  Control.addVariable("BulkLAFlightAngleZTop",0.0);  // Step down angle
  Control.addVariable("BulkLAFlightAngleZBase",0.0); // Step up angle
  Control.addVariable("BulkLAFlightHeight",10.0);    // Full height
  Control.addVariable("BulkLAFlightWidth",23.3);     // Full width
  Control.addVariable("BulkLAFlightNLiner",0);       // Liner


  // SHUTTER BAY
  Control.addVariable("ShutterBayXStep",0.0);  
  Control.addVariable("ShutterBayYStep",0.0);  
  Control.addVariable("ShutterBayZStep",0.0);
  Control.addVariable("ShutterBayXYangle",0.0); 
  Control.addVariable("ShutterBayZangle",0.0);
  Control.addVariable("ShutterBayRadius",550.0);
  Control.addVariable("ShutterBayHeight",400.0);
  Control.addVariable("ShutterBayDepth",400.0);
  Control.addVariable("ShutterBayMat","CastIron");

  // Guide BAY [ All 4 same ]
  Control.addVariable("GuideBayXStep",0.0);  
  Control.addVariable("GuideBayYStep",0.0);  
  Control.addVariable("GuideBayZStep",0.0);
  Control.addVariable("GuideBayZangle",0.0);
  Control.addVariable("GuideBayViewAngle",128.0); 
  Control.addVariable("GuideBayInnerHeight",11.0);
  Control.addVariable("GuideBayInnerDepth",11.0);
  Control.addVariable("GuideBayMidRadius",170.0);
  Control.addVariable("GuideBayHeight",32.0);
  Control.addVariable("GuideBayDepth",40.0);
  Control.addVariable("GuideBayMat","CastIron");
  Control.addVariable("GuideBay1XYangle",0.0); 
  Control.addVariable("GuideBay2XYangle",180.0); 
  Control.addVariable("GuideBay3XYangle",0.0); 
  Control.addVariable("GuideBay4XYangle",180.0); 
  Control.addVariable("GuideBay1NItems",11);  
  Control.addVariable("GuideBay2NItems",11);  
  Control.addVariable("GuideBay3NItems",11);  
  Control.addVariable("GuideBay4NItems",11);


  EssBeamLinesVariables(Control);
  EssReflectorVariables(Control);
  EssSANSVariables(Control);
  ODINvariables(Control);
  LOKIvariables(Control);
  NMXvariables(Control);
  EssButterflyModerator(Control);
  EssWheel(Control);
  EssBunkerVariables(Control);

  F5Variables(Control);

  Control.addVariable("sdefEnergy",2000.0);  

   // STUFF FOR 90 angle:
  /*
  Control.addVariable("LowPreABlockSide",0);  
  Control.addVariable("LowPreBBlockSide",0);  
  Control.addVariable("LowPreABlockActive",0);  
  Control.addVariable("LowPreBBlockActive",0);  
  Control.addVariable("TopPreABlockActive",0);  
  Control.addVariable("TopPreBBlockActive",0);  
  // */
  /*
  Control.addVariable("GuideBay2XYangle",90.0); 
  Control.addVariable("GuideBay4XYangle",-90.0); 

  Control.addVariable("LowBFlightMasterXY",-90.0);  // Angle out
  Control.addVariable("LowPreViewAngle2",180.0-90.0);  
  Control.addVariable("LowPreBBlockXYangle",-90.0);  // Angle out

  Control.addVariable("LowAFlightXStep",1.5);     // Angle
  Control.addVariable("LowBFlightXStep",-1.5);     // Angle
  */
  return;
}

void
EssButterflyModerator(FuncDataBase& Control)
  /*!
    Create all the Conic moderator option variables
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("essVariables[F]","EssButterflyModerator");

  Control.addVariable("LowFlyXStep",0.0);  
  Control.addVariable("LowFlyYStep",0.0);  
  Control.addVariable("LowFlyZStep",0.0);
  Control.addVariable("LowFlyXYangle",90.0);
  Control.addVariable("LowFlyZangle",180.0);
  Control.addVariable("LowFlyTotalHeight",7.6);
  
  Control.addVariable("LowFlyLeftLobeXStep",1.0);  
  Control.addVariable("LowFlyLeftLobeYStep",0.0);  

  Control.addVariable("LowFlyLeftLobeCorner1",Geometry::Vec3D(0,0.45,0));
  Control.addVariable("LowFlyLeftLobeCorner2",Geometry::Vec3D(-14.4,-13.2,0));
  Control.addVariable("LowFlyLeftLobeCorner3",Geometry::Vec3D(14.4,-13.2,0));
  
  Control.addVariable("LowFlyLeftLobeRadius1",5.0);
  Control.addVariable("LowFlyLeftLobeRadius2",2.506);
  Control.addVariable("LowFlyLeftLobeRadius3",2.506);

  Control.addVariable("LowFlyLeftLobeModMat","LH05ortho");
  Control.addVariable("LowFlyLeftLobeModTemp",20.0);

  Control.addVariable("LowFlyLeftLobeNLayers",4);
  Control.addVariable("LowFlyLeftLobeThick1",0.3);
  Control.addVariable("LowFlyLeftLobeMat1","Aluminium20K");

  Control.addVariable("LowFlyLeftLobeHeight1",0.3);
  Control.addVariable("LowFlyLeftLobeDepth1",0.3);
  Control.addVariable("LowFlyLeftLobeTemp1",20.0);
  
  Control.addVariable("LowFlyLeftLobeThick2",0.5);
  Control.addVariable("LowFlyLeftLobeMat2","Void");

  Control.addVariable("LowFlyLeftLobeHeight2",0.5);
  Control.addVariable("LowFlyLeftLobeDepth2",0.5);

  Control.addVariable("LowFlyLeftLobeThick3",0.3);
  Control.addVariable("LowFlyLeftLobeMat3","Aluminium");

  Control.addVariable("LowFlyLeftLobeHeight3",0.0);
  Control.addVariable("LowFlyLeftLobeDepth3",0.0);

  Control.addVariable("LowFlyFlowGuideBaseThick",0.2);
  Control.addVariable("LowFlyFlowGuideBaseLen",8.5);
  Control.addVariable("LowFlyFlowGuideArmThick",0.2);
  Control.addVariable("LowFlyFlowGuideArmLen",8.0);
  Control.addVariable("LowFlyFlowGuideBaseArmSep",0.1);
  Control.addVariable("LowFlyFlowGuideBaseOffset",Geometry::Vec3D(0,-10.7,0));
  Control.addVariable("LowFlyFlowGuideArmOffset",Geometry::Vec3D(0,-9,0));
  Control.addVariable("LowFlyFlowGuideWallMat","Aluminium20K");
  Control.addVariable("LowFlyFlowGuideWallTemp",20.0);
  
  Control.addVariable("LowFlyRightLobeXStep",-1.0);  
  Control.addVariable("LowFlyRightLobeYStep",0.0);  

  Control.addVariable("LowFlyRightLobeCorner1",Geometry::Vec3D(0,0.45,0));
  Control.addVariable("LowFlyRightLobeCorner2",Geometry::Vec3D(-14.4,-13.2,0));
  Control.addVariable("LowFlyRightLobeCorner3",Geometry::Vec3D(14.4,-13.2,0));

  Control.addVariable("LowFlyRightLobeRadius1",5.0);
  Control.addVariable("LowFlyRightLobeRadius2",2.506);
  Control.addVariable("LowFlyRightLobeRadius3",2.506);

  Control.addVariable("LowFlyRightLobeModMat","LH05ortho");
  Control.addVariable("LowFlyRightLobeModTemp",20.0);

  Control.addVariable("LowFlyRightLobeNLayers",4);
  Control.addVariable("LowFlyRightLobeThick1",0.3);
  Control.addVariable("LowFlyRightLobeMat1","Aluminium20K");

  Control.addVariable("LowFlyRightLobeHeight1",0.3);
  Control.addVariable("LowFlyRightLobeDepth1",0.3);
  Control.addVariable("LowFlyRightLobeTemp1",20.0);
  
  Control.addVariable("LowFlyRightLobeThick2",0.5);
  Control.addVariable("LowFlyRightLobeMat2","Void");

  Control.addVariable("LowFlyRightLobeHeight2",0.5);
  Control.addVariable("LowFlyRightLobeDepth2",0.5);

  Control.addVariable("LowFlyRightLobeThick3",0.3);
  Control.addVariable("LowFlyRightLobeMat3","Aluminium");

  Control.addVariable("LowFlyRightLobeHeight3",0.0);
  Control.addVariable("LowFlyRightLobeDepth3",0.0);

  Control.addVariable("LowFlyMidWaterCutLayer",3);
  Control.addVariable("LowFlyMidWaterMidYStep",4.635);
  Control.addVariable("LowFlyMidWaterMidAngle",90);
  Control.addVariable("LowFlyMidWaterLength",10.98);
 

  Control.addVariable("LowFlyMidWaterWallThick",0.2);
  Control.addVariable("LowFlyMidWaterModMat","H2O");
  Control.addVariable("LowFlyMidWaterWallMat","Aluminium");
  Control.addVariable("LowFlyMidWaterModTemp",300.0);

  Control.addVariable("LowFlyLeftWaterWidth",10.6672);
  Control.addVariable("LowFlyLeftWaterWallThick",0.347);
  Control.addVariable("LowFlyLeftWaterModMat","H2O");
  Control.addVariable("LowFlyLeftWaterWallMat","Aluminium");
  Control.addVariable("LowFlyLeftWaterModTemp",300.0);

  Control.addVariable("LowFlyRightWaterWidth",10.6672);
  Control.addVariable("LowFlyRightWaterWallThick",0.347);
  Control.addVariable("LowFlyRightWaterModMat","H2O");
  Control.addVariable("LowFlyRightWaterWallMat","Aluminium");
  Control.addVariable("LowFlyRightWaterModTemp",300.0);

  Control.addVariable("TopFlyXStep",0.0);  
  Control.addVariable("TopFlyYStep",0.0);  
  Control.addVariable("TopFlyZStep",0.0);
  Control.addVariable("TopFlyXYangle",90.0);
  Control.addVariable("TopFlyZangle",0.0);
  Control.addVariable("TopFlyTotalHeight",4.6);
  
  Control.addVariable("TopFlyLeftLobeXStep",1.0);  
  Control.addVariable("TopFlyLeftLobeYStep",0.0);  

  Control.addVariable("TopFlyLeftLobeCorner1",Geometry::Vec3D(0,0.45,0));
  Control.addVariable("TopFlyLeftLobeCorner2",Geometry::Vec3D(-14.4,-13.2,0));
  Control.addVariable("TopFlyLeftLobeCorner3",Geometry::Vec3D(14.4,-13.2,0));
  
  Control.addVariable("TopFlyLeftLobeRadius1",5.0);
  Control.addVariable("TopFlyLeftLobeRadius2",2.506);
  Control.addVariable("TopFlyLeftLobeRadius3",2.506);

  Control.addVariable("TopFlyLeftLobeModMat","LH05ortho");
  Control.addVariable("TopFlyLeftLobeModTemp",20.0);

  Control.addVariable("TopFlyLeftLobeNLayers",4);
  Control.addVariable("TopFlyLeftLobeThick1",0.3);
  Control.addVariable("TopFlyLeftLobeMat1","Aluminium20K");

  Control.addVariable("TopFlyLeftLobeHeight1",0.3);
  Control.addVariable("TopFlyLeftLobeDepth1",0.3);
  Control.addVariable("TopFlyLeftLobeTemp1",20.0);
  
  Control.addVariable("TopFlyLeftLobeThick2",0.5);
  Control.addVariable("TopFlyLeftLobeMat2","Void");

  Control.addVariable("TopFlyLeftLobeHeight2",0.5);
  Control.addVariable("TopFlyLeftLobeDepth2",0.5);

  Control.addVariable("TopFlyLeftLobeThick3",0.3);
  Control.addVariable("TopFlyLeftLobeMat3","Aluminium");

  Control.addVariable("TopFlyLeftLobeHeight3",0.0);
  Control.addVariable("TopFlyLeftLobeDepth3",0.0);

  Control.addVariable("TopFlyFlowGuideBaseThick",0.2);
  Control.addVariable("TopFlyFlowGuideBaseLen",8.5);
  Control.addVariable("TopFlyFlowGuideArmThick",0.2);
  Control.addVariable("TopFlyFlowGuideArmLen",8.0);
  Control.addVariable("TopFlyFlowGuideBaseArmSep",0.1);
  Control.addVariable("TopFlyFlowGuideBaseOffset",Geometry::Vec3D(0,-10.7,0));
  Control.addVariable("TopFlyFlowGuideArmOffset",Geometry::Vec3D(0,-9,0));
  Control.addVariable("TopFlyFlowGuideWallMat","Aluminium20K");
  Control.addVariable("TopFlyFlowGuideWallTemp",20.0);
  
  Control.addVariable("TopFlyRightLobeXStep",-1.0);  
  Control.addVariable("TopFlyRightLobeYStep",0.0);  

  Control.addVariable("TopFlyRightLobeCorner1",Geometry::Vec3D(0,0.45,0));
  Control.addVariable("TopFlyRightLobeCorner2",Geometry::Vec3D(-14.4,-13.2,0));
  Control.addVariable("TopFlyRightLobeCorner3",Geometry::Vec3D(14.4,-13.2,0));

  Control.addVariable("TopFlyRightLobeRadius1",5.0);
  Control.addVariable("TopFlyRightLobeRadius2",2.506);
  Control.addVariable("TopFlyRightLobeRadius3",2.506);

  Control.addVariable("TopFlyRightLobeModMat","LH05ortho");
  Control.addVariable("TopFlyRightLobeModTemp",20.0);

  Control.addVariable("TopFlyRightLobeNLayers",4);
  Control.addVariable("TopFlyRightLobeThick1",0.3);
  Control.addVariable("TopFlyRightLobeMat1","Aluminium20K");

  Control.addVariable("TopFlyRightLobeHeight1",0.3);
  Control.addVariable("TopFlyRightLobeDepth1",0.3);
  Control.addVariable("TopFlyRightLobeTemp1",20.0);
  
  Control.addVariable("TopFlyRightLobeThick2",0.5);
  Control.addVariable("TopFlyRightLobeMat2","Void");

  Control.addVariable("TopFlyRightLobeHeight2",0.5);
  Control.addVariable("TopFlyRightLobeDepth2",0.5);

  Control.addVariable("TopFlyRightLobeThick3",0.3);
  Control.addVariable("TopFlyRightLobeMat3","Aluminium");

  Control.addVariable("TopFlyRightLobeHeight3",0.0);
  Control.addVariable("TopFlyRightLobeDepth3",0.0);


  Control.addVariable("TopFlyMidWaterCutLayer",3);
  Control.addVariable("TopFlyMidWaterMidYStep",4.635);
  Control.addVariable("TopFlyMidWaterMidAngle",90);
  Control.addVariable("TopFlyMidWaterLength",10.98);
 

  Control.addVariable("TopFlyMidWaterWallThick",0.2);
  Control.addVariable("TopFlyMidWaterModMat","H2O");
  Control.addVariable("TopFlyMidWaterWallMat","Aluminium");
  Control.addVariable("TopFlyMidWaterModTemp",300.0);

  Control.addVariable("TopFlyLeftWaterWidth",10.6672);
  Control.addVariable("TopFlyLeftWaterWallThick",0.347);
  Control.addVariable("TopFlyLeftWaterModMat","H2O");
  Control.addVariable("TopFlyLeftWaterWallMat","Aluminium");
  Control.addVariable("TopFlyLeftWaterModTemp",300.0);

  Control.addVariable("TopFlyRightWaterWidth",10.6672);
  Control.addVariable("TopFlyRightWaterWallThick",0.347);
  Control.addVariable("TopFlyRightWaterModMat","H2O");
  Control.addVariable("TopFlyRightWaterWallMat","Aluminium");
  Control.addVariable("TopFlyRightWaterModTemp",300.0);

  Control.addVariable("LowPreModNLayers",5);
  Control.addVariable("LowPreModHeight0",1.5);
  Control.addVariable("LowPreModDepth0",1.5);
  Control.addVariable("LowPreModRadius0",30.0);
  Control.addVariable("LowPreModMat0","H2O");
  Control.addVariable("LowPreModHeight1",0);
  Control.addVariable("LowPreModDepth1",0);
  Control.addVariable("LowPreModRadius1",0.3);
  Control.addVariable("LowPreModMat1","Aluminium");
  Control.addVariable("LowPreModHeight2",0);
  Control.addVariable("LowPreModDepth2",0);
  Control.Parse("BeRefRadius-LowPreModRadius0-LowPreModRadius1");
  Control.addVariable("LowPreModRadius2");
  Control.addVariable("LowPreModMat2","Iron10H2O");
  Control.addVariable("LowPreModHeight3",0.3);
  Control.addVariable("LowPreModDepth3",0.3);
  Control.Parse("BeRefWallThick");
  Control.addVariable("LowPreModRadius3");
  Control.addVariable("LowPreModMat3","Aluminium");
  Control.addVariable("LowPreModHeight4", 0.0);
  Control.addVariable("LowPreModDepth4",  0.0);
  Control.addVariable("LowPreModRadius4", 0.0);
  Control.addVariable("LowPreModMat4","Void");
  //  Control.Parse("LowAFlightAngleZBase");
  Control.addVariable("LowPreModTiltAngle", 0.0);

  Control.addVariable("LowPreModFlowGuideWallThick", 0.3);
  Control.addVariable("LowPreModFlowGuideWallMat", "Aluminium");
  Control.addVariable("LowPreModFlowGuideNBaffles", 9);
  Control.addVariable("LowPreModFlowGuideGapWidth", 3);

  Control.addVariable("TopPreModNLayers",5);
  Control.addVariable("TopPreModHeight0",1.5);
  Control.addVariable("TopPreModDepth0",1.5);
  Control.addVariable("TopPreModRadius0",30.0);
  Control.addVariable("TopPreModMat0","H2O");
  Control.addVariable("TopPreModHeight1",0);
  Control.addVariable("TopPreModDepth1",0);
  Control.addVariable("TopPreModRadius1",0.3);
  Control.addVariable("TopPreModMat1","Aluminium");
  Control.addVariable("TopPreModHeight2",0);
  Control.addVariable("TopPreModDepth2",0);
  Control.Parse("BeRefRadius-TopPreModRadius0-TopPreModRadius1");
  Control.addVariable("TopPreModRadius2");
  Control.addVariable("TopPreModMat2","Iron10H2O");
  Control.addVariable("TopPreModHeight3",0.3);
  Control.addVariable("TopPreModDepth3",0.3);
  Control.Parse("BeRefWallThick");
  Control.addVariable("TopPreModRadius3");
  Control.addVariable("TopPreModMat3","Aluminium");
  Control.addVariable("TopPreModHeight4", 0.0);
  Control.addVariable("TopPreModDepth4",  0.0);
  Control.addVariable("TopPreModRadius4", 0.0);
  Control.addVariable("TopPreModMat4","Void");
  //  Control.Parse("TopAFlightAngleZTop");
  Control.addVariable("TopPreModTiltAngle", 0.0);
  Control.addVariable("TopPreModTiltRadius", 20.0);

  Control.addVariable("TopPreWingMat", "H2O");
  Control.addVariable("TopPreWingThick", 0.8);
  Control.addVariable("TopPreWingWallMat", "Aluminium");
  Control.addVariable("TopPreWingWallThick", 0.3);
  Control.Parse("TopAFlightAngleZTop");
  Control.addVariable("TopPreWingTiltAngle");
  Control.addVariable("TopPreWingTiltRadius", 20.0);

  Control.addVariable("LowPreWingMat", "H2O");
  Control.addVariable("LowPreWingThick", 0.8);
  Control.addVariable("LowPreWingWallMat", "Aluminium");
  Control.addVariable("LowPreWingWallThick", 0.3);
  Control.Parse("LowAFlightAngleZTop");
  Control.addVariable("LowPreWingTiltAngle");
  Control.addVariable("LowPreWingTiltRadius", 20.0);


  Control.addVariable("TopCapWingMat", "H2O");
  Control.addVariable("TopCapWingThick", 0.8);
  Control.addVariable("TopCapWingWallMat", "Aluminium");
  Control.addVariable("TopCapWingWallThick", 0.3);
  Control.Parse("TopAFlightAngleZBase");
  Control.addVariable("TopCapWingTiltAngle");
  Control.addVariable("TopCapWingTiltRadius", 20.1);

  Control.addVariable("LowCapWingMat", "H2O");
  Control.addVariable("LowCapWingThick", 0.8);
  Control.addVariable("LowCapWingWallMat", "Aluminium");
  Control.addVariable("LowCapWingWallThick", 0.3);
  Control.Parse("LowAFlightAngleZBase");
  Control.addVariable("LowCapWingTiltAngle");
  Control.addVariable("LowCapWingTiltRadius", 20.1);

  Control.Parse("LowPreModFlowGuideWallThick");
  Control.addVariable("TopPreModFlowGuideWallThick");
  Control.addVariable("TopPreModFlowGuideWallMat", "Aluminium");
  Control.Parse("LowPreModFlowGuideNBaffles");
  Control.addVariable("TopPreModFlowGuideNBaffles");
  Control.Parse("LowPreModFlowGuideGapWidth");
  Control.addVariable("TopPreModFlowGuideGapWidth");

  Control.addVariable("LowCapModNLayers",4);
  Control.addVariable("LowCapModHeight0",0.5);
  Control.addVariable("LowCapModDepth0", 0.5);
  Control.addVariable("LowCapModRadius0",19.5);
  Control.addVariable("LowCapModMat0","H2O");
  Control.addVariable("LowCapModHeight1",0.3);
  Control.addVariable("LowCapModDepth1",0.0);
  Control.addVariable("LowCapModThick1",0.3);
  Control.addVariable("LowCapModMat1","Aluminium");
  Control.addVariable("LowCapModHeight2",0.0);
  Control.addVariable("LowCapModDepth2",0.0);
  Control.Parse("BeRefRadius-LowCapModRadius0-LowCapModRadius1");
  Control.addVariable("LowCapModRadius2");
  Control.addVariable("LowCapModMat2","Be5H2O");
  Control.addVariable("LowCapModHeight3",0);
  Control.addVariable("LowCapModDepth3", 0.3);
  Control.Parse("BeRefWallThick");
  Control.addVariable("LowCapModRadius3");
  //  Control.Parse("BeRefWallMat");
  Control.addVariable("LowCapModMat3", "Aluminium");
  //  Control.Parse("LowAFlightAngleZBase");
  Control.addVariable("LowCapModTiltAngle", 0.0);

  Control.addVariable("LowCapModFlowGuideWallThick", 0.3);
  Control.addVariable("LowCapModFlowGuideWallMat", "Aluminium");
  Control.addVariable("LowCapModFlowGuideNBaffles", 9);
  Control.addVariable("LowCapModFlowGuideGapWidth", 3);

  Control.addVariable("TopCapModNLayers",4);
  Control.addVariable("TopCapModHeight0",0.5);
  Control.addVariable("TopCapModDepth0", 0.5);
  Control.addVariable("TopCapModRadius0",19.5);
  Control.addVariable("TopCapModMat0","H2O");
  Control.addVariable("TopCapModHeight1",0.3);
  Control.addVariable("TopCapModDepth1",0.0);
  Control.addVariable("TopCapModThick1",0.3);
  Control.addVariable("TopCapModMat1","Aluminium");
  Control.addVariable("TopCapModHeight2",0.0);
  Control.addVariable("TopCapModDepth2",0.0);
  Control.Parse("BeRefRadius-TopCapModRadius0-TopCapModRadius1");
  Control.addVariable("TopCapModRadius2");
  Control.addVariable("TopCapModMat2","Be5H2O");
  Control.addVariable("TopCapModHeight3",0);
  Control.addVariable("TopCapModDepth3", 0.3);
  Control.Parse("BeRefWallThick");
  Control.addVariable("TopCapModRadius3");
  //  Control.Parse("BeRefWallMat");
  Control.addVariable("TopCapModMat3", "Aluminium");
  //  Control.Parse("TopAFlightAngleZBase");
  Control.addVariable("TopCapModTiltAngle", 0.0);
  Control.Parse("TopPreModTiltRadius");
  Control.addVariable("TopCapModTiltRadius");


  Control.Parse("LowCapModFlowGuideWallThick");
  Control.addVariable("TopCapModFlowGuideWallThick");
  Control.addVariable("TopCapModFlowGuideWallMat", "Aluminium");
  Control.Parse("LowCapModFlowGuideNBaffles");
  Control.addVariable("TopCapModFlowGuideNBaffles");
  Control.Parse("LowCapModFlowGuideGapWidth");
  Control.addVariable("TopCapModFlowGuideGapWidth");

  return;
}


  
void
EssReflectorVariables(FuncDataBase& Control)
  /*!
    Add ESS JRefl beamline 
    \param Control :: Data to add
   */
{
  ELog::RegMethod RegA("essVariables[F]","EssReflectorVariables");

  Control.addVariable("JReflXStep",0.0);       
  Control.addVariable("JReflYStep",0.0);       
  Control.addVariable("JReflZStep",0.0);       
  Control.addVariable("JReflXYAngle",0.0);       
  Control.addVariable("JReflZAngle",0.0);       
  Control.addVariable("JReflLength",3190.0);       
  Control.addVariable("JReflHeight",100.0);       
  Control.addVariable("JReflDepth",100.0);       
  Control.addVariable("JReflLeftWidth",30.0);       
  Control.addVariable("JReflRightWidth",30.0);       
  Control.addVariable("JReflFeMat","Stainless304");       
  Control.addVariable("JReflNShapes",3);       
  Control.addVariable("JReflNShapeLayers",3);       

  Control.addVariable("JRefl0TypeID","Tapper");
  Control.addVariable("JRefl0HeightStart",2.0);
  Control.addVariable("JRefl0HeightEnd",2.0);
  Control.addVariable("JRefl0WidthStart",10.0);
  Control.addVariable("JRefl0WidthEnd",25.0);
  Control.addVariable("JRefl0Length",1070.0);       

  Control.addVariable("JRefl1TypeID","Rectangle");
  Control.addVariable("JRefl1Height",2.0);
  Control.addVariable("JRefl1Width",25.0);
  Control.addVariable("JRefl1Length",1120.0);
  Control.addVariable("JRefl1ZAngle",0.25);       

  Control.addVariable("JRefl2TypeID","Tapper");
  Control.addVariable("JRefl2HeightStart",3.0);
  Control.addVariable("JRefl2HeightEnd",3.0);
  Control.addVariable("JRefl2WidthStart",25.0);
  Control.addVariable("JRefl2WidthEnd",10.0);
  Control.addVariable("JRefl2Length",1310.0);
  Control.addVariable("JRefl2ZAngle",-0.25);       


  Control.addVariable("JReflLayerMat0","Void");
  Control.addVariable("JReflLayerThick1",0.4);
  Control.addVariable("JReflLayerMat1","Glass");
  Control.addVariable("JReflLayerThick2",1.5);
  Control.addVariable("JReflLayerMat2","Void");
  return;
}

void
EssSANSVariables(FuncDataBase& Control)
  /*!
    Add ESS JRefl beamline 
    \param Control :: Data to add
   */
{
  ELog::RegMethod RegA("essVariables[F]","EssReflectorVariables");

  Control.addVariable("JSANSXStep",0.0);       
  Control.addVariable("JSANSYStep",0.0);       
  Control.addVariable("JSANSZStep",0.0);       
  Control.addVariable("JSANSXYAngle",0.0);       
  Control.addVariable("JSANSZAngle",0.0);       
  Control.addVariable("JSANSLength",3190.0);       
  Control.addVariable("JSANSHeight",100.0);       
  Control.addVariable("JSANSDepth",100.0);       
  Control.addVariable("JSANSLeftWidth",30.0);       
  Control.addVariable("JSANSRightWidth",30.0);       
  Control.addVariable("JSANSFeMat","Stainless304");       
  Control.addVariable("JSANSNShapes",2);       
  Control.addVariable("JSANSNShapeLayers",3);       

  Control.addVariable("JSANS0TypeID","Bend");
  Control.addVariable("JSANS0AHeight",16.0);
  Control.addVariable("JSANS0AWidth",16.0);
  Control.addVariable("JSANS0BWidth",16.0);
  Control.addVariable("JSANS0AngDir",0.0);
  Control.addVariable("JSANS0Length",400.0);       
  Control.addVariable("JSANS0Radius",4000.0);       

  Control.addVariable("JSANS1TypeID","Rectangle");
  Control.addVariable("JSANS1Height",16.0);
  Control.addVariable("JSANS1Width",16.0);
  Control.addVariable("JSANS1Length",1120.0);
  Control.addVariable("JSANS1ZAngle",0.0);       

  Control.addVariable("JSANSLayerMat0","Void");
  Control.addVariable("JSANSLayerThick1",0.4);
  Control.addVariable("JSANSLayerMat1","Glass");
  Control.addVariable("JSANSLayerThick2",1.5);
  Control.addVariable("JSANSLayerMat2","Void");
  return;
}


void
EssBunkerVariables(FuncDataBase& Control)
  /*!
    Create all the bunker variables
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("essVariables[F]","EssBunkerVariables");

  Control.addVariable("ABunkerLeftPhase",65.0);
  Control.addVariable("ABunkerRightPhase",65.0);
  Control.addVariable("ABunkerLeftAngle",0.0);
  Control.addVariable("ABunkerRightAngle",0.0);
  Control.addVariable("ABunkerNSectors",11);
  
  Control.addVariable("ABunkerWallRadius",1300.0);
  Control.addVariable("ABunkerFloorDepth",100.0);
  Control.addVariable("ABunkerRoofHeight",100.0);

  Control.addVariable("ABunkerWallThick",250.0);
  Control.addVariable("ABunkerSideThick",100.0);
  Control.addVariable("ABunkerRoofThick",100.0);
  Control.addVariable("ABunkerFloorThick",100.0);

  Control.addVariable("ABunkerWallMat","Steel71");

  Control.addVariable("ABunkerNLayers",12);
  Control.addVariable("ABunkerWallMat1","Tungsten");
  Control.addVariable("ABunkerWallMat2","Tungsten");
  Control.addVariable("ABunkerWallMat3","Stainless304");
  Control.addVariable("ABunkerWallMat4","Steel71");
  Control.addVariable("ABunkerWallMat9","Tungsten");
      
  
  return;
}

void
F5Variables(FuncDataBase& Control)
/*!
  Create DUMMY variables for all F5 collimators
 */
{
    Control.addVariable("F5Length", 440.0);
    Control.addVariable("F5Delta", 0.0);
    Control.addVariable("F5ViewWidth", 6);
    Control.addVariable("F5LinkPoint", 0.0);
    Control.addVariable("F5XB", 11.0);
    Control.addVariable("F5YB", -14.0);
    Control.addVariable("F5ZB", 14.0);
    Control.addVariable("F5XC", 11.0);
    Control.addVariable("F5YC", -8.0);
    Control.addVariable("F5ZC", 14.0);
    Control.addVariable("F5ZG", 11.0);
    Control.addVariable("F5X", 999.0);
    Control.addVariable("F5Y", -18.0);
    Control.addVariable("F5Z", 13.0);

    Control.addVariable("F15Length", 440.0);
    Control.addVariable("F15Delta", 0.0);
    Control.addVariable("F15ViewWidth", 6);
    Control.addVariable("F15LinkPoint", 0.0);
    Control.addVariable("F15XB", 11.0);
    Control.addVariable("F15YB", -14.0);
    Control.addVariable("F15ZB", 14.0);
    Control.addVariable("F15XC", 11.0);
    Control.addVariable("F15YC", -8.0);
    Control.addVariable("F15ZC", 14.0);
    Control.addVariable("F15ZG", 11.0);
    Control.addVariable("F15X", -999.0);
    Control.addVariable("F15Y", -18.0);
    Control.addVariable("F15Z", 13.0);

    Control.addVariable("F25Length", 440.0);
    Control.addVariable("F25Delta", 0.0);
    Control.addVariable("F25ViewWidth", 6);
    Control.addVariable("F25LinkPoint", 0.0);
    Control.addVariable("F25XB", 11.0);
    Control.addVariable("F25YB", -14.0);
    Control.addVariable("F25ZB", 14.0);
    Control.addVariable("F25XC", 11.0);
    Control.addVariable("F25YC", -8.0);
    Control.addVariable("F25ZC", 14.0);
    Control.addVariable("F25ZG", 11.0);
    Control.addVariable("F25X", -999.0);
    Control.addVariable("F25Y", -18.0);
    Control.addVariable("F25Z", 13.0);


    Control.addVariable("F35Length", 440.0);
    Control.addVariable("F35Delta", 0.0);
    Control.addVariable("F35ViewWidth", 6);
    Control.addVariable("F35LinkPoint", 0.0);
    Control.addVariable("F35XB", 11.0);
    Control.addVariable("F35YB", -14.0);
    Control.addVariable("F35ZB", 14.0);
    Control.addVariable("F35XC", 11.0);
    Control.addVariable("F35YC", -8.0);
    Control.addVariable("F35ZC", 14.0);
    Control.addVariable("F35ZG", 11.0);
    Control.addVariable("F35X", -999.0);
    Control.addVariable("F35Y", -18.0);
    Control.addVariable("F35Z", 13.0);

    Control.addVariable("F45Length", 440.0);
    Control.addVariable("F45Delta", 0.0);
    Control.addVariable("F45ViewWidth", 6);
    Control.addVariable("F45LinkPoint", 0.0);
    Control.addVariable("F45XB", 11.0);
    Control.addVariable("F45YB", -14.0);
    Control.addVariable("F45ZB", 14.0);
    Control.addVariable("F45XC", 11.0);
    Control.addVariable("F45YC", -8.0);
    Control.addVariable("F45ZC", 14.0);
    Control.addVariable("F45ZG", 11.0);
    Control.addVariable("F45X", -999.0);
    Control.addVariable("F45Y", -18.0);
    Control.addVariable("F45Z", 13.0);

    Control.addVariable("F55Length", 440.0);
    Control.addVariable("F55Delta", 0.0);
    Control.addVariable("F55ViewWidth", 6);
    Control.addVariable("F55LinkPoint", 0.0);
    Control.addVariable("F55XB", 11.0);
    Control.addVariable("F55YB", -14.0);
    Control.addVariable("F55ZB", 14.0);
    Control.addVariable("F55XC", 11.0);
    Control.addVariable("F55YC", -8.0);
    Control.addVariable("F55ZC", 14.0);
    Control.addVariable("F55ZG", 11.0);
    Control.addVariable("F55X", -999.0);
    Control.addVariable("F55Y", -18.0);
    Control.addVariable("F55Z", 13.0);

    Control.addVariable("F65Length", 440.0);
    Control.addVariable("F65Delta", 0.0);
    Control.addVariable("F65ViewWidth", 6);
    Control.addVariable("F65LinkPoint", 0.0);
    Control.addVariable("F65XB", 11.0);
    Control.addVariable("F65YB", -14.0);
    Control.addVariable("F65ZB", 14.0);
    Control.addVariable("F65XC", 11.0);
    Control.addVariable("F65YC", -8.0);
    Control.addVariable("F65ZC", 14.0);
    Control.addVariable("F65ZG", 11.0);
    Control.addVariable("F65X", -999.0);
    Control.addVariable("F65Y", -18.0);
    Control.addVariable("F65Z", 13.0);

    Control.addVariable("F75Length", 440.0);
    Control.addVariable("F75Delta", 0.0);
    Control.addVariable("F75ViewWidth", 6);
    Control.addVariable("F75LinkPoint", 0.0);
    Control.addVariable("F75XB", 11.0);
    Control.addVariable("F75YB", -14.0);
    Control.addVariable("F75ZB", 14.0);
    Control.addVariable("F75XC", 11.0);
    Control.addVariable("F75YC", -8.0);
    Control.addVariable("F75ZC", 14.0);
    Control.addVariable("F75ZG", 11.0);
    Control.addVariable("F75X", -999.0);
    Control.addVariable("F75Y", -18.0);
    Control.addVariable("F75Z", 13.0);

    Control.addVariable("F85Length", 440.0);
    Control.addVariable("F85Delta", 0.0);
    Control.addVariable("F85ViewWidth", 6);
    Control.addVariable("F85LinkPoint", 0.0);
    Control.addVariable("F85XB", 11.0);
    Control.addVariable("F85YB", -14.0);
    Control.addVariable("F85ZB", 14.0);
    Control.addVariable("F85XC", 11.0);
    Control.addVariable("F85YC", -8.0);
    Control.addVariable("F85ZC", 14.0);
    Control.addVariable("F85ZG", 11.0);
    Control.addVariable("F85X", -999.0);
    Control.addVariable("F85Y", -18.0);
    Control.addVariable("F85Z", 13.0);

    Control.addVariable("F95Length", 440.0);
    Control.addVariable("F95Delta", 0.0);
    Control.addVariable("F95ViewWidth", 6);
    Control.addVariable("F95LinkPoint", 0.0);
    Control.addVariable("F95XB", 11.0);
    Control.addVariable("F95YB", -14.0);
    Control.addVariable("F95ZB", 14.0);
    Control.addVariable("F95XC", 11.0);
    Control.addVariable("F95YC", -8.0);
    Control.addVariable("F95ZC", 14.0);
    Control.addVariable("F95ZG", 11.0);
    Control.addVariable("F95X", -999.0);
    Control.addVariable("F95Y", -18.0);
    Control.addVariable("F95Z", 13.0);

    Control.addVariable("F105Length", 440.0);
    Control.addVariable("F105Delta", 0.0);
    Control.addVariable("F105ViewWidth", 6);
    Control.addVariable("F105LinkPoint", 0.0);
    Control.addVariable("F105XB", 11.0);
    Control.addVariable("F105YB", -14.0);
    Control.addVariable("F105ZB", 14.0);
    Control.addVariable("F105XC", 11.0);
    Control.addVariable("F105YC", -8.0);
    Control.addVariable("F105ZC", 14.0);
    Control.addVariable("F105ZG", 11.0);
    Control.addVariable("F105X", 999.0);
    Control.addVariable("F105Y", -18.0);
    Control.addVariable("F105Z", 13.0);

    Control.addVariable("F115Length", 440.0);
    Control.addVariable("F115Delta", 0.0);
    Control.addVariable("F115ViewWidth", 6);
    Control.addVariable("F115LinkPoint", 0.0);
    Control.addVariable("F115XB", 11.0);
    Control.addVariable("F115YB", -14.0);
    Control.addVariable("F115ZB", 14.0);
    Control.addVariable("F115XC", 11.0);
    Control.addVariable("F115YC", -8.0);
    Control.addVariable("F115ZC", 14.0);
    Control.addVariable("F115ZG", 11.0);
    Control.addVariable("F115X", -999.0);
    Control.addVariable("F115Y", -18.0);
    Control.addVariable("F115Z", 13.0);

    Control.addVariable("F125Length", 440.0);
    Control.addVariable("F125Delta", 0.0);
    Control.addVariable("F125ViewWidth", 6);
    Control.addVariable("F125LinkPoint", 0.0);
    Control.addVariable("F125XB", 11.0);
    Control.addVariable("F125YB", -14.0);
    Control.addVariable("F125ZB", 14.0);
    Control.addVariable("F125XC", 11.0);
    Control.addVariable("F125YC", -8.0);
    Control.addVariable("F125ZC", 14.0);
    Control.addVariable("F125ZG", 11.0);
    Control.addVariable("F125X", -999.0);
    Control.addVariable("F125Y", -18.0);
    Control.addVariable("F125Z", 13.0);


    Control.addVariable("F135Length", 440.0);
    Control.addVariable("F135Delta", 0.0);
    Control.addVariable("F135ViewWidth", 6);
    Control.addVariable("F135LinkPoint", 0.0);
    Control.addVariable("F135XB", 11.0);
    Control.addVariable("F135YB", -14.0);
    Control.addVariable("F135ZB", 14.0);
    Control.addVariable("F135XC", 11.0);
    Control.addVariable("F135YC", -8.0);
    Control.addVariable("F135ZC", 14.0);
    Control.addVariable("F135ZG", 11.0);
    Control.addVariable("F135X", -999.0);
    Control.addVariable("F135Y", -18.0);
    Control.addVariable("F135Z", 13.0);

    Control.addVariable("F145Length", 440.0);
    Control.addVariable("F145Delta", 0.0);
    Control.addVariable("F145ViewWidth", 6);
    Control.addVariable("F145LinkPoint", 0.0);
    Control.addVariable("F145XB", 11.0);
    Control.addVariable("F145YB", -14.0);
    Control.addVariable("F145ZB", 14.0);
    Control.addVariable("F145XC", 11.0);
    Control.addVariable("F145YC", -8.0);
    Control.addVariable("F145ZC", 14.0);
    Control.addVariable("F145ZG", 11.0);
    Control.addVariable("F145X", -999.0);
    Control.addVariable("F145Y", -18.0);
    Control.addVariable("F145Z", 13.0);

    Control.addVariable("F155Length", 440.0);
    Control.addVariable("F155Delta", 0.0);
    Control.addVariable("F155ViewWidth", 6);
    Control.addVariable("F155LinkPoint", 0.0);
    Control.addVariable("F155XB", 11.0);
    Control.addVariable("F155YB", -14.0);
    Control.addVariable("F155ZB", 14.0);
    Control.addVariable("F155XC", 11.0);
    Control.addVariable("F155YC", -8.0);
    Control.addVariable("F155ZC", 14.0);
    Control.addVariable("F155ZG", 11.0);
    Control.addVariable("F155X", -999.0);
    Control.addVariable("F155Y", -18.0);
    Control.addVariable("F155Z", 13.0);

    Control.addVariable("F165Length", 440.0);
    Control.addVariable("F165Delta", 0.0);
    Control.addVariable("F165ViewWidth", 6);
    Control.addVariable("F165LinkPoint", 0.0);
    Control.addVariable("F165XB", 11.0);
    Control.addVariable("F165YB", -14.0);
    Control.addVariable("F165ZB", 14.0);
    Control.addVariable("F165XC", 11.0);
    Control.addVariable("F165YC", -8.0);
    Control.addVariable("F165ZC", 14.0);
    Control.addVariable("F165ZG", 11.0);
    Control.addVariable("F165X", -999.0);
    Control.addVariable("F165Y", -18.0);
    Control.addVariable("F165Z", 13.0);

    Control.addVariable("F175Length", 440.0);
    Control.addVariable("F175Delta", 0.0);
    Control.addVariable("F175ViewWidth", 6);
    Control.addVariable("F175LinkPoint", 0.0);
    Control.addVariable("F175XB", 11.0);
    Control.addVariable("F175YB", -14.0);
    Control.addVariable("F175ZB", 14.0);
    Control.addVariable("F175XC", 11.0);
    Control.addVariable("F175YC", -8.0);
    Control.addVariable("F175ZC", 14.0);
    Control.addVariable("F175ZG", 11.0);
    Control.addVariable("F175X", -999.0);
    Control.addVariable("F175Y", -18.0);
    Control.addVariable("F175Z", 13.0);

    Control.addVariable("F185Length", 440.0);
    Control.addVariable("F185Delta", 0.0);
    Control.addVariable("F185ViewWidth", 6);
    Control.addVariable("F185LinkPoint", 0.0);
    Control.addVariable("F185XB", 11.0);
    Control.addVariable("F185YB", -14.0);
    Control.addVariable("F185ZB", 14.0);
    Control.addVariable("F185XC", 11.0);
    Control.addVariable("F185YC", -8.0);
    Control.addVariable("F185ZC", 14.0);
    Control.addVariable("F185ZG", 11.0);
    Control.addVariable("F185X", -999.0);
    Control.addVariable("F185Y", -18.0);
    Control.addVariable("F185Z", 13.0);

    Control.addVariable("F195Length", 440.0);
    Control.addVariable("F195Delta", 0.0);
    Control.addVariable("F195ViewWidth", 6);
    Control.addVariable("F195LinkPoint", 0.0);
    Control.addVariable("F195XB", 11.0);
    Control.addVariable("F195YB", -14.0);
    Control.addVariable("F195ZB", 14.0);
    Control.addVariable("F195XC", 11.0);
    Control.addVariable("F195YC", -8.0);
    Control.addVariable("F195ZC", 14.0);
    Control.addVariable("F195ZG", 11.0);
    Control.addVariable("F195X", -999.0);
    Control.addVariable("F195Y", -18.0);
    Control.addVariable("F195Z", 13.0);
}
  
void
EssBeamLinesVariables(FuncDataBase& Control)
  /*!
    Create all the beamline variables
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("essVariables[F]","EssBeamLinesVariables");
  for(size_t i=0;i<4;i++)
    {
      const std::string baseKey=
	StrFunc::makeString("G",i+1)+"BLine";
      // BeamLine in guide bay
      Control.addVariable(baseKey+"XStep",0.0);  
      Control.addVariable(baseKey+"YStep",0.0);  
      Control.addVariable(baseKey+"ZStep",0.0);
      Control.addVariable(baseKey+"Zangle",0.0);
      Control.addVariable(baseKey+"Mat","CastIron");
      Control.addVariable(baseKey+"BeamXYAngle",0.0); 
      Control.addVariable(baseKey+"BeamZAngle",0.0);
      Control.addVariable(baseKey+"BeamXStep",0.0);
      Control.addVariable(baseKey+"BeamZStep",0.0);  
      Control.addVariable(baseKey+"BeamHeight",3.0);
      Control.addVariable(baseKey+"BeamWidth",7.6);
      Control.addVariable(baseKey+"NSegment",3);
      Control.addVariable(baseKey+"Width1",22.0);
      Control.addVariable(baseKey+"Height1",22.0);
      Control.addVariable(baseKey+"Width2",28.0);
      Control.addVariable(baseKey+"Height2",44.0);
      Control.addVariable(baseKey+"Width3",40.0);
      Control.addVariable(baseKey+"Height3",60.0);
      Control.addVariable(baseKey+"Length1",170.0);
      Control.addVariable(baseKey+"Length2",170.0);
      Control.addVariable(baseKey+"1XYangle",30.0); 
      Control.addVariable(baseKey+"2XYangle",24.0); 
      Control.addVariable(baseKey+"3XYangle",18.0); 
      Control.addVariable(baseKey+"4XYangle",12.0); 
      Control.addVariable(baseKey+"5XYangle",6.0); 
      Control.addVariable(baseKey+"6XYangle",0); 
      Control.addVariable(baseKey+"7XYangle",-6.0); 
      Control.addVariable(baseKey+"8XYangle",-12.0);
      Control.addVariable(baseKey+"9XYangle",-18.0); 
      Control.addVariable(baseKey+"10XYangle",-24.0); 
      Control.addVariable(baseKey+"11XYangle",-30.0);
      Control.addVariable(baseKey+"Filled",0);
      Control.addVariable(baseKey+"Active",0);      
    }
  return;
}

}  // NAMESPACE setVariable
