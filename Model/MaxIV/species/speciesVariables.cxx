/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   species/speciesVariables.cxx
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
#include "maxivVariables.h"

#include "CFFlanges.h"

#include "BeamMountGenerator.h"
#include "CollGenerator.h"
#include "CrossGenerator.h"
#include "GateValveGenerator.h"
#include "GratingMonoGenerator.h"
#include "JawValveGenerator.h"
#include "CylGateValveGenerator.h"

#include "MirrorGenerator.h"
#include "PipeGenerator.h"
#include "PipeTubeGenerator.h"
#include "PortTubeGenerator.h"
#include "PortItemGenerator.h"
#include "RingDoorGenerator.h"
#include "PipeShieldGenerator.h"
#include "SimpleChicaneGenerator.h"
#include "SplitPipeGenerator.h"
#include "BellowGenerator.h"
#include "TriggerGenerator.h"
#include "SqrFMaskGenerator.h"
#include "TwinPipeGenerator.h"
#include "VacBoxGenerator.h"
#include "TankMonoVesselGenerator.h"
#include "GratingUnitGenerator.h"
#include "WallLeadGenerator.h"

namespace setVariable
{

namespace speciesVar
{
  void undulatorVariables(FuncDataBase&,const std::string&);
  void frontMaskVariables(FuncDataBase&,const std::string&);

