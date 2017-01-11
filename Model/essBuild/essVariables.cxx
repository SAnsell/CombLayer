/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/essVariables.cxx
 *
 * Copyright (c) 2004-2016 by Stuart Ansell/Konstantin Batkov
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
#include "Exception.h"
#include "essVariables.h"

namespace setVariable
{

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
  ELog::RegMethod RegA("setVariable","EssVariables");

  Control.addVariable("zero",0.0);     // Zero
  Control.addVariable("one",1.0);      // one

  Control.addVariable("EngineeringActive",0);      // NO engineering

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

  Control.addVariable("BeRefXStep",0.0);  
  Control.addVariable("BeRefYStep",0.0);  
  Control.addVariable("BeRefZStep",0.0);
  Control.addVariable("BeRefXYangle",0.0); 
  Control.addVariable("BeRefZangle",0.0);
  Control.addVariable("BeRefRadius",35);
  Control.addVariable("BeRefHeight",37.3); // 74.6/2.0
  Control.addVariable("BeRefDepth",37.3); // 74.6/2.0
  Control.addVariable("BeRefWallThick",3);
  Control.addVariable("BeRefWallThickLow",0.0);
  Control.addVariable("BeRefTargetSepThick",13.0);
  Control.addVariable("BeRefLowVoidThick",2.3);
  Control.addVariable("BeRefTopVoidThick",2.3);
  Control.addVariable("BeRefLowRefMat","Be5H2O");
  Control.addVariable("BeRefTopRefMat","Be5H2O");
  Control.addVariable("BeRefTopWallMat","SS316L");
  Control.addVariable("BeRefLowWallMat","SS316L");
  Control.addVariable("BeRefTargSepMat","Iron_10H2O");

  Control.addVariable("BeRefTopInnerStructureNLayers", 6);
  Control.addVariable("BeRefTopInnerStructureBaseLen1", 0.258);
  Control.addVariable("BeRefTopInnerStructureBaseLen2", 0.075);
  Control.addVariable("BeRefTopInnerStructureBaseLen3", 0.258);
  Control.addVariable("BeRefTopInnerStructureBaseLen4", 0.075);
  Control.addVariable("BeRefTopInnerStructureBaseLen5", 0.258);
  Control.addVariable("BeRefTopInnerStructureMat0", "Beryllium");
  Control.addVariable("BeRefTopInnerStructureMat1", "Be30H2O");
  Control.addVariable("BeRefTopInnerStructureMat2", "Beryllium");
  Control.addVariable("BeRefTopInnerStructureMat3", "Be30H2O");
  Control.addVariable("BeRefTopInnerStructureMat4", "Beryllium");
  Control.addVariable("BeRefTopInnerStructureMat5", "Be30H2O");

  Control.addVariable("BeRefLowInnerStructureNLayers", 6);
  Control.addVariable("BeRefLowInnerStructureBaseLen1", 0.258);
  Control.addVariable("BeRefLowInnerStructureBaseLen2", 0.075);
  Control.addVariable("BeRefLowInnerStructureBaseLen3", 0.258);
  Control.addVariable("BeRefLowInnerStructureBaseLen4", 0.075);
  Control.addVariable("BeRefLowInnerStructureBaseLen5", 0.258);
  Control.addVariable("BeRefLowInnerStructureMat0", "Beryllium");
  Control.addVariable("BeRefLowInnerStructureMat1", "Be30H2O");
  Control.addVariable("BeRefLowInnerStructureMat2", "Beryllium");
  Control.addVariable("BeRefLowInnerStructureMat3", "Be30H2O");
  Control.addVariable("BeRefLowInnerStructureMat4", "Beryllium");
  Control.addVariable("BeRefLowInnerStructureMat5", "Be30H2O");

  Control.addVariable("BulkXStep",0.0);
  Control.addVariable("BulkYStep",0.0);
  Control.addVariable("BulkZStep",0.0);
  Control.addVariable("BulkXYangle",0.0);
  Control.addVariable("BulkZangle",0.0);
  Control.addVariable("BulkNLayer",3);

