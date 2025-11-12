/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   danmax/danmaxVariables.cxx
 *
 * Copyright (c) 2004-2025 by Stuart Ansell / Konstantin Batkov
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
#include <memory>
#include <array>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "maxivVariables.h"

#include "CFFlanges.h"
#include "PipeGenerator.h"
#include "BellowGenerator.h"
#include "GateValveGenerator.h"
#include "JawValveGenerator.h"
#include "PipeTubeGenerator.h"
#include "PortTubeGenerator.h"
#include "PortItemGenerator.h"
#include "DoublePortItemGenerator.h"
#include "VacBoxGenerator.h"
#include "MonoBoxGenerator.h"
#include "FlangeMountGenerator.h"
#include "BeamPairGenerator.h"
#include "MonoShutterGenerator.h"
#include "CollGenerator.h"
#include "SqrFMaskGenerator.h"
#include "PortChicaneGenerator.h"
#include "TriggerGenerator.h"
#include "CylGateValveGenerator.h"
#include "DCMTankGenerator.h"
#include "MonoBlockXstalsGenerator.h"
#include "MLMonoGenerator.h"
#include "BremBlockGenerator.h"
#include "OpticsHutGenerator.h"
#include "ExptHutGenerator.h"
#include "MovableSafetyMaskGenerator.h"
#include "HeatAbsorberToyamaGenerator.h"

// [4] see [4] in R3RingVariablesToyama.cxx

// References
// [1] CARATELLI Drawing 06769-01-000
// [2] CARATELLI Drawing 06769-03-000

namespace setVariable
{

namespace danmaxVar
{

void undulatorVariables(FuncDataBase&,const std::string&);
void frontMaskVariables(FuncDataBase&,const std::string&);
void monoShutterVariables(FuncDataBase&,const std::string&);
void connectVariables(FuncDataBase&,const std::string&);
void opticsHutVariables(FuncDataBase&,const std::string&);
void exptHutVariables(FuncDataBase&,const std::string&);

void lensPackage(FuncDataBase&,const std::string&);
void mirrorMonoPackage(FuncDataBase&,const std::string&);
void monoPackage(FuncDataBase&,const std::string&);
void viewPackage(FuncDataBase&,const std::string&);
void viewBPackage(FuncDataBase&,const std::string&);
void beamStopPackage(FuncDataBase&,const std::string&);
void revBeamStopPackage(FuncDataBase&,const std::string&);

void
undulatorVariables(FuncDataBase& Control,
		   const std::string& frontKey)
  /*!
    Builds the variables for the undulator
    \param Control :: Database
    \param frontKey :: prename
  */
{
  ELog::RegMethod RegA("danmaxVariables[F]","undulatorVariables");
  setVariable::PipeGenerator PipeGen;

  const double undulatorLen(300.0);
  PipeGen.setMat("Stainless304");
  PipeGen.setNoWindow();   // no window
  PipeGen.setCF<setVariable::CF63>();
  PipeGen.generatePipe(Control,frontKey+"UPipe",undulatorLen);
  Control.addVariable(frontKey+"UPipeYStep",-undulatorLen/2.0);

  Control.addVariable(frontKey+"UPipeWidth",6.0);
  Control.addVariable(frontKey+"UPipeHeight",0.6);
  Control.addVariable(frontKey+"UPipePipeThick",0.2);

  // undulator
  Control.addVariable(frontKey+"UndulatorVGap",1.1);  // mininum 11mm
  Control.addVariable(frontKey+"UndulatorLength",270.0);   // 46.2mm*30*2
  Control.addVariable(frontKey+"UndulatorMagnetWidth",6.0);
  Control.addVariable(frontKey+"UndulatorMagnetDepth",3.0);
  Control.addVariable(frontKey+"UndulatorSupportWidth",12.0);
  Control.addVariable(frontKey+"UndulatorSupportThick",8.0);
  Control.addVariable(frontKey+"UndulatorSupportLength",4.0);  // extra
  Control.addVariable(frontKey+"UndulatorSupportVOffset",2.0);
  Control.addVariable(frontKey+"UndulatorStandWidth",6.0);
  Control.addVariable(frontKey+"UndulatorStandHeight",8.0);
  Control.addVariable(frontKey+"UndulatorVoidMat","Void");
  Control.addVariable(frontKey+"UndulatorMagnetMat","NbFeB");
  Control.addVariable(frontKey+"UndulatorSupportMat","Copper");
  Control.addVariable(frontKey+"UndulatorStandMat","Aluminium");

  Control.addVariable(frontKey+"UndulatorFlipX",1);

  return;
}

void
frontMaskVariables(FuncDataBase& Control,
		   const std::string& preName)
  /*!
    Variable for the front maste
    \param Control :: Database
    \param preName :: Beamline name
  */
{
  ELog::RegMethod RegA("danmaxVariables[F]","frontMaskVariables");

  setVariable::SqrFMaskGenerator FMaskGen;
  setVariable::BellowGenerator BellowGen;

  Control.addVariable(preName+"BellowALength",10.0); // [4]


  constexpr double FM1Length(40.0); // [4]
  constexpr double FM2Length(50.5); // TODO: not indicated in [4]
  ELog::EM << "FM2 length ??? " << ELog::endWarn;
  constexpr double MSMLength(40.0); //

  const double FM1dist(1104.75+FM1Length/2.0); // [4]
  const double FM2dist(1597.08+FM2Length/2.0); //
  //  const double FM2dist(1597.08+FM2Length/2.0); // [4]
  const double MSMdist(1600.0); //

  FMaskGen.setCF<CF100>();
  FMaskGen.setFrontGap(2.53, 2.53); //
  double backWidth = 1.19; //
  double backHeight = backWidth;
  FMaskGen.setBackGap(backWidth, backHeight);
  FMaskGen.setMinSize(FM1Length-CF100::flangeLength-Geometry::zeroTol,
		      backWidth, backHeight);
  FMaskGen.generateColl(Control,preName+"FM1",FM1dist,FM1Length);

  Control.addVariable(preName+"BellowBLength",10.0); // [4]
  Control.addVariable(preName+"BellowCLength",10.0); // [4]

  FMaskGen.setFrontGap(1.65, 1.65); //
  backWidth = 1.54; //
  backHeight = 0.154; //
  FMaskGen.setBackGap(backWidth, backHeight);
  FMaskGen.setMinSize(FM2Length-CF100::flangeLength-Geometry::zeroTol,
		      backWidth, backHeight);
  FMaskGen.generateColl(Control,preName+"FM2",FM2dist,FM2Length);

  BellowGen.generateBellow(Control,preName+"BellowCA",10.0); // [4]

  HeatAbsorberToyamaGenerator HAGen;

  constexpr double heatAbsorberLength = 26.5;  // [4]
  constexpr double heatAbsorberDist(1673.3); // [4]

  HAGen.generate(Control,preName+"HeatAbsorber",heatAbsorberLength);
  Control.addVariable(preName+"HeatAbsorberYStep",heatAbsorberDist);


  // NOT PRESENT :::
  // FMaskGen.setFrontGap(0.84,0.582);
  // FMaskGen.setBackGap(0.750,0.357);

  // FMaskGen.setMinAngleSize(12.0,1600.0, 100.0, 100.0);
  // FMaskGen.generateColl(Control,preName+"CollC",17/2.0,17.0);

  // approx for 100uRad x 100uRad
  FMaskGen.setFrontAngleSize(FM2dist,150.0,150.0);
  FMaskGen.setMinAngleSize(12.0,FM2dist, 100.0, 100.0 );
  FMaskGen.setBackAngleSize(FM2dist, 160.0,160.0 );
  FMaskGen.generateColl(Control,preName+"CollC",17.0/2.0,17.0);

  setVariable::PipeGenerator PipeGen;
  PipeGen.setNoWindow();
  PipeGen.setCF<setVariable::CF40>(); // dummy
  PipeGen.generatePipe(Control,preName+"MSMEntrancePipe",5.0); // dummy


  // Movable Safety Mask
  BellowGen.setMat("Stainless304", "Stainless304");
  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,preName+"BellowPreMSM",14.0); // guess