  void transferVariables(FuncDataBase&,const std::string&);
  void opticsHutVariables(FuncDataBase&,const std::string&);
  void opticsBeamVariables(FuncDataBase&,const std::string&);
  void preOpticsVariables(FuncDataBase&,const std::string&);
  void m1MirrorVariables(FuncDataBase&,const std::string&);
  void monoVariables(FuncDataBase&,const std::string&);
  void m3MirrorVariables(FuncDataBase&,const std::string&);
  void splitterVariables(FuncDataBase&,const std::string&);
  void slitPackageVariables(FuncDataBase&,const std::string&);

  
void
collimatorVariables(FuncDataBase& Control,
		    const std::string& collKey)
  /*!
    Builds the variables for the collimator
    \param Control :: Database
    \param collKey :: prename
  */
{
  ELog::RegMethod RegA("speciesVariables[F]","collimatorVariables");

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
undulatorVariables(FuncDataBase& Control,
		   const std::string& undKey)
  /*!
    Builds the variables for the collimator
    \param Control :: Database
    \param undKey :: prename
  */
{
  ELog::RegMethod RegA("speciesVariables[F]","collimatorVariables");
  setVariable::PipeGenerator PipeGen;

  const double L(280.0);
  PipeGen.setMat("Aluminium");
  PipeGen.setNoWindow();   // no window
  PipeGen.setCF<setVariable::CF63>();
  PipeGen.generatePipe(Control,undKey+"UPipe",L);

  Control.addVariable(undKey+"UPipeWidth",6.0);
  Control.addVariable(undKey+"UPipeHeight",0.6);
  Control.addVariable<double>(undKey+"UPipeYStep",-L/2.0);
  Control.addVariable(undKey+"UPipeFeThick",0.2);

  // undulator  
  Control.addVariable(undKey+"UndulatorVGap",1.1);  // mininum 11mm
  Control.addVariable(undKey+"UndulatorLength",247.989);   // 46.2mm*30*2
  Control.addVariable(undKey+"UndulatorMagnetWidth",6.6);
  Control.addVariable(undKey+"UndulatorMagnetDepth",3.3);
  Control.addVariable(undKey+"UndulatorMagnetCorner",0.5);
  Control.addVariable(undKey+"UndulatorMagnetCutSurf",0.01);
  Control.addVariable(undKey+"UndulatorSupportWidth",12.0);
  Control.addVariable(undKey+"UndulatorSupportThick",8.0);
  Control.addVariable(undKey+"UndulatorSupportLength",4.0);  // extra
  Control.addVariable(undKey+"UndulatorSupportVOffset",2.0);
  Control.addVariable(undKey+"UndulatorStandWidth",6.0);
  Control.addVariable(undKey+"UndulatorStandHeight",8.0);
  Control.addVariable(undKey+"UndulatorVoidMat","Void");
  Control.addVariable(undKey+"UndulatorMagnetMat","NbFeB");
  Control.addVariable(undKey+"UndulatorSupportMat","Copper");
  Control.addVariable(undKey+"UndulatorStandMat","Aluminium");

    
  return;
}

void
splitterVariables(FuncDataBase& Control,
  		  const std::string& splitKey)
  /*!
    Builds the variables for the splitter at
    the end of the opticsHut/opticsBeam
    \param Control :: Database
    \param splitKey :: prename
  */
{
  ELog::RegMethod RegA("speciesVariables[F]","splitVariables");
  setVariable::TwinPipeGenerator TwinGen;
  setVariable::BellowGenerator BellowGen;
  setVariable::GateValveGenerator GateGen;
  setVariable::PipeGenerator PipeGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;

  setVariable::PortTubeGenerator PTubeGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::PipeShieldGenerator ShieldGen;

  TwinGen.setCF<CF40>();
  TwinGen.setJoinFlangeCF<CF100>();
  TwinGen.setAPos(-3.0,0);
  TwinGen.setBPos(3.0,0);
  TwinGen.setXYAngle(4.0,-4.0);
  TwinGen.generateTwin(Control,splitKey+"Splitter",0.0,8.0);  

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,splitKey+"BellowAA",10.0);
  BellowGen.generateBellow(Control,splitKey+"BellowBA",10.0);

  const std::string gateNameA=splitKey+"PumpTubeAA";
  const std::string gateNameB=splitKey+"PumpTubeBA";
  SimpleTubeGen.setCF<CF40>();
  SimpleTubeGen.setCap();
  SimpleTubeGen.generateTube(Control,gateNameA,79.4);
  SimpleTubeGen.generateTube(Control,gateNameB,107.5);
  Control.addVariable(gateNameA+"NPorts",1);   // beam ports
  Control.addVariable(gateNameB+"NPorts",1);   // beam ports
  const Geometry::Vec3D ZVec(0,0,1);
  PItemGen.setCF<setVariable::CF40>(CF40::outerRadius+0.45);
  PItemGen.setPlate(0.0,"Void");  
  PItemGen.generatePort(Control,gateNameA+"Port0",Geometry::Vec3D(0,0,0),ZVec);
  PItemGen.generatePort(Control,gateNameB+"Port0",Geometry::Vec3D(0,0,0),ZVec);

  BellowGen.generateBellow(Control,splitKey+"BellowAB",10.0);
  BellowGen.generateBellow(Control,splitKey+"BellowBB",10.0);

  GateGen.setLength(3.5);
  GateGen.setCubeCF<setVariable::CF40>();
  GateGen.generateValve(Control,splitKey+"GateAA",0.0,0);
  GateGen.generateValve(Control,splitKey+"GateBA",0.0,0);

  BellowGen.generateBellow(Control,splitKey+"BellowAC",10.0);
  BellowGen.generateBellow(Control,splitKey+"BellowBC",10.0);

  GateGen.setLength(3.5);
  GateGen.setCubeCF<setVariable::CF40>();
  GateGen.generateValve(Control,splitKey+"GateAB",0.0,0);
  GateGen.generateValve(Control,splitKey+"GateBB",0.0,0);
  
  PipeGen.setMat("Stainless304");
  PipeGen.setNoWindow();   // no window
  PipeGen.setCF<setVariable::CF40>();
  PipeGen.generatePipe(Control,splitKey+"OutPipeA",102.5);
  PipeGen.generatePipe(Control,splitKey+"OutPipeB",102.5);

  ShieldGen.setPlate(120.0,60.0,7.0);
  ShieldGen.generateShield(Control,splitKey+"ScreenC",
			   Geometry::Vec3D(0,20.0,0.0),0.0);

  return;
}
  
void
preOpticsVariables(FuncDataBase& Control,
		   const std::string& opticsKey)
  /*!
    Builds the variables for the optics going to the M1-Mirror
    in the optics hutch
    \param Control :: Database
    \param opticsKey :: prename
  */
{
  ELog::RegMethod RegA("speciesVariables[F]","preOpticsVariables");

  setVariable::BellowGenerator BellowGen;
  setVariable::TriggerGenerator TGen;
  setVariable::CylGateValveGenerator GVGen;
  setVariable::PipeGenerator PipeGen;
  
  PipeGen.setNoWindow();   
  PipeGen.setMat("Stainless304");
  
  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,opticsKey+"BellowA",12.5);

