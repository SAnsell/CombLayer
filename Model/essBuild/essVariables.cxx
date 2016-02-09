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
  Control.addVariable("LowBFlightZangle",0.0);     // Angle out
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
  
  EssProtonBeam(Control);
  EssBeamLinesVariables(Control);
  ODINvariables(Control);
  DREAMvariables(Control);
  shortDREAMvariables(Control);
  shortDREAM2variables(Control);
  shortODINvariables(Control);
  ESTIAvariables(Control);
  LOKIvariables(Control);
  NMXvariables(Control);
  VORvariables(Control);
      
  EssButterflyModerator(Control);
  EssWheel(Control);
  EssBunkerVariables(Control);

  F5Variables(Control);

  Control.addVariable("sdefEnergy",2000.0);  

  // port version:
  Control.addVariable("portSourceASpread",0.0);
  Control.addVariable("portSourceHeight",3.0);
  Control.addVariable("portSourceEStart",3.0);
  Control.addVariable("portSourceEEnd",4.0);
  Control.addVariable("portSourceNE",2);


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
      Control.addVariable(baseKey+"Mat","Stainless304");
      Control.addVariable(baseKey+"BeamXYAngle",0.0); 
      Control.addVariable(baseKey+"BeamZAngle",0.0);
      Control.addVariable(baseKey+"BeamXStep",0.0);
      Control.addVariable(baseKey+"BeamZStep",0.0);  
      Control.addVariable(baseKey+"BeamHeight",3.0);
      Control.addVariable(baseKey+"BeamWidth",7.6);
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
      Control.addVariable(baseKey+"1XYangle",90.0-30.0); 
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
      Control.addVariable(baseKey+"21XYangle",-90.0+30.0);
      Control.addVariable(baseKey+"Filled",0);
      Control.addVariable(baseKey+"Active",0);


    }
  return;
}

}  // NAMESPACE setVariable
