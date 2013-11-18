/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   old/oldPressure.cxx
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
#include <vector>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <boost/regex.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "InputControl.h"
#include "Element.h"
#include "Material.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Tensor.h"
#include "Vec3D.h"
#include "Triple.h"
#include "NRange.h"
#include "NList.h"
#include "Tally.h"
#include "cellFluxTally.h"
#include "pointTally.h"
#include "heatTally.h"
#include "surfaceTally.h"
#include "tallyFactory.h"
#include "TallyCreate.h"
#include "Transform.h"
#include "Quaternion.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "Rules.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Weights.h"
#include "WeightModification.h"
#include "PhysImp.h"
#include "KGroup.h"
#include "PhysicsCards.h"
#include "AlterSurfBase.h"
#include "AlterSurfTS2.h"
#include "RemoveCell.h"
#include "TS2remove.h"
#include "TMRCreate.h"
#include "shutterInfo.h"
#include "ShutterCreate.h"
#include "SimProcess.h"
#include "SurInter.h"
#include "Simulation.h"

namespace ELog 
{
  ELog::OutputLog<EReport> EM;
  ELog::OutputLog<FileReport> FM("Spectrum.log");
  ELog::OutputLog<StreamReport> CellM;
}

Simulation Sim;
std::vector<shutterSystem::shutterInfo> SInfo;

void setVariable(FuncDataBase&); ///< Set all the variables

int 
main(int argc,char* argv[])
{
  std::vector<std::string> Names;  
  InputControl::mainVector(argc,argv,Names);
  int multi(1);
  const int multiFlag(InputControl::flagVExtract(Names,"m","multi",multi));
  int nps(600000);
  InputControl::flagVExtract(Names,"n","nps",nps);

  // Now count parameters:
  if (Names.size()<2)
    {
      std::cerr<<"Insufficient files ::"
	       <<" Eng.i out.x"<<std::endl
	       <<"Options :"<<std::endl
	       <<" -m multinum :: Number of index files with different RND seeds."
	       <<" -n Nps :: Number of point"
	       <<std::endl;
      exit(1);
    }    

  const std::string Fname=Names[0];         // Master file name
  const std::string Oname=Names[1];         // Output file name

  /*!
    The big variable setting
  */

  std::istringstream cx;
  setVariable(Sim.getDataBase());
  Sim.registerAlterSurface(new AlterSurfTS2(Sim.getDataBase()));
  Sim.registerRemoveCell(new TS2remove());   // database/cells get set later

  // Definitions section 
  try
    {
      Sim.readMaster(Fname);
      TMRSystem::setVirtualContainers(Sim);
//      TMRSystem::addSapphire(Sim);
      TMRSystem::makeNGroove(Sim);
      TMRSystem::makeBGroove(Sim);
      TMRSystem::addIradBeamline(Sim);  
      shutterSystem::createShutterInfo(SInfo);
      shutterSystem::addVoidVessel(Sim);    
      shutterSystem::addShutters(Sim,SInfo);
      for(unsigned int i=0;i<SInfo.size();i++)
	shutterSystem::addShutterUnits(Sim,SInfo,i);
      shutterSystem::addBulkShield(Sim,SInfo);
      shutterSystem::processInnerVoid(Sim,shutterSystem::outer);

      ELog::EM.lock();
      Sim.removeDeadCells();            // Generic
      ELog::EM.dispatch(0);
      ELog::EM.lock();
      Sim.removeDeadSurfaces();         // Generic
      ELog::EM.dispatch(0);

      Sim.setCutter(76);

      // NEED  to set all the cells from simulations here
      Sim.getPC().setCells("imp",1,0);                // Set a zero cell
//      WeightSystem::addForcedCollision(Sim,40.0);

      // Put pointtallies for chips      
      
//      Sim.getPC().setCells(1,0);	
//      Sim.getPC().setCells(21203,0);	 // Vacuum layer on 
//      Sim.getPC().setCells(6401,0);	 // Vacuum layer on 
//      Sim.getPC().setCells(613,0);	

//      Sim.getPC().setCells(76,0);	
//      tallySystem::addFullHeatBlock(Sim);
      // Stuff for irradiation calculation
//      std::vector<int> TungstenCells;
//      TungstenCells.push_back(2);
//      addF4Tally(Sim,4,"n",TungstenCells);
//      addF4Tally(Sim,14,"h",TungstenCells);
//      addF4Tally(Sim,24,"p",TungstenCells);
      tallySystem::addF5Tally(Sim,25,33002,33004,33003,14502,14402,-1000.0);
//      tallySystem::addF5Tally(Sim,35,132,162,163,10605,10606,1000.0);
      tallySystem::widenF5Tally(Sim,25,0,-1.0);        // -2cm vertial
      tallySystem::widenF5Tally(Sim,25,1,0.5);       // -1cm horrizontal
//      addF1Tally(Sim,11,21412);
//      addF1Tally(Sim,21,21411);
//      addF1Tally(Sim,31,21416);
      TMRSystem::setWeights(Sim);
      Sim.removeComplements();
      Sim.getPC().setNPS(600000);
      Sim.getPC().setRND(34564531L);

      // WRITE OUT MANY
      if (multiFlag)
	SimProcess::writeMany(Sim,Oname,multi);
      else
	Sim.write(Oname);
      Sim.writeCinder();
    }
  catch (ColErr::ExBase& A)
    {
      std::cerr<<A.what()<<std::endl;
      exit(1);
    }
}