  // will be rotated vertical
  TGen.setCF<CF100>();
  TGen.setVertical(15.0,25.0);
  TGen.setSideCF<setVariable::CF40>(10.0); // add centre distance?
  TGen.generateTube(Control,opticsKey+"TriggerPipe");

  GVGen.generateGate(Control,opticsKey+"GateTubeA",0);
  
  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,opticsKey+"BellowB",16.0);
  return;
}

void
m1MirrorVariables(FuncDataBase& Control,
		  const std::string& mirrorKey)
  /*!
    Builds the variables for the M1 Mirror
    \param Control :: Database
    \param mirrorKey :: prename
  */
{
  ELog::RegMethod RegA("speciesVariables[F]","m1MirrorVariables");

  setVariable::PipeGenerator PipeGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::MirrorGenerator MirrGen;
  setVariable::BellowGenerator BellowGen;
  setVariable::PipeShieldGenerator ShieldGen;

  PipeGen.setNoWindow();   // no window
  PipeGen.setMat("Stainless304");   
  PipeGen.setCF<setVariable::CF40>();
  PipeGen.setBFlangeCF<setVariable::CF120>();
  PipeGen.generatePipe(Control,mirrorKey+"PipeA",6.2);
  
  const std::string mName=mirrorKey+"M1Tube";
  SimpleTubeGen.setPipe(15.0,1.0,17.8,1.0);
  SimpleTubeGen.setCap(1,1);
  SimpleTubeGen.generateTube(Control,mName,36.0);

  Control.addVariable(mName+"NPorts",2);   // beam ports
  const Geometry::Vec3D ZVec(0,0,1);
  PItemGen.setCF<setVariable::CF120>(31.0);  // include inner radis
  PItemGen.setPlate(0.0,"Void");  
  PItemGen.generatePort(Control,mName+"Port0",Geometry::Vec3D(0,0,0),ZVec);

  PItemGen.setCF<setVariable::CF63>(16.0+5.0);
  const Geometry::Vec3D RVec(-sin(M_PI*4.0/180.0),0,cos(M_PI*4.0/180.0));
  PItemGen.setPlate(0.0,"Void");  
  PItemGen.generatePort(Control,mName+"Port1",Geometry::Vec3D(0,0,0),-RVec);

  // mirror in M1Tube 
  MirrGen.setPlate(28.0,1.0,9.0);  //guess
  // x/y/z/theta/phi/radius
  MirrGen.generateMirror(Control,mirrorKey+"M1Mirror",0.0,0.0, 0.0, -2.0, 0.0,0.0);
  Control.addVariable(mirrorKey+"M1MirrorYAngle",90.0);
  Control.addVariable(mirrorKey+"M1MirrorZAngle",-90.0);

  
  BellowGen.setCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,mirrorKey+"BellowC",16.0);

  PipeGen.setCF<CF40>();
  PipeGen.setAFlangeCF<setVariable::CF63>();
  PipeGen.generatePipe(Control,mirrorKey+"PipeB",15.5);

  // ystep : wing
  ShieldGen.generateShield(Control,mirrorKey+"ScreenA",
			   Geometry::Vec3D(0,-2.0,0.0),0.0);
  // Extra lead brick
  Control.addVariable(mirrorKey+"LeadBrickXStep",12.0);
  Control.addVariable(mirrorKey+"LeadBrickYStep",4.0);  // half depth
  Control.addVariable(mirrorKey+"LeadBrickWidth",5.0);
  Control.addVariable(mirrorKey+"LeadBrickHeight",5.0);
  Control.addVariable(mirrorKey+"LeadBrickDepth",8.0);
  Control.addVariable(mirrorKey+"LeadBrickDefMat","Lead");
    

  return;
}


