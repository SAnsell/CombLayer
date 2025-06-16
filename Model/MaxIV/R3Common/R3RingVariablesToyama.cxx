/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   R3Common/R3RingVariables.cxx
 *
 * Copyright (c) 2004-2025 by  Konstantin Batkov / Stuart Ansell
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
#include <cassert>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"

#include "CFFlanges.h"

#include "PipeGenerator.h"
#include "CornerPipeGenerator.h"
#include "PipeTubeGenerator.h"
#include "VacBoxGenerator.h"
#include "BellowGenerator.h"
#include "CrossGenerator.h"
#include "PortItemGenerator.h"
#include "HeatDumpGenerator.h"
#include "GateValveGenerator.h"
#include "CylGateValveGenerator.h"
#include "BeamMountGenerator.h"

#include "EPSeparatorGenerator.h"
#include "R3ChokeChamberGenerator.h"
#include "MagnetM1Generator.h"
#include "MagnetU1Generator.h"
#include "BremBlockGenerator.h"
#include "HeatAbsorberToyamaGenerator.h"
#include "ProximityShieldingGenerator.h"
#include "CleaningMagnetGenerator.h"


// References
// [1] ForMAX and MicroMAX Frontend Technical Specification
//     http://localhost:8080/maxiv/work-log/micromax/pictures/front-end/formax-and-micromax-frontend-technical-specification.pdf/view
// [2] Toyama ForMAX Mechanical drawings
//     http://localhost:8080/maxiv/work-log/tomowise/toyama_formax_fe_mechanical_drawings.pdf/view
// [3] 236798-Toyama-front-end.step

namespace setVariable
{
  // defined in R3RingVariables
  void collimatorVariables(FuncDataBase&,const std::string&);
  void wallVariables(FuncDataBase&,const std::string&);

  void heatDumpTableToyama(FuncDataBase&,const std::string&);
  void heatDumpVariablesToyama(FuncDataBase&,const std::string&);
  void shutterTableToyama(FuncDataBase&,const std::string&);
  void moveApertureTableToyama(FuncDataBase&,const std::string&);
  void R3FrontEndToyamaVariables(FuncDataBase& Control, const std::string& beamlineKey);


void
moveApertureTableToyama(FuncDataBase& Control,
		  const std::string& frontKey)
  /*!
    Builds the variables for the moveable apperature table
    containing two movable apertures, pumping and bellows
    \param Control :: Database
    \param frontKey :: prename
  */
{
  ELog::RegMethod RegA("R3RingVariables[F]","moveApertureTableToyama");

  setVariable::BellowGenerator BellowGen;
  setVariable::PipeGenerator PipeGen;
  setVariable::CrossGenerator CrossGen;

  PipeGen.setNoWindow();   // no window
  PipeGen.setMat("Stainless304");
  PipeGen.setCF<CF40>();
  PipeGen.setBFlangeCF<CF63>();
  PipeGen.generatePipe(Control,frontKey+"PipeB",15.0);

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,frontKey+"BellowDA",10.0); // [2]

