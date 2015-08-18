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

void
EssWheel(FuncDataBase& Control)
  /*!
    Variables that are used for the segmented wheel
    \param Control :: Segment variables
   */
{
  // WHEEL SHAFT

  Control.addVariable("SegWheelShaftNLayers",6);
  Control.addVariable("SegWheelShaftTopHeight",400.0);
  Control.addVariable("SegWheelShaftBaseHeight",-13.5);
  // Control.addVariable("SegWheelShaftHeight",435.0);

  // Control.addVariable("SegWheelShaftRadius",25.0);
  // see WheelInnerRadius below (28cm): it's the external joint radius
  Control.addVariable("SegWheelShaftJointThick",3.0);
  Control.addVariable("SegWheelShaftCoolThick",4.0);
  Control.addVariable("SegWheelShaftCladThick",3.0);
  Control.addVariable("SegWheelShaftVoidThick",1.0);
  Control.addVariable("SegWheelShaftSupportRadius",40.0);
  Control.addVariable("SegWheelShaftSupportThick",2.5);
  Control.addVariable("SegWheelShaftBaseThick",3.0);
  Control.addVariable("SegWheelShaftBaseFootThick",13.5);

  Control.addVariable("SegWheelCladShaftMat","Stainless304");
  Control.addVariable("SegWheelCoolingShaftMatInt","Stainless304");
  Control.addVariable("SegWheelCoolingShaftMatExt","Void");
 
  // TARGET

  Control.addVariable("SegWheelXStep",0.0);  
  Control.addVariable("SegWheelYStep",115.0);  
  Control.addVariable("SegWheelZStep",0.0);
  Control.addVariable("SegWheelXYangle",0.0); 
  Control.addVariable("SegWheelZangle",0.0);
  //
  Control.addVariable("SegWheelTargetHeight",8.0);
  Control.addVariable("SegWheelTargetSectorOffsetX",0.0);  
  Control.addVariable("SegWheelTargetSectorOffsetY",142.6476749356);
  Control.addVariable("SegWheelTargetSectorOffsetZ",0.0);
  Control.addVariable("SegWheelTargetSectorAngleXY",1.1234180949);
  Control.addVariable("SegWheelTargetSectorAngleZ",0.0);
  Control.addVariable("SegWheelTargetSectorApertureXY",2.2468361899);
  Control.addVariable("SegWheelTargetSectorApertureZ",0.0);
  Control.addVariable("SegWheelTargetSectorNumber",33);
  //
  Control.addVariable("SegWheelCoolantThickOut",0.15);
  Control.addVariable("SegWheelCoolantThickIn",0.65);
  //
  Control.addVariable("SegWheelCaseThickZ",0.5);
  Control.addVariable("SegWheelCaseThickX",0.1);
  //
  Control.addVariable("SegWheelVoidThick",2.15);  //distance target-ion tube height

  Control.addVariable("SegWheelInnerRadius",28.0);
  Control.addVariable("SegWheelCoolantRadiusOut",124.8);
  Control.addVariable("SegWheelCoolantRadiusIn",114.5);
  Control.addVariable("SegWheelCaseRadius",125.0);
  Control.addVariable("SegWheelVoidRadius",126.0);

  // Material types 1:2:3
  Control.addVariable("SegWheelWMat","Tungsten");
  Control.addVariable("SegWheelSteelMat","Stainless304");
  Control.addVariable("SegWheelHeMat","helium");

  Control.addVariable("SegWheelInnerMat","Stainless304");

  Control.addVariable("SegWheelNLayers",25);

  Control.addVariable("SegWheelRadius1",84.27);
  Control.addVariable("SegWheelMatTYPE1",1);

  Control.addVariable("SegWheelRadius2",85.65);
  Control.addVariable("SegWheelMatTYPE2",2);

  Control.addVariable("SegWheelRadius3",97.02);
  Control.addVariable("SegWheelMatTYPE3",3);

  Control.addVariable("SegWheelRadius4",97.52);
  Control.addVariable("SegWheelMatTYPE4",2);

  Control.addVariable("SegWheelRadius5",102.37); 
  Control.addVariable("SegWheelMatTYPE5",3);

  Control.addVariable("SegWheelRadius6",102.77);
  Control.addVariable("SegWheelMatTYPE6",2);

  Control.addVariable("SegWheelRadius7",106.97);
  Control.addVariable("SegWheelMatTYPE7",3);

  Control.addVariable("SegWheelRadius8",107.29);
  Control.addVariable("SegWheelMatTYPE8",2);

  Control.addVariable("SegWheelRadius9",110.49);
  Control.addVariable("SegWheelMatTYPE9",3);

  Control.addVariable("SegWheelRadius10",110.78);
  Control.addVariable("SegWheelMatTYPE10",2); 

  Control.addVariable("SegWheelRadius11",112.78);
  Control.addVariable("SegWheelMatTYPE11",3); 

  Control.addVariable("SegWheelRadius12",113.05);
  Control.addVariable("SegWheelMatTYPE12",2); 

  Control.addVariable("SegWheelRadius13",114.65);
  Control.addVariable("SegWheelMatTYPE13",3); 

  Control.addVariable("SegWheelRadius14",114.9);
  Control.addVariable("SegWheelMatTYPE14",2); 

  // Control.addVariable("SegWheelRadius15",114.9);
  // Control.addVariable("SegWheelMatTYPE15",1); 

  Control.addVariable("SegWheelRadius15",116.5);
  Control.addVariable("SegWheelMatTYPE15",3); 

  Control.addVariable("SegWheelRadius16",116.75);
  Control.addVariable("SegWheelMatTYPE16",2); 

  Control.addVariable("SegWheelRadius17",118.35);
  Control.addVariable("SegWheelMatTYPE17",3); 

  Control.addVariable("SegWheelRadius18",118.6);
  Control.addVariable("SegWheelMatTYPE18",2); 

  Control.addVariable("SegWheelRadius19",120.0);
  Control.addVariable("SegWheelMatTYPE19",3); 

  Control.addVariable("SegWheelRadius20",120.25);
  Control.addVariable("SegWheelMatTYPE20",2); 

  Control.addVariable("SegWheelRadius21",121.65);
  Control.addVariable("SegWheelMatTYPE21",3); 

  Control.addVariable("SegWheelRadius22",121.9);
  Control.addVariable("SegWheelMatTYPE22",2); 

  Control.addVariable("SegWheelRadius23",123.1);
  Control.addVariable("SegWheelMatTYPE23",3); 

  Control.addVariable("SegWheelRadius24",123.35);
  Control.addVariable("SegWheelMatTYPE24",2); 

  Control.addVariable("SegWheelRadius25",124.55);
  Control.addVariable("SegWheelMatTYPE25",3); 
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
  Control.addVariable("LowAFlightAngleZTop",0.0);  // Step down angle
  Control.addVariable("LowAFlightAngleZBase",0.0); // Step up angle
  Control.addVariable("LowAFlightHeight",7.6);     // Full height = LowFlyTotalHeight
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
  Control.addVariable("LowBFlightAngleZTop",0.0);  // Step down angle
  Control.addVariable("LowBFlightAngleZBase",0.0); // Step up angle
  Control.addVariable("LowBFlightHeight",7.6);     // Full height = LowFlyTotalHeight
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
  Control.addVariable("TopAFlightXYangle", 0.0);
  Control.addVariable("TopAFlightZangle", 0.0);
  Control.addVariable("TopAFlightAngleZTop", 0.0);  // Step down angle
  Control.addVariable("TopAFlightAngleZBase", 0.0); // Step up angle
  //  Control.Parse("TopFlyTotalHeight");
  Control.addVariable("TopAFlightHeight", 4.6);     // Full height = TopFlyTotalHeight
  Control.addVariable("TopAFlightWidth", 130.7);     // Full width
  Control.addVariable("TopAFlightNLiner", 1);      // Liner
  Control.addVariable("TopAFlightLinerThick1", 0.4);      // Liner
  Control.addVariable("TopAFlightLinerMat1","Aluminium");      // Liner

  Control.addVariable("TopBFlightXStep", 0.0);      // Step from centre
  Control.addVariable("TopBFlightZStep", 0.0);      // Step from centre
  Control.addVariable("TopBFlightAngleXY1", 60.0);  // Angle out
  Control.addVariable("TopBFlightAngleXY2", 60.0);  // Angle out
  Control.addVariable("TopBFlightXYangle", 0.0);
  Control.addVariable("TopBFlightZangle", 0.0);
  Control.addVariable("TopBFlightAngleZTop", 0.0);  // Step down angle
  Control.addVariable("TopBFlightAngleZBase", 0.0); // Step up angle
  Control.addVariable("TopBFlightHeight", 4.6);     // Full height = TopFlyTotalHeight
  Control.addVariable("TopBFlightWidth", 10.7);     // Full width
  Control.addVariable("TopBFlightNLiner", 1);     
  Control.addVariable("TopBFlightLinerThick1", 0.4); 
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

  Control.addVariable("WheelCladShaftMat","Stainless304");
  Control.addVariable("WheelMainShaftMat","Stainless304");

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
  Control.addVariable("WheelSteelMat","Stainless304");
  Control.addVariable("WheelHeMat","helium");
  Control.addVariable("WheelInnerMat","Stainless304");

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
  Control.addVariable("BeRefRefMat","Be300K");

  Control.addVariable("BeRefWallMat","Aluminium");
  Control.addVariable("BeRefTargSepMat","Void");

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
  Control.Parse("BeRefHeight/2.0+BeRefWallThick+TopBeRefWaterDiscHeight0+TopBeRefWaterDiscDepth0+TopBeRefWaterDiscHeight1+0.2");
  Control.addVariable("BulkHeight1");
  Control.Parse("BeRefHeight/2.0+BeRefWallThick+LowBeRefWaterDiscHeight0+LowBeRefWaterDiscDepth0+LowBeRefWaterDiscHeight1+0.2");
  Control.addVariable("BulkDepth1");
  Control.addVariable("BulkMat1","Void");

  Control.addVariable("BulkRadius2",65.0);
  Control.addVariable("BulkHeight2",75.0);
  Control.addVariable("BulkDepth2",75.0);
  Control.addVariable("BulkMat2","Stainless304");           // stainless

  // Bulk steel layer [No individual guides]
  Control.addVariable("BulkRadius3",200.0);
  Control.addVariable("BulkHeight3",200.0);
  Control.addVariable("BulkDepth3",200.0);
  Control.addVariable("BulkMat3","CastIron");           // Bulk Steel


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
  Control.addVariable("GuideBayViewAngle",68.0); 
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
  Control.addVariable("GuideBay1NItems",12);  
  Control.addVariable("GuideBay2NItems",12);  
  Control.addVariable("GuideBay3NItems",12);  
  Control.addVariable("GuideBay4NItems",12);  

  EssBeamLinesVariables(Control);
  EssReflectorVariables(Control);
  EssSANSVariables(Control);
  ODINvariables(Control);
  LOKIvariables(Control);
  NMXvariables(Control);
  EssButterflyModerator(Control);
  EssWheel(Control);
  EssBunkerVariables(Control);


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

  Control.addVariable("LowFlyLeftLobeHeight",6.0);
  Control.addVariable("LowFlyLeftLobeModMat","ParaH2");
  Control.addVariable("LowFlyLeftLobeModTemp",20.0);

  Control.addVariable("LowFlyLeftLobeNLayers",4);
  Control.addVariable("LowFlyLeftLobeThick1",0.3);
  Control.addVariable("LowFlyLeftLobeMat1","Aluminium");
  Control.addVariable("LowFlyLeftLobeVThick1",0.3);
  Control.addVariable("LowFlyLeftLobeModTemp1",20.0);
  
  Control.addVariable("LowFlyLeftLobeThick2",0.5);
  Control.addVariable("LowFlyLeftLobeMat2","Void");

  Control.addVariable("LowFlyLeftLobeHeight2",0.5);
  Control.addVariable("LowFlyLeftLobeDepth2",0.5);

  Control.addVariable("LowFlyLeftLobeThick3",0.3);
  Control.addVariable("LowFlyLeftLobeMat3","Aluminium");
  Control.addVariable("LowFlyLeftLobeVThick3",0.3);

  Control.addVariable("LowFlyLeftLobeThick4",0.5);
  Control.addVariable("LowFlyLeftLobeMat4","Void");
  Control.addVariable("LowFlyLeftLobeVThick4",0.3);

  Control.addVariable("LowFlyFlowGuideBaseThick",0.3);
  Control.addVariable("LowFlyFlowGuideBaseLen",5.0);
  Control.addVariable("LowFlyFlowGuideArmThick",0.3);
  Control.addVariable("LowFlyFlowGuideArmLen",8.0);
  Control.addVariable("LowFlyFlowGuideBaseArmSep",0.1);
  Control.addVariable("LowFlyFlowGuideBaseOffset",Geometry::Vec3D(0,-10.7,0));
  Control.addVariable("LowFlyFlowGuideArmOffset",Geometry::Vec3D(0,-9,0));
  Control.addVariable("LowFlyFlowGuideWallMat","Aluminiumn");
  Control.addVariable("LowFlyFlowGuideWallTemp",20.0);
  
  Control.addVariable("LowFlyRightLobeXStep",-1.0);  
  Control.addVariable("LowFlyRightLobeYStep",0.0);  

  Control.addVariable("LowFlyRightLobeCorner1",Geometry::Vec3D(0,0.45,0));
  Control.addVariable("LowFlyRightLobeCorner2",Geometry::Vec3D(-14.4,-13.2,0));
  Control.addVariable("LowFlyRightLobeCorner3",Geometry::Vec3D(14.4,-13.2,0));

  Control.addVariable("LowFlyRightLobeRadius1",5.0);
  Control.addVariable("LowFlyRightLobeRadius2",2.506);
  Control.addVariable("LowFlyRightLobeRadius3",2.506);

  Control.addVariable("LowFlyRightLobeHeight",6.0);
  Control.addVariable("LowFlyRightLobeModMat","ParaH2");
  Control.addVariable("LowFlyRightLobeModTemp",20.0);

  Control.addVariable("LowFlyRightLobeNLayers",4);
  Control.addVariable("LowFlyRightLobeThick1",0.3);
  Control.addVariable("LowFlyRightLobeMat1","Aluminium");
  Control.addVariable("LowFlyRightLobeVThick1",0.3);
  Control.addVariable("LowFlyRightLobeModTemp1",20.0);
  
  Control.addVariable("LowFlyRightLobeThick2",0.5);
  Control.addVariable("LowFlyRightLobeMat2","Void");
  Control.addVariable("LowFlyRightLobeVThick2",0.8);

  Control.addVariable("LowFlyRightLobeThick3",0.3);
  Control.addVariable("LowFlyRightLobeMat3","Aluminium");
  Control.addVariable("LowFlyRightLobeVThick3",0.3);

  Control.addVariable("LowFlyRightLobeThick4",0.5);
  Control.addVariable("LowFlyRightLobeMat4","Void");
  Control.addVariable("LowFlyRightLobeVThick4",0.3);

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

  Control.addVariable("LowPreModNLayers",2);
  Control.addVariable("LowPreModHeight0",2.0);
  Control.addVariable("LowPreModDepth0",2.0);
  Control.addVariable("LowPreModRadius0",30.0);
  Control.addVariable("LowPreModMat0","H2O");
  Control.addVariable("LowPreModHeight1",0.3);
  Control.addVariable("LowPreModDepth1",0.3);
  Control.addVariable("LowPreModRadius1",0.3);
  Control.addVariable("LowPreModMat1","Aluminium");

  Control.addVariable("LowCapModNLayers",2);
  Control.addVariable("LowCapModHeight0",2.0);
  Control.addVariable("LowCapModDepth0",2.0);
  Control.addVariable("LowCapModRadius0",30.0);
  Control.addVariable("LowCapModMat0","H2O");
  Control.addVariable("LowCapModHeight1",0.3);
  Control.addVariable("LowCapModDepth1",0.3);
  Control.addVariable("LowCapModRadius1",0.3);
  Control.addVariable("LowCapModMat1","Aluminium");

  // TOP MODERATOR

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

  Control.addVariable("TopFlyLeftLobeModMat","ParaOrtho%5.0");  // LH05ortho
  Control.addVariable("TopFlyLeftLobeModTemp",20.0);

  Control.addVariable("TopFlyLeftLobeNLayers",4);
  Control.addVariable("TopFlyLeftLobeThick1",0.3);
  Control.addVariable("TopFlyLeftLobeMat1","Aluminium");

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

  Control.addVariable("TopFlyRightLobeModMat","ParaOrtho%5.0");
  Control.addVariable("TopFlyRightLobeModTemp",20.0);

  Control.addVariable("TopFlyRightLobeNLayers",4);
  Control.addVariable("TopFlyRightLobeThick1",0.3);
  Control.addVariable("TopFlyRightLobeMat1","Aluminium");  // 20K

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
  Control.addVariable("TopPreModMat2","Stainless304");
  Control.addVariable("TopPreModHeight3",0.3);
  Control.addVariable("TopPreModDepth3",0.3);
  Control.Parse("BeRefWallThick");
  Control.addVariable("TopPreModRadius3");
  Control.addVariable("TopPreModMat3","Aluminium");
  Control.addVariable("TopPreModHeight4", 0.0);
  Control.addVariable("TopPreModDepth4",  0.85);
  Control.addVariable("TopPreModRadius4", 0.0);
  Control.addVariable("TopPreModMat4","Void");

  Control.addVariable("TopCapModNLayers",4);
  Control.addVariable("TopCapModHeight0",0.5);
  Control.addVariable("TopCapModDepth0", 0.5);
  Control.addVariable("TopCapModRadius0",19.5);
  Control.addVariable("TopCapModMat0","H2O");
  Control.addVariable("TopCapModHeight1",0.6);
  Control.addVariable("TopCapModDepth1",0.0);
  Control.addVariable("TopCapModRadius1",0.6);
  Control.addVariable("TopCapModMat1","Aluminium");
  Control.addVariable("TopCapModHeight2",0.0);
  Control.addVariable("TopCapModDepth2",0.0);
  Control.Parse("BeRefRadius-TopCapModRadius0-TopCapModRadius1");
  Control.addVariable("TopCapModRadius2");
  Control.addVariable("TopCapModMat2","Be300K");
  Control.addVariable("TopCapModHeight3",0);
  Control.addVariable("TopCapModDepth3", 0.3);
  Control.Parse("BeRefWallThick");
  Control.addVariable("TopCapModRadius3");
  //  Control.Parse("BeRefWallMat");
  Control.addVariable("TopCapModMat3", "Aluminium");

  
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

  Control.addVariable("LowABunkerLeftPhase",35.0);
  Control.addVariable("LowABunkerRightPhase",35.0);
  Control.addVariable("LowABunkerLeftAngle",0.0);
  Control.addVariable("LowABunkerRightAngle",0.0);
  Control.addVariable("LowABunkerNSectors",11);
  
  Control.addVariable("LowABunkerWallRadius",1300.0);
  Control.addVariable("LowABunkerFloorDepth",100.0);
  Control.addVariable("LowABunkerRoofHeight",100.0);

  Control.addVariable("LowABunkerWallThick",250.0);
  Control.addVariable("LowABunkerSideThick",100.0);
  Control.addVariable("LowABunkerRoofThick",100.0);
  Control.addVariable("LowABunkerFloorThick",100.0);

  Control.addVariable("LowABunkerWallMat","Steel71");

  Control.addVariable("LowABunkerNLayers",12);
  Control.addVariable("LowABunkerWallMat1","Tungsten");
  Control.addVariable("LowABunkerWallMat2","Tungsten");
  Control.addVariable("LowABunkerWallMat3","Stainless304");
  Control.addVariable("LowABunkerWallMat4","Steel71");
  Control.addVariable("LowABunkerWallMat9","Tungsten");
      
  
  return;
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
    }
  return;
}

}  // NAMESPACE setVariable