void
slitPackageVariables(FuncDataBase& Control,
		     const std::string& slitKey)
  /*!
    Builds the variables for the Slit package
    \param Control :: Database
    \param slitKey :: prename
  */
{
  ELog::RegMethod RegA("speciesVariables[F]","slitPackageVariables");

  setVariable::BeamMountGenerator BeamMGen;
  setVariable::BellowGenerator BellowGen;
  setVariable::GateValveGenerator GateGen;
  setVariable::PipeGenerator PipeGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::PipeShieldGenerator ShieldGen;

  // joined and open
  GateGen.setLength(3.5);
  GateGen.setCubeCF<setVariable::CF40>();
  GateGen.generateValve(Control,slitKey+"GateA",0.0,0);

  PipeGen.setMat("Stainless304");
  PipeGen.setNoWindow();   // no window
  PipeGen.setCF<setVariable::CF40>();
  PipeGen.setBFlangeCF<setVariable::CF150>();
  PipeGen.generatePipe(Control,slitKey+"PipeC",5.4);

  const std::string sName=slitKey+"SlitTube";
  const double tLen(50.2);
  SimpleTubeGen.setCF<CF150>();
  SimpleTubeGen.generateTube(Control,sName,tLen);  

  Control.addVariable(sName+"NPorts",4);   // beam ports (lots!!)
  PItemGen.setCF<setVariable::CF63>(CF150::outerRadius+6.1);
  PItemGen.setPlate(setVariable::CF63::flangeLength,"Stainless304");

  // -1/5 missed
  const Geometry::Vec3D XVec(1,0,0);
  const Geometry::Vec3D ZVec(0,0,1);
  const Geometry::Vec3D PStep(0,tLen/10.0,0);
  Geometry::Vec3D CPt(0.0,-tLen/2.0,0.0);
  CPt+=PStep*3.0;
  PItemGen.generatePort(Control,sName+"Port0",CPt,-XVec);
  CPt+=PStep*1.8;
  PItemGen.generatePort(Control,sName+"Port1",CPt,XVec);
  CPt+=PStep*1.8;
  PItemGen.generatePort(Control,sName+"Port2",CPt,-ZVec);
  CPt+=PStep*1.8;
  PItemGen.setOuterVoid(0); /// ???
  PItemGen.generatePort(Control,sName+"Port3",CPt,ZVec);

  // Jaw units:
  BeamMGen.setThread(1.0,"Nickel");
  BeamMGen.setLift(0.0,2.5);
  BeamMGen.setEdgeBlock(3.0,3.0,2.0,0.0,"Stainless304");    
  const std::string jawKey[]={"JawMinusX","JawPlusX","JawMinusZ","JawPlusZ"};
  for(size_t i=0;i<4;i++)
    {
      const std::string fname=slitKey+jawKey[i];
      BeamMGen.generateMount(Control,fname,1);  // in beam
    }		       

  PipeGen.setCF<setVariable::CF63>();
  PipeGen.setAFlangeCF<setVariable::CF150>();
  PipeGen.generatePipe(Control,slitKey+"PipeD",34.0);

  // ystep : wing
  ShieldGen.generateShield(Control,slitKey+"ScreenB",
			   Geometry::Vec3D(0,0,0.0),0.0);
  return;
}