  BellowGen.setCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,frontKey+"BellowE",14.0); // [2]

  // Aperture pipe is movable:
  PipeGen.setCF<CF63>();
  PipeGen.generatePipe(Control,frontKey+"AperturePipeA",30.0); // [2]
  //  Control.addVariable(frontKey+"AperturePipeAYStep",14.0);
  collimatorVariables(Control,frontKey+"MoveCollA");

  BellowGen.setCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,frontKey+"BellowF",8); //14.0); // [2]

  // Stepped 420mm from pipeB so bellows/aperturePipe can move freely
  CrossGen.setMat("Stainless304");
  CrossGen.setPlates(0.5,2.0,2.0);     // wall/Top/base
  CrossGen.setTotalPorts(7.5,7.5);     // len of ports (after main)
  CrossGen.generateDoubleCF<setVariable::CF63,setVariable::CF100>
    (Control,frontKey+"IonPC",0.0,15.74,28.70);   // height/depth

  // [FREE FLOATING]
  BellowGen.setCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,frontKey+"BellowG",14.0); // [2]

  // Aperture pipe is movable:
  PipeGen.setCF<CF63>();
  PipeGen.generatePipe(Control,frontKey+"AperturePipeB",30.0); // [2]
  //  Control.addVariable(frontKey+"AperturePipeBYStep",14.0);
  collimatorVariables(Control,frontKey+"MoveCollB");
  Control.addVariable(frontKey+"MoveCollBYAngle",180.0);

  // [FREE FLOATING]
  BellowGen.setCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,frontKey+"BellowH",8); //14.0); // [2]

  // [End fix for BellowH]
  PipeGen.setCF<CF40>();
  PipeGen.setAFlangeCF<CF63>();
  PipeGen.generatePipe(Control,frontKey+"PipeC",32.5-11-17); // [2]
  ELog::EM << "PipeC length reduce by 17 cm to avoid geometric errors. Check abs placements / lengths of other components" << ELog::endWarn;
  Control.addVariable(frontKey+"PipeCYStep",52.0);

  return;
}


