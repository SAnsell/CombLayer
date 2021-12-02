/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   R3Common/R3RingVariables.cxx
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"

#include "CFFlanges.h"
#include "RingDoorGenerator.h"

#include "PipeGenerator.h"
#include "PipeTubeGenerator.h"
#include "VacBoxGenerator.h"
#include "SplitPipeGenerator.h"
#include "BellowGenerator.h"
#include "CrossGenerator.h"
#include "PortItemGenerator.h"
#include "HeatDumpGenerator.h"
#include "GateValveGenerator.h"
#include "CylGateValveGenerator.h"
#include "BeamMountGenerator.h"
#include "WallLeadGenerator.h"

#include "EPSeparatorGenerator.h"
#include "R3ChokeChamberGenerator.h"
#include "MagnetM1Generator.h"

namespace setVariable
{  

void heatDumpTable(FuncDataBase&,const std::string&);
void heatDumpVariables(FuncDataBase&,const std::string&);
void shutterTable(FuncDataBase&,const std::string&);
void moveApertureTable(FuncDataBase&,const std::string&);
void collimatorVariables(FuncDataBase&,const std::string&);
void wallVariables(FuncDataBase&,const std::string&);

void
wallVariables(FuncDataBase& Control,const std::string& wallKey)
 /*!
    Set the variables for the frontend wall
    \param Control :: DataBase to use
    \param wallKey :: name before part names
  */
{
  ELog::RegMethod RegA("R3RingVariables[F]","wallVariables");

  WallLeadGenerator LGen;
  LGen.setWidth(140.0,70.0);
  LGen.generateWall(Control,wallKey,3.0);

  return;
}
  
void
collimatorVariables(FuncDataBase& Control,const std::string& collKey)
  /*!
    Builds the variables for the collimator
    \param Control :: Database
    \param collKey :: prename
  */
{
  ELog::RegMethod RegA("R3RingVariables[F]","collimatorVariables");

  Control.addVariable(collKey+"Width",4.0);
  Control.addVariable(collKey+"Height",4.0);
  Control.addVariable(collKey+"Length",16.0);
  Control.addVariable(collKey+"InnerAWidth",1.2);
  Control.addVariable(collKey+"InnerAHeight",1.2);
  Control.addVariable(collKey+"InnerBWidth",1.2);
  Control.addVariable(collKey+"InnerBHeight",1.2);
  Control.addVariable(collKey+"Mat","Tantalum");

  return;
}

void
moveApertureTable(FuncDataBase& Control,
		  const std::string& frontKey)
  /*!
    Builds the variables for the moveable apperature table
    containing two movable apertures, pumping and bellows
    \param Control :: Database
    \param frontKey :: prename
  */
{
  ELog::RegMethod RegA("cosaxsVariables[F]","moveApertureTable");

  setVariable::BellowGenerator BellowGen;
  setVariable::PipeGenerator PipeGen;
  setVariable::CrossGenerator CrossGen;

  PipeGen.setWindow(-2.0,0.0);   // no window
  PipeGen.setMat("Stainless304");
  PipeGen.setCF<CF40>();
  PipeGen.setBFlangeCF<CF63>();
  PipeGen.generatePipe(Control,frontKey+"PipeB",15.0);

  BellowGen.setCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,frontKey+"BellowE",14.0);

  // Aperture pipe is movable:
  PipeGen.setCF<CF63>();
  PipeGen.generatePipe(Control,frontKey+"AperturePipe",24.0);
  Control.addVariable(frontKey+"AperturePipeYStep",14.0);
  collimatorVariables(Control,frontKey+"MoveCollA");
  
  BellowGen.setCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,frontKey+"BellowF",14.0);

  // Stepped 420mm from pipeB so bellows/aperturePipe can move freely
  CrossGen.setMat("Stainless304");
  CrossGen.setPlates(0.5,2.0,2.0);     // wall/Top/base
  CrossGen.setTotalPorts(7.5,7.5);     // len of ports (after main)
  CrossGen.generateDoubleCF<setVariable::CF63,setVariable::CF100>
    (Control,frontKey+"IonPC",52.0,15.74,28.70);   // height/depth

  // [FREE FLOATING]
  BellowGen.setCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,frontKey+"BellowG",14.0);

  // Aperture pipe is movable:
  PipeGen.setCF<CF63>();
  PipeGen.generatePipe(Control,frontKey+"AperturePipeB",24.0);
  Control.addVariable(frontKey+"AperturePipeBYStep",14.0);
  collimatorVariables(Control,frontKey+"MoveCollB");
  Control.addVariable(frontKey+"MoveCollBYAngle",180.0);
  
  // [FREE FLOATING]
  BellowGen.setCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,frontKey+"BellowH",14.0);

  // [End fix for BellowH]
  PipeGen.setCF<CF40>();
  PipeGen.setAFlangeCF<CF63>();
  PipeGen.generatePipe(Control,frontKey+"PipeC",10.0);
  Control.addVariable(frontKey+"PipeCYStep",52.0);
  
  return;
}
  