void
setVariable(FuncDataBase& Control)
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
  Control.addVariable("beamsize",1.7);      // Radius the beam
  Control.addVariable("flightradius",1.8);  // Radius the beam liner
  Control.addVariable("radius",3.1);        // Radius of the target
  Control.addVariable("tarlength",29.65);   // Length of target
  Control.addVariable("conerad",-2.0);      // Radius of the target cone
  Control.addVariable("conelength",6.4);    // Length of the target cone
  Control.addVariable("cornerradius",-2.0); // Radius of the corner sphere
  Control.addVariable("cornerdist",6.4);    // Dist of corner sphere
  Control.addVariable("tarwater",0.2);      // Target water thickness. (not also much cange top/side water)
  Control.addVariable("topwater",0.2);      // top water 
  Control.addVariable("sidewater",0.2);     // side target water
  Control.addVariable("fwater",0.0);        // Water over front of target
  Control.addVariable("fsteel",0.0);        // Thickness of steel over front face
  Control.addVariable("steel",0.3);         // Steel target thickness.
  Control.addVariable("fextn",0.0);         // forward extension  
  Control.addVariable("tarcage",0.0);       // Target vacuum gap
  Control.addVariable("coolant",11);        // Material for coolant  (was D2O == 31)
  Control.addVariable("modcool",11);        // Material for top/bottom coolant 
  Control.addVariable("manifold",32);       // Material for surround (Steel)
  Control.addVariable("tungmat",38);        // Material for target (Ta pure)
  Control.addVariable("flightwall",0.5);    // thicknes of steel in EPB
  Control.addVariable("cutangc",0.0);       // Top cut angle (to groove)
  Control.addVariable("cutangd",0.0);       // Bottom cut angle (to base)
  Control.addVariable("cladding",0.1);      // Ta Cladding on W
  Control.addVariable("lobheight",0.5);     // seperation of lob sides
  Control.addVariable("loblen",6.411);      // length of lobes (>radius+tar+steel-lh)
  Control.addVariable("lobactive",0);       // Use the lobe code
  Control.addVariable("targettemp",450.0);  // Temperature in Tunstgen
  Control.addVariable("cooltemp",360.0);    // Temperature of water cooling
  Control.addVariable("refmat",37);         // Reflector material (37 == solid be@rt)
  Control.addVariable("inccool",0.0);       // Spacer in the middle of primary window
  Control.addVariable("inconnelwin",0.1);   // Proton Window thickness x2 as there are two
  Control.addVariable("protwinmat",5);      // Proton Win material
  Control.addVariable("protcoolmat",11);    // Proton Win coolant (H2O)