  MovableSafetyMaskGenerator MSMGen;
  MSMGen.generate(Control,preName+"MSM",MSMLength, "undulator"); //
  Control.addVariable(preName+"MSMYStep",MSMdist);

  BellowGen.generateBellow(Control,preName+"BellowPostMSM",14.0); // guess

  PipeGen.setMat("Stainless304L"); // dummy
  PipeGen.generatePipe(Control,preName+"MSMExitPipe",100.0); // dummy

  return;
}


void
opticsHutVariables(FuncDataBase& Control,
		   const std::string& hutName)
  /*!
    Optics hut variables
    \param Control :: DataBase to add
    \param hutName :: Optics hut name
    \param xOffset  :: Wall step as beam line moved
  */
{
  ELog::RegMethod RegA("danmaxVariables","opticsHutVariables");

  const double beamMirrorShift(-0.6);

  OpticsHutGenerator OGen;

  OGen.setSkin(0.2);
  OGen.setBackLead(5.0); // "Lead Thickness Back Wall" Section A-A [1]
  OGen.setWallLead(1.2); // "Lead Thickness Side Wall", Section A-A [1]
  OGen.setRoofLead(1.2); // "Roof Lead Thickness", top view [1]
  OGen.addHole(Geometry::Vec3D(beamMirrorShift,0,0),3.5);
  const double opticsHutLength = 1010.0; // Section A-A in [1]
  OGen.generateHut(Control,hutName, opticsHutLength); 
  const double opticsHutOuterWidth = 259.7;
  Control.addVariable(hutName+"OutWidth", opticsHutOuterWidth); // Section A-A [1]
  Control.addVariable(hutName+"Height", 411.0-131.88); // Optics hutch height (Section B-B) - optical-axis height (back view) [1]

  Control.addVariable(hutName+"RingStepLength", opticsHutLength-155.2); // Section A-A [1]
  Control.addVariable(hutName+"RingStepWidth",566.7-opticsHutOuterWidth-120.2); // Section A-A [1]

  Control.addVariable(hutName+"BackPlateThick", 7.0); // "Lead Thickness", back view [1]
  const double backPlateSideLength = 200.0; // back view [1]
  Control.addVariable(hutName+"BackPlateWidth", backPlateSideLength); 
  Control.addVariable(hutName+"BackPlateHeight", backPlateSideLength);

  Control.addVariable(hutName+"NChicane",2);
  PortChicaneGenerator PGen;
  PGen.setSize(8.0,80.0,45.0);
  PGen.generatePortChicane(Control,hutName+"Chicane0",320.0,-25.0);
  PGen.generatePortChicane(Control,hutName+"Chicane1",-350.0,-25.0);

  return;
}

void
connectVariables(FuncDataBase& Control,
		 const std::string& beamName)
  /*!
    Optics hut variables
    \param Control :: DataBase to add
    \param beamName :: beamline name
  */
{
  ELog::RegMethod RegA("danmaxVariables[F]","connectVariables");

  Control.addVariable(beamName+"OuterLeft",70.0);
  Control.addVariable(beamName+"OuterRight",60.0);
  Control.addVariable(beamName+"OuterTop",70.0);

  const Geometry::Vec3D OPos(0,0,0);
  const Geometry::Vec3D ZVec(0,0,-1);

  setVariable::BellowGenerator BellowGen;
  setVariable::PipeGenerator PipeGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::PortItemGenerator PItemGen;

  PipeGen.setMat("Stainless304");
  PipeGen.setCF<setVariable::CF40>();
  PipeGen.setNoWindow();

  const std::string connectName(beamName+"ConnectShield");
  Control.addVariable(connectName+"Height",50.0);
  Control.addVariable(connectName+"Width",70.0);
  Control.addVariable(connectName+"Length",858.4);
  Control.addVariable(connectName+"Thick",0.5);
  Control.addVariable(connectName+"SkinThick",0.1);

  Control.addVariable(connectName+"SkinMat","Stainless304");
  Control.addVariable(connectName+"Mat","Lead");
  Control.addVariable(connectName+"VoidMat","Void");

  PipeGen.generatePipe(Control,beamName+"PipeA",425.0);

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,beamName+"BellowA",16.0);