void
monoVariables(FuncDataBase& Control,
	      const std::string& monoKey)
  /*!
    Builds the variables for the mono packge
    \param Control :: Database
    \param slitKey :: prename
  */
{
  ELog::RegMethod RegA("speciesVariables[F]","monoVariables");

  setVariable::PipeGenerator PipeGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::TankMonoVesselGenerator MBoxGen;
  setVariable::GratingMonoGenerator MXtalGen;
  setVariable::GratingUnitGenerator MUnitGen;

  PipeGen.setMat("Stainless304");
  PipeGen.setNoWindow(); 
  PipeGen.setCF<setVariable::CF63>();
  PipeGen.setBFlange(17.8,1.0);
  PipeGen.generatePipe(Control,monoKey+"OffPipeA",3.0);
  Control.addVariable(monoKey+"OffPipeAFlangeBackZStep",-7.0);
  

  //
  const double  outerRadius(54.91+1.2);
  MBoxGen.setCF<CF63>();   // set ports
  MBoxGen.setAFlange(17.8,1.0);
  MBoxGen.setBFlange(17.8,1.0);
  MBoxGen.setPortLength(7.5,7.5); // La/Lb
  // ystep/width/height/depth/length
  MBoxGen.generateBox(Control,monoKey+"MonoVessel",42.2,36.45,36.45);
  //  Control.addVariable(monoKey+"MonoVesselPortAZStep",-7);   //
  Control.addVariable(monoKey+"MonoVesselFlangeAZStep",-7);     //
  Control.addVariable(monoKey+"MonoVesselFlangeBZStep",-7);     //
  Control.addVariable(monoKey+"MonoVesselPortBZStep",3.2);      // from primary


  const std::string portName=monoKey+"MonoVessel";
  Control.addVariable(monoKey+"MonoVesselNPorts",1);   // beam ports (lots!!)
  PItemGen.setCF<setVariable::CF120>(outerRadius+5.0);
  PItemGen.setPlate(0.0,"Void");  
  PItemGen.generatePort(Control,portName+"Port0",
			Geometry::Vec3D(0,5.0,10.0),
			Geometry::Vec3D(1,0,0));

  // crystals
  MXtalGen.generateGrating(Control,monoKey+"MonoXtal",0.0,3.0);
  // monounit
  MUnitGen.generateGrating(Control,monoKey+"Grating",0.0,0.0);
  
  PipeGen.setCF<setVariable::CF63>();
  PipeGen.setAFlange(17.8,1.0);
  PipeGen.generatePipe(Control,monoKey+"OffPipeB",3.0);
  Control.addVariable(monoKey+"OffPipeBFlangeFrontZStep",-7.0);

  return;
}

void
m3MirrorVariables(FuncDataBase& Control,
		  const std::string& preKey)
  /*!
    Builds the variables for the mono packge
    \param Control :: Database
    \param preKey :: prename
  */
{
  ELog::RegMethod RegA("speciesVariables[F]","m3MirrorVariables");

  setVariable::BellowGenerator BellowGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::JawValveGenerator JawGen;
  setVariable::PipeGenerator PipeGen;
  setVariable::PortItemGenerator PItemGen;

  PipeGen.setMat("Stainless304");
  PipeGen.setNoWindow(); 
  
  BellowGen.setCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,preKey+"BellowD",9.0);

  PipeGen.setCF<setVariable::CF63>();
  PipeGen.generatePipe(Control,preKey+"PipeE",7.5);
  
  BellowGen.setCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,preKey+"BellowE",14.0);

  PipeGen.setCF<setVariable::CF63>();
  PipeGen.setBFlangeCF<setVariable::CF100>();
  PipeGen.generatePipe(Control,preKey+"PipeF",7.5);

  // Simple slits
  JawGen.setCF<setVariable::CF100>();
  JawGen.setLength(4.0);
  JawGen.setSlits(3.0,2.0,0.2,"Stainless304");
  JawGen.generateSlits(Control,preKey+"MirrorJaws",0.0,0.8,0.8);
  
  // ystep/width/height/depth/length
  // 

  const std::string mName=preKey+"M3Tube";
  SimpleTubeGen.setPipe(15.0,1.0,17.8,1.0);
  SimpleTubeGen.setCap(1,1);
  SimpleTubeGen.generateTube(Control,mName,36.0);

  Control.addVariable(mName+"NPorts",2);   // beam ports
  const Geometry::Vec3D ZVec(0,0,1);
  PItemGen.setCF<setVariable::CF100>(16.0+5.0);
  PItemGen.setPlate(0.0,"Void");  
  PItemGen.generatePort(Control,mName+"Port0",Geometry::Vec3D(0,0,0),ZVec);

  PItemGen.setCF<setVariable::CF100>(16.0+27.0);
  PItemGen.setPlate(0.0,"Void");  
  PItemGen.generatePort(Control,mName+"Port1",Geometry::Vec3D(0,0,0),-ZVec);

  return;
}

