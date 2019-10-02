/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   danmax/danmaxVariables.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#include "maxivVariables.h"

#include "CFFlanges.h"
#include "PipeGenerator.h"
#include "SplitPipeGenerator.h"
#include "BellowGenerator.h"
#include "LeadPipeGenerator.h"
#include "CrossGenerator.h"
#include "GateValveGenerator.h"
#include "JawValveGenerator.h"
#include "PipeTubeGenerator.h"
#include "PortTubeGenerator.h"
#include "PortItemGenerator.h"
#include "VacBoxGenerator.h"
#include "FlangeMountGenerator.h"
#include "BeamMountGenerator.h"
#include "MirrorGenerator.h"
#include "MonoShutterGenerator.h"
#include "ShutterUnitGenerator.h"
#include "CollGenerator.h"
#include "PortChicaneGenerator.h"
#include "RingDoorGenerator.h"
#include "LeadBoxGenerator.h"
#include "PipeShieldGenerator.h"
#include "WallLeadGenerator.h"
#include "QuadUnitGenerator.h"
#include "DipoleChamberGenerator.h"

namespace setVariable
{

namespace danmaxVar
{

void undulatorVariables(FuncDataBase&,const std::string&);
void frontMaskVariables(FuncDataBase&,const std::string&);
void wallVariables(FuncDataBase&,const std::string&);
void monoShutterVariables(FuncDataBase&,const std::string&);
void exptHutVariables(FuncDataBase&,const std::string&);
  

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

  setVariable::PortTubeGenerator PTubeGen;
  
  PTubeGen.setMat("Stainless304");
  PTubeGen.setPipe(30.0,1.0);
  PTubeGen.setPortCF<CF63>();
  PTubeGen.setPortLength(15.0,15.0);

  // ystep/length ystep == -90cm
  PTubeGen.generateTube(Control,frontKey+"UndulatorTube",303.0,310.0);
  Control.addVariable(frontKey+"UndulatorTubeNPorts",0);
  
  // Undulator
  Control.addVariable(frontKey+"UndulatorLength",300.0);
  Control.addVariable(frontKey+"UndulatorBlockWidth",8.0);
  Control.addVariable(frontKey+"UndulatorBlockHeight",8.0);
  Control.addVariable(frontKey+"UndulatorBlockDepth",8.0);
  Control.addVariable(frontKey+"UndulatorBlockHGap",0.2);
  Control.addVariable(frontKey+"UndulatorBlockVGap",0.96);

  Control.addVariable(frontKey+"UndulatorBlockVCorner",1.0);
  Control.addVariable(frontKey+"UndulatorBlockHCorner",2.0);

  
  Control.addVariable(frontKey+"UndulatorVoidMat",0);
  Control.addVariable(frontKey+"UndulatorBlockMat","NbFeB");
    