  PipeGen.setBFlangeCF<setVariable::CF100>();
  PipeGen.generatePipe(Control,beamName+"FlangeA",5.0);

  SimpleTubeGen.setMat("Stainless304");
  SimpleTubeGen.setCF<CF100>();
  // ystep/length
  SimpleTubeGen.generateTube(Control,beamName+"IonPumpA",10.0);
  Control.addVariable(beamName+"IonPumpANPorts",1);
  PItemGen.setCF<setVariable::CF40>(10.0+CF100::outerRadius);
  PItemGen.setOuterVoid(0);
  PItemGen.setPlate(0.0,"Void");
  PItemGen.generatePort(Control,beamName+"IonPumpAPort0",OPos,ZVec);

  PipeGen.setCF<setVariable::CF40>();
  PipeGen.setAFlangeCF<setVariable::CF100>();
  PipeGen.generatePipe(Control,beamName+"FlangeB",5.0);

  BellowGen.generateBellow(Control,beamName+"BellowB",16.0);

  PipeGen.setCF<setVariable::CF40>();
  PipeGen.generatePipe(Control,beamName+"PipeB",325.0);

  return;
}

void
exptHutVariables(FuncDataBase& Control,
		 const std::string& beamName)
  /*!
    Optics hut variables
    \param Control :: DataBase to add
    \param beamName :: Beamline name
  */
{
  ELog::RegMethod RegA("danmaxVariables[F]","exptHutVariables");

  setVariable::ExptHutGenerator EGen;


  const double beamMirrorShift(0.6);
  const std::string hutName(beamName+"ExptHut");

  EGen.setFrontHole(beamMirrorShift,0.0,4.0);
  EGen.setCorner(atan(167.4/281.5)*180.0/M_PI,281.5); // Section A-A [2]
  EGen.setFrontLead(0.4); // "Lead Thickness Upstream Wall", Section A-A [2]
  EGen.setBackLead(0.6); // "Lead Thickness Downstream Wall", Section A-A [2]
  EGen.setRoofLead(0.4); // "Roof Thk Pb", Section A-A [2]
  EGen.setWallLead(0.4); // "Lead Thickness Side Wall", Section A-A [2]

  EGen.generateHut(Control,hutName,1845.0, 1401.3); // Hutch length: Section A-A [2]
  Control.addVariable(hutName+"RingWidth",204.8); // Section A-A [2]
  Control.addVariable(hutName+"OutWidth",260.2); // Section A-A [2]
  Control.addVariable(hutName+"Height",375.0-130.0); // Hutch height (Coupe B-B) - optical-axis height (front view) [2]

  // // lead shield on pipe
  // Control.addVariable(beamName+"PShieldXStep",beamMirrorShift);
  // Control.addVariable(beamName+"PShieldYStep",1.0);
  // Control.addVariable(beamName+"PShieldLength",1.0);
  // Control.addVariable(beamName+"PShieldWidth",10.0);
  // Control.addVariable(beamName+"PShieldHeight",10.0);
  // Control.addVariable(beamName+"PShieldWallThick",0.2);
  // Control.addVariable(beamName+"PShieldClearGap",0.3);
  // Control.addVariable(beamName+"PShieldWallMat","Stainless304");
  // Control.addVariable(beamName+"PShieldMat","Lead");

  Control.addVariable(hutName+"NChicane",2);
  PortChicaneGenerator PGen;
  PGen.setSize(4.0,40.0,30.0);
  PGen.generatePortChicane(Control,hutName+"Chicane0","Left",150.0,-5.0);
  PGen.generatePortChicane(Control,hutName+"Chicane1","Left",-270.0,-5.0);

  return;
}