void
opticsBeamVariables(FuncDataBase& Control,
		    const std::string& opticKey)
  /*!
    Builds the variables for the moveable apperature table
    containing two movable apertures, pumping and bellows
    \param Control :: Database
    \param opticKey :: prename
  */
{
  ELog::RegMethod RegA("speciesVariables[F]","opticsBeamVariables");


  Control.addVariable(opticKey+"OuterLeft",90.0);
  Control.addVariable(opticKey+"OuterRight",110.0);
  Control.addVariable(opticKey+"OuterTop",110.0);
  Control.addVariable(opticKey+"VoidMat","Void");
  
  preOpticsVariables(Control,opticKey);
  m1MirrorVariables(Control,opticKey);
  slitPackageVariables(Control,opticKey);
  monoVariables(Control,opticKey);
  m3MirrorVariables(Control,opticKey);
  splitterVariables(Control,opticKey);
  return;
}


void
opticsHutVariables(FuncDataBase& Control,
		   const std::string& hutName)
  /*!
    Optics hut variables
    \param Control :: DataBase to add
    \param preName :: Beamline name
    \param caveName :: Cave name
  */
{
  ELog::RegMethod RegA("speciesVariables","opticsHutVariables");

  Control.addVariable(hutName+"Height",242.0);  
  Control.addVariable(hutName+"Length",678.5);
  // total inner width [short] - 295.0cm

  Control.addVariable(hutName+"OutWidth",241.9);   
  Control.addVariable(hutName+"RingShortWidth",115.1);
  // total inner width [short] - 400.0cm 
  Control.addVariable(hutName+"RingLongWidth",176.3);
  // length to first full width point
  Control.addVariable(hutName+"StepLen",245.7);
  Control.addVariable(hutName+"Extension",100.0);

  // THIS IS WRONG but the diagram is a mess.
  Control.addVariable(hutName+"InnerSkin",0.3);
  Control.addVariable(hutName+"PbWallThick",0.4);
  Control.addVariable(hutName+"PbRoofThick",0.2);
  Control.addVariable(hutName+"PbFrontThick",0.4);
  Control.addVariable(hutName+"PbBackThick",0.4);
  Control.addVariable(hutName+"OuterSkin",0.3);

  Control.addVariable(hutName+"VoidMat","Void");
  Control.addVariable(hutName+"InnerMat","Stainless304");
  Control.addVariable(hutName+"PbMat","Lead");
  Control.addVariable(hutName+"OuterMat","Stainless304");
  
  Control.addVariable(hutName+"InletXStep",0.0);
  Control.addVariable(hutName+"InletZStep",0.0);
  Control.addVariable(hutName+"InletRadius",5.0);


  Control.addVariable(hutName+"Hole0Offset",Geometry::Vec3D(23.0,0,4));
  Control.addVariable(hutName+"Hole0ZStep",1.0);
  Control.addVariable(hutName+"Hole0Radius",5.0);

  Control.addVariable(hutName+"Hole1Offset",Geometry::Vec3D(58.0,0,4));
  Control.addVariable(hutName+"Hole1Radius",5.0);


  //  Control.addVariable(hutName+"InnerFarVoid",15.0);
  //  Control.addVariable(hutName+"OuterFarVoid",15.0);
  Control.addVariable(hutName+"NChicane",0);

  SimpleChicaneGenerator PGen;
  PGen.generateSimpleChicane(Control,hutName+"Chicane0",170.0,-25.0);
  PGen.generateSimpleChicane(Control,hutName+"Chicane1",370.0,-25.0);


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
  ELog::RegMethod RegA("speciesVariables[F]","moveApertureTable");

  setVariable::BellowGenerator BellowGen;
  setVariable::PipeGenerator PipeGen;
  setVariable::CrossGenerator CrossGen;

  PipeGen.setNoWindow();   // no window
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
  CrossGen.setPlates(0.5,2.0,2.0);  // wall/Top/base
  CrossGen.setTotalPorts(7.0,7.0);     // len of ports (after main)
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
transferVariables(FuncDataBase& Control,
		  const std::string& transKey)
/*!
    Set the variables for the transfer regions
    \param Control :: DataBase to use
    \param transKey :: name before part names
  */
{
  ELog::RegMethod RegA("speciesVariables[F]","transferVariables");
  setVariable::PipeGenerator PipeGen;

  PipeGen.setNoWindow();   // no window
  PipeGen.setMat("Stainless304");
  PipeGen.setCF<setVariable::CF40>(); // was 2cm (why?)
  PipeGen.generatePipe(Control,transKey+"JoinPipe",185.0);

  return;
}

void
wallVariables(FuncDataBase& Control,
	      const std::string& wallKey)
  /*!
    Set the variables for the frontend wall
    \param Control :: DataBase to use
    \param frontKey :: name before part names
  */
{
  ELog::RegMethod RegA("speciesVariables[F]","wallVariables");

  WallLeadGenerator LGen;

  LGen.generateWall(Control,wallKey,3.0);
  return;
}


void
frontMaskVariables(FuncDataBase& Control,
		   const std::string& frontKey)
  /*!
    Variable for the front maste
    \param Control :: Database
    \param frontKey :: Beamline name
  */
{
  ELog::RegMethod RegA("maxpeemVariables[F]","frontMaskVariables");
  setVariable::SqrFMaskGenerator FMaskGen;

  const double FMdist(456.0);

  // collimator block
  
  FMaskGen.setCF<CF63>();
  FMaskGen.setBFlangeCF<CF40>();
  FMaskGen.setFrontGap(3.99,1.97);  //1033.8
  FMaskGen.setBackGap(0.71,0.71);
  FMaskGen.setMinSize(10.2,0.71,0.71);
  FMaskGen.generateColl(Control,frontKey+"CollA",FMdist,15.0);

  return;
}


}  // NAMESPACE speciesVar

  
void
SPECIESvariables(FuncDataBase& Control)
  /*!
    Function to set the control variables and constants
    -- This version is for Photon Moderator
    \param Control :: Function data base to add constants too
  */
{
  ELog::RegMethod RegA("speciesVariables[F]","speciesVariables");
  RingDoorGenerator RGen(1);  // (1) :: make R1 door
  
  Control.addVariable("sdefType","Wiggler");
  // add ring door to our sector
  RGen.generateDoor(Control,"R1RingRingDoor",50.0);
  Control.addVariable("R1RingRingDoorWallID",8);

  speciesVar::undulatorVariables(Control,"SpeciesFrontBeam");
  // ystep [0] / dipole pipe / exit pipe
  setVariable::R1FrontEndVariables(Control,"SpeciesFrontBeam",25.0);
  speciesVar::frontMaskVariables(Control,"SpeciesFrontBeam");
  
  speciesVar::wallVariables(Control,"SpeciesWallLead");
  speciesVar::transferVariables(Control,"Species");
  speciesVar::opticsHutVariables(Control,"SpeciesOpticsHut");
  speciesVar::opticsBeamVariables(Control,"SpeciesOpticsBeam");

  return;
}

}  // NAMESPACE setVariable