  Control.addParse<double>("BulkRadius1","BeRefRadius+BeRefWallThick+0.2");
  /*!
    \todo : This is ugly conterintuative
    and going to break if anyone make a change
  */

  /*  Control.Parse("BeRefHeight/2.0+BeRefWallThick+"
                "TopBeRefWaterDiscHeight0+TopBeRefWaterDiscDepth0+"
		"TopBeRefWaterDiscHeight1+0.2");  */
  Control.addVariable("BulkHeight1", 41.5);
  /*  Control.Parse("BeRefHeight/2.0+BeRefWallThick+LowBeRefWaterDiscHeight0"
      "+LowBeRefWaterDiscDepth0+LowBeRefWaterDiscHeight1+0.2");*/
  Control.addVariable("BulkDepth1", 41.5);
  Control.addVariable("BulkMat1","Void");

  Control.addVariable("BulkRadius2",65.0);
  Control.addVariable("BulkHeight2",75.0);
  Control.addVariable("BulkDepth2",75.0);
  Control.addVariable("BulkMat2","Iron_10H2O");        // SA: using hand-made mixture because CL can't generate volume fractions

  // Bulk steel layer [No individual guides]
  Control.addVariable("BulkRadius3",200.0);
  Control.addVariable("BulkHeight3",200.0);
  Control.addVariable("BulkDepth3",200.0);
  Control.addVariable("BulkMat3","Iron_10H2O");        // SA: using hand-made mixture because CL can't generate volume fractions

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
  Control.addVariable("ShutterBaySkin",6.0);
  Control.addVariable("ShutterBayTopSkin",6.0);
  Control.addVariable("ShutterBayMat","CastIron");
  Control.addVariable("ShutterBaySkinMat","Void");

  // Guide BAY [ All 4 same ]
  Control.addVariable("GuideBayXStep",0.0);  
  Control.addVariable("GuideBayYStep",0.0);  
  Control.addVariable("GuideBayZStep",0.0);
  Control.addVariable("GuideBayZAngle",0.0);
  Control.addVariable("GuideBayViewAngle",128.0); 
  Control.addVariable("GuideBayInnerHeight",20.0);
  Control.addVariable("GuideBayInnerDepth",10.3);
  Control.addVariable("GuideBayMidRadius",170.0);
  Control.addVariable("GuideBayHeight",32.0);
  Control.addVariable("GuideBayDepth",40.0);
  Control.addVariable("GuideBayMat","CastIron");
  Control.addVariable("GuideBay1XYAngle",0.0); 
  Control.addVariable("GuideBay2XYAngle",180.0); 
  Control.addVariable("GuideBay3XYAngle",0.0); 
  Control.addVariable("GuideBay4XYAngle",180.0); 
  Control.addVariable("GuideBay1NItems",21);  
  Control.addVariable("GuideBay2NItems",21);  
  Control.addVariable("GuideBay3NItems",21);  
  Control.addVariable("GuideBay4NItems",21);

  Control.addVariable("GuideBay4InnerDepth",5.8);

  // Twister
  Control.addVariable("TwisterXStep",11.0);
  Control.addVariable("TwisterYStep",-62.0);
  Control.addVariable("TwisterZStep",0.0);
  Control.addVariable("TwisterXYAngle",10.0);
  Control.addVariable("TwisterZangle",0.0);
  Control.addVariable("TwisterShaftRadius",18.5);
  Control.addVariable("TwisterShaftHeight",120.0+222.4);
  Control.addVariable("TwisterShaftMat","Iron_10H2O");
  Control.addVariable("TwisterShaftWallThick",3.0);
  Control.addVariable("TwisterShaftWallMat","Iron_10H2O"); 
  Control.addVariable("TwisterShaftBearingRadius",4);
  Control.addVariable("TwisterShaftBearingHeight",54.4);
  Control.addVariable("TwisterShaftBearingWallThick",12.2);
  Control.addVariable("TwisterPlugFrameRadius",105.0);
  Control.addVariable("TwisterPlugFrameWallThick",3.0);
  Control.addVariable("TwisterPlugFrameHeight",57.6);
  Control.addVariable("TwisterPlugFrameDepth",60.6);
  Control.addVariable("TwisterPlugFrameAngle",38.0);
  Control.addVariable("TwisterPlugFrameMat","Iron_10H2O");
  Control.addVariable("TwisterPlugFrameWallThick",3.0);
  Control.addVariable("TwisterPlugFrameWallMat","Iron_10H2O");

