/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   delft/delftVariables.cxx
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
DelftModel(FuncDataBase& Control)
  /*!
    Function to set the control variables and constants
    -- This version is for TS1 (real version : non-model)
    \param Control :: Function data base to add constants too
  */
{
  ELog::RegMethod Rega("delftVariables[f]","DelftModel");
// -----------
// GLOBAL stuff
// -----------

  Control.addVariable("zero",0.0);     // Zero
  Control.addVariable("one",1.0);      // One

  Control.addVariable("delftGridXSize",6); // Crossing side
  Control.addVariable("delftGridYSize",7); // port side

  Control.addVariable("delftGridXStep",0.0); // X step
  Control.addVariable("delftGridYStep",0.0); // Y step
  Control.addVariable("delftGridZStep",0.0); // Z step
  Control.addVariable("delftGridXYangle",0.0); // Z step
  Control.addVariable("delftGridZangle",0.0); // Z step

  Control.addVariable("delftGridYSize",7); // port side

  Control.addVariable("delftGridWidth",46.254);  
  Control.addVariable("delftGridDepth",56.70); 

  Control.addVariable("delftGridTop",90.0); 
  Control.addVariable("delftGridBase",36.0); 
  Control.addVariable("delftGridPlateThick",12.70); 
  Control.addVariable("delftGridPlateRadius",2.54); 
  Control.addVariable("delftGridPlateMat","Aluminium"); 
  Control.addVariable("delftGridWaterMat","H2O"); 

  // Plate defined on : Capital / Number / lowercase 
  // Missing assumes all
  Control.addVariable("delftGridMat","H2O");  // Water !!

  Control.addVariable("delftElementNFuel",19);  // Number of elements
  Control.addVariable("delftElementXStep",0.0);  
  Control.addVariable("delftElementYStep",0.0);  
  Control.addVariable("delftElementZStep",0.0);  
  Control.addVariable("delftElementXYAngle",0.0);  
  Control.addVariable("delftElementZAngle",0.0);  

  // Special for Hf Element
  Control.addVariable("delftElementNFuelC2",23);   // Number of elements  
  Control.addVariable("delftElementCladDepthC2",0.039);   
  Control.addVariable("delftElementFuelDepthC2",0.051);
  Control.addVariable("delftElementWaterDepthC2",0.22);      

  Control.addVariable("delftHfCutSize",3);      
  Control.addVariable("delftHfLift",0.0);      
  Control.addVariable("delftHfAbsHeight",100.0);      
  Control.addVariable("delftHfAbsMat","Hafnium");      
  Control.addVariable("delftHfBladeMat","Aluminium");      
  Control.addVariable("delftHfAbsThick",0.3);      
  Control.addVariable("delftHfBladeThick",0.15);      
  Control.addVariable("delftHfAbsWidth",6.275);   
  Control.addVariable("delftHfBladeWidth",0.2);   
  
  // FUEL ELEMENT
  Control.addVariable("delftElementNFuelDivide",1);   
  Control.addVariable("delftElementDepth",7.94);   
  Control.addVariable("delftElementWidth",7.56);   

  Control.addVariable("delftElementFuelWidth",6.2);   
  Control.addVariable("delftElementFuelHeight",60.0);   
  Control.addVariable("delftElementFuelDepth",0.05);   

  Control.addVariable("delftElementCladWidth",0.225);   
  Control.addVariable("delftElementCladHeight",1.0);   
  Control.addVariable("delftElementCladDepth",0.035);   

  Control.addVariable("delftElementWaterDepth",0.3);   

  Control.addVariable("delftElementTopHeight",5.0);   
  Control.addVariable("delftElementBaseHeight",5.0);   
  Control.addVariable("delftElementBaseRadius",2.54);   

  Control.addVariable("delftElementBarRadius",0.425);   
  Control.addVariable("delftElementBarOffset",1.575);     // distance down

  Control.addVariable("delftElementFuelMat","Uranium");       // approximate U
  Control.addVariable("delftElementAlMat","Aluminium");       
  Control.addVariable("delftElementWaterMat","H2O");   

  // GENERAL CONTROL 
  Control.addVariable("delftControlAbsMat","B4C");    
  Control.addVariable("delftControlCladMat","Aluminium");    
  Control.addVariable("delftControlCStartIndex",5);    
  Control.addVariable("delftControlCEndIndex",14);    
  Control.addVariable("delftControlVoidRadius",0.625);    
  Control.addVariable("delftControlVoidOffset",1.75);    
  Control.addVariable("delftControlAbsThick",1.72);    // Radius * 2
  Control.addVariable("delftControlCaseThick",0.25);    
  Control.addVariable("delftControlWaterThick",0.305);    
  Control.addVariable("delftControlPlateThick",0.3);    
  Control.addVariable("delftControlPlateHeight",22.0);    
  Control.addVariable("delftControlCapHeight",22.0);    
  Control.addVariable("delftControlInnerLength",62.0);    
  Control.addVariable("delftControlOuterLength",74.5);   // Al from base-top
  Control.addVariable("delftControlOuterCapLen",3.9);   // Al from base-top
  Control.addVariable("delftControlEndCap",2.5);   // Al from base-top
  Control.addVariable("delftControlLift",46.39);    

  // Be Blocks
  Control.addVariable("delftBeWidth",7.6);    
  Control.addVariable("delftBeDepth",8.0);    
  Control.addVariable("delftBeTopHeight",34.15); 
  Control.addVariable("delftBeMat","Be300K"); 


  // Be plugelement:
  Control.addVariable("delftBePlugWidth",7.6);    
  Control.addVariable("delftBePlugDepth",8.0);    
  Control.addVariable("delftBePlugTopHeight",46.0); 
   
  Control.addVariable("delftBePlugSampleRad",0.62);    
  Control.addVariable("delftBePlugSampleXOffset",1.0);    
  Control.addVariable("delftBePlugSampleZOffset",0.0);    
  Control.addVariable("delftBePlugVacRadius",0.80);    
  Control.addVariable("delftBePlugCaseRadius",0.95);    
  Control.addVariable("delftBePlugCoreRadius",2.0);    
  Control.addVariable("delftBePlugOuterRadius",2.25);    
  Control.addVariable("delftBePlugWaterRadius",2.5);    
  Control.addVariable("delftBePlugTopLocator",5.3);
  Control.addVariable("delftBePlugLocatorThick",1.0);
  Control.addVariable("delftBePlugTopPlug",1.2);    
  Control.addVariable("delftBePlugBeLen",67.4);    
  Control.addVariable("delftBePlugEndStop",2.7);    

  Control.addVariable("delftBePlugPlugThick",1.5);    
  Control.addVariable("delftBePlugPlugZOffset",-8.74);    

  Control.addVariable("delftBePlugSampleMat","Gadolinium");    
  Control.addVariable("delftBePlugPipeMat","Aluminium");    
  Control.addVariable("delftBePlugBeMat","Be300K");    
  Control.addVariable("delftBePlugWaterMat","H2O");    

  // AirBox elemetn:
  // Irradiation element:
  Control.addVariable("delftAirBoxWidth",7.6);    
  Control.addVariable("delftAirBoxDepth",8.0);    
  Control.addVariable("delftAirBoxHeight",46.0); 
  Control.addVariable("delftAirBoxEdgeGap",0.3);    
  Control.addVariable("delftAirBoxWallThick",0.5);    
  Control.addVariable("delftAirBoxWallMat","Aluminium");    
  Control.addVariable("delftAirBoxInnerMat","Void");    


  // Irradiation element:
  Control.addVariable("delftIrradWidth",7.6);    
  Control.addVariable("delftIrradDepth",8.0);    
  Control.addVariable("delftIrradTopHeight",46.0); 
   
  Control.addVariable("delftIrradSampleRad",0.62);    
  Control.addVariable("delftIrradSampleXOffset",1.0);    
  Control.addVariable("delftIrradSampleZOffset",0.0);    
  Control.addVariable("delftIrradVacRadius",0.80);    
  Control.addVariable("delftIrradCaseRadius",0.95);    
  Control.addVariable("delftIrradCoreRadius",2.0);    
  Control.addVariable("delftIrradOuterRadius",2.25);    
  Control.addVariable("delftIrradWaterRadius",2.5);    
  // Outer irradiation suraces
  Control.addVariable("delftIrradTopLocator",5.3);
  Control.addVariable("delftIrradLocatorThick",1.0);
  Control.addVariable("delftIrradTopPlug",1.2);    
  Control.addVariable("delftIrradBeLen",67.4);    
  Control.addVariable("delftIrradEndStop",2.7);    

  Control.addVariable("delftIrradPlugThick",1.5);    
  Control.addVariable("delftIrradPlugZOffset",-8.74);    

  Control.addVariable("delftIrradSampleMat","Gadolinium");    
  Control.addVariable("delftIrradPipeMat","Aluminium");    
  Control.addVariable("delftIrradBeMat","Be300K");    
  Control.addVariable("delftIrradWaterMat","H2O");    

  // pool
  Control.addVariable("delftPoolXStep",-4.0);    
  Control.addVariable("delftPoolYStep",0.0);    
  Control.addVariable("delftPoolZStep",0.0);    
  Control.addVariable("delftPoolFrontWidth",100.0);    
  Control.addVariable("delftPoolBackWidth",240.0);    
  Control.addVariable("delftPoolBeamSide",240.0);    
  Control.addVariable("delftPoolExtendBeamSide",300.0);    
  Control.addVariable("delftPoolBeamSideBackLength",150.0);    
  Control.addVariable("delftPoolBeamSideFrontLength",70.0);    
  Control.addVariable("delftPoolBeamSideExtendLength",208.0);    
  Control.addVariable("delftPoolFrontLength",180.0);    
  Control.addVariable("delftPoolDoorWidth",100.0);    
  Control.addVariable("delftPoolDoorLength",90.0);    

  Control.addVariable("delftPoolBase",200.0);    
  Control.addVariable("delftPoolSurface",600.0);    
  Control.addVariable("delftPoolWaterMat","H2O");    
  
  // FlightLines
  
  Control.addVariable("delftFlight1XStep",23.127+1.8);
  Control.addVariable("delftFlight1YStep",0.0);
  Control.addVariable("delftFlight1ZStep",0.0);
  Control.addVariable("delftFlight1XYAngle",0.0);
  Control.addVariable("delftFlight1ZAngle",0.0);

  Control.addVariable("delftFlight1Length",1000.0);
  Control.addVariable("delftFlight1InnerRadius",10.3);
  Control.addVariable("delftFlight1InnerWall",0.4);
  Control.addVariable("delftFlight1OuterRadius",11.75);
  Control.addVariable("delftFlight1OuterWall",0.5);

  Control.addVariable("delftFlight1FrontWall",0.7);
  Control.addVariable("delftFlight1FrontGap",0.1);
  Control.addVariable("delftFlight1FrontIWall",0.5);

  Control.addVariable("delftFlight1InterThick",0.2);
  Control.addVariable("delftFlight1InterWallThick",0.3);
  Control.addVariable("delftFlight1InterFrontWall",0.333);
  Control.addVariable("delftFlight1InterYOffset",77.2);
  Control.addVariable("delftFlight1InterMat","B4C");

  Control.addVariable("delftFlight1WallMat","Aluminium");
  Control.addVariable("delftFlight1GapMat","Void");

  // FLIGHT 2:
  Control.addVariable("delftFlight2XStep",23.127+1.8);
  Control.addVariable("delftFlight2YStep",0);
  Control.addVariable("delftFlight2ZStep",-24.5);
  Control.addVariable("delftFlight2XYAngle",21.0);
  Control.addVariable("delftFlight2ZAngle",0.0);

  Control.addVariable("delftFlight2Length",1000.0);
  Control.addVariable("delftFlight2InnerRadius",7.5);
  Control.addVariable("delftFlight2InnerWall",0.5);
  Control.addVariable("delftFlight2OuterRadius",9.25);
  Control.addVariable("delftFlight2OuterWall",0.5);

  Control.addVariable("delftFlight2FrontWall",0.7);
  Control.addVariable("delftFlight2FrontGap",4.4);
  Control.addVariable("delftFlight2FrontIWall",0.5);

  Control.addVariable("delftFlight2InterThick",0.2);
  Control.addVariable("delftFlight2InterWallThick",0.3);
  Control.addVariable("delftFlight2InterFrontWall",0.333);
  Control.addVariable("delftFlight2InterYOffset",77.2);
  Control.addVariable("delftFlight2InterMat","B4C");

  Control.addVariable("delftFlight2WallMat","Aluminium");
  Control.addVariable("delftFlight2GapMat","Void");

  // FLIGHT 3:
  Control.addVariable("delftFlight3XStep",23.127+1.8);
  Control.addVariable("delftFlight3YStep",0.0);
  Control.addVariable("delftFlight3ZStep",24.5);
  Control.addVariable("delftFlight3XYAngle",-26.0);
  Control.addVariable("delftFlight3ZAngle",0.0);

  Control.addVariable("delftFlight3Length",1000.0);
  Control.addVariable("delftFlight3InnerRadius",7.5);
  Control.addVariable("delftFlight3InnerWall",0.5);
  Control.addVariable("delftFlight3OuterRadius",9.25);
  Control.addVariable("delftFlight3OuterWall",0.5);

  Control.addVariable("delftFlight3FrontWall",0.7);
  Control.addVariable("delftFlight3FrontGap",4.4);
  Control.addVariable("delftFlight3FrontIWall",0.5);

  Control.addVariable("delftFlight3InterThick",0.2);
  Control.addVariable("delftFlight3InterWallThick",0.3);
  Control.addVariable("delftFlight3InterFrontWall",0.333);
  Control.addVariable("delftFlight3InterYOffset",77.2);
  Control.addVariable("delftFlight3InterMat","B4C");

  Control.addVariable("delftFlight3WallMat","Aluminium");
  Control.addVariable("delftFlight3GapMat","Void");

  // FLIGHT 4:  Lower 
  Control.addVariable("delftFlight4XStep",-(23.127+1.8));
  Control.addVariable("delftFlight4YStep",0.00);
  Control.addVariable("delftFlight4ZStep",24.5);
  Control.addVariable("delftFlight4XYAngle",26.0);
  Control.addVariable("delftFlight4ZAngle",0.0);

  Control.addVariable("delftFlight4Length",1000.0);
  Control.addVariable("delftFlight4InnerRadius",7.5);
  Control.addVariable("delftFlight4InnerWall",0.5);
  Control.addVariable("delftFlight4OuterRadius",9.25);
  Control.addVariable("delftFlight4OuterWall",0.7);

  Control.addVariable("delftFlight4FrontWall",0.7);
  Control.addVariable("delftFlight4FrontGap",4.4);
  Control.addVariable("delftFlight4FrontIWall",0.5);

  Control.addVariable("delftFlight4InterThick",0.2);
  Control.addVariable("delftFlight4InterWallThick",0.3);
  Control.addVariable("delftFlight4InterFrontWall",0.333);
  Control.addVariable("delftFlight4InterYOffset",77.2);
  Control.addVariable("delftFlight4InterMat","B4C");

  Control.addVariable("delftFlight4WallMat","Aluminium");
  Control.addVariable("delftFlight4GapMat","H2O");

  // Centre flight 5 
  Control.addVariable("delftFlight5XStep",-(23.127+1.8));
  Control.addVariable("delftFlight5YStep",0.0);
  Control.addVariable("delftFlight5ZStep",0.0);
  Control.addVariable("delftFlight5XYAngle",0.0);
  Control.addVariable("delftFlight5ZAngle",0.0);

  Control.addVariable("delftFlight5Length",1000.0);
  Control.addVariable("delftFlight5InnerRadius",10.3);
  Control.addVariable("delftFlight5InnerWall",0.4);
  Control.addVariable("delftFlight5OuterRadius",11.75);
  Control.addVariable("delftFlight5OuterWall",0.5);

  Control.addVariable("delftFlight5FrontWall",0.7);
  Control.addVariable("delftFlight5FrontGap",0.1);
  Control.addVariable("delftFlight5FrontIWall",0.5);

  Control.addVariable("delftFlight5InterThick",0.2);
  Control.addVariable("delftFlight5InterWallThick",0.3);
  Control.addVariable("delftFlight5InterFrontWall",0.333);
  Control.addVariable("delftFlight5InterYOffset",77.2);
  Control.addVariable("delftFlight5InterMat","B4C");

  Control.addVariable("delftFlight5WallMat","Aluminium");
  Control.addVariable("delftFlight5GapMat","Void");

  // FLIGHT 6:  Lower 
  Control.addVariable("delftFlight6XStep",-(23.127+1.8));
  Control.addVariable("delftFlight6YStep",0.0);
  Control.addVariable("delftFlight6ZStep",-24.5);
  Control.addVariable("delftFlight6XYAngle",21.0);
  Control.addVariable("delftFlight6ZAngle",0.0);

  Control.addVariable("delftFlight6Length",1000.0);
  Control.addVariable("delftFlight6InnerRadius",4.5);
  Control.addVariable("delftFlight6InnerWall",0.5);
  Control.addVariable("delftFlight6OuterRadius",9.25);
  Control.addVariable("delftFlight6OuterWall",0.7);

  Control.addVariable("delftFlight6FrontWall",0.7);
  Control.addVariable("delftFlight6FrontGap",4.4);
  Control.addVariable("delftFlight6FrontIWall",0.5);

  Control.addVariable("delftFlight6InterThick",0.2);
  Control.addVariable("delftFlight6InterWallThick",0.3);
  Control.addVariable("delftFlight6InterFrontWall",0.333);
  Control.addVariable("delftFlight6InterYOffset",77.2);
  Control.addVariable("delftFlight6InterMat","B4C");

  Control.addVariable("delftFlight6WallMat","Aluminium");
  Control.addVariable("delftFlight6GapMat","H2O");

  // HYDROGEN MODERATOR

  Control.addVariable("delftH2XStep",0.0);      
  Control.addVariable("delftH2YStep",10.0);      
  Control.addVariable("delftH2ZStep",0.0);      

  Control.addVariable("delftH2FrontRadius",-20.0);      // Radius of outer H2
  Control.addVariable("delftH2BackRadius",-20.0);      // Radius of outer H2
  Control.addVariable("delftH2Depth",5.00);          // Depth of the H2
  Control.addVariable("delftH2SideRadius",8.00);      // Depth of the H2
  Control.addVariable("delftH2InnerXShift",0.0);     // offset of the curve
  Control.addVariable("delftH2AlBack",0.70);        // Divide material 7mm
  Control.addVariable("delftH2AlFront",0.50);         // Front block
  Control.addVariable("delftH2AlSide",0.50);          // Side block
  Control.addVariable("delftH2AlTop",0.50);           // Top layer [away]
  Control.addVariable("delftH2AlBase",0.50);          // Target side
  Control.addVariable("delftH2SurfThick",0.8);       // H2 Para thickness
  Control.addVariable("delftH2InnerSurfDist",2.8);    // Displacement 

  Control.addVariable("delftH2ModTemp",20.0);         // Temperature of H2 
  Control.addVariable("delftH2ModMat","ParaH2");            // Liquid H2
  Control.addVariable("delftH2AlMat","Aluminium");              // Aluminium
  Control.addVariable("delftH2SurfMat","Aluminium");           // Aluminium


  // CONE MODERATOR

  Control.addVariable("coneXStep",0.0);      
  Control.addVariable("coneYStep",3.0);      
  Control.addVariable("coneZStep",0.0);      
  Control.addVariable("coneDepth",5.0);      
  Control.addVariable("coneLength",50.0);      
  Control.addVariable("coneInnerAngle",11.0);      
  Control.addVariable("coneOuterAngle",11.0);      
  Control.addVariable("coneFrontCut",10.0);      

  Control.addVariable("coneAlView",0.5);      
  Control.addVariable("coneFaceThick",1.0);      
  Control.addVariable("coneAlBack",1.0);      

  Control.addVariable("coneModTemp",20.0);         // Temperature of H2 
  Control.addVariable("coneModMat","ParaH2");            // Liquid H2
  Control.addVariable("coneAlMat","Aluminium");              // Aluminium

  // CONTAINER

  Control.addVariable("delftH2ContVacPosRadius",30.2);   //  First Vac layer
  Control.addVariable("delftH2ContVacNegRadius",60.2);   // 
  Control.addVariable("delftH2ContVacPosGap",2.3);       // 
  Control.addVariable("delftH2ContVacNegGap",1.2);       // 
  Control.addVariable("delftH2ContVacSide",0.3);   //

  Control.addVariable("delftH2ContAlPos",0.15);    // First Al layer
  Control.addVariable("delftH2ContAlNeg",0.15);    // 
  Control.addVariable("delftH2ContAlSide",0.15);    // Sides of view

  Control.addVariable("delftH2ContTerPos",0.2);    // Teriary GAP
  Control.addVariable("delftH2ContTerNeg",0.2);    // 
  Control.addVariable("delftH2ContTerSide",0.2);    //

  Control.addVariable("delftH2ContOutPos",0.2);    // Outer Al layer
  Control.addVariable("delftH2ContOutNeg",0.2);    // Actual curve
  Control.addVariable("delftH2ContOutSide",0.2);    // Sides of view

  Control.addVariable("delftH2ContClearPos",0.6);    // Actual curve
  Control.addVariable("delftH2ContClearNeg",0.6);    // Actual curve
  Control.addVariable("delftH2ContClearSide",0.6);    // Sides of view

  Control.addVariable("delftH2ContAlMat","Aluminium");            
  Control.addVariable("delftH2ContOutMat","Aluminium");           

  Control.addVariable("delftH2NGroove",0);      

  Control.addVariable("delftH2GrooveFace",-1);      
  Control.addVariable("delftH2GrooveYStep",-3.0);      
  Control.addVariable("delftH2GrooveZStep",0.0);      
  Control.addVariable("delftH2GrooveXYAngleA",25.0);      
  Control.addVariable("delftH2GrooveXYAngleB",-25.0);      
  Control.addVariable("delftH2GrooveSiMat","Void");      
  Control.addVariable("delftH2GrooveSiTemp",0.0);      
  Control.addVariable("delftH2GrooveHeight",8.0);        

  Control.addVariable("delftH2Groove1XStep",-4.0);      
  Control.addVariable("delftH2Groove2XStep",-2.0);      
  Control.addVariable("delftH2Groove3XStep",2.0);      
  Control.addVariable("delftH2Groove4XStep",4.0);      

  // Default Spherica model
  Control.addVariable("sphereH2XStep",0.0);      
  Control.addVariable("sphereH2YStep",14.0);      
  Control.addVariable("sphereH2ZStep",0.0);      

  Control.addVariable("sphereH2InnerRadius",5.1);   // Radius of inner H2
  Control.addVariable("sphereH2InnerAl",0.3);      // Radius of iner Al  H2
  Control.addVariable("sphereH2OuterRadius",8.1);      // Radius of outer H2
  Control.addVariable("sphereH2OuterAl",0.3);      
  Control.addVariable("sphereH2OutYShift",-0.50);   // Shift of centre

  Control.addVariable("sphereH2ForwardStep",3.55);   // Shift of centre
  Control.addVariable("sphereH2CapThick",1.75);      // Shift of centre

  Control.addVariable("sphereH2PipeLen",30.0);      
  Control.addVariable("sphereH2PipeRadius",1.0);      
  Control.addVariable("sphereH2PipeAlRadius",1.2);      

  Control.addVariable("sphereH2ModTemp",20.0);         // Temperature of H2 
  Control.addVariable("sphereH2ModMat","ParaH2");            // Liquid H2
  Control.addVariable("sphereH2AlMat","Aluminium");              // Aluminium

  Control.addVariable("R2InsertXStep",0.0);
  Control.addVariable("R2InsertYStep",100.0);
  Control.addVariable("R2InsertZStep",0.0);
  Control.addVariable("R2InsertZAngle",0.0);
  Control.addVariable("R2InsertXYAngle",0.0);
  Control.addVariable("R2InsertRadius",10.05);
  Control.addVariable("R2InsertLength",100.0);
  Control.addVariable("R2InsertMat","Aluminium");               // Al
  Control.addVariable("R2InsertNSlots",4);            // **FOUR**

  // Generics
  Control.addVariable("R2SlotEndThick",1.0);
  Control.addVariable("R2SlotGlassMat","Glass");
  Control.addVariable("R2SlotDivideThick",0.25);
  Control.addVariable("R2SlotNChannels",1);
  
  // ROG Beamline 
  Control.addVariable("R2Slot1XStep",0.0);
  Control.addVariable("R2Slot1ZStep",4.693);
  Control.addVariable("R2Slot1AxisAngle",0.0);
  Control.addVariable("R2Slot1ZAngle",1.002);
  Control.addVariable("R2Slot1XYAngle",0.0);
  Control.addVariable("R2Slot1XSize",10.0);
  Control.addVariable("R2Slot1ZSize",2.65);

  // NDP
  Control.addVariable("R2Slot2NChannels",1);
  Control.addVariable("R2Slot2XStep",4.175);
  Control.addVariable("R2Slot2ZStep",-1.232);
  Control.addVariable("R2Slot2AxisAngle",90.0);
  Control.addVariable("R2Slot2XYAngle",-2.2869);
  Control.addVariable("R2Slot2ZAngle",0.0);
  Control.addVariable("R2Slot2XSize",6.0);
  Control.addVariable("R2Slot2ZSize",1.05);

  // SESANS
  Control.addVariable("R2Slot3XStep",1.725);
  Control.addVariable("R2Slot3ZStep",-2.432);
  Control.addVariable("R2Slot3AxisAngle",90.0);
  Control.addVariable("R2Slot3XYAngle",-1.23736);
  Control.addVariable("R2Slot3ZAngle",0.0);
  Control.addVariable("R2Slot3XSize",10.0);
  Control.addVariable("R2Slot3ZSize",2.65);

  // SANS
  Control.addVariable("R2Slot4XStep",-1.725);
  Control.addVariable("R2Slot4ZStep",-2.432);
  Control.addVariable("R2Slot4AxisAngle",90.0);
  Control.addVariable("R2Slot4XYAngle",1.23736);
  Control.addVariable("R2Slot4ZAngle",0.0);
  Control.addVariable("R2Slot4XSize",10.0);
  Control.addVariable("R2Slot4ZSize",2.65);

  // BE SURROUND:
  Control.addVariable("R2RefXStep",0.0);
  Control.addVariable("R2RefYStep",0.0);
  Control.addVariable("R2RefZStep",0.0);
  Control.addVariable("R2RefActive",1);  
  Control.addVariable("R2RefLength",10.0);  
  Control.addVariable("R2RefInnerRadius",13.25);  
  Control.addVariable("R2RefOuterRadius",18.25);  
  Control.addVariable("R2RefMat","Be300K");  

  return;
}

}  // NAMESPACE setVariable
