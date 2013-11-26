/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   build/TS1variables.cxx
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
TS1layout(FuncDataBase& Control)
  /*!
    Function to set the control variables and constants
    -- This version is for TS1
    \param Control :: Function data base to add constants too
  */
{

// -----------
// GLOBAL stuff
// -----------

  Control.addVariable("zero",0.0);      // Zero
  Control.addVariable("one",1.0);      // Zero

// ------------
// Target stuff
// ------------
  Control.addVariable("beamsize",3.4);            // Radius the beam
  Control.addVariable("flightradius",3.6);        // Radius the beam in the flightline
  Control.addVariable("cladding",0.1);            // Ta Cladding on W
  Control.addVariable("radiusInner",3.4);         // Inner Radius
  Control.addVariable("radiusOuter",1.5);         // Inner Radius
  Control.addVariable("twater",0.15);             // Inner Radius
  Control.addVariable("steel",0.5);
  Control.addVariable("targetlength",25);         // Target length

  Control.Parse("(beamsize/3.0)*((8.0*log(2.0))^0.5)");
  Control.addVariable("gaussbeam"); 
  Control.addVariable("targetDX",3.0);           // Shift in target height
  Control.addVariable("targetDY",0.0);           // Shift in target horrizontal
  Control.addVariable("beamDX",0.0);             // Shift in beam height
  Control.addVariable("beamDY",0.0);             // Shift in beam horrizontal


  Control.addVariable("tungmat","Tungsten");       // Material for target (W pure)
  Control.addVariable("coolant","D2O");              // Material for coolant 
  Control.addVariable("cladmat","Tantalum");         // Cladding material

// ------------
// Decoupler
// ------------ 

  Control.addVariable("nvane",0);
  Control.addVariable("nslice",0);
  Control.addVariable("vaneThick",0.12);
  Control.addVariable("vanemat","Cadmium");        // cadmium

// ----------
// Split
// ----------

  Control.addVariable("hotCentre",14.55);         // This needs to be fixed
  Control.addVariable("hotViewHeight",5.0);       // The half-height 
  Control.addVariable("watAngleA",160);           // Top angle
  Control.addVariable("watAngleB",122);           // top
  Control.addVariable("watAngleC",160-48);    //
  Control.addVariable("watAngleD",134);    // 
  Control.addVariable("merlinAngleA",65);    // Merlin side (85)
  Control.addVariable("merlinAngleB",95);    // Merlin side (80)
  Control.addVariable("waterViewHorr",12.0);      // Total view of water
  Control.addVariable("merlinViewHorr",10.0);      // Total view of merlin

  Control.addVariable("coldCentre",-14.55);       // This needs to be fixed
  Control.addVariable("coldViewHeight",5.0);          // The half-height 

  
  Control.addVariable("methAngleFar",26);      // Extra rotation off the main flight angle
  Control.addVariable("methAngleHSide",75);      // Extra rotation off the main flight angle
  Control.addVariable("methViewHorr",12.0);      // Total view of methane
  Control.addVariable("HMAngleHSide",45);   // Rotation off methane (hydrogen side)
  Control.addVariable("HMAngleFarSide",75);   // Rotation off methant (far side)

  Control.addVariable("HAngleSide",85);   // Rotation off hydrogen
  // WATER VIEW

// ---------------
// WATER Moderator
// ---------------
  Control.addVariable("watCY",0.0);         // 
  Control.addVariable("watCZ",21.0);    //  from the target front (same a ch4)
  Control.addVariable("wangle",-58.0);   // 

  Control.addVariable("watHeight",5.0);       // Water height (from centre)
  Control.addVariable("watBase",5.0);         // Water base  (from centre)
  Control.addVariable("watRWidth",6.0);       // Water width (from centre)
  Control.addVariable("watLWidth",6.0);       // Water width (from center)
  Control.addVariable("watThick",2.1);        // 1/2 thickness 
  Control.addVariable("waterAl",0.3);         // Aluminium thickness
  
  Control.addVariable("watPoisMat","Gadolinium");        // Poison material (Gd)
  Control.addVariable("watPoisThick",0.05);   // Poison thickness
  Control.addVariable("watPoisDepth",1.5);    // Poison depth

  Control.Parse("watBase+waterAl");
  Control.addVariable("wTotalBase");
  Control.Parse("watHeight+waterAl");
  Control.addVariable("wTotalTop");
  Control.Parse("watThick+waterAl");
  Control.addVariable("wTotalThick");
  Control.Parse("watLWidth+waterAl");
  Control.addVariable("wTotalLWidth");
  Control.Parse("watRWidth+waterAl");
  Control.addVariable("wTotalRWidth");

// ---------------
// Merlin Moderator
// ---------------
  Control.addVariable("merlinCY",0.0);     // 
  Control.addVariable("merlinCZ",5.0);    // from the target front (same a ch4)
  Control.addVariable("oangle",80.0);     //  merlin angle

  Control.addVariable("merlinHeight",5.0);       // methane away from target
  Control.addVariable("merlinBase",5.0);         // methane size to the target
  Control.addVariable("merlinRWidth",6.0);       // methane width
  Control.addVariable("merlinLWidth",6.0);       // methane width
  Control.addVariable("merlinThick",2.1);        //  methane thickness
  Control.addVariable("merlinAl",0.3);          //  methane Al thickness

  Control.Parse("merlinBase+waterAl");
  Control.addVariable("oTotalBase");
  Control.Parse("merlinHeight+merlinAl");
  Control.addVariable("oTotalTop");
  Control.Parse("merlinThick+waterAl");
  Control.addVariable("oTotalThick");
  Control.Parse("merlinLWidth+waterAl");
  Control.addVariable("oTotalLWidth");
  Control.Parse("watRWidth+waterAl");
  Control.addVariable("oTotalRWidth");

  // Rotational anclge
  Control.Parse("merlinCY*sind(oangle)+merlinCZ*cosd(oangle)");
  Control.addVariable("orotX");
  Control.Parse("merlinCY*cosd(oangle)-merlinCZ*sind(oangle)");
  Control.addVariable("orotY");

// -----------------
// Methane moderator
// -----------------

  Control.addVariable("methCY",0.0);          //  centres : 
  Control.addVariable("methCZ",21.0);         //  shifed down from the target front
  Control.addVariable("mangle",58.0);         // 

  Control.addVariable("methPoison",0.005);      // Methane poisoning
  Control.addVariable("methHeight",5.0);        // methane away from target
  Control.addVariable("methBase",5.0);          // methane size to the target
  Control.addVariable("methRWidth",6.0);        // methane width
  Control.addVariable("methLWidth",6.0);        // methane width
  Control.addVariable("methThick",2.1);         //  methane thickness

  Control.addVariable("methFlatRad",20.0);       // Radius

  Control.addVariable("methInnerAl",0.3);
  Control.addVariable("methVac",0.2);
  Control.addVariable("methOuterAl",0.2);
  Control.addVariable("methClear",0.5);

  Control.addVariable("mPreAl",0.3);
  Control.addVariable("mPreFExt",0.0);          // ForwardExtent
  Control.addVariable("mPreBExt",0.0);          // BackExtend
  Control.addVariable("mPreRWidth",0.0);        // Right width
  Control.addVariable("mPreLWidth",0.0);        // Left width
  Control.addVariable("mPreBase",0.0);          // distance from target
  Control.addVariable("mPreTop",0.6);           // toward target

  Control.addVariable("mliqtemp",100.0);           // Methan temperature
  Control.addVariable("methmat",29);            // Methane material (liq)
  Control.addVariable("gdmat","Gadolinium");    // Methane poisioning

  Control.Parse("methInnerAl+methVac+methOuterAl");
  Control.addVariable("mLayers");

  // Rotation angles for planes:
  Control.Parse("methCY*sind(mangle)+methCZ*cosd(mangle)");
  Control.addVariable("mrotX");
  Control.Parse("methCY*cosd(mangle)-methCZ*sind(mangle)");
  Control.addVariable("mrotY");

  // Centre of c/x circles:
  Control.Parse("methCY-methFlatRad*sind(mangle)");
  Control.addVariable("mcirY");

  Control.Parse("methCZ-methFlatRad*cosd(mangle)");
  Control.addVariable("mcirZ");
  

  // CH4 PreMod::
  Control.Parse("methThick+mLayers+methClear");
  Control.addVariable("mTotalThick");

  if (Control.EvalVar<double>("mPreTop")>0.0)
    Control.Parse("methHeight+mLayers+methClear+mPreTop+2*mPreAl");
  else
    Control.Parse("methHeight+mLayers+methClear");
  Control.addVariable("mTotalTop");

  if (Control.EvalVar<double>("mPreBase")>0.0)
    Control.Parse("methBase+mLayers+methClear+mPreBase+2*mPreAl");
  else
    Control.Parse("methBase+mLayers+methClear");
  Control.addVariable("mTotalBase");

  if (Control.EvalVar<double>("mPreRWidth")>0.0)
    Control.Parse("methRWidth+mLayers+mPreRWidth+2*mPreAl+methClear");
  else
    Control.Parse("methRWidth+mLayers+methClear");
  Control.addVariable("mTotalRWidth");

  if (Control.EvalVar<double>("mPreLWidth")>0.0)
    Control.Parse("methLWidth+mLayers+mPreLWidth+2*mPreAl+methClear");
  else
    Control.Parse("methLWidth+mLayers+methClear");
  Control.addVariable("mTotalLWidth");

// -----------------
// Hydrogen Moderator
// -----------------
  Control.addVariable("hliqtemp",20);            // Hydrogen material
  Control.addVariable("hymat",25);            // Hydrogen material
  Control.addVariable("almat",5);             // aluminium material
  Control.addVariable("watmat",11);             // aluminium material

  Control.addVariable("HBase",6.2);          // The half-height (towards target)
  Control.addVariable("hCY",0.0);                 // Centre position
  Control.addVariable("hCZ",5.0);                //Centre position
  Control.addVariable("hangle",100.0);             // hydrogen angle

// Liquid table
  Control.addVariable("hLiqWidth",5.0);
  Control.addVariable("hLiqBackDepth",1.5);
  Control.addVariable("hLiqFrontDepth",3.5);
  Control.addVariable("hLiqHeight",5.0);          //

// First layer Al
  Control.addVariable("hAlInnerThick",0.4);    // Thickness of inner aluminium
// Vac layer 1:
  Control.addVariable("hvacgap",0.2);         // Vac gap
// Second layer Al
  Control.addVariable("hAlMidThick",0.2);    // Thickness of inner aluminium
// Third layer Al
  Control.addVariable("hAlOuterThick",0.2);    // Thickness of inner aluminium

// Pre-mod Al:
  Control.addVariable("preWater",1.0);
  Control.addVariable("preBaseWater",1.0);
  Control.addVariable("preTopWater",1.0);
  Control.addVariable("preBackWater",1.5);
  Control.addVariable("preModAl",0.3);
  
// -----------------------
// FLIGHT LINES DECOUPLER
// ------------------------

  Control.addVariable("MethFlightAlFar",0.3);            // Al on far side [5002]
  Control.addVariable("MethFlightAlNear",0.3);           // Al close side [5001]
  Control.addVariable("MethFlightAlRoof",0.3);           // Al on top side [5005]
  Control.addVariable("MethFlightAlFloor",0.3);          // Al on floor side [5006]
  Control.addVariable("H2FlightAlFar",0.3);              // Hydronge near [5012]
  Control.addVariable("H2FlightAlOut",0.3);              // Hydrogen far [5011]
  Control.addVariable("H2FlightAlNear",0.3);             // Hydrogen close [5021]

  Control.addVariable("MethFlightCdFar",0.12);            // Cd on far sice
  Control.addVariable("MethFlightCdNear",0.12);           // Cd close side 
  Control.addVariable("MethFlightCdRoof",0.12);           // Cd on far side
  Control.addVariable("MethFlightCdFloor",0.12);          // Cd on far side
  Control.addVariable("H2FlightCdFar",0.12);              // Hydronge near
  Control.addVariable("H2FlightCdOut",0.12);              // Hydrogen far
  Control.addVariable("H2FlightCdNear",0.12);             // Hydrogen close
  


// ----------
// Reflector
// ----------

  Control.addVariable("refmat",37);         // Reflector material (37 == solid be@rt)

  Control.addVariable("refsize",35);       // size of the reflector (half width)
  Control.addVariable("refedge",9);        // size of the flat ends
  Control.addVariable("refxshift",0);      // x shift of reflector
  Control.addVariable("refyshift",0);      // y shift of reflector
  Control.addVariable("refzshift",15);    // z shift of reflector




  Control.Parse("methInnerAl+methVac+methOuterAl");
  Control.addVariable("mLayers");
  
  // WATER

  Control.Parse("watCY*sind(wangle)+watCZ*cosd(wangle)");
  Control.addVariable("wrotX");
  Control.Parse("watCY*cosd(wangle)-watCZ*sind(wangle)");
  Control.addVariable("wrotY");

  // MERLIN

  // HYDROGEN

  Control.Parse("hCY*sind(mangle)+hCZ*cosd(mangle)");
  Control.addVariable("hrotX");
  Control.Parse("hCY*cosd(mangle)-hCZ*sind(mangle)");
  Control.addVariable("hrotY");

// hLiqWidth+
  Control.Parse("hAlInnerThick+2*hvacgap+hAlMidThick+hAlOuterThick");
  Control.addVariable("hLayers");
  Control.Parse("hLiqWidth+hLayers+preModAl+preWater");
  Control.addVariable("hFullWidth");              // Full width of the who 

  Control.addVariable("hLiqBackDepth",1.5);
  Control.Parse("hLiqWidth+hLayers+preModAl+preWater");

  Control.Parse("hLiqBackDepth+hLayers+preModAl+preBackWater");
  Control.addVariable("hBackDepth");          //  methane thickness

  Control.Parse("hLiqFrontDepth+hLayers");
  Control.addVariable("hFrontDepth");          //  methane thickness

  Control.Parse("hLiqHeight+hLayers+preModAl+preTopWater");
  Control.addVariable("HTop");          // The half-height (away from target)

  Control.Parse("hLiqHeight+hLayers+preModAl+preBaseWater");
  Control.addVariable("HBase");          // The half-height (away from target)

// Stuff for Be ratios:

  Control.Parse("wTotalTop");            // Add max after merlin
  Control.addVariable("hotTop");
  Control.Parse("wTotalBase");
  Control.addVariable("hotBase");
  Control.Parse("max(HBase,mTotalBase)");
  Control.addVariable("coldBase");
  Control.Parse("max(HTop,mTotalTop)");
  Control.addVariable("coldTop");

  return;
}

} // NAMESPACE setVariable
