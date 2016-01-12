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
  Control.addVariable("BilbaoWheelYStep",112.2);
  Control.addVariable("BilbaoWheelZStep",0.0);
  Control.addVariable("BilbaoWheelXYangle",-15.0);
  Control.addVariable("BilbaoWheelZangle",0.0);
  Control.addVariable("BilbaoWheelTargetHeight",8.0);
  Control.addVariable("BilbaoWheelVoidTungstenThick", 0.1);
  Control.addVariable("BilbaoWheelSteelTungstenThick", 0.2);
  Control.addVariable("BilbaoWheelTemp",300.0);
  Control.addVariable("BilbaoWheelCoolantThick",0.5);
  Control.addVariable("BilbaoWheelCaseThick",1.0);
  Control.addVariable("BilbaoWheelCaseThickIn",1.5);
  Control.addVariable("BilbaoWheelVoidThick",1.5);

  Control.addVariable("BilbaoWheelInnerRadius",45);
  Control.addVariable("BilbaoWheelCoolantRadiusIn",64.07);
  Control.addVariable("BilbaoWheelCoolantRadiusOut",128.95);
  Control.addVariable("BilbaoWheelCaseRadius",129.15);
  Control.addVariable("BilbaoWheelVoidRadius",131.15);
  Control.addVariable("BilbaoWheelAspectRatio", 0.00138);
  Control.addVariable("BilbaoWheelTemperature", 600);

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

  // Inner structure - engineering details
  Control.addVariable("BilbaoWheelInnerStructureBrickLength",3);
  Control.addVariable("BilbaoWheelInnerStructureBrickWidth",1);
  Control.addVariable("BilbaoWheelInnerStructureBrickMat","Tungsten600K");

  Control.addVariable("BilbaoWheelInnerStructureBrickGapLength",0.2);
  Control.addVariable("BilbaoWheelInnerStructureBrickGapWidth",0.2);
  Control.addVariable("BilbaoWheelInnerStructureBrickGapMat","Helium"); // shoud be the same as BilbaoWheelHeMat

  Control.addVariable("BilbaoWheelInnerStructureNSectors", 36);
  Control.addVariable("BilbaoWheelInnerStructureNBrickSectors", 0);
  Control.addVariable("BilbaoWheelInnerStructureSectorSepThick", 1.0);
  Control.addVariable("BilbaoWheelInnerStructureSectorSepMat", "SS316L785");

  Control.addVariable("BilbaoWheelInnerStructureNSteelLayers", 3);
  Control.addVariable("BilbaoWheelInnerStructureBrickSteelMat", "SS316L");

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

  pipeVariables(Control);
  
  
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
  Control.addVariable("LowAFlightAngleZTop",0.9); // Step up angle ESS-0032315.3
  Control.addVariable("LowAFlightAngleZBase",0.9);  // Step down angle ESS-0032315.3
  Control.addVariable("LowAFlightHeight",6.1);
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
  Control.addVariable("LowBFlightHeight");
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
  ELog::EM << "TopAFlightAngleZTop must be 1.3 but we use 1.1 - otherwise it cuts the tharget wheel" << ELog::endCrit;
  Control.addVariable("TopAFlightAngleZTop", 1.1);  // Step down angle !!! 1.455 is too much - fight line cuts the Bilbao target wheel ESS-0032315.3
  Control.addVariable("TopAFlightAngleZBase", 1.33); // Step up angle ESS-0032315.3
  //  Control.Parse("TopFlyTotalHeight");
  Control.addVariable("TopAFlightHeight", 2.9);     // Full height = TopFlyTotalHeight
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
  Control.addVariable("BeRefRadius",35);
  Control.addVariable("BeRefHeight",74.6); // 20 cm height of Be WITH THE GIVEN HEIGHTS OF CUP MODERATORS AND TARGET-MODERATOR DISTANCES
  Control.addVariable("BeRefWallThick",3); // measured by a liner from the Yannnick's BeRef slides 2015-11-11, page #3
  Control.addVariable("BeRefWallThickLow",0.0);
  Control.addVariable("BeRefTargetSepThick",13.0);
  Control.addVariable("BeRefLowVoidThick",2.3);
  Control.addVariable("BeRefTopVoidThick",2.3);
  Control.addVariable("BeRefRefMat","Be5H2O");
  //  Control.addVariable("BeRefRefMat","Be300K");


  Control.addVariable("BeRefWallMat","SS316L"); // Marc said 12 Jan 2016
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

  Control.addVariable("BeRefInnerStructureNLayers", 6);
  Control.addVariable("BeRefInnerStructureBaseLen1", 0.258);
  Control.addVariable("BeRefInnerStructureBaseLen2", 0.258+0.075);
  Control.addVariable("BeRefInnerStructureBaseLen3", 0.258*2+0.075);
  Control.addVariable("BeRefInnerStructureBaseLen4", 0.258*2+0.075+0.075);
  Control.addVariable("BeRefInnerStructureBaseLen5", 0.258*2+0.075+0.075+0.258);
  Control.addVariable("BeRefInnerStructureMat0", "Beryllium");
  Control.addVariable("BeRefInnerStructureMat1", "H2O");
  Control.addVariable("BeRefInnerStructureMat2", "Beryllium");
  Control.addVariable("BeRefInnerStructureMat3", "H2O");
  Control.addVariable("BeRefInnerStructureMat4", "Beryllium");
  Control.addVariable("BeRefInnerStructureMat5", "H2O");


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
  Control.addVariable("LowFlyTotalHeight",7.8); // to center it at -15.3 as in ESS-0032315.3 (mind 4 mm cold Al thick)
  
  Control.addVariable("LowFlyLeftLobeXStep",1.0);  
  Control.addVariable("LowFlyLeftLobeYStep",0.0);  

  Control.addVariable("LowFlyLeftLobeCorner1",Geometry::Vec3D(0,0.5,0));
  Control.addVariable("LowFlyLeftLobeCorner2",Geometry::Vec3D(-14.4,-13.2,0));
  Control.addVariable("LowFlyLeftLobeCorner3",Geometry::Vec3D(14.4,-13.2,0));
  
  Control.addVariable("LowFlyLeftLobeRadius1",5.0);
  Control.addVariable("LowFlyLeftLobeRadius2",2.506);
  Control.addVariable("LowFlyLeftLobeRadius3",2.506);

  Control.addVariable("LowFlyLeftLobeModMat","HPARA"); // email from LZ 27 Nov 2015
  Control.addVariable("LowFlyLeftLobeModTemp",20.0);

  Control.addVariable("LowFlyLeftLobeNLayers",4);
  Control.addVariable("LowFlyLeftLobeThick1",0.4);
  Control.addVariable("LowFlyLeftLobeMat1","Aluminium20K");

  Control.addVariable("LowFlyLeftLobeHeight1",0.4);
  Control.addVariable("LowFlyLeftLobeDepth1",0.4);
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

  Control.addVariable("LowFlyRightLobeCorner1",Geometry::Vec3D(0,0.5,0));
  Control.addVariable("LowFlyRightLobeCorner2",Geometry::Vec3D(-14.4,-13.2,0));
  Control.addVariable("LowFlyRightLobeCorner3",Geometry::Vec3D(14.4,-13.2,0));

  Control.addVariable("LowFlyRightLobeRadius1",5.0);
  Control.addVariable("LowFlyRightLobeRadius2",2.506);
  Control.addVariable("LowFlyRightLobeRadius3",2.506);

  Control.addVariable("LowFlyRightLobeModMat","HPARA"); // email from LZ 27 Nov 2015
  Control.addVariable("LowFlyRightLobeModTemp",20.0);

  Control.addVariable("LowFlyRightLobeNLayers",4);
  Control.addVariable("LowFlyRightLobeThick1",0.4);
  Control.addVariable("LowFlyRightLobeMat1","Aluminium20K");

  Control.addVariable("LowFlyRightLobeHeight1",0.4);
  Control.addVariable("LowFlyRightLobeDepth1",0.4);
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
  Control.addVariable("LowFlyLeftWaterSideWaterThick",2.2);
  Control.addVariable("LowFlyLeftWaterSideWaterMat","H2O");
  Control.addVariable("LowFlyLeftWaterSideWaterCutAngle",60.0);
  Control.addVariable("LowFlyLeftWaterSideWaterCutDist",0.0);
  Control.addVariable("LowFlyLeftWaterModMat","H2O");
  Control.addVariable("LowFlyLeftWaterWallMat","Aluminium");
  Control.addVariable("LowFlyLeftWaterModTemp",300.0);

  Control.addVariable("LowFlyRightWaterWidth",10.6672);
  Control.addVariable("LowFlyRightWaterWallThick",0.347);
  Control.addVariable("LowFlyRightWaterSideWaterThick",2.2);
  Control.addVariable("LowFlyRightWaterSideWaterMat","H2O");
  Control.addVariable("LowFlyRightWaterSideWaterCutAngle",60.0);
  Control.addVariable("LowFlyRightWaterSideWaterCutDist",0.0);
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

  Control.addVariable("TopFlyLeftLobeCorner1",Geometry::Vec3D(0,0.5,0));
  Control.addVariable("TopFlyLeftLobeCorner2",Geometry::Vec3D(-14.4,-13.2,0));
  Control.addVariable("TopFlyLeftLobeCorner3",Geometry::Vec3D(14.4,-13.2,0));
  
  Control.addVariable("TopFlyLeftLobeRadius1",5.0);
  Control.addVariable("TopFlyLeftLobeRadius2",2.506);
  Control.addVariable("TopFlyLeftLobeRadius3",2.506);

  Control.addVariable("TopFlyLeftLobeModMat","HPARA"); // email from LZ 27 Nov 2015
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

  Control.addVariable("TopFlyRightLobeCorner1",Geometry::Vec3D(0,0.5,0));
  Control.addVariable("TopFlyRightLobeCorner2",Geometry::Vec3D(-14.4,-13.2,0));
  Control.addVariable("TopFlyRightLobeCorner3",Geometry::Vec3D(14.4,-13.2,0));

  Control.addVariable("TopFlyRightLobeRadius1",5.0);
  Control.addVariable("TopFlyRightLobeRadius2",2.506);
  Control.addVariable("TopFlyRightLobeRadius3",2.506);

  Control.addVariable("TopFlyRightLobeModMat","HPARA"); // email from LZ 27 Nov 2015
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
  Control.addVariable("TopFlyLeftWaterSideWaterThick",2.2);
  Control.addVariable("TopFlyLeftWaterSideWaterMat","H2O");
  Control.addVariable("TopFlyLeftWaterSideWaterCutAngle",60.0);
  Control.addVariable("TopFlyLeftWaterSideWaterCutDist",0.0);
  Control.addVariable("TopFlyLeftWaterModMat","H2O");
  Control.addVariable("TopFlyLeftWaterWallMat","Aluminium");
  Control.addVariable("TopFlyLeftWaterModTemp",300.0);

  Control.addVariable("TopFlyRightWaterWidth",10.6672);
  Control.addVariable("TopFlyRightWaterWallThick",0.347);
  Control.addVariable("TopFlyRightWaterSideWaterThick",2.2);
  Control.addVariable("TopFlyRightWaterSideWaterMat","H2O");
  Control.addVariable("TopFlyRightWaterSideWaterCutAngle",60.0);
  Control.addVariable("TopFlyRightWaterSideWaterCutDist",0.0);
  Control.addVariable("TopFlyRightWaterModMat","H2O");
  Control.addVariable("TopFlyRightWaterWallMat","Aluminium");
  Control.addVariable("TopFlyRightWaterModTemp",300.0);

  Control.addVariable("LowPreModNLayers",4);
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
  Control.Parse("BeRefRadius-LowPreModRadius0-LowPreModRadius1+BeRefWallThick-0.0"); // 0.0 is LowPreModRadius3
  Control.addVariable("LowPreModRadius2");
  Control.addVariable("LowPreModMat2","Iron10H2O");
  Control.addVariable("LowPreModHeight3",0.3);
  Control.addVariable("LowPreModDepth3",0.3);
  Control.addVariable("LowPreModRadius3", 0.0);
  Control.addVariable("LowPreModMat3","Aluminium");
  //  Control.Parse("LowAFlightAngleZBase");
  Control.addVariable("LowPreModTiltAngle", 0.0);

  Control.addVariable("LowPreModFlowGuideWallThick", 0.3);
  Control.addVariable("LowPreModFlowGuideWallMat", "Aluminium");
  Control.addVariable("LowPreModFlowGuideNBaffles", 9);
  Control.addVariable("LowPreModFlowGuideGapWidth", 3);

  Control.addVariable("TopPreModNLayers",4);
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
  Control.Parse("BeRefRadius-TopPreModRadius0-TopPreModRadius1+BeRefWallThick-0.0"); // 0.0 is TopPreModRadius3
  Control.addVariable("TopPreModRadius2");
  Control.addVariable("TopPreModMat2","Iron10H2O");
  Control.addVariable("TopPreModHeight3",0.3);
  Control.addVariable("TopPreModDepth3",0.3);
  //  Control.Parse("BeRefWallThick");
  Control.addVariable("TopPreModRadius3", 0.0);
  Control.addVariable("TopPreModMat3","Aluminium");
  //  Control.Parse("TopAFlightAngleZTop");
  Control.addVariable("TopPreModTiltAngle", 0.0);
  Control.addVariable("TopPreModTiltRadius", 20.0);

  Control.addVariable("TopPreWingMat", "H2O");
  Control.addVariable("TopPreWingThick", 0.45); // ESS-0032315.3
  Control.addVariable("TopPreWingWallMat", "Aluminium");
  Control.addVariable("TopPreWingWallThick", 0.3);
  Control.addVariable("TopPreWingTiltAngle", 2.0);
  Control.addVariable("TopPreWingTiltRadius", 39.0/2); // =19.5 ESS-0032315.3

  Control.addVariable("LowPreWingMat", "H2O");
  Control.addVariable("LowPreWingThick", 0.55);  // ESS-0032315.3
  Control.addVariable("LowPreWingWallMat", "Aluminium");
  Control.addVariable("LowPreWingWallThick", 0.3);
  Control.addVariable("LowPreWingTiltAngle", 0.9); // ESS-0032315.3
  Control.addVariable("LowPreWingTiltRadius", 39.0/2.0);  // = 19.5  ESS-0032315.3


  Control.addVariable("TopCapWingMat", "H2O");
  Control.addVariable("TopCapWingThick", 0.45); // ESS-0032315.3
  Control.addVariable("TopCapWingWallMat", "Aluminium");
  Control.addVariable("TopCapWingWallThick", 0.3);
  Control.addVariable("TopCapWingTiltAngle", 1.33); // ESS-0032315.3
  Control.addVariable("TopCapWingTiltRadius", 39.0/2); // = 19.5  ESS-0032315.3

  Control.addVariable("LowCapWingMat", "H2O");
  Control.addVariable("LowCapWingThick", 0.55);  // ESS-0032315.3
  Control.addVariable("LowCapWingWallMat", "Aluminium");
  Control.addVariable("LowCapWingWallThick", 0.3);
  Control.addVariable("LowCapWingTiltAngle", 0.9); // ESS-0032315.3
  Control.addVariable("LowCapWingTiltRadius", 39.0/2.0); // = 19.5  ESS-0032315.3

  Control.Parse("LowPreModFlowGuideWallThick");
  Control.addVariable("TopPreModFlowGuideWallThick");
  Control.addVariable("TopPreModFlowGuideWallMat", "Aluminium");
  Control.Parse("LowPreModFlowGuideNBaffles");
  Control.addVariable("TopPreModFlowGuideNBaffles");
  Control.Parse("LowPreModFlowGuideGapWidth");
  Control.addVariable("TopPreModFlowGuideGapWidth");

  Control.addVariable("LowCapModNLayers",2);
  Control.addVariable("LowCapModHeight0",0.5);
  Control.addVariable("LowCapModDepth0", 0.5);
  Control.addVariable("LowCapModRadius0",35);
  Control.addVariable("LowCapModMat0","H2O");
  Control.addVariable("LowCapModHeight1",0.0);
  Control.addVariable("LowCapModDepth1",0.3);
  Control.Parse("BeRefWallThick");
  Control.addVariable("LowCapModRadius1");
  Control.addVariable("LowCapModMat1","Aluminium");
  //  Control.Parse("LowAFlightAngleZBase");
  Control.addVariable("LowCapModTiltAngle", 0.0);

  Control.addVariable("LowCapModFlowGuideWallThick", 0.3);
  Control.addVariable("LowCapModFlowGuideWallMat", "Aluminium");
  Control.addVariable("LowCapModFlowGuideNBaffles", 9);
  Control.addVariable("LowCapModFlowGuideGapWidth", 3);

  Control.addVariable("TopCapModNLayers",2);
  Control.addVariable("TopCapModHeight0",0.5);
  Control.addVariable("TopCapModDepth0", 0.5);
  Control.addVariable("TopCapModRadius0",35);
  Control.addVariable("TopCapModMat0","H2O");
  Control.addVariable("TopCapModHeight1",0.0);
  Control.addVariable("TopCapModDepth1",0.3);
  Control.Parse("BeRefWallThick");
  Control.addVariable("TopCapModRadius1");
  Control.addVariable("TopCapModMat1","Aluminium");
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


    Control.addVariable("F205Length", 440.0);
    Control.addVariable("F205Delta", 0.0);
    Control.addVariable("F205ViewWidth", 6);
    Control.addVariable("F205LinkPoint", 0.0);
    Control.addVariable("F205XB", 11.0);
    Control.addVariable("F205YB", -14.0);
    Control.addVariable("F205ZB", 14.0);
    Control.addVariable("F205XC", 11.0);
    Control.addVariable("F205YC", -8.0);
    Control.addVariable("F205ZC", 14.0);
    Control.addVariable("F205ZG", 11.0);
    Control.addVariable("F205X", 999.0);
    Control.addVariable("F205Y", -18.0);
    Control.addVariable("F205Z", 13.0);

    Control.addVariable("F215Length", 440.0);
    Control.addVariable("F215Delta", 0.0);
    Control.addVariable("F215ViewWidth", 6);
    Control.addVariable("F215LinkPoint", 0.0);
    Control.addVariable("F215XB", 11.0);
    Control.addVariable("F215YB", -14.0);
    Control.addVariable("F215ZB", 14.0);
    Control.addVariable("F215XC", 11.0);
    Control.addVariable("F215YC", -8.0);
    Control.addVariable("F215ZC", 14.0);
    Control.addVariable("F215ZG", 11.0);
    Control.addVariable("F215X", -999.0);
    Control.addVariable("F215Y", -18.0);
    Control.addVariable("F215Z", 13.0);

    Control.addVariable("F225Length", 440.0);
    Control.addVariable("F225Delta", 0.0);
    Control.addVariable("F225ViewWidth", 6);
    Control.addVariable("F225LinkPoint", 0.0);
    Control.addVariable("F225XB", 11.0);
    Control.addVariable("F225YB", -14.0);
    Control.addVariable("F225ZB", 14.0);
    Control.addVariable("F225XC", 11.0);
    Control.addVariable("F225YC", -8.0);
    Control.addVariable("F225ZC", 14.0);
    Control.addVariable("F225ZG", 11.0);
    Control.addVariable("F225X", -999.0);
    Control.addVariable("F225Y", -18.0);
    Control.addVariable("F225Z", 13.0);


    Control.addVariable("F235Length", 440.0);
    Control.addVariable("F235Delta", 0.0);
    Control.addVariable("F235ViewWidth", 6);
    Control.addVariable("F235LinkPoint", 0.0);
    Control.addVariable("F235XB", 11.0);
    Control.addVariable("F235YB", -14.0);
    Control.addVariable("F235ZB", 14.0);
    Control.addVariable("F235XC", 11.0);
    Control.addVariable("F235YC", -8.0);
    Control.addVariable("F235ZC", 14.0);
    Control.addVariable("F235ZG", 11.0);
    Control.addVariable("F235X", -999.0);
    Control.addVariable("F235Y", -18.0);
    Control.addVariable("F235Z", 13.0);

    Control.addVariable("F245Length", 440.0);
    Control.addVariable("F245Delta", 0.0);
    Control.addVariable("F245ViewWidth", 6);
    Control.addVariable("F245LinkPoint", 0.0);
    Control.addVariable("F245XB", 11.0);
    Control.addVariable("F245YB", -14.0);
    Control.addVariable("F245ZB", 14.0);
    Control.addVariable("F245XC", 11.0);
    Control.addVariable("F245YC", -8.0);
    Control.addVariable("F245ZC", 14.0);
    Control.addVariable("F245ZG", 11.0);
    Control.addVariable("F245X", -999.0);
    Control.addVariable("F245Y", -18.0);
    Control.addVariable("F245Z", 13.0);

    Control.addVariable("F255Length", 440.0);
    Control.addVariable("F255Delta", 0.0);
    Control.addVariable("F255ViewWidth", 6);
    Control.addVariable("F255LinkPoint", 0.0);
    Control.addVariable("F255XB", 11.0);
    Control.addVariable("F255YB", -14.0);
    Control.addVariable("F255ZB", 14.0);
    Control.addVariable("F255XC", 11.0);
    Control.addVariable("F255YC", -8.0);
    Control.addVariable("F255ZC", 14.0);
    Control.addVariable("F255ZG", 11.0);
    Control.addVariable("F255X", -999.0);
    Control.addVariable("F255Y", -18.0);
    Control.addVariable("F255Z", 13.0);

    Control.addVariable("F265Length", 440.0);
    Control.addVariable("F265Delta", 0.0);
    Control.addVariable("F265ViewWidth", 6);
    Control.addVariable("F265LinkPoint", 0.0);
    Control.addVariable("F265XB", 11.0);
    Control.addVariable("F265YB", -14.0);
    Control.addVariable("F265ZB", 14.0);
    Control.addVariable("F265XC", 11.0);
    Control.addVariable("F265YC", -8.0);
    Control.addVariable("F265ZC", 14.0);
    Control.addVariable("F265ZG", 11.0);
    Control.addVariable("F265X", -999.0);
    Control.addVariable("F265Y", -18.0);
    Control.addVariable("F265Z", 13.0);

    Control.addVariable("F275Length", 440.0);
    Control.addVariable("F275Delta", 0.0);
    Control.addVariable("F275ViewWidth", 6);
    Control.addVariable("F275LinkPoint", 0.0);
    Control.addVariable("F275XB", 11.0);
    Control.addVariable("F275YB", -14.0);
    Control.addVariable("F275ZB", 14.0);
    Control.addVariable("F275XC", 11.0);
    Control.addVariable("F275YC", -8.0);
    Control.addVariable("F275ZC", 14.0);
    Control.addVariable("F275ZG", 11.0);
    Control.addVariable("F275X", -999.0);
    Control.addVariable("F275Y", -18.0);
    Control.addVariable("F275Z", 13.0);

    Control.addVariable("F285Length", 440.0);
    Control.addVariable("F285Delta", 0.0);
    Control.addVariable("F285ViewWidth", 6);
    Control.addVariable("F285LinkPoint", 0.0);
    Control.addVariable("F285XB", 11.0);
    Control.addVariable("F285YB", -14.0);
    Control.addVariable("F285ZB", 14.0);
    Control.addVariable("F285XC", 11.0);
    Control.addVariable("F285YC", -8.0);
    Control.addVariable("F285ZC", 14.0);
    Control.addVariable("F285ZG", 11.0);
    Control.addVariable("F285X", -999.0);
    Control.addVariable("F285Y", -18.0);
    Control.addVariable("F285Z", 13.0);

    Control.addVariable("F295Length", 440.0);
    Control.addVariable("F295Delta", 0.0);
    Control.addVariable("F295ViewWidth", 6);
    Control.addVariable("F295LinkPoint", 0.0);
    Control.addVariable("F295XB", 11.0);
    Control.addVariable("F295YB", -14.0);
    Control.addVariable("F295ZB", 14.0);
    Control.addVariable("F295XC", 11.0);
    Control.addVariable("F295YC", -8.0);
    Control.addVariable("F295ZC", 14.0);
    Control.addVariable("F295ZG", 11.0);
    Control.addVariable("F295X", -999.0);
    Control.addVariable("F295Y", -18.0);
    Control.addVariable("F295Z", 13.0);

    Control.addVariable("F305Length", 440.0);
    Control.addVariable("F305Delta", 0.0);
    Control.addVariable("F305ViewWidth", 6);
    Control.addVariable("F305LinkPoint", 0.0);
    Control.addVariable("F305XB", 11.0);
    Control.addVariable("F305YB", -14.0);
    Control.addVariable("F305ZB", 14.0);
    Control.addVariable("F305XC", 11.0);
    Control.addVariable("F305YC", -8.0);
    Control.addVariable("F305ZC", 14.0);
    Control.addVariable("F305ZG", 11.0);
    Control.addVariable("F305X", 999.0);
    Control.addVariable("F305Y", -18.0);
    Control.addVariable("F305Z", 13.0);

    Control.addVariable("F315Length", 440.0);
    Control.addVariable("F315Delta", 0.0);
    Control.addVariable("F315ViewWidth", 6);
    Control.addVariable("F315LinkPoint", 0.0);
    Control.addVariable("F315XB", 11.0);
    Control.addVariable("F315YB", -14.0);
    Control.addVariable("F315ZB", 14.0);
    Control.addVariable("F315XC", 11.0);
    Control.addVariable("F315YC", -8.0);
    Control.addVariable("F315ZC", 14.0);
    Control.addVariable("F315ZG", 11.0);
    Control.addVariable("F315X", -999.0);
    Control.addVariable("F315Y", -18.0);
    Control.addVariable("F315Z", 13.0);

    Control.addVariable("F325Length", 440.0);
    Control.addVariable("F325Delta", 0.0);
    Control.addVariable("F325ViewWidth", 6);
    Control.addVariable("F325LinkPoint", 0.0);
    Control.addVariable("F325XB", 11.0);
    Control.addVariable("F325YB", -14.0);
    Control.addVariable("F325ZB", 14.0);
    Control.addVariable("F325XC", 11.0);
    Control.addVariable("F325YC", -8.0);
    Control.addVariable("F325ZC", 14.0);
    Control.addVariable("F325ZG", 11.0);
    Control.addVariable("F325X", -999.0);
    Control.addVariable("F325Y", -18.0);
    Control.addVariable("F325Z", 13.0);


    Control.addVariable("F335Length", 440.0);
    Control.addVariable("F335Delta", 0.0);
    Control.addVariable("F335ViewWidth", 6);
    Control.addVariable("F335LinkPoint", 0.0);
    Control.addVariable("F335XB", 11.0);
    Control.addVariable("F335YB", -14.0);
    Control.addVariable("F335ZB", 14.0);
    Control.addVariable("F335XC", 11.0);
    Control.addVariable("F335YC", -8.0);
    Control.addVariable("F335ZC", 14.0);
    Control.addVariable("F335ZG", 11.0);
    Control.addVariable("F335X", -999.0);
    Control.addVariable("F335Y", -18.0);
    Control.addVariable("F335Z", 13.0);

    Control.addVariable("F345Length", 440.0);
    Control.addVariable("F345Delta", 0.0);
    Control.addVariable("F345ViewWidth", 6);
    Control.addVariable("F345LinkPoint", 0.0);
    Control.addVariable("F345XB", 11.0);
    Control.addVariable("F345YB", -14.0);
    Control.addVariable("F345ZB", 14.0);
    Control.addVariable("F345XC", 11.0);
    Control.addVariable("F345YC", -8.0);
    Control.addVariable("F345ZC", 14.0);
    Control.addVariable("F345ZG", 11.0);
    Control.addVariable("F345X", -999.0);
    Control.addVariable("F345Y", -18.0);
    Control.addVariable("F345Z", 13.0);

    Control.addVariable("F355Length", 440.0);
    Control.addVariable("F355Delta", 0.0);
    Control.addVariable("F355ViewWidth", 6);
    Control.addVariable("F355LinkPoint", 0.0);
    Control.addVariable("F355XB", 11.0);
    Control.addVariable("F355YB", -14.0);
    Control.addVariable("F355ZB", 14.0);
    Control.addVariable("F355XC", 11.0);
    Control.addVariable("F355YC", -8.0);
    Control.addVariable("F355ZC", 14.0);
    Control.addVariable("F355ZG", 11.0);
    Control.addVariable("F355X", -999.0);
    Control.addVariable("F355Y", -18.0);
    Control.addVariable("F355Z", 13.0);

    Control.addVariable("F365Length", 440.0);
    Control.addVariable("F365Delta", 0.0);
    Control.addVariable("F365ViewWidth", 6);
    Control.addVariable("F365LinkPoint", 0.0);
    Control.addVariable("F365XB", 11.0);
    Control.addVariable("F365YB", -14.0);
    Control.addVariable("F365ZB", 14.0);
    Control.addVariable("F365XC", 11.0);
    Control.addVariable("F365YC", -8.0);
    Control.addVariable("F365ZC", 14.0);
    Control.addVariable("F365ZG", 11.0);
    Control.addVariable("F365X", -999.0);
    Control.addVariable("F365Y", -18.0);
    Control.addVariable("F365Z", 13.0);

    Control.addVariable("F375Length", 440.0);
    Control.addVariable("F375Delta", 0.0);
    Control.addVariable("F375ViewWidth", 6);
    Control.addVariable("F375LinkPoint", 0.0);
    Control.addVariable("F375XB", 11.0);
    Control.addVariable("F375YB", -14.0);
    Control.addVariable("F375ZB", 14.0);
    Control.addVariable("F375XC", 11.0);
    Control.addVariable("F375YC", -8.0);
    Control.addVariable("F375ZC", 14.0);
    Control.addVariable("F375ZG", 11.0);
    Control.addVariable("F375X", -999.0);
    Control.addVariable("F375Y", -18.0);
    Control.addVariable("F375Z", 13.0);

    Control.addVariable("F385Length", 440.0);
    Control.addVariable("F385Delta", 0.0);
    Control.addVariable("F385ViewWidth", 6);
    Control.addVariable("F385LinkPoint", 0.0);
    Control.addVariable("F385XB", 11.0);
    Control.addVariable("F385YB", -14.0);
    Control.addVariable("F385ZB", 14.0);
    Control.addVariable("F385XC", 11.0);
    Control.addVariable("F385YC", -8.0);
    Control.addVariable("F385ZC", 14.0);
    Control.addVariable("F385ZG", 11.0);
    Control.addVariable("F385X", -999.0);
    Control.addVariable("F385Y", -18.0);
    Control.addVariable("F385Z", 13.0);

    Control.addVariable("F395Length", 440.0);
    Control.addVariable("F395Delta", 0.0);
    Control.addVariable("F395ViewWidth", 6);
    Control.addVariable("F395LinkPoint", 0.0);
    Control.addVariable("F395XB", 11.0);
    Control.addVariable("F395YB", -14.0);
    Control.addVariable("F395ZB", 14.0);
    Control.addVariable("F395XC", 11.0);
    Control.addVariable("F395YC", -8.0);
    Control.addVariable("F395ZC", 14.0);
    Control.addVariable("F395ZG", 11.0);
    Control.addVariable("F395X", -999.0);
    Control.addVariable("F395Y", -18.0);
    Control.addVariable("F395Z", 13.0);

    Control.addVariable("F405Length", 440.0);
    Control.addVariable("F405Delta", 0.0);
    Control.addVariable("F405ViewWidth", 6);
    Control.addVariable("F405LinkPoint", 0.0);
    Control.addVariable("F405XB", 11.0);
    Control.addVariable("F405YB", -14.0);
    Control.addVariable("F405ZB", 14.0);
    Control.addVariable("F405XC", 11.0);
    Control.addVariable("F405YC", -8.0);
    Control.addVariable("F405ZC", 14.0);
    Control.addVariable("F405ZG", 11.0);
    Control.addVariable("F405X", 999.0);
    Control.addVariable("F405Y", -18.0);
    Control.addVariable("F405Z", 13.0);

    Control.addVariable("F415Length", 440.0);
    Control.addVariable("F415Delta", 0.0);
    Control.addVariable("F415ViewWidth", 6);
    Control.addVariable("F415LinkPoint", 0.0);
    Control.addVariable("F415XB", 11.0);
    Control.addVariable("F415YB", -14.0);
    Control.addVariable("F415ZB", 14.0);
    Control.addVariable("F415XC", 11.0);
    Control.addVariable("F415YC", -8.0);
    Control.addVariable("F415ZC", 14.0);
    Control.addVariable("F415ZG", 11.0);
    Control.addVariable("F415X", -999.0);
    Control.addVariable("F415Y", -18.0);
    Control.addVariable("F415Z", 13.0);

    Control.addVariable("F425Length", 440.0);
    Control.addVariable("F425Delta", 0.0);
    Control.addVariable("F425ViewWidth", 6);
    Control.addVariable("F425LinkPoint", 0.0);
    Control.addVariable("F425XB", 11.0);
    Control.addVariable("F425YB", -14.0);
    Control.addVariable("F425ZB", 14.0);
    Control.addVariable("F425XC", 11.0);
    Control.addVariable("F425YC", -8.0);
    Control.addVariable("F425ZC", 14.0);
    Control.addVariable("F425ZG", 11.0);
    Control.addVariable("F425X", -999.0);
    Control.addVariable("F425Y", -18.0);
    Control.addVariable("F425Z", 13.0);


    Control.addVariable("F435Length", 440.0);
    Control.addVariable("F435Delta", 0.0);
    Control.addVariable("F435ViewWidth", 6);
    Control.addVariable("F435LinkPoint", 0.0);
    Control.addVariable("F435XB", 11.0);
    Control.addVariable("F435YB", -14.0);
    Control.addVariable("F435ZB", 14.0);
    Control.addVariable("F435XC", 11.0);
    Control.addVariable("F435YC", -8.0);
    Control.addVariable("F435ZC", 14.0);
    Control.addVariable("F435ZG", 11.0);
    Control.addVariable("F435X", -999.0);
    Control.addVariable("F435Y", -18.0);
    Control.addVariable("F435Z", 13.0);

    Control.addVariable("F445Length", 440.0);
    Control.addVariable("F445Delta", 0.0);
    Control.addVariable("F445ViewWidth", 6);
    Control.addVariable("F445LinkPoint", 0.0);
    Control.addVariable("F445XB", 11.0);
    Control.addVariable("F445YB", -14.0);
    Control.addVariable("F445ZB", 14.0);
    Control.addVariable("F445XC", 11.0);
    Control.addVariable("F445YC", -8.0);
    Control.addVariable("F445ZC", 14.0);
    Control.addVariable("F445ZG", 11.0);
    Control.addVariable("F445X", -999.0);
    Control.addVariable("F445Y", -18.0);
    Control.addVariable("F445Z", 13.0);

    Control.addVariable("F455Length", 440.0);
    Control.addVariable("F455Delta", 0.0);
    Control.addVariable("F455ViewWidth", 6);
    Control.addVariable("F455LinkPoint", 0.0);
    Control.addVariable("F455XB", 11.0);
    Control.addVariable("F455YB", -14.0);
    Control.addVariable("F455ZB", 14.0);
    Control.addVariable("F455XC", 11.0);
    Control.addVariable("F455YC", -8.0);
    Control.addVariable("F455ZC", 14.0);
    Control.addVariable("F455ZG", 11.0);
    Control.addVariable("F455X", -999.0);
    Control.addVariable("F455Y", -18.0);
    Control.addVariable("F455Z", 13.0);

    Control.addVariable("F465Length", 440.0);
    Control.addVariable("F465Delta", 0.0);
    Control.addVariable("F465ViewWidth", 6);
    Control.addVariable("F465LinkPoint", 0.0);
    Control.addVariable("F465XB", 11.0);
    Control.addVariable("F465YB", -14.0);
    Control.addVariable("F465ZB", 14.0);
    Control.addVariable("F465XC", 11.0);
    Control.addVariable("F465YC", -8.0);
    Control.addVariable("F465ZC", 14.0);
    Control.addVariable("F465ZG", 11.0);
    Control.addVariable("F465X", -999.0);
    Control.addVariable("F465Y", -18.0);
    Control.addVariable("F465Z", 13.0);

    Control.addVariable("F475Length", 440.0);
    Control.addVariable("F475Delta", 0.0);
    Control.addVariable("F475ViewWidth", 6);
    Control.addVariable("F475LinkPoint", 0.0);
    Control.addVariable("F475XB", 11.0);
    Control.addVariable("F475YB", -14.0);
    Control.addVariable("F475ZB", 14.0);
    Control.addVariable("F475XC", 11.0);
    Control.addVariable("F475YC", -8.0);
    Control.addVariable("F475ZC", 14.0);
    Control.addVariable("F475ZG", 11.0);
    Control.addVariable("F475X", -999.0);
    Control.addVariable("F475Y", -18.0);
    Control.addVariable("F475Z", 13.0);

    Control.addVariable("F485Length", 440.0);
    Control.addVariable("F485Delta", 0.0);
    Control.addVariable("F485ViewWidth", 6);
    Control.addVariable("F485LinkPoint", 0.0);
    Control.addVariable("F485XB", 11.0);
    Control.addVariable("F485YB", -14.0);
    Control.addVariable("F485ZB", 14.0);
    Control.addVariable("F485XC", 11.0);
    Control.addVariable("F485YC", -8.0);
    Control.addVariable("F485ZC", 14.0);
    Control.addVariable("F485ZG", 11.0);
    Control.addVariable("F485X", -999.0);
    Control.addVariable("F485Y", -18.0);
    Control.addVariable("F485Z", 13.0);

    Control.addVariable("F495Length", 440.0);
    Control.addVariable("F495Delta", 0.0);
    Control.addVariable("F495ViewWidth", 6);
    Control.addVariable("F495LinkPoint", 0.0);
    Control.addVariable("F495XB", 11.0);
    Control.addVariable("F495YB", -14.0);
    Control.addVariable("F495ZB", 14.0);
    Control.addVariable("F495XC", 11.0);
    Control.addVariable("F495YC", -8.0);
    Control.addVariable("F495ZC", 14.0);
    Control.addVariable("F495ZG", 11.0);
    Control.addVariable("F495X", -999.0);
    Control.addVariable("F495Y", -18.0);
    Control.addVariable("F495Z", 13.0);

    Control.addVariable("F505Length", 440.0);
    Control.addVariable("F505Delta", 0.0);
    Control.addVariable("F505ViewWidth", 6);
    Control.addVariable("F505LinkPoint", 0.0);
    Control.addVariable("F505XB", 11.0);
    Control.addVariable("F505YB", -14.0);
    Control.addVariable("F505ZB", 14.0);
    Control.addVariable("F505XC", 11.0);
    Control.addVariable("F505YC", -8.0);
    Control.addVariable("F505ZC", 14.0);
    Control.addVariable("F505ZG", 11.0);
    Control.addVariable("F505X", 999.0);
    Control.addVariable("F505Y", -18.0);
    Control.addVariable("F505Z", 13.0);

    Control.addVariable("F515Length", 440.0);
    Control.addVariable("F515Delta", 0.0);
    Control.addVariable("F515ViewWidth", 6);
    Control.addVariable("F515LinkPoint", 0.0);
    Control.addVariable("F515XB", 11.0);
    Control.addVariable("F515YB", -14.0);
    Control.addVariable("F515ZB", 14.0);
    Control.addVariable("F515XC", 11.0);
    Control.addVariable("F515YC", -8.0);
    Control.addVariable("F515ZC", 14.0);
    Control.addVariable("F515ZG", 11.0);
    Control.addVariable("F515X", -999.0);
    Control.addVariable("F515Y", -18.0);
    Control.addVariable("F515Z", 13.0);

    Control.addVariable("F525Length", 440.0);
    Control.addVariable("F525Delta", 0.0);
    Control.addVariable("F525ViewWidth", 6);
    Control.addVariable("F525LinkPoint", 0.0);
    Control.addVariable("F525XB", 11.0);
    Control.addVariable("F525YB", -14.0);
    Control.addVariable("F525ZB", 14.0);
    Control.addVariable("F525XC", 11.0);
    Control.addVariable("F525YC", -8.0);
    Control.addVariable("F525ZC", 14.0);
    Control.addVariable("F525ZG", 11.0);
    Control.addVariable("F525X", -999.0);
    Control.addVariable("F525Y", -18.0);
    Control.addVariable("F525Z", 13.0);


    Control.addVariable("F535Length", 440.0);
    Control.addVariable("F535Delta", 0.0);
    Control.addVariable("F535ViewWidth", 6);
    Control.addVariable("F535LinkPoint", 0.0);
    Control.addVariable("F535XB", 11.0);
    Control.addVariable("F535YB", -14.0);
    Control.addVariable("F535ZB", 14.0);
    Control.addVariable("F535XC", 11.0);
    Control.addVariable("F535YC", -8.0);
    Control.addVariable("F535ZC", 14.0);
    Control.addVariable("F535ZG", 11.0);
    Control.addVariable("F535X", -999.0);
    Control.addVariable("F535Y", -18.0);
    Control.addVariable("F535Z", 13.0);

    Control.addVariable("F545Length", 440.0);
    Control.addVariable("F545Delta", 0.0);
    Control.addVariable("F545ViewWidth", 6);
    Control.addVariable("F545LinkPoint", 0.0);
    Control.addVariable("F545XB", 11.0);
    Control.addVariable("F545YB", -14.0);
    Control.addVariable("F545ZB", 14.0);
    Control.addVariable("F545XC", 11.0);
    Control.addVariable("F545YC", -8.0);
    Control.addVariable("F545ZC", 14.0);
    Control.addVariable("F545ZG", 11.0);
    Control.addVariable("F545X", -999.0);
    Control.addVariable("F545Y", -18.0);
    Control.addVariable("F545Z", 13.0);

    Control.addVariable("F555Length", 440.0);
    Control.addVariable("F555Delta", 0.0);
    Control.addVariable("F555ViewWidth", 6);
    Control.addVariable("F555LinkPoint", 0.0);
    Control.addVariable("F555XB", 11.0);
    Control.addVariable("F555YB", -14.0);
    Control.addVariable("F555ZB", 14.0);
    Control.addVariable("F555XC", 11.0);
    Control.addVariable("F555YC", -8.0);
    Control.addVariable("F555ZC", 14.0);
    Control.addVariable("F555ZG", 11.0);
    Control.addVariable("F555X", -999.0);
    Control.addVariable("F555Y", -18.0);
    Control.addVariable("F555Z", 13.0);

    Control.addVariable("F565Length", 440.0);
    Control.addVariable("F565Delta", 0.0);
    Control.addVariable("F565ViewWidth", 6);
    Control.addVariable("F565LinkPoint", 0.0);
    Control.addVariable("F565XB", 11.0);
    Control.addVariable("F565YB", -14.0);
    Control.addVariable("F565ZB", 14.0);
    Control.addVariable("F565XC", 11.0);
    Control.addVariable("F565YC", -8.0);
    Control.addVariable("F565ZC", 14.0);
    Control.addVariable("F565ZG", 11.0);
    Control.addVariable("F565X", -999.0);
    Control.addVariable("F565Y", -18.0);
    Control.addVariable("F565Z", 13.0);

    Control.addVariable("F575Length", 440.0);
    Control.addVariable("F575Delta", 0.0);
    Control.addVariable("F575ViewWidth", 6);
    Control.addVariable("F575LinkPoint", 0.0);
    Control.addVariable("F575XB", 11.0);
    Control.addVariable("F575YB", -14.0);
    Control.addVariable("F575ZB", 14.0);
    Control.addVariable("F575XC", 11.0);
    Control.addVariable("F575YC", -8.0);
    Control.addVariable("F575ZC", 14.0);
    Control.addVariable("F575ZG", 11.0);
    Control.addVariable("F575X", -999.0);
    Control.addVariable("F575Y", -18.0);
    Control.addVariable("F575Z", 13.0);

    Control.addVariable("F585Length", 440.0);
    Control.addVariable("F585Delta", 0.0);
    Control.addVariable("F585ViewWidth", 6);
    Control.addVariable("F585LinkPoint", 0.0);
    Control.addVariable("F585XB", 11.0);
    Control.addVariable("F585YB", -14.0);
    Control.addVariable("F585ZB", 14.0);
    Control.addVariable("F585XC", 11.0);
    Control.addVariable("F585YC", -8.0);
    Control.addVariable("F585ZC", 14.0);
    Control.addVariable("F585ZG", 11.0);
    Control.addVariable("F585X", -999.0);
    Control.addVariable("F585Y", -18.0);
    Control.addVariable("F585Z", 13.0);

    Control.addVariable("F595Length", 440.0);
    Control.addVariable("F595Delta", 0.0);
    Control.addVariable("F595ViewWidth", 6);
    Control.addVariable("F595LinkPoint", 0.0);
    Control.addVariable("F595XB", 11.0);
    Control.addVariable("F595YB", -14.0);
    Control.addVariable("F595ZB", 14.0);
    Control.addVariable("F595XC", 11.0);
    Control.addVariable("F595YC", -8.0);
    Control.addVariable("F595ZC", 14.0);
    Control.addVariable("F595ZG", 11.0);
    Control.addVariable("F595X", -999.0);
    Control.addVariable("F595Y", -18.0);
    Control.addVariable("F595Z", 13.0);

    Control.addVariable("F605Length", 440.0);
    Control.addVariable("F605Delta", 0.0);
    Control.addVariable("F605ViewWidth", 6);
    Control.addVariable("F605LinkPoint", 0.0);
    Control.addVariable("F605XB", 11.0);
    Control.addVariable("F605YB", -14.0);
    Control.addVariable("F605ZB", 14.0);
    Control.addVariable("F605XC", 11.0);
    Control.addVariable("F605YC", -8.0);
    Control.addVariable("F605ZC", 14.0);
    Control.addVariable("F605ZG", 11.0);
    Control.addVariable("F605X", 999.0);
    Control.addVariable("F605Y", -18.0);
    Control.addVariable("F605Z", 13.0);

    Control.addVariable("F615Length", 440.0);
    Control.addVariable("F615Delta", 0.0);
    Control.addVariable("F615ViewWidth", 6);
    Control.addVariable("F615LinkPoint", 0.0);
    Control.addVariable("F615XB", 11.0);
    Control.addVariable("F615YB", -14.0);
    Control.addVariable("F615ZB", 14.0);
    Control.addVariable("F615XC", 11.0);
    Control.addVariable("F615YC", -8.0);
    Control.addVariable("F615ZC", 14.0);
    Control.addVariable("F615ZG", 11.0);
    Control.addVariable("F615X", -999.0);
    Control.addVariable("F615Y", -18.0);
    Control.addVariable("F615Z", 13.0);

    Control.addVariable("F625Length", 440.0);
    Control.addVariable("F625Delta", 0.0);
    Control.addVariable("F625ViewWidth", 6);
    Control.addVariable("F625LinkPoint", 0.0);
    Control.addVariable("F625XB", 11.0);
    Control.addVariable("F625YB", -14.0);
    Control.addVariable("F625ZB", 14.0);
    Control.addVariable("F625XC", 11.0);
    Control.addVariable("F625YC", -8.0);
    Control.addVariable("F625ZC", 14.0);
    Control.addVariable("F625ZG", 11.0);
    Control.addVariable("F625X", -999.0);
    Control.addVariable("F625Y", -18.0);
    Control.addVariable("F625Z", 13.0);


    Control.addVariable("F635Length", 440.0);
    Control.addVariable("F635Delta", 0.0);
    Control.addVariable("F635ViewWidth", 6);
    Control.addVariable("F635LinkPoint", 0.0);
    Control.addVariable("F635XB", 11.0);
    Control.addVariable("F635YB", -14.0);
    Control.addVariable("F635ZB", 14.0);
    Control.addVariable("F635XC", 11.0);
    Control.addVariable("F635YC", -8.0);
    Control.addVariable("F635ZC", 14.0);
    Control.addVariable("F635ZG", 11.0);
    Control.addVariable("F635X", -999.0);
    Control.addVariable("F635Y", -18.0);
    Control.addVariable("F635Z", 13.0);

    Control.addVariable("F645Length", 440.0);
    Control.addVariable("F645Delta", 0.0);
    Control.addVariable("F645ViewWidth", 6);
    Control.addVariable("F645LinkPoint", 0.0);
    Control.addVariable("F645XB", 11.0);
    Control.addVariable("F645YB", -14.0);
    Control.addVariable("F645ZB", 14.0);
    Control.addVariable("F645XC", 11.0);
    Control.addVariable("F645YC", -8.0);
    Control.addVariable("F645ZC", 14.0);
    Control.addVariable("F645ZG", 11.0);
    Control.addVariable("F645X", -999.0);
    Control.addVariable("F645Y", -18.0);
    Control.addVariable("F645Z", 13.0);

    Control.addVariable("F655Length", 440.0);
    Control.addVariable("F655Delta", 0.0);
    Control.addVariable("F655ViewWidth", 6);
    Control.addVariable("F655LinkPoint", 0.0);
    Control.addVariable("F655XB", 11.0);
    Control.addVariable("F655YB", -14.0);
    Control.addVariable("F655ZB", 14.0);
    Control.addVariable("F655XC", 11.0);
    Control.addVariable("F655YC", -8.0);
    Control.addVariable("F655ZC", 14.0);
    Control.addVariable("F655ZG", 11.0);
    Control.addVariable("F655X", -999.0);
    Control.addVariable("F655Y", -18.0);
    Control.addVariable("F655Z", 13.0);

    Control.addVariable("F665Length", 440.0);
    Control.addVariable("F665Delta", 0.0);
    Control.addVariable("F665ViewWidth", 6);
    Control.addVariable("F665LinkPoint", 0.0);
    Control.addVariable("F665XB", 11.0);
    Control.addVariable("F665YB", -14.0);
    Control.addVariable("F665ZB", 14.0);
    Control.addVariable("F665XC", 11.0);
    Control.addVariable("F665YC", -8.0);
    Control.addVariable("F665ZC", 14.0);
    Control.addVariable("F665ZG", 11.0);
    Control.addVariable("F665X", -999.0);
    Control.addVariable("F665Y", -18.0);
    Control.addVariable("F665Z", 13.0);

    Control.addVariable("F675Length", 440.0);
    Control.addVariable("F675Delta", 0.0);
    Control.addVariable("F675ViewWidth", 6);
    Control.addVariable("F675LinkPoint", 0.0);
    Control.addVariable("F675XB", 11.0);
    Control.addVariable("F675YB", -14.0);
    Control.addVariable("F675ZB", 14.0);
    Control.addVariable("F675XC", 11.0);
    Control.addVariable("F675YC", -8.0);
    Control.addVariable("F675ZC", 14.0);
    Control.addVariable("F675ZG", 11.0);
    Control.addVariable("F675X", -999.0);
    Control.addVariable("F675Y", -18.0);
    Control.addVariable("F675Z", 13.0);

    Control.addVariable("F685Length", 440.0);
    Control.addVariable("F685Delta", 0.0);
    Control.addVariable("F685ViewWidth", 6);
    Control.addVariable("F685LinkPoint", 0.0);
    Control.addVariable("F685XB", 11.0);
    Control.addVariable("F685YB", -14.0);
    Control.addVariable("F685ZB", 14.0);
    Control.addVariable("F685XC", 11.0);
    Control.addVariable("F685YC", -8.0);
    Control.addVariable("F685ZC", 14.0);
    Control.addVariable("F685ZG", 11.0);
    Control.addVariable("F685X", -999.0);
    Control.addVariable("F685Y", -18.0);
    Control.addVariable("F685Z", 13.0);

    Control.addVariable("F695Length", 440.0);
    Control.addVariable("F695Delta", 0.0);
    Control.addVariable("F695ViewWidth", 6);
    Control.addVariable("F695LinkPoint", 0.0);
    Control.addVariable("F695XB", 11.0);
    Control.addVariable("F695YB", -14.0);
    Control.addVariable("F695ZB", 14.0);
    Control.addVariable("F695XC", 11.0);
    Control.addVariable("F695YC", -8.0);
    Control.addVariable("F695ZC", 14.0);
    Control.addVariable("F695ZG", 11.0);
    Control.addVariable("F695X", -999.0);
    Control.addVariable("F695Y", -18.0);
    Control.addVariable("F695Z", 13.0);

    Control.addVariable("F705Length", 440.0);
    Control.addVariable("F705Delta", 0.0);
    Control.addVariable("F705ViewWidth", 6);
    Control.addVariable("F705LinkPoint", 0.0);
    Control.addVariable("F705XB", 11.0);
    Control.addVariable("F705YB", -14.0);
    Control.addVariable("F705ZB", 14.0);
    Control.addVariable("F705XC", 11.0);
    Control.addVariable("F705YC", -8.0);
    Control.addVariable("F705ZC", 14.0);
    Control.addVariable("F705ZG", 11.0);
    Control.addVariable("F705X", 999.0);
    Control.addVariable("F705Y", -18.0);
    Control.addVariable("F705Z", 13.0);

    Control.addVariable("F715Length", 440.0);
    Control.addVariable("F715Delta", 0.0);
    Control.addVariable("F715ViewWidth", 6);
    Control.addVariable("F715LinkPoint", 0.0);
    Control.addVariable("F715XB", 11.0);
    Control.addVariable("F715YB", -14.0);
    Control.addVariable("F715ZB", 14.0);
    Control.addVariable("F715XC", 11.0);
    Control.addVariable("F715YC", -8.0);
    Control.addVariable("F715ZC", 14.0);
    Control.addVariable("F715ZG", 11.0);
    Control.addVariable("F715X", -999.0);
    Control.addVariable("F715Y", -18.0);
    Control.addVariable("F715Z", 13.0);

    Control.addVariable("F725Length", 440.0);
    Control.addVariable("F725Delta", 0.0);
    Control.addVariable("F725ViewWidth", 6);
    Control.addVariable("F725LinkPoint", 0.0);
    Control.addVariable("F725XB", 11.0);
    Control.addVariable("F725YB", -14.0);
    Control.addVariable("F725ZB", 14.0);
    Control.addVariable("F725XC", 11.0);
    Control.addVariable("F725YC", -8.0);
    Control.addVariable("F725ZC", 14.0);
    Control.addVariable("F725ZG", 11.0);
    Control.addVariable("F725X", -999.0);
    Control.addVariable("F725Y", -18.0);
    Control.addVariable("F725Z", 13.0);


    Control.addVariable("F735Length", 440.0);
    Control.addVariable("F735Delta", 0.0);
    Control.addVariable("F735ViewWidth", 6);
    Control.addVariable("F735LinkPoint", 0.0);
    Control.addVariable("F735XB", 11.0);
    Control.addVariable("F735YB", -14.0);
    Control.addVariable("F735ZB", 14.0);
    Control.addVariable("F735XC", 11.0);
    Control.addVariable("F735YC", -8.0);
    Control.addVariable("F735ZC", 14.0);
    Control.addVariable("F735ZG", 11.0);
    Control.addVariable("F735X", -999.0);
    Control.addVariable("F735Y", -18.0);
    Control.addVariable("F735Z", 13.0);

    Control.addVariable("F745Length", 440.0);
    Control.addVariable("F745Delta", 0.0);
    Control.addVariable("F745ViewWidth", 6);
    Control.addVariable("F745LinkPoint", 0.0);
    Control.addVariable("F745XB", 11.0);
    Control.addVariable("F745YB", -14.0);
    Control.addVariable("F745ZB", 14.0);
    Control.addVariable("F745XC", 11.0);
    Control.addVariable("F745YC", -8.0);
    Control.addVariable("F745ZC", 14.0);
    Control.addVariable("F745ZG", 11.0);
    Control.addVariable("F745X", -999.0);
    Control.addVariable("F745Y", -18.0);
    Control.addVariable("F745Z", 13.0);

    Control.addVariable("F755Length", 440.0);
    Control.addVariable("F755Delta", 0.0);
    Control.addVariable("F755ViewWidth", 6);
    Control.addVariable("F755LinkPoint", 0.0);
    Control.addVariable("F755XB", 11.0);
    Control.addVariable("F755YB", -14.0);
    Control.addVariable("F755ZB", 14.0);
    Control.addVariable("F755XC", 11.0);
    Control.addVariable("F755YC", -8.0);
    Control.addVariable("F755ZC", 14.0);
    Control.addVariable("F755ZG", 11.0);
    Control.addVariable("F755X", -999.0);
    Control.addVariable("F755Y", -18.0);
    Control.addVariable("F755Z", 13.0);

    Control.addVariable("F765Length", 440.0);
    Control.addVariable("F765Delta", 0.0);
    Control.addVariable("F765ViewWidth", 6);
    Control.addVariable("F765LinkPoint", 0.0);
    Control.addVariable("F765XB", 11.0);
    Control.addVariable("F765YB", -14.0);
    Control.addVariable("F765ZB", 14.0);
    Control.addVariable("F765XC", 11.0);
    Control.addVariable("F765YC", -8.0);
    Control.addVariable("F765ZC", 14.0);
    Control.addVariable("F765ZG", 11.0);
    Control.addVariable("F765X", -999.0);
    Control.addVariable("F765Y", -18.0);
    Control.addVariable("F765Z", 13.0);

    Control.addVariable("F775Length", 440.0);
    Control.addVariable("F775Delta", 0.0);
    Control.addVariable("F775ViewWidth", 6);
    Control.addVariable("F775LinkPoint", 0.0);
    Control.addVariable("F775XB", 11.0);
    Control.addVariable("F775YB", -14.0);
    Control.addVariable("F775ZB", 14.0);
    Control.addVariable("F775XC", 11.0);
    Control.addVariable("F775YC", -8.0);
    Control.addVariable("F775ZC", 14.0);
    Control.addVariable("F775ZG", 11.0);
    Control.addVariable("F775X", -999.0);
    Control.addVariable("F775Y", -18.0);
    Control.addVariable("F775Z", 13.0);

    Control.addVariable("F785Length", 440.0);
    Control.addVariable("F785Delta", 0.0);
    Control.addVariable("F785ViewWidth", 6);
    Control.addVariable("F785LinkPoint", 0.0);
    Control.addVariable("F785XB", 11.0);
    Control.addVariable("F785YB", -14.0);
    Control.addVariable("F785ZB", 14.0);
    Control.addVariable("F785XC", 11.0);
    Control.addVariable("F785YC", -8.0);
    Control.addVariable("F785ZC", 14.0);
    Control.addVariable("F785ZG", 11.0);
    Control.addVariable("F785X", -999.0);
    Control.addVariable("F785Y", -18.0);
    Control.addVariable("F785Z", 13.0);

    Control.addVariable("F795Length", 440.0);
    Control.addVariable("F795Delta", 0.0);
    Control.addVariable("F795ViewWidth", 6);
    Control.addVariable("F795LinkPoint", 0.0);
    Control.addVariable("F795XB", 11.0);
    Control.addVariable("F795YB", -14.0);
    Control.addVariable("F795ZB", 14.0);
    Control.addVariable("F795XC", 11.0);
    Control.addVariable("F795YC", -8.0);
    Control.addVariable("F795ZC", 14.0);
    Control.addVariable("F795ZG", 11.0);
    Control.addVariable("F795X", -999.0);
    Control.addVariable("F795Y", -18.0);
    Control.addVariable("F795Z", 13.0);

    Control.addVariable("F805Length", 440.0);
    Control.addVariable("F805Delta", 0.0);
    Control.addVariable("F805ViewWidth", 6);
    Control.addVariable("F805LinkPoint", 0.0);
    Control.addVariable("F805XB", 11.0);
    Control.addVariable("F805YB", -14.0);
    Control.addVariable("F805ZB", 14.0);
    Control.addVariable("F805XC", 11.0);
    Control.addVariable("F805YC", -8.0);
    Control.addVariable("F805ZC", 14.0);
    Control.addVariable("F805ZG", 11.0);
    Control.addVariable("F805X", 999.0);
    Control.addVariable("F805Y", -18.0);
    Control.addVariable("F805Z", 13.0);

    Control.addVariable("F815Length", 440.0);
    Control.addVariable("F815Delta", 0.0);
    Control.addVariable("F815ViewWidth", 6);
    Control.addVariable("F815LinkPoint", 0.0);
    Control.addVariable("F815XB", 11.0);
    Control.addVariable("F815YB", -14.0);
    Control.addVariable("F815ZB", 14.0);
    Control.addVariable("F815XC", 11.0);
    Control.addVariable("F815YC", -8.0);
    Control.addVariable("F815ZC", 14.0);
    Control.addVariable("F815ZG", 11.0);
    Control.addVariable("F815X", -999.0);
    Control.addVariable("F815Y", -18.0);
    Control.addVariable("F815Z", 13.0);

    Control.addVariable("F825Length", 440.0);
    Control.addVariable("F825Delta", 0.0);
    Control.addVariable("F825ViewWidth", 6);
    Control.addVariable("F825LinkPoint", 0.0);
    Control.addVariable("F825XB", 11.0);
    Control.addVariable("F825YB", -14.0);
    Control.addVariable("F825ZB", 14.0);
    Control.addVariable("F825XC", 11.0);
    Control.addVariable("F825YC", -8.0);
    Control.addVariable("F825ZC", 14.0);
    Control.addVariable("F825ZG", 11.0);
    Control.addVariable("F825X", -999.0);
    Control.addVariable("F825Y", -18.0);
    Control.addVariable("F825Z", 13.0);


    Control.addVariable("F835Length", 440.0);
    Control.addVariable("F835Delta", 0.0);
    Control.addVariable("F835ViewWidth", 6);
    Control.addVariable("F835LinkPoint", 0.0);
    Control.addVariable("F835XB", 11.0);
    Control.addVariable("F835YB", -14.0);
    Control.addVariable("F835ZB", 14.0);
    Control.addVariable("F835XC", 11.0);
    Control.addVariable("F835YC", -8.0);
    Control.addVariable("F835ZC", 14.0);
    Control.addVariable("F835ZG", 11.0);
    Control.addVariable("F835X", -999.0);
    Control.addVariable("F835Y", -18.0);
    Control.addVariable("F835Z", 13.0);

    Control.addVariable("F845Length", 440.0);
    Control.addVariable("F845Delta", 0.0);
    Control.addVariable("F845ViewWidth", 6);
    Control.addVariable("F845LinkPoint", 0.0);
    Control.addVariable("F845XB", 11.0);
    Control.addVariable("F845YB", -14.0);
    Control.addVariable("F845ZB", 14.0);
    Control.addVariable("F845XC", 11.0);
    Control.addVariable("F845YC", -8.0);
    Control.addVariable("F845ZC", 14.0);
    Control.addVariable("F845ZG", 11.0);
    Control.addVariable("F845X", -999.0);
    Control.addVariable("F845Y", -18.0);
    Control.addVariable("F845Z", 13.0);

    Control.addVariable("F855Length", 440.0);
    Control.addVariable("F855Delta", 0.0);
    Control.addVariable("F855ViewWidth", 6);
    Control.addVariable("F855LinkPoint", 0.0);
    Control.addVariable("F855XB", 11.0);
    Control.addVariable("F855YB", -14.0);
    Control.addVariable("F855ZB", 14.0);
    Control.addVariable("F855XC", 11.0);
    Control.addVariable("F855YC", -8.0);
    Control.addVariable("F855ZC", 14.0);
    Control.addVariable("F855ZG", 11.0);
    Control.addVariable("F855X", -999.0);
    Control.addVariable("F855Y", -18.0);
    Control.addVariable("F855Z", 13.0);

    Control.addVariable("F865Length", 440.0);
    Control.addVariable("F865Delta", 0.0);
    Control.addVariable("F865ViewWidth", 6);
    Control.addVariable("F865LinkPoint", 0.0);
    Control.addVariable("F865XB", 11.0);
    Control.addVariable("F865YB", -14.0);
    Control.addVariable("F865ZB", 14.0);
    Control.addVariable("F865XC", 11.0);
    Control.addVariable("F865YC", -8.0);
    Control.addVariable("F865ZC", 14.0);
    Control.addVariable("F865ZG", 11.0);
    Control.addVariable("F865X", -999.0);
    Control.addVariable("F865Y", -18.0);
    Control.addVariable("F865Z", 13.0);

    Control.addVariable("F875Length", 440.0);
    Control.addVariable("F875Delta", 0.0);
    Control.addVariable("F875ViewWidth", 6);
    Control.addVariable("F875LinkPoint", 0.0);
    Control.addVariable("F875XB", 11.0);
    Control.addVariable("F875YB", -14.0);
    Control.addVariable("F875ZB", 14.0);
    Control.addVariable("F875XC", 11.0);
    Control.addVariable("F875YC", -8.0);
    Control.addVariable("F875ZC", 14.0);
    Control.addVariable("F875ZG", 11.0);
    Control.addVariable("F875X", -999.0);
    Control.addVariable("F875Y", -18.0);
    Control.addVariable("F875Z", 13.0);

    Control.addVariable("F885Length", 440.0);
    Control.addVariable("F885Delta", 0.0);
    Control.addVariable("F885ViewWidth", 6);
    Control.addVariable("F885LinkPoint", 0.0);
    Control.addVariable("F885XB", 11.0);
    Control.addVariable("F885YB", -14.0);
    Control.addVariable("F885ZB", 14.0);
    Control.addVariable("F885XC", 11.0);
    Control.addVariable("F885YC", -8.0);
    Control.addVariable("F885ZC", 14.0);
    Control.addVariable("F885ZG", 11.0);
    Control.addVariable("F885X", -999.0);
    Control.addVariable("F885Y", -18.0);
    Control.addVariable("F885Z", 13.0);

    Control.addVariable("F895Length", 440.0);
    Control.addVariable("F895Delta", 0.0);
    Control.addVariable("F895ViewWidth", 6);
    Control.addVariable("F895LinkPoint", 0.0);
    Control.addVariable("F895XB", 11.0);
    Control.addVariable("F895YB", -14.0);
    Control.addVariable("F895ZB", 14.0);
    Control.addVariable("F895XC", 11.0);
    Control.addVariable("F895YC", -8.0);
    Control.addVariable("F895ZC", 14.0);
    Control.addVariable("F895ZG", 11.0);
    Control.addVariable("F895X", -999.0);
    Control.addVariable("F895Y", -18.0);
    Control.addVariable("F895Z", 13.0);

    Control.addVariable("F905Length", 440.0);
    Control.addVariable("F905Delta", 0.0);
    Control.addVariable("F905ViewWidth", 6);
    Control.addVariable("F905LinkPoint", 0.0);
    Control.addVariable("F905XB", 11.0);
    Control.addVariable("F905YB", -14.0);
    Control.addVariable("F905ZB", 14.0);
    Control.addVariable("F905XC", 11.0);
    Control.addVariable("F905YC", -8.0);
    Control.addVariable("F905ZC", 14.0);
    Control.addVariable("F905ZG", 11.0);
    Control.addVariable("F905X", 999.0);
    Control.addVariable("F905Y", -18.0);
    Control.addVariable("F905Z", 13.0);

    Control.addVariable("F915Length", 440.0);
    Control.addVariable("F915Delta", 0.0);
    Control.addVariable("F915ViewWidth", 6);
    Control.addVariable("F915LinkPoint", 0.0);
    Control.addVariable("F915XB", 11.0);
    Control.addVariable("F915YB", -14.0);
    Control.addVariable("F915ZB", 14.0);
    Control.addVariable("F915XC", 11.0);
    Control.addVariable("F915YC", -8.0);
    Control.addVariable("F915ZC", 14.0);
    Control.addVariable("F915ZG", 11.0);
    Control.addVariable("F915X", -999.0);
    Control.addVariable("F915Y", -18.0);
    Control.addVariable("F915Z", 13.0);

    Control.addVariable("F925Length", 440.0);
    Control.addVariable("F925Delta", 0.0);
    Control.addVariable("F925ViewWidth", 6);
    Control.addVariable("F925LinkPoint", 0.0);
    Control.addVariable("F925XB", 11.0);
    Control.addVariable("F925YB", -14.0);
    Control.addVariable("F925ZB", 14.0);
    Control.addVariable("F925XC", 11.0);
    Control.addVariable("F925YC", -8.0);
    Control.addVariable("F925ZC", 14.0);
    Control.addVariable("F925ZG", 11.0);
    Control.addVariable("F925X", -999.0);
    Control.addVariable("F925Y", -18.0);
    Control.addVariable("F925Z", 13.0);


    Control.addVariable("F935Length", 440.0);
    Control.addVariable("F935Delta", 0.0);
    Control.addVariable("F935ViewWidth", 6);
    Control.addVariable("F935LinkPoint", 0.0);
    Control.addVariable("F935XB", 11.0);
    Control.addVariable("F935YB", -14.0);
    Control.addVariable("F935ZB", 14.0);
    Control.addVariable("F935XC", 11.0);
    Control.addVariable("F935YC", -8.0);
    Control.addVariable("F935ZC", 14.0);
    Control.addVariable("F935ZG", 11.0);
    Control.addVariable("F935X", -999.0);
    Control.addVariable("F935Y", -18.0);
    Control.addVariable("F935Z", 13.0);

    Control.addVariable("F945Length", 440.0);
    Control.addVariable("F945Delta", 0.0);
    Control.addVariable("F945ViewWidth", 6);
    Control.addVariable("F945LinkPoint", 0.0);
    Control.addVariable("F945XB", 11.0);
    Control.addVariable("F945YB", -14.0);
    Control.addVariable("F945ZB", 14.0);
    Control.addVariable("F945XC", 11.0);
    Control.addVariable("F945YC", -8.0);
    Control.addVariable("F945ZC", 14.0);
    Control.addVariable("F945ZG", 11.0);
    Control.addVariable("F945X", -999.0);
    Control.addVariable("F945Y", -18.0);
    Control.addVariable("F945Z", 13.0);

    Control.addVariable("F955Length", 440.0);
    Control.addVariable("F955Delta", 0.0);
    Control.addVariable("F955ViewWidth", 6);
    Control.addVariable("F955LinkPoint", 0.0);
    Control.addVariable("F955XB", 11.0);
    Control.addVariable("F955YB", -14.0);
    Control.addVariable("F955ZB", 14.0);
    Control.addVariable("F955XC", 11.0);
    Control.addVariable("F955YC", -8.0);
    Control.addVariable("F955ZC", 14.0);
    Control.addVariable("F955ZG", 11.0);
    Control.addVariable("F955X", -999.0);
    Control.addVariable("F955Y", -18.0);
    Control.addVariable("F955Z", 13.0);

    Control.addVariable("F965Length", 440.0);
    Control.addVariable("F965Delta", 0.0);
    Control.addVariable("F965ViewWidth", 6);
    Control.addVariable("F965LinkPoint", 0.0);
    Control.addVariable("F965XB", 11.0);
    Control.addVariable("F965YB", -14.0);
    Control.addVariable("F965ZB", 14.0);
    Control.addVariable("F965XC", 11.0);
    Control.addVariable("F965YC", -8.0);
    Control.addVariable("F965ZC", 14.0);
    Control.addVariable("F965ZG", 11.0);
    Control.addVariable("F965X", -999.0);
    Control.addVariable("F965Y", -18.0);
    Control.addVariable("F965Z", 13.0);

    Control.addVariable("F975Length", 440.0);
    Control.addVariable("F975Delta", 0.0);
    Control.addVariable("F975ViewWidth", 6);
    Control.addVariable("F975LinkPoint", 0.0);
    Control.addVariable("F975XB", 11.0);
    Control.addVariable("F975YB", -14.0);
    Control.addVariable("F975ZB", 14.0);
    Control.addVariable("F975XC", 11.0);
    Control.addVariable("F975YC", -8.0);
    Control.addVariable("F975ZC", 14.0);
    Control.addVariable("F975ZG", 11.0);
    Control.addVariable("F975X", -999.0);
    Control.addVariable("F975Y", -18.0);
    Control.addVariable("F975Z", 13.0);

    Control.addVariable("F985Length", 440.0);
    Control.addVariable("F985Delta", 0.0);
    Control.addVariable("F985ViewWidth", 6);
    Control.addVariable("F985LinkPoint", 0.0);
    Control.addVariable("F985XB", 11.0);
    Control.addVariable("F985YB", -14.0);
    Control.addVariable("F985ZB", 14.0);
    Control.addVariable("F985XC", 11.0);
    Control.addVariable("F985YC", -8.0);
    Control.addVariable("F985ZC", 14.0);
    Control.addVariable("F985ZG", 11.0);
    Control.addVariable("F985X", -999.0);
    Control.addVariable("F985Y", -18.0);
    Control.addVariable("F985Z", 13.0);

    Control.addVariable("F995Length", 440.0);
    Control.addVariable("F995Delta", 0.0);
    Control.addVariable("F995ViewWidth", 6);
    Control.addVariable("F995LinkPoint", 0.0);
    Control.addVariable("F995XB", 11.0);
    Control.addVariable("F995YB", -14.0);
    Control.addVariable("F995ZB", 14.0);
    Control.addVariable("F995XC", 11.0);
    Control.addVariable("F995YC", -8.0);
    Control.addVariable("F995ZC", 14.0);
    Control.addVariable("F995ZG", 11.0);
    Control.addVariable("F995X", -999.0);
    Control.addVariable("F995Y", -18.0);
    Control.addVariable("F995Z", 13.0);

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