  // Target Station main building
  Control.addVariable("TSMainBuildingXStep", 0.0);
  Control.addVariable("TSMainBuildingYStep", 0.0);
  Control.addVariable("TSMainBuildingZStep", 0.0);
  Control.addVariable("TSMainBuildingXYangle", 0.0);
  Control.addVariable("TSMainBuildingZangle", 0.0);
  Control.addVariable("TSMainBuildingLength", 2200*2.0);
  Control.addVariable("TSMainBuildingWidth",  1090*2.0);
  Control.addVariable("TSMainBuildingHeight", 1000.0);
  Control.addVariable("TSMainBuildingDepth",  750.0);
  Control.addVariable("TSMainBuildingMainMat",  "Concrete");
  Control.addVariable("TSMainBuildingRoomMat",  "Void");

  
  EssProtonBeam(Control);
  EssBeamLinesVariables(Control);
  EssPipeVariables(Control);
  
  BEERvariables(Control);
  BIFROSTvariables(Control);
  CSPECvariables(Control);
  DREAMvariables(Control);
  ESTIAvariables(Control);
  FREIAvariables(Control);
  LOKIvariables(Control);
  MAGICvariables(Control);
  MIRACLESvariables(Control);
  NMXvariables(Control);
  ODINvariables(Control);
  TREXvariables(Control);
  VESPAvariables(Control);
  VORvariables(Control);
  simpleITEMvariables(Control);

  shortDREAMvariables(Control);
  shortDREAM2variables(Control);
  shortNMXvariables(Control);
  shortODINvariables(Control);
  
  EssButterflyModerator(Control);
  EssWheel(Control);
  EssBunkerVariables(Control);
  EssIradVariables(Control);
  EssFlightLineVariables(Control);
  F5Variables(Control);

  Control.addVariable("sdefEnergy",2000.0);
  Control.addVariable("sdefWidth",    7.0);
  Control.addVariable("sdefHeight",   1.6);
  Control.addVariable("sdefYPos",    -500);
  Control.addVariable("sdefPDF","uniform");
  
  // port version:
  Control.addVariable("portSourceASpread",0.0);
  Control.addVariable("portSourceHeight",3.0);
  Control.addVariable("portSourceEStart",3.0);
  Control.addVariable("portSourceEEnd",4.0);
  Control.addVariable("portSourceNE",2);


  // FINAL:
  Control.resetActive();
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
      Control.addVariable(baseKey+"Mat","Stainless304");
      Control.addVariable(baseKey+"BeamXYAngle",0.0); 
      Control.addVariable(baseKey+"BeamZAngle",0.0);
      Control.addVariable(baseKey+"BeamXStep",0.0);
      Control.addVariable(baseKey+"BeamZStep",0.0);  
      Control.addVariable(baseKey+"BeamHeight",6.0);
      Control.addVariable(baseKey+"BeamWidth",6.0);
      Control.addVariable(baseKey+"NSegment",2);
      Control.addVariable(baseKey+"SideGap",0.6);
      Control.addVariable(baseKey+"BaseGap",0.1);
      Control.addVariable(baseKey+"TopGap",0.8); 
      Control.addVariable(baseKey+"Width1",16.0);
      if (i==3)
	Control.addVariable(baseKey+"Depth1",5.5);
      else 
	Control.addVariable(baseKey+"Depth1",5.0);