void
viewPackage(FuncDataBase& Control,const std::string& viewKey)
  /*!
    Builds the variables for the ViewTube
    \param Control :: Database
    \param viewKey :: prename including view
  */
{
  ELog::RegMethod RegA("speciesVariables[F]","viewPackage");

  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::BellowGenerator BellowGen;
  setVariable::GateValveGenerator GateGen;
  setVariable::FlangeMountGenerator FlangeGen;

  // will be rotated vertical
  const std::string pipeName=viewKey+"ViewTube";
  SimpleTubeGen.setCF<CF100>();
  SimpleTubeGen.setCap();
  // up 15cm / 32.5cm down : Measured
  SimpleTubeGen.generateTube(Control,pipeName,47.5);


  Control.addVariable(pipeName+"NPorts",3);   // beam ports (lots!!)

  PItemGen.setCF<setVariable::CF40>(CF100::outerRadius+5.0);
  PItemGen.setPlate(0.0,"Void");
  PItemGen.generatePort(Control,pipeName+"Port0",
			Geometry::Vec3D(0,8.75,0),
			Geometry::Vec3D(0,0,1));
  PItemGen.generatePort(Control,pipeName+"Port1",
			Geometry::Vec3D(0,8.75,0),
			Geometry::Vec3D(0,0,-1));
  PItemGen.setPlate(0.0,"Stainless304");
  PItemGen.setCF<setVariable::CF40>(sqrt(2.0)*CF100::outerRadius+8.0);
  PItemGen.generatePort(Control,pipeName+"Port2",
			Geometry::Vec3D(0,8.75,0),
			Geometry::Vec3D(-1,0,-1));

  FlangeGen.setNoPlate();
  FlangeGen.setBlade(2.0,2.0,0.3,-45.0,"Graphite",1);
  FlangeGen.generateMount(Control,viewKey+"ViewTubeScreen",1);  // in beam

  return;
}

void
lensPackage(FuncDataBase& Control,const std::string& lensKey)
  /*!
    Builds the variables for the ViewTube
    \param Control :: Database
    \param lensKey :: prename including view
  */
{
  ELog::RegMethod RegA("danmaxVariables[F]","lensPackage");

  setVariable::MonoBoxGenerator MBoxGen;
  setVariable::PortItemGenerator PItemGen;

  const std::string lensName=lensKey+"LensBox";

  MBoxGen.setCF<CF40>();
  //W/H/D/L
  MBoxGen.generateBox(Control,lensName,20.0,12.5,8.0,48.0);


  const size_t NPorts(6);
  Control.addVariable(lensName+"NPorts",NPorts);   // beam ports (lots!!)

  PItemGen.setCF<setVariable::CF40>(0.5);
  const Geometry::Vec3D Z(0,0,1);
  const Geometry::Vec3D YStep(0,48.0/6.5,0);
  Geometry::Vec3D Pt(-YStep*2.5);
  for(size_t i=0;i<NPorts;i++)
    {
      const std::string portName("Port"+std::to_string(i));
      PItemGen.generatePort(Control,lensName+portName,Pt,Z);
      Pt+=YStep;
    }
  return;
}

void
viewBPackage(FuncDataBase& Control,const std::string& viewKey)
  /*!
    Builds the variables for the ViewTube
    \param Control :: Database
    \param viewKey :: prename including view
  */
{
  ELog::RegMethod RegA("danmaxVariables[F]","viewBPackage");

  setVariable::PortTubeGenerator PTubeGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::FlangeMountGenerator FlangeGen;

  const std::string pipeName=viewKey+"ViewTubeB";
  PTubeGen.setMat("Stainless304");
  PTubeGen.setPipeCF<CF150>();
  PTubeGen.setPortCF<CF40>();
  PTubeGen.setPortLength(3.0,3.0);
  // ystep/width/height/depth/length
  PTubeGen.generateTube(Control,pipeName,0.0,30.0);


  // will be rotated vertical

  Control.addVariable(pipeName+"NPorts",3);   // beam ports (lots!!)

  PItemGen.setCF<setVariable::CF100>(CF150::outerRadius+5.0);
  PItemGen.generatePort(Control,pipeName+"Port0",
			Geometry::Vec3D(0,-9,0),
			Geometry::Vec3D(0,0,1));
  PItemGen.setCF<setVariable::CF100>(CF150::outerRadius+7.0);
  PItemGen.generatePort(Control,pipeName+"Port1",
			Geometry::Vec3D(0,9,0),
			Geometry::Vec3D(0,0,1));
  PItemGen.setCF<setVariable::CF40>(sqrt(2.0)*CF150::outerRadius+5.0);
  PItemGen.generatePort(Control,pipeName+"Port2",
			Geometry::Vec3D(0,4.5,0),
			Geometry::Vec3D(-1,-1,0));

  FlangeGen.setNoPlate();
  FlangeGen.setBlade(2.0,2.0,0.3,-45.0,"Graphite",1);
  FlangeGen.generateMount(Control,pipeName+"Screen",1);  // in beam

  return;
}