  return;
}

void
wallVariables(FuncDataBase& Control,
	      const std::string& wallKey)
 /*!
    Set the variables for the frontend lead wall
    \param Control :: DataBase to use
    \param wallKey :: name before part names
  */
{
  ELog::RegMethod RegA("danmaxVariables[F]","wallVariables");
 
  WallLeadGenerator LGen;
  LGen.setWidth(70.0,140.0);
  LGen.generateWall(Control,wallKey,3.0);
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

  setVariable::CollGenerator CollGen;
    
  CollGen.setFrontGap(2.62,1.86);       // 1033.8
  CollGen.setBackGap(1.54,1.42);
  // Approximated to get 1.2mrad x 1.1mrad
  CollGen.setMinAngleSize(29.0,1033.0,1200.0,1100.0);  // Approximated to get 1mrad x 1mrad
  CollGen.generateColl(Control,preName+"CollA",0.0,34.0);

  CollGen.setFrontGap(2.13,2.146);
  CollGen.setBackGap(0.756,0.432);

  // approx for 800uRad x 200uRad  
  CollGen.setMinAngleSize(32.0,1600.0,800.0,200.0);
  CollGen.generateColl(Control,preName+"CollB",0.0,34.2);

  // FM 3:
  CollGen.setMain(1.20,"Copper","Void");
  CollGen.setFrontGap(0.84,0.582);
  CollGen.setBackGap(0.750,0.357);

  // approx for 400uRad x 100uRad
  CollGen.setMinAngleSize(12.0,1600.0, 400.0, 100.0);
  CollGen.generateColl(Control,preName+"CollC",0.0,17.0);

  return;
}


void
opticsHutVariables(FuncDataBase& Control,
		   const std::string& hutName)
  /*!
    Optics hut variables
    \param Control :: DataBase to add
    \param hutName :: Optics hut name
  */
{
  ELog::RegMethod RegA("danmaxVariables","opticsHutVariables");
  
  Control.addVariable(hutName+"Height",200.0);
  Control.addVariable(hutName+"Length",1034.6);
  Control.addVariable(hutName+"OutWidth",200.0);
  Control.addVariable(hutName+"RingWidth",110.0);
  Control.addVariable(hutName+"RingWallLen",105.0);
  Control.addVariable(hutName+"RingWallAngle",18.50);
  Control.addVariable(hutName+"RingConcThick",100.0);
  
  Control.addVariable(hutName+"InnerThick",0.2);
  
  Control.addVariable(hutName+"PbWallThick",2.0);
  Control.addVariable(hutName+"PbRoofThick",2.0);
  Control.addVariable(hutName+"PbFrontThick",2.0);
  Control.addVariable(hutName+"PbBackThick",10.0);

  Control.addVariable(hutName+"OuterThick",0.3);

  Control.addVariable(hutName+"InnerOutVoid",10.0);
  Control.addVariable(hutName+"OuterOutVoid",10.0);

  Control.addVariable(hutName+"SkinMat","Stainless304");
  Control.addVariable(hutName+"RingMat","Concrete");
  Control.addVariable(hutName+"PbMat","Lead");

  Control.addVariable(hutName+"HoleXStep",0.0);
  Control.addVariable(hutName+"HoleZStep",5.0);
  Control.addVariable(hutName+"HoleRadius",3.5);

  Control.addVariable(hutName+"InletXStep",0.0);
  Control.addVariable(hutName+"InletZStep",0.0);
  Control.addVariable(hutName+"InletRadius",5.0);


  Control.addVariable(hutName+"NChicane",4);
  PortChicaneGenerator PGen;
  PGen.generatePortChicane(Control,hutName+"Chicane0",470.0,-25.0);
  PGen.generatePortChicane(Control,hutName+"Chicane1",370.0,-25.0);
  PGen.generatePortChicane(Control,hutName+"Chicane2",-70.0,-25.0);
  PGen.generatePortChicane(Control,hutName+"Chicane3",-280.0,-25.0);


  return;
}

void
exptHutVariables(FuncDataBase& Control,
		 const std::string& hutName)
/*!
    Optics hut variables
    \param Control :: DataBase to add
    \param hutName :: Expt hut name
  */
{
  ELog::RegMethod RegA("danmaxVariables","opticsHutVariables");

  Control.addVariable(hutName+"YStep",1850.0);
  Control.addVariable(hutName+"Depth",120.0);
  Control.addVariable(hutName+"Height",200.0);
  Control.addVariable(hutName+"Length",858.4);
  Control.addVariable(hutName+"OutWidth",198.50);
  Control.addVariable(hutName+"RingWidth",248.6);
  Control.addVariable(hutName+"InnerThick",0.3);
  Control.addVariable(hutName+"PbThick",0.5);
  Control.addVariable(hutName+"OuterThick",0.3);
  Control.addVariable(hutName+"FloorThick",50.0);

  Control.addVariable(hutName+"InnerOutVoid",10.0);
  Control.addVariable(hutName+"OuterOutVoid",10.0);

  Control.addVariable(hutName+"VoidMat","Air");
  Control.addVariable(hutName+"SkinMat","Stainless304");
  Control.addVariable(hutName+"PbMat","Lead");
  Control.addVariable(hutName+"FloorMat","Concrete");

  Control.addVariable(hutName+"HoleXStep",0.0);
  Control.addVariable(hutName+"HoleZStep",5.0);
  Control.addVariable(hutName+"HoleRadius",7.0);
  Control.addVariable(hutName+"HoleMat","Lead");


  Control.addVariable(hutName+"NChicane",4);
  PortChicaneGenerator PGen;
  PGen.generatePortChicane(Control,hutName+"Chicane0","Right",270.0,-25.0);
  PGen.generatePortChicane(Control,hutName+"Chicane1","Right",170.0,-25.0);
  PGen.setSize(4.0,40.0,30.0);
  PGen.generatePortChicane(Control,hutName+"Chicane2","Right",-70.0,-25.0);
  PGen.setSize(4.0,30.0,90.0);
  PGen.generatePortChicane(Control,hutName+"Chicane3","Right",70.0,15.0);
  /*
  PGen.generatePortChicane(Control,hutName+"Chicane1",370.0,-25.0);
  PGen.generatePortChicane(Control,hutName+"Chicane2",-70.0,-25.0);
  PGen.generatePortChicane(Control,hutName+"Chicane3",-280.0,-25.0);
  */

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

  setVariable::GateValveGenerator GateGen;
  setVariable::BellowGenerator BellowGen;
  setVariable::MonoShutterGenerator MShutterGen;
  
  // up / up (true)
  MShutterGen.generateShutter(Control,preName+"MonoShutter",1,1);  
  
  // bellows on shield block
  BellowGen.setCF<setVariable::CF40>();
  BellowGen.setAFlangeCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,preName+"BellowG",0,10.0);    