      Control.addVariable(baseKey+"Height1",12.0);
      Control.addVariable(baseKey+"Width2",28.0);
      Control.addVariable(baseKey+"Height2",22.0);
      Control.addVariable(baseKey+"Depth2",22.0);
      Control.addVariable(baseKey+"Length1",170.0);
      Control.addVariable(baseKey+"1XYangle",90.0-30.0);   // N1
      Control.addVariable(baseKey+"2XYangle",90.0-35.3); 
      Control.addVariable(baseKey+"3XYangle",90.0-42.0); 
      Control.addVariable(baseKey+"4XYangle",90.0-47.3); 
      Control.addVariable(baseKey+"5XYangle",90.0-54.0); 
      Control.addVariable(baseKey+"6XYangle",90.0-59.3); 
      Control.addVariable(baseKey+"7XYangle",90.0-66.0); 
      Control.addVariable(baseKey+"8XYangle",90.0-71.3); 
      Control.addVariable(baseKey+"9XYangle",90.0-78.0);
      Control.addVariable(baseKey+"10XYangle",90.0-83.3); 
      Control.addVariable(baseKey+"11XYangle",90.0-90.0);

      Control.addVariable(baseKey+"12XYangle",-90.0+84.0);
      Control.addVariable(baseKey+"13XYangle",-90.0+78.0);
      Control.addVariable(baseKey+"14XYangle",-90.0+72.0);
      Control.addVariable(baseKey+"15XYangle",-90.0+66.0);
      Control.addVariable(baseKey+"16XYangle",-90.0+60.0);
      Control.addVariable(baseKey+"17XYangle",-90.0+54.0);
      Control.addVariable(baseKey+"18XYangle",-90.0+48.0);
      Control.addVariable(baseKey+"19XYangle",-90.0+42.0);
      Control.addVariable(baseKey+"20XYangle",-90.0+36.0);
      Control.addVariable(baseKey+"21XYangle",-90.0+30.0);  // W1
      Control.addVariable(baseKey+"Filled",0);
      Control.addVariable(baseKey+"Active",0);


    }
  return;
}

