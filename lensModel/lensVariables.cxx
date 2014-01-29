/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   lensModel/lensVariables.cxx
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
#include <boost/format.hpp>
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
LensModel(FuncDataBase& Control)
  /*!
    Function to set the control variables and constants
    \param Control :: Function data base to add constants too
  */
{
// -----------
// GLOBAL stuff
// -----------

  Control.addVariable("zero",0.0);      // Zero
  Control.addVariable("one",1.0);       // One

// ------------
// Target stuff
// ------------
  Control.addVariable("beamsize",1.7);           // Radius the beam

  Control.addVariable("siModWidth",8.0);         // Box height (half)
  Control.addVariable("siModHeight",8.0);        // Box across beam (half)
  Control.addVariable("siModLength",8.0);     // Box vertial (half)
  Control.addVariable("siModSiThick",0.05);      // Thickness of Silicon
  Control.addVariable("siModPolyThick",0.2);     // Thicness of poly

  Control.addVariable("siModSideAlThick",0.5);   // Side thickness
  Control.addVariable("siModTopAlThick",1.0);    // Base thickness
  Control.addVariable("siModBaseAlThick",1.0);   // Base thickness
  Control.addVariable("siModSiMat","SiCrystal"); // Material for Silicon
  Control.addVariable("siModSurroundMat","Aluminium"); // Surround Al
  Control.addVariable("siModPolyMat","Poly");    // Material for polyethene
  Control.addVariable("siModTemp",20.0);         // Temperature

  // CANDLESTICK Support
  Control.addVariable("candleBaseThick",1.905);    // Base thickness
  Control.addVariable("candleBaseWidth",10.0);     // Base width (Beam)
  Control.addVariable("candleBaseLength",15.875);  // Base length (x-Beam)
  Control.addVariable("candleBaseXoffset",-1.2875);    // Base x-Beam offset
  Control.addVariable("candleBaseYoffset",0.0);    // Base beam offset


  Control.addVariable("candleVSHeight",57.150);   // Full width to surround
//  Control.addVariable("candleVSWidth",1.0)    // Width [def:Base]
  Control.addVariable("candleVSDepth",1.905);    // Full length to surround
//  Control.addVariable("candleBaseXoffset",0.0);  // support x-Beam offset
//  Control.addVariable("candleBaseYoffset",0.0);  // support beam offset

  Control.addVariable("candleFlatLength",35.0);   // Full length (X)
//  Control.addVariable("candleFlatWidth",10.0);    // Width (Y)  [def:Width]
//  Control.addVariable("candleFlatThick",5.0);     // Thickness (Z) [def: Depth]

  Control.addVariable("candleVacBase",15.0825);   //  base distance (-ve)
  Control.addVariable("candleVacTop",58.26);      //  top distance (+ve)
  Control.addVariable("candleVacLowMX",10.376);   // X-beam distance (-ve)
  Control.addVariable("candleVacLowPX",15.6575);  // X-beam distance (+ve)
  Control.addVariable("candleVacLowMY",6.0325);   // beam distance (-ve)
  Control.addVariable("candleVacLowPY",6.0325);   // beam distance (+ve)
  Control.addVariable("candleVacCylX",64.2525);   // circular inset (beam)
  Control.addVariable("candleVacCylY",0.0);       // circular inset (x-beam)
  Control.addVariable("candleVacZExt",43.9724);   // Pipe height (lower)
  Control.addVariable("candleVacRadius",14.97);   // Vacuum radius

  Control.addVariable("candleAlSideThick",1.1);          // Side material
  Control.addVariable("candleAlBaseThick",1.1);      // Base material
  Control.addVariable("candleAlTopThick",1.1);       // Top plate thickness
  
  Control.addVariable("candleSkinForwardClear",1.5); // forward [away from beam]
  Control.addVariable("candleSkinBackClear",1.5); 
  Control.addVariable("candleSkinBaseClear",0.3); 
  Control.addVariable("candleSkinMSideClear",1.5);
  Control.addVariable("candleSkinPSideClear",1.5);

  Control.addVariable("candleSkinCutBaseClear",0.3); 
  Control.addVariable("candleSkinCircleClear",1.5); 
  Control.addVariable("candleSkinCutSideClear",1.5); 

  // Layers:
  Control.addVariable("layersWAngleXY1",23.0);    
  Control.addVariable("layersWAngleXY2",-23.0);    
  Control.addVariable("layersWHeight",12.0);    
  Control.addVariable("layersWWidth",12.0);    
  Control.addVariable("layersNWedgeLayers",1);    
  Control.addVariable("layersWLinerMat0","Aluminium");    
  Control.addVariable("layersWLiner0",0.3);    

  Control.addVariable("flightNumber",3);   // forward [away from beam]

  Control.addVariable("FL1XStep",0.0);      // Step from centre
  Control.addVariable("FL1ZStep",0.0);      // Step from centre
  Control.addVariable("FL1AngleXY1",0.0);  // Angle out
  Control.addVariable("FL1AngleXY2",0.0);  // Angle out
  Control.addVariable("FL1AngleZTop",0.0);  // Step down angle
  Control.addVariable("FL1AngleZBase",0.0); // Step up angle
  Control.addVariable("FL1Height",10.0);     // Full height
  Control.addVariable("FL1Width",10.0);     // Full width
  Control.addVariable("FL1masterXY",23.0); // Angle
  Control.addVariable("FL1masterZ",0.0); // Angle

  Control.addVariable("FL2XStep",0.0);      // Step from centre
  Control.addVariable("FL2ZStep",0.0);      // Step from centre
  Control.addVariable("FL2AngleXY1",0.0);  // Angle out
  Control.addVariable("FL2AngleXY2",0.0);  // Angle out
  Control.addVariable("FL2AngleZTop",0.0);  // Step down angle
  Control.addVariable("FL2AngleZBase",0.0); // Step up angle
  Control.addVariable("FL2Height",10.0);     // Full height
  Control.addVariable("FL2Width",10.0);     // Full width
  Control.addVariable("FL2masterXY",0.0); // Angle
  Control.addVariable("FL2masterZ",0.0); // Angle

  Control.addVariable("FL3XStep",0.0);      // Step from centre
  Control.addVariable("FL3ZStep",0.0);      // Step from centre
  Control.addVariable("FL3AngleXY1",0.0);  // Angle out
  Control.addVariable("FL3AngleXY2",0.0);  // Angle out
  Control.addVariable("FL3AngleZTop",0.0);  // Step down angle
  Control.addVariable("FL3AngleZBase",0.0); // Step up angle
  Control.addVariable("FL3Height",10.0);     // Full height
  Control.addVariable("FL3Width",10.0);     // Full width
  Control.addVariable("FL3masterXY",-23.0); // Angle
  Control.addVariable("FL3masterZ",0.0); // Angle


  Control.addVariable("protonEnergy",9.0); // Proton energy [MeV]
  Control.addVariable("protonYOffset",-12.02694); // Offset of centre [-10.02]
  Control.addVariable("protonHeight",5.08);             
  Control.addVariable("protonWidth",5.08); 
  Control.addVariable("protonAngle",135.0); 
  Control.addVariable("protonWallThick",0.5); 
  Control.addVariable("protonWallMat","Aluminium"); 
  
  Control.addVariable("protonTargetBeThick",0.4); 
  Control.addVariable("protonTargetBeWidth",11.58); 
  Control.addVariable("protonTargetBeHeight",16.64); 
  Control.addVariable("protonTargetWaterThick",2.2); 
  Control.addVariable("protonTargetWaterHeight",16.64); 
  Control.addVariable("protonTargetWaterWidth",11.58); 
  Control.addVariable("protonTargetThick",2.5); 
  Control.addVariable("protonTargetWidth",16.64); 
  Control.addVariable("protonTargetHeight",20.32); 

  Control.addVariable("protonTargetMat","Be300K"); 
  Control.addVariable("protonTargetCoolant","H2O"); 
  Control.addVariable("protonTargetSurround","Aluminium"); 

  Control.addVariable("layersWaterRadius",25.0);    // Inner radius of the water
  Control.addVariable("layersDCRadius",26.95);
  Control.addVariable("layersLeadRadius",41.95);
  Control.addVariable("layersBPolyRadius",61.95);
  Control.addVariable("layersEpoxyRadius",76.95);
  Control.addVariable("layersOutPolyRadius",106.95);
  Control.addVariable("layersOutVoidRadius",10000.0);

  Control.addVariable("layersWaterDepth",25.9);          // Water down
  Control.addVariable("layersDCDepth",30.9);     // Steel down
  Control.addVariable("layersLeadDepth",50.9);           // 
  Control.addVariable("layersBPolyDepth",75.9);          // 
  Control.addVariable("layersEpoxyDepth",95.9);              // 
  Control.addVariable("layersOutPolyDepth",125.9);             // 

  Control.addVariable("layersWaterHeight",25.9);         // Water up
  Control.addVariable("layersDCHeight",36.6);    // Steel up
  Control.addVariable("layersLeadHeight",94.1);           // 41.95
  Control.addVariable("layersBPolyHeight",154.1);          // 61.95
  Control.addVariable("layersEpoxyHeight",194.1);             // 76.96
  Control.addVariable("layersOutPolyHeight",244.1);      

  Control.addVariable("layersWaterAlThick",0.635);
  
  // Control.addVariable("waterRadius",25);    // Inner radius of the water
  // Control.addVariable("DCRadius",31.25);
  // Control.addVariable("leadRadius",60.0);
  // Control.addVariable("bPolyRadius",90.0);
  // Control.addVariable("epoxyRadius",110.0);
  // Control.addVariable("outPolyRadius",135.0);

  Control.addVariable("layersWaterMat","H2O");            // Full == 25cm
  Control.addVariable("layersDCMat","B-Poly");                // Borated Poly
  Control.addVariable("layersLeadMat","Lead");             // 41.95
  Control.addVariable("layersBPolyMat","B-Poly");            // 61.95
  Control.addVariable("layersEpoxyMat","B-Poly");            // epoxy+lead+boron
  Control.addVariable("layersOutPolyMat","Poly");          // 106.95

  Control.addVariable("candleMat","Aluminium");         // Support material
  Control.addVariable("layersAlMat","Aluminium");       // Al Material
  Control.addVariable("lensSourceRadial",4.0);     // Radial spread


  // Variable transfer
  Control.Parse("layersWaterHeight");
  Control.addVariable("candleWaterHeight"); 
  Control.Parse("layersWaterAlThick");
  Control.addVariable("candleWaterAlThick"); 
  Control.Parse("layersAlMat");
  Control.addVariable("candleAlMat"); 

  return;
}

} // NAMESPACE setVariable