#//  Control.addVariable("lobheight",0.0);   // seperation of lob sides
#//  Control.addVariable("loblen",0.0);      // length of lobes (>radius+tar+steel-lh)
#// Coupled Moderator  
#// -- Centre defined as 

  Control.addVariable("zshift",4.8);        //  Groove shift
  Control.addVariable("yshift",0.0);        //  Relative to target
  Control.addVariable("groove",2.0);        // Size of the groove vertical
  Control.addVariable("gangle",72);         // Angle of the groove moderator
  Control.addVariable("pmsize",0.5);        // Premoderator size
  Control.addVariable("pinneral",0.5);      // Al thickness next to target
  Control.addVariable("pouteral",0.5);      // Al thickness near moderator  
  Control.addVariable("underpm",0.75);      // Premoderator size into target space
  Control.addVariable("lead",0.0);          // Lead in the methane side
  Control.addVariable("groovelength",2.5);  // 0.0 => no groove
  Control.addVariable("gflightal",0.5);     // thickness of Al in flight line
  Control.addVariable("gheight",2.0);       // distance up to the groove
  Control.addVariable("gwidth",4.15);       // half width of the groove
  Control.addVariable("gwallal",0.5);       // Thickness of the viewed wall al
  Control.addVariable("gsndal",0.10);       // Thickness of the al in the groove secondary
  Control.addVariable("goutal",0.10);       // Thickness of the al in the groove secondary
  Control.addVariable("grvalrad",20.0);     // Radius of the vacuum layers of the groove
  
  Control.addVariable("gsideal",0.7);      // Thickness of the al on the sides of the groove
  Control.addVariable("gbaseal",0.7);      // Thickness of the al on the base/top of the groove
  Control.addVariable("cplheight",6.0);     // Half Height of the moderator
  Control.addVariable("hfacevgap",11.0);    // View size of the H- face 
  Control.addVariable("hfaceoffset",0.0);   // Height to start viewing
  Control.addVariable("hydrothick",6.5);    // Thickness of the H2 
  Control.addVariable("meththick",7.0);     // Thickness of the CH4
  Control.addVariable("chmodwidth",9.8);    // Width (half) of CH4 +H2 mods
  Control.addVariable("hydrowidth",6.0);    // Width (half) of the H2 face 

  Control.addVariable("hwaterlayer",2.0);   // water round hydrogen face of moderator
  Control.addVariable("hwaterlength",10.0); // length of water
  Control.addVariable("methmidal",0.0);     // Middle seperator betweh Ch4 and H2
  Control.addVariable("hydrmidal",0.7);     // Middle seperator betweh Ch4 and H2
  Control.addVariable("vacmid",0.0);        // Middle seperator betweh Ch4 and H2
  
  Control.addVariable("innermetal",5);      // First layer material type (aluminum) [normally 5]
  Control.addVariable("innerhal",1.0);      // First layer of Al of the groove [non-view]
  Control.addVariable("innerhalface",0.35); // First layer of Al hydrogen view
  Control.addVariable("innerfrontal",0.7);  // First layer of Al groove side (non-viewed) 
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
//  Control.addVariable("gradius",7.25);      // Radius of the groove (changed from 6.81)
  Control.addVariable("gradius",50.0);      // Radius of the groove (changed from 6.81)

  Control.addVariable("premethextra",1.0);  // Extra on the methane side of the pre-mod
  Control.addVariable("prehextra",1.0);     // Extra on the hydrogen side of the pre-mod
  Control.addVariable("presideextra",1.0);  // Extra added to the sides of the moderator
  Control.addVariable("premethdepth",2.0);  // Depth of the methane side moderator
  Control.addVariable("prehydrdepth",2.2);  // Depth of the methane side moderator
  Control.addVariable("modspace",0.4);      // space between pre-moderator and teritaliary (h-side)
  Control.addVariable("premodspace",0.3);   // space between pre-moderator base of moderator
  Control.addVariable("pinneral",0.25);     // Al thickness of near moderator
  Control.addVariable("pouteral",0.25);     // Al thickness of target/side
      
  Control.addVariable("chtypegrv",4);       // Type of material in groove side
  Control.addVariable("zrtype",11);         // Zr/water in middle
  Control.addVariable("hytype",16);         // Hydrogen type/15/20K
  Control.addVariable("jkttype",12);        // Water in jacket
  Control.addVariable("methtemp",26.0);     // Methane temperature
  Control.addVariable("hhtemp",20.0);       // Hydrogen temperature 
  Control.addVariable("gfillmat",0);        // Filled with Al (or vaccum)
  Control.addVariable("midlaytemp",77.0);   // Temperature in Tunstgen 
  Control.addVariable("hywallal",0.5);      // Hydrogen wall aluminium thickness
  Control.addVariable("wallalcoupled",0.5); // coupled wall aluminium thickness
  Control.addVariable("pipewallal",0.5);    // Hydrogen pipe wall aluminium thickness

