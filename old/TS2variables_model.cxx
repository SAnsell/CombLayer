/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   old/TS2variables_model.cxx
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


void
setVariable_TS2model(FuncDataBase& Control)
  /*!
    Function to set the control variables and constants
    -- This version is for TS2 model [ChipNT1.i]
    \param Control :: Function data base to add constants too
  */
{

// Coupled Moderator  
// -- Centre defined as 
  
  Control.addVariable("zshift",4.8);        //  Groove shift
  Control.addVariable("yshift",0.0);        //  Relative to target
  Control.addVariable("groove",3.0);        // Size of the groove vertical
  Control.addVariable("gangle",72);         // Angle of the groove moderator
  Control.addVariable("pmsize",0.5);        // Premoderator size
  Control.addVariable("pouteral",0.15);     // Al thickness near moderator  
  Control.addVariable("underpm",0.75);      // Premoderator size into target space
  Control.addVariable("lead",0.0);          // Lead in the methane side
  Control.addVariable("groovelength",4.5);  // 0.0 => no groove
  Control.addVariable("gflightal",0.5);     // thickness of Al in flight line
  Control.addVariable("gheight",1.5);       // distance up to the groove
  Control.addVariable("gwidth",4.15);       // half width of the groove
  Control.addVariable("gwallal",0.15);      // Thickness of the viewed wall al
  Control.addVariable("gsndal",0.10);       // Thickness of the al in the groove secondary
  Control.addVariable("goutal",0.10);       // Thickness of the al in the groove secondary
  Control.addVariable("grvalrad",20.0);     // Radius of the vacuum layers of the groove
  
  Control.addVariable("gsideal",0.15);      // Thickness of the al on the sides of the groove
  Control.addVariable("gbaseal",0.15);      // Thickness of the al on the base/top of the groove
  Control.addVariable("cplheight",6.0);     // Half Height of the moderator
  Control.addVariable("hfacevgap",11.0);    // View size of the H- face 
  Control.addVariable("hfaceoffset",0.0);   // Height to start viewing
  Control.addVariable("hydrothick",6.5);    // Thickness of the H2 
  Control.addVariable("meththick",7.0);     // Thickness of the CH4
  Control.addVariable("chmodwidth",9.8);    // Width (half) of CH4 +H2 mods
  Control.addVariable("hydrowidth",7.0);    // Width (half) of the H2 face 

  Control.addVariable("hwaterlayer",2.0);   // water round hydrogen face of moderator
  Control.addVariable("hwaterlength",14.0); // length of water
  Control.addVariable("methmidal",0.0);     // Middle seperator betweh Ch4 and H2
  Control.addVariable("hydrmidal",0.7);     // Middle seperator betweh Ch4 and H2
  Control.addVariable("vacmid",0.0);        // Middle seperator betweh Ch4 and H2
  
  Control.addVariable("innermetal",5);      // First layer material type (aluminium) [normally 5]
  Control.addVariable("innerhal",0.5);      // First layer of Al of the groove [non-view]
  Control.addVariable("innerhalface",0.35); // First layer of Al hydrogen view
  Control.addVariable("innerfrontal",0.5);  // First layer of Al groove side (non-viewd)
  Control.addVariable("midhal",0.3);        // Middle layer of Al of the groove
  Control.addVariable("midfrontal",0.10);    // Middle layer of Al of the groove view
  Control.addVariable("midhalface",0.15);    // Middle layer of Al view on H face
  Control.addVariable("outhal",0.3);        // Outer layer of Al of the coupled moderator
  Control.addVariable("outfrontal",0.1);    // Outer layer of Al of the grove viewed face
  Control.addVariable("outhalface",0.1);    // Outer layer of Al viewed on H face
  Control.addVariable("vacspacer",0.6);     // Outer vac spacer
  Control.addVariable("fsthvac",0.6);       // First layer of Vacuum of the groove
  Control.addVariable("frontfsthvac",0.6);  // First layer of Vacuum infront of groove (front)
  Control.addVariable("sndhvac",0.2);       // Second layer of Vacuum of the groove
  Control.addVariable("gfrontvac",0.5);     // Vac layer infront of vac layer
  Control.addVariable("sidefsthvac",0.5);    // Vac on side of hydrogen

  Control.addVariable("preforward",0.5);    // Distance that pre-mod extends.
  Control.addVariable("preal",0.15);        // Al thickness of premod
  Control.addVariable("hradius",20.0);      // Radius of the hydrogen face
  Control.addVariable("gradius",7.25);      // Radius of the groove (changed from 6.81)

  Control.addVariable("premethextra",1.0);  // Extra on the methane side of the pre-mod
  Control.addVariable("prehextra",1.0);     // Extra on the hydrogen side of the pre-mod
  Control.addVariable("presideextra",1.0);  // Extra added to the sides of the moderator
  Control.addVariable("premethdepth",2.0);  // Depth of the methane side moderator
  Control.addVariable("prehydrdepth",2.2);  // Depth of the methane side moderator
  Control.addVariable("modspace",0.4);      // space between pre-moderator and teritaliary (h-side)
  Control.addVariable("premodspace",0.3);   // space between pre-moderator base of moderator
  Control.addVariable("pinneral",0.25);     // Al thickness of near moderator
  Control.addVariable("pouteral",0.25);     // Al thickness of target/side
      
  Control.addVariable("gPreMat",45);       // Type of material in groove side
  Control.addVariable("chtypegrv",4);       // Type of material in groove side
  Control.addVariable("zrtype",11);         // Zr/water in middle
  Control.addVariable("hytype",16);         // Hydrogen type/15/20K
  Control.addVariable("jkttype",12);        // Water in jacket
  Control.addVariable("methtemp",26.0);     // Methane temperature
  Control.addVariable("hhtemp",20.0);       // Hydrogen temperature 
  Control.addVariable("gfillmat",0);        // Filled with Al (or vaccum)
  Control.addVariable("midlaytemp",77.0);   // Temperature in Tunstgen 
  Control.addVariable("hywallal",0.5);      // Hydrogen wall aluminium thickness
  Control.addVariable("wallAlGroove",0.5);    // coupled wall aluminium thickness [flight]
  Control.addVariable("wallAlHydrogen",0.5);  // coupled wall aluminium thickness [flight]
  Control.addVariable("pipewallal",0.5);    // Hydrogen pipe wall aluminium thickness

// Thermocouple holder
  Control.addVariable("thermoft",0.0);      // Holder front thickness (0.5)
  Control.addVariable("thermost",0.0);      // holder side thickness  (0.5)
  Control.addVariable("tflapout",0.0);      // holder out going of component (0.5)
  Control.addVariable("tflapwidth",0.0);    // holder extend of component (1.5)
  Control.addVariable("tboltdepth",0.0);    // Screw depth                (2.0)
  Control.addVariable("tboltradius",0.0);   // Bolt radius                (0.3)
  Control.addVariable("tboltmat",3);        // Screw material (stainless steel)
  Control.addVariable("thermmat",5);        // holder material (normally AL5083)

// Castle fragment
  Control.addVariable("ncastle",0);        // Number of castle componenets
  Control.addVariable("casmat",5);         // Material for components (Al)
  Control.addVariable("castlefgap",0.5);   // Gap to the front of the groove
  Control.addVariable("castlebgap",2);     // Gap to the hydrogen face
  Control.addVariable("cfrac",0.5);        // how much Al vs Methane
  Control.addVariable("castleextra",0.0);  // Extent that the casle extends upwards

//--------------
// Decoupled
//--------------

  Control.addVariable("zdshift",4.6);      //  Decoupled shift relative to target
  Control.addVariable("ydshift",0.0);      // Shift of the moderator

  Control.addVariable("dangle",33);        // Angle of the decoupled moderator (To EPB)
  Control.addVariable("DCPreModTThick",0.75);   // Decoupled premod total thickness
  Control.addVariable("DCPreModAlThick",0.1);   // Decoupled premod Al thickness
  Control.addVariable("DCPreModMat",12);        // Water in the premod
  Control.addVariable("DCPreModAlMat",45);      // material for the premod 2014 Al 
  Control.addVariable("dcpmext",1.5);      // Extension of premoderator 
  Control.addVariable("dclead",0.0);       // lead in the decoupled 
  Control.addVariable("centwidth",7.2);    // Half Central width of both face
  Control.addVariable("centheight",6.0);   // Half Central height of both faces
  Control.addVariable("activewidth",6.0);  // Half active width of faces
  Control.addVariable("gdlayer",0.012);    // Gd/Cd thickness  (in vanes)
  Control.addVariable("poisonsep",0.0);    // Gd/Cd thickness between broad and narrow
  Control.addVariable("cdNarrowL",0.12);    // thickness of cd decoupling layer (outer narrow side)
  Control.addVariable("cdBroadL",0.12);     // thickness of cd decoupling layer (outer broad side) 
  Control.addVariable("lbewidth",0.0);     // Thickness of Be on the left side -ve (mid spacer ==> narrow)
  Control.addVariable("rbewidth",0.0);     // Thickness of Be of the right side +ve (mid spacer ==> broad)
  Control.addVariable("lmiddcsep",0.0);    // Thickness of Al separator in midde of mod [left side (-ve y,narrow)]
  Control.addVariable("rmiddcsep",0.0);    // Thickness of Al separator in midde of mod [right side (+ve y,broad)]
  Control.addVariable("nlength",1.9);      // Thickness of CH4 on narrow side
  Control.addVariable("blength",3.3);      // Thickness of CH4 on broad side
  Control.addVariable("jacket",2.0);       // Thickness of jacket layers
  Control.addVariable("window",12.0);      // View Window gap (decoupled)
  Control.addVariable("dalpress",0.3);     // Inner pressure thickness
  Control.addVariable("vacgap",0.5);       // Vac gap between inner and mid
  Control.addVariable("daltern",0.2);      // Mid pressure thickness
  Control.addVariable("midvacgap",0.2);    // Mid Vac gap between mid and outer
  Control.addVariable("dalheat",0.2);      // Outer heat shield Al
  Control.addVariable("DCleftGap",0.6);    // Outer side gap
  Control.addVariable("DCleftPipe",1.2);   // Space for pipes on the right
  Control.addVariable("DCrightGap",0.6);   // Outer side gap
  Control.addVariable("DCrightPipe",0.0);  // Space for pipes on the right
  Control.addVariable("innerPipeR",0.3);   // Inner CH4 pipe radius
  Control.addVariable("outerPipeR",0.6);   // Inner CH4 pipe radius
  Control.addVariable("wallalnarrow",0.0); // Flight path Al thickness narrow side
  Control.addVariable("wallalbroad",0.5);  // Flight path Al thickness broad side
  Control.addVariable("flightLMat",5);     // Flight line material
  Control.addVariable("alradius",31.6);    // Radius of the aluminium faces
  Control.addVariable("dmethtemp",26.0);   // Decoupled methane temperature
  Control.addVariable("chtypenarrow",4);   // Type of material narrow side
  Control.addVariable("chtypebroad",4);    // Type of material broad side
  Control.addVariable("poisonmat",6);      // Poison material in cell 
  Control.addVariable("DCwishPoison",37);  // Poison material close to wish (be) 
  Control.addVariable("DClmxPoison",37);   // Poison material close to lmx (be)
  Control.addVariable("decouplemat",7);    // Decoupler material
  Control.addVariable("bucketmat",7);      // Decoupler material in bucket
  Control.addVariable("centmat",16);       // Central material (BE/H2/CH4 etc)
  Control.addVariable("radradius",0.3);    // Radius of component compartment
  Control.addVariable("radmainsep",5.0);   // Main separation of the supports
  Control.addVariable("nrad",5);           // Number of items in the list
  Control.addVariable("radheight",5.0);    // Radiator height
  Control.addVariable("radcentre",0.0);    // Radiator centre (0.0 == middle)
  Control.addVariable("radalthick",0.1);   // Thickness of the al around the radiator
  Control.addVariable("radmat",5);         // Wall of the Al radiator pipes
  Control.addVariable("dcinnermat",5);     // Inner material of decoupled (aluminium) [normally 5]

// Extra for broad hole
  Control.addVariable("bgvwidth",0.0);      // 1/2 width of the narrow groove
  Control.addVariable("bgvheight",0.0);     // 1/2 Vertial of the narrow groove  
  Control.addVariable("bgvlength",0.0);     // Out dist of the narrow groove 
  Control.addVariable("bgvoffset",2.5);      // Raise/lower of the groove
  Control.addVariable("bgvInnerOffset",1.0); // Raise/lower of the groove (+ve towards target)
  Control.addVariable("bgvangle",-10);        // Angle of the narrow groove width 
  Control.addVariable("bgvholewidth",0.0);   // 1/2 hole width
  Control.addVariable("bgvholeheight",0.0);  // 1/2 hole height
  Control.addVariable("bgvholedepth",0.0);   // hole depth (from outer face)
  Control.addVariable("bgvholeangle",28);    // Angle of the narrow groove width
  Control.addVariable("bgvholemat",41);      // Material to fill hole [41==silicon]
  Control.addVariable("bgvedgemat",33);      // Edge material 

// Extra for hole narrow
  Control.addVariable("ngvwidth",0.0);        // 1/2 width of the narrow groove
  Control.addVariable("ngvheight",0.0);       // 1/2 Vertial of the narrow groove  
  Control.addVariable("ngvlength",0.0);       // Out dist of the narrow groove 
  Control.addVariable("ngvoffset",3.5);       // Raise/lower of the groove
  Control.addVariable("ngvInnerOffset",0.0);  // Raise/lower of the groove (+ve towards target)
  Control.addVariable("ngvangle",-10);        // Angle of the narrow groove width 
  Control.addVariable("ngvholewidth",1.06);   // 1/2 hole width
  Control.addVariable("ngvholeheight",1.06);  // 1/2 hole height
  Control.addVariable("ngvholedepth",1.76);    // hole depth (from outer face)
  Control.addVariable("ngvholeangle",10);     // Angle of the narrow groove width
  Control.addVariable("ngvholemat",41);       // Material to fill hole [41==silicon]
  Control.addVariable("ngvedgemat",33);       // Edge material 

/// Vanes on narrow side
  Control.addVariable("nvane",0);         // Number of vanes
  Control.addVariable("nslice",0);        // Number of vanes slices for chequeborad
  Control.addVariable("vanelength",0.8);   // Length of vane (if <0) then to base
  Control.addVariable("vanewidth",0.0015); // thickness vane
  Control.addVariable("vanemat",4);        // material of vane
  Control.addVariable("vaneback",6);       // material of back of vane
/// Outer posioning
  Control.addVariable("cdtopext",15.0);    // outer poisoning depth at top
  Control.addVariable("cdtopsize",0.12);   // outer poisoning thickness at top
  Control.addVariable("cdcycradius",17.5); // outer poisoning radius
  Control.addVariable("cdcycsize",0.12);   // outer poisoning thickness at radius
  Control.addVariable("cdbasedepth",12.5); // outer poisoning depth (base)
  Control.addVariable("cdbasesize",0.12);  // outer poisoning thickness at base

/// Pipework (COUPLED)

  Control.addVariable("pipeclear",0.6);    // Pipe clearance from Be
  Control.addVariable("hpipelength",10);   // Length of H pipe into the hydrogen
  Control.addVariable("cypipeoff",-1.50);  // viewFace shift from the centre line (groove pipe)
  Control.addVariable("czpipeoff",0.00);   // Accross Face shift from the centre line (goove pipe)
  Control.addVariable("cradius",3.645);    // radius of encasement pipe (inner)
  Control.addVariable("pinal",0.16);       // thickness of vacuum al (encasement)
  Control.addVariable("pipevac",0.48);     // thickness of vacuum (encasement)
  Control.addVariable("poutal",0.16);      // thickness of outer al layer

  Control.addVariable("hpoffy",-1.20);     // viewface position of hydrogen pipe (+ve towards H2 face)
  Control.addVariable("hpoffz",0.00);      // across position of hydrogen pipe
  Control.addVariable("hprad",0.865);      // thickness of inner hydrogen 
  Control.addVariable("hpinal",0.20);      // thickness of Al inner hydrogen pipe
  Control.addVariable("hpout",0.835);      // thickness of outer hydrogen flow 
  Control.addVariable("hpoutal",0.20);     // thickness of outer hydrogen Al

  Control.addVariable("methipy",2.50);     // viewface position of inlet pipe (+ve towards H2 face)
  Control.addVariable("methipz",-1.00);    // across position of inlet pipe
  Control.addVariable("methopy",2.50);     // viewface position of inlet pipe (+ve towards H2 face)
  Control.addVariable("methopz",1.0);      // across position of inlet pipe
  Control.addVariable("methrad",0.4);      // radius of the methane in/out pipe
  Control.addVariable("methal",0.1);       // thickness of the methane pipe thickness
  Control.addVariable("pipetemp",100);     // Temperature of the pipes
  Control.addVariable("chliquid",29);      // Material for inner pipe of ch4 (liquid ch4)
  
/// New rods
  Control.addVariable("cdrodn",0);         // number of cd rods (in a full circle)
  Control.addVariable("cdrodradius",0.8);  // Radius of the cd rods.
  Control.addVariable("cdrodthick",0.1);   // Radius of the cd rods.
  
  Control.addVariable("ovactemp",300.0);   // Temperature on outside of vacuum vessel

  // TO DELETE:
  Control.addVariable("refmat",37);        // Reflector material (solid be@rt)
  Control.addVariable("reflectal",0.2);    // Al layer in reflector
  Control.addVariable("refgroove",3.3);    // Height of the groove in ref
  Control.addVariable("refsize",35);       // size of the reflector (half width)
  Control.addVariable("refend",30.0*sqrt(2.0)); // size to the flat ends
  Control.addVariable("refsteel",1);       // width of the steel layer
  Control.addVariable("refxshift",0);      // x shift of reflector
  Control.addVariable("refyshift",0);      // y shift of reflector
  Control.addVariable("refzshift",-10);    // z shift of reflector
  Control.addVariable("cdbasesize",0.12);  // outer poisoning thickness at base
  // END DELETE:

  Control.Parse("refsize-refzshift/sqrt(2)");
  Control.addVariable("platePoint");                // edges for corners

// Sapphire
  Control.addVariable("nSapphire",0);         // Sapphire layers
  Control.addVariable("sfThick",0.05);         // Sapphire thickness
  Control.addVariable("sapphiremat",0);        // sapphiremat  Vacuum/Sapphire(40)/Silicon(41)
  
// Flight line angles
  Control.addVariable("igfla",26.5);    // Inner Groove face 1
  Control.addVariable("igflb",37.1);    // Inner Groove face 2 

  Control.addVariable("gfla",26.5);    // Groove face 1 (flight line)
  Control.addVariable("gflb",26.5);    // Groove face 2 (flight line) [37.1 -- new position for E6] 
  Control.addVariable("gflc",53.2);    // Groove Hydrogen face 1
  Control.addVariable("gfld",9.9);     // Groove Hydrogen face 2 
// Decoupled flight line
  Control.addVariable("nFaceAngleA",27.34);      // Narrow face 1 (nfla,b,c,
  Control.addVariable("nFaceAngleB",11.01);    // Narrow face 2 
  Control.addVariable("bFaceAngleA",27.06);      // Broad face 1 
  Control.addVariable("bFaceAngleB",12.02);    // Broad face 2


  // OLD irradiation system


  // IRRADIATIION NEW:
  Control.addVariable("irInnerRadius",2.25);    // Radius of hole with material
  Control.addVariable("irContainerRadius",2.40);// Radius of outer container
  Control.addVariable("irBeRadius",2.60);       // Radius of Be cut
  Control.addVariable("irContainerMat",3);       // Radius of Be cut
  Control.addVariable("irTCut",8.75);           // Cut of target from front face in centre of beamline  
  Control.addVariable("irAngle",-46);           // Angle of beamline
  Control.addVariable("irSlope",-4.76);         // Angle of beamline
  Control.addVariable("irTarOut",12.0);         // Be (inner) thickness 
  Control.addVariable("irInAThick",0.0);        // Section from target
  Control.addVariable("irInBThick",0.0);        // Section from target
  Control.addVariable("irInCThick",0.8);        // Section from target
  Control.addVariable("irInAMat",0);            // (inner) material
  Control.addVariable("irInBMat",0);            // (inner) material
  Control.addVariable("irInCMat",0);            // (inner) material

  Control.addVariable("irOutAThick",0.0);       // Section ref side
  Control.addVariable("irOutBThick",0.0);       // Section ref side
  Control.addVariable("irOutCThick",0.0);       // Section ref side
  Control.addVariable("irOutAMat",42);          // (outer) material
  Control.addVariable("irOutBMat",38);          // (outer) material
  Control.addVariable("irOutCMat",3);           // (outer) material


  Control.Parse("refsize-refzshift/sqrt(2)");
  Control.addVariable("platePoint");                // edges for corners

  // Control stuff
  // TARGET
  Control.addVariable("pi",M_PI);          // Pi since it is not defined
  Control.Parse("innerhal+fsthvac+midhal+sndhvac+outhal");
  Control.addVariable("skins"); 

  Control.Parse("pi*igfla/180.0");
  Control.addVariable("igflar"); 
  Control.Parse("pi*igflb/180.0");
  Control.addVariable("igflbr"); 
  Control.Parse("pi*gfla/180.0");
  Control.addVariable("gflar"); 
  Control.Parse("pi*gflb/180.0");
  Control.addVariable("gflbr"); 
  Control.Parse("pi*gflc/180.0");
  Control.addVariable("gflcr"); 
  Control.Parse("pi*gfld/180.0");
  Control.addVariable("gfldr"); 

  // Control.Parse("flightradius+flightwall+tarcage+sidewater");
  // Control.addVariable("targetnose"); 
  Control.Parse("(beamsize/3.0)*((8.0*log(2.0))^0.5)");
  Control.addVariable("gaussbeam"); 
  Control.Parse("methmidal+vacmid+hydrmidal");
  Control.addVariable("methhydsep"); 
  Control.Parse("underpm+pmsize");
  Control.addVariable("fullpre"); 
  Control.Parse("hydrothick+methhydsep");
  Control.addVariable("outerhsurf"); 
  Control.Parse("meththick");
  Control.addVariable("outercsurf"); 
  Control.Parse("gwallal+fsthvac+gsndal+sndhvac+goutal");
  Control.addVariable("windskins"); 
  Control.Parse("meththick+skins");
  Control.addVariable("clength"); 
  Control.Parse("outerhsurf+skins");
  Control.addVariable("dlength"); 
  Control.Parse("pi*gangle/180.0");
  Control.addVariable("grr"); 
  Control.Parse("meththick-groovelength");
  Control.addVariable("gcval");               // depth of groove 
  Control.Parse("-(gradius^2.0-gwidth^2.0)^0.5+gcval");
  Control.addVariable("gcentre"); 
  Control.Parse("gflar+gflbr");
  Control.addVariable("cwidth"); 
  Control.Parse("gflar-gflbr");
  Control.addVariable("cdiff"); 
  
  // DC centre
  Control.Parse("2.0*(radalthick+radradius)+0.1");
  Control.addVariable("radiator"); 
  Control.Parse("(radiator+lmiddcsep+rmiddcsep+blength+rbewidth-nlength-lbewidth)/2");
  Control.addVariable("DCcentreY");
  Control.Parse("(DCleftGap-DCrightGap-DCrightPipe)/2");
  Control.addVariable("DCcentreZ");

  Control.addVariable("sectorangle"); 
  Control.Parse("gflar-(0.5+cfrac)*sectorangle");
  Control.addVariable("canglem"); 
  Control.Parse("gflar+(cfrac-0.5)*sectorangle");
  Control.addVariable("canglep"); 
  Control.Parse("gcval-gwidth*(1.0/tan(cwidth)+cos(cdiff)/sin(cwidth))");
  Control.addVariable("yginter"); 
  Control.Parse("-gwidth*sin(cdiff)/sin(cwidth)");
  Control.addVariable("zginter"); 
  Control.Parse("hradius-(methhydsep+hydrothick)");
  Control.addVariable("hcentre"); 
  Control.Parse("innerfrontal+gfrontvac+midfrontal+sndhvac+outfrontal");
  Control.addVariable("gfrontskins"); 
  Control.Parse("innerhal+sidefsthvac+midhal+sndhvac+outhal");
  Control.addVariable("sideskins"); 
  Control.Parse("innerhalface+fsthvac+midhalface+sndhvac+outhalface");
  Control.addVariable("faceskins"); 
  Control.Parse("meththick+innerfrontal+gfrontvac-(grvalrad^2-(chmodwidth+innerhal+sidefsthvac)^2)^0.5");
  Control.addVariable("gfcentre"); 
  Control.Parse("-(lbewidth+lmiddcsep+radiator/2.0)+DCcentreY");
  Control.addVariable("radline"); 
  Control.Parse("-radheight/2.0");
  Control.addVariable("radlowx"); 
  Control.Parse("centheight+dalpress+vacgap+dalheat+midvacgap+daltern+gdlayer");
  Control.addVariable("totdcheight"); 
  Control.Parse("centwidth+dalpress+vacgap+dalheat+midvacgap+daltern+gdlayer+DCleftPipe+DCleftGap");
  Control.addVariable("DCleftWidth"); 
  Control.Parse("centwidth+dalpress+vacgap+dalheat+midvacgap+daltern+gdlayer+DCrightPipe+DCrightGap");
  Control.addVariable("DCrightWidth"); 
  Control.Parse("rbewidth+poisonsep+dalheat+vacgap+daltern+midvacgap+dalpress+blength");
  Control.addVariable("rdcleng"); 
  Control.Parse("rbewidth+poisonsep+dalheat+vacgap+daltern+midvacgap+dalpress+blength+bgvlength");
  Control.addVariable("rdclengextra"); 
  Control.Parse("lbewidth+dalheat+vacgap+daltern+midvacgap+dalpress+nlength");
  Control.addVariable("ldcleng"); 
  Control.Parse("lbewidth+poisonsep+dalheat+vacgap+daltern+midvacgap+dalpress+nlength+ngvlength");
  Control.addVariable("ldclengextra"); 
  Control.Parse("-totdcheight-(t2TargetBoreRadius+DCPreModTThick+dclead)");
  Control.addVariable("decoupleCent"); 

  // Stuff for pipe
  Control.Parse("(-nlength+3*blength)/4.0");
  Control.addVariable("pipeAY");
  Control.Parse("(-3*nlength+blength)/4.0");
  Control.addVariable("pipeBY");
  Control.Parse("centwidth+dalpress+(DCleftPipe+vacgap)/2.0");
  Control.addVariable("pipeAZ");
  Control.addVariable("pipeBZ");

  Control.Parse("alradius+poisonsep-ldcleng");
  Control.addVariable("ncentre"); 
  Control.Parse("-alradius+rdcleng");
  Control.addVariable("bcentre"); 
  Control.Parse("pi*dangle/180.0");
  Control.addVariable("drr"); 
  Control.Parse("dalheat+vacgap+daltern+midvacgap+dalpress+gdlayer-totdcheight");
  Control.addVariable("dchbase"); 
  Control.Parse("-(outerhsurf+windskins)");
  Control.addVariable("hviewipt"); 
  Control.Parse("-sin(igflar)*gcval+cos(igflar)*gwidth");
  Control.addVariable("idfla"); 
  Control.Parse("-sin(gflar)*gcval+cos(gflar)*gwidth");
  Control.addVariable("dfla"); 
  Control.Parse("-sin(igflbr)*gcval+cos(igflbr)*gwidth");
  Control.addVariable("idflb"); 
  Control.Parse("-sin(gflbr)*gcval+cos(gflbr)*gwidth");
  Control.addVariable("dflb"); 
  Control.Parse("sin(gflcr)*hviewipt+cos(gflcr)*hydrowidth");
  Control.addVariable("dflc"); 
  Control.Parse("sin(gfldr)*hviewipt+cos(gfldr)*hydrowidth");
  Control.addVariable("dfld"); 
  Control.Parse("pi*nFaceAngleA/180.0");
  Control.addVariable("nFaceAngleAR"); 
  Control.Parse("pi*nFaceAngleB/180.0");
  Control.addVariable("nFaceAngleBR");
  Control.Parse("pi*bFaceAngleA/180.0");
  Control.addVariable("bFaceAngleAR"); 
  Control.Parse("pi*bFaceAngleB/180.0");
  Control.addVariable("bFaceAngleBR"); 
  Control.Parse("cos(nFaceAngleAR)*activewidth");
  Control.addVariable("nFaceAInter"); 
  Control.Parse("cos(nFaceAngleBR)*activewidth");
  Control.addVariable("nFaceBInter"); 
  Control.Parse("cos(bFaceAngleAR)*activewidth");
  Control.addVariable("bFaceAInter"); 
  Control.Parse("cos(bFaceAngleBR)*activewidth");
  Control.addVariable("bFaceBInter"); 
  Control.Parse("t2TargetBoreRadius+pinneral+pouteral+pmsize+premodspace+lead");
  Control.addVariable("couplelow"); 
  Control.Parse("couplelow+skins+cplheight");
  Control.addVariable("couplecent"); 
  Control.Parse("canglem+sectorangle");
  Control.addVariable("special"); 


 // SPECIAL CONDITONS:
 // 173:($centheight+$cdlayer+$wallalnarrow)
 // 11106 4 px $ngvheight+$ngvoffset
 // 173 must be highter than 11106

  Control.Parse("centheight+cdNarrowL+wallalnarrow");
  const double ch=Control.Eval();
  Control.Parse("ngvheight+ngvoffset");
  const double gh=Control.Eval();
  if (gh>ch)
    {
      ELog::EM<<"XGroove :: setting ngvoffset from : "<<Control.EvalVar<double>("ngvoffset");
      // set gh==ch by changing ngvoffset:
      Control.Parse("centheight+cdNarrowL+wallalnarrow-ngvheight"); //+wallalnarrow-ngvheight");
      Control.setVariable("ngvoffset");
      ELog::EM<<" to : "<<Control.EvalVar<double>("ngvoffset")<<ELog::endWarn;
      exit(1);
    }
  return;
}
