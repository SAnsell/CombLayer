/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   old/clayerOld.cxx
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
#include <boost/shared_ptr.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
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
#include "varList.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Weights.h"
#include "WeightModification.h"
#include "PhysImp.h"
#include "KGroup.h"
#include "Source.h"
#include "PhysicsCards.h"
#include "AlterSurfBase.h"
#include "AlterSurfTS2.h"
#include "RemoveCell.h"
#include "TS2remove.h"
#include "TMRCreate.h"
#include "MainProcess.h"
#include "shutterInfo.h"
#include "insertInfo.h"
#include "insertBaseInfo.h"
#include "ShutterCreate.h"
#include "Hutch.h"
#include "SimProcess.h"
#include "SurInter.h"
#include "Simulation.h"
#include "SimPHITS.h"
#include "ChipTally.h"
#include "GeneralShutter.h"
#include "ChipIRShutter.h"
#include "BeamLineTally.h"

namespace ELog 
{
  ELog::OutputLog<EReport> EM;
  ELog::OutputLog<FileReport> FM("ChipDatum.pts");
  ELog::OutputLog<StreamReport> CellM;
}

std::vector<shutterSystem::shutterInfo> SInfo;
std::vector<boost::shared_ptr<shutterSystem::GeneralShutter> > GData;

void setVariable(FuncDataBase&); ///< Set all the variables