// Thermocouple holder
  Control.addVariable("thermoft",0.5);      // Holder front thickness
  Control.addVariable("thermost",0.5);      // holder side thickness
  Control.addVariable("tflapout",0.5);      // holder out going of component
  Control.addVariable("tflapwidth",1.5);    // holder extend of component
  Control.addVariable("tboltdepth",2.0);    // Screw depth 
  Control.addVariable("tboltradius",0.3);   // Bolt radius
  Control.addVariable("tboltmat",3);        // Screw material (stainless steel)
  Control.addVariable("thermmat",5);        // holder material (normally AL5083)

// Castle fragment
  Control.addVariable("ncastle",0);        // Number of castle componenets
  Control.addVariable("casmat",5);         // Material for components (Al)
  Control.addVariable("castlefgap",0.5);   // Gap to the front of the groove
  Control.addVariable("castlebgap",2);     // Gap to the hydrogen face
  Control.addVariable("cfrac",0.5);        // how much Al vs Methane
  Control.addVariable("castleextra",0.0);  // Extent that the casle extends upwards

// Decoupled
  Control.addVariable("zdshift",4.6);      //  Decoupled shift relative to target
  Control.addVariable("ydshift",0.0);      // Shift of the moderator

  Control.addVariable("dangle",33);        // Angle of the decoupled moderator
  Control.addVariable("dcpm",0.75);        // Decoupled premod size
  Control.addVariable("dcpmext",1.5);      // Extension of premoderator
  Control.addVariable("dclead",0.0);       // lead in the decoupled
  Control.addVariable("centwidth",7.2);    // Half Central width of both face
  Control.addVariable("centheight",6.0);   // Half Central height of both faces
  Control.addVariable("activewidth",6.0);  // Half active width of faces
  Control.addVariable("gdlayer",0.012);    // Gd/Cd thickness  (in vanes)
  Control.addVariable("poisonsep",0.0);    // Gd/Cd thickness between broad and narrow
  Control.addVariable("cdNarrowL",0.12);    // thickness of cd decoupling layer (outer narrow side)
  Control.addVariable("cdBroadL",0.12);     // thickness of cd decoupling layer (outer broad side) 
  Control.addVariable("cdlayer",0.12);     // thickness of cd decoupling layer (outer)
  Control.addVariable("lbewidth",0.0);     // Thickness of Be on the left side -ve (mid spacer ==> narrow)
  Control.addVariable("rbewidth",0.0);     // Thickness of Be of the right side +ve (mid spacer ==> broad)
  Control.addVariable("lmiddcsep",0.0);    // Thickness of Al separator in midde of mod (left side (-ve y,narrow))
  Control.addVariable("rmiddcsep",0.0);    // Thickness of Al separator in midde of mod (right side (+ve y,broad))
  Control.addVariable("nlength",1.9);      // Thickness of CH4 on narrow side
  Control.addVariable("blength",3.3);      // Thickness of CH4 on broad side
  Control.addVariable("jacket",2.0);       // Thickness of jacket layers
  Control.addVariable("window",12.0);      // View Window gap (decoupled)
  Control.addVariable("dalpress",0.5);     // Inner pressure thickness  [normally 0.3cm]
  Control.addVariable("vacgap",0.5);       // Vac gap between inner and outer
  Control.addVariable("daltern",0.2);      // Mid pressure thickness
  Control.addVariable("midvacgap",0.2);    // Mid Vac gap between inner and outer
  Control.addVariable("dalheat",0.2);      // Outer heat shield Al
  Control.addVariable("DCleftGap",0.6);    // Outer side gap
  Control.addVariable("DCleftPipe",1.2);   // Space for pipes on the right
  Control.addVariable("DCrightGap",0.6);   // Outer side gap
  Control.addVariable("DCrightPipe",0.0);  // Space for pipes on the right
  Control.addVariable("innerPipeR",0.3);   // Inner CH4 pipe radius
  Control.addVariable("outerPipeR",0.6);   // Inner CH4 pipe radius
  Control.addVariable("wallalnarrow",0.5); // Flight path Al thickness narrow side [Ref view]
  Control.addVariable("wallalbroad",0.5);  // Flight path Al thickness broad side  [Ref view]
  Control.addVariable("flightLMat",5);     // Flight line material
  Control.addVariable("alradius",31.6);    // Radius of the aluminium faces
  Control.addVariable("chtype",4);         // Type of material narrow side
  Control.addVariable("chtypebroad",4);    // Type of material broad side
  Control.addVariable("poisonmat",6);      // Poison material in cell
  Control.addVariable("DCwishPoison",6);  // Poison material close to wish (Gd) 
  Control.addVariable("DClmxPoison",6);   // Poison material close to lmx (be)
  Control.addVariable("decouplemat",7);    // Decoupler material
  Control.addVariable("bucketmat",7);      // Decoupler material in bucket
  Control.addVariable("centmat",16);       // Central material (BE/H2/CH4 etc)
  Control.addVariable("radradius",0.3);    // Radius of component compartment
  Control.addVariable("radmainsep",5.0);   // Main separation of the supports
  Control.addVariable("nrad",5);           // Number of items in the list
  Control.addVariable("radheight",5.0);    // Radiator height
  Control.addVariable("radcentre",0.0);    // Radiator centre (0.0 == middle)
  Control.addVariable("radalthick",0.1);   // Thickness of the al around the radiator
  Control.addVariable("radmat",5);         // Wall of the Al radiator pipes material
  Control.addVariable("dcinnermat",3);     // Inner metal on the decoupled moderator
  

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
  Control.addVariable("ngvwidth",0.0);      // 1/2 width of the narrow groove
  Control.addVariable("ngvheight",0.0);     // 1/2 Vertial of the narrow groove  
  Control.addVariable("ngvlength",0.0);     // Out dist of the narrow groove 
  Control.addVariable("ngvoffset",2.5);      // Raise/lower of the groove
  Control.addVariable("ngvInnerOffset",1.0); // Raise/lower of the groove (+ve towards target)
  Control.addVariable("ngvangle",-10);        // Angle of the narrow groove width 
  Control.addVariable("ngvholewidth",0.0);   // 1/2 hole width
  Control.addVariable("ngvholeheight",0.0);  // 1/2 hole height
  Control.addVariable("ngvholedepth",0.0);   // hole depth (from outer face)
  Control.addVariable("ngvholeangle",28);    // Angle of the narrow groove width
  Control.addVariable("ngvholemat",41);      // Material to fill hole [41==silicon]
  Control.addVariable("ngvedgemat",33);      // Edge material 

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
/// Reflector
  Control.addVariable("reflectal",0.2);    // Al layer in reflector
  Control.addVariable("refgroove",3.3);    // Height of the groove in ref
  Control.addVariable("refsize",35);       // size of the reflector (half width)
  Control.addVariable("refedge",9);        // size of the flat ends
  Control.addVariable("refsteel",1);       // width of the steel layer
  Control.addVariable("refxshift",0);      // x shift of reflector
  Control.addVariable("refyshift",0);      // y shift of reflector
  Control.addVariable("refzshift",-10);    // z shift of reflector
  Control.addVariable("cdbasesize",0.12);  // outer poisoning thickness at base

