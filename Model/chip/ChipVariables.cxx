/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   chip/ChipVariables.cxx
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
#include <iterator>
#include <memory>

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
ChipVariables(FuncDataBase& Control)
  /*!
    Function to set the control variables and constants
    -- This version is for TS2
    \param Control :: Function data base to add constants too
  */
{
  // CHIP SHUTTER INSERT:
  Control.addVariable("chipInsertIN",1);          // Insert number of inner cylinder
  Control.addVariable("chipInsertON",1);          // Insert number of outer cylinder
  Control.addVariable("chipShutterFStep",8.0);     // Forward step off insert.
  Control.addVariable("chipShutterMFStep",16.0);   // Second step off insert

  Control.addVariable("chipShutterShineMat","Lead+Sb");      // shine material [lead+sb]
  Control.addVariable("chipShutterShineFrontLeft",5.0); // Front left
  Control.addVariable("chipShutterShineFrontRight",5.0); // Front left
  Control.addVariable("chipShutterShineFrontDown",12.0); // Front left
  Control.addVariable("chipShutterShineFrontUp",4.18); // Front left

  Control.addVariable("chipShutterShineEndLeft",5.0); // End left
  Control.addVariable("chipShutterShineEndRight",5.0); // End left
  Control.addVariable("chipShutterShineEndDown",8.65); // End down
  Control.addVariable("chipShutterShineEndUp",11.19); // End left

  Control.addVariable("chipShutterShineHeight",4.0);  // shine Height
  Control.addVariable("chipShutterShineWidth",4.0);   // shine Width 
  Control.addVariable("chipShutterShineVoffset",2.0); // shine vertical offset
  Control.addVariable("chipShutterShineHoffset",0.0);    // shine horrizonal
  Control.addVariable("chipShutterShineInsertStep",3.5); // Inset at far end
   
  Control.addVariable("chipShutterBackScrapThick",1.5); 
  Control.addVariable("chipShutterBackScrapExtra",0.5); 
  Control.addVariable("chipShutterBackScrapMat","Aluminium"); 


  Control.addVariable("chipShutterIR1",8.2);        // Radius of cylinder
  Control.addVariable("chipShutterIR2",10.3);       // Radius of cylinder 
  Control.addVariable("chipShutterIL1_1",1.0);     // Length of cylinder
  Control.addVariable("chipShutterIL1_2",1.0);     // Length of cylinder
  Control.addVariable("chipShutterIL2_1",1.0);     // Length of cylinder
  Control.addVariable("chipShutterIL2_2",1.0);     // Length of cylinder
  Control.addVariable("chipShutterIM1_1","Stainless304");       // Material of cylinder
  Control.addVariable("chipShutterIM1_2","Void");       // Material of cylinder
  Control.addVariable("chipShutterIM2_1","Stainless304");       // Material of cylinder
  Control.addVariable("chipShutterIM2_2","Void");       // Material of cylinder

  Control.addVariable("chipShutterIME_1","Stainless304");       // Material of external
  Control.addVariable("chipShutterIME_2","Void");       // Material of external
  Control.addVariable("chipShutterILE_1",1.0);     // length of external
  Control.addVariable("chipShutterILE_2",1.0);     // length of external

  // Chip Insert Section [In BulkInsert object]
  Control.addVariable("bulkInsert1IWidth",22.50);
  Control.addVariable("bulkInsert1OWidth",22.50);
  Control.addVariable("bulkInsert1Width",37.50);
  Control.addVariable("chipInsertZAngle",1.5);    // Angle relative to beam
  Control.addVariable("chipInsertXYAngle",0.0);   // Angle relative to beam
  Control.addVariable("chipInsertRZDisp",10.45);    // Lift of pipe
  Control.addVariable("chipInsertRXDisp",0.0);    // Shift across beam
  Control.addVariable("chipInsertFStep",3.0);    // Forward step [down beam]
  Control.addVariable("chipInsertBStep",31.0);    // Back step [up beam]
  Control.addVariable("chipInsertRadius",20.0);    // Radius
  Control.addVariable("chipInsertLowCut",10.0);    // Low cut from the base
  Control.addVariable("chipInsertFrontMat","Void");      // Material
  Control.addVariable("chipInsertBackMat","Void");      // Material
  Control.addVariable("chipInsertDefMat","CastIron");      // Material
  Control.addVariable("chipInsertNLayers",0);     // Number of layers
  Control.addVariable("chipInsertMat_2","CastIron");      // Cast steel

  Control.addVariable("chipInsertClearSideGap",0.6);      
  Control.addVariable("chipInsertClearBaseGap",1.71);    
  Control.addVariable("chipInsertClearTopGap",2.0);      

  Control.addVariable("chipInsertClearSideOff",3.0);     
  Control.addVariable("chipInsertClearBaseOff",3.0);    
  Control.addVariable("chipInsertClearTopOff",3.0);      

  // Insert Cylinders:
  Control.addVariable("chipCylInsertNCylinder",1);      // 
  Control.addVariable("chipCylInsertRadius1",8.0);      // 
  Control.addVariable("chipCylInsertThick1",1.0);      // 
  Control.addVariable("chipCylInsertMat1","Stainless304");      // 
  Control.addVariable("chipCylInsertAOffVec1",
		      Geometry::Vec3D(0,0,0)); 
  Control.addVariable("chipCylInsertBOffVec1",
		      Geometry::Vec3D(0,0,0)); 

  // Front lead Plate: 
  Control.addVariable("chipPb1Active",0);      // 
  Control.addVariable("chipPb1ZAngle",0.0);      // 
  Control.addVariable("chipPb1XYAngle",0.0);      // 
  Control.addVariable("chipPb1YStep",10.0);      // 
  Control.addVariable("chipPb1Thick",9.0);      // 
  Control.addVariable("chipPb1DefMat","Lead");      // 
  Control.addVariable("chipPb1SupportMat","Stainless304");      //  304 stainless
  Control.addVariable("chipPb1CentSpace",1.7);      // 
  Control.addVariable("chipPb1Radius",0.75);      // 
  Control.addVariable("chipPb1LinerThick",0.0);      // 
  // Back lead Plate
  Control.addVariable("chipPb2Active",0);      // 
  Control.addVariable("chipPb2ZAngle",0.0);      // 
  Control.addVariable("chipPb2XYAngle",0.0);      // 
  Control.addVariable("chipPb2YStep",-50.0);      // 
  Control.addVariable("chipPb2Thick",9.0);      // 
  Control.addVariable("chipPb2DefMat","Lead");      // 
  Control.addVariable("chipPb2SupportMat","Stainless304");      //  304 stainless
  Control.addVariable("chipPb2CentSpace",4.0);      // 
  Control.addVariable("chipPb2Radius",1.5);      // 
  Control.addVariable("chipPb2LinerThick",0.0);      // 
  
  // FILTER BLOCK
  Control.addVariable("chipFilterXStep",0.0);       // Accross guide
  Control.addVariable("chipFilterYStep",500.0);     // Distance down guide
  Control.addVariable("chipFilterZStep",0.0);       // Vertical
  Control.addVariable("chipFilterXYAngle",0.0);     // Horrizontal rotation
  Control.addVariable("chipFilterZAngle",0.0);      // Vertical

  Control.addVariable("chipFilterOuterLen",52.0);      // +30 on inner
  Control.addVariable("chipFilterOuterWidth",95.0);      // +30 on inner
  Control.addVariable("chipFilterOuterHeight",87.0);      // Vertical

  Control.addVariable("chipFilterInnerLen",52.0);      // +30 on inner
  Control.addVariable("chipFilterInnerWidth",92.0);      // +30 on inner
  Control.addVariable("chipFilterInnerLen",87.0);      // Vertical

  // GUIDE::
  Control.addVariable("chipGuideAngle",1.5);      // Angle of guide [vertical]
  Control.addVariable("chipGuideSideAngle",0.65); // Side Angle of guide

  Control.addVariable("chipGuideXShift",-10.0);     // General X Shift
  Control.addVariable("chipGuideZShift",28.0);      // General Z Shift;

  Control.addVariable("chipGuideXBeamShift",-7.0);     // Guide X Shift
  Control.addVariable("chipGuideZBeamShift",28.0);     // Guide Z Shift;

  Control.addVariable("chipGuideInnerARoofZ",13.0);      // Near
  Control.addVariable("chipGuideInnerBRoofZ",24.0);
  Control.addVariable("chipGuideInnerAFloorZ",46.0);      // Near
  Control.addVariable("chipGuideInnerBFloorZ",13.0);      // Far

  Control.addVariable("chipGuideInnerALWall",10.0);      // near (left)
  Control.addVariable("chipGuideInnerARWall",26.0);      // near (right)
  Control.addVariable("chipGuideInnerBLWall",10.0);
  Control.addVariable("chipGuideInnerBRWall",28.0);

  Control.addVariable("chipGuideNLiner",2);  
  Control.addVariable("chipGuideLiner1Thick",0.5);  
  Control.addVariable("chipGuideLiner2Thick",2.0);  
  Control.addVariable("chipGuideLiner1Mat","Stainless304");  
  Control.addVariable("chipGuideLiner2Mat","Lead");  

  Control.addVariable("chipGuideRoofSteel",54.0);  
  Control.addVariable("chipGuideFloorSteel",56.0);
  Control.addVariable("chipGuideLeftSteel",66.0+18.0);    // 46
  Control.addVariable("chipGuideRightSteel",101.3);
  Control.addVariable("chipGuideLeftSteelInner",50.1);   //30
  Control.addVariable("chipGuideRightSteelInner",71.70);
  Control.addVariable("chipGuideLeftSteelAngle",6.5);  
  Control.addVariable("chipGuideRightSteelAngle",7.88); // was 6.0

  Control.addVariable("chipGuideRoofConc",220.0);  
  Control.addVariable("chipGuideFloorConc",190.0);
  Control.addVariable("chipGuideRightConc",119.885);
  Control.addVariable("chipGuideLeftConcInner",60.8);
  //  Control.addVariable("chipGuideLeftConcInner",67.181);

  Control.addVariable("chipGuideRightConcInner",86.043);
  Control.addVariable("chipGuideLeftConcAngle",5.5719);
  Control.addVariable("chipGuideRightConcAngle",6.0);

  Control.addVariable("chipGuideBlockWallThick",20.0);
  Control.addVariable("chipGuideBlockWallHeight",410.0);
  Control.addVariable("chipGuideBlockWallLength",-498.0);

  // extra wall on left [TSA side]
  Control.addVariable("chipGuideExtraWallThick",20.0);
  Control.addVariable("chipGuideExtraWallHeight",410.0);
  Control.addVariable("chipGuideExtraWallLength",250.0);
  Control.addVariable("chipGuideExtraWallSideAngle",45.0); 
  Control.addVariable("chipGuideExtraWallEndAngle",45.0); 
 
// extra wall on right [W2 side]
  Control.addVariable("chipGuideRightWallThick",20.0);
  Control.addVariable("chipGuideRightWallHeight",410.0);
  Control.addVariable("chipGuideRightWallLength",120.0);

// Remedial wall on right/west [W2 side]
  Control.addVariable("chipGuideRemedialVoid",0);     // MAKE remedial void
    
  Control.addVariable("chipGuideRemedialWallThick",78.0);
  Control.addVariable("chipGuideRemedialWallHeight",610.0);

  Control.addVariable("chipGuideRemedialWestNLayers",6);
  Control.addVariable("chipGuideWRemedMat_0","B-Poly");
  Control.addVariable("chipGuideWRemedMat_1","ChipIRSteel");
  Control.addVariable("chipGuideWRemedMat_2","ChipIRSteel");
  Control.addVariable("chipGuideWRemedMat_3","Concrete");
  Control.addVariable("chipGuideWRemedMat_4","Concrete");   
  Control.addVariable("chipGuideWRemedMat_5","Concrete");    

  Control.addVariable("chipGuideWRemedLen_1",-5.0);  // Steel
  Control.addVariable("chipGuideWRemedLen_2",-18.0);  // Steel
  Control.addVariable("chipGuideWRemedLen_3",-18.0);   // Steel inner
  Control.addVariable("chipGuideWRemedLen_4",-18.0);   // Concreate
  Control.addVariable("chipGuideWRemedLen_5",-16.0);   // Contrete inner

  
  // wedge block on left [TSA side]
//  Control.addVariable("chipGuideLeftWedgeThick",80.0);
//  Control.addVariable("chipGuideLeftWedgeAngle",20.0);
//  Control.addVariable("chipGuideLeftWedgeHeight",240.0);

  Control.addVariable("chipGuideLinerMat","Lead");  
  Control.addVariable("chipGuideSteelMat","ChipIRSteel");  
  Control.addVariable("chipGuideConcMat","NELCOConcrete");  
  Control.addVariable("chipGuideWallMat","NELCOConcrete");  
  Control.addVariable("chipGuideRemedialSteelMat","ChipIRSteel");
  Control.addVariable("chipGuideRemedialConcMat","NELCOConcrete"); 


  Control.addVariable("chipGPlateZAngle",0.0);    // Angle relative to beam
  Control.addVariable("chipGPlateXYAngle",0.0);   // Angle relative to beam
  Control.addVariable("chipGPlateFStep",10.0);    // Forward step [down beam]
  Control.addVariable("chipGPlateWidth",-1.0);    // Negative [all]
  Control.addVariable("chipGPlateHeight",-1.0);   // Negative [all]
  Control.addVariable("chipGPlateDepth",0.2);     // thinkness
  Control.addVariable("chipGPlateDefMat","Aluminium");      // Material
  Control.addVariable("chipGPlateNLayers",0);     // Number of layers

  
  Control.addVariable("chipGuideNLayers",3);    // Number of layers
  Control.addVariable("chipGuideNConcLayers",3);    // Number of layers

  Control.addVariable("chipGuideSteelFrac_1",0.3);  // Tungsten inner
  Control.addVariable("chipGuideSteelFrac_2",0.6);   // Steel inner
  Control.addVariable("chipGuideSteelFrac_3",0.8);   // Steel inner

  Control.addVariable("chipGuideSteelFrac_1",0.05);  // Tungsten inner
  Control.addVariable("chipGuideSteelFrac_2",0.2);   // Steel inner
  Control.addVariable("chipGuideSteelFrac_3",0.3);   // Steel inner
  Control.addVariable("chipGuideSteelFrac_4",0.4);   // Contrete inner
  Control.addVariable("chipGuideSteelFrac_5",0.5);   // Concrete inner
  Control.addVariable("chipGuideSteelFrac_6",0.6);   // Concrete inner
  Control.addVariable("chipGuideSteelFrac_7",0.7);   // Concrete outer

  Control.addVariable("chipGuideMat_0","ChipIRSteel");   // Steel
  Control.addVariable("chipGuideMat_1","ChipIRSteel");         // lead liner
  Control.addVariable("chipGuideMat_2","ChipIRSteel");    // Steel
  Control.addVariable("chipGuideMat_4","Concrete");    // concrete [normal]
 
  Control.addVariable("chipHutYStart",1253.1);    // Y start position
  Control.addVariable("chipHutXStep",35.46);      // X-shift
  Control.addVariable("chipHutYStep",0.0);      // Y-shift
  Control.addVariable("chipHutZLine",28.0);      // Z-shift

  // ROOM:: 
  Control.addVariable("chipSndAngle",0.25);        // Secondary angle [degrees]
  Control.addVariable("chipHutScreenY",1.0);       // Thickness of screen
  Control.addVariable("chipHutGuideWidth",48.0);   // Nose width [hole]
  Control.addVariable("chipHutFLWidth",125.05);    // Outside nose width [left]
  Control.addVariable("chipHutFRWidth",218.95);    // Outside node width [right]
  Control.addVariable("chipHutFullLWidth",235.46); // Outside width [left]
  Control.addVariable("chipHutPartRWidth",293.26); // Outside width [right]
  Control.addVariable("chipHutFullRWidth",425.84); // Outside width [right]
  Control.addVariable("chipHutMainLen",1105.1);    // Full hutch length
  Control.addVariable("chipHutLSlopeLen",957.2);   // Left side slope/flat intercept
  Control.addVariable("chipHutRSlopeLen",326.56);  // Right slope/flat intercept
  Control.addVariable("chipHutPartLen",715.23);    // Part Step length 
  Control.addVariable("chipHutExtLen",1460.1);    // Extended hutch length 
  Control.addVariable("chipHutRoof",230.5);       // Roof from target centre
  Control.addVariable("chipHutFloor",280.5);      // Floor from target centre [into earth]
  Control.addVariable("chipHutFalseFloorSurf",1.0);    // False floor
  Control.addVariable("chipHutFalseFloorVoid",100.0); // False floor
  Control.addVariable("chipHutRoofThick",90.5);   // Roof thickness
  Control.addVariable("chipHutFloorDepth",90.5);  // Floor thickness
  Control.addVariable("chipHutLWallThick",90.5);  // Wall thickness
  Control.addVariable("chipHutRWallThick",90.5);  // Wall thickness
  Control.addVariable("chipHutFWallThick",90.5);  // Front Wall thickness
  Control.addVariable("chipHutBWallThick",90.5);  // Back Wall thickness

  Control.addVariable("chipHutEdgeVoid",20.0);  // Void at left wall
  // WALKWAY
  Control.addVariable("chipHutExtDoor",20.0);        // Door thickness
  Control.addVariable("chipHutWalkInWall",224.5);    // Inner walkway wall
  Control.addVariable("chipHutWalkOutWall",168.0);   // Outer walkway wall

  Control.addVariable("chipHutBlockYBeg",673.73);    // Start of block on door
  Control.addVariable("chipHutBlockYEnd",841.73);    // Block is 360mm thick
  Control.addVariable("chipHutBlockOut",166.76);     // Block is 360mm thick
  Control.addVariable("chipHutBlockSndLen",953.8);   // Block on beam stop

  // Feedthough under door:
  // Positions from left corner  : W2 wall/Floor/door
  Control.addVariable("chipHutFBNFeed",5);   // Nfeed
  Control.addVariable("chipHutFBLen",50.0);   // Length
  Control.addVariable("chipHutFBmat","Concrete");   // Block on beam stop
  Control.addVariable("chipHutFBCent0",Geometry::Vec3D(49.0,0,29.5));  
  Control.addVariable("chipHutFBCent1",Geometry::Vec3D(24.5,0,73.0));  
  Control.addVariable("chipHutFBCent2",Geometry::Vec3D(45.1,0,70.0));  
  Control.addVariable("chipHutFBCent3",Geometry::Vec3D(68.7,0,70.0));  
  Control.addVariable("chipHutFBCent4",Geometry::Vec3D(93.3,0,70.0));  
  Control.addVariable("chipHutFBSize0",12.50);  // Radius of square [half width]
  Control.addVariable("chipHutFBSize1",5.0);  
  Control.addVariable("chipHutFBSize2",8.0);  
  Control.addVariable("chipHutFBSize3",8.0);  
  Control.addVariable("chipHutFBSize4",8.0);  

  // Sample
  Control.addVariable("chipHutNSamples",8);
  Control.addVariable("chipSampleDefMat","Aluminium");   // Aluminium
  Control.addVariable("chipSampleWidth",40.0);   
  Control.addVariable("chipSampleHeight",40.0);  
  Control.addVariable("chipSampleTableNum",2);    // Table 2
  Control.addVariable("chipSampleXYAngle",0.0); 
  Control.addVariable("chipSampleZAngle",0.0); 
  Control.addVariable("chipSampleXStep",0.0);  
  Control.addVariable("chipSampleZStep",40.0);   // From table top
  Control.addVariable("chipSampleDepth",0.5);    // thickness 5mm


  Control.addVariable("chipSample1YStep",-10.0);   // Relative to table centre  
  Control.addVariable("chipSample2YStep",-5.0);   
  Control.addVariable("chipSample3YStep",0.0);   
  Control.addVariable("chipSample4YStep",5.0);   
  Control.addVariable("chipSample5YStep",10.0);  
  Control.addVariable("chipSample6YStep",15.0);  
  Control.addVariable("chipSample7YStep",20.0);  
  Control.addVariable("chipSample8YStep",25.0);  


  // Feedthrough
  Control.addVariable("chipNWires",4);
  // Control.addVariable("chipWiresColl1Offset",Geometry::Vec3D(-100,110,50));  
  Control.addVariable("chipWiresColl1Offset",Geometry::Vec3D(-230,240,50));  
  Control.addVariable("chipWiresColl2Offset",Geometry::Vec3D(-350,360,50));  
  Control.addVariable("chipWiresColl3Offset",Geometry::Vec3D(-450,460,50));
  Control.addVariable("chipWiresColl4Offset",Geometry::Vec3D(-730,740,50));  

  Control.addVariable("chipWiresCollHeight",7.50);
  Control.addVariable("chipWiresCollWidth",7.50);
  Control.addVariable("chipWiresCollTrack0",Geometry::Vec3D(19.5,0,0));
  Control.addVariable("chipWiresCollTrack1",Geometry::Vec3D(19.5,0,-41.0));
  Control.addVariable("chipWiresCollTrack2",Geometry::Vec3D(34.5,0,-41.0));
  Control.addVariable("chipWiresCollTrack3",Geometry::Vec3D(34.5,51.0,-41.0));
  Control.addVariable("chipWiresCollTrack4",Geometry::Vec3D(71.8,51.0,-41.0));
  Control.addVariable("chipWiresCollTrack5",Geometry::Vec3D(71.8,51.0,-66.1));
  Control.addVariable("chipWiresCollTrack6",Geometry::Vec3D(91.3,51.0,-66.1));
 
  // BEAM STOP::
  Control.addVariable("chipHutBSOpenXStep",-17.0);        // X offset of Void
  Control.addVariable("chipHutBSOpenZStep",50.0);        // Z offset of Void
  Control.addVariable("chipHutBSOpenWidth",120.0);        // Z offset of Void
  Control.addVariable("chipHutBSOpenHeight",100.0);        // Z offset of Void

  Control.addVariable("chipHutWestExtraThick",60.0);        // Width of extra
  Control.addVariable("chipHutWestExtraLength",600.0);      // Distance down


  Control.addVariable("chipHutWestNDivide",6);   
  Control.addVariable("chipHutWestMat_0","B-Poly");
  Control.addVariable("chipHutWestMat_1","ChipIRSteel");
  Control.addVariable("chipHutWestMat_2","ChipIRSteel");
  Control.addVariable("chipHutWestMat_3","Concrete");
  Control.addVariable("chipHutWestMat_4","Concrete");   
  Control.addVariable("chipHutWestMat_5","Concrete");    

  Control.addVariable("chipHutWestLen_1",-5.0);  // Steel
  Control.addVariable("chipHutWestLen_2",-18.0);  // Steel
  Control.addVariable("chipHutWestLen_3",-18.0);   // Steel inner
  Control.addVariable("chipHutWestLen_4",-9.0);   // Concrete
  Control.addVariable("chipHutWestLen_5",-7.0);   // Contrete 

  Control.addVariable("chipBSXStep",-12.50);      // +ve to TSA
  Control.addVariable("chipBSYStep",-24.0);      // [from outersurf 32]
  Control.addVariable("chipBSZStep",50.0);       // 

  Control.addVariable("chipBSInnerWidth",188.0);      // Beam Stop width 
  Control.addVariable("chipBSInnerHeight",192.5);     // Beam Stop height
  Control.addVariable("chipBSInnerLength",226.0);      // Beam Stop length

  Control.addVariable("chipBSSteelWidth",20.0);      // Beam Stop width 
  Control.addVariable("chipBSSteelHeight",20.0);     // Beam Stop heigh
  Control.addVariable("chipBSSteelLength",20.0);      // Beam Stop length
  Control.addVariable("chipBSSteelDepth",20.0);      // Beam Stop depth

  Control.addVariable("chipBSVoidWidth",5.0);      // void width 
  Control.addVariable("chipBSVoidHeight",5.5);     // void heigh
  Control.addVariable("chipBSVoidLength",5.0);     // void length
  Control.addVariable("chipBSVoidDepth",5.0);      // Void Depth

  Control.addVariable("chipBSConcWidth",30.0);      // Beam Stop width 
  Control.addVariable("chipBSConcHeight",30.5);     // Beam Stop heigh
  Control.addVariable("chipBSConcDepth",30.0);      
  Control.addVariable("chipBSConcLength",30.0);     

  Control.addVariable("chipBSConcMat","Concrete");      // Concrete
  Control.addVariable("chipBSSteelMat","Stainless304");     // Steel
  Control.addVariable("chipBSInnerMat","Stainless304");   // Inner material

  Control.addVariable("chipBSInnerAngle1",0);       // Angle of plates
  Control.addVariable("chipBSInnerMat1","Aluminium");         // Al
  Control.addVariable("chipBSInnerThick1",0.5);     // 2cm : 

  Control.addVariable("chipBSInnerAngle2",0);       // Angle of plates
  Control.addVariable("chipBSInnerMat2","Void");         // Al
  Control.addVariable("chipBSInnerThick2",22.5);     // 2cm : 

  Control.addVariable("chipBSInnerAngle3",0);
  Control.addVariable("chipBSInnerMat3","Lead");         // Lead layer
  Control.addVariable("chipBSInnerThick3",2.0);     // Thickness 2.0cm

  Control.addVariable("chipBSInnerAngle4",0);
  Control.addVariable("chipBSInnerMat4","Void");         // void layer
  Control.addVariable("chipBSInnerThick4",2.5);     // Thickness 2.5cm

  Control.addVariable("chipBSInnerAngle5",0);
  Control.addVariable("chipBSInnerMat5","LithiumCarbonate");         // LiCO3
  Control.addVariable("chipBSInnerThick5",15.0);     // Thickness 5cm

  Control.addVariable("chipBSInnerAngle6",0);
  Control.addVariable("chipBSInnerMat6","Aluminium");           // Al 
  Control.addVariable("chipBSInnerThick6",5.0);       // Thickness 5cm

  Control.addVariable("chipBSInnerAngle7",0.0);
  Control.addVariable("chipBSInnerMat7","Stainless304");          // Steel
  Control.addVariable("chipBSInnerThick7",25.0);

  Control.addVariable("chipBSInnerAngle8",0);
  Control.addVariable("chipBSInnerMat8","Aluminium");           // Al layer
  Control.addVariable("chipBSInnerThick8",5.0);        // Thickness 15cm

  Control.addVariable("chipBSInnerAngle9",0);
  Control.addVariable("chipBSInnerMat9","Stainless304");             // Steel 
  Control.addVariable("chipBSInnerThick9",25.0);        // Thickness 30cm

  Control.addVariable("chipBSInnerAngle10",0);
  Control.addVariable("chipBSInnerMat10","Poly");            // Poly
  Control.addVariable("chipBSInnerThick10",30.0);        // 

  Control.addVariable("chipBSInnerAngle11",0);
  Control.addVariable("chipBSInnerMat11","Concrete");          //  Concrete
  Control.addVariable("chipBSInnerThick11",30.0);       // 3cm

  Control.addVariable("chipBSInnerAngle12",0);
  Control.addVariable("chipBSInnerMat12","Void");          // 
  Control.addVariable("chipBSInnerThick12",2.5);       // 25mm void

  Control.addVariable("chipBSInnerAngle13",0);
  Control.addVariable("chipBSInnerMat13","Lead");          // High density lead
  Control.addVariable("chipBSInnerThick13",3.0);       // 3cm

  Control.addVariable("chipBSInnerAngle14",0);
  Control.addVariable("chipBSInnerMat14","Void");          // Void
  Control.addVariable("chipBSInnerThick14",2.5);       // 


  // THESE 5 MUST GO:
  Control.addVariable("chipHutBeamStopRoof",160.0);    // roof height
  Control.addVariable("chipHutBeamStopRoofThick",80.0);// roof height
  Control.addVariable("chipHutBeamStopYBackWall",20.0); // BS Back wall
  Control.addVariable("chipHutBeamStopXSideWall",40.0); // BS Side wall
  Control.addVariable("chipHutBeamStopBase",100.0);     // BS base thickness

  //  Does this do anything ????
  Control.addVariable("chipHutZshift",10.0);     // Shift upwards of Z
  Control.addVariable("chipHutRoofMat","CastIron");        // Steel
  Control.addVariable("chipHutWallMat","CastIron");        // Steel
  Control.addVariable("chipHutInnerWallMat","Concrete");   // Concrete 
  Control.addVariable("chipHutRearVoidMat","Concrete");    // concrete at back
  Control.addVariable("chipHutFalseFloorMat","Aluminium");   // Aluminium
  Control.addVariable("chipHutFloorMat","Concrete");       // Concrete

  // Hutch inner stuff:
  Control.addVariable("chipHutTrimMat","Stainless304");        // Steel
  Control.addVariable("chipHutTrimXStep",12.0);   // Shift of the x positoin
  Control.addVariable("chipHutTrimYStep",231.1);  // Distance from hutch start [Y]
  Control.addVariable("chipHutTrimZStep",19.0);   // up shift of the hole

  Control.addVariable("chipHutTrimWidth",40.0);  // Horr half-beam width [hole]
  Control.addVariable("chipHutTrimHeight",40.0);  // Vertial half-height [hole]
  Control.addVariable("chipHutTrimDepth",35.0);   // Thickness in beam

  Control.addVariable("chipHutTrimNLayers",5);   
  Control.addVariable("chipHutTrimFrac_1",-0.5);          
  Control.addVariable("chipHutTrimFrac_2",-1.5);          
  Control.addVariable("chipHutTrimFrac_3",-0.5);          
  Control.addVariable("chipHutTrimFrac_4",-29.0);          
  Control.addVariable("chipHutTrimFrac_5",-29.0);   
       
  Control.addVariable("chipHutTrimMat_0","Aluminium");      // Al
  Control.addVariable("chipHutTrimMat_1","Boron");     // Boron
  Control.addVariable("chipHutTrimMat_2","Aluminium");      // Al
  Control.addVariable("chipHutTrimMat_3","CastIron");     // Steel
  Control.addVariable("chipHutTrimMat_4","Aluminium");      // Al

  Control.addVariable("chipHutScreenMat","B4C");         // Screen mat

  Control.addVariable("chipHutTable1XYAngle",0.0);     // XY Rotation [relative to beam]
  Control.addVariable("chipHutTable1ZAngle",0.0);      // Z Rotation [relative to beam]
  Control.addVariable("chipHutTable1FStep",297.0);     // Start from hutch wall
  Control.addVariable("chipHutTable1XStep",0.0);      // Accross beam
  Control.addVariable("chipHutTable1Height",-40.0);    // Height relative to beam centre
  Control.addVariable("chipHutTable1Width",80.0);     // Width
  Control.addVariable("chipHutTable1Length",96.5);    // Length
  Control.addVariable("chipHutTable1SurThick",5.0);    // Surface Thickness
  Control.addVariable("chipHutTable1SideThick",15.0);  // Side thickness
  Control.addVariable("chipHutTable1TopMat","Aluminium");        // Aluminium
  Control.addVariable("chipHutTable1Mat","Stainless304");        // Steel

  Control.addVariable("chipHutTable2XYAngle",0.0);     // XY Rotation
  Control.addVariable("chipHutTable2ZAngle",0.0);      // Z Rotation 
  Control.addVariable("chipHutTable2FStep",590.90);    // Start from hutch wall
  Control.addVariable("chipHutTable2XStep",0.0);       // Accross beam [+ve towards TSA]
  Control.addVariable("chipHutTable2Height",-20.0);    // Height relative to beam centre
  Control.addVariable("chipHutTable2Width",96.0);      // Width
  Control.addVariable("chipHutTable2Length",196.0);    // Length
  Control.addVariable("chipHutTable2SurThick",5.0);    // Surface Thickness
  Control.addVariable("chipHutTable2SideThick",15.0);  // Side thickness
  Control.addVariable("chipHutTable2TopMat","Aluminium");        // Aluminium
  Control.addVariable("chipHutTable2Mat","Stainless304");        // Steel

  // Hutch beamstop
  Control.addVariable("chipHutWalkY",635.0);           // Walk extension length
  Control.addVariable("chipHutWalkXWrap",297.4);       // First corner length
  Control.addVariable("chipHutWalkWallThick",50.0);    // Wall thickness
  Control.addVariable("chipHutWalkSndX",450.0);        // Total wall thickness


  Control.addVariable("chipHutLWallNDivide",14); // N-layers in the wall
  Control.addVariable("chipHutRWallNDivide",14); // N-layers in the wall
  // Control.addVariable("chipHutWallFrac_1",-5.0);   // Aluminium inner [5cm]
  // Control.addVariable("chipHutWallFrac_2",-7.2);   // Layers of Steel
  // Control.addVariable("chipHutWallFrac_3",-7.2);   // Layers of Steel
  // Control.addVariable("chipHutWallFrac_4",-7.2);   // Layers of Steel
  // Control.addVariable("chipHutWallFrac_5",-7.2);   // Layers of Steel
  Control.addVariable("chipHutLWallMat_0","Aluminium");    // Aluminium       
  Control.addVariable("chipHutLWallMat_1","CastIron");   // Cast steel     
  Control.addVariable("chipHutLWallMat_6","Concrete");   // Concrete
  Control.addVariable("chipHutRWallMat_0","Aluminium");    // Aluminium       
  Control.addVariable("chipHutRWallMat_1","CastIron");   // Cast steel     
  Control.addVariable("chipHutRWallMat_6","Concrete");   // Concrete
  Control.addVariable("chipHutLWallFrac_1",-0.5);   // Al inner [5cm]
  Control.addVariable("chipHutLWallFrac_2",-7.20);   // Al inner [5cm]
  Control.addVariable("chipHutLWallFrac_3",-7.20);   // Al inner [5cm]
  Control.addVariable("chipHutLWallFrac_4",-7.20);   // Al inner [5cm]
  Control.addVariable("chipHutLWallFrac_5",-7.20);   // Al inner [5cm]
  Control.addVariable("chipHutLWallFrac_6",-7.20);   // Al inner [5cm]

  Control.addVariable("chipHutRWallFrac_1",-0.5);   // Al inner [5cm]
  Control.addVariable("chipHutRWallFrac_2",-7.20);   // Al inner [5cm]
  Control.addVariable("chipHutRWallFrac_3",-7.20);   // Al inner [5cm]
  Control.addVariable("chipHutRWallFrac_4",-7.20);   // Al inner [5cm]
  Control.addVariable("chipHutRWallFrac_5",-7.20);   // Al inner [5cm]
  Control.addVariable("chipHutRWallFrac_6",-7.20);   // Al inner [5cm]


  Control.addVariable("chipHutRoofNDivide",14); 
  Control.addVariable("chipHutRoofFrac_1",-0.5);   // Al inner [5cm]
  Control.addVariable("chipHutRoofFrac_2",-7.20);   // Steel section
  Control.addVariable("chipHutRoofFrac_3",-7.20);   // 
  Control.addVariable("chipHutRoofFrac_4",-7.20);   // 
  Control.addVariable("chipHutRoofFrac_5",-7.20);   // Steel section
  Control.addVariable("chipHutRoofFrac_6",-7.20);   // Steel section
  Control.addVariable("chipHutRoofMat_0","Aluminium");     
  Control.addVariable("chipHutRoofMat_1","CastIron"); 
  Control.addVariable("chipHutRoofMat_6","Concrete");   // Concrete       

  Control.addVariable("chipHutFloorNDivide",1);        //
  // Control.addVariable("chipHutFloorFrac_1",-2.0);          
  // Control.addVariable("chipHutFloorFrac_2",-56.0);      
  // Control.addVariable("chipHutFloorFrac_3",-20.0);      
  // Control.addVariable("chipHutFloorMat_0","Aluminium");      // Al
  // Control.addVariable("chipHutFloorMat_1","Void");     // Void
  // Control.addVariable("chipHutFloorMat_2","Concrete");     // Concrete
  
  Control.addVariable("chipPreXYAngle",0.0);           // XY rotation
  Control.addVariable("chipPreZAngle",0.0);            // Z rotation
  Control.addVariable("chipPreFStep",20.0);            // Forward step 
  Control.addVariable("chipPreXStep",74.0);           // +ve to TSA wall
  Control.addVariable("chipPreZStep",16.0);             // Up step 
  Control.addVariable("chipPreRadius",80.0);           // Full Radius
  Control.addVariable("chipPreDepth",68.0);            // Full thickness (?)
  Control.addVariable("chipPreDefMat","Stainless304");              // Steel
  Control.addVariable("chipPreHoleIndex",6);           // Index [0 for angle]
  Control.addVariable("chipPreHoleAngOff",0.0);        // Angle
  Control.addVariable("chipPreNHole",7);               // Number of holes

  Control.addVariable("chipPreHole1Shape",4);          // Octagon
  Control.addVariable("chipPreHole1Radius",2.5);      // Radius
  Control.addVariable("chipPreHole1ROffset",50.0);     // R-Displacement
  Control.addVariable("chipPreHole1Angle",0.0);        // Angle [in degree]

  Control.addVariable("chipPreHole2Shape",4);           // Octagon
  Control.addVariable("chipPreHole2Radius",5.0);       // Radius
  Control.addVariable("chipPreHole2ROffset",50.0);      // R-Displacement
  Control.addVariable("chipPreHole2Angle",35.0);        // Angle [in degree]

  Control.addVariable("chipPreHole3Shape",4);           // Octagon
  Control.addVariable("chipPreHole3Radius",7.5);        // Radius
  Control.addVariable("chipPreHole3ROffset",50.0);      // R-Displacement
  Control.addVariable("chipPreHole3Angle",70.0);        // Angle [in degree]

  Control.addVariable("chipPreHole4Shape",4);           // Octagon
  Control.addVariable("chipPreHole4Radius",10.00);      // Radius
  Control.addVariable("chipPreHole4ROffset",50.0);      // R-Displacement
  Control.addVariable("chipPreHole4Angle",115.0);       // Angle [in degree]

  Control.addVariable("chipPreHole5Shape",4);           // Octagon
  Control.addVariable("chipPreHole5Radius",12.5);       // Radius
  Control.addVariable("chipPreHole5ROffset",50.0);      // R-Displacement
  Control.addVariable("chipPreHole5Angle",175.0);       // Angle [in degree]

  Control.addVariable("chipPreHole6Shape",2);           // Square
  Control.addVariable("chipPreHole6Radius",20.0);       // 
  Control.addVariable("chipPreHole6ROffset",50.0);      // R-Displacement
  Control.addVariable("chipPreHole6Angle",250.0);       // Angle [in degree]

  Control.addVariable("chipPreHole7Shape",0);           // Nothing
  Control.addVariable("chipPreHole7Radius",20.0);       // Doesn't matter
  Control.addVariable("chipPreHole7ROffset",50.0);      // R-Displacement
  Control.addVariable("chipPreHole7Angle",310.0);       // Angle [in degree]
  
  Control.addVariable("chipPreInnerWall",1.0);
  Control.addVariable("chipPreInnerWallMat","Void");        // void
  Control.addVariable("chipPreNLayers",6);       
  Control.addVariable("chipPreFrac_1",-5.0);           // 5cm Al
  Control.addVariable("chipPreFrac_2",-10.0);          // 10cm PB
  Control.addVariable("chipPreFrac_3",-25.0);          // 25cm Steel 
  Control.addVariable("chipPreFrac_4",-3.0);           // 3cm Ni
  Control.addVariable("chipPreFrac_5",-20.0);          // 20cm Steel
  Control.addVariable("chipPreMat_0","Aluminium");    // Al
  Control.addVariable("chipPreMat_1","Lead");   // Pb
  Control.addVariable("chipPreMat_2","CastIron");   // Fe
  Control.addVariable("chipPreMat_3","Nickel");   // Ni
  Control.addVariable("chipPreMat_4","CastIron");   // Fe
  Control.addVariable("chipPreMat_5","Aluminium");    // Al

  Control.addVariable("chipColBoxXStep",26.0);
  Control.addVariable("chipColBoxFStep",100.0);
  Control.addVariable("chipColBoxZStep",30.0);
  Control.addVariable("chipColBoxXYangle",1.0);
  Control.addVariable("chipColBoxZangle",1.0);
  Control.addVariable("chipColBoxHeight",90.0);
  Control.addVariable("chipColBoxWidth",90.0);
  Control.addVariable("chipColBoxDepth",112.0);

  Control.addVariable("chipColBoxDefMat","Stainless304");
  Control.addVariable("chipColBoxOutMat","Stainless304");
  Control.addVariable("chipColBoxRoofThick",12.8);
  Control.addVariable("chipColBoxFloorThick",12.8);
  Control.addVariable("chipColBoxFrontThick",12.8);
  Control.addVariable("chipColBoxBackThick",12.8);
  Control.addVariable("chipColBoxViewX",40.0);
  Control.addVariable("chipColBoxViewZ",40.0);



  Control.addVariable("chipJawNLayers",5);          
  Control.addVariable("chipJawMat0","Stainless304");          // Fe
  Control.addVariable("chipJawMat1","Tungsten");              // W
  Control.addVariable("chipJawMat2","Aluminium");             // Al
  Control.addVariable("chipJawMat3","Nickel");                // W
  Control.addVariable("chipJawMat4","Stainless304");          // W

  Control.addVariable("chipJawLinerThick",0.5);         // lead thickness
  Control.addVariable("chipJawLinerMat","Lead");       // lead

  Control.addVariable("chipJawXStep",15.6);       // Step accross [+ve to TSA]
  Control.addVariable("chipJawYStep",248.0);       // Forward step 
  Control.addVariable("chipJawZStep",19.0);        // Forward step 
  Control.addVariable("chipJawXYangle",1.0);       // Rotation angle
  Control.addVariable("chipJawZangle",1.0);        // Z-

  Control.addVariable("chipJawZOpen",40.0);        // Gap in middel
  Control.addVariable("chipJawZThick",50.0);       // Height away from gap
  Control.addVariable("chipJawZCross",90.0);       // Width across gap
  Control.addVariable("chipJawZLen",50.0);         // Thickness along gap

  Control.addVariable("chipJawGap",10.0);        // Gap in middel
  
  Control.addVariable("chipJawXOpen",40.0);        // Gap in middel
  Control.addVariable("chipJawXThick",50.0);       // Thickness [Y]
  Control.addVariable("chipJawXCross",90.0);       // Width across gap
  Control.addVariable("chipJawXLen",30.0);         //  along gap

  Control.addVariable("chipJawXHeight",228.0);
  Control.addVariable("chipJawYHeight",120.0);
  Control.addVariable("chipJawZHeight",228.0);
  Control.addVariable("chipJawWallThick",0.0);
  Control.addVariable("chipJawxJawMat","Stainless304");           
  Control.addVariable("chipJawzJawMat","Stainless304");           
  Control.addVariable("chipJawWallMat","Aluminium");              
  
  Control.addVariable("chipColHGap",20.0);          // Gap
  Control.addVariable("chipColHOffset",0.0);        // Centre offset 
  Control.addVariable("chipColHWidth",120.0);
  Control.addVariable("chipColHHeight",180.0);
  Control.addVariable("chipColHDepth",50.0); 
  Control.addVariable("chipColHDefMat","Stainless304");              // Steel

  Control.addVariable("chipColHInnerWall",0.5);        // lead thickness
  Control.addVariable("chipColHInnerWallMat","Lead");       // lead
  Control.addVariable("chipColHNLayers",3);       
  Control.addVariable("chipColHFrac_1",-10.0);  
  Control.addVariable("chipColHFrac_2",-30.0);       
  Control.addVariable("chipColHMat_0","Stainless304");     // Fe
  Control.addVariable("chipColHMat_1","Tungsten");    // W
  Control.addVariable("chipColHMat_2","Aluminium");     // AL

  Control.addVariable("chipSourceAngle",35.0);         // Angle on the source
  Control.addVariable("chipSourceRadial",12.0);        // Width


  Control.addVariable("CGSlice0CentOffset",Geometry::Vec3D(95.0,212.5,0.0));
  Control.addVariable("CGSlice0Length",30.0);
  Control.addVariable("CGSlice0Height",-1.0);
  Control.addVariable("CGSlice0InsertKey","SteelRightVertical");
  Control.addVariable("CGSliceWidth",5.0);
  Control.addVariable("CGSliceXYAngle",90.0);
  
  Control.addVariable("chipGuideCutsActive",1);    // Number of cuts
  Control.addVariable("chipGuideNCuts",16);    // Number of cuts
    
  Control.addVariable("CGCut0CentOffset",Geometry::Vec3D(55.0,10.0,50.0));
  Control.addVariable("CGCut1CentOffset",Geometry::Vec3D(60.0,126.0,50.0));
  Control.addVariable("CGCut2CentOffset",Geometry::Vec3D(60.0,253.0,50.0));
  Control.addVariable("CGCut3CentOffset",Geometry::Vec3D(60.0,378.0,50.0));
  Control.addVariable("CGCut4CentOffset",Geometry::Vec3D(75.0,558.0,50.0));

  Control.addVariable("CGCut5CentOffset",Geometry::Vec3D(85.0,276.0,50.0));
  Control.addVariable("CGCut6CentOffset",Geometry::Vec3D(85.0,427.0,50.0));

  // left side
  Control.addVariable("CGCut7CentOffset",Geometry::Vec3D(-28.0,12.0,50.0));
  Control.addVariable("CGCut8CentOffset",Geometry::Vec3D(-40.0,90.0,50.0));
  Control.addVariable("CGCut9CentOffset",Geometry::Vec3D(-28.0,129.0,50.0));
  Control.addVariable("CGCut10CentOffset",Geometry::Vec3D(-28.0,253.0,50.0));
  Control.addVariable("CGCut11CentOffset",Geometry::Vec3D(-60.0,273.0,50.0));
  Control.addVariable("CGCut12CentOffset",Geometry::Vec3D(-28.0,378.0,50.0));
  Control.addVariable("CGCut13CentOffset",Geometry::Vec3D(-60.0,427.0,50.0));
  Control.addVariable("CGCut14CentOffset",Geometry::Vec3D(-70.0,520.0,50.0));
  Control.addVariable("CGCut15CentOffset",Geometry::Vec3D(-60.0,535.5,50.0));
  
  Control.addVariable("CGCut0Length",30.0);
  Control.addVariable("CGCut1Length",40.0);
  Control.addVariable("CGCut2Length",40.0);
  Control.addVariable("CGCut3Length",40.0);
  Control.addVariable("CGCut4Length",40.0);

  Control.addVariable("CGCut5Length",30.0);
  Control.addVariable("CGCut6Length",30.0);

  Control.addVariable("CGCut7Length",30.0);
  Control.addVariable("CGCut8Length",30.0);
  Control.addVariable("CGCut9Length",30.0);
  Control.addVariable("CGCut10Length",40.0);
  Control.addVariable("CGCut11Length",30.0);
  Control.addVariable("CGCut12Length",40.0);
  Control.addVariable("CGCut13Length",40.0);
  Control.addVariable("CGCut14Length",30.0);
  Control.addVariable("CGCut15Length",25.0);

  Control.addVariable("CGCut0InsertKey","SteelInnerRight");
  Control.addVariable("CGCut1InsertKey","SteelInnerRight");
  Control.addVariable("CGCut2InsertKey","SteelInnerRight");
  Control.addVariable("CGCut3InsertKey","SteelInnerRight");
  Control.addVariable("CGCut4InsertKey","SteelInnerRight");
  Control.addVariable("CGCut5InsertKey","SteelInnerRight");
  Control.addVariable("CGCut6InsertKey","SteelInnerRight");
  Control.addVariable("CGCut7InsertKey","SteelInnerLeft");
  Control.addVariable("CGCut8InsertKey","SteelInnerLeft");
  Control.addVariable("CGCut9InsertKey","SteelInnerLeft");
  Control.addVariable("CGCut10InsertKey","SteelInner");
  Control.addVariable("CGCut11InsertKey","SteelInner");
  Control.addVariable("CGCut12InsertKey","SteelOuterLeft");
  Control.addVariable("CGCut13InsertKey","SteelOuterLeft");
  Control.addVariable("CGCut14InsertKey","SteelOuterLeft");
  Control.addVariable("CGCut15InsertKey","SteelOuterLeft");

  Control.addVariable("CGCutWidth",15.0);
  Control.addVariable("CGCutHeight",3.5);
  Control.addVariable("CGCutXYAngle",90.0);

  
  
  
  // Control stuff
  return;
}

}  // NAMESPACE setVariables