void
EssFlightLineVariables(FuncDataBase& Control)
  /*!
    Set the flightline variables
    \param Control :: Database for variables
   */
{
  ELog::RegMethod RegA("essVariables[F]","EssFlightLineVariables");

  // upper flight lines
  
  Control.addVariable("TopAFlightTapSurf", "cone");
  Control.addVariable("TopAFlightXStep", 0.0);      // Step from centre
  Control.addVariable("TopAFlightZStep", 0.0);      // Step from centre

  Control.addVariable("TopAFlightAngleXY1",60.0);  // Angle out
  Control.addVariable("TopAFlightAngleXY2",60.0);  // Angle out
  Control.addVariable("TopAFlightXYangle",180.0);
  Control.addVariable("TopAFlightZangle",0.0);
  Control.addVariable("TopAFlightAngleZTop",1.1);  // Step down angle !!! 1.455 is too much - fight line cuts the Bilbao target wheel ESS-0032315.3
  Control.addVariable("TopAFlightAngleZBase",1.33); // Step up angle ESS-0032315.3

  //  Control.Parse("TopFlyTotalHeight");
  Control.addVariable("TopAFlightHeight", 2.9);     // Full height = TopFlyTotalHeight
  Control.addVariable("TopAFlightWidth", 10.7);     // Full width
  Control.addVariable("TopAFlightNLiner", 1);      // Liner
  Control.addVariable("TopAFlightLinerThick1", 0.3);      // Liner
  Control.addVariable("TopAFlightLinerMat1","Aluminium");      // Liner

  // The moderator focal points are (89,54)
  // The wedge focus at (80,48). This is a result of the line of sight from
  // the neutron beam channels towards the moderator
  const double wedgeFocusX = 8.0; // email from Rickard Holmberg 15 Sep 2016
  const double wedgeFocusY = 4.8; // email from Rickard Holmberg 15 Sep 2016
  
  const int TopAFlightNWedges = 14;
  Control.addVariable("TopAFlightNWedges",TopAFlightNWedges);

  const double t1 = 3.5+11.95*3; // Rickard Holmberg: slides 9-10
  const double dt1 = 11.95; // Rickard Holmberg ESS-0037906, ESS-0038057 + slide 7
  const double t2 = -9.1; // Rickard Holmberg slide 12
  const double dt2 = 5.975; // Rickard Holmberg ESS-0037906, ESS-0038057
  
  std::vector<double> TopAFlightWedgeTheta;
  TopAFlightWedgeTheta.push_back(t1);
  for (size_t i=1; i<=3; i++)
    TopAFlightWedgeTheta.push_back(t1-dt1*i);

  // central wedge: Rickard Holmberg slide 14
  TopAFlightWedgeTheta.push_back(-2.8);
  TopAFlightWedgeTheta.push_back(t2);
  for (size_t i=1; i<=8; i++)
    TopAFlightWedgeTheta.push_back(t2-dt2*i);

  double xstep(0);
  double ystep(0);

  const double defLength = 30.0; // default length ESS-003805
  const double defWidth  = 5.7; // default base width ESS-003805
  double length(defLength), width(defWidth);
  
  for (size_t i=1; i<=TopAFlightNWedges; i++)
    {
      const std::string baseKey = StrFunc::makeString("TopAFlightWedge", i);

      if (i==1)
	{
	  length = 25.0; // shorter due to interference with the wheel
	  width = 5.8;
	}
      else
	{
	  length = defLength;
	  width = defWidth;
	}

      if (i==5) // central, the thick one
	{
	  Control.addVariable(baseKey+"BaseWidth", 12.0+2*length*std::tan(4*M_PI/180));  // Rickard Holmberg slide 14

	  Control.addVariable(baseKey+"TipAngle",  8.0); // Rickard Holmberg slide 14

	  Control.addVariable(baseKey+"XStep", 0.0);
	  Control.addVariable(baseKey+"YStep", 0.0);
	}
      else
	{
	  Control.addVariable(baseKey+"BaseWidth", width); // ESS-0038057

	  xstep = wedgeFocusX;
	  ystep = (i<=4) ? wedgeFocusY : -wedgeFocusY;
	  Control.addVariable(baseKey+"XStep", xstep);
	  Control.addVariable(baseKey+"YStep", ystep);
	}
      Control.addVariable(baseKey+"ZStep", 13.7);

      Control.addVariable(baseKey+"TipAngle", 6.0); // ESS-0038057, ESS-003805
      Control.addVariable(baseKey+"Theta", TopAFlightWedgeTheta[i-1]);

      Control.addVariable(baseKey+"Length",length); // ESS-0038057
      Control.addVariable(baseKey+"Mat","SS316L");
    }

  
  Control.addVariable("TopBFlightTapSurf", "cone");
  Control.addVariable("TopBFlightXStep", 0.0);      // Step from centre
  Control.addVariable("TopBFlightZStep", 0.0);      // Step from centre
  Control.addVariable("TopBFlightAngleXY1", 60.0);  // Angle out
  Control.addVariable("TopBFlightAngleXY2", 60.0);  // Angle out
  Control.addVariable("TopBFlightXYangle", 0.0);
  Control.addVariable("TopBFlightZangle", 0.0);
  Control.Parse("TopAFlightAngleZTop");
  Control.addVariable("TopBFlightAngleZTop");  //   // Step down angle
  Control.Parse("TopAFlightAngleZBase");
  Control.addVariable("TopBFlightAngleZBase"); // Step up angle
  Control.Parse("TopAFlightHeight");
  Control.addVariable("TopBFlightHeight");     // Full height = TopFlyTotalHeight
  Control.Parse("TopAFlightWidth");
  Control.addVariable("TopBFlightWidth");     // Full width
  Control.addVariable("TopBFlightNLiner", 1);     
  Control.Parse("TopAFlightLinerThick1"); 
  Control.addVariable("TopBFlightLinerThick1"); 
  Control.addVariable("TopBFlightLinerMat1","Aluminium");

  // CHANGED FROM 12
  const int TopBFlightNWedges = 2;
  Control.addVariable("TopBFlightNWedges",TopBFlightNWedges);
  std::vector<double> TopBFlightWedgeTheta;

  const double t3 = (9.1+dt2*8)-180; // email from Rickard Holmberg 15 Sep, slide 5
  TopBFlightWedgeTheta.push_back(t3);
  for (size_t i=1; i<=8; i++)
    TopBFlightWedgeTheta.push_back(t3-dt2*i);
  const double t4 = -15.45-180; // email from Rickard Holmberg 15 Sep, slide 6
  TopBFlightWedgeTheta.push_back(t4);
  for (size_t i=1; i<=2; i++)
    TopBFlightWedgeTheta.push_back(t4-dt1*i);

  if (TopBFlightNWedges > TopBFlightWedgeTheta.size())
    throw ColErr::RangeError<int>(TopBFlightNWedges,0,
				  static_cast<int>(TopBFlightWedgeTheta.size()),
				  "NWedges should not exceed size of TopBFlightWedgeTheta");
  
  for (size_t i=1; i<=TopBFlightNWedges; i++)
    {
      const std::string baseKey = StrFunc::makeString("TopBFlightWedge", i);

      if (i==12)
	{
	  length = 25.0; // shorter due to interference with the wheel
	  width = 5.8;
	}
      else
	{
	  length = defLength;
	  width = defWidth;
	}

      Control.addVariable(baseKey+"BaseWidth", width);
      Control.addVariable(baseKey+"TipAngle",  6.0); // ESS-003805

      const double xstep = -wedgeFocusX;
      const double ystep = (i<=9) ? -wedgeFocusY : wedgeFocusY;
      Control.addVariable(baseKey+"XStep", xstep);
      Control.addVariable(baseKey+"YStep", ystep);
      Control.addVariable(baseKey+"ZStep", 13.7);

      Control.addVariable(baseKey+"Theta", TopBFlightWedgeTheta[i-1]);

      Control.addVariable(baseKey+"Length", length);
      Control.addVariable(baseKey+"Mat","SS316L");
    }


  // lower flight lines

  Control.addVariable("LowAFlightTapSurf", "cone");
  Control.addVariable("LowAFlightXStep",0.0); // Step from centre
  Control.addVariable("LowAFlightZStep",0.0);      // Step from centre
  Control.addVariable("LowAFlightXYangle",0.0);  // Angle out
  Control.addVariable("LowAFlightZangle",0.0);  // Angle out
  Control.addVariable("LowAFlightAngleXY1",60.0);  // Angle out
  Control.addVariable("LowAFlightAngleXY2",60.0);  // Angle out
  Control.addVariable("LowAFlightAngleZTop",0.9);  // Step down angle ESS-0032315.3
  Control.addVariable("LowAFlightAngleZBase",0.9); // Step up angle ESS-0032315.3
  Control.addVariable("LowAFlightHeight",6.1);     // Full height = LowFlyTotalHeight
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

  Control.addVariable("LowBFlightTapSurf", "cone");
  Control.addVariable("LowBFlightXStep",0.0);     // Angle
  Control.addVariable("LowBFlightZStep",0.0);      // Step from centre
  Control.addVariable("LowBFlightXYangle",180.0);  // Angle out
  Control.addVariable("LowBFlightZangle",0.0);     // Angle out
  Control.addVariable("LowBFlightAngleXY1",60.0);  // Angle out
  Control.addVariable("LowBFlightAngleXY2",60.0);  // Angle out
  Control.addVariable("LowBFlightAngleZTop",0.9);  // Step down angle
  Control.addVariable("LowBFlightAngleZBase",0.9); // Step up angle
  Control.addVariable("LowBFlightHeight",6.1);     // Full height = LowFlyTotalHeight
  Control.addVariable("LowBFlightWidth",10.7);     // Full width
  Control.addVariable("LowBFlightNLiner",1);      // Liner
  Control.addVariable("LowBFlightLinerThick1",0.4);   
  Control.addVariable("LowBFlightLinerMat1","Aluminium");      


  return;
}

}  // NAMESPACE setVariable