void
shutterTableToyama(FuncDataBase& Control,
	     const std::string& frontKey)
  /*!
    Set the variables for the Toyama front-end shutter table (number 3)
    \param Control :: DataBase to use
    \param frontKey :: name before part names
  */
{
  ELog::RegMethod RegA("R3RingVariables[F]","shutterTableToyama");

  setVariable::BellowGenerator BellowGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::PipeGenerator PipeGen;
  setVariable::BeamMountGenerator BeamMGen;
  setVariable::CylGateValveGenerator GVGen;
  setVariable::ProximityShieldingGenerator PSGen;
  setVariable::BremBlockGenerator BBGen;

  // Lengths are based on [2]
  constexpr double bellowILength = 10.0;
  constexpr double florTubeALength = 12.0; //
  constexpr double bellowJLength = 10.0;
  constexpr double gateTubeBLength = 7.2; //
  constexpr double proxiShieldAPipeLength = 21.5;
  constexpr double proxiShieldBPipeLength = 20.0;
  // safety shutter
  constexpr double shutterLength = 57.8; // [2]
  constexpr double offPipeALength = 6.8; // approx
  constexpr double shutterBoxLength = shutterLength - offPipeALength;

  constexpr double bremCollTotalLength = 21.0; //[2] (OffPipeB + BremCollPipe)
  constexpr double offPipeBLength = 2.6; // as small as possible
  constexpr double bremCollPipeLength = bremCollTotalLength - offPipeBLength;

  constexpr double proxiShieldBPipeEnd = 2110.0 - 2.97; // [2, page1]
  constexpr double bellowIYstep = proxiShieldBPipeEnd - proxiShieldBPipeLength -
    bremCollTotalLength - shutterLength - proxiShieldAPipeLength - gateTubeBLength -
    bellowJLength - florTubeALength - bellowILength;
  // same as counting from Movable Mask 2
  // 18692.8 + 300 + 140 + 17.5 + 325

  assert(std::abs(bellowIYstep - 1947.53)<Geometry::zeroTol && "Wrong shutter table length.");

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,frontKey+"BellowI",bellowILength);
  Control.addVariable(frontKey+"BellowIYStep",bellowIYstep);

  SimpleTubeGen.setCF<CF66_TDC>();
  SimpleTubeGen.setCap();
  SimpleTubeGen.generateTube(Control,frontKey+"FlorTubeA",16.0);
  Control.addVariable(frontKey+"FlorTubeARadius",7.63/2-0.2);

  // beam ports
  const std::string florName(frontKey+"FlorTubeA");
  Control.addVariable(florName+"NPorts",4);
  const Geometry::Vec3D XVec(1,0,0);
  const Geometry::Vec3D ZVec(0,0,1);

  PItemGen.setCF<setVariable::CF40>(CF100::outerRadius+2.0);
  PItemGen.setPlate(0.0,"Void");
  PItemGen.setOuterVoid(0);
  PItemGen.generatePort(Control,florName+"Port0",Geometry::Vec3D(0,0,0),ZVec);
  PItemGen.generatePort(Control,florName+"Port1",Geometry::Vec3D(0,0,0),-ZVec);
  PItemGen.generatePort(Control,florName+"Port2",Geometry::Vec3D(0,0,0),XVec);
  PItemGen.generatePort(Control,florName+"Port3",Geometry::Vec3D(0,0,0),-XVec);

  Control.addVariable(florName+"Port0Length",6);
  Control.addVariable(florName+"Port1Length",6);


  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,frontKey+"BellowJ",bellowJLength);

  // V3 Valve
  GVGen.generateGate(Control,frontKey+"GateTubeB",0);
  Control.addVariable(frontKey+"GateTubeBRadius",3.25);
  Control.addVariable(frontKey+"GateTubeBWallThick",0.2);
  Control.addVariable(frontKey+"GateTubeBPortThick",0.6-0.45);

  PipeGen.setMat("Stainless304");
  PipeGen.setNoWindow();   // no window
  PipeGen.setCF<setVariable::CF40>();
  PipeGen.setBFlangeCF<setVariable::CF150>();
  PipeGen.generatePipe(Control,frontKey+"OffPipeA",offPipeALength);
  Control.addVariable(frontKey+"OffPipeAFlangeBZStep",3.0);


  const std::string shutterName=frontKey+"ShutterBox";
  SimpleTubeGen.setCF<CF150>();
  SimpleTubeGen.setCap(0,0);
  SimpleTubeGen.generateTube(Control,shutterName,shutterBoxLength);
  Control.addVariable(shutterName+"NPorts",2);

  // 20cm above port tube
  PItemGen.setCF<setVariable::CF50>(14.0);
  PItemGen.setPlate(setVariable::CF50::flangeLength,"Stainless304");
  // lift is actually 60mm [check]
  BeamMGen.setThread(1.0,"Nickel");
  BeamMGen.setLift(5.0,0.0);
  BeamMGen.setCentreBlock(6.0,6.0,20.0,0.0,"Tungsten");

  // centre of mid point
  Geometry::Vec3D CPos(0,-shutterBoxLength/4.0,0);
  for(size_t i=0;i<2;i++)
    {
      const std::string name=frontKey+"ShutterBoxPort"+std::to_string(i);
      const std::string fname=frontKey+"Shutter"+std::to_string(i);

      PItemGen.generatePort(Control,name,CPos,ZVec);
      BeamMGen.generateMount(Control,fname,1);      // out of beam:upflag=1
      CPos+=Geometry::Vec3D(0,shutterBoxLength/2.0,0);
    }

  PipeGen.setCF<setVariable::CF63>();
  PipeGen.setAFlangeCF<setVariable::CF150>();
  PipeGen.generatePipe(Control,frontKey+"OffPipeB",offPipeBLength);
  Control.addVariable(frontKey+"OffPipeBFlangeAZStep",3.0);
  Control.addVariable(frontKey+"OffPipeBZStep",-3.0);
  Control.addVariable(frontKey+"OffPipeBFlangeBType",0);

  Control.addVariable(frontKey+"BremBlockRadius",3.0);
  Control.addVariable(frontKey+"BremBlockLength",20.0);
  Control.addVariable(frontKey+"BremBlockHoleWidth",2.0);
  Control.addVariable(frontKey+"BremBlockHoleHeight",2.0);
  Control.addVariable(frontKey+"BremBlockMainMat","Tungsten");

  PSGen.generate(Control,frontKey+"ProxiShieldA", 15.0); // CAD
  Control.addVariable(frontKey+"ProxiShieldAYStep",3.53); // CAD
  Control.addVariable(frontKey+"ProxiShieldABoreRadius",0.0);

  PipeGen.setCF<setVariable::CF40>();
  PipeGen.generatePipe(Control,frontKey+"ProxiShieldAPipe",proxiShieldAPipeLength);
  PSGen.generate(Control,frontKey+"ProxiShieldA", 15.0); // CAD
  Control.addVariable(frontKey+"ProxiShieldAYStep",3.53); // CAD
  Control.addVariable(frontKey+"ProxiShieldABoreRadius",0.0);

  Control.copyVarSet(frontKey+"ProxiShieldAPipe", frontKey+"ProxiShieldBPipe");
  Control.copyVarSet(frontKey+"ProxiShieldA", frontKey+"ProxiShieldB");
  Control.addVariable(frontKey+"ProxiShieldBPipeLength",proxiShieldBPipeLength);
  Control.addVariable(frontKey+"ProxiShieldBPipeFlangeARadius",7.5);
  Control.addVariable(frontKey+"ProxiShieldBYStep",setVariable::CF40::flangeLength+0.1); // approx

  // Bremsstrahulung collimator
  std::string name;
  name=frontKey+"BremCollPipe";
  constexpr double bremCollLength(20.0); // collimator block inside BremCollPipe:  CAD+[1, page 26],[2]

  constexpr double bremCollRadius(3.0); // CAD and [1, page 26]
  PipeGen.setCF<setVariable::CF100>();
  PipeGen.generatePipe(Control,name,bremCollPipeLength);
  constexpr double bremCollPipeInnerRadius = 4.5; // CAD
  Control.addVariable(name+"Radius",bremCollPipeInnerRadius);
  Control.addVariable(name+"FlangeARadius",bremCollPipeInnerRadius+CF100::wallThick);
  Control.addVariable(name+"FlangeBRadius",7.5); // CAD
  Control.addVariable(name+"FlangeAInnerRadius",bremCollRadius);
  Control.addVariable(name+"FlangeBInnerRadius",bremCollRadius);

  BBGen.centre();
  BBGen.setMaterial("Tungsten", "Void");
  BBGen.setLength(bremCollLength);
  BBGen.setRadius(bremCollRadius);
  // Calculated by PI and AR based on angular acceptance of FM2 + safety margin
  BBGen.setAperature(-1.0, 2.7, 0.7,  2.7, 0.7,   2.7, 0.7);
  BBGen.generateBlock(Control,frontKey+"BremColl",0);
  Control.addVariable(frontKey+"BremCollYStep",(bremCollPipeLength-bremCollLength)/2.0);

  return;
}