void
shutterTable(FuncDataBase& Control,
	     const std::string& frontKey)
  /*!
    Set the variables for the shutter table (number 3)
    \param Control :: DataBase to use
    \param frontKey :: name before part names
  */
{
  ELog::RegMethod RegA("R3RingVariables[F]","shutterTable");

  setVariable::BellowGenerator BellowGen;
  setVariable::GateValveGenerator GateGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::PipeGenerator PipeGen;
  setVariable::BeamMountGenerator BeamMGen;
  setVariable::CylGateValveGenerator GVGen;
    
  // joined and open
  GateGen.setLength(3.5);
  GateGen.setCubeCF<setVariable::CF40>();
  GateGen.generateValve(Control,frontKey+"GateA",0.0,0);
  
  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,frontKey+"BellowI",10.0);
  
  SimpleTubeGen.setCF<CF100>();
  SimpleTubeGen.setCap();
  SimpleTubeGen.generateTube(Control,frontKey+"FlorTubeA",16.0);

  // beam ports
  const std::string florName(frontKey+"FlorTubeA");
  Control.addVariable(florName+"NPorts",4);
  const Geometry::Vec3D XVec(1,0,0);
  const Geometry::Vec3D ZVec(0,0,1);

  PItemGen.setCF<setVariable::CF40>(CF100::outerRadius+2.0);
  PItemGen.setPlate(0.0,"Void");  
  PItemGen.generatePort(Control,florName+"Port0",Geometry::Vec3D(0,0,0),ZVec);
  PItemGen.generatePort(Control,florName+"Port1",Geometry::Vec3D(0,0,0),-ZVec);
  PItemGen.generatePort(Control,florName+"Port2",Geometry::Vec3D(0,0,0),XVec);
  PItemGen.generatePort(Control,florName+"Port3",Geometry::Vec3D(0,0,0),-XVec);

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,frontKey+"BellowJ",10.0);

  GVGen.generateGate(Control,frontKey+"GateTubeB",0);
  
  PipeGen.setMat("Stainless304");
  PipeGen.setWindow(-2.0,0.0);   // no window
  PipeGen.setCF<setVariable::CF40>();
  PipeGen.setBFlangeCF<setVariable::CF150>();
  PipeGen.generatePipe(Control,frontKey+"OffPipeA",6.8);
  Control.addVariable(frontKey+"OffPipeAFlangeBackZStep",3.0);


  const std::string shutterName=frontKey+"ShutterBox";
  const double sBoxLen(51.0);
  SimpleTubeGen.setCF<CF150>();
  SimpleTubeGen.setCap(0,0);
  SimpleTubeGen.generateTube(Control,shutterName,sBoxLen);
  Control.addVariable(frontKey+"ShutterBoxNPorts",2);
  
  // 20cm above port tube
  PItemGen.setCF<setVariable::CF50>(14.0);
  PItemGen.setPlate(setVariable::CF50::flangeLength,"Stainless304");
  // lift is actually 60mm [check]
  BeamMGen.setThread(1.0,"Nickel");
  BeamMGen.setLift(5.0,0.0);
  BeamMGen.setCentreBlock(6.0,6.0,20.0,0.0,"Tungsten");  

  // centre of mid point
  Geometry::Vec3D CPos(0,-sBoxLen/4.0,0);
  for(size_t i=0;i<2;i++)
    {
      const std::string name=frontKey+"ShutterBoxPort"+std::to_string(i);
      const std::string fname=frontKey+"Shutter"+std::to_string(i);
      
      PItemGen.generatePort(Control,name,CPos,ZVec);
      BeamMGen.generateMount(Control,fname,1);      // out of beam:upflag=1
      CPos+=Geometry::Vec3D(0,sBoxLen/2.0,0);
    }

  PipeGen.setCF<setVariable::CF63>();
  PipeGen.setAFlangeCF<setVariable::CF150>();
  PipeGen.generatePipe(Control,frontKey+"OffPipeB",20.0);
  Control.addVariable(frontKey+"OffPipeBFlangeFrontZStep",3.0);
  Control.addVariable(frontKey+"OffPipeBZStep",-3.0);

  Control.addVariable(frontKey+"BremBlockRadius",3.0);
  Control.addVariable(frontKey+"BremBlockLength",20.0);
  Control.addVariable(frontKey+"BremBlockHoleWidth",2.0);
  Control.addVariable(frontKey+"BremBlockHoleHeight",2.0);
  Control.addVariable(frontKey+"BremBlockMainMat","Tungsten");

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.setAFlangeCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,frontKey+"BellowK",11.05);
  
  return;
}