void
beamStopPackage(FuncDataBase& Control,const std::string& viewKey)
  /*!
    Builds the variables for the ViewTube 2
    \param Control :: Database
    \param viewKey :: prename
  */
{
  ELog::RegMethod RegA("speciesVariables[F]","beamStopPackage");

  setVariable::PipeGenerator PipeGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::DoublePortItemGenerator DItemGen;
  setVariable::BremBlockGenerator BremGen;
  setVariable::JawValveGenerator JawGen;

  // will be rotated vertical
  const std::string pipeName=viewKey+"BeamStopTube";

  SimpleTubeGen.setCF<CF150>();
  SimpleTubeGen.setCap(1,1);
  // up 16cm / 37.5cm down : Measured +1cm up for clearance
  SimpleTubeGen.generateTube(Control,pipeName,53.5);

  Control.addVariable(pipeName+"NPorts",2);   // beam ports (lots!!)

  // BOTH PORTS COMPLETLEY NON-STANDARD:
  // Ports 11cm + 15(inner)cm + 10cm   ==> 36.0
  DItemGen.setDCF<CF63,CF40>(6.5,4.0);
  DItemGen.setPlate(0.0,"Void");

  DItemGen.generatePort(Control,pipeName+"Port0",
			Geometry::Vec3D(0,10.75,0),  // 53.5/2-16.0
			Geometry::Vec3D(0,0,1));

  PItemGen.setCF<setVariable::CF150>(12.5);  // needs to be CF75
  PItemGen.setPlate(0.0,"Void");
  PItemGen.generatePort(Control,pipeName+"Port1",
			Geometry::Vec3D(0,10.75,0),
			Geometry::Vec3D(0,0,-1));

  BremGen.centre();
  BremGen.setCube(10.0,10.0);
  BremGen.setAperature(5.0, 0.4,0.4, 0.4,0.4, 0.4,0.4);  // WRONG
  BremGen.generateBlock(Control,viewKey+"BeamStop",10.75);
  Control.addVariable(viewKey+"BeamStopXAngle",90);

   // Single slit pair
  JawGen.setRadius(8.0);
  JawGen.setWallThick(2.0);
  JawGen.setLength(10.0);
  JawGen.setSlits(3.0,2.0,0.2,"Tantalum");
  JawGen.generateSlits(Control,viewKey+"SlitsA",0.0,0.8,0.8);

  PipeGen.setMat("Stainless304");
  PipeGen.setNoWindow();
  PipeGen.setCF<setVariable::CF40>();
  PipeGen.setAFlangeCF<setVariable::CF150>();
  PipeGen.generatePipe(Control,viewKey+"SlitsAOut",4.0);

  return;
}

void
revBeamStopPackage(FuncDataBase& Control,
		   const std::string& viewKey)
  /*!
    Builds the variables for the reversed slit tube/beamstop
    \param Control :: Database
    \param viewKey :: prename
  */
{
  ELog::RegMethod RegA("danmaxVariables[F]","revBeamStopPackage");

  setVariable::PipeGenerator PipeGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::DoublePortItemGenerator DItemGen;
  setVariable::BremBlockGenerator BremGen;
  setVariable::JawValveGenerator JawGen;

  // will be rotated vertical
  const std::string pipeName=viewKey+"RevBeamStopTube";

  SimpleTubeGen.setCF<CF150>();
  SimpleTubeGen.setCap(1,1);
  // up 15cm / 38.5cm down : Measured +1up clearance
  SimpleTubeGen.generateTube(Control,pipeName,53.5);
  Control.addVariable(pipeName+"YAngle",180.0);  // rotate
  Control.addVariable(pipeName+"NPorts",2);   // beam ports (lots!!)

  // BOTH PORTS COMPLETLEY NON-STANDARD:
  // Ports 11cm + 15(inner)cm + 10cm   ==> 36.0
  DItemGen.setDCF<CF63,CF40>(6.5,4.0);
  DItemGen.setPlate(0.0,"Void");

  DItemGen.generatePort(Control,pipeName+"Port0",
			Geometry::Vec3D(0,10.75,0),  // 53.5/2-15.0
			Geometry::Vec3D(0,0,1));

  PItemGen.setCF<setVariable::CF150>(12.5);  // needs to be CF75
  PItemGen.setPlate(0.0,"Void");
  PItemGen.generatePort(Control,pipeName+"Port1",
			Geometry::Vec3D(0,10.75,0),
			Geometry::Vec3D(0,0,-1));

  BremGen.centre();
  BremGen.setCube(10.0,10.0);
  BremGen.setAperature(5.0, 0.4,0.4, 0.4,0.4, 0.4,0.4);  // WRONG
  BremGen.generateBlock(Control,viewKey+"RevBeamStop",10.75);
  Control.addVariable(viewKey+"RevBeamStopXAngle",90);

   // Single slit pair
  JawGen.setRadius(8.0);
  JawGen.setWallThick(2.0);
  JawGen.setLength(10.0);
  JawGen.setSlits(3.0,2.0,0.2,"Tantalum");
  JawGen.generateSlits(Control,viewKey+"SlitsB",0.0,0.8,0.8);

  PipeGen.setMat("Stainless304");
  PipeGen.setNoWindow();
  PipeGen.setCF<setVariable::CF40>();
  PipeGen.setBFlangeCF<setVariable::CF150>();
  PipeGen.generatePipe(Control,viewKey+"SlitsBOut",4.0);

  return;
}

