/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   essBuild/essVariables.cxx
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
  void EssConicModerator(FuncDataBase&);
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
  Control.addVariable("LowModHeight",8.0);
  Control.addVariable("LowModMat","ParaH2");
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

  Control.addVariable("LowAFlightXStep",-3.2+1.5);      // Step from centre
  Control.addVariable("LowAFlightZStep",0.0);      // Step from centre
  Control.addVariable("LowAFlightAngleXY1",30.0);  // Angle out
  Control.addVariable("LowAFlightAngleXY2",30.0);  // Angle out
  Control.addVariable("LowAFlightAngleZTop",0.0);  // Step down angle
  Control.addVariable("LowAFlightAngleZBase",0.0); // Step up angle
  Control.addVariable("LowAFlightHeight",10.0);     // Full height
  Control.addVariable("LowAFlightWidth",22.0);     // Full width
  Control.addVariable("LowAFlightNLiner",4);      // Liner
  Control.addVariable("LowAFlightLinerThick1",0.2);      // Liner
  Control.addVariable("LowAFlightLinerMat1","Aluminium");      // Liner
  Control.addVariable("LowAFlightLinerThick2",1.5);      // Liner
  Control.addVariable("LowAFlightLinerMat2","H2O");      // Liner
  Control.addVariable("LowAFlightLinerThick3",0.2);      // Liner
  Control.addVariable("LowAFlightLinerMat3","Aluminium");      // Liner
  Control.addVariable("LowAFlightLinerThick4",0.3);      // Liner
  Control.addVariable("LowAFlightLinerMat4","Void");      // Liner

  Control.addVariable("LowAFlightLinerCap1",1);      // Liner
  Control.addVariable("LowAFlightLinerCap2",2);      // Liner
  Control.addVariable("LowAFlightLinerCap3",1);      // Liner

  

  Control.addVariable("LowBFlightXStep",3.5);     // Angle
  Control.addVariable("LowBFlightZStep",0.0);      // Step from centre
  Control.addVariable("LowBFlightMasterXY",0.0);  // Angle out
  Control.addVariable("LowBFlightAngleXY1",30.0);  // Angle out
  Control.addVariable("LowBFlightAngleXY2",30.0);  // Angle out
  Control.addVariable("LowBFlightAngleZTop",0.0);  // Step down angle
  Control.addVariable("LowBFlightAngleZBase",0.0); // Step up angle
  Control.addVariable("LowBFlightHeight",10.0);     // Full height
  Control.addVariable("LowBFlightWidth",20.0);     // Full width
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

  Control.addVariable("LowPreABlockSide",0);  
  Control.addVariable("LowPreABlockActive",1);  
  Control.addVariable("LowPreABlockWidth",3.8);  
  Control.addVariable("LowPreABlockHeight",10.0);  
  Control.addVariable("LowPreABlockLength",11.80);  
  Control.addVariable("LowPreABlockWallThick",0.2);  
  Control.addVariable("LowPreABlockGap",0.4);  
  Control.addVariable("LowPreABlockWallMat","Aluminium");  
  Control.addVariable("LowPreABlockWaterMat","H2O");  

  // Other block
  Control.addVariable("LowPreBBlockSide",0);  
  Control.addVariable("LowPreBBlockActive",1);  
  Control.addVariable("LowPreBBlockWidth",3.8);  
  Control.addVariable("LowPreBBlockHeight",10.0);  
  Control.addVariable("LowPreBBlockLength",10.8);  
  Control.addVariable("LowPreBBlockWallThick",0.2);  
  Control.addVariable("LowPreBBlockGap",0.4);  
  Control.addVariable("LowPreBBlockWallMat","Aluminium");  
  Control.addVariable("LowPreBBlockWaterMat","H2O");  

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

  Control.addVariable("TopPreABlockSide",1);  
  Control.addVariable("TopPreABlockActive",0);  
  Control.addVariable("TopPreABlockWidth",4.0);  
  Control.addVariable("TopPreABlockHeight",10.0);  
  Control.addVariable("TopPreABlockLength",10.80);  
  Control.addVariable("TopPreABlockWallThick",0.2);  
  Control.addVariable("TopPreABlockGap",0.4);  
  Control.addVariable("TopPreABlockWallMat","Aluminium");  
  Control.addVariable("TopPreABlockWaterMat","H2O");  

  // Other block
  Control.addVariable("TopPreBBlockActive",1);  
  Control.addVariable("TopPreBBlockSide",0);  
  Control.addVariable("TopPreBBlockWidth",4.0);  
  Control.addVariable("TopPreBBlockHeight",10.0);  
  Control.addVariable("TopPreBBlockLength",10.8);  
  Control.addVariable("TopPreBBlockWallThick",0.2);  
  Control.addVariable("TopPreBBlockGap",0.4);  
  Control.addVariable("TopPreBBlockWallMat","Aluminium");  
  Control.addVariable("TopPreBBlockWaterMat","H2O");  
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

  // TOP A FLIGHT
  Control.addVariable("TopAFlightXStep",-4.5);      // Step from centre
  Control.addVariable("TopAFlightZStep",0.0);      // Step from centre
  Control.addVariable("TopAFlightAngleXY1",30.0);  // Angle out
  Control.addVariable("TopAFlightAngleXY2",30.0);  // Angle out
  Control.addVariable("TopAFlightAngleZTop",0.0);  // Step down angle
  Control.addVariable("TopAFlightAngleZBase",0.0); // Step up angle
  Control.addVariable("TopAFlightHeight",10.0);     // Full height
  Control.addVariable("TopAFlightWidth",20.0);     // Full width
  Control.addVariable("TopAFlightNLiner",1);      // Liner
  Control.addVariable("TopAFlightLinerThick1",0.5);      // Liner
  Control.addVariable("TopAFlightLinerMat1","Aluminium");      // Liner

  // B FLIGHT
  Control.addVariable("TopBFlightXStep",4.5);     // Angle
  Control.addVariable("TopBFlightZStep",0.0);      // Step from centre
  Control.addVariable("TopBFlightAngleXY1",30.0);  // Angle out
  Control.addVariable("TopBFlightAngleXY2",30.0);  // Angle out
  Control.addVariable("TopBFlightAngleZTop",0.0);  // Step down angle
  Control.addVariable("TopBFlightAngleZBase",0.0); // Step up angle
  Control.addVariable("TopBFlightHeight",10.0);     // Full height
  Control.addVariable("TopBFlightWidth",20.0);     // Full width
  Control.addVariable("TopBFlightNLiner",1);      // Liner
  Control.addVariable("TopBFlightLinerThick1",0.5);   
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
  Control.addVariable("WheelYStep",115.0);  
  Control.addVariable("WheelZStep",0.0);
  Control.addVariable("WheelXYangle",0.0); 
  Control.addVariable("WheelZangle",0.0);
  Control.addVariable("WheelTargetHeight",8.0);
  Control.addVariable("WheelCoolantThick",0.5);
  Control.addVariable("WheelCaseThick",0.5);
  Control.addVariable("WheelVoidThick",1.0);

  Control.addVariable("WheelInnerRadius",82.0);
  Control.addVariable("WheelCoolantRadius",125.0);
  Control.addVariable("WheelCaseRadius",127.0);
  Control.addVariable("WheelVoidRadius",130.0);

  Control.addVariable("WheelWMat","Tungsten");
  Control.addVariable("WheelSteelMat","Stainless304");
  Control.addVariable("WheelHeMat","helium");
  Control.addVariable("WheelInnerMat","Stainless304");

  Control.addVariable("WheelNLayers",28);

  Control.addVariable("WheelRadius1",83.87);
  Control.addVariable("WheelMatTYPE1",1);

  Control.addVariable("WheelRadius2",85.25);
  Control.addVariable("WheelMatTYPE2",2);

  Control.addVariable("WheelRadius3",96.62);
  Control.addVariable("WheelMatTYPE3",3);

  Control.addVariable("WheelRadius4",97.12);
  Control.addVariable("WheelMatTYPE4",2);

  Control.addVariable("WheelRadius5",100.00);    // WRONG
  Control.addVariable("WheelMatTYPE5",3);

  Control.addVariable("WheelRadius6",101.97);
  Control.addVariable("WheelMatTYPE6",2);

  Control.addVariable("WheelRadius7",102.17);
  Control.addVariable("WheelMatTYPE7",1);

  Control.addVariable("WheelRadius8",102.57);
  Control.addVariable("WheelMatTYPE8",2);

  Control.addVariable("WheelRadius9",106.77);
  Control.addVariable("WheelMatTYPE9",3);

  Control.addVariable("WheelRadius10",107.09);
  Control.addVariable("WheelMatTYPE10",2);

  Control.addVariable("WheelRadius11",110.29);
  Control.addVariable("WheelMatTYPE11",3);

  Control.addVariable("WheelRadius12",110.58);
  Control.addVariable("WheelMatTYPE12",2); 

  Control.addVariable("WheelRadius13",112.58);
  Control.addVariable("WheelMatTYPE13",3); 

  Control.addVariable("WheelRadius14",113.3);
  Control.addVariable("WheelMatTYPE14",2); 

  Control.addVariable("WheelRadius15",114.45);
  Control.addVariable("WheelMatTYPE15",3); 

  Control.addVariable("WheelRadius16",114.7);
  Control.addVariable("WheelMatTYPE16",2); 

  Control.addVariable("WheelRadius17",114.9);
  Control.addVariable("WheelMatTYPE17",1); 

  Control.addVariable("WheelRadius18",116.5);
  Control.addVariable("WheelMatTYPE18",3); 

  Control.addVariable("WheelRadius19",116.75);
  Control.addVariable("WheelMatTYPE19",2); 

  Control.addVariable("WheelRadius20",118.35);
  Control.addVariable("WheelMatTYPE20",3); 

  Control.addVariable("WheelRadius21",118.6);
  Control.addVariable("WheelMatTYPE21",2); 

  Control.addVariable("WheelRadius22",120.0);
  Control.addVariable("WheelMatTYPE22",3); 

  Control.addVariable("WheelRadius23",120.25);
  Control.addVariable("WheelMatTYPE23",2); 

  Control.addVariable("WheelRadius24",121.65);
  Control.addVariable("WheelMatTYPE24",3); 

  Control.addVariable("WheelRadius25",121.9);
  Control.addVariable("WheelMatTYPE25",2); 

  Control.addVariable("WheelRadius26",123.1);
  Control.addVariable("WheelMatTYPE26",3); 

  Control.addVariable("WheelRadius27",123.35);
  Control.addVariable("WheelMatTYPE27",2); 

  Control.addVariable("WheelRadius28",124.55);
  Control.addVariable("WheelMatTYPE28",3); 

  Control.addVariable("BeRefXStep",0.0);  
  Control.addVariable("BeRefYStep",0.0);  
  Control.addVariable("BeRefZStep",0.0);
  Control.addVariable("BeRefXYangle",0.0); 
  Control.addVariable("BeRefZangle",0.0);
  Control.addVariable("BeRefRadius",29.9);
  Control.addVariable("BeRefHeight",89.8);
  Control.addVariable("BeRefWallThick",0.1);
  Control.addVariable("BeRefTargSepThick",12.30);
  Control.addVariable("BeRefRefMat","Be300K");
  Control.addVariable("BeRefWallMat","Aluminium");
  Control.addVariable("BeRefTargSepMat","Stainless304");

  

  Control.addVariable("BulkXStep",0.0);
  Control.addVariable("BulkYStep",0.0);
  Control.addVariable("BulkZStep",0.0);
  Control.addVariable("BulkXYangle",0.0);
  Control.addVariable("BulkZangle",0.0);
  Control.addVariable("BulkNLayer",3);

  Control.Parse("BeRefRadius+3.0");
  Control.addVariable("BulkRadius1");
  Control.addVariable("BulkHeight1",48.0);
  Control.addVariable("BulkDepth1",48.0);
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
  Control.addVariable("ShutterBayRadius",600.0);
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
  EssConicModerator(Control);
  EssWheel(Control);


  Control.addVariable("sdefEnergy",2500.0);  

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
EssConicModerator(FuncDataBase& Control)
  /*!
    Create all the Conic moderator option variables
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("essVariables[F]","EssConicModerator");
  
  //  Control.addVariable("LowAFlightXStep",0.0);      // Step from centre


  // CONE MODERATOR
  Control.addVariable("LowConeModXStep",-0.0);      
  Control.addVariable("LowConeModYStep",4.0);      
  Control.addVariable("LowConeModZStep",-18.0);      
  Control.addVariable("LowConeModXYAngle",125.15);      
  Control.addVariable("LowConeModZAngle",0.0);      

  Control.addVariable("LowConeModIWidth",2.0);      
  Control.addVariable("LowConeModIHeight",2.0);      
  Control.addVariable("LowConeModOWidth",20.0);      
  Control.addVariable("LowConeModOHeight",10.0);      
  Control.addVariable("LowConeModLength",20.0);      
  Control.addVariable("LowConeModFaceThick",2.0);      
  Control.addVariable("LowConeModThick",1.5);      

  Control.addVariable("LowConeModAlThick",0.5);      

  Control.addVariable("LowConeModVacGap",0.3);      
  Control.addVariable("LowConeModWaterAlThick",0.5);      
  Control.addVariable("LowConeModWaterThick",2.0);      
  Control.addVariable("LowConeModVoidGap",0.3);      
  Control.addVariable("LowConeModWaterMat","H2O");      

  Control.addVariable("LowConeModModTemp",20.0);         // Temperature of H2 
  Control.addVariable("LowConeModModMat","ParaH2");            // Liquid H2
  Control.addVariable("LowConeModAlMat","Aluminium");              // Aluminium


  // CONE MODERATOR
  Control.addVariable("LowConeModBXStep",-0.0);      
  Control.addVariable("LowConeModBYStep",4.0);      
  Control.addVariable("LowConeModBZStep",-18.0);      
  Control.addVariable("LowConeModBXYAngle",125.15-180.0);      
  Control.addVariable("LowConeModBZAngle",0.0);      

  Control.addVariable("LowConeModBIWidth",2.0);      
  Control.addVariable("LowConeModBIHeight",2.0);      
  Control.addVariable("LowConeModBOWidth",20.0);      
  Control.addVariable("LowConeModBOHeight",10.0);      
  Control.addVariable("LowConeModBLength",20.0);      
  Control.addVariable("LowConeModBFaceThick",2.0);      
  Control.addVariable("LowConeModBThick",1.5);      

  Control.addVariable("LowConeModBAlThick",0.5);      

  Control.addVariable("LowConeModBVacGap",0.3);      
  Control.addVariable("LowConeModBWaterAlThick",0.5);      
  Control.addVariable("LowConeModBWaterThick",2.0);      
  Control.addVariable("LowConeModBVoidGap",0.3);      
  Control.addVariable("LowConeModBWaterMat","H2O");      

  Control.addVariable("LowConeModBModTemp",20.0);        // Temp. of H2
  Control.addVariable("LowConeModBModMat","ParaH2");        // Liquid H2
  Control.addVariable("LowConeModBAlMat","Aluminium");       // Aluminium

}


void
EssBeamLinesVariables(FuncDataBase& Control)
  /*!
    Create all the beamline variabes
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("essVariables[F]","EssBeamLinesVariables");
  for(int i=0;i<4;i++)
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
      Control.addVariable(baseKey+"BeamHeight",7.2);
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
      Control.addVariable(baseKey+"1XYangle",27.50); 
      Control.addVariable(baseKey+"2XYangle",22.5); 
      Control.addVariable(baseKey+"3XYangle",17.5); 
      Control.addVariable(baseKey+"4XYangle",12.5); 
      Control.addVariable(baseKey+"5XYangle",7.5); 
      Control.addVariable(baseKey+"6XYangle",2.5); 
      Control.addVariable(baseKey+"7XYangle",-2.5); 
      Control.addVariable(baseKey+"8XYangle",-7.5);
      Control.addVariable(baseKey+"9XYangle",-12.5); 
      Control.addVariable(baseKey+"10XYangle",-17.5); 
      Control.addVariable(baseKey+"11XYangle",-22.5); 
      Control.addVariable(baseKey+"12XYangle",-27.50); 
    }
  return;
}

}  // NAMESPACE setVariable