void
heatDumpTable(FuncDataBase& Control,
	      const std::string& frontKey)
  /*!
    Builds the variables for the heat dump table
    containing the heatdump and a gate valve [non-standard]
    \param Control :: Database
    \param frontKey :: prename
  */
{
  ELog::RegMethod RegA("R3RingVariables[F]","heatDumpTable");

  setVariable::BellowGenerator BellowGen;
  setVariable::PipeGenerator PipeGen;
  setVariable::CrossGenerator CrossGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::PortItemGenerator PItemGen;
    
  PipeGen.setWindow(-2.0,0.0);   // no window
  PipeGen.setMat("Stainless304");
  
  heatDumpVariables(Control,frontKey);

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,frontKey+"BellowD",10.0);

  // will be rotated vertical
  // length 8.1
  CylGateValveGenerator CGTGen;
  CGTGen.generateGate(Control,frontKey+"GateTubeA",0);


  CrossGen.setMat("Stainless304");
  CrossGen.setPlates(0.5,2.0,2.0);  // wall/Top/base
  CrossGen.setTotalPorts(10.0,10.0);     // len of ports (after main)
  CrossGen.generateDoubleCF<setVariable::CF40,setVariable::CF100>
    (Control,frontKey+"IonPB",0.0,26.6,26.6);

  return;
}

 
void
heatDumpVariables(FuncDataBase& Control,const std::string& frontKey)
  /*!
    Build the heat dump variables
    \param Control :: Database
    \param frontKey :: prename
   */
{
  ELog::RegMethod RegA("R3RingVariables[F]","heatDumpVariables");
  
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::HeatDumpGenerator HeatGen;

  const double heatDumpDist(1708.75);
    
  SimpleTubeGen.setMat("Stainless304");
  SimpleTubeGen.setCF<CF150>();
  SimpleTubeGen.setCap(1,0);
  SimpleTubeGen.generateTube(Control,frontKey+"HeatBox",20.0);
  Control.addVariable(frontKey+"HeatBoxYStep",heatDumpDist);
  Control.addVariable(frontKey+"HeatBoxNPorts",2);


  // beam ports
  PItemGen.setCF<setVariable::CF40>(13.05);
  PItemGen.setPlate(0.0,"Void");  


  const Geometry::Vec3D ZVec(0,0,1);
  const std::string heatName=frontKey+"HeatBoxPort";
  const std::string hName=frontKey+"HeatDumpFlange";
  PItemGen.generatePort(Control,heatName+"0",Geometry::Vec3D(0,0,0),ZVec);
  PItemGen.generatePort(Control,heatName+"1",Geometry::Vec3D(0,0,0),-ZVec);

  
  const std::string hDump(frontKey+"HeatDump");
  HeatGen.setCF<CF100>();
  HeatGen.setTopCF<CF150>();
  HeatGen.generateHD(Control,hDump,1);
  
  return;
}

void
R3RingDoors(FuncDataBase& Control,const std::string& preName)
  /*!
    Construct variables for R3RingDoors 
    \param Control :: Control value
    \param preName :: Prename (r3ring typically)
  */
{
  ELog::RegMethod RegA("R3RingVariables[F]","R3RingDoors");

  RingDoorGenerator RGen;
  
  Control.addVariable(preName+"RingDoorWallID",1);
  
  RGen.generateDoor(Control,preName+"RingDoor",2400.0);
  return;
}