void
heatDumpTableToyama(FuncDataBase& Control,
	      const std::string& frontKey)
  /*!
    Builds the variables for the heat dump table
    containing the heatdump and a gate valve [non-standard]
    \param Control :: Database
    \param frontKey :: prename
  */
{
  ELog::RegMethod RegA("R3RingVariables[F]","heatDumpTableToyama");

  setVariable::BellowGenerator BellowGen;
  setVariable::PipeGenerator PipeGen;
  setVariable::CrossGenerator CrossGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::PortItemGenerator PItemGen;

  PipeGen.setNoWindow();
  PipeGen.setMat("Stainless304");

  heatDumpVariablesToyama(Control,frontKey);

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,frontKey+"BellowD",10.0); // [2]

  setVariable::CylGateValveGenerator GVGen;
  GVGen.generateGate(Control,frontKey+"GateTubePreMM1",0);
  // TODO: all dimensions except total y-length are dummy
  Control.addVariable(frontKey+"GateTubePreMM1PortThick",3.4); // to make 14 cm length as in the CAD file
  Control.addVariable(frontKey+"GateTubePreMM1PortRadius",1.9);
  Control.addVariable(frontKey+"GateTubePreMM1PortFlangeRadius",3.5);
  Control.addVariable(frontKey+"GateTubePreMM1Radius",5);
  Control.addVariable(frontKey+"GateTubePreMM1TopRadius",10);

  // Fast gate valve
  setVariable::GateValveGenerator GateGen;
  GateGen.setLength(3.5);
  GateGen.setCubeCF<setVariable::CF40>();
  GateGen.generateValve(Control,frontKey+"GateA",0.0,0);
  const double gateAYAngle(-30.0); // approx
  Control.addVariable(frontKey+"GateAYAngle",gateAYAngle);

  CrossGen.setMat("Stainless304");
  CrossGen.setPlates(0.5,2.0,2.0);  // wall/Top/base
  CrossGen.setTotalPorts(10.0,10.0);     // len of ports (after main)
  CrossGen.generateDoubleCF<setVariable::CF40,setVariable::CF100>
    (Control,frontKey+"IonPB",0.0,26.6,26.6);
  Control.addVariable(frontKey+"IonPBYAngle",-gateAYAngle);

  return;
}


