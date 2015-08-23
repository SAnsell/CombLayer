/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   zoom/ZoomVariables.cxx
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#include <boost/format.hpp>

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
#include "inputParam.h"
#include "variableSetup.h"

namespace setVariable
{

double B4Cspacer(FuncDataBase&,const int,
		 const double,const double);
double SteelBlock(FuncDataBase&,const int,const double,
		  const double,const double,const double,
		  const double,const double,
		  const int);

void
ZoomVariables(const mainSystem::inputParam& IParam,
	      FuncDataBase& Control)
  /*!
    Function to set the control variables and constants
    -- This version is for TS2
    \param Control :: Function data base to add constants too
  */
{
// -----------
// GLOBAL stuff
// -----------
  
  // E1/ZOOM BEAMLINE

  Control.addVariable("zoomBendInnerMat","CastIron");     // Zoom Inner
  Control.addVariable("zoomBendWallMat","Aluminium");     // Zoom Box
  Control.addVariable("zoomBendVertAngle",90.0);     // Rotation about centre
  Control.addVariable("zoomBendXAngle",24.0); // change of angle [mRad]
  Control.addVariable("zoomBendEnterXStep",-3.95); // Rotation about centre
  Control.addVariable("zoomBendEnterZStep",-8.8); // Rotation about centre
  Control.addVariable("zoomBendAngle",-24.0);      // bend angle [mRad]
  Control.addVariable("zoomBendXStep",0.0);     // Master Step
  Control.addVariable("zoomBendYStep",3.3);     // Master Step
  Control.addVariable("zoomBendZStep",0.0);     // Master Step

  Control.addVariable("zoomBendPathWidth",3.0);     // Width of guide
  Control.addVariable("zoomBendPathHeight",2.0);    // Heigh of guide
  Control.addVariable("zoomBendNVanes",5);          // Silicon vanes
  Control.addVariable("zoomBendVaneMat","SiCrystal");        // Silicon
  Control.addVariable("zoomBendVaneThick",0.03);    // Silicon thickness
  
  Control.addVariable("zoomBendWallThick",1.0);     // Wall thickness

  Control.addVariable("zoomBendSec1Length",115.04);
  Control.addVariable("zoomBendSec1Width",26.0);
  Control.addVariable("zoomBendSec1Height",28.4);
  Control.addVariable("zoomBendSec1XStep",0.0);
  Control.addVariable("zoomBendSec1ZStep",-3.0);

  Control.addVariable("zoomBendSec2Length",129.00);
  Control.addVariable("zoomBendSec2Width",37.0);
  Control.addVariable("zoomBendSec2Height",38.6);
  Control.addVariable("zoomBendSec2XStep",0.0);
  Control.addVariable("zoomBendSec2ZStep",0.0);

  Control.addVariable("zoomBendSec3Length",33.6);  // Numbers from SANS2D
  Control.addVariable("zoomBendSec3Width",30.50);
  Control.addVariable("zoomBendSec3Height",28.0);
  Control.addVariable("zoomBendSec3XStep",0.0);
  Control.addVariable("zoomBendSec3ZStep",-5.75); 

  Control.addVariable("zoomBendSec4Length",46.5);
  Control.addVariable("zoomBendSec4Width",14.0);
  Control.addVariable("zoomBendSec4Height",28.0);
  Control.addVariable("zoomBendSec4XStep",-8.0);
  Control.addVariable("zoomBendSec4ZStep",-9.0);

  // steps in attenuation
  Control.addVariable("zoomBendNAttn",17);
  Control.addVariable("zoomBendAttnZStep",4.0);  // 8 cm back
  Control.addVariable("zoomBendAttn1YPos",29.5);
  Control.addVariable("zoomBendAttn1Dist",4.0);
  Control.addVariable("zoomBendAttn2YPos",46.3);
  Control.addVariable("zoomBendAttn2Dist",4.0);
  Control.addVariable("zoomBendAttn3YPos",62.0);
  Control.addVariable("zoomBendAttn3Dist",7.0);
  Control.addVariable("zoomBendAttn4YPos",90.5);
  Control.addVariable("zoomBendAttn4Dist",6.5);
  Control.addVariable("zoomBendAttn5YPos",108.5);
  Control.addVariable("zoomBendAttn5Dist",4.0);
  Control.addVariable("zoomBendAttn6YPos",120.0);
  Control.addVariable("zoomBendAttn6Dist",4.0);
  Control.addVariable("zoomBendAttn7YPos",130.0);
  Control.addVariable("zoomBendAttn7Dist",4.0);
  Control.addVariable("zoomBendAttn8YPos",147.5);
  Control.addVariable("zoomBendAttn8Dist",4.0);
  Control.addVariable("zoomBendAttn9YPos",162.5);
  Control.addVariable("zoomBendAttn9Dist",4.0);
  Control.addVariable("zoomBendAttn10YPos",177.0);
  Control.addVariable("zoomBendAttn10Dist",4.0);
  Control.addVariable("zoomBendAttn11YPos",195.0);
  Control.addVariable("zoomBendAttn11Dist",4.0);
  Control.addVariable("zoomBendAttn12YPos",206.5);
  Control.addVariable("zoomBendAttn12Dist",4.0);
  Control.addVariable("zoomBendAttn13YPos",216.7);
  Control.addVariable("zoomBendAttn13Dist",4.0);
  Control.addVariable("zoomBendAttn14YPos",234.0);
  Control.addVariable("zoomBendAttn14Dist",6.5);
  Control.addVariable("zoomBendAttn15YPos",263.0);
  Control.addVariable("zoomBendAttn15Dist",7.0);
  Control.addVariable("zoomBendAttn16YPos",284.7);
  Control.addVariable("zoomBendAttn16Dist",4.0);
  Control.addVariable("zoomBendAttn17YPos",295.0);
  Control.addVariable("zoomBendAttn17Dist",4.0);

  Control.addVariable("zoomBendShieldMat","CastIron");     // Zoom outer shield
  Control.addVariable("zoomBendNLayers",8);       // Zoom outer layers
  Control.addVariable("zoomBendMat_3","CastIron"); 
  Control.addVariable("zoomBendMat_4","CastIron"); 
  Control.addVariable("zoomBendMat_6","CastIron"); 
  Control.addVariable("zoomBendMat_7","CastIron"); 

  Control.addVariable("zoomChopperOuterOffset",-700);   
  Control.addVariable("zoomChopperXStep",14.2);   
  Control.addVariable("zoomChopperYStep",0.0);   
  Control.addVariable("zoomChopperZStep",0.0);   
  Control.addVariable("zoomChopperLength",56.6);   // bulk:Coll 468+940
  Control.addVariable("zoomChopperDepth",93.9); 
  Control.addVariable("zoomChopperHeight",102.0); 
  Control.addVariable("zoomChopperLeftWidth",111.8); 
  Control.addVariable("zoomChopperRightWidth",58.2); 
  Control.addVariable("zoomChopperLeftAngle",14.0); 
  Control.addVariable("zoomChopperRightAngle",4.4); 
  Control.addVariable("zoomChopperLeftWaxAngle",14.0); 
  Control.addVariable("zoomChopperRightWaxAngle",4.4);
  Control.addVariable("zoomChopperWaxLeftSkin",12.0);
  Control.addVariable("zoomChopperWaxRightSkin",12.0); 

  Control.addVariable("zoomChopperNShield",8); 
  Control.addVariable("zoomChopperShieldMat_0","Tungsten");      // W
  Control.addVariable("zoomChopperShieldMat_1","Stainless304");
  Control.addVariable("zoomChopperShieldMat_2","B-Poly");
  Control.addVariable("zoomChopperShieldMat_3","Aluminium");
  Control.addVariable("zoomChopperShieldMat_4","Stainless304");
  Control.addVariable("zoomChopperShieldMat_5","Aluminium");
  Control.addVariable("zoomChopperShieldMat_6","B-Poly");
  Control.addVariable("zoomChopperShieldMat_7","Tungsten");      // W
  Control.addVariable("zoomChopperShieldLen_1",-2.0);      //-ve is length
  Control.addVariable("zoomChopperShieldLen_2",-7.5);      //-ve is length
  Control.addVariable("zoomChopperShieldLen_3",-3.0);      //-ve is length
  Control.addVariable("zoomChopperShieldLen_4",-2.0);      // not frac
  Control.addVariable("zoomChopperShieldLen_5",-7.5);      //
  Control.addVariable("zoomChopperShieldLen_6",-3.0);      //-ve is length
  Control.addVariable("zoomChopperShieldLen_7",-2.0);      //-ve is length
  Control.addVariable("zoomChopperShieldLen_8",-12.0);      //-ve is length

  // First item
  Control.addVariable("zoomChopperVoidChanLeft",35.80);  // from 27
  Control.addVariable("zoomChopperVoidChanRight",12.20); 
  Control.addVariable("zoomChopperVoidChanUp",21.0);    // GUESS
  Control.addVariable("zoomChopperVoidChanDown",24.0);  // GUESS

  Control.addVariable("zoomChopperVoidLeftWidth",67.3);   // bulk:Coll 468+940
  Control.addVariable("zoomChopperVoidRightWidth",12.2);  
  Control.addVariable("zoomChopperVoidHeight",34.25);  
  Control.addVariable("zoomChopperVoidDepth",48.00); 
  Control.addVariable("zoomChopperVoidCut",32.20);
  
  // third item
  Control.addVariable("zoomChopperVoidEnd",30.5); 
  Control.addVariable("zoomChopperVoidEndLeft",67.30);  
  Control.addVariable("zoomChopperVoidEndRight",12.20+15.0); 
  Control.addVariable("zoomChopperVoidEndUp",21.0);
  Control.addVariable("zoomChopperVoidEndDown",22.0);     // GUESS

  Control.addVariable("zoomChopperVoidEndMat","Stainless304");   //
  //  Control.addVariable("zoomChopperVoidEndMat",0);   // 
  Control.addVariable("zoomChopperVoidEndBeamLeft",3.0);  
  Control.addVariable("zoomChopperVoidEndBeamRight",3.0); 
  Control.addVariable("zoomChopperVoidEndBeamUp",2.0);
  Control.addVariable("zoomChopperVoidEndBeamDown",2.0);     

  Control.addVariable("zoomChopperWallMat","CastIron"); 
  Control.addVariable("zoomChopperWaxMat","Poly");  


  Control.addVariable("zoomDiskXStep",-25.0);
  Control.addVariable("zoomDiskYStep",4.0);
  Control.addVariable("zoomDiskZStep",0.0);
  Control.addVariable("zoomDiskXYangle",0.0);
  Control.addVariable("zoomDiskZangle",0.0);

  Control.addVariable("zoomDiskGap",3.0);
  Control.addVariable("zoomDiskInnerRadius",3.0);
  Control.addVariable("zoomDiskOuterRadius",20.0);
  Control.addVariable("zoomDiskNDisk",2);

  Control.addVariable("zoomDisk0Thick",4.0);
  Control.addVariable("zoomDisk1Thick",4.0);
  Control.addVariable("zoomDiskInnerMat","Aluminium");
  Control.addVariable("zoomDiskOuterMat","Inconnel");

  Control.addVariable("zoomDisk0NBlades",1);
  Control.addVariable("zoomDisk1NBlades",1);
  Control.addVariable("zoomDisk0PhaseAngle0",95.0);
  Control.addVariable("zoomDisk0OpenAngle0",30.0);
  Control.addVariable("zoomDisk1PhaseAngle0",95.0);
  Control.addVariable("zoomDisk1OpenAngle0",30.0);
  
  
  Control.addVariable("zoomRoofNSteel",5);     // -ve away to target
  Control.addVariable("zoomRoofNExtra",6);     // -ve away to target
  Control.addVariable("zoomRoofStep1",0.0);     // -ve away to target
  Control.addVariable("zoomRoofStep2",150.0);     //  GUESS
  Control.addVariable("zoomRoofStep3",533.5);     // 
  Control.addVariable("zoomRoofStep4",571.4);     // 
  Control.addVariable("zoomRoofStep5",608.4);     // 
  Control.addVariable("zoomRoofHeight1",8.5);    // 
  Control.addVariable("zoomRoofHeight2",37.5);   // 
  Control.addVariable("zoomRoofHeight3",59.1);   // GUESS
  Control.addVariable("zoomRoofHeight4",37.5);   // GUESS
  Control.addVariable("zoomRoofHeight5",15.9);   // GUESS


  Control.addVariable("zoomRoofEStep1",-10.4);     // -ve away to target
  Control.addVariable("zoomRoofEStep2",150.0-7.0);     //  GUESS
  Control.addVariable("zoomRoofEStep3",478.4+105.0);     // 
  Control.addVariable("zoomRoofEStep4",478.4+140.0);     // 
  Control.addVariable("zoomRoofEStep5",478.4+175.0);     // 
  Control.addVariable("zoomRoofEStep6",478.4+210.0);     // 
  Control.addVariable("zoomRoofEHeight1",25.0+8.5);      // 
  Control.addVariable("zoomRoofEHeight2",30.5+37.5);     // 
  Control.addVariable("zoomRoofEHeight3",24.8+59.1);
  Control.addVariable("zoomRoofEHeight4",0.8+59.1);
  Control.addVariable("zoomRoofEHeight5",-23.2+59.1);
  Control.addVariable("zoomRoofEHeight6",-47.2+59.1);

  Control.addVariable("zoomRoofMat","CastIron");          // cast steel
  Control.addVariable("zoomRoofExtraMat","B-Poly");     // borated poly

  Control.addVariable("zoomCollimatorXStep",-25.0);   
  Control.addVariable("zoomCollimatorZStep",-12.5);   
  Control.addVariable("zoomCollimatorLength",425.8);   
  Control.addVariable("zoomCollimatorDepth",103.9); 
  Control.addVariable("zoomCollimatorHeight",102.0);
  
  Control.addVariable("zoomCollimatorLeftWidth",132.05); 
  Control.addVariable("zoomCollimatorRightWidth",100.2); 
  Control.addVariable("zoomCollimatorLeftInnerWidth",84.05); 
  Control.addVariable("zoomCollimatorRightInnerWidth",82.2); 
  Control.addVariable("zoomCollimatorLeftInnerAngle",14.0); 
  Control.addVariable("zoomCollimatorRightInnerAngle",4.4);
  Control.addVariable("zoomCollimatorLeftWaxSkin",12.0);
  Control.addVariable("zoomCollimatorRightWaxSkin",12.0); 
  Control.addVariable("zoomCollimatorFeMat","CastIron");         
  Control.addVariable("zoomCollimatorWaxMat","Poly");         // Cast steel


  Control.addVariable("zoomCollInsertNItem",1);
  Control.addVariable("zoomCollInsertXStep",0.0);
  Control.addVariable("zoomCollInsertYStep",3.0);
  Control.addVariable("zoomCollInsertZStep",0.0);
  Control.addVariable("zoomCollInsertIndex",0);
  Control.addVariable("zoomCollInsertLength",415.0); 
  Control.addVariable("zoomCollInsertWidth",80.0);  
  Control.addVariable("zoomCollInsertHeight",20.0);  
  Control.addVariable("zoomCollInsertWallThick",2.0);  
  Control.addVariable("zoomCollInsertWindowThick",0.4);
  Control.addVariable("zoomCollInsertWindowMat","Aluminium");
  Control.addVariable("zoomCollInsertWallMat","Aluminium");

  Control.addVariable("zoomCollInsertGuideOffset",2.0); 
  Control.addVariable("zoomCollInsertGuideSep",7.0);
  Control.addVariable("zoomCollInsertGuideThick",0.5);
  Control.addVariable("zoomCollInsertGuideWidth",3.0);
  Control.addVariable("zoomCollInsertGuideHeight",2.0);
  Control.addVariable("zoomCollInsertGuideMat","SiO2"); 
 
  Control.addVariable("zoomCollimatorStackXShift",10.0);  // GUESS
  Control.addVariable("zoomCollimatorStackZShift",0.0);  // GUESS
  Control.addVariable("zoomCollimatorStackHeight",25.6);  // GUESS
  Control.addVariable("zoomCollimatorStackWidth",125.6);  // GUESS

  Control.addVariable("zoomCollimatorNLayers",0); 

  Control.addVariable("zoomColStackLength",400.0);   
  Control.addVariable("zoomColStackHeight",20.1); 
  Control.addVariable("zoomColStackNItem",3);   
  Control.addVariable("zoomColStackIndex",0);         /// View index   

  Control.addVariable("zoomColStackWidth0",11.5);    // GUESS
  Control.addVariable("zoomColStackStepDist0",200.0);       // GUESS
  Control.addVariable("zoomColStackStepLift0",2.1);       // GUESS
  Control.addVariable("zoomColStackClear0",0.5);       // GUESS
  Control.addVariable("zoomColStackPathWidth0",3.0);       // GUESS
  Control.addVariable("zoomColStackPathHeight0",3.0);       // GUESS
  Control.addVariable("zoomColStackMat0","Stainless304");          // 

  Control.addVariable("zoomColStackWidth1",11.5);    // GUESS
  // END DEPRECIATED
  
  Control.addVariable("zoomPrimaryLength",425.8);   
  Control.addVariable("zoomPrimaryDepth",93.9); 
  Control.addVariable("zoomPrimaryHeight",102.0); 
  Control.addVariable("zoomPrimaryLeftWidth",90.3); 
  Control.addVariable("zoomPrimaryRightWidth",90.3); 
  Control.addVariable("zoomPrimaryFeMat","CastIron"); 
  Control.addVariable("zoomPrimaryCutX",0.0);
  Control.addVariable("zoomPrimaryCutZ",0.0);
  Control.addVariable("zoomPrimaryCutWidth",25.0); 
  Control.addVariable("zoomPrimaryCutHeight",25.6); 
  Control.addVariable("zoomPrimaryNLayers",3); 
  Control.addVariable("zoomPrimaryFrac_1",-5.0); 
  Control.addVariable("zoomPrimaryFrac_2",-15.0); 
  Control.addVariable("zoomPrimaryMat_1","CastIron"); 
  Control.addVariable("zoomPrimaryMat_2","Concrete");   /// Concrete

  Control.addVariable("zoomGuideLength",400.0);     // Zoom box length
  Control.addVariable("zoomGuideAUp",1.5);          // Zoom box inner 
  Control.addVariable("zoomGuideADown",1.50);        // Zoom box inner 
  Control.addVariable("zoomGuideALeft",1.50);      
  Control.addVariable("zoomGuideARight",1.50);      
  Control.addVariable("zoomGuideBUp",3.0);          // Zoom box inner 
  Control.addVariable("zoomGuideBDown",3.0);        // Zoom box inner 
  Control.addVariable("zoomGuideBLeft",3.0);      
  Control.addVariable("zoomGuideBRight",3.0);      
  Control.addVariable("zoomGuideMidUp",12.5);          // Zoom guide mid
  Control.addVariable("zoomGuideMidDown",12.5);        // Zoom guide mid 
  Control.addVariable("zoomGuideMidLeft",12.5);      
  Control.addVariable("zoomGuideMidRight",12.5);      
  Control.addVariable("zoomGuideRoof",90.0);         
  Control.addVariable("zoomGuideFloor",90.0);       
  Control.addVariable("zoomGuideWallLeft",90.0);    
  Control.addVariable("zoomGuideWallRight",90.0);  
  Control.addVariable("zoomGuideInnerCollMat","CastIron");    
  Control.addVariable("zoomGuideShieldMat","CastIron");    
  Control.addVariable("zoomGuideNInsert",-1);      // Guide inner layers
  Control.addVariable("zoomGuideICutterLen",13.5); // Inner guide unit
  Control.addVariable("zoomGuideIFocusLen",2.5);   // Inner focus unit
  Control.addVariable("zoomGuideInnerMat_8","B4C");   // Guide inner second
  Control.addVariable("zoomGuideInnerMat_24","B4C");  // Guide inner sector

  Control.addVariable("zoomGuideNLayers",2);          // Guide outer layers
  Control.addVariable("zoomGuideShieldFrac_1",(0.475/0.775)); 
  Control.addVariable("zoomGuideShieldMat_1","Concrete");  // Concrete

  Control.addVariable("zoomShutterColletHGap",0.6);   //   clearance gap
  Control.addVariable("zoomShutterColletVGap",0.6);   //   clearance gap
  Control.addVariable("zoomShutterColletFGap",0.6);   //   clearance gap
  Control.addVariable("zoomShutterColletMat","CastIron");      //   surround material  

  Control.addVariable("zoomShutterXStart",0.0);        // Start Z position
  Control.addVariable("zoomShutterZStart",3.0);        // Start Z position
  Control.addVariable("zoomShutterGuideXAngle",0.0);   // rotation in mRad
  Control.addVariable("zoomShutterGuideZAngle",-24.0);  // drop in mRad

  const int steelNumber(IParam.getValue<int>("zoomShutterGN"));
  const int b4cNumber(3);
  const double steelWidthA(8.075);
  const double steelWidthB(10.50);
  const double steelHeightA(7.075);
  const double steelHeightB(8.1);

  Control.addVariable("zoomShutterBlock1FStep",0.1);    //   Inner blocks
  Control.addVariable("zoomShutterBlock1CentX",0.0);    //   Inner blocks
  Control.addVariable("zoomShutterBlock1CentZ",0.0);    //   Inner blocks
  Control.addVariable("zoomShutterBlock1Len",3.0);      //   Inner blocks
  Control.addVariable("zoomShutterBlock1Width",steelWidthA);  //   Inner blocks
  Control.addVariable("zoomShutterBlock1Height",steelHeightA); //   Height
  Control.addVariable("zoomShutterBlock1VGap",3.0);     //   Height
  Control.addVariable("zoomShutterBlock1HGap",3.0);     //   Hor. Gap (full)
  Control.addVariable("zoomShutterBlock1Mat","Void");        //   Spacer


  // ts1System::shutterVar zoomBlock("zoom");
  // zoomBlock.setSteelNumber(steelNumber);
  // zoomBlock.buildVar(Control,10,-1.75,0.0

  int blockIndex(2);  
  const double steelLen(22.4/steelNumber);
  // B4C scrapper #1 
  B4Cspacer(Control,blockIndex,-1.75,0.0);
  SteelBlock(Control,blockIndex+b4cNumber,-2.125,
	     steelLen,steelWidthA,steelHeightA,
	     4.2,5.5,steelNumber);
  blockIndex+=steelNumber+b4cNumber;

  // B4C scrapper #2 
  B4Cspacer(Control,blockIndex,-2.01,0.0);  
  SteelBlock(Control,blockIndex+b4cNumber,-2.125,
	     steelLen,steelWidthA,steelHeightA,4.2,5.5,steelNumber);
  blockIndex+=steelNumber+b4cNumber;

  // B4C scrapper #3
  B4Cspacer(Control,blockIndex,-2.21,0.0);
  SteelBlock(Control,blockIndex+b4cNumber,-2.125,
	     steelLen,steelWidthA,steelHeightA,4.2,5.5,steelNumber);
  blockIndex+=steelNumber+b4cNumber;

  // B4C scrapper #4
  B4Cspacer(Control,blockIndex,-2.50,0.0);
  SteelBlock(Control,blockIndex+b4cNumber,-2.125,
	     steelLen,steelWidthA,steelHeightA,4.2,5.5,steelNumber);
  blockIndex+=steelNumber+b4cNumber;

  // B4C scrapper #5
  B4Cspacer(Control,blockIndex,-2.74,0.0);
  SteelBlock(Control,blockIndex+b4cNumber,-2.9,
	     steelLen,steelWidthB,steelHeightB,4.2,6.0,steelNumber);
  blockIndex+=steelNumber+b4cNumber;

  // B4C scrapper #6
  B4Cspacer(Control,blockIndex,-3.08,0.0);
  SteelBlock(Control,blockIndex+b4cNumber,-2.9,
	     steelLen,steelWidthB,steelHeightB,4.2,6.0,steelNumber);
  blockIndex+=steelNumber+b4cNumber;

  // B4C scrapper #7
  B4Cspacer(Control,blockIndex,-3.375,0.0);
  SteelBlock(Control,blockIndex+b4cNumber,-2.9,
	     steelLen,steelWidthB,steelHeightB,4.2,6.0,steelNumber);
  blockIndex+=steelNumber+b4cNumber;

  // B4C scrapper #8
  B4Cspacer(Control,blockIndex,-3.66,0.0);
  SteelBlock(Control,blockIndex+b4cNumber,-2.9,
	     steelLen,steelWidthB,steelHeightB,4.2,6.0,steelNumber);
  blockIndex+=steelNumber+b4cNumber;

  // B4C scrapper #9
  B4Cspacer(Control,blockIndex,-3.95,0.0);
  blockIndex+=b4cNumber;

  const std::string finalBlock=
    StrFunc::makeString(std::string("zoomShutterBlock"),blockIndex);
  Control.addVariable("zoomShutterNBlocks",blockIndex);     //   Inner blocks


  Control.addVariable(finalBlock+"CentX",0.0);  //   Inner blocks
  Control.addVariable(finalBlock+"CentZ",0.0);  //   Inner blocks
  Control.addVariable(finalBlock+"Len",3.4);    //   Inner blocks
  Control.addVariable(finalBlock+"VGap",4.0);   //   Height
  Control.addVariable(finalBlock+"HGap",4.0);   //   Hor. Gap (full)
  Control.addVariable(finalBlock+"Mat","Void");      //   Spacer

  Control.addVariable("zoomSampleLength",200.0);     // Zoom box length
  Control.addVariable("zoomSampleUp",50.0);          // Zoom box inner 
  Control.addVariable("zoomSampleDown",50.0);        // Zoom box inner 
  Control.addVariable("zoomSampleLeft",50.0);        
  Control.addVariable("zoomSampleRight",50.0);       
  Control.addVariable("zoomSampleRoof",75.0);         
  Control.addVariable("zoomSampleFloor",75.0);       
  Control.addVariable("zoomSampleWallLeft",75.0);    
  Control.addVariable("zoomSampleWallRight",75.0);  
  Control.addVariable("zoomSampleShieldMat","Concrete");    
  Control.addVariable("zoomSampleNLayers",10);        // Box inner layers


  Control.addVariable("zoomTankXStep",0.0);          // Step accross beam
  Control.addVariable("zoomTankYStep",300.0);
  Control.addVariable("zoomTankZStep",0.0);
  Control.addVariable("zoomTankXYangle",0.0);
  Control.addVariable("zoomTankZangle",0.0);

  Control.addVariable("zoomTankNCylinder",3);
  Control.addVariable("zoomTankRadius1",15.0);
  Control.addVariable("zoomTankRadius2",25.0);
  Control.addVariable("zoomTankRadius3",45.0);
  Control.addVariable("zoomTankCDepth1",142.5/3);
  Control.addVariable("zoomTankCDepth2",142.5/3);
  Control.addVariable("zoomTankCDepth3",142.5/3);
  Control.addVariable("zoomTankCXStep1",0.0);
  Control.addVariable("zoomTankCXStep2",2.0);
  Control.addVariable("zoomTankCXStep3",5.0);
  Control.addVariable("zoomTankCZStep1",0.0);
  Control.addVariable("zoomTankCZStep2",0.0);
  Control.addVariable("zoomTankCZStep3",0.0);
  Control.addVariable("zoomTankCylThick",1.0);
  Control.addVariable("zoomTankWindowThick",0.5);
  Control.addVariable("zoomTankWindowRadius",12.0);
  Control.addVariable("zoomTankWallThick",1.0);
  Control.addVariable("zoomTankWidth",193.0);
  Control.addVariable("zoomTankLength",1100.0);
  Control.addVariable("zoomTankHeight",273.0);
  Control.addVariable("zoomTankWallMat","Stainless304");
  Control.addVariable("zoomTankWindowMat","SiCrystal");

  Control.addVariable("zoomHutchXStep",-25.0);          ///< Step accross beam
  Control.addVariable("zoomHutchYStep",0.0);
  Control.addVariable("zoomHutchZStep",0.0);
  Control.addVariable("zoomHutchXYangle",0.0);          ///< Step accross beam
  Control.addVariable("zoomHutchZangle",0.0);

  Control.addVariable("zoomHutchFrontLeftWidth",122.5);
  Control.addVariable("zoomHutchFrontRightWidth",137.5);
  Control.addVariable("zoomHutchMidLeftWidth",147.5);     // GUESS
  Control.addVariable("zoomHutchMidRightWidth",172.5);
  Control.addVariable("zoomHutchBackLeftWidth",147.5);    // GUESS
  Control.addVariable("zoomHutchBackRightWidth",273.5);   // 421.0-BLW [Guess]
  Control.addVariable("zoomHutchFrontLeftLen",913.5);
  Control.addVariable("zoomHutchFrontRightLen",214.0);
  Control.addVariable("zoomHutchMidLeftLen",1373.5);
  Control.addVariable("zoomHutchMidRightLen",1373.5);
  Control.addVariable("zoomHutchFullLen",2119.0);
  Control.addVariable("zoomHutchRoof",230.5);          // from ChipIR
  Control.addVariable("zoomHutchFloor",280.5);
  Control.addVariable("zoomHutchWallThick",30.5);      // Wall thickness 
  Control.addVariable("zoomHutchRoofThick",50.0);      // Roof thickness 
  Control.addVariable("zoomHutchFloorThick",30.5);      // Floor thickness 
  Control.addVariable("zoomHutchWallMat","Concrete");          // Concrete
  Control.addVariable("zoomHutchRoofMat","Concrete");          // Concrete
  Control.addVariable("zoomHutchFloorMat","Concrete");          // Concrete

  Control.addVariable("zoomHutchPortRadius",4.5);      // radius
  
  return;
}

double
B4Cspacer(FuncDataBase& Control,const int index,
	  const double CX,const double CZ)
  /*!
    Function to do the populate of the shutter blockxs
    \param Control :: Func database
    \param index :: index
    \param CX :: Displacement in CX
    \param CZ :: Displacement in CZ
    \return Length of this component
  */
{
  boost::format FMT("zoomShutterBlock%d%s");
  const std::string b4cMat("B4C");

  // B4C scrapper #1 
  Control.addVariable((FMT % index % "CentX").str(),CX);
  Control.addVariable((FMT % index % "CentZ").str(),CZ);
  Control.addVariable((FMT % index % "Len").str(),0.635);
  Control.addVariable((FMT % index % "VGap").str(),5.0);
  Control.addVariable((FMT % index % "HGap").str(),3.5);
  Control.addVariable((FMT % index % "Mat").str(),b4cMat);
  Control.addVariable((FMT % (index+1) % "Mat").str(),"Void");
  Control.addVariable((FMT % (index+1) % "Len").str(),0.2);
  Control.addVariable((FMT % (index+2) % "Mat").str(),b4cMat);
  Control.addVariable((FMT % (index+2) % "Len").str(),0.635);
  Control.addVariable((FMT % (index+2) % "VGap").str(),3.5);
  Control.addVariable((FMT % (index+2) % "HGap").str(),5.0);
  
  return 2*0.635+0.2;
}

double
SteelBlock(FuncDataBase& Control,const int index,const double CX,
	   const double Len,const double HSize,const double VSize,
	   const double VG,const double HG,
	   const int extra)
  /*!
    Construct a steel block for the Zoom beamline
    \param Control :: Database item
    \param index :: Index nubmer 
    \param CX :: Centre offset distance
    \param Len :: Length
    \param Size :: Height/Width
    \param VG :: Vertical gap
    \param HG :: Horrizontal gap
    \param extra :: Filling space for Material ==3 
   */
{
  boost::format FMT("zoomShutterBlock%d%s");
  Control.addVariable((FMT % index % "CentX").str(),CX);
  Control.addVariable((FMT % index % "Len").str(),Len);
  Control.addVariable((FMT % index % "Width").str(),HSize);
  Control.addVariable((FMT % index % "Height").str(),VSize);
  Control.addVariable((FMT % index % "VGap").str(),VG);
  Control.addVariable((FMT % index % "HGap").str(),HG);
  Control.addVariable((FMT % index % "Mat").str(),"Stainless304");
  for(int i=0;i<extra;i++)
    Control.addVariable((FMT % (index+i) % "Mat").str(),"Stainless304");
  return Len*extra;
}

}  // NAMESPACE setVariables