void
monoPackage(FuncDataBase& Control,const std::string& monoKey)
  /*!
    Builds the variables for the mono packge
    \param Control :: Database
    \param slitKey :: prename
  */
{
  ELog::RegMethod RegA("danmaxVariables[F]","monoPackage");

  setVariable::PortItemGenerator PItemGen;
  setVariable::DCMTankGenerator MBoxGen;
  setVariable::MonoBlockXstalsGenerator MXtalGen;

  // ystep/width/height/depth/length
  //
  MBoxGen.setCF<CF40>();   // set ports
  MBoxGen.setPortLength(7.5,7.5); // La/Lb
  MBoxGen.setBPortOffset(-0.6,0.0);    // note -1mm from crystal offset
  // radius : Heigh / depth  [need heigh = 0]
  MBoxGen.generateBox(Control,monoKey+"MonoVessel",30.0,0.0,16.0);

  //  Control.addVariable(monoKey+"MonoVesselPortAZStep",-7);   //
  //  Control.addVariable(monoKey+"MonoVesselFlangeAZStep",-7);     //
  //  Control.addVariable(monoKey+"MonoVesselFlangeBZStep",-7);     //
  Control.addVariable(monoKey+"MonoVesselPortBXStep",-0.6);      // from primary

  const std::string portName=monoKey+"MonoVessel";
  Control.addVariable(monoKey+"MonoVesselNPorts",0);   // beam ports (lots!!)
  PItemGen.setCF<setVariable::CF63>(5.0+31.2);
  PItemGen.setWindowPlate(2.5,2.0,-0.8,"Stainless304","LeadGlass");
  PItemGen.generatePort(Control,portName+"Port0",
  			Geometry::Vec3D(0,5.0,-10.0),
  			Geometry::Vec3D(1,0,0));

  // crystals gap 7mm
  MXtalGen.generateXstal(Control,monoKey+"MBXstals",0.0,3.0);


  return;
}

void
mirrorMonoPackage(FuncDataBase& Control,const std::string& monoKey)
  /*!
    Builds the variables for the mirror mono package (MLM)
    \param Control :: Database
    \param monoKey :: prename
  */
{
  ELog::RegMethod RegA("danmaxVariables[F]","mirrorMonoPackage");

  setVariable::PortItemGenerator PItemGen;
  setVariable::VacBoxGenerator MBoxGen;
  setVariable::MLMonoGenerator MXtalGen;

  // ystep/width/height/depth/length
  //
  MBoxGen.setCF<CF40>();   // set ports
  MBoxGen.setAllThick(1.5,2.5,1.0,1.0,1.0); // Roof/Base/Width/Front/Back
  MBoxGen.setPortLength(7.5,7.5); // La/Lb
  MBoxGen.setBPortOffset(-0.4,0.0);    // note -1mm from crystal offset
  // width / heigh / depth / length
  MBoxGen.generateBox
    (Control,monoKey+"MLMVessel",57.0,12.5,31.0,109.0);

  Control.addVariable(monoKey+"MLMVesselPortBXStep",0.0);   // from primary

  const std::string portName=monoKey+"MLMVessel";
  Control.addVariable(monoKey+"MLMVesselNPorts",0);   // beam ports (lots!!)
  PItemGen.setCF<setVariable::CF63>(5.0);
  PItemGen.setPlate(4.0,"LeadGlass");
  PItemGen.generatePort(Control,portName+"Port0",
			Geometry::Vec3D(0,5.0,-10.0),
			Geometry::Vec3D(1,0,0));

  // crystals gap 4mm
  MXtalGen.generateMono(Control,monoKey+"MLM",-10.0,0.3,0.3);

  return;
}

void
monoShutterVariables(FuncDataBase& Control,
		     const std::string& preName)
  /*!
    Construct Mono Shutter variables
    \param Control :: Database for variables
    \param preName :: Control ssytem
   */
{
  ELog::RegMethod RegA("danmaxVariables","monoShutterVariables");

  setVariable::PipeGenerator PipeGen;
  setVariable::BellowGenerator BellowGen;
  setVariable::MonoShutterGenerator MShutterGen;

  // up / up (true)
  MShutterGen.generateShutter(Control,preName+"MonoShutter",1,1);

  PipeGen.setMat("Stainless304");
  PipeGen.setNoWindow();
  PipeGen.setCF<setVariable::CF40>();
  PipeGen.setBFlangeCF<setVariable::CF63>();
  PipeGen.generatePipe(Control,preName+"MonoAdaptorA",4.0);
  PipeGen.setAFlangeCF<setVariable::CF63>();
  PipeGen.setBFlangeCF<setVariable::CF40>();
  PipeGen.generatePipe(Control,preName+"MonoAdaptorB",4.0);


  // bellows on shield block
  BellowGen.setCF<setVariable::CF40>();
  BellowGen.setAFlangeCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,preName+"BellowL",10.0);


  return;
}