void
heatDumpVariablesToyama(FuncDataBase& Control,const std::string& frontKey)
  /*!
    Build the heat dump variables
    \param Control :: Database
    \param frontKey :: prename
   */
{
  ELog::RegMethod RegA("R3RingVariables[F]","heatDumpVariablesToyama");

  setVariable::BellowGenerator BellowGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::PortItemGenerator PItemGen;

  constexpr double heatAbsorberLength = 26.5;  // [2]

  const double heatAbsorberDist(1700.0); // centre

  SimpleTubeGen.setMat("Stainless304");
  SimpleTubeGen.setCF<CF150>();
  SimpleTubeGen.setCap(1,0);
  SimpleTubeGen.generateTube(Control,frontKey+"HeatBox",20.0);


  // beam ports
  PItemGen.setCF<setVariable::CF40>(13.05);
  PItemGen.setPlate(0.0,"Void");


  const Geometry::Vec3D ZVec(0,0,1);
  const std::string heatName=frontKey+"HeatBoxPort";
  const std::string hName=frontKey+"HeatDumpFlange";
  PItemGen.generatePort(Control,heatName+"0",Geometry::Vec3D(0,0,0),ZVec);
  PItemGen.generatePort(Control,heatName+"1",Geometry::Vec3D(0,0,0),-ZVec);


  // new Toyama


  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,frontKey+"BellowPreHA",14.0); // [2]

  HeatAbsorberToyamaGenerator HAGen;
  HAGen.generate(Control,frontKey+"HeatAbsorber",heatAbsorberLength);
  Control.addVariable(frontKey+"HeatAbsorberYStep",heatAbsorberDist);

  BellowGen.generateBellow(Control,frontKey+"BellowPostHA",14.0); // [2]


  return;
}

