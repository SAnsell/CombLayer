/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   build/TS2variables.cxx
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
#include "shutterVar.h"
#include "variableSetup.h"

namespace setVariable
{

void
TS2layout(FuncDataBase& Control)
  /*!
    Function to set the control variables and constants
    -- This version is for TS2
    \param Control :: Function data base to add constants too
  */
{
// -----------
// GLOBAL stuff
// -----------

  Control.addVariable("zero",0.0);      // Zero
  Control.addVariable("one",1.0);       // One

// TARGET STUFF
  Control.addVariable("sdefEnergy",800.0);  
// ----------
// TALLY stuff
// ----------

  Control.addVariable("gridTallyOrigin",Geometry::Vec3D(0,0,0));
  Control.addVariable("gridTallyXVec",Geometry::Vec3D(1,0,0));
  Control.addVariable("gridTallyZVec",Geometry::Vec3D(0,0,1));

  Control.addVariable("gridTallyYShift",1.0);
  Control.addVariable("gridTallyHSize",25.0);
  Control.addVariable("gridTallyVSize",25.0);
  Control.addVariable("gridTallyAOffset",0.0);   // Axis offset 
  Control.addVariable("gridTallyHOffset",0.0);   // Horrizontal (X) 
  Control.addVariable("gridTallyVOffset",0.0);   // Vertical offset
 
// ------------
// Target stuff
// ------------
  Control.addVariable("beamsize",1.7);      // Radius the beam
  Control.addVariable("beamDX",0.0);        // Shift in beam height
  Control.addVariable("beamDY",0.0);        // Shift in beam horrizontal

  // REFLECTOR
  Control.addVariable("reflectXYAngle",-45.0);   // x-y angle
  Control.addVariable("reflectXStep",0);      // x shift of reflector
  Control.addVariable("reflectYStep",10.0);     // y shift of reflector
  Control.addVariable("reflectZStep",0);      // z shift of reflector
  Control.addVariable("reflectXYSize",35.0);    // half width (xy direction)
  Control.addVariable("reflectZSize",35.0);     // half height
  Control.addVariable("reflectCornerAngle",45.0);     // End cut
  Control.addVariable("reflectCutSize",30.0*sqrt(2.0));     // End cut
  Control.addVariable("reflectMat","Be300K");     // End cut


// NEW COUPLED STUFF:
  
  Control.addVariable("grooveXStep",0.0);        //  Groove shift
  Control.addVariable("grooveYStep",4.8);        //  Groove shift
  Control.addVariable("grooveZStep",-12.9);      //  Groove shift
  Control.addVariable("grooveZAngle",0.0);       //  Overall angle
  Control.addVariable("grooveXYAngle",-72.0);     // Angle of the groove [relative to Y]
  Control.addVariable("grooveRadius",7.25);      // Radius of groove curve
  Control.addVariable("grooveInnerCut",4.15);    // Cut to the groove line
  Control.addVariable("grooveInnerWidth",8.3);   // Half width of groove 
  Control.addVariable("grooveInnerHeight",3.0);   // Height of groove 
  Control.addVariable("grooveInnerXShift",0.0);     // Centre of groove shift
  Control.addVariable("grooveInnerZShift",3.0);     // Centre of groove shift
  Control.addVariable("grooveIXYAngleE1",26.5);     // Right rotation
  Control.addVariable("grooveIXYAngleE5",26.5);     // Left rotation

  // Al section
  Control.addVariable("grooveAlInnerCurve",0.15);    // Actual curve
  Control.addVariable("grooveAlInnerSides",0.15);    // Sides of view
  Control.addVariable("grooveAlInnerUpDown",0.15);   // up/down
  Control.addVariable("grooveAlFront",0.50);         // Front block
  Control.addVariable("grooveAlSide",0.50);          // Side block
  Control.addVariable("grooveAlTop",0.50);           // Top layer [away]
  Control.addVariable("grooveAlBase",0.50);          // Target side

  Control.addVariable("grooveMethDepth",7.00);      // Depth of the CH4
  Control.addVariable("grooveMethWidth",19.60);     // Width of the CH4
  Control.addVariable("grooveMethHeight",12.0);     // Full Height
  //  Control.addVariable("grooveModMat","CH4inFoam");  // CH4 + 10% Al
  Control.addVariable("grooveModMat","Void");  // CH4 + 10% Al
  Control.addVariable("grooveAlMat","Aluminium");   // Aluminium mat
  Control.addVariable("grooveModTemp",26.0);        // Moderator temperature

  Control.addVariable("grooveFlightXStep",0.0);      // Step from centre
  Control.addVariable("grooveFlightZStep",3.0);      // Step from centre
  Control.addVariable("grooveFlightAngleXY1",26.5);  // Angle out
  Control.addVariable("grooveFlightAngleXY2",26.5);  // Angle out
  Control.addVariable("grooveFlightAngleZTop",2.5);  // Step down angle
  Control.addVariable("grooveFlightAngleZBase",0.0); // Step up angle
  Control.addVariable("grooveFlightHeight",3.3);     // Full height
  Control.addVariable("grooveFlightWidth",18.3);     // Full width

  Control.addVariable("groovePMWidth",25.8);     // Full width
  Control.addVariable("groovePMDepth",10.2);     // Full Depth 
  Control.addVariable("groovePMHeight",1.95);    // Full Height
  Control.addVariable("groovePMAlThick",0.3);    // Al skin thickness
  Control.addVariable("groovePMAlMat","Aluminium");        // Aluminium Material
  Control.addVariable("groovePMModMat","H2O");        // pre-mod Material
  Control.addVariable("groovePMModTemp",333.0);        // Temp of water

  // HYDROGEN MODERATOR

  Control.addVariable("hydrogenRadius",20.0);      // Radius of outer H2
  Control.addVariable("hydrogenDepth",5.00);        // Depth of the H2
  Control.addVariable("hydrogenWidth",19.60);          // Width of the CH4
  Control.addVariable("hydrogenHeight",12.0);         // Full Height
  Control.addVariable("hydrogenInnerXShift",0.0);     // offset of the curve
  Control.addVariable("hydrogenAlDivide",0.70);        // Divide material 7mm
  Control.addVariable("hydrogenAlFront",0.50);         // Front block
  Control.addVariable("hydrogenAlSide",0.50);          // Side block
  Control.addVariable("hydrogenAlTop",0.50);           // Top layer [away]
  Control.addVariable("hydrogenAlBase",0.50);          // Target side
  Control.addVariable("hydrogenSurfThick",0.8);       // H2 Para thickness
  Control.addVariable("hydrogenInnerSurfDist",2.8);    // Displacement 

  Control.addVariable("hydrogenModTemp",20.0);         // Temperature of H2 
  Control.addVariable("hydrogenModMat","ParaH2");            // Liquid H2
  Control.addVariable("hydrogenAlMat","Aluminium");              // Aluminium
  Control.addVariable("hydrogenSurfMat","ParaOrtho");        // 50/50 mix

  // ORTHOH Section [ if used flag : -orthoH ]
  Control.addVariable("orthoGrooveThick",0.2);       // 2mm on the groove side
  Control.addVariable("orthoGrooveWidth",9.0);       // Section on H2 side
  Control.addVariable("orthoGrooveHeight",3.0);      // Section on H2 side
  Control.addVariable("orthoHydroThick",0.2);        // 2mm on the hydrogen side
  Control.addVariable("orthoMat","ParaOrtho");       // 2mm on the hydrogen side

  // PIPES FOR COUPLED
  Control.addVariable("cplPipeXOffset",0.0);         // Pipe offset 
  Control.addVariable("cplPipeYOffset",1.2);         // Pipe offset [from divide]
  Control.addVariable("cplPipeFullLen",30.0);         // Pipe length

  Control.addVariable("cplPipeInnerRad",3.645);        // Inner 
  Control.addVariable("cplPipeInnerAlRad",3.805);      // Inner al
  Control.addVariable("cplPipeOutVacRad",4.285);       // Outer vac 
  Control.addVariable("cplPipeOutAlRad",4.445);        // Outer Al layer
  Control.addVariable("cplPipeOutRad",5.045);          // Pipe outer radius

  Control.addVariable("cplPipeInnerMat","Void");        // Inner 
  Control.addVariable("cplPipeInnerAlMat","Aluminium"); // Inner al
  Control.addVariable("cplPipeOutVacMat","Void");       // Outer vac 
  Control.addVariable("cplPipeOutAlMat","Aluminium");   // Outer Al layer
  Control.addVariable("cplPipeOutMat","Void");          // Pipe outer radius

  Control.addVariable("cplPipeHydXOff",0.0);         // Pipe offset 
  Control.addVariable("cplPipeHydYOff",1.7);         // Pipe offset [from divide]
  Control.addVariable("cplPipeHydLen",10.0);         // Pipe length
  Control.addVariable("cplPipeHydRad",1.065);        // Pipe outer radius
  Control.addVariable("cplPipeHydThick",0.2);        // Pipe Thickness

  Control.addVariable("hydroFlightXStep",0.0);      // Step from centre
  Control.addVariable("hydroFlightZStep",0.0);      // Step from centre
  Control.addVariable("hydroFlightAngleXY1",53.2);  // Angle out
  Control.addVariable("hydroFlightAngleXY2",9.9);  // Angle out
  Control.addVariable("hydroFlightAngleZTop",0.0);  // Step down angle
  Control.addVariable("hydroFlightAngleZBase",0.0); // Step up angle
  Control.addVariable("hydroFlightHeight",11.5);    // Full height
  Control.addVariable("hydroFlightWidth",14.3);     // Full width

  Control.addVariable("hydroPMWidth",25.8);     // Full width
  Control.addVariable("hydroPMDepth",10.2);     // Full Depth 
  Control.addVariable("hydroPMHeight",1.95);    // Full Height
  Control.addVariable("hydroPMAlThick",0.3);    // Al skin thickness
  Control.addVariable("hydroPMAlMat","Aluminium");        // Aluminium Material
  Control.addVariable("hydroPMModMat","H2O");        // Water
  Control.addVariable("hydroPMModTemp",333.0);    // 60C

  Control.addVariable("hornPMSideExt",3.2);     // Full width
  Control.addVariable("hornPMHeightExt",3.2);     // Full Depth 
  Control.addVariable("hornPMBackExt",0.0);    // Full Height
  Control.addVariable("hornPMForwardExt",3.2);    // Full Height
  Control.addVariable("hornPMVacInner",0.51);    // Vac inner thickness
  Control.addVariable("hornPMVacOuter",0.41);    // Vac outer thickness
  Control.addVariable("hornPMAlInner",0.35);    // Al skin thickness [inner]
  Control.addVariable("hornPMAlOuter",0.41);    // Al skin thickness [outer]
  Control.addVariable("hornPMWingLen",15.0);    // Al skin thickness [outer]
  Control.addVariable("hornPMAlMat","Aluminium");        // Aluminium Material
  Control.addVariable("hornPMModMat","H2O");        // Water
  Control.addVariable("hornPMModTemp",333.0);    // 60C

  // VACUUM LAYER
  Control.addVariable("cvacVacPosRadius",20.0);      // Radius of groove curve
  Control.addVariable("cvacVacNegRadius",20.0);      // Radius of hydro curve
  Control.addVariable("cvacVacPosGap",3.66666);      // Gap on groove side
  Control.addVariable("cvacVacNegGap",0.6);          // Gap on Hydrogen side
  Control.addVariable("cvacVacSide",0.5);            // Gap on Hydrogen side
  Control.addVariable("cvacVacTop",0.6);             // Gap on Ref top
  Control.addVariable("cvacVacBase",0.6);            // Gap on Target base

  // AL LAYER
  Control.addVariable("cvacAlPos",0.15);         // Gap on groove side
  Control.addVariable("cvacAlNeg",0.15);          // Gap on Hydrogen side
  Control.addVariable("cvacAlSide",0.3);          // Gap on Hydrogen side
  Control.addVariable("cvacAlTop",0.3);           // Gap on Ref top
  Control.addVariable("cvacAlBase",0.15);          // Gap on Target base

  // Tertiary LAYER
  Control.addVariable("cvacTerPos",0.2);         // Gap on groove side
  Control.addVariable("cvacTerNeg",0.2);          // Gap on Hydrogen side
  Control.addVariable("cvacTerSide",0.2);          // Gap on Hydrogen side
  Control.addVariable("cvacTerTop",0.2);           // Gap on Ref top
  Control.addVariable("cvacTerBase",0.2);          // Gap on Target base

  // Outer Al LAYER
  Control.addVariable("cvacOutPos",0.10);         // Gap on groove sidex
  Control.addVariable("cvacOutNeg",0.10);          // Gap on Hydrogen side
  Control.addVariable("cvacOutSide",0.30);          // Gap on Hydrogen side
  Control.addVariable("cvacOutTop",0.30);           // Gap on Ref top
  Control.addVariable("cvacOutBase",0.30);          // Gap on Target base

  // Outer Al LAYER
  Control.addVariable("cvacClearPos",0.30);        // Clear Pos
  Control.addVariable("cvacClearNeg",0.30);        // Clear Neg
  Control.addVariable("cvacClearSide",0.30);       // Clearance on side
  Control.addVariable("cvacClearTop",0.30);        // Clearance on top
  Control.addVariable("cvacClearBase",0.30);       // Clearance on Base

  Control.addVariable("cvacAlMat","Aluminium");           // Inner Material
  Control.addVariable("cvacOutMat","Aluminium");          // Outer Material 

  // PLACEHOLDER
  Control.addVariable("decPlateXStep",0.0);        //  X [across] shift
  Control.addVariable("decPlateYStep",4.6);        //  Y [target] shift
  Control.addVariable("decPlateZStep",11.862);     //  Z Shift 
  Control.addVariable("decPlateZAngle",0.0);       //  Tilt angle
  Control.addVariable("decPlateXYAngle",57.0);     // Angle  [relative to Y]
  Control.addVariable("decPlateWidth",14.0);       //  Tilt angle
  Control.addVariable("decPlateHeight",12.0);       //  Tilt angle
  Control.addVariable("decPlateDepth",5.0);       //  Tilt angle
  Control.addVariable("decPlateDefMat","Aluminium");       //  Tilt angle

    //  NEW DECOUPLED MODERATOR

  Control.addVariable("decoupledXStep",0.0);        //  X [across] shift
  Control.addVariable("decoupledYStep",4.6);        //  Y [target] shift
  Control.addVariable("decoupledZStep",11.862);     //  Z Shift 
  Control.addVariable("decoupledZAngle",0.0);       //  Tilt angle
  Control.addVariable("decoupledXYAngle",57.0);     // Angle  [relative to Y]
  Control.addVariable("decoupledEastRadius",30.2);  // Radius of East curve
  Control.addVariable("decoupledWestRadius",30.2);  // Radius of West curve

  Control.addVariable("decoupledWidth",14.4);   // Full width
  Control.addVariable("decoupledHeight",12.0);   // Full height
  Control.addVariable("decoupledEastDepth",2.6);   // East from centre
  Control.addVariable("decoupledWestDepth",3.3);   // West from centre

  Control.addVariable("decoupledAlCurve",0.3);    // Actual curve
  Control.addVariable("decoupledAlSides",0.3);    // Sides of view
  Control.addVariable("decoupledAlUpDown",0.3);   // up/down

  Control.addVariable("decLayerCentMat","CH4Liq");   // liquid methane
  Control.addVariable("decLayerCentTemp",90.0);   // Central temperature
  Control.addVariable("decLayerNLayers",4);   // up/down
  Control.addVariable("decLayer1Thick",0.3);   // He layer
  Control.addVariable("decLayer1Temp",20.0);   // He layer
  Control.addVariable("decLayer1Mat","helium");       // First layer
  Control.addVariable("decLayer2Thick",0.15);   // Al layer
  Control.addVariable("decLayer2Temp",20.0);   
  Control.addVariable("decLayer2Mat","Aluminium");   
  Control.addVariable("decLayer3Thick",0.4);   // Type 2 Methane
  Control.addVariable("decLayer3Temp",20.0);   
  Control.addVariable("decLayer3Mat","CH4AlTypeII");   
  Control.addVariable("decLayer4Thick",1.0);   // Type 2 Methane
  Control.addVariable("decLayer4Temp",20.0);   
  Control.addVariable("decLayer4Mat","CH4+Al26K");   

  Control.addVariable("dvacVacPosRadius",30.2);   //  First Vac layer
  Control.addVariable("dvacVacNegRadius",30.2);   // 
  Control.addVariable("dvacVacPosGap",0.3);       // 
  Control.addVariable("dvacVacNegGap",0.3);       // 
  Control.addVariable("dvacVacLSide",1.3);   //
  Control.addVariable("dvacVacRSide",0.3);   //
  Control.addVariable("dvacVacBase",0.3);    // 
  Control.addVariable("dvacVacTop",0.3);     // 

  Control.addVariable("dvacAlPos",0.15);    // First Al layer
  Control.addVariable("dvacAlNeg",0.15);    // 
  Control.addVariable("dvacAlSide",0.15);    // Sides of view
  Control.addVariable("dvacAlBase",0.15);    // up/down
  Control.addVariable("dvacAlTop",0.15);     // up/down

  Control.addVariable("dvacTerPos",0.2);    // Teriary GAP
  Control.addVariable("dvacTerNeg",0.2);    // 
  Control.addVariable("dvacTerSide",0.2);    //
  Control.addVariable("dvacTerBase",0.2);   // 
  Control.addVariable("dvacTerTop",0.2);   // up/down

  Control.addVariable("dvacOutPos",0.2);    // Outer Al layer
  Control.addVariable("dvacOutNeg",0.2);    // Actual curve
  Control.addVariable("dvacOutSide",0.2);    // Sides of view
  Control.addVariable("dvacOutBase",0.2);   // up/down
  Control.addVariable("dvacOutTop",0.2);   // up/down

  Control.addVariable("dvacClearPos",0.6);    // Actual curve
  Control.addVariable("dvacClearNeg",0.6);    // Actual curve
  Control.addVariable("dvacClearSide",0.6);    // Sides of view
  Control.addVariable("dvacClearBase",0.6);   // up/down
  Control.addVariable("dvacClearTop",0.6);   // up/down
  Control.addVariable("dvacAlMat","Aluminium");             // Aluminium mat
  Control.addVariable("dvacOutMat","Aluminium");             // Aluminium mat

  Control.addVariable("decoupledModMat","CH4inFoam");           // CH4 + 10% Al 
  Control.addVariable("decoupledAlMat","Aluminium");             // Aluminium mat
  Control.addVariable("decoupledModTemp",26.0);        // Moderator temperature
  
  // Poisoning:
  Control.addVariable("decPoisonNBlades",7);        // Number of blades
  Control.addVariable("decPoisonBladeWidth",0.15);    // Thickness [total]
  Control.addVariable("decPoisonBladeGap",0.7);       // Space of blade
  Control.addVariable("decPoisonAbsThick",0.05);      // Space in blade
  Control.addVariable("decPoisonYLength",1.0);       // Planeare
  Control.addVariable("decPoisonZLength",-1.0);       // -ve : to to bottom
  Control.addVariable("decPoisonXStep",0.0);       
  Control.addVariable("decPoisonYStep",0.0);       
  Control.addVariable("decPoisonZStep",0.0);       

  Control.addVariable("decPoisonModTemp",26.0);           // CH4 + 10% Al [+Ar]

  Control.addVariable("decPoisonModMat","CH4inFoam");  // 
  Control.addVariable("decPoisonBladeMat","SiCrystal");  // Silicon
  Control.addVariable("decPoisonAbsMat","Gadolinium");   // GD
  
  Control.addVariable("wishFlightXStep",0.0);       // Step from centre
  Control.addVariable("wishFlightZStep",0.0);       // Step from centre
  Control.addVariable("wishFlightAngleXY1",11.01);  // Angle out
  Control.addVariable("wishFlightAngleXY2",27.34);  // Angle out
  Control.addVariable("wishFlightAngleZTop",0.0);   // Step down angle
  Control.addVariable("wishFlightAngleZBase",0.0);  // Step up angle
  Control.addVariable("wishFlightHeight",11.5);     // Full height
  Control.addVariable("wishFlightWidth",14.3);      // Full width
  
  Control.addVariable("wishFlightNLiner",2);           // Number of layers
  Control.addVariable("wishFlightLinerThick1",0.5);    // Thickness
  Control.addVariable("wishFlightLinerThick2",0.12);   // Thickness
  Control.addVariable("wishFlightLinerMat1","Aluminium");  
  Control.addVariable("wishFlightLinerMat2","Cadmium");

  Control.addVariable("narrowFlightXStep",0.0);       // Step from centre
  Control.addVariable("narrowFlightZStep",0.0);       // Step from centre
  Control.addVariable("narrowFlightAngleXY1",27.06);  // Angle out
  Control.addVariable("narrowFlightAngleXY2",12.02);  // Angle out
  Control.addVariable("narrowFlightAngleZTop",0.0);   // Step down angle
  Control.addVariable("narrowFlightAngleZBase",0.0);  // Step up angle
  Control.addVariable("narrowFlightHeight",11.5);     // Full height
  Control.addVariable("narrowFlightWidth",14.3);      // Full width
  Control.addVariable("narrowFlightInnerMat","Void");  // Material

  Control.addVariable("decPMWidth",25.8);     // Full width
  Control.addVariable("decPMDepth",10.2);     // Full Depth 
  Control.addVariable("decPMHeight",0.75);    // Full Height
  Control.addVariable("decPMAlThick",0.1);    // Al skin thickness
  Control.addVariable("decPMAlMat","Aluminium");        // Aluminium Material
  Control.addVariable("decPMModMat","H2O");      // Aluminium Material
  Control.addVariable("decPMModTemp",333.0);  // Aluminium Material
  
  // DECOUPLED PIPE
  Control.addVariable("decPipeXOffset",0.58);         // Pipe offset [east]
  Control.addVariable("decPipeYOffset",0.0);          // Pipe offset [from divide]
  Control.addVariable("decPipeHeInXStep",2.0);        // In pipe movment
  Control.addVariable("decPipeHeInYStep",0.0);        // In pipe movment
  Control.addVariable("decPipeHeOutXStep",-2.0);       // In pipe movment
  Control.addVariable("decPipeHeOutYStep",0.0);        // In pipe movment
  // Drop by 10
  Control.addVariable("decPipeHeTrack0",Geometry::Vec3D(0,0,-4.0));  
  Control.addVariable("decPipeHeTrack1",Geometry::Vec3D(3.0,0,-4.0));  
  Control.addVariable("decPipeHeTrack2",Geometry::Vec3D(3.0,0,-10.0));  
  Control.addVariable("decPipeHeTrack3",Geometry::Vec3D(0.0,0,-10.0));  
  Control.addVariable("decPipeHeTrack4",Geometry::Vec3D(0.0,0,-6.0));  
  Control.addVariable("decPipeHeTrack5",Geometry::Vec3D(-4.0,0,-6.0));  
  Control.addVariable("decPipeHeTrack6",Geometry::Vec3D(-4.0,0,-10.0));  
  Control.addVariable("decPipeHeTrack7",Geometry::Vec3D(-7.0,0,-10.0));  
  Control.addVariable("decPipeHeTrack8",Geometry::Vec3D(-7.0,0,-4.0));  
  Control.addVariable("decPipeHeTrack9",Geometry::Vec3D(-4.0,0.0,-4.0));  

  Control.addVariable("decPipeYOffset",0.0);          // Pipe offset [from divid
  Control.addVariable("decPipeFullLen",20.0);         // Pipe length

  Control.addVariable("decPipeOutRad",4.735);         // Pipe Outer radius
  Control.addVariable("decPipeOutAlRad",3.735);        
  Control.addVariable("decPipeOutVacRad",3.535);        
  Control.addVariable("decPipeInnerAlRad",3.235);        
  Control.addVariable("decPipeInnerRad",2.935);        
  Control.addVariable("decPipeHeRad",0.3);        
  Control.addVariable("decPipeHeAlRad",0.45);        

  Control.addVariable("decPipeInnerMat","Void");        // Inner 
  Control.addVariable("decPipeInnerAlMat","Aluminium");      // Inner al
  Control.addVariable("decPipeOutVacMat","Void");       // Outer vac 
  Control.addVariable("decPipeOutAlMat","Aluminium");        // Outer Al layer
  Control.addVariable("decPipeOutMat","Void");          // Pipe outer radius
  Control.addVariable("decPipeHeMat","Void");          // Pipe outer radius
  Control.addVariable("decPipeHeAlMat","Aluminium");          // Pipe outer radius
  

  // CD BUCKET:
  Control.addVariable("cdBucketXStep",0.0);      // Offset
  Control.addVariable("cdBucketYStep",-4.8);     // Rotation angle  
  Control.addVariable("cdBucketRadius",17.5);    // Rotation angle  
  Control.addVariable("cdBucketThick",0.12);     // Rotation angle  
  Control.addVariable("cdBucketOpenZ",5.0);      // Rotation angle  
  Control.addVariable("cdBucketTopZ",28.0);      // Top of bucket
  Control.addVariable("cdBucketMat","Cadmium");  // Material

  // REFLECTOR COOL PADS:
  Control.addVariable("reflectNPads",1);      // Number of cooling pads
  Control.addVariable("coolPad1FixIndex",2);   // Index
  Control.addVariable("coolPadXStep",0.0);   
  Control.addVariable("coolPad1ZStep",-17.5);   
  Control.addVariable("coolPad1Thick",3.75);   
  Control.addVariable("coolPad1Width",44.0);   
  Control.addVariable("coolPad1Height",26.8);   
  Control.addVariable("coolPadNZigZag",8);   // Index
  Control.addVariable("coolPadCent1",Geometry::Vec3D(-17,1.87,-8));
  Control.addVariable("coolPadCent2",Geometry::Vec3D(13,1.87,-7)); 
  Control.addVariable("coolPadCent3",Geometry::Vec3D(13,1.87,-3)); 
  Control.addVariable("coolPadCent4",Geometry::Vec3D(-17,1.87,-2));
  Control.addVariable("coolPadCent5",Geometry::Vec3D(-17,1.87,2));
  Control.addVariable("coolPadCent6",Geometry::Vec3D(13,1.87,3)); 
  Control.addVariable("coolPadCent7",Geometry::Vec3D(13,1.87,7)); 
  Control.addVariable("coolPadCent8",Geometry::Vec3D(-17,1.87,8));
  Control.addVariable("coolPadIWidth",2.0);   // 
  Control.addVariable("coolPadIDepth",0.5);   // 
  Control.addVariable("coolPadIMat","H2O");   // Inner material (light water)
  Control.addVariable("coolPadMat","Aluminium");   

  // REFLECTOR CUT SYSTEM
  Control.addVariable("chipIRCutActive",1);          // Flag for building
  Control.addVariable("chipIRCutXYAngle",46.0);      // Rotation angle
  Control.addVariable("chipIRCutZAngle",4.76);       // Rotation angle
  Control.addVariable("chipIRCutTargetDepth",4.55);  // Depth down target
  Control.addVariable("chipIRCutTargetOut",12.0);    // Rotation angle
  Control.addVariable("chipIRCutRadius",2.55);       // Radius [outer]
  Control.addVariable("chipIRCutMat","Void");        // Material [void]

  // VOID VESSEL:
  // NEED To move 168 and 169 out by : 4.2cm on BOTH:
  // shift is 17.59 (chip side) and 7.79 (non-chipsied)
  Control.addVariable("voidCoupleEastShift",4.2);        // Void extra space on East Coupled ()
  Control.addVariable("voidCoupleWestShift",4.2);        // Void extra space on West Coupled ()
  Control.addVariable("voidDCTopShift",4.2);        // Void extra space on Top window
  Control.addVariable("voidDCLowChipShift",17.59);  // Void extra space on Low window (chip)
  Control.addVariable("voidDCLowW4Shift",7.79);     // Void extra space on Low window (non-chip)
  // This is not uniform UPDATE 
  Control.addVariable("voidThick",7.50);        // Void vessel thickness 

  Control.addVariable("voidGap",5.00);          // Void vessel Gap surround
  Control.addVariable("voidTop",50.1);          // Void vessel top height
  Control.addVariable("voidBase",122.6);        // Void outer base
  Control.addVariable("voidSide",72.50);        // Void outer side
  Control.addVariable("voidFront",120.85);      // Void vessel front
  Control.addVariable("voidBack",187.0);        // Void vessel back
  Control.addVariable("voidFDepth",23.7);       // Void vessel Depth of rounding
  Control.addVariable("voidFAngle",30.0);       // Void vessel Angle 
  Control.addVariable("voidWindowView",9.0);    // Void vessel window gaps

  Control.addVariable("voidWindowWTopLen",94.50); // Window West Top length
  Control.addVariable("voidWindowETopLen",76.50); // Window East Top length
  Control.addVariable("voidWindowWLowLen",94.50); // Window West Lower length
  Control.addVariable("voidWindowELowLen",121.50); // Window Ease Lower length

  Control.addVariable("voidWindowWTopZ",12.10); // Window West Top Z off
  Control.addVariable("voidWindowETopZ",12.10); // Window East Top Z off
  Control.addVariable("voidWindowWLowZ",-12.10); // Window West Lower Z off
  Control.addVariable("voidWindowELowZ",-12.10); // Window Ease Lower Z off 
  
  // Offsets towards EPB [centres]:
  Control.addVariable("voidWindowWTopOff",40.50);    // West Top Y off
  Control.addVariable("voidWindowETopOff",-53.25);         // East Top Y off
  Control.addVariable("voidWindowWLowOff",-44.20);     // West Lower Y off
  Control.addVariable("voidWindowELowOff",22.05);     // Ease Lower Y off 
  // Angle views
  Control.addVariable("voidWindowAngleLen",16.0); // Angle view both the same
  Control.addVariable("voidWindowAngleROffset",-6.0); // Right offset
  Control.addVariable("voidWindowAngleLOffset",-6.0); // Left offset
  Control.addVariable("voidWindowThick",0.50);  // Void vessel window thickness
  Control.addVariable("voidWindowMat","Aluminium");       // Void vessel window material
  Control.addVariable("voidMat","Stainless304");       // Void vessel window material
  Control.addVariable("voidXoffset",4.475);     // Shutter offset to target centre

  Control.addVariable("portSize",9.0);           // viewport size

  Control.addVariable("t2TargetBoreRadius",3.70);  // Master bore  
  // TARGET of TS2
  Control.addVariable("t2TargetXStep",0.0);           // Step ref centre
  Control.addVariable("t2TargetYStep",4.2);           // Step ref centre
  Control.addVariable("t2TargetZStep",0.0);           // Step ref centre
  Control.addVariable("t2TargetMainLength",33.0);       // Length from 
  Control.addVariable("t2TargetCoreRadius",2.814);      // W radius 
  Control.addVariable("t2TargetSurfThick",0.3);         // skin layer
  Control.addVariable("t2TargetwSphDisplace",1.0025);   // W Centre
  Control.addVariable("t2TargetwSphRadius",2.3975);     // W radius 
  Control.addVariable("t2TargetwPlaneCut",1.6975);      // W plane cuts
  Control.addVariable("t2TargetwPlaneAngle",45.0);      // W plane angle rot

  Control.addVariable("t2TargetCladThick",0.086);         // Ta radius 
  Control.addVariable("t2TargetWaterThick",0.2);        // Water radius
  Control.addVariable("t2TargetPressureThick",0.3);     // Presure radius

  // Front cladding
  Control.addVariable("t2TargetTaSphDisplace",0.600);    // [Diag 233]
  Control.addVariable("t2TargetTaSphRadius",3.10);       // Ta radius 
  Control.addVariable("t2TargetTaPlaneCut",1.8975);      // W plane cuts
  
  // XFlow:
  Control.addVariable("t2TargetXFlowCyl",3.20);   // Outer cylinder of XFlow
  Control.addVariable("t2TargetXFlowOutDisplace",1.4); // Outer Sphere
  // From diage 235: fig4.
  Control.addVariable("t2TargetXFlowOutMidRadius",2.9);   // Top cap cut
  Control.addVariable("t2TargetXFlowOutRadius",3.1);   // Outer Sphere

  Control.addVariable("t2TargetXFlowInnerRadius",2.9);   // Cut for inner water
  Control.addVariable("t2TargetXFlowInnerDisplace",0.6); // Cut for inner water
  Control.addVariable("t2TargetXFlowIPlaneCut",1.5);     // Cut for inner water
  Control.addVariable("t2TargetXFlowOutCutAngle",-45.0);    // Angle or rotate

  Control.addVariable("t2TargetXFlowOPlaneCut",1.9);    // Cut left/right

  Control.addVariable("t2TargetTCapInnerRadius",3.10); 
  Control.addVariable("t2TargetTCapOuterRadius",3.25); // Outer Sphere
  Control.addVariable("t2TargetTCapDisplace",1.4);     // Outer Sphere

  Control.addVariable("t2CapYOffset",4.5);      // Length from flange

  Control.addVariable("t2TargetWMat","Tungsten");             // Solid Tungsten
  Control.addVariable("t2TargetTaMat","Tantalum");            // Solid Ta
  Control.addVariable("t2TargetWaterMat","H2O");         // Light water

  Control.addVariable("t2TargetTargetTemp",650.0);     // Inner core temp
  Control.addVariable("t2TargetWaterTemp",350.0);      // Water temp
  Control.addVariable("t2TargetExternTemp",330.0);     // Outer temp
  
  Control.addVariable("t2TargetFlangeYDist",60.20);    // Start distance from front
  Control.addVariable("t2TargetFlangeYStep",0.3);      // Start distance from front
  Control.addVariable("t2TargetFlangeRadius",5.750);     // Radius outer
  Control.addVariable("t2TargetFlangeLen",8.70);         // Full size 
  Control.addVariable("t2TargetFlangeThick",2.5);        // Join size [Guess]
  Control.addVariable("t2TargetFlangeClear",0.6);        // Flange clearange
  Control.addVariable("t2TargetFlangeMat","Stainless304");   // Flange material
  Control.addVariable("t2TargetNLayers",0);            // Flange material

  // MOLY SECTION:
  Control.addVariable("tMolyNPlates",2);
  Control.addVariable("tMolyPYThick",1.0);
  Control.addVariable("tMolyP1YDist",12.50);       // distance from flat
  Control.addVariable("tMolyP2YDist",16.50);       // distance from flat
  Control.addVariable("tMolyP3YDist",4.50);       // distance from flat
  Control.addVariable("tMolyP4YDist",5.50);       // distance from flat
  Control.addVariable("tMolyPMat","Molybdimum");

  Control.addVariable("tMolyNCutSph",1);
  Control.addVariable("tMolyCutSph1Radius",4.8);
  Control.addVariable("tMolyCutSph1Dist",0.4);
  Control.addVariable("tMolyCutSph1Cent",Geometry::Vec3D(0,4.5,0));
  Control.addVariable("tMolyCutSph1Axis",Geometry::Vec3D(0,1,0));
  Control.addVariable("tMolyCutSphMat","Molybdimum");

  Control.addVariable("tMolyNCone",1);
  Control.addVariable("tMolyCone1AngleA",45.0);
  Control.addVariable("tMolyCone1AngleB",45.0);
  Control.addVariable("tMolyCone1Dist",1.0);
  Control.addVariable("tMolyCone1Cent",Geometry::Vec3D(0,8,0));
  Control.addVariable("tMolyCone1Axis",Geometry::Vec3D(0,1,0));
  Control.addVariable("tMolyCone1Mat","Molydbimum");

  // SHUTTER VESSEL:
  Control.addVariable("voidXoffset",4.475);       // Shutter offset to target centre

  Control.addVariable("shutterUpperSteel",852.1);     // top thickness
  Control.addVariable("shutterLowerSteel",1000.0);    // base thickness
  Control.addVariable("shutterHeight",131.4);         // Total shutter height
  Control.addVariable("shutterDepth",131.4);         // Total shutter height
  Control.addVariable("shutterWidth",28.00);          // Full width

  Control.addVariable("shutterVoidZOffset",0.0);        // centre of steel hole
 
  Control.addVariable("shutterVoidHeight",28.0);             // Void height
  Control.addVariable("shutterVoidHeightInner",18.00);     // Gap height
  Control.addVariable("shutterVoidHeightOuter",22.30);     // Gap height
  Control.addVariable("shutterVoidWidthInner",18.00);      // Gap width
  Control.addVariable("shutterVoidWidthOuter",24.00);      // Gap width
  Control.addVariable("shutterVoidDivide",66.00);             // Smaller gap len
  // 
  Control.addVariable("shutterSteelMat","CastIron");         // Cast iron

  Control.addVariable("shutter1Height",120.0);         // New drawing 8711-300
  Control.addVariable("shutter1Depth",120.0);         // New drawing 8711-300
  Control.addVariable("shutter1VoidHeightInner",25.0);       // ChipIR size
  Control.addVariable("shutter1VoidHeightOuter",25.0);       // ChipIR size
  Control.addVariable("shutter1VoidDivide",-1.0);          // ChipIR no divide
  Control.addVariable("shutter1VoidWidthInner",21.00);      // Gap width

  Control.addVariable("shutterClearGap",0.6);      // Gap out step
  Control.addVariable("shutterClearBoxStep",2.0);      // Gap out step
  Control.addVariable("shutterClearBoxLen",25.0);      // Gap length of block
  Control.addVariable("shutterClearNStep",2);      // Gap length of block
  Control.addVariable("shutterClearCent0",50.0);      
  Control.addVariable("shutterClearCent1",150.0);     
  
  Control.addVariable("shutter1VoidZOffset",-30.0);
  Control.addVariable("shutter2VoidZOffset",0.0);

  Control.addVariable("shutter1GapSize",25);        // ChipIR size
  Control.addVariable("shutterGapSize",22.30);      // Gap size
  Control.addVariable("shutter1ZAngle",0.0);        // shutter slope/ up/down
  //  Control.addVariable("shutter1centreZoffset",7.0); // Central shutter
  Control.addVariable("shutter1Closed",0);          // Open shutter (false)
  Control.addVariable("shutter2Closed",1);          // closed shutter (true)
  Control.addVariable("shutterClosed",0);           // Open shutter [imp:0]
  Control.addVariable("shutter1ClosedZOffset",42.0);   // Closed distance
  Control.addVariable("shutter2ClosedZOffset",28.0);   // Closed distance
  Control.addVariable("shutterClosedZOffset",28.0);    // Closed distance
  
  // ALL SHUTTERS HAVE DIFFERENT POSITIONS:
  Control.addVariable("shutter1OpenZShift",48.86);    // ChipIR
  Control.addVariable("shutter2OpenZShift",11.86);    // W2 : 
  Control.addVariable("shutter3OpenZShift",11.86);    // W3
  Control.addVariable("shutter4OpenZShift",11.86);    // W4
  Control.addVariable("shutter5OpenZShift",-12.90);   // W5 [Hydrogen]
  Control.addVariable("shutter6OpenZShift",-12.90);   // W6
  Control.addVariable("shutter7OpenZShift",-12.90);   // W7
  Control.addVariable("shutter8OpenZShift",-12.90);   // W8
  Control.addVariable("shutter9OpenZShift",-12.90);   // W9
  Control.addVariable("shutter10OpenZShift",-9.90);   // E1 Zoom [Groove]
  Control.addVariable("shutter11OpenZShift",-9.90);   // E2 
  Control.addVariable("shutter12OpenZShift",-9.90);   // E3
  Control.addVariable("shutter13OpenZShift",-9.90);   // E4 
  Control.addVariable("shutter14OpenZShift",-9.90);   // E5
  Control.addVariable("shutter15OpenZShift",11.86);   // E6  [Optional]
  Control.addVariable("shutter16OpenZShift",11.86);
  Control.addVariable("shutter17OpenZShift",11.86);
  Control.addVariable("shutter18OpenZShift",11.86);

  Control.addVariable("shutter1XYAngle",46.00);       // Angle W1
  Control.addVariable("shutter2XYAngle",59.00);       // Angle W2
  Control.addVariable("shutter3XYAngle",72.00);       // Angle W3
  Control.addVariable("shutter4XYAngle",85.00);       // Angle W4
  Control.addVariable("shutter5XYAngle",98.00);       // Hydrogen [W5]
  Control.addVariable("shutter6XYAngle",111.00);      // W6
  Control.addVariable("shutter7XYAngle",124.00);      // W7
  Control.addVariable("shutter8XYAngle",137.00);      // W8
  Control.addVariable("shutter9XYAngle",150.00);      // W9
  Control.addVariable("shutter10XYAngle",-46.00);     // E1  [Groove]
  Control.addVariable("shutter11XYAngle",-59.00);     // E2
  Control.addVariable("shutter12XYAngle",-72.00);     // E3
  Control.addVariable("shutter13XYAngle",-85.00);     // E4
  Control.addVariable("shutter14XYAngle",-98.00);     // E5
  Control.addVariable("shutter15XYAngle",-111.00);    // E6 [Groove/broad]
  Control.addVariable("shutter16XYAngle",-124.00);    // E7 [Broad]
  Control.addVariable("shutter17XYAngle",-137.00);    // E8
  Control.addVariable("shutter18XYAngle",-150.00);    // E9                

  //  Control.addVariable("shutterInsertChipGap",25);  // Shutter Insert thickness
  Control.addVariable("shutterHWidth",12.0);       // Shutter Half width
  Control.addVariable("shutterMat","CastIron");    // shutter Material
  Control.addVariable("shutterSurMat","CastIron"); // shutter surround mat
  Control.addVariable("shutterSupportMat","CastIron");  // shutter support mat

  // Standard shutter:
  Control.addVariable("shutterBlockFlag",3);         // Flag 1:top/ 2 Base
  Control.addVariable("shutterBlockCent1",22.5);
  Control.addVariable("shutterBlockCent2",177.5);
  Control.addVariable("shutterBlockLen",30.0);
  Control.addVariable("shutterBlockHeight",25.0);
  Control.addVariable("shutterBlockVGap",7.50);
  Control.addVariable("shutterBlockHGap",3.0);
  Control.addVariable("shutterBlockMat","Concrete");
  // ChipIR shutter:
  
  Control.addVariable("shutterBlock1Flag",1);  // only top
  Control.addVariable("shutterBlock1Cent1",32.5);
  Control.addVariable("shutterBlock1Cent2",175.0);
  Control.addVariable("shutterBlock1Cent3",190.0);
  Control.addVariable("shutterBlock1Len1",15.0);
  Control.addVariable("shutterBlock1Len2",10.0);
  Control.addVariable("shutterBlock1Len3",20.0);
  Control.addVariable("shutterBlock1Height",36.0);
  Control.addVariable("shutterBlock1VGap",8.0);
  Control.addVariable("shutterBlock1HGap",1.5);
  Control.addVariable("shutterBlock1Mat1","Aluminium"); 
  Control.addVariable("shutterBlock1Mat2","Aluminium");
  Control.addVariable("shutterBlock1Mat3","B-Poly");

  // BULK INSERT
  Control.addVariable("bulkInsertIHeight",56.25);
  Control.addVariable("bulkInsertIWidth",32.50);
  Control.addVariable("bulkInsertOHeight",61.25);
  Control.addVariable("bulkInsertOWidth",37.50);
  Control.addVariable("bulkInsertZOffset",0.00);
  Control.addVariable("bulkInsert10InnerMat","Stainless304");
  Control.addVariable("bulkInsert10OuterMat","Stainless304");
  Control.addVariable("bulkInsert11InnerMat","Stainless304");
  Control.addVariable("bulkInsert11OuterMat","Stainless304");
  Control.addVariable("bulkInsert2InnerMat","Stainless304");
  Control.addVariable("bulkInsert2OuterMat","Stainless304");

  Control.addVariable("bulkInsertImpZero",0);         // All importance 1

  // TORPEDO
  Control.addVariable("torpedoHeight",18.00);
  Control.addVariable("torpedoWidth",18.00);
  Control.addVariable("torpedo0ZOffset",11.86);    // ChipIR
  Control.addVariable("torpedo1ZOffset",11.86);    // W2 : 
  Control.addVariable("torpedo2ZOffset",11.86);    // W3
  Control.addVariable("torpedo3ZOffset",11.86);    // W4
  Control.addVariable("torpedo4ZOffset",-12.90);    // W5 [Hydrogen]
  Control.addVariable("torpedo5ZOffset",-12.90);    // W6
  Control.addVariable("torpedo6ZOffset",-12.90);    // W7
  Control.addVariable("torpedo7ZOffset",-12.90);    // W8
  Control.addVariable("torpedo8ZOffset",-12.90);    // W9
  Control.addVariable("torpedo9ZOffset",-9.90);   // E1 Zoom [Groove]
  Control.addVariable("torpedo10ZOffset",-9.90);   // E2 
  Control.addVariable("torpedo11ZOffset",-9.90);   // E3
  Control.addVariable("torpedo12ZOffset",-9.90);   // E4 
  Control.addVariable("torpedo13ZOffset",-9.90);   // E5
  Control.addVariable("torpedo14ZOffset",11.86);   // E6  [Optional]
  Control.addVariable("torpedo15ZOffset",11.86);
  Control.addVariable("torpedo16ZOffset",11.86);
  Control.addVariable("torpedo17ZOffset",11.86);
  
  // BULK VESSEL:
  Control.addVariable("bulkTorpedoRadius",166.0);    // Torpedo level
  Control.addVariable("bulkShutterRadius",366.0);    // Shield level
  Control.addVariable("bulkInnerRadius",481.0);      // Inner Steel
  Control.addVariable("bulkOuterRadius",600.6);      // Outer Steel
  Control.addVariable("bulkFloor",1004.6);           // Bulk shield floor
  Control.addVariable("bulkRoof",1171.1);            // Bulk shield top
  Control.addVariable("bulkIronMat","CastIron");     // bulk material

  Control.addVariable("bulkInThick",115.0);         // Bulk First compartment
  Control.addVariable("bulkOutThick",119.0);        // Bulk Second compartment
  Control.addVariable("bulkInTop",50.2);            // Bulk Height (from centre line)
  Control.addVariable("bulkInBase",63.2);           // Bulk Base (from centre line)
  Control.addVariable("bulkOutTop",52.2);           // Bulk Height (from centre line)
  Control.addVariable("bulkOutBase",68.8);          // Bulk Base (from centre line)

  Control.addVariable("bulkInWidth",36);        // Bulk Height [cm] (from centre line)
  Control.addVariable("bulkOutWidth",40);       // Bulk Width [cm] (from centre line) 
  // Control stuff


  // Beam Window (PROTON FLIGHT PATH)
  Control.addVariable("ProtonVoidViewRadius",3.4);  // Radius [guesses]
  Control.addVariable("BWindowYStep",-6.0);  // Y step
  Control.addVariable("BWindowIncThick1",0.3);  // Inconel Thickness 1
  Control.addVariable("BWindowWaterThick",0.1); // Light water Thickness
  Control.addVariable("BWindowIncThick2",0.3); // Inconel Thickness 2
  Control.addVariable("BWindowInconelMat","Inconnel");  // Inconel
  Control.addVariable("BWindowWaterMat","H2O");   // Light water


  ts1System::shutterVar let("let"); // LET shutter number [west 7]
  let.buildVar(Control,0.0,0.0,0.0,9.0,4.0,0.0,0.0); 


  return;
}

}  // NAMESPACE setVariables
