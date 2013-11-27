/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   t1Build/t1BaseVariables.cxx
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#include "shutterVar.h"
#include "variableSetup.h"

namespace setVariable
{

void
TS1PlateTarget(FuncDataBase& Control)
  /*!
    Set up a Cylindrical target
    \param Control :: DataBase
   */
{
  ELog::RegMethod RegA("t1BaseVariables[F]","TS1PlateTarget");

  // Pressure vessel
  Control.addVariable("PVesselXStep",0.0);           // X Step
  Control.addVariable("PVesselYStep",0.0);           // Y Step
  Control.addVariable("PVesselZStep",0.0);           // Z step
  Control.addVariable("PVesselXYangle",0.0);         // Rotation about Z
  Control.addVariable("PVesselZangle",0.0);          // Rotation about X

  Control.addVariable("PVesselWidth",13.03);         // Half width
  Control.addVariable("PVesselHeight",7.10);         // Half height
  // Note add void length here:
  //  Control.addVariable("PVesselLength",54.06);     
  Control.addVariable("PVesselLength",17.51);        // Target[36.55] + 17.51
  Control.addVariable("PVesselFront",4.2);           // Forward step Org
  Control.addVariable("PVesselTopWallThick",2.70);   // Thickness (Z)
  Control.addVariable("PVesselSideWallThick",2.40);  // Thickness (X/Y)
  Control.addVariable("PVesselFrontWallThick",4.20); // Thickness (X/Y)
  Control.addVariable("PVesselWallMat","Stainless304");           // Steel 304
  Control.addVariable("PVesselTaMat","Tantalum");            // Ta material
  Control.addVariable("PVesselWaterMat","D2O");      // D2O  [if 88 then 80% D2O + 20% H2O (by VOLUME)]  

  Control.addVariable("PVesselCornerCutX",6.87);          
  Control.addVariable("PVesselCornerCutY",2.65);          

  Control.addVariable("ProtonVoidViewRadius",4.0);  
  Control.addVariable("PVesselViewSteelRadius",4.6); 
  Control.addVariable("PVesselViewFrontThick",0.3);                        

  Control.addVariable("PVesselEndYStep1",6.26);           // check this !!!     #    
  Control.addVariable("PVesselEndXOutSize1",37.0);          
  Control.addVariable("PVesselEndZOutSize1",24.0); 
  Control.addVariable("PVesselEndYStep2",2.56);           // check this !!!         
  Control.addVariable("PVesselEndXOutSize2",37.0);          
  Control.addVariable("PVesselEndZOutSize2",26.0);     
  Control.addVariable("PVesselSteelBulkThick",13.94); 
  Control.addVariable("PVesselSteelBulkRadius",23.0); 

  Control.addVariable("PVesselWatInThick",5.96); 
  Control.addVariable("PVesselWatInRad",1.25); 


  Control.addVariable("PVesselNBwch",6);          

  Control.addVariable("PVesselBegXstep0",-7.68);
  Control.addVariable("PVesselBegXstep1",7.68);   
  Control.addVariable("PVesselBegXstep2",-8.98);
  Control.addVariable("PVesselBegXstep3",8.98); 
  Control.addVariable("PVesselBegXstep4",-10.28);      
  Control.addVariable("PVesselBegXstep5",10.28);
   
  Control.addVariable("PVesselEndXstep0",-4.06);   
  Control.addVariable("PVesselEndXstep1",4.06);
  Control.addVariable("PVesselEndXstep2",-9.10);
  Control.addVariable("PVesselEndXstep3",9.10);
  Control.addVariable("PVesselEndXstep4",-14.14);               
  Control.addVariable("PVesselEndXstep5",14.14); 
                                             
  Control.addVariable("PVesselBigWchbegThick",0.94);
  Control.addVariable("PVesselBigWchbegZstep",0.0);
  Control.addVariable("PVesselBigWchbegHeight",8.80); 
  Control.addVariable("PVesselBigWchbegWidth",0.7);
  Control.addVariable("PVesselBigWchendThick",2.0);
  Control.addVariable("PVesselBigWchendZstep",2.40);
  Control.addVariable("PVesselBigWchendHeight",4.0); 
  Control.addVariable("PVesselBigWchendWidth",4.20);
                            

  Control.addVariable("PVesselWinHouseOutRad",5.40);         
  Control.addVariable("PVesselWinHouseThick",3.0);           


  // Pressure vessel water channels:

  Control.addVariable("PVesselNChannel",8);          
  Control.addVariable("PVesselChannel0Width",1.23);          
  Control.addVariable("PVesselChannel0Height",0.25);          
  Control.addVariable("PVesselChannel0XOffset",8.08);          
  Control.addVariable("PVesselChannel0ZOffset",5.65);          
  Control.addVariable("PVesselChannel0InStep",0.83);
  Control.addVariable("PVesselChannel0InDepth",0.75);                   
  Control.addVariable("PVesselChannel0MidGap",0.75);          
  Control.addVariable("PVesselChannel0Mat",31);      

  Control.addVariable("PVesselChannel1Width",1.23);          
  Control.addVariable("PVesselChannel1Height",0.25);          
  Control.addVariable("PVesselChannel1XOffset",-8.08);          
  Control.addVariable("PVesselChannel1ZOffset",5.65);          
  Control.addVariable("PVesselChannel1InStep",0.83);
  Control.addVariable("PVesselChannel1InDepth",0.75);                             
  Control.addVariable("PVesselChannel1MidGap",0.75);          
  Control.addVariable("PVesselChannel1Mat",31);  

  Control.addVariable("PVesselChannel2Width",1.23);          
  Control.addVariable("PVesselChannel2Height",0.25);          
  Control.addVariable("PVesselChannel2XOffset",8.08);          
  Control.addVariable("PVesselChannel2ZOffset",-5.65);          
  Control.addVariable("PVesselChannel2InStep",0.83);
  Control.addVariable("PVesselChannel2InDepth",0.75);                             
  Control.addVariable("PVesselChannel2MidGap",0.75);          
  Control.addVariable("PVesselChannel2Mat",31);       

  Control.addVariable("PVesselChannel3Width",1.23);          
  Control.addVariable("PVesselChannel3Height",0.25);          
  Control.addVariable("PVesselChannel3XOffset",-8.08);          
  Control.addVariable("PVesselChannel3ZOffset",-5.65);          
  Control.addVariable("PVesselChannel3InStep",0.83);
  Control.addVariable("PVesselChannel3InDepth",0.75);                             
  Control.addVariable("PVesselChannel3MidGap",0.75);          
  Control.addVariable("PVesselChannel3Mat",31);                 

  Control.addVariable("PVesselChannel4Width",0.96);          
  Control.addVariable("PVesselChannel4Height",0.25);          
  Control.addVariable("PVesselChannel4XOffset",10.87);          
  Control.addVariable("PVesselChannel4ZOffset",5.65);          
  Control.addVariable("PVesselChannel4InStep",0.83);
  Control.addVariable("PVesselChannel4InDepth",0.75);                             
  Control.addVariable("PVesselChannel4MidGap",0.75);          
  Control.addVariable("PVesselChannel4Mat",31);      

  Control.addVariable("PVesselChannel5Width",0.96);          
  Control.addVariable("PVesselChannel5Height",0.25);          
  Control.addVariable("PVesselChannel5XOffset",-10.87);          
  Control.addVariable("PVesselChannel5ZOffset",5.65);          
  Control.addVariable("PVesselChannel5InStep",0.83);
  Control.addVariable("PVesselChannel5InDepth",0.75);                             
  Control.addVariable("PVesselChannel5MidGap",0.75);          
  Control.addVariable("PVesselChannel5Mat",31);  

  Control.addVariable("PVesselChannel6Width",0.96);          
  Control.addVariable("PVesselChannel6Height",0.25);          
  Control.addVariable("PVesselChannel6XOffset",10.87);          
  Control.addVariable("PVesselChannel6ZOffset",-5.65);          
  Control.addVariable("PVesselChannel6InStep",0.83);
  Control.addVariable("PVesselChannel6InDepth",0.75);                             
  Control.addVariable("PVesselChannel6MidGap",0.75);          
  Control.addVariable("PVesselChannel6Mat",31);          

  Control.addVariable("PVesselChannel7Width",0.96);          
  Control.addVariable("PVesselChannel7Height",0.25);          
  Control.addVariable("PVesselChannel7XOffset",-10.87);          
  Control.addVariable("PVesselChannel7ZOffset",-5.65);          
  Control.addVariable("PVesselChannel7InStep",0.83);
  Control.addVariable("PVesselChannel7InDepth",0.75);                             
  Control.addVariable("PVesselChannel7MidGap",0.75);          
  Control.addVariable("PVesselChannel7Mat",31);     

  Control.addVariable("PVesselSideZCenter",5.40);         // check this
  Control.addVariable("PVesselSideXOffset",11.38);         // Offset
  Control.addVariable("PVesselSideHeight",2.325);         //
  Control.addVariable("PVesselSideWidth",0.45);         //  

  // TARGET PLATES:
  Control.addVariable("TPlateNBlocks",12);         // Number of plates:
  Control.addVariable("TPlateWidth",5.40);         // Width (half)
  Control.addVariable("TPlateHeight",5.90);        // Height (half)
  Control.addVariable("TPlateWaterHeight",4.40);   // Heigth (half) [water]
  Control.addVariable("TPlateWWidth",5.25);        // Tungsten Width (half) 
  Control.addVariable("TPlateWHeight",4.00);       // Tungsten Height (half) 

  Control.addVariable("TPlateBPlateThick",2.2);    // Backplate thickness
  Control.addVariable("TPlateBPlateIThick",0.6);    // Backplate thickness
  Control.addVariable("TPlateBPlateIWidth",1.1);    // Backplate thickness
  Control.addVariable("TPlateBPlateIOffset",2.95);    // Backplate thickness
  Control.addVariable("TPlateBPlatePinRadius",0.4);    // Backplate thickness
  
  Control.addVariable("TPlateFeMat",3);            // Steel
  Control.addVariable("TPlateWMat",38);            // Tungsten 
  Control.addVariable("TPlateWaterMat",31);        // Water [D2O]
  Control.addVariable("TPlateTaMat",32);           // Ta material 

  Control.addVariable("TPlateWaterThick",0.2);    // water thick
  Control.addVariable("TPlateTaThick",0.2);       // Ta thickness

  Control.addVariable("TPlateThick0",1.1);         // Tungsten 
  Control.addVariable("TPlateThick1",1.1);         // Tungsten 
  Control.addVariable("TPlateThick2",1.2);         // Tungsten 
  Control.addVariable("TPlateThick3",1.35);         // Tungsten 
  Control.addVariable("TPlateThick4",1.5);         // Tungsten 
  Control.addVariable("TPlateBlockType5",0);         // Void
  Control.addVariable("TPlateThick5",5.0);         // Tungsten 
  //  Control.addVariable("TPlateThick5",1.8);         // Tungsten 
  Control.addVariable("TPlateThick6",2.1);         // Tungsten 
  Control.addVariable("TPlateThick7",2.6);         // Tungsten 
  Control.addVariable("TPlateThick8",3.4);         // Tungsten 
  Control.addVariable("TPlateThick9",4.0);         // Tungsten 
  Control.addVariable("TPlateThick10",4.6);         // Tungsten 
  Control.addVariable("TPlateThick11",4.6);         // Tungsten 

  Control.addVariable("TPlateFeedXOffset",2.35);         // Offset
  Control.addVariable("TPlateFeedHeight",0.25);         // Offset
  Control.addVariable("TPlateFeedWidth",1.825);         // Offset



  // Water Dividers
  Control.addVariable("WDivideConnectHeight",8.8);     

  Control.addVariable("WDivideFBlkConnect",3);     
  Control.addVariable("WDivideFBlkSize",1.45);     
  Control.addVariable("WDivideFBlkSndStep",4.63); 
  Control.addVariable("WDivideFBlkSndOut",4.23);          
  Control.addVariable("WDivideFBlkSndWidth",0.6);     
  Control.addVariable("WDivideFBlkSndLen",1.0);     
  Control.addVariable("WDivideFBlkWallThick",0.2);     
  Control.addVariable("WDivideFBlkEndLen",2.04);     
  Control.addVariable("WDivideFBlkEndThick",0.6);     

  Control.addVariable("WDivideMBlkConnect",7);     
  Control.addVariable("WDivideMBlkSize",3.23);       // step from targe
  Control.addVariable("WDivideMBlkOutRad",1.20);     // Outer radius    
  Control.addVariable("WDivideMBlkInRad",1.50);      // Inner radius
  Control.addVariable("WDivideMBlkExtLen",2.05);     
  Control.addVariable("WDivideMBlkExtWidth",0.6);     
  Control.addVariable("WDivideMBlkWallThick",0.2);     
  Control.addVariable("WDivideMBlkEndLen",2.04);     
  Control.addVariable("WDivideMBlkEndThick",0.6);     

  Control.addVariable("WDivideBBlkConnect",12);     
  Control.addVariable("WDivideBBlkSize",1.93);       // step from target
  Control.addVariable("WDivideBBlkOutRad",0.80);     // Outer radius    
  Control.addVariable("WDivideBBlkInRad",0.80);      // Inner radius
  Control.addVariable("WDivideBBlkExtLen",1.00);     
  Control.addVariable("WDivideBBlkExtWidth",0.7);     
  Control.addVariable("WDivideBBlkWallThick",0.2);     
  Control.addVariable("WDivideBBlkEndLen",2.04);     
  Control.addVariable("WDivideBBlkEndThick",0.7);     

  Control.addVariable("WDivideInsOutThick",10.90);     
  Control.addVariable("WDivideInsInThick",9.40);       
  Control.addVariable("WDivideInsWidth",4.00);     
  Control.addVariable("WDivideInsHeight",5.00);
  Control.addVariable("WDivideInsRad",1.25);              
  Control.addVariable("WDivideMat",3);      // Steel
  Control.addVariable("WDivideInsMat",31);   // Water [D2O]

  Control.addVariable("WDivideCornerThick",1.60);     
  Control.addVariable("WDivideCornerWidth",9.40);     

  return;
}

void
TS1CylTarget(FuncDataBase& Control)
  /*!
    Set up a Cylindrical target
    \param Control :: DataBase
   */
{
  ELog::RegMethod RegA("t1BaseVariables[F]","TS1CylTarget");

  Control.addVariable("t1CylTargetBoreRadius",5.70);  // Master bore  
  // TARGET of TS2
  Control.addVariable("t1CylTargetXOffset",0.0);           // Offset ref centre
  Control.addVariable("t1CylTargetYOffset",3.0);           // Offset ref centre
  Control.addVariable("t1CylTargetZOffset",0.0);           // Offset ref centre
  Control.addVariable("t1CylTargetMainLength",33.0);       // Length from 
  Control.addVariable("t1CylTargetCoreRadius",4.9);       // W radius 
  Control.addVariable("t1CylTargetSurfThick",0.2);        // skin layer
  Control.addVariable("t1CylTargetwSphDisplace",1.0025);   // W Centre
  Control.addVariable("t1CylTargetwSphRadius",4.3975);     // W radius 
  Control.addVariable("t1CylTargetwPlaneCut",3.6975);      // W plane cuts
  Control.addVariable("t1CylTargetwPlaneAngle",45.0);      // W plane angle rot

  Control.addVariable("t1CylTargetCladThick",0.086);           // Ta thickness
  Control.addVariable("t1CylTargetWaterThick",0.2);        // Water radius
  Control.addVariable("t1CylTargetPressureThick",0.3);     // Presure radius


  // Front cladding
  Control.addVariable("t1CylTargetTaSphDisplace",0.600);    // [Diag 233]
  Control.addVariable("t1CylTargetTaSphRadius",5.10);       // Ta radius 
  Control.addVariable("t1CylTargetTaPlaneCut",3.8975);      // W plane cuts
  
  // XFlow:
  Control.addVariable("t1CylTargetXFlowCyl",5.20);   // Outer cylinder of XFlow
  Control.addVariable("t1CylTargetXFlowOutDisplace",1.4); // Outer Sphere
  // From diage 235: fig4.
  Control.addVariable("t1CylTargetXFlowOutMidRadius",4.9);   // Top cap cut
  Control.addVariable("t1CylTargetXFlowOutRadius",5.1);   // Outer Sphere

  Control.addVariable("t1CylTargetXFlowInnerRadius",4.9);   // Cut for inner water
  Control.addVariable("t1CylTargetXFlowInnerDisplace",0.6); // Cut for inner water
  Control.addVariable("t1CylTargetXFlowIPlaneCut",1.5);     // Cut for inner water
  Control.addVariable("t1CylTargetXFlowOutCutAngle",-45.0);    // Angle or rotate

  Control.addVariable("t1CylTargetXFlowOPlaneCut",1.9);    // Cut left/right

  Control.addVariable("t1CylTargetTCapInnerRadius",5.10); 
  Control.addVariable("t1CylTargetTCapOuterRadius",5.25); // Outer Sphere
  Control.addVariable("t1CylTargetTCapDisplace",1.4);     // Outer Sphere

  Control.addVariable("t2CapYOffset",4.5);      // Length from flange

  Control.addVariable("t1CylTargetWMat",38);             // Solid Tungsten
  Control.addVariable("t1CylTargetTaMat",32);            // Solid Ta
  Control.addVariable("t1CylTargetWaterMat",11);         // Light water

  Control.addVariable("t1CylTargetTargetTemp",650);             // Inner core temp
  Control.addVariable("t1CylTargetWaterTemp",350);        // Water temp
  Control.addVariable("t1CylTargetExternTemp",330);       // Outer temp
  
  Control.addVariable("t1CylTargetFlangeYDist",60.20);      // Start distance from front
  Control.addVariable("t1CylTargetFlangeYStep",0.3);        // Start distance from front
  //  Control.addVariable("t1CylTargetFlangeRadius",5.750);     // Radius outer
  Control.addVariable("t1CylTargetFlangeRadius",6.750);     // Radius outer ?
  Control.addVariable("t1CylTargetFlangeLen",8.70);         // Full size 
  Control.addVariable("t1CylTargetFlangeThick",2.5);        // Join size [Guess]
  Control.addVariable("t1CylTargetFlangeClear",0.6);        // Flange clearange
  Control.addVariable("t1CylTargetFlangeMat",3);            // Flange material
  Control.addVariable("t1CylTargetNLayers",0);            // Flange material
  return;
}


void
TS1InnerTarget(FuncDataBase& Control)
  /*!
    Set up a Inner cooled target
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("t1BaseVariables[F]","TS1InnerTarget");

  Control.addVariable("t1InnerXStep",0.0);           // Offset ref centre
  Control.addVariable("t1InnerYStep",0.0);           // Offset ref centre
  Control.addVariable("t1InnerZStep",0.0);           // Offset ref centre

  Control.addVariable("t1InnerMainLength",25.0);       // Length from 
  Control.addVariable("t1InnerCoreRadius",4.60);        // W radius 
  Control.addVariable("t1InnerCladThick",0.20);         // skin layer
  Control.addVariable("t1InnerWaterThick",0.30);         // skin layer
  Control.addVariable("t1InnerPressThick",0.20);         // skin layer
  Control.addVariable("t1InnerVoidThick",0.50);         // skin layer
  Control.addVariable("t1InnerSphYStep",0.0);          // W Centre
  Control.addVariable("t1InnerSphRadius",4.6);       // W radius 

  Control.addVariable("t1InnerWMat",38);             // Solid Tungsten
  Control.addVariable("t1InnerTaMat",32);            // Solid Ta
  Control.addVariable("t1InnerWaterMat",11);         // Light water

  Control.addVariable("t1InnerTargetTemp",650);       // Inner core temp
  Control.addVariable("t1InnerWaterTemp",350);        // Water temp
  Control.addVariable("t1InnerExternTemp",330);       // Outer temp

  Control.addVariable("t1InnerNSphere",3);           // Number of spheres
  Control.addVariable("t1InnerFrontWater",1.0);      // front water radius
  Control.addVariable("t1InnerInnerClad",0.20);      // inner cladding
  Control.addVariable("t1InnerInnerWater",0.30);      // inner cladding

  Control.addVariable("t1InnerSRadius1",1.5);      
  Control.addVariable("t1InnerSCent1",Geometry::Vec3D(0,3.0,0));
  Control.addVariable("t1InnerSRadius2",2.0);      
  Control.addVariable("t1InnerSCent2",Geometry::Vec3D(0,6.0,0));      

  Control.addVariable("t1InnerSRadius3",2.5);  
  Control.addVariable("t1InnerSCent3",Geometry::Vec3D(0,10.0,0));      // inner cladding

  Control.addVariable("t1InnerConeRadius",5.0);// End point cone radus
  return;
}

void
TS1CylFluxTrap(FuncDataBase& Control)
  /*!
    Set up a Modifiactions to Cylindrical target
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("t1BaseVariables[F]","TS1CylFluxTrap");
  // MOLY SECTION:
  Control.addVariable("t1CylFluxTrapNPlates",8);
  Control.addVariable("t1CylFluxTrapPThick",0.7);
  Control.addVariable("t1CylFluxTrapPMat",11);
  Control.addVariable("t1CylFluxTrapPLayerMat",32);
  Control.addVariable("t1CylFluxTrapPLayerThick",0.2);

  Control.addVariable("t1CylFluxTrapP1Dist",0.50);       // distance from flat
  Control.addVariable("t1CylFluxTrapP2Dist",1.50);       // distance from flat
  Control.addVariable("t1CylFluxTrapP3Dist",3.00);       // distance from flat
  Control.addVariable("t1CylFluxTrapP4Dist",5.00);       // distance from flat
  Control.addVariable("t1CylFluxTrapP5Dist",7.50);       // distance from flat
  Control.addVariable("t1CylFluxTrapP6Dist",9.50);       // distance from flat
  Control.addVariable("t1CylFluxTrapP7Dist",12.50);       // distance from flat
  Control.addVariable("t1CylFluxTrapP8Dist",15.50);       // distance from flat

  Control.addVariable("t1CylFluxTrapNCutSph",0);
  Control.addVariable("t1CylFluxTrapCutSph1Radius",4.8);
  Control.addVariable("t1CylFluxTrapCutSph1Dist",0.4);
  Control.addVariable("t1CylFluxTrapCutSph1Cent",Geometry::Vec3D(0,4.5,0));
  Control.addVariable("t1CylFluxTrapCutSph1Axis",Geometry::Vec3D(0,1,0));
  Control.addVariable("t1CylFluxTrapCutSphMat",61);

  Control.addVariable("t1CylFluxTrapNCone",0);
  Control.addVariable("t1CylFluxTrapCone1AngleA",-45.0);
  Control.addVariable("t1CylFluxTrapCone1AngleB",30.0);
  Control.addVariable("t1CylFluxTrapCone1Dist",-4.0);
  Control.addVariable("t1CylFluxTrapCone1Cent",Geometry::Vec3D(0,12,0));
  Control.addVariable("t1CylFluxTrapCone1Axis",Geometry::Vec3D(0,1,0));
  Control.addVariable("t1CylFluxTrapCone1Mat",11);
  Control.addVariable("t1CylFluxTrapCone1LayerMat",32);
  Control.addVariable("t1CylFluxTrapCone1LayerThick",0.3);

  Control.addVariable("t1CylFluxTrapCone2AngleA",-45.0);
  Control.addVariable("t1CylFluxTrapCone2AngleB",-45.0);
  Control.addVariable("t1CylFluxTrapCone2Dist",2.0);
  Control.addVariable("t1CylFluxTrapCone2Cent",Geometry::Vec3D(0,18,0));
  Control.addVariable("t1CylFluxTrapCone2Axis",Geometry::Vec3D(0,1,0));
  Control.addVariable("t1CylFluxTrapCone2Mat",11);
  return;
}

void
TS1CylFluxTrapReal(FuncDataBase& Control)
  /*!
    Set up a Modifiactions to Cylindrical target
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("t1BaseVariables[F]","TS1CylFluxTrap");
  // MOLY SECTION:
  Control.addVariable("t1CylFluxTrapNPlates",8);
  Control.addVariable("t1CylFluxTrapPThick",0.7);
  Control.addVariable("t1CylFluxTrapPMat",11);
  Control.addVariable("t1CylFluxTrapPLayerMat",32);
  Control.addVariable("t1CylFluxTrapPLayerThick",0.2);

  Control.addVariable("t1CylFluxTrapP1Dist",0.50);       // distance from flat
  Control.addVariable("t1CylFluxTrapP2Dist",1.50);       // distance from flat
  Control.addVariable("t1CylFluxTrapP3Dist",3.00);       // distance from flat
  Control.addVariable("t1CylFluxTrapP4Dist",5.00);       // distance from flat
  Control.addVariable("t1CylFluxTrapP5Dist",7.50);       // distance from flat
  Control.addVariable("t1CylFluxTrapP6Dist",9.50);       // distance from flat
  Control.addVariable("t1CylFluxTrapP7Dist",12.50);       // distance from flat
  Control.addVariable("t1CylFluxTrapP8Dist",15.50);       // distance from flat

  Control.addVariable("t1CylFluxTrapNCutSph",0);
  Control.addVariable("t1CylFluxTrapCutSph1Radius",4.8);
  Control.addVariable("t1CylFluxTrapCutSph1Dist",0.4);
  Control.addVariable("t1CylFluxTrapCutSph1Cent",Geometry::Vec3D(0,4.5,0));
  Control.addVariable("t1CylFluxTrapCutSph1Axis",Geometry::Vec3D(0,1,0));
  Control.addVariable("t1CylFluxTrapCutSphMat",61);

  Control.addVariable("t1CylFluxTrapNCone",0);
  Control.addVariable("t1CylFluxTrapCone1AngleA",-45.0);
  Control.addVariable("t1CylFluxTrapCone1AngleB",30.0);
  Control.addVariable("t1CylFluxTrapCone1Dist",-4.0);
  Control.addVariable("t1CylFluxTrapCone1Cent",Geometry::Vec3D(0,12,0));
  Control.addVariable("t1CylFluxTrapCone1Axis",Geometry::Vec3D(0,1,0));
  Control.addVariable("t1CylFluxTrapCone1Mat",11);
  Control.addVariable("t1CylFluxTrapCone1LayerMat",32);
  Control.addVariable("t1CylFluxTrapCone1LayerThick",0.3);

  Control.addVariable("t1CylFluxTrapCone2AngleA",-45.0);
  Control.addVariable("t1CylFluxTrapCone2AngleB",-45.0);
  Control.addVariable("t1CylFluxTrapCone2Dist",2.0);
  Control.addVariable("t1CylFluxTrapCone2Cent",Geometry::Vec3D(0,18,0));
  Control.addVariable("t1CylFluxTrapCone2Axis",Geometry::Vec3D(0,1,0));
  Control.addVariable("t1CylFluxTrapCone2Mat",11);
  return;
}

void
TS1Cannelloni(FuncDataBase& Control)
  /*!
    Set up a cannelloni target
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("t1BaseVariables[F]","TS1InnerTarget");

  Control.addVariable("t1CannelloniXStep",0.0);           // Offset ref centre
  Control.addVariable("t1CannelloniYStep",0.0);           // Offset ref centre
  Control.addVariable("t1CannelloniZStep",0.0);           // Offset ref centre

  Control.addVariable("t1CannelloniMainLength",25.0);       // Length from 
  Control.addVariable("t1CannelloniCoreRadius",4.30);        // W radius 
  Control.addVariable("t1CannelloniWallClad",0.30);        // W radius 
  Control.addVariable("t1CannelloniWallThick",0.50);        // W radius 
  Control.addVariable("t1CannelloniVoidThick",0.60);        // W radius 

  Control.addVariable("t1CannelloniTubeClad",0.20);       // skin layer
  Control.addVariable("t1CannelloniTubeRadius",0.80);     // skin layer
  Control.addVariable("t1CannelloniFrontThick",0.30);     // skin layer
  Control.addVariable("t1CannelloniFrontWater",0.20);     // skin layer

  Control.addVariable("t1CannelloniWMat",38);             // Solid Tungsten
  Control.addVariable("t1CannelloniTaMat",32);            // Solid Ta
  Control.addVariable("t1CannelloniWaterMat",11);         // Light water

  Control.addVariable("t1CannelloniTargetTemp",650);       // Inner core temp
  Control.addVariable("t1CannelloniWaterTemp",350);        // Water temp
  Control.addVariable("t1CannelloniExternTemp",330);       // Outer temp

  return;
}


void
TS1EllipticCylTarget(FuncDataBase& Control)
  /*!
    Set up a Cylindrical target
    \param Control :: DataBase
   */
{
  ELog::RegMethod RegA("t1BaseVariables[F]","TS1EllCylTarget");

  Control.addVariable("t1EllCylTargetBoreRadius",5.70);  // Master bore  

  Control.addVariable("t1EllCylTargetXStep",0.0);     // Offset ref centre
  Control.addVariable("t1EllCylTargetYStep",0.0);     // Offset ref centre
  Control.addVariable("t1EllCylTargetZStep",0.0);     // Offset ref centre
  Control.addVariable("t1EllCylTargetMainLength",33.0);       // Length from 
  Control.addVariable("t1EllCylTargetXRadius",4.0);       // W radius 
  Control.addVariable("t1EllCylTargetZRadius",4.9);       // W radius 
  Control.addVariable("t1EllCylTargetCladThick",0.2);        // skin layer
  Control.addVariable("t1EllCylTargetWaterThick",0.2);        // Water radius
  Control.addVariable("t1EllCylTargetPressThick",0.3);     // Presure radius
  Control.addVariable("t1EllCylTargetVoidThick",0.5);     // Presure radius
  Control.addVariable("t1EllCylTargetCutAngle",45);      // Presure radius

  Control.addVariable("t1EllCylTargetWMat",38);             // Solid Tungsten
  Control.addVariable("t1EllCylTargetTaMat",32);            // Solid Ta
  Control.addVariable("t1EllCylTargetWaterMat",11);         // Light water
  Control.addVariable("t1EllCylTargetTargetTemp",650);      // Inner core temp
  Control.addVariable("t1EllCylTargetWaterTemp",350);        // Water temp
  Control.addVariable("t1EllCylTargetExternTemp",330);       // Outer temp
  
  return;
}


void
TS1base(FuncDataBase& Control)
  /*!
    Function to set the control variables and constants
    -- This version is for TS1 (real version : non-model)
    \param Control :: Function data base to add constants too
  */
{

// -----------
// GLOBAL stuff
// -----------

  Control.addVariable("zero",0.0);      // Zero
  Control.addVariable("one",1.0);      // one

// ------------
// Target stuff
// ------------
  Control.addVariable("beamsize",3.4);         // Radius the beam
  Control.addVariable("flightradius",3.6);     // Radius the beam in the flightline
  // REAL VOID VESSEL
  Control.addVariable("voidYoffset",9.2);            // Dave Bellenger      
  Control.addVariable("t1CylVesselRadius",161.0);     // Radius from G4Model
  Control.addVariable("t1CylVesselClearance",2.0);     // H.J. drawing
  Control.addVariable("t1CylVesselTopRadius",350.0);  
  Control.addVariable("t1CylVesselBaseRadius",600.0);   
  Control.addVariable("t1CylVesselHeight",205.0);      // Full height minus top  
  Control.addVariable("t1CylVesselWallThick",2.0);    
  Control.addVariable("t1CylVesselWallMat",54);       // Cast Iron

  Control.addVariable("t1CylVesselNPorts",18);       // Number of ports
  Control.addVariable("t1CylVesselPortHeight",14.0);       // Height
  Control.addVariable("t1CylVesselPortWidth",20.0);       // Width
  
  Control.addVariable("t1CylVesselPortCHeight1",-14.00);    // [Methane]
  Control.addVariable("t1CylVesselPortCHeight2",-14.00);    // N2  	
  Control.addVariable("t1CylVesselPortCHeight3",-14.00);    // N3	
  Control.addVariable("t1CylVesselPortCHeight4",-14.00);    // [Hydrogen
  Control.addVariable("t1CylVesselPortCHeight5",-14.00);     // N5 	
  Control.addVariable("t1CylVesselPortCHeight6",-14.00);    // N6	
  Control.addVariable("t1CylVesselPortCHeight7",14.00);    // [Water]  
  Control.addVariable("t1CylVesselPortCHeight8",14.00);    // N8	
  Control.addVariable("t1CylVesselPortCHeight9",14.00);    // N9	
  Control.addVariable("t1CylVesselPortCHeight10",14.00);   // [Water]  
  Control.addVariable("t1CylVesselPortCHeight11",14.00);   // S2 	
  Control.addVariable("t1CylVesselPortCHeight12",14.00);   // S3	
  Control.addVariable("t1CylVesselPortCHeight13",14.00);   // [Merlin] 
  Control.addVariable("t1CylVesselPortCHeight14",14.00);   // S5	
  Control.addVariable("t1CylVesselPortCHeight15",-14.00);   // [Methane]
  Control.addVariable("t1CylVesselPortCHeight16",-14.00);    // S7	
  Control.addVariable("t1CylVesselPortCHeight17",-14.00);    // S8	
  Control.addVariable("t1CylVesselPortCHeight18",-14.00);    // S9       
       
  Control.addVariable("t1CylVesselPortAngle1",46.0);       // Angle
  Control.addVariable("t1CylVesselPortAngle2",59.00);       // Angle 
  Control.addVariable("t1CylVesselPortAngle3",72.00);       // Angle
  Control.addVariable("t1CylVesselPortAngle4",85.00);       // Angle 
  Control.addVariable("t1CylVesselPortAngle5",98.00);       // Angle 
  Control.addVariable("t1CylVesselPortAngle6",111.00);      // Angle 
  Control.addVariable("t1CylVesselPortAngle7",124.00);      // Angle 
  Control.addVariable("t1CylVesselPortAngle8",137.00);      // Angle 
  Control.addVariable("t1CylVesselPortAngle9",150.00);      // HET  
  Control.addVariable("t1CylVesselPortAngle10",-46.00);     // Angle
  Control.addVariable("t1CylVesselPortAngle11",-59.00);     // Angle
  Control.addVariable("t1CylVesselPortAngle12",-72.00);     // Angle 
  Control.addVariable("t1CylVesselPortAngle13",-85.00);     // Angle
  Control.addVariable("t1CylVesselPortAngle14",-98.00);     // Angle 
  Control.addVariable("t1CylVesselPortAngle15",-111.00);    // Angle 
  Control.addVariable("t1CylVesselPortAngle16",-124.00);    // Angle 
  Control.addVariable("t1CylVesselPortAngle17",-137.00);    // Angle 
  Control.addVariable("t1CylVesselPortAngle18",-150.00);    // Angle           

    // BULK INSERT T1
  // Control.addVariable("t1bulkInsertIHeight",56.25);
  // Control.addVariable("t1bulkInsertIWidth",32.50);
  // Control.addVariable("t1bulkInsertOHeight",61.25);
  // Control.addVariable("t1bulkInsertOWidth",37.50);
  // Control.addVariable("t1bulkInsertZOffset",0.00);

  // BULK VESSEL:
  Control.addVariable("t1BulkShutterRadius",366.0);    // Shield level
  Control.addVariable("t1BulkInnerRadius",481.0);      // Inner Steel
  Control.addVariable("t1BulkOuterRadius",600.6);      // Outer Steel
  Control.addVariable("t1BulkFloor",270.5);           // Bulk shield floor  
  Control.addVariable("t1BulkRoof",509.5);            // Bulk shield top
  Control.addVariable("t1BulkIronMat",54);             // bulk material

  Control.addVariable("t1BulkInThick",115.0);         // Bulk First compartment
  Control.addVariable("t1BulkOutThick",119.0);        // Bulk Second compartment
  Control.addVariable("t1BulkInTop",50.2);            // Bulk Height (from centre line)
  Control.addVariable("t1BulkInBase",63.2);           // Bulk Base (from centre line)
  Control.addVariable("t1BulkOutTop",52.2);           // Bulk Height (from centre line)
  Control.addVariable("t1BulkOutBase",68.8);          // Bulk Base (from centre line)

  Control.addVariable("t1BulkInWidth",36);        // Bulk Height [cm] (from centre line)
  Control.addVariable("t1BulkOutWidth",40);       // Bulk Width [cm] (from centre 

  // SHUTTERS
  Control.addVariable("shutterUpperSteel",190.5);     // top thickness
  Control.addVariable("shutterLowerSteel",265.90);    // base thickness
  Control.addVariable("shutterHeight",262.8);         // Total shutter height
  Control.addVariable("shutterWidth",28.00);          // Full width

  Control.addVariable("shutterVoidZOffset",0.0);        // centre of steel hole
 
  Control.addVariable("shutterVoidHeight",28.0);             // Void height
  Control.addVariable("shutterVoidHeightInner",18.00);     // Gap height
  Control.addVariable("shutterVoidHeightOuter",22.30);     // Gap height
  Control.addVariable("shutterVoidWidthInner",18.00);      // Gap width
  Control.addVariable("shutterVoidWidthOuter",24.00);      // Gap width
  Control.addVariable("shutterVoidDivide",66.00);             // Smaller gap len
  // 
  Control.addVariable("shutterSteelMat",54);         // Cast iron

//  Control.addVariable("shutter1Height",106.0);         // New drawing 8711-300
//  Control.addVariable("shutter1VoidHeightInner",25.0);       // ChipIR size
//  Control.addVariable("shutter1VoidHeightOuter",25.0);       // ChipIR size
//  Control.addVariable("shutter1VoidDivide",-1.0);          // ChipIR no divide
//  Control.addVariable("shutter10VoidWidthInner",21.00);      // Gap width
  
//  Control.addVariable("shutter1VoidZOffset",-30.0);
//  Control.addVariable("shutter2VoidZOffset",0.0);

//  Control.addVariable("shutter1GapSize",25);        // ChipIR size
  Control.addVariable("shutterGapSize",22.30);      // Gap size
//  Control.addVariable("shutter1ZAngle",0.0);        // shutter slope/ up/down
  //  Control.addVariable("shutter1centreZoffset",7.0); // Central shutter
//  Control.addVariable("shutter1Closed",0);          // Open shutter (false)
//  Control.addVariable("shutter2Closed",1);          // closed shutter (true)
//  Control.addVariable("shutterClosed",0);           // Open shutter [imp:0]
//  Control.addVariable("shutter1ClosedZOffset",35.0);   // Closed distance
//  Control.addVariable("shutter2ClosedZOffset",28.0);   // Closed distance
  Control.addVariable("shutterClosedZOffset",28.0);    // Closed distance
  
  // ALL SHUTTERS HAVE DIFFERENT POSITIONS:
  Control.addVariable("shutter1OpenZShift",-14.00);    // N1 [Methane] 
  Control.addVariable("shutter2OpenZShift",-14.00);    // N2  	       
  Control.addVariable("shutter3OpenZShift",-14.00);    // N3 [Hydrogen]	       
  Control.addVariable("shutter4OpenZShift",-14.00);    // N4
  Control.addVariable("shutter5OpenZShift",-14.00);    // N5 	       
  Control.addVariable("shutter6OpenZShift",-14.00);    // N6	       
  Control.addVariable("shutter7OpenZShift",14.00);     // N7 [Water]   
  Control.addVariable("shutter8OpenZShift",14.00);     // N8	       
  Control.addVariable("shutter9OpenZShift",14.00);     // N9	       
  Control.addVariable("shutter10OpenZShift",14.00);    // S1 [Water]   
  Control.addVariable("shutter11OpenZShift",14.00);    // S2 	       
  Control.addVariable("shutter12OpenZShift",14.00);    // S3	       
  Control.addVariable("shutter13OpenZShift",14.00);    // S4 [Merlin]  
  Control.addVariable("shutter14OpenZShift",14.00);    // S5	       
  Control.addVariable("shutter15OpenZShift",-14.00);   // S6 [Methane] 
  Control.addVariable("shutter16OpenZShift",-14.00);   // S7	       
  Control.addVariable("shutter17OpenZShift",-14.00);   // S8	       
  Control.addVariable("shutter18OpenZShift",-14.00);   // S9           

  Control.addVariable("shutter1XYAngle",46.00);       // Angle N1 [Methane]
  Control.addVariable("shutter2XYAngle",59.00);       // Angle N2
  Control.addVariable("shutter3XYAngle",72.00);       // Angle N3 [Hydrogen]
  Control.addVariable("shutter4XYAngle",85.00);       // Angle N4
  Control.addVariable("shutter5XYAngle",98.00);       // Angle N5
  Control.addVariable("shutter6XYAngle",111.00);      // Angle N6
  Control.addVariable("shutter7XYAngle",124.00);      // Angle N7 [Water]
  Control.addVariable("shutter8XYAngle",137.00);      // Angle N8
  Control.addVariable("shutter9XYAngle",150.00);      // Angle N9
  Control.addVariable("shutter10XYAngle",-46.00);     // Angle S1 [Water]
  Control.addVariable("shutter11XYAngle",-59.00);     // Angle S2
  Control.addVariable("shutter12XYAngle",-72.00);     // Angle S3
  Control.addVariable("shutter13XYAngle",-85.00);     // Angle S4 [Merlin]
  Control.addVariable("shutter14XYAngle",-98.00);     // Angle S5
  Control.addVariable("shutter15XYAngle",-111.00);    // Angle S6 [Methane]
  Control.addVariable("shutter16XYAngle",-124.00);    // Angle S7
  Control.addVariable("shutter17XYAngle",-137.00);    // Angle S8
  Control.addVariable("shutter18XYAngle",-150.00);    // Angle S9                

  //  Control.addVariable("shutterInsertChipGap",25);  // Shutter Insert thickness
  Control.addVariable("shutterHWidth",12.0);       // Shutter Half width
  Control.addVariable("shutterMat",54);             // shutter Material
  Control.addVariable("shutterSurMat",54);          // shutter surround mat
  Control.addVariable("shutterSupportMat",54);      // shutter support mat

  // Standard shutter:
  Control.addVariable("shutterBlockFlag",3);         // Flag 1:top/ 2 Base
  Control.addVariable("shutterBlockCent1",22.5);
  Control.addVariable("shutterBlockCent2",177.5);
  Control.addVariable("shutterBlockLen",30.0);
  Control.addVariable("shutterBlockHeight",25.0);
  Control.addVariable("shutterBlockVGap",7.50);
  Control.addVariable("shutterBlockHGap",3.0);
  Control.addVariable("shutterBlockMat",50);
  
  // BULK INSERT
  Control.addVariable("bulkInsertIHeight",56.25);
  Control.addVariable("bulkInsertIWidth",32.50);
  Control.addVariable("bulkInsertOHeight",61.25);
  Control.addVariable("bulkInsertOWidth",37.50);
  Control.addVariable("bulkInsertZOffset",0.00);

  // Mono Plug
  Control.addVariable("MonoTopNPlugs",3);
  Control.addVariable("MonoTopPlugClearance",3.0);
  Control.addVariable("MonoTopDivideLen",3.0);
  Control.addVariable("MonoTopSteelMat",3);
  Control.addVariable("MonoTopConcMat",29);
  Control.addVariable("MonoTopPlugRadius1",50.0);  
  Control.addVariable("MonoTopPlugRadius2",62.0);  
  Control.addVariable("MonoTopPlugRadius3",75.0);  
  Control.addVariable("MonoTopPlugZLen1",141.0);  
  Control.addVariable("MonoTopPlugZLen2",264.0);  
  Control.addVariable("MonoTopPlugZLen3",381.0);  

  Control.addVariable("MonoBaseNPlugs",1);
  Control.addVariable("MonoBasePlugClearance",1.0);
  Control.addVariable("MonoBaseDivideLen",3.0);
  Control.addVariable("MonoBaseSteelMat",3);
  Control.addVariable("MonoBaseConcMat",29);
  Control.addVariable("MonoBasePlugRadius1",90.0);
  Control.addVariable("MonoBasePlugZLen1",50.0);
  

  // SHUTTERS COLLIMATION:

  ts1System::shutterVar Sandals("sandals"); // SANDALS shutter number [north 1]; former SYM
  ts1System::shutterVar Prisma("prisma"); // PRISMA shutter number [north 2]; former SANDALS
  ts1System::shutterVar Surf("surf"); // SURF shutter number [north 3]
  ts1System::shutterVar Crisp("crisp"); // CRISP shutter number [north 4]
  ts1System::shutterVar Loq("loq"); // LOQ shutter number [north 5]
  ts1System::shutterVar Iris("iris"); // IRIS shutter number [north 6]
  ts1System::shutterVar PolarisII("polarisII"); // POLARISII shutter number [north 7]; former HIPD
  ts1System::shutterVar Tosca("tosca"); // TOSCA shutter number [north 8]; former HTIS
  ts1System::shutterVar Het("het"); // HET shutter number [north 9]
  ts1System::shutterVar Maps("maps"); // MAPS shutter number [south 1]; former POL
  ts1System::shutterVar Vesuvio("vesuvio"); // VESUVIO shutter number [south 2]; former eV
  ts1System::shutterVar Sxd("sxd"); // SXD shutter number [south 3]; former SXS
  ts1System::shutterVar Merlin("merlin"); // MERLIN shutter number [south 4]
  ts1System::shutterVar S5("s5"); // S5 shutter number [south 5]
  ts1System::shutterVar Mari("mari"); // MARI shutter number [south 6]; former MET
  ts1System::shutterVar Gem("gem"); // GEM shutter number [south 7]
  ts1System::shutterVar Hrpd("hrpd"); // HRPD shutter number [south 8]
  ts1System::shutterVar Pearl("pearl"); // PEARL shutter number [south 9]; former TEST
     
  // Blocks:
  // xstep : xang(dir) : zang(dir) : beamX(size) [1st b4c]: beamZ(size) [1st b4c]: angleX(reduction) : angleZ(reduction)          

  // Cylinders:
  // xstep : xang(dir) : zang(dir) : Diameter(size)  : angleR(reduction) 
 
  // Comment = what's missing        
  // * = xang is IN
  
  Sandals.buildCylVar(Control,-1.6,-1.6,0.0,7.3309,0.3595); // xstep = estimated
  Prisma.buildVar(Control,0.0,-1.7,0.0,6.17,8.61,0.35,0.47); // xstep
  Surf.buildVar(Control,2.0,2.2,-1.5,6.74,4.8,0.05,0.44); // (xstep; zang = estimated) 
  Crisp.buildVar(Control,2.0,2.2,-1.5,7.91,4.57,0.22,0.275); // (xstep; zang = estimated)
  Loq.buildVar(Control,0.0,2.2,0.0,7.9,7.9,0.395,0.395); // xstep
  Iris.buildVar(Control,0.0,2.0,0.0,7.0,7.0,0.0,0.0); // DRAWINGS* (default beam size = 7 x 7 cm)
  PolarisII.buildVar(Control,0.0,-1.7,0.0,8.22,7.93,0.35,0.22); // xstep
  Tosca.buildVar(Control,0.0,-1.5,0.0,8.4,8.0,0.16,0.088); // from Instr. Techical. Spec. ver1 (xstep) 
  Het.buildVar(Control,-0.6,-1.3,0.0,7.90,7.90,0.63,0.63);    
  Maps.buildVar(Control,0.0,1.3,0.0,7.34,7.34,0.22,0.22); // xstep 
  Vesuvio.buildVar(Control,0.0,1.7,0.0,7.38,7.38,0.58,0.58);
  Sxd.buildVar(Control,2.0,2.0,0.0,8.88,8.88,0.58,0.58); // (xstep = estimate)
  Merlin.buildVar(Control,-2.0,-2.2,0.0,9.4,9.4,0.0,0.0); // DRAWINGS* (beam size = 9.4 x 9.4 cm - from Rob Bewley)
  S5.buildVar(Control,0.0,-2.2,0.0,7.0,7.0,0.0,0.0); // CLOSED
  Mari.buildVar(Control,0.0,2.0,0.0,7.74,7.74,0.48,0.48); // xstep 
  Gem.buildVar(Control,0.0,1.4,0.0,7.0,7.0,0.0,0.0);  // DRAWINGS* (default beam size = 7 x 7 cm)
  Hrpd.buildVar(Control,0.0,1.5,0.0,7.0,7.0,0.0,0.0);  // DRAWINGS* (default beam size = 7 x 7 cm)
  Pearl.buildVar(Control,0.0,1.3,0.0,7.0,7.0,0.0,0.0);  // DRAWINGS* (default beam size = 7 x 7 cm)
    
  return;
}  


}  // NAMESPACE setVariable