template<typename JoinPipeCCF>
void
shieldVariables(FuncDataBase& Control)
  /*!
    Shield variables
    \param Control :: DataBase to add
  */
{
  ELog::RegMethod RegA("danmaxVariables","shieldVariables");

  const std::string preName("DanMAX");

  Control.addVariable(preName+"GuillotineLength",0.6); // Coupe C-C [2]
  Control.addVariable(preName+"GuillotineWidth",40.0); // Coupe C-C [2]
  Control.addVariable(preName+"GuillotineHeight",40.0); // Coupe C-C [2]
  Control.addVariable(preName+"GuillotineWallThick",0.0); // Coupe C-C [2], no guillotine wall shown
  Control.addVariable(preName+"GuillotineClearGap",4.1-2.0*(JoinPipeCCF::innerRadius+JoinPipeCCF::wallThick)); // Coupe C-C [2]
  Control.addVariable(preName+"GuillotineWallMat","Stainless304");
  Control.addVariable(preName+"GuillotineMat","Lead");

  Control.addVariable(preName+"NShieldYStep",10.2);
  Control.addVariable(preName+"NShieldLength",7.0);
  Control.addVariable(preName+"NShieldWidth",80.0);
  Control.addVariable(preName+"NShieldHeight",80.0);
  Control.addVariable(preName+"NShieldWallThick",0.5);
  Control.addVariable(preName+"NShieldClearGap",0.2);
  Control.addVariable(preName+"NShieldWallMat","Stainless304");
  Control.addVariable(preName+"NShieldMat","Poly");

  Control.addVariable(preName+"OuterShieldYStep",10.2);
  Control.addVariable(preName+"OuterShieldLength",5.0);
  Control.addVariable(preName+"OuterShieldWidth",80.0);
  Control.addVariable(preName+"OuterShieldHeight",80.0);
  Control.addVariable(preName+"OuterShieldWallThick",0.5);
  Control.addVariable(preName+"OuterShieldClearGap",0.2);
  Control.addVariable(preName+"OuterShieldWallMat","Stainless304");
  Control.addVariable(preName+"OuterShieldMat","Poly");
  return;
}


void
opticsSlitPackage(FuncDataBase& Control,
		  const std::string& opticsName)
  /*!
    Builds the DM2 slit package
    \param Control :: Function data base for variables
    \param opticsName :: PreNaem
   */
{
  setVariable::PortTubeGenerator PortTubeGen;
  setVariable::JawValveGenerator JawGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::BeamPairGenerator BeamMGen;

    /// SLIT PACKAGE

  const std::string sName=opticsName+"SlitTube";
  const double tLen(48.2);
  PortTubeGen.setPipeCF<CF200>();

  PortTubeGen.setPortCF<CF40>();
  PortTubeGen.setPortLength(-5.0,-5.0);
  PortTubeGen.generateTube(Control,sName,0.0,tLen);

  Control.addVariable(sName+"NPorts",3);   // beam ports (lots!!)
  PItemGen.setCF<setVariable::CF150>(CF200::outerRadius+6.1);
  PItemGen.setPlate(setVariable::CF150::flangeLength,"Stainless304");

  // Top port 16.0: Side 20.0cm  from front :  Vacuum 1/2 way
  //
  const Geometry::Vec3D topJaw(0.0,16.0-tLen/2.0,0.0);
  const Geometry::Vec3D sideJaw(0.0,20.0-tLen/2.0,0.0);
  const Geometry::Vec3D vacPort(0.0,0.0,0.0);

  const Geometry::Vec3D XVec(1,0,0);
  const Geometry::Vec3D ZVec(0,0,1);

  PItemGen.setOuterVoid(1); ///
  PItemGen.generatePort(Control,sName+"Port0",topJaw,ZVec);
  PItemGen.generatePort(Control,sName+"Port1",sideJaw,XVec);
  PItemGen.generatePort(Control,sName+"Port2",vacPort,-ZVec);
  //  PItemGen.generatePort(Control,sName+"Port3",CPt,ZVec);

  // Jaw units:
  BeamMGen.setThread(0.5,"Nickel");
  BeamMGen.setLift(0.0,2.5);
  BeamMGen.setGap(1.0,1.0);
  BeamMGen.setXYStep(0.6,0.0,-0.6,0);
  BeamMGen.setBlock(4.0,2.0,1.0,0.0,"Tungsten");

  const std::string jawKey[]={"JawX","JawZ"};
  for(size_t i=0;i<2;i++)
    {
      const std::string fname=opticsName+jawKey[i];
      BeamMGen.generateMount(Control,fname,0);  // outer of beam
    }

  return;
}

