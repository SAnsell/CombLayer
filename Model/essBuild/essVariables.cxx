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

  // FLIGHT LINE

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
  
  Control.addVariable("BeRefXStep",0.0);  
  Control.addVariable("BeRefYStep",0.0);  
  Control.addVariable("BeRefZStep",0.0);
  Control.addVariable("BeRefXYangle",0.0); 
  Control.addVariable("BeRefZangle",0.0);
  Control.addVariable("BeRefRadius",35);
  Control.addVariable("BeRefHeight",74.6);
  Control.addVariable("BeRefWallThick",3);
  Control.addVariable("BeRefWallThickLow",0.0);
  Control.addVariable("BeRefTargetSepThick",13.0);
  Control.addVariable("BeRefLowVoidThick",2.3);
  Control.addVariable("BeRefTopVoidThick",2.3);
  Control.addVariable("BeRefLowRefMat","Be5H2O");
  Control.addVariable("BeRefTopRefMat","Be5H2O");
  //  Control.addVariable("BeRefRefMat","Be300K");

  Control.addVariable("BeRefTopWallMat","SS316L");
  Control.addVariable("BeRefLowWallMat","SS316L");
  Control.addVariable("BeRefTargSepMat","Iron10H2O");

  ///< TODO : Fix double variable dependency !!!
  
  Control.addVariable("BeRefInnerStructureBeRadius", 12.5);
  Control.addVariable("BeRefInnerStructureBeMat", "Be10H2O");
  Control.addVariable("BeRefInnerStructureBeWallThick", 0.3);
  Control.addVariable("BeRefInnerStructureBeWallMat", "Aluminium");

  Control.addVariable("BeRefInnerStructureNLayers", 6);
  Control.addVariable("BeRefInnerStructureBaseLen1", 0.258);
  Control.addVariable("BeRefInnerStructureBaseLen2", 0.075);
  Control.addVariable("BeRefInnerStructureBaseLen3", 0.258);
  Control.addVariable("BeRefInnerStructureBaseLen4", 0.075);
  Control.addVariable("BeRefInnerStructureBaseLen5", 0.258);
  Control.addVariable("BeRefInnerStructureMat0", "Beryllium");
  Control.addVariable("BeRefInnerStructureMat1", "Be30H2O");
  Control.addVariable("BeRefInnerStructureMat2", "Beryllium");
  Control.addVariable("BeRefInnerStructureMat3", "Be30H2O");
  Control.addVariable("BeRefInnerStructureMat4", "Beryllium");
  Control.addVariable("BeRefInnerStructureMat5", "Be30H2O");

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
  Control.addVariable("GuideBayInnerHeight",20.0);
  Control.addVariable("GuideBayInnerDepth",10.3);
  Control.addVariable("GuideBayMidRadius",170.0);
  Control.addVariable("GuideBayHeight",32.0);
  Control.addVariable("GuideBayDepth",40.0);
  Control.addVariable("GuideBayMat","CastIron");
  Control.addVariable("GuideBay1XYangle",0.0); 
  Control.addVariable("GuideBay2XYangle",180.0); 
  Control.addVariable("GuideBay3XYangle",0.0); 
  Control.addVariable("GuideBay4XYangle",180.0); 
  Control.addVariable("GuideBay1NItems",21);  
  Control.addVariable("GuideBay2NItems",21);  
  Control.addVariable("GuideBay3NItems",21);  
  Control.addVariable("GuideBay4NItems",21);

  Control.addVariable("GuideBay4InnerDepth",5.8);

  // Twister
  Control.addVariable("TwisterXStep",11.0);
  Control.addVariable("TwisterYStep",-62.0);
  Control.addVariable("TwisterZStep",0.0);
  Control.addVariable("TwisterXYangle",10.0);
  Control.addVariable("TwisterZangle",0.0);
  Control.addVariable("TwisterShaftRadius",18.5);
  Control.addVariable("TwisterShaftHeight",120.0+222.4);
  Control.addVariable("TwisterShaftMat","Iron10H2O");
  Control.addVariable("TwisterShaftWallThick",3.0);
  Control.addVariable("TwisterShaftWallMat","Iron10H2O"); 
  Control.addVariable("TwisterShaftBearingRadius",4);
  Control.addVariable("TwisterShaftBearingHeight",54.4);
  Control.addVariable("TwisterShaftBearingWallThick",12.2);
  Control.addVariable("TwisterPlugFrameRadius",105.0);
  Control.addVariable("TwisterPlugFrameWallThick",3.0);
  Control.addVariable("TwisterPlugFrameHeight",57.6);
  Control.addVariable("TwisterPlugFrameDepth",60.6);
  Control.addVariable("TwisterPlugFrameAngle",38.0);
  Control.addVariable("TwisterPlugFrameMat","Iron10H2O");
  Control.addVariable("TwisterPlugFrameWallThick",3.0);
  Control.addVariable("TwisterPlugFrameWallMat","Iron10H2O");

  
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
  NMXvariables(Control);
  ODINvariables(Control);
  VESPAvariables(Control);
  VORvariables(Control);
  simpleITEMvariables(Control);

  shortDREAMvariables(Control);
  shortDREAM2variables(Control);
  shortODINvariables(Control);
  
  EssButterflyModerator(Control);
  EssWheel(Control);
  EssBunkerVariables(Control);
  EssIradVariables(Control);

  F5Variables(Control);

  Control.addVariable("sdefEnergy",2000.0);
  // port version:
  Control.addVariable("portSourceASpread",0.0);
  Control.addVariable("portSourceHeight",3.0);
  Control.addVariable("portSourceEStart",3.0);
  Control.addVariable("portSourceEEnd",4.0);
  Control.addVariable("portSourceNE",2);


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

}  // NAMESPACE setVariable