// Sapphire
  Control.addVariable("nSapphire",0);         // Sapphire layers
  Control.addVariable("sfThick",0.05);         // Sapphire thickness
  Control.addVariable("sapphiremat",0);        // sapphiremat  Vacuum/Sapphire(40)/Silicon(41)

// Flight line angles
  Control.addVariable("igfla",0.0);       // Inner Groove face 1
  Control.addVariable("igflb",0.0);       // Inner Groove face 2

  Control.addVariable("gfla",26.5);    // Groove face 1
  Control.addVariable("gflb",26.5);    // Groove face 2 
  Control.addVariable("gflc",44.2);    // Groove Hydrogen face 1
  Control.addVariable("gfld",8.5);     // Groove Hydrogen face 2 
// Decoupled flight line
  Control.addVariable("nFaceAngleA",27.34);      // Narrow face 1 (nfla,b,c,
  Control.addVariable("nFaceAngleB",11.01);    // Narrow face 2 
  Control.addVariable("bFaceAngleA",27.06);      // Broad face 1 
  Control.addVariable("bFaceAngleB",12.02);    // Broad face 2

// OUTER STEEL
  Control.addVariable("shieldrad",150);  // Outer shield radius
  Control.addVariable("shieldthick",600);   // Outer shield thickness
  Control.addVariable("shieldmat",3);       // Steel 304
  Control.addVariable("voidHeight",100);    // void heigh
  Control.addVariable("shieldHeight",300);   // top of the steel
  Control.addVariable("voidBase",100);      // void heigh
  Control.addVariable("shieldBase",300);    // top of the steel

  // IRRADIATIION NEW:
  Control.addVariable("irInnerRadius",2.0);     // Radius of beam
  Control.addVariable("irTCut",10.0);           // Cut of target from front face in centre of beamline  
  Control.addVariable("irAngle",-46);           // Angle of beamline
  Control.addVariable("irSlope",-4.9);          // Angle of beamline
  Control.addVariable("irTarOut",12.0);         // Be (inner) thickness 
  Control.addVariable("irInAThick",1.5);        // Section from target
  Control.addVariable("irInBThick",1.1);        // Section from target
  Control.addVariable("irInCThick",0.8);        // Section from target
  Control.addVariable("irInAMat",3);            // (inner) material
  Control.addVariable("irInBMat",5);            // (inner) material
  Control.addVariable("irInCMat",41);            // (inner) material

  Control.addVariable("irOutAThick",0.8);       // Section ref side
  Control.addVariable("irOutBThick",0.5);       // Section ref side
  Control.addVariable("irOutCThick",0.0);       // Section ref side
  Control.addVariable("irOutAMat",42);          // (outer) material
  Control.addVariable("irOutBMat",33);          // (outer) material
  Control.addVariable("irOutCMat",3);           // (outer) material

  Control.addVariable("irShutALen",2.0);        // Shutter Item length
  Control.addVariable("irShutAH",2.0);          // Shutter Item height
  Control.addVariable("irShutAMat",44);         // Shutter Item Material
  Control.addVariable("irShutBLen",2.0);        // Shutter Item length
  Control.addVariable("irShutBH",2.0);          // Shutter Item height
  Control.addVariable("irShutBMat",44);         // Shutter Item Material
  Control.addVariable("irShutCLen",2.0);        // Shutter Item length
  Control.addVariable("irShutCH",2.0);          // Shutter Item height
  Control.addVariable("irShutCMat",44);         // Shutter Item Material


  // VOID VESSEL:
  // NEED To move 168 and 169 out by : 4.2cm on BOTH:
  // shift is 17.59 (chip side) and 7.79 (non-chipsied)
  Control.addVariable("voidDCTopShift",4.2);        // Void extra space on Top window
  Control.addVariable("voidDCLowChipShift",17.59);  // Void extra space on Low window (chip)
  Control.addVariable("voidDCLowW4Shift",7.79);     // Void extra space on Low window (non-chip)
  Control.addVariable("voidThick",7.42);        // Void vessel thickness
  Control.addVariable("voidGap",4.45);          // Void vessel Gap surround
  Control.addVariable("voidTop",50.1);          // Void vessel top height
  Control.addVariable("voidBase",122.6);        // Void vessel base
  Control.addVariable("voidSide",72.7);         // Void vessel side
  Control.addVariable("voidFront",121.7);       // Void vessel front
  Control.addVariable("voidBack",187.0);        // Void vessel back
  Control.addVariable("voidFDepth",23.7);       // Void vessel Depth of rounding
  Control.addVariable("voidFAngle",30.0);       // Void vessel Angle 
  Control.addVariable("voidWindow",8.8);        // Void vessel window gaps
  Control.addVariable("voidWindowThick",0.8);   // Void vessel window thickness

  Control.addVariable("portSize",9.0);          // viewport size

  // SHUTTER VESSEL:
  Control.addVariable("shutterZoffset",0.0);    // Shutter offset to target centre
  Control.addVariable("shutterFCut",28.9);      // Cut in the inner steel (EPB side)
  Control.addVariable("shutterInThick",185.7);  // Shutter Inner steel
  Control.addVariable("shutterOutThick",416.6); // Shutter Outer steel
  Control.addVariable("shutterRoof",1004.6);    // Shutter Inner roof steel
  Control.addVariable("shutterBase",1171.1);    // Shutter Inner base steel
  Control.addVariable("shutterInnerThick",185.7);      // Shutter Inner steel
  Control.addVariable("shutterInsertBaseVoid",152.5);  // Shutter Insert thickness
  Control.addVariable("shutterInsertTopVoid",171.1); // Shutter Insert thickness
  Control.addVariable("shutterInsertThick",118.9); // Shutter Insert thickness
  Control.addVariable("shutterInsertGap",11.15);   // Shutter Insert thickness
  Control.addVariable("shutterInsertChipGap",25);  // Shutter Insert thickness
  Control.addVariable("shutterHWidth",12.0);       // Shutter Half width
  Control.addVariable("shutterMat",3);             // shutter Material
  Control.addVariable("shutterSurMat",3);          // shutter surround mat
  // BULK VESSEL:
  Control.addVariable("bulkInThick",111.5);    // Bulk First compartment
  Control.addVariable("bulkOutThick",120.80);  // Bulk Second compartment
  Control.addVariable("bulkInTop",50.2);       // Bulk Height (from centre line)
  Control.addVariable("bulkInBase",63.2);      // Bulk Base (from centre line)
  Control.addVariable("bulkOutTop",52.2);       // Bulk Height (from centre line)
  Control.addVariable("bulkOutBase",68.8);      // Bulk Base (from centre line)

  Control.addVariable("bulkInWidth",36);       // Bulk Height (from centre line)
  Control.addVariable("bulkOutWidth",40);       // Bulk Height (from centre line)
  

  // ROOM:: 
  Control.addVariable("iShield",200);           // Steel shielding
  Control.addVariable("iradFrontWall",1000);
  Control.addVariable("iRoomWidth",100);        // half width of room
  Control.addVariable("iradBackWall",1400);     // Distance to back wall (4metre room)
  Control.addVariable("iRoomFloor",100);           
  Control.addVariable("iRoomHeight",100);
  Control.addVariable("iWallmat",3);            // Room material
  Control.addVariable("iFScatWidth",16);        // Scatter width
  Control.addVariable("iScatMat",23);           // Scatter material
  Control.addVariable("iFgap",5.0);             // Gap between front or system and scatter
  Control.addVariable("iFBeThick",12.2);        // Size of the scattering unit

  // ROOM:: 
  Control.addVariable("iShield",200);           // Steel shielding
  Control.addVariable("iradFrontWall",1000);
  Control.addVariable("iRoomWidth",100);        // half width of room
  Control.addVariable("iradBackWall",1400);     // Distance to back wall (4metre room)
  Control.addVariable("iRoomFloor",100);           
  Control.addVariable("iRoomHeight",100);
  Control.addVariable("iWallmat",3);            // Room material
  Control.addVariable("iFScatWidth",16);        // Scatter width
  Control.addVariable("iScatMat",23);           // Scatter material
  Control.addVariable("iFgap",5.0);             // Gap between front or system and scatter
  Control.addVariable("iFBeThick",12.2);        // Size of the scattering unit