    // joined and open
  GateGen.setCF<setVariable::CF40>();
  GateGen.generateValve(Control,preName+"GateE",0.0,0);
  return;
}

void
shieldVariables(FuncDataBase& Control)
  /*!
    Shield variables
    \param Control :: DataBase to add
  */
{
  ELog::RegMethod RegA("danmaxVariables","shieldVariables");

  const std::string preName("Danmax");
  
  Control.addVariable(preName+"PShieldLength",10.0);
  Control.addVariable(preName+"PShieldWidth",80.0);
  Control.addVariable(preName+"PShieldHeight",80.0);
  Control.addVariable(preName+"PShieldWallThick",0.5);
  Control.addVariable(preName+"PShieldClearGap",1.0);
  Control.addVariable(preName+"PShieldWallMat","Stainless304");
  Control.addVariable(preName+"PShieldMat","Lead");
   
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
monoVariables(FuncDataBase& Control,const double YStep)
  /*!
    Set the variables for the mono
    \param Control :: DataBase to use
    \param YStep :: Distance to step
  */
{
  ELog::RegMethod RegA("danmaxVariables[F]","monoVariables");
  setVariable::PortItemGenerator PItemGen;
    
  const std::string preName("DanmaxOpticsLine");

  Control.addVariable(preName+"MonoVacYStep",YStep);
  Control.addVariable(preName+"MonoVacZStep",2.0);
  Control.addVariable(preName+"MonoVacRadius",33.0);
  Control.addVariable(preName+"MonoVacRingWidth",21.5);
  Control.addVariable(preName+"MonoVacOutWidth",16.5);
  Control.addVariable(preName+"MonoVacWallThick",1.0);

  Control.addVariable(preName+"MonoVacDoorThick",2.54);
  Control.addVariable(preName+"MonoVacBackThick",2.54);
  Control.addVariable(preName+"MonoVacDoorFlangeRad",4.0);
  Control.addVariable(preName+"MonoVacRingFlangeRad",4.0);
  Control.addVariable(preName+"MonoVacDoorFlangeLen",2.54);
  Control.addVariable(preName+"MonoVacRingFlangeLen",2.54);

  Control.addVariable(preName+"MonoVacInPortZStep",-2.0);
  Control.addVariable(preName+"MonoVacOutPortZStep",2.0);
  
  Control.addVariable(preName+"MonoVacPortRadius",5.0);
  Control.addVariable(preName+"MonoVacPortLen",4.65);
  Control.addVariable(preName+"MonoVacPortThick",0.3);
  Control.addVariable(preName+"MonoVacPortFlangeLen",1.5);
  Control.addVariable(preName+"MonoVacPortFlangeRad",2.7);

  Control.addVariable(preName+"MonoVacWallMat","Stainless304");
  // CRYSTALS:
  Control.addVariable(preName+"MonoXtalZStep",-2.0);
  Control.addVariable(preName+"MonoXtalGap",4.0);
  Control.addVariable(preName+"MonoXtalTheta",10.0);
  Control.addVariable(preName+"MonoXtalPhiA",0.0);
  Control.addVariable(preName+"MonoXtalPhiA",0.0);
  Control.addVariable(preName+"MonoXtalWidth",10.0);
  Control.addVariable(preName+"MonoXtalLengthA",8.0);
  Control.addVariable(preName+"MonoXtalLengthB",12.0);
  Control.addVariable(preName+"MonoXtalThickA",4.0);
  Control.addVariable(preName+"MonoXtalThickB",3.0);
  Control.addVariable(preName+"MonoXtalBaseALength",10.0);
  Control.addVariable(preName+"MonoXtalBaseBLength",14.0);
  Control.addVariable(preName+"MonoXtalBaseGap",0.3);
  Control.addVariable(preName+"MonoXtalBaseThick",1.0);
  Control.addVariable(preName+"MonoXtalBaseExtra",2.0);
  
  Control.addVariable(preName+"MonoXtalMat","Silicon80K");
  Control.addVariable(preName+"MonoXtalBaseMat","Copper");

  // 20cm above port tube
  const Geometry::Vec3D XVecMinus(-1,0,0);
  PItemGen.setCF<setVariable::CF50>(10.0);
  PItemGen.setPlate(1.0,"LeadGlass");

  PItemGen.generatePort(Control,preName+"MonoVacPort0",
			Geometry::Vec3D(0,0,0),XVecMinus);
  return;
}

void
opticsVariables(FuncDataBase& Control,
		const std::string& beamName)
  /*
    Vacuum optics components in the optics hutch
    \param Control :: Function data base
    \param preName :: beamline name
  */
{
  ELog::RegMethod RegA("danmaxVariables[F]","danmaxVariables");

  const std::string opticsName(beamName+"OpticsLine");

  Control.addVariable(opticsName+"OuterLeft",70.0);
  Control.addVariable(opticsName+"OuterRight",50.0);
  Control.addVariable(opticsName+"OuterTop",60.0);

  setVariable::PipeGenerator PipeGen;
  setVariable::BellowGenerator BellowGen;
  setVariable::CrossGenerator CrossGen;
  setVariable::VacBoxGenerator VBoxGen;
  setVariable::PortTubeGenerator PTubeGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::GateValveGenerator GateGen;
  setVariable::JawValveGenerator JawGen;
  setVariable::FlangeMountGenerator FlangeGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::MirrorGenerator MirrGen;

  PipeGen.setNoWindow();   // no window
  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,opticsName+"InitBellow",0,6.0);


  // will be rotated vertical
  const std::string pipeName=opticsName+"TriggerPipe";
  SimpleTubeGen.setCF<CF100>();
  SimpleTubeGen.setCap();
  // up 15cm / 25cm down : Measured
  SimpleTubeGen.generateTube(Control,pipeName,0.0,40.0);

  Control.addVariable(pipeName+"NPorts",2);   // beam ports
  const Geometry::Vec3D ZVec(0,0,1);
  PItemGen.setCF<setVariable::CF40>(5.0);
  PItemGen.setPlate(0.0,"Void");  
  PItemGen.generatePort(Control,pipeName+"Port0",Geometry::Vec3D(0,5.0,0),ZVec);
  PItemGen.generatePort(Control,pipeName+"Port1",Geometry::Vec3D(0,5.0,0),-ZVec);

  // will be rotated vertical
  const std::string gateName=opticsName+"GateTubeA";
  SimpleTubeGen.setCF<CF63>();
  SimpleTubeGen.setCap();
  SimpleTubeGen.generateTube(Control,gateName,0.0,30.0);
  Control.addVariable(gateName+"NPorts",2);   // beam ports

  PItemGen.setCF<setVariable::CF40>(3.45);
  PItemGen.setPlate(0.0,"Void");  
  PItemGen.generatePort(Control,gateName+"Port0",Geometry::Vec3D(0,0,0),ZVec);
  PItemGen.generatePort(Control,gateName+"Port1",Geometry::Vec3D(0,0,0),-ZVec);

  FlangeGen.setNoPlate();
  FlangeGen.setBlade(4.0,5.0,0.3,0.0,"Stainless304",1);  // 22 rotation
  FlangeGen.generateMount(Control,opticsName+"GateTubeAItem",0);  // in beam
  
  PipeGen.setCF<setVariable::CF40>(); 
  BellowGen.setCF<setVariable::CF40>();

  BellowGen.generateBellow(Control,opticsName+"BellowA",0,16.0);
  PipeGen.generatePipe(Control,opticsName+"PipeA",0,38.3);
  BellowGen.generateBellow(Control,opticsName+"BellowB",0,16.0);

  // ACTUAL ROUND PIPE + 4 filter tubles and 1 base tube [large]
  return;
}