void
R3FrontEndToyamaVariables(FuncDataBase& Control,
		    const std::string& beamlineKey)
  /*!
    Set the variables for the Toyama front-end
    \param Control :: DataBase to use
    \param beamlineKey :: name of beamline
  */
{
  ELog::RegMethod RegA("R3RingVariables[F]","R3FrontEndToyamaVariables");

  std::string name;

  setVariable::BellowGenerator BellowGen;
  setVariable::PipeGenerator PipeGen;
  setVariable::CornerPipeGenerator CPipeGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;

  setVariable::EPSeparatorGenerator ESGen;
  setVariable::R3ChokeChamberGenerator CCGen;

  const std::string frontKey(beamlineKey+"FrontBeam");
  // So that we can assume it's the straight section centre [2, page 1]
  // 160 is the REW thickness
  Control.addVariable(frontKey+"YStep",-2110.0-160.0+20);
  Control.addVariable(frontKey+"XStep",0.0);
  Control.addVariable(frontKey+"OuterRadius",60.0);

  // BuildZone offset
  Control.addVariable(frontKey+"FrontOffset",-450.0);

  PipeGen.setCF<CF40>();
  PipeGen.setNoWindow();
  PipeGen.setMat("Copper");
  // placeholder length (100.0)
  PipeGen.generatePipe(Control,frontKey+"PrePipe",197.4); // to make straight section length 6.81 m
  PipeGen.generatePipe(Control,frontKey+"TransPipe",100.0);

  setVariable::MagnetM1Generator M1Gen;
  M1Gen.generateBlock(Control,frontKey+"M1Block");

  setVariable::MagnetU1Generator U1Gen;
  U1Gen.generateBlock(Control,frontKey+"U1Block");

  name=frontKey+"EPSeparator";
  ESGen.generatePipe(Control,name);
  Control.addVariable(name+"Length",63.33); // to place Crotch absorber at 5.3243 m (Nils)
  Control.addVariable(name+"WallWidth",6.25+1); // added 1 to avoid geometric error

  CCGen.generateChamber(Control,frontKey+"ChokeChamber");
  CCGen.generateInsert(Control,frontKey+"ChokeInsert");
  Control.addVariable(frontKey+"ChokeInsertPlateGap",0.9); // [3]

  CPipeGen.setCF<CF40>();
  CPipeGen.setAFlangeCF<CF25>();
  CPipeGen.setMat("Aluminium","Stainless304");
  CPipeGen.generatePipe(Control,frontKey+"DipolePipe",800.0);

  PipeGen.setCF<CF25>();
  PipeGen.setMat("Stainless304");
  PipeGen.generatePipe(Control,frontKey+"ETransPipe",800.0);

  //  Note bellow reversed for FM fixed:
  BellowGen.setCF<setVariable::CF40>();
  BellowGen.setAFlangeCF<setVariable::CF100>();
  BellowGen.generateBellow(Control,frontKey+"BellowA",16.0);

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.setAFlangeCF<setVariable::CF100>();
  BellowGen.generateBellow(Control,frontKey+"BellowB",16.0);

  PipeGen.setCF<CF40>();
  PipeGen.setMat("Stainless304");
  PipeGen.generatePipe(Control,frontKey+"CollABPipe",222.0);

  setVariable::CleaningMagnetGenerator CLGen;
  CLGen.generate(Control,frontKey+"PermanentMagnet");
  Control.addVariable(frontKey+"PermanentMagnetYStep",64.7); // [3]
  Control.addVariable(frontKey+"PermanentMagnetYokeLength",31.4); // should be the same length as the magnet, but making it 1 mm shorter to avoid zero volume cells
  Control.addVariable(frontKey+"PermanentMagnetGap",5.2); // [3]

  Control.addVariable(frontKey+"ECutDiskYStep",5.0);
  Control.addVariable(frontKey+"ECutDiskLength",0.1);
  Control.addVariable(frontKey+"ECutDiskRadius",1.0);
  Control.addVariable(frontKey+"ECutDiskMat","H2Gas#0.1");

  Control.addVariable(frontKey+"ECutMagDiskYStep",2.0);
  Control.addVariable(frontKey+"ECutMagDiskLength",0.1);
  // note: CF40::innerRadius is some complex template type
  Control.addVariable
    (frontKey+"ECutMagDiskRadius",static_cast<double>(CF25::innerRadius));
  Control.addVariable(frontKey+"ECutMagDiskMat","H2Gas#0.1");

  // note : reversed becaues using fixed FM
  BellowGen.setCF<setVariable::CF40>();
  BellowGen.setAFlangeCF<setVariable::CF100>();
  BellowGen.generateBellow(Control,frontKey+"BellowC",16.0);

  PipeGen.setCF<setVariable::CF40>();
  PipeGen.setAFlangeCF<setVariable::CF100>();
  PipeGen.generatePipe(Control,frontKey+"CollExitPipe",165.5);

  // Create HEAT DUMP
  heatDumpTableToyama(Control,frontKey);
  moveApertureTableToyama(Control,frontKey);
  shutterTableToyama(Control,frontKey);

  wallVariables(Control,beamlineKey+"WallLead");

  return;
}

}  // NAMESPACE setVariable