int 
main(int argc,char* argv[])
{
  // For output stream

  ELog::RegMethod RControl("","main");
  // Set up output information:
  ELog::EM.setNBasePtr(RControl.getBasePtr());
  ELog::EM.setTypeFlag(0);
  ELog::EM.setActive(255);

  ELog::FM.setNBasePtr(RControl.getBasePtr());
  ELog::FM.setActive(255);
  ELog::FM.setTypeFlag(0);

  ELog::FM<<"Version == "<<version::Instance()->getVersion()+1<<ELog::endDiag;

  std::vector<std::string> Names;  
  std::map<std::string,std::string> Values;  
  std::map<std::string,std::string> AddValues;  
  InputControl::mainVector(argc,argv,Names);
  // Input control:
  int multi(1);
  int nps(600000);
  long int randNum(375642321L);
  const int axisFlag(InputControl::flagExtract(Names,"a","axis"));
  const int endfFlag(InputControl::flagExtract(Names,"e","endf"));
  InputControl::flagVExtract(Names,"m","multi",multi);
  InputControl::flagVExtract(Names,"n","nps",nps);
  InputControl::flagVExtract(Names,"s","random",randNum);
  const int phitsFlag(InputControl::flagExtract(Names,"p","phits"));
  const int renumberFlag(InputControl::flagExtract(Names,"r","renumber"));

  // Set up output information:
  ELog::EM.setNBasePtr(RControl.getBasePtr());
  ELog::EM.setTypeFlag(0);
  ELog::EM.setActive(255);
  ELog::FM.setActive(255);

  long int randNum(375642321L);
  std::vector<std::string> Names;  
  std::map<std::string,std::string> AddValues;  
  std::map<std::string,std::string> Values;  
  InputControl::mainVector(argc,argv,Names);
  int multi(1);
  const int multiFlag(InputControl::flagVExtract(Names,"m","multi",multi));
  int nps(600000);
  InputControl::flagVExtract(Names,"n","nps",nps);

  InputControl::flagVExtract(Names,"s","random",randNum);
  const int phitsFlag(InputControl::flagExtract(Names,"p","phits"));
  const int renumberFlag(InputControl::flagExtract(Names,"r","renumber"));

  std::string varName;
  std::string varExpr;
  // Added values
  while(InputControl::flagVExtract(Names,"va","addvalue",varName,varExpr))
    {
      AddValues[varName]=varExpr;
    }
  while(InputControl::flagVExtract(Names,"v","value",varName,varExpr))
    {
      Values[varName]=varExpr;
    }

  // Now count parameters:
  if (Names.size()<2)
    {
      std::cout<<"Insufficient files ::"
	       <<" Eng.i out.x"<<std::endl
	       <<"Options :"<<std::endl
	       <<" -m multinum :: Number of index files with different RND seeds."<<std::endl
	       <<" -n Nps :: Number of point"<<std::endl
	       <<" -p PHITS output "<<std::endl
	       <<" -r renumber output cell "<<std::endl
	       <<" -s random number seed "<<std::endl
	       <<std::endl;
      exit(1);
    }    

  Simulation* SimPtr=(phitsFlag) ? new SimPHITS : new Simulation;

  const std::string Fname=Names[0];         // Master file name
  const std::string Oname=Names[1];         // Output file name

  /*!
    The big variable setting
  */

  std::istringstream cx;
  setVariable(SimPtr->getDataBase());
  SimPtr->registerAlterSurface(new AlterSurfTS2(SimPtr->getDataBase()));
  SimPtr->registerRemoveCell(new TS2remove());   // database/cells get set later
  // Definitions section 
  std::vector<int> heatCells;
  for(int i=0;i<3;i++)
    {
      heatCells.push_back(80002+i);
      heatCells.push_back(80011+i);
    }
  try
    {
      for(int MCIndex=0;MCIndex<multi;MCIndex++)
        {
      	  SimPtr->resetAll();
	  setVariable(SimPtr->getDataBase());
	  mainSystem::setRunTimeVariable(SimPtr->getDataBase(),
					 Values,AddValues);
	  SimPtr->readMaster(Fname);

	  TMRSystem::setVirtualContainers(*SimPtr);
	  //      TMRSystem::addSapphire(Sim);
	  TMRSystem::makeNGroove(*SimPtr);
	  TMRSystem::makeBGroove(*SimPtr);
	  TMRSystem::addIradBeamline(*SimPtr); 
	  
	  shutterSystem::createShutterInfo(SInfo,GData);
	  shutterSystem::setShutterCentre(*SimPtr,SInfo);
	  shutterSystem::addVoidVessel(*SimPtr);    
	  shutterSystem::addShutters(*SimPtr,SInfo);
	  for(unsigned int i=0;i<SInfo.size();i++)
	    shutterSystem::addShutterUnits(*SimPtr,SInfo,GData,i);

	  shutterSystem::addBulkShield(*SimPtr,SInfo);
	  shutterSystem::processInnerVoid(*SimPtr,shutterSystem::outer);
	  //	  shutterSystem::processInnerVoid(*SimPtr,shutterSystem::reflector);
	  //	  hutchSystem::createWallsChipIR(*SimPtr,SInfo[0]);

	  ELog::EM.lock();
	  SimPtr->removeDeadCells();            // Generic
	  ELog::EM.dispatch(0);
	  ELog::EM.lock();
	  SimPtr->removeDeadSurfaces();         // Generic
	  ELog::EM.dispatch(0);
	  
	  SimPtr->setCutter(76);
	  
	  // NEED  to set all the cells from simulations here
	  SimPtr->getPC().setCells("imp",1,0);                // Set a zero cell
	  //      WeightSystem::addForcedCollision(Sim,40.0);
	  
	  // Sets a line of tallies at different angles
	  TMRSystem::setAllTally(*SimPtr,SInfo);
      
	  //	  TMRSystem::removeTallyWindows(*SimPtr);
	  TMRSystem::setWeights(*SimPtr);
	  SimPtr->removeComplements();
	  SimPtr->getPC().setNPS(nps);
	  SimPtr->getPC().setRND(randNum);	
	  //	  tallySystem::addHeatBlock(*SimPtr,heatCells);
	  if (renumberFlag) 
	    {
	      SimPtr->renumberCells();
	      SimPtr->renumberSurfaces();
	    }
	  SimProcess::writeIndexSim(*SimPtr,Oname,MCIndex);
	}
      SimPtr->writeCinder();
    }
  catch (ColErr::ExBase& A)
    {
      std::cerr<<A.what()<<std::endl;
      exit(1);
    }
  delete SimPtr;
  return 0;
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
  Control.addVariable("beamDX",0.0);        // Shift in beam height
  Control.addVariable("beamDY",0.0);        // Shift in beam horrizontal
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
  Control.addVariable("groove",3.0);        // Size of the groove vertical
  Control.addVariable("gangle",72);         // Angle of the groove moderator
  Control.addVariable("pmsize",0.5);        // Premoderator size
  Control.addVariable("pinneral",0.15);     // Al thickness next to target
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
/// Reflector
  Control.addVariable("reflectal",0.2);    // Al layer in reflector
  Control.addVariable("refgroove",3.3);    // Height of the groove in ref
  Control.addVariable("refsize",35);       // size of the reflector (half width)
  Control.addVariable("refend",30.0*sqrt(2.0)); // size to the flat ends
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

// OUTER STEEL
  Control.addVariable("shieldrad",150);  // Outer shield radius
  Control.addVariable("shieldthick",600);  // Outer shield thickness
  Control.addVariable("shieldmat",3);    // Steel 304
  Control.addVariable("voidHeight",100);    // void heigh
  Control.addVariable("shieldHeight",300);    // top of the steel
  Control.addVariable("voidBase",100);       // void heigh
  Control.addVariable("shieldBase",300);    // top of the steel

  // IRRADIATIION NEW:
  Control.addVariable("irInnerRadius",2.0);     // Radius of beam
  Control.addVariable("irTCut",10.0);           // Cut of target from front face in centre of beamline  
  Control.addVariable("irAngle",-46);           // Angle of beamline
  Control.addVariable("irSlope",-4.76);          // Angle of beamline
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
  Control.addVariable("irOutBMat",38);          // (outer) material
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
  Control.addVariable("voidWinMat",5);          // Void vessel window material

  Control.addVariable("portSize",9.0);          // viewport size

  // SHUTTER VESSEL:
  Control.addVariable("shutterZoffset",-4.475);    // Shutter offset to target centre
  Control.addVariable("shutterFCut",28.9);         // Cut in the inner steel (EPB side)
  Control.addVariable("shutterInThick",185.7);     // Shutter Inner steel
  Control.addVariable("shutterOutThick",416.6);    // Shutter Outer steel
  Control.addVariable("shutterRoof",1004.6);       // Shutter Inner roof steel
  Control.addVariable("shutterBase",1171.1);       // Shutter Inner base steel
  Control.addVariable("shutterInnerThick",185.7);      // Shutter Inner steel
  Control.addVariable("shutterInsertBaseVoid",152.5);  // Shutter Insert thickness
  Control.addVariable("shutterInsertTopVoid",171.1);   // Shutter Insert thickness
  Control.addVariable("shutterInsertThick",118.9); // Shutter Insert thickness
  Control.addVariable("shutterInsertGap",11.15);   // Shutter Insert thickness
  Control.addVariable("shutterInsertChipGap",25);  // Shutter Insert thickness
  Control.addVariable("shutterHWidth",12.0);       // Shutter Half width
  Control.addVariable("shutterMat",3);             // shutter Material
  Control.addVariable("shutterSurMat",3);          // shutter surround mat
  // CHIP SHUTTER INSERT:
  Control.addVariable("chipInsertIN",1);          // Insert number of inner cylinder
  Control.addVariable("chipInsertON",1);          // Insert number of outer cylinder
  Control.addVariable("chipIFStep",8.0);          // Forward step off chip IF.
  Control.addVariable("chipMFStep",16.0);         // Second step off chip IF.
  Control.addVariable("chipShineMat",3);          // Shutter shine material
  Control.addVariable("chipShineHeight",4.0);     // Shutter shine Height
  Control.addVariable("chipShineWidth",4.0);      // Shutter shine Width 
  Control.addVariable("chipShineVoffset",0.0);    // Shutter shine vertical offset
  Control.addVariable("chipShineHoffset",-2.0);    // Shutter shine horrizonal 

   
  Control.addVariable("chipIR1",8.2);            // Radius of cylinder
  Control.addVariable("chipIR2",10.3);           // Radius of cylinder 
  Control.addVariable("chipIL1_1",1.0);            // Length of cylinder
  Control.addVariable("chipIL1_2",1.0);            // Length of cylinder
  Control.addVariable("chipIL2_1",1.0);            // Length of cylinder
  Control.addVariable("chipIL2_2",1.0);            // Length of cylinder
  Control.addVariable("chipIM1_1",3);              // Material of cylinder
  Control.addVariable("chipIM1_2",0);              // Material of cylinder
  Control.addVariable("chipIM2_1",3);              // Material of cylinder
  Control.addVariable("chipIM2_2",0);              // Material of cylinder

  Control.addVariable("chipIME_1",3);              // Material of external
  Control.addVariable("chipIME_2",0);              // Material of external
  Control.addVariable("chipILE_1",1.0);            // length of external
  Control.addVariable("chipILE_2",1.0);            // length of external

  // BULK VESSEL:
  Control.addVariable("bulkInThick",111.5);    // Bulk First compartment
  Control.addVariable("bulkOutThick",120.80);  // Bulk Second compartment
  Control.addVariable("bulkInTop",50.2);       // Bulk Height (from centre line)
  Control.addVariable("bulkInBase",63.2);      // Bulk Base (from centre line)
  Control.addVariable("bulkOutTop",52.2);       // Bulk Height (from centre line)
  Control.addVariable("bulkOutBase",68.8);      // Bulk Base (from centre line)

  Control.addVariable("bulkInWidth",36);        // Bulk Height [cm] (from centre line)
  Control.addVariable("bulkOutWidth",40);       // Bulk Width [cm] (from centre line) 
  

  // ROOM:: 
  Control.addVariable("chipSndAngle",0.25);      // Secondary angle [degrees]
  Control.addVariable("chipHutYStart",1235.0);   // Start of hutch from target centre
  Control.addVariable("chipHutYLen",1350.0);     // Full hutch length
  Control.addVariable("chipHutYFlat",350.0);     // Length of straight edge
  Control.addVariable("chipHutNoseWidth",240.0); // Nose width
  Control.addVariable("chipHutFrontWidth",382.129); // Outside width
  Control.addVariable("chipHutFullWidth",610.00); // Outside width
  Control.addVariable("chipHutYNose",250.0);     // Nose length
  Control.addVariable("chipHutYSlopeDist",1000.0);     // Sloped lendth
  Control.addVariable("chipHutRXStep",135.50);     // Right step 
  Control.addVariable("chipHutRoof",409.5);       // Roof from target centre
  Control.addVariable("chipHutFloor",190.5);      // Floor from target centre
  Control.addVariable("chipHutCollRoof",154.5);   // Height of collimator roof
  Control.addVariable("chipHutRoofSteel",100.0);   // Thickness of roof steel
  Control.addVariable("chipHutLeftWallSteel",100.0);   // Thickness of left wall
  Control.addVariable("chipHutRightWallSteel",100.0);  // Thickness of right wall 

  Control.addVariable("chipHutRoofMat",3);       // Steel
  Control.addVariable("chipHutWallMat",3);       // Steel
  // Hutch inner stuff:
  Control.addVariable("chipHutFrontTrimLen",100.0);  // Thickness initial trimmer
  Control.addVariable("chipHutFrontTrimWidth",150.0);  // Horrizontal beam width
  Control.addVariable("chipHutFrontTrimHeight",75.0);  // Vertial height from beam point
  Control.addVariable("chipHutFrontTrimDepth",75.0);  // Vertial height from beam point

  
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
  Control.Parse("-totdcheight-(targettop+DCPreModTThick+dclead)");
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
      exit(1);
    }

  //
  // EARLY RETURN ::
  //  - This bit is needed for a groove but not debugged for no groove
  // 
  return;

  if (Control.EvalVar<double>("ngvlength")>0.0)
    Control.Parse("ngvheight-daltern-midvacgap-dalheat-vacgap-dalpress+ngvoffset");
  else
    Control.Parse("totdcheight-(gdlayer+dalheat+vacgap+dalpress+daltern+midvacgap");      
  const double p11686=Control.Eval();
  Control.Parse("ngvholeheight+ngvoffset+ngvInnerOffset");
  const double p12186=Control.Eval();
  std::cout<<"p11 = "<<p11686<<" "<<p12186<<std::endl;
  if (p12186>p11686)
    {
      ELog::EM.Estream()<<"XGroove :: setting ngvInnerOffset from : "
			       <<Control.EvalVar<double>("ngvInnerOffset");
      Control.setVariable("ngvInnerOffset",Control.EvalVar<double>("ngvInnerOffset")-p12186+p11686);
      ELog::EM.Estream()<<" to : "<<Control.EvalVar<double>("ngvInnerOffset");
      ELog::EM.warning();
      exit(1);
    }

  return;
}