void
connectingVariables(FuncDataBase& Control)
  /*!
    Variables for the connecting region
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("danmaxVariables[F]","connectingVariables");

  const std::string baseName="DanmaxConnect";
  const Geometry::Vec3D OPos(0,0,0);
  const Geometry::Vec3D ZVec(0,0,-1);

  Control.addVariable(baseName+"OuterRadius",60.0);
  
  setVariable::BellowGenerator BellowGen;
  setVariable::LeadPipeGenerator LeadPipeGen;
  setVariable::PortTubeGenerator PTubeGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::LeadBoxGenerator LBGen;
  setVariable::PipeShieldGenerator PSGen;
  
  PItemGen.setCF<setVariable::CF40>(3.0);
  PItemGen.setPlate(0.0,"Void");  
  
  BellowGen.setCF<CF40>();  
  BellowGen.generateBellow(Control,baseName+"BellowA",0,10.0);

  LBGen.setPlate(15.0,15.0,0.6);
  LBGen.generateBox(Control,baseName+"LeadA",5.0,12.0);
    
  LeadPipeGen.setCF<CF40>();
  LeadPipeGen.setCladdingThick(0.5);
  LeadPipeGen.generateCladPipe(Control,baseName+"PipeA",10.0,152.0);
  
  PTubeGen.setMat("Stainless304");
  PTubeGen.setPipeCF<CF40>();
  PTubeGen.setPortCF<CF40>();
  PTubeGen.setPortLength(3.0,3.0);
  // ystep/width/height/depth/length
  PTubeGen.generateTube(Control,baseName+"IonPumpA",0.0,4.0);
  Control.addVariable(baseName+"IonPumpANPorts",1);
  PItemGen.generatePort(Control,baseName+"IonPumpAPort0",OPos,ZVec);


  // temp offset
  LBGen.setPlate(15.0,15.0,0.6);
  LBGen.generateBox(Control,baseName+"PumpBoxA",5.50,12.0);
  //  PSGen.generateShield(Control,baseName+"PumpBoxAFShield",0.0,0.0);

  LeadPipeGen.generateCladPipe(Control,baseName+"PipeB",
			       PTubeGen.getTotalLength(4.0),188.0);
  
  BellowGen.generateBellow(Control,baseName+"BellowB",0,10.0);
  LBGen.generateBox(Control,baseName+"LeadB",5.0,12.0);
  
  LeadPipeGen.generateCladPipe(Control,baseName+"PipeC",10.0,188.0);

  // ystep/width/height/depth/length
  PTubeGen.generateTube(Control,baseName+"IonPumpB",0.0,4.0);
  LBGen.generateBox(Control,baseName+"PumpBoxB",5.5,12.0);
  
  Control.addVariable(baseName+"IonPumpBNPorts",1);
  PItemGen.generatePort(Control,baseName+"IonPumpBPort0",OPos,ZVec);
  
  LeadPipeGen.generateCladPipe(Control,baseName+"PipeD",
			       PTubeGen.getTotalLength(4.0),172.0);


  BellowGen.generateBellow(Control,baseName+"BellowC",0,10.0);
  LBGen.generateBox(Control,baseName+"LeadC",5.0,12.0);
  
  return;
}

}  // NAMESPACE danmaxVar
  
void
DANMAXvariables(FuncDataBase& Control)
  /*!
    Function to set the control variables and constants
    -- This version is for Photon Moderator
    \param Control :: Function data base to add constants too
  */
{
  ELog::RegMethod RegA("danmaxVariables[F]","danmaxVariables");

  Control.addVariable("sdefType","Wiggler");

  setVariable::PipeGenerator PipeGen;
  setVariable::LeadPipeGenerator LeadPipeGen;

  PipeGen.setWindow(-2.0,0.0);   // no window

  danmaxVar::undulatorVariables(Control,"DanmaxFrontBeam");
  // ystep / dipole pipe / exit pipe
  setVariable::R3FrontEndVariables
    (Control,"DanmaxFrontBeam",30.0,624,5);
  danmaxVar::frontMaskVariables(Control,"DanmaxFrontBeam");
    
  danmaxVar::wallVariables(Control,"DanmaxWallLead");
  
  PipeGen.setMat("Stainless304");
  PipeGen.setCF<setVariable::CF40>(); 
  PipeGen.generatePipe(Control,"DanmaxJoinPipe",0,125.0);

  danmaxVar::opticsHutVariables(Control,"DanmaxOpticsHut");
  danmaxVar::opticsVariables(Control,"Danmax");

  LeadPipeGen.setCF<setVariable::CF40>();
  LeadPipeGen.setCladdingThick(0.5);
  LeadPipeGen.generateCladPipe(Control,"DanmaxJoinPipeB",0,54.0);

  danmaxVar::shieldVariables(Control);
  danmaxVar::connectingVariables(Control);

  // note bellow skip
  LeadPipeGen.generateCladPipe(Control,"DanmaxJoinPipeC",10.0,80.0);


  danmaxVar::exptHutVariables(Control,"DanmaxExpt");

  const std::string exptName="DanmaxExptLine";
  
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