void
R3FrontEndVariables(FuncDataBase& Control,
		    const std::string& beamlineKey)
  /*!
    Set the variables for the front end
    \param Control :: DataBase to use
    \param beamlineKey :: name of beamline
  */
{
  ELog::RegMethod RegA("R3RingVariables[F]","R3FrontEndVariables");

  setVariable::BellowGenerator BellowGen;
  setVariable::PipeGenerator PipeGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::VacBoxGenerator VBoxGen;

  setVariable::EPSeparatorGenerator ESGen;
  setVariable::R3ChokeChamberGenerator CCGen;

  const std::string frontKey(beamlineKey+"FrontBeam");
  // Master off set from division --
  Control.addVariable(frontKey+"YStep",524.4);
  Control.addVariable(frontKey+"XStep",0.0);  
  Control.addVariable(frontKey+"OuterRadius",60.0);

  // BuildZone offset
  Control.addVariable(frontKey+"FrontOffset",-400.0);  

  PipeGen.setNoWindow();
  PipeGen.setMat("Copper");
  // placeholder length (100.0)
  PipeGen.generatePipe(Control,frontKey+"TransPipe",100.0); 
  
  setVariable::MagnetM1Generator M1Gen;
  M1Gen.generateBlock(Control,frontKey+"M1Block");

  ESGen.generatePipe(Control,frontKey+"EPSeparator");
  
  CCGen.generateChamber(Control,frontKey+"ChokeChamber");
  CCGen.generateInsert(Control,frontKey+"ChokeInsert");

  PipeGen.setCF<CF40>();
  PipeGen.setMat("Stainless304");
  PipeGen.generatePipe(Control,frontKey+"DipolePipe",800.00); 

  //  Note bellow reversed for FM fixed:
  BellowGen.setCF<setVariable::CF40>();
  BellowGen.setAFlangeCF<setVariable::CF100>();
  BellowGen.generateBellow(Control,frontKey+"BellowA",16.0);

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.setAFlangeCF<setVariable::CF100>();
  BellowGen.generateBellow(Control,frontKey+"BellowB",16.0);
  
  PipeGen.setCF<CF40>();
  PipeGen.generatePipe(Control,frontKey+"CollABPipe",222.0);

  Control.addVariable(frontKey+"ECutDiskYStep",5.0);
  Control.addVariable(frontKey+"ECutDiskLength",0.1);
  Control.addVariable(frontKey+"ECutDiskRadius",1.0);
  Control.addVariable(frontKey+"ECutDiskDefMat","H2Gas#0.1");
  
  Control.addVariable(frontKey+"ECutMagDiskYStep",2.0);
  Control.addVariable(frontKey+"ECutMagDiskLength",0.1);
  // note: CF40::innerRadius is some complex template type 
  Control.addVariable
    (frontKey+"ECutMagDiskRadius",static_cast<double>(CF25::innerRadius));
  Control.addVariable(frontKey+"ECutMagDiskDefMat","H2Gas#0.1");

  // note : reversed becaues using fixed FM
  BellowGen.setCF<setVariable::CF40>();
  BellowGen.setAFlangeCF<setVariable::CF100>();
  BellowGen.generateBellow(Control,frontKey+"BellowC",16.0);

  PipeGen.setCF<setVariable::CF40>();
  PipeGen.setAFlangeCF<setVariable::CF100>();
  PipeGen.generatePipe(Control,frontKey+"CollExitPipe",165.5);

  // Create HEAT DUMP
  heatDumpTable(Control,frontKey);
  moveApertureTable(Control,frontKey);
  shutterTable(Control,frontKey);
  
  PipeGen.setCF<setVariable::CF40>(); 
  PipeGen.generatePipe(Control,frontKey+"ExitPipe",24.0);

  wallVariables(Control,beamlineKey+"WallLead");
  return;
}

  
void
R3RingVariables(FuncDataBase& Control)
  /*!
    Function to set the control variables and constants
    for the R3 concrete shielding walls
    \param Control :: Function data base to add constants too
  */
{
  ELog::RegMethod RegA("R3RingVariables[F]","R3RingVariables");

  const std::string preName("R3Ring");

  Control.addVariable(preName+"FullOuterRadius",14000.0);
  Control.addVariable(preName+"IcosagonRadius",7865.0);       // U
  Control.addVariable(preName+"BeamRadius",8409.0-48.0);       // 528m circum.
  Control.addVariable(preName+"IcosagonWallThick",90.0);
  Control.addVariable(preName+"OffsetCornerX",716.0);
  Control.addVariable(preName+"OffsetCornerY",558.0);
  Control.addVariable(preName+"OuterWall",110.0);
  Control.addVariable(preName+"OuterWallCut",-40.0);
  Control.addVariable(preName+"RatchetWall",120.0);

  Control.addVariable(preName+"Insulation",10.0);
  Control.addVariable(preName+"InsulationCut",400.0);
  Control.addVariable(preName+"InsulationDepth",80.0);

  
  Control.addVariable(preName+"Height",180.0);
  Control.addVariable(preName+"Depth",130.0);
  Control.addVariable(preName+"RoofThick",100.0);
  Control.addVariable(preName+"FloorThick",100.0);

  Control.addVariable(preName+"WallMat","Concrete");  
  Control.addVariable(preName+"FloorMat","Concrete");
  Control.addVariable(preName+"RoofMat","Concrete");
  
  R3RingDoors(Control,preName);

  return;
}

}  // NAMESPACE setVariable