//  Control.Parse("(iradEAThick+iradEBThick+iradECThick)-(refsize+refedge)");
  // Intersect of line 185 and 21002:
  
  Control.Parse("refsize-refzshift/sqrt(2)");
  Control.addVariable("platePoint");                // p or (-p)
  
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

  Control.Parse("pi*cutangc/180.0");
  Control.addVariable("tsa"); 
  Control.Parse("pi*cutangd/180.0");
  Control.addVariable("tsb"); 
  Control.Parse("1.0/((radius+topwater)^2)");
  Control.addVariable("topa"); 
  Control.Parse("1.0/((radius+sidewater)^2)");
  Control.addVariable("sideb"); 
  Control.Parse("1.0/((radius+topwater+steel)^2)");
  Control.addVariable("toppresa"); 
  Control.Parse("1.0/((radius+sidewater+steel)^2)");
  Control.addVariable("sidepresb"); 
  Control.Parse("1.0/((radius+topwater+steel+tarcage)^2)");
  Control.addVariable("topvaca"); 
  Control.Parse("1.0/((radius+sidewater+steel+tarcage)^2)");
  Control.addVariable("sidevacb"); 
  Control.Parse("1.0/((radius+topwater+steel+tarcage+pinneral)^2)");
  Control.addVariable("toppma"); 
  Control.Parse("1.0/((radius+sidewater+steel+tarcage+pinneral)^2)");
  Control.addVariable("sidepmb"); 
  Control.Parse("radius+sidewater+steel+tarcage");
  Control.addVariable("targetside"); 
  Control.Parse("radius+topwater+steel+tarcage");
  Control.addVariable("targettop"); 
  Control.Parse("flightradius+flightwall+tarcage+sidewater");
  Control.addVariable("targetnose"); 
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
  Control.Parse("-cwidth/(2*ncastle+1)");

  // DC centre
  Control.Parse("(blength+rbewidth-nlength-lbewidth)/2");
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
  Control.Parse("2.0*(radalthick+radradius)+0.1");
  Control.addVariable("radiator"); 
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
  Control.Parse("centwidth+dalpress+vacgap+dalheat+midvacgap+daltern+gdlayer+DCleftPipe+DCleftGap");
  Control.addVariable("rdcleng"); 
  Control.Parse("rbewidth+poisonsep+dalheat+vacgap+daltern+midvacgap+dalpress+blength+bgvlength");
  Control.addVariable("rdclengextra"); 
  Control.Parse("lbewidth+dalheat+vacgap+daltern+midvacgap+dalpress+blength");
  Control.addVariable("ldcleng"); 
  Control.Parse("lbewidth+poisonsep+dalheat+vacgap+daltern+midvacgap+dalpress+nlength+ngvlength");
  Control.addVariable("ldclengextra"); 
  Control.Parse("-totdcheight-(targettop+dcpm+dclead)");
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
  Control.Parse("targettop+pinneral+pouteral+pmsize+premodspace+lead");
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
      ELog::EM.Estream()<<" to : "<<Control.EvalVar<double>("ngvoffset");
      ELog::EM.warning();
    }
  Control.Parse("ngvheight-daltern-midvacgap-dalheat-vacgap-dalpress+ngvoffset");
  const double p11606=Control.Eval();
  Control.Parse("ngvholeheight+ngvoffset+ngvInnerOffset");
  const double p12106=Control.Eval();
  std::cout<<"p11 = "<<p11606<<" "<<p12106<<std::endl;
  if (p12106>p11606)
    {
      ELog::EM.Estream()<<"XGroove :: setting ngvInnerOffset from : "
			       <<Control.EvalVar<double>("ngvInnerOffset");
      Control.setVariable("ngvInnerOffset",Control.EvalVar<double>("ngvInnerOffset")-p12106+p11606);
      ELog::EM.Estream()<<" to : "<<Control.EvalVar<double>("ngvInnerOffset");
      ELog::EM.warning();

    }

  return;
}

void
addIradBeamline(Simulation& System,FuncDataBase& Control)
  /*!
    Add an irradiation beamline
    \param System :: Simulation model to add
    \param Control :: Variables list
   */
{
  const double IHS=Control.EvalVar<double>("iradHsize");
//  const double sThick=Control.EvalVar<double>("shieldthick");
  if (IHS<=0.0)
    return;

  
  MonteCarlo::Qhull* voidPtr=System.findQhull(75);
  if (voidPtr==0)
    {
      std::cerr<<"Not BASE CELL "<<75<<std::endl;
      exit(1);
    }

  // If we have the void the add the outside line
  voidPtr->addSurfString("(21401 : -21402 : -21403 : 21404 : 21406 : -21405 : -185 )");
  voidPtr->addSurfString("(21301:-21302:-21303:21304:21405:-185)");


  return;
}