void
opticsVariables(FuncDataBase& Control,
		const std::string& beamName)
  /*
    Vacuum optics components in the optics hutch
    \param Control :: Function data base
    \param beamName :: beamline name
  */
{
  ELog::RegMethod RegA("danmaxVariables[F]","opticsVariables");

  const std::string opticsName(beamName+"OpticsLine");

  Control.addVariable(opticsName+"OuterLeft",70.0);
  Control.addVariable(opticsName+"OuterRight",60.0);
  Control.addVariable(opticsName+"OuterTop",70.0);

  setVariable::PipeGenerator PipeGen;
  setVariable::BellowGenerator BellowGen;
  setVariable::BremBlockGenerator BremGen;
  setVariable::FlangeMountGenerator FlangeGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::TriggerGenerator TGen;
  setVariable::CylGateValveGenerator GVGen;
  setVariable::SqrFMaskGenerator FMaskGen;

  setVariable::GateValveGenerator GateGen;

  PipeGen.setNoWindow();   // no window
  PipeGen.setMat("Stainless304");
  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,opticsName+"InitBellow",6.0);

  // will be rotated vertical
  TGen.setCF<CF100>();
  TGen.setVertical(15.0,25.0);
  TGen.setSideCF<setVariable::CF40>(10.0); // add centre distance?
  TGen.generateTube(Control,opticsName+"TriggerUnit");

  GVGen.generateGate(Control,opticsName+"GateTubeA",0);  // open

  PipeGen.setCF<setVariable::CF40>();
  BellowGen.setCF<setVariable::CF40>();

  BellowGen.generateBellow(Control,opticsName+"BellowA",16.0);
  PipeGen.generatePipe(Control,opticsName+"PipeA",38.3);
  BellowGen.generateBellow(Control,opticsName+"BellowB",16.0);

  const double FM2dist(1624.2);
  FMaskGen.setCF<CF63>();
  FMaskGen.setFrontGap(2.13,2.146);
  FMaskGen.setBackGap(0.756,0.432);
  FMaskGen.setMinAngleSize(10.0,FM2dist, 100.0,100.0 );
  // step to +7.5 to make join with fixedComp:linkpt
  FMaskGen.generateColl(Control,opticsName+"BremCollA",7.5,15.0);

  // filter pipe [add filter later]
  PipeGen.generatePipe(Control,opticsName+"FilterPipe",3.0);

  GateGen.setCylCF<setVariable::CF40>();
  GateGen.setLength(1.1);
  GateGen.generateValve(Control,opticsName+"GateA",0.0,0);

  // laue monochromator
  BellowGen.generateBellow(Control,opticsName+"BellowC",8.0);
  PipeGen.generatePipe(Control,opticsName+"LauePipe",257.0);
  BellowGen.generateBellow(Control,opticsName+"BellowD",8.0);

  opticsSlitPackage(Control,opticsName);

  GateGen.generateValve(Control,opticsName+"GateB",0.0,0);
  BellowGen.generateBellow(Control,opticsName+"BellowE",16.0);

  monoPackage(Control,opticsName);

  GateGen.generateValve(Control,opticsName+"GateC",0.0,0);

  viewPackage(Control,opticsName);

  GateGen.setLength(3.1);
  GateGen.generateValve(Control,opticsName+"GateD",0.0,0);
  BellowGen.generateBellow(Control,opticsName+"BellowF",10.0);

  mirrorMonoPackage(Control,opticsName);
  BellowGen.generateBellow(Control,opticsName+"BellowG",16.0);

  GateGen.generateValve(Control,opticsName+"GateE",0.0,0);

  beamStopPackage(Control,opticsName);

  BellowGen.generateBellow(Control,opticsName+"BellowH",10.0);

  viewBPackage(Control,opticsName);

  BellowGen.generateBellow(Control,opticsName+"BellowI",10.0);

  lensPackage(Control,opticsName);

  GateGen.generateValve(Control,opticsName+"GateF",0.0,0);
  BellowGen.generateBellow(Control,opticsName+"BellowJ",10.0);

  revBeamStopPackage(Control,opticsName);

  BellowGen.generateBellow(Control,opticsName+"BellowK",10.0);
  Control.addVariable(opticsName+"BellowKYAngle",180.0);

  monoShutterVariables(Control,opticsName);
  GateGen.setBladeThick(0.3);
  GateGen.generateValve(Control,opticsName+"GateG",0.0,0);

  return;
}

}  // NAMESPACE danmaxVar

void
DANMAXvariables(FuncDataBase& Control)
  /*!
    Function to set the control variables and constants
    \param Control :: Function data base to add constants too
  */
{
  ELog::RegMethod RegA("danmaxVariables[F]","danmaxVariables");

  Control.addVariable("sdefType","Wiggler");

  setVariable::PipeGenerator PipeGen;
  //  setVariable::LeadPipeGenerator LeadPipeGen;

  PipeGen.setNoWindow();

  const std::string beamLineName("DanMAX");
  const std::string frontKey(beamLineName+"FrontBeam");

  danmaxVar::undulatorVariables(Control,frontKey);
  setVariable::R3FrontEndToyamaVariables(Control,beamLineName);
  //  Control.addVariable("DanmaxFrontBeamXStep",beamXStep);
  danmaxVar::frontMaskVariables(Control,frontKey);

  Control.addVariable(frontKey+"ProxiShieldAWallMat","Void"); // [AR 251104: checked by JR 251103]
  Control.addVariable(frontKey+"ProxiShieldBWallMat","Void"); // [AR 251104: checked by JR 251103]

  PipeGen.setMat("Stainless304");
  PipeGen.setCF<setVariable::CF40>();
  PipeGen.generatePipe(Control,beamLineName+"JoinPipe",190.0); // dummy

  danmaxVar::opticsHutVariables(Control,beamLineName+"OpticsHut");
  danmaxVar::opticsVariables(Control,beamLineName);

  PipeGen.setCF<setVariable::CF40>();
  PipeGen.generatePipe(Control,beamLineName+"JoinPipeB",49.3);

  danmaxVar::shieldVariables<setVariable::CF40>(Control);
  danmaxVar::connectVariables(Control,beamLineName+"ConnectUnit");

  PipeGen.setCF<setVariable::CF40>();
  PipeGen.setWindow(2.7, 0.005);
  PipeGen.setWindowMat("Diamond");
  PipeGen.generatePipe(Control,beamLineName+"JoinPipeC",54.0);

  danmaxVar::exptHutVariables(Control,beamLineName);

  const std::string exptName=beamLineName+"ExptLine";

  Control.addVariable(exptName+"BeamStopYStep",806.0);
  Control.addVariable(exptName+"BeamStopRadius",10.0);
  Control.addVariable(exptName+"BeamStopThick",5.0);
  Control.addVariable(exptName+"BeamStopMat","Stainless304");

  Control.addVariable(exptName+"SampleYStep",406.0);
  Control.addVariable(exptName+"SampleRadius",10.0);
  Control.addVariable(exptName+"SampleMat","Copper");

  return;
}

}  // NAMESPACE setVariable
