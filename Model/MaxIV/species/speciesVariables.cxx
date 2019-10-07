/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   species/speciesVariables.cxx
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

#include "CFFlanges.h"

#include "BeamMountGenerator.h"
#include "CollGenerator.h"
#include "CrossGenerator.h"
#include "FlangeMountGenerator.h"
#include "GateValveGenerator.h"
#include "GratingMonoGenerator.h"
#include "HeatDumpGenerator.h"
#include "JawValveGenerator.h"
#include "LeadBoxGenerator.h"

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
#include "LeadPipeGenerator.h"
#include "SqrFMaskGenerator.h"
#include "TwinPipeGenerator.h"
#include "VacBoxGenerator.h"
#include "TankMonoVesselGenerator.h"
#include "GratingUnitGenerator.h"
#include "WallLeadGenerator.h"
#include "QuadUnitGenerator.h"
#include "DipoleChamberGenerator.h"

namespace setVariable
{

namespace speciesVar
{
  void undulatorVariables(FuncDataBase&,const std::string&);
  void collimatorVariables(FuncDataBase&,const std::string&);
  void moveApertureTable(FuncDataBase&,const std::string&);
  void heatDumpVariables(FuncDataBase&,const std::string&);
  void shutterTable(FuncDataBase&,const std::string&);
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
  PipeGen.generatePipe(Control,undKey+"UPipe",0,L);

  Control.addVariable(undKey+"UPipeWidth",6.0);
  Control.addVariable(undKey+"UPipeHeight",0.6);
  Control.addVariable<double>(undKey+"UPipeYStep",-L/2.0);
  Control.addVariable(undKey+"UPipeFeThick",0.2);

  // undulator  
  Control.addVariable(undKey+"UndulatorVGap",1.1);  // mininum 11mm
  Control.addVariable(undKey+"UndulatorLength",247.989);   // 46.2mm*30*2
  Control.addVariable(undKey+"UndulatorMagnetWidth",6.0);
  Control.addVariable(undKey+"UndulatorMagnetDepth",3.0);
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
  TwinGen.setAPos(-2.7,0);
  TwinGen.setBPos(2.7,0);
  TwinGen.setXYAngle(4.0,-4.0);
  TwinGen.generateTwin(Control,splitKey+"Splitter",0.0,8.0);  

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,splitKey+"BellowAA",0,10.0);
  BellowGen.generateBellow(Control,splitKey+"BellowBA",0,10.0);

  const std::string gateNameA=splitKey+"PumpTubeAA";
  const std::string gateNameB=splitKey+"PumpTubeBA";
  SimpleTubeGen.setCF<CF40>();
  SimpleTubeGen.setCap();
  SimpleTubeGen.generateTube(Control,gateNameA,0.0,79.4);
  SimpleTubeGen.generateTube(Control,gateNameB,0.0,107.5);
  Control.addVariable(gateNameA+"NPorts",1);   // beam ports
  Control.addVariable(gateNameB+"NPorts",1);   // beam ports
  const Geometry::Vec3D ZVec(0,0,1);
  PItemGen.setCF<setVariable::CF40>(0.45);
  PItemGen.setPlate(0.0,"Void");  
  PItemGen.generatePort(Control,gateNameA+"Port0",Geometry::Vec3D(0,0,0),ZVec);
  PItemGen.generatePort(Control,gateNameB+"Port0",Geometry::Vec3D(0,0,0),ZVec);

  BellowGen.generateBellow(Control,splitKey+"BellowAB",0,10.0);
  BellowGen.generateBellow(Control,splitKey+"BellowBB",0,10.0);

  GateGen.setLength(3.5);
  GateGen.setCF<setVariable::CF40>();
  GateGen.generateValve(Control,splitKey+"GateAA",0.0,0);
  GateGen.generateValve(Control,splitKey+"GateBA",0.0,0);

  BellowGen.generateBellow(Control,splitKey+"BellowAC",0,10.0);
  BellowGen.generateBellow(Control,splitKey+"BellowBC",0,10.0);

  GateGen.setLength(3.5);
  GateGen.setCF<setVariable::CF40>();
  GateGen.generateValve(Control,splitKey+"GateAB",0.0,0);
  GateGen.generateValve(Control,splitKey+"GateBB",0.0,0);
  
  PipeGen.setMat("Stainless304");
  PipeGen.setNoWindow();   // no window
  PipeGen.setCF<setVariable::CF40>();
  PipeGen.generatePipe(Control,splitKey+"OutPipeA",0,102.5);
  PipeGen.generatePipe(Control,splitKey+"OutPipeB",0,102.5);

  ShieldGen.setPlate(120.0,60.0,7.0);
  ShieldGen.generateShield(Control,splitKey+"ScreenC",32.0,0.0);

  return;
}
  
void
preOpticsVariables(FuncDataBase& Control,
		   const std::string& frontKey)
  /*!
    Builds the variables for the optics going to the M1-Mirror
    in the optics hutch
    \param Control :: Database
    \param frontKey :: prename
  */
{
  ELog::RegMethod RegA("speciesVariables[F]","preOpticsVariables");
  setVariable::BellowGenerator BellowGen;
  setVariable::GateValveGenerator GateGen;
  setVariable::PipeGenerator PipeGen;
  setVariable::CrossGenerator CrossGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::PortItemGenerator PItemGen;
    
  PipeGen.setNoWindow();   // no window
  PipeGen.setMat("Stainless304");
  
  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,frontKey+"BellowA",0,12.5);

    // flange if possible
  CrossGen.setPlates(0.5,2.0,2.0);       // wall/Top/base
  CrossGen.setTotalPorts(10.0,10.0);     // len of ports (after main)
  CrossGen.setMat("Stainless304");
  // height/depth
  CrossGen.generateDoubleCF<setVariable::CF40,setVariable::CF100>
    (Control,frontKey+"IonPA",0.0,24.4,36.6);

  // joined and open
  GateGen.setLength(3.5);
  GateGen.setCF<setVariable::CF40>();
  GateGen.generateValve(Control,frontKey+"GateRing",0.0,0);
  
  // will be rotated vertical
  const std::string gateName=frontKey+"GateTubeA";
  SimpleTubeGen.setCF<CF63>();
  SimpleTubeGen.setCap();
  SimpleTubeGen.generateTube(Control,gateName,0.0,30.0);
  Control.addVariable(gateName+"NPorts",2);   // beam ports
  const Geometry::Vec3D ZVec(0,0,1);
  PItemGen.setCF<setVariable::CF40>(0.45);
  PItemGen.setPlate(0.0,"Void");  
  PItemGen.generatePort(Control,gateName+"Port0",Geometry::Vec3D(0,0,0),ZVec);
  PItemGen.generatePort(Control,gateName+"Port1",Geometry::Vec3D(0,0,0),-ZVec);

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,frontKey+"BellowB",0,16.0);
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
  PipeGen.generatePipe(Control,mirrorKey+"PipeA",0,6.2);
  
  const std::string mName=mirrorKey+"M1Tube";
  SimpleTubeGen.setPipe(15.0,1.0,17.8,1.0);
  SimpleTubeGen.setCap(1,1);
  SimpleTubeGen.generateTube(Control,mName,0.0,36.0);

  Control.addVariable(mName+"NPorts",2);   // beam ports
  const Geometry::Vec3D ZVec(0,0,1);
  PItemGen.setCF<setVariable::CF120>(15.0);
  PItemGen.setPlate(0.0,"Void");  
  PItemGen.generatePort(Control,mName+"Port0",Geometry::Vec3D(0,0,0),ZVec);

  PItemGen.setCF<setVariable::CF63>(5.0);
  const Geometry::Vec3D RVec(-sin(M_PI*4.0/180.0),0,cos(M_PI*4.0/180.0));
  PItemGen.setPlate(0.0,"Void");  
  PItemGen.generatePort(Control,mName+"Port1",Geometry::Vec3D(0,0,0),-RVec);

  // mirror in M1Tube 
  MirrGen.setPlate(28.0,1.0,9.0);  //guess
  // y/z/theta/phi/radius
  MirrGen.generateMirror(Control,mirrorKey+"M1Mirror",0.0, 0.0, -2.0, 0.0,0.0);
  Control.addVariable(mirrorKey+"M1MirrorYAngle",90.0);
  Control.addVariable(mirrorKey+"M1MirrorZAngle",-90.0);

  
  BellowGen.setCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,mirrorKey+"BellowC",0,16.0);

  PipeGen.setCF<CF40>();
  PipeGen.setAFlangeCF<setVariable::CF63>();
  PipeGen.generatePipe(Control,mirrorKey+"PipeB",0,15.5);

  // ystep : wing
  ShieldGen.generateShield(Control,mirrorKey+"ScreenA",0.0,0.0);

  // Extra lead brick
  Control.addVariable(mirrorKey+"LeadBrickXStep",12.0);
  Control.addVariable(mirrorKey+"LeadBrickYStep",4.0);  // half depth
  Control.addVariable(mirrorKey+"LeadBrickWidth",5.0);
  Control.addVariable(mirrorKey+"LeadBrickHeight",5.0);
  Control.addVariable(mirrorKey+"LeadBrickDepth",8.0);
  Control.addVariable(mirrorKey+"LeadBrickMat","Lead");
    

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
  GateGen.setCF<setVariable::CF40>();
  GateGen.generateValve(Control,slitKey+"GateA",0.0,0);

  PipeGen.setMat("Stainless304");
  PipeGen.setNoWindow();   // no window
  PipeGen.setCF<setVariable::CF40>();
  PipeGen.setBFlangeCF<setVariable::CF150>();
  PipeGen.generatePipe(Control,slitKey+"PipeC",0,5.4);

  const std::string sName=slitKey+"SlitTube";
  const double tLen(50.2);
  SimpleTubeGen.setCF<CF150>();
  SimpleTubeGen.generateTube(Control,sName,0.0,tLen);  

  Control.addVariable(sName+"NPorts",4);   // beam ports (lots!!)
  PItemGen.setCF<setVariable::CF63>(6.1);
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
  PipeGen.generatePipe(Control,slitKey+"PipeD",0,34.0);

  // ystep : wing
  ShieldGen.generateShield(Control,slitKey+"ScreenB",0.0,0.0);

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
  PipeGen.generatePipe(Control,monoKey+"OffPipeA",0,3.0);
  Control.addVariable(monoKey+"OffPipeAFlangeBackZStep",-7.0);
  
  // ystep/width/height/depth/length
  // 
  MBoxGen.setCF<CF63>();   // set ports
  MBoxGen.setAFlange(17.8,1.0);
  MBoxGen.setBFlange(17.8,1.0);
  MBoxGen.setPortLength(7.5,7.5); // La/Lb
  MBoxGen.generateBox(Control,monoKey+"MonoVessel",0.0,42.2,36.45,36.45);
  //  Control.addVariable(monoKey+"MonoVesselPortAZStep",-7);   //
  Control.addVariable(monoKey+"MonoVesselFlangeAZStep",-7);     //
  Control.addVariable(monoKey+"MonoVesselFlangeBZStep",-7);     //
  Control.addVariable(monoKey+"MonoVesselPortBZStep",3.2);      // from primary


  const std::string portName=monoKey+"MonoVessel";
  Control.addVariable(monoKey+"MonoVesselNPorts",1);   // beam ports (lots!!)
  PItemGen.setCF<setVariable::CF120>(5.0);
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
  PipeGen.generatePipe(Control,monoKey+"OffPipeB",0,3.0);
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
  BellowGen.generateBellow(Control,preKey+"BellowD",0,9.0);

  PipeGen.setCF<setVariable::CF63>();
  PipeGen.generatePipe(Control,preKey+"PipeE",0,7.5);
  
  BellowGen.setCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,preKey+"BellowE",0,14.0);

  PipeGen.setCF<setVariable::CF63>();
  PipeGen.setBFlangeCF<setVariable::CF100>();
  PipeGen.generatePipe(Control,preKey+"PipeF",0,7.5);

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
  SimpleTubeGen.generateTube(Control,mName,0.0,36.0);

  Control.addVariable(mName+"NPorts",2);   // beam ports
  const Geometry::Vec3D ZVec(0,0,1);
  PItemGen.setCF<setVariable::CF100>(5.0);
  PItemGen.setPlate(0.0,"Void");  
  PItemGen.generatePort(Control,mName+"Port0",Geometry::Vec3D(0,0,0),ZVec);

  PItemGen.setCF<setVariable::CF100>(27.0);
  PItemGen.setPlate(0.0,"Void");  
  PItemGen.generatePort(Control,mName+"Port1",Geometry::Vec3D(0,0,0),-ZVec);

  return;
}

void
opticsBeamVariables(FuncDataBase& Control,
		    const std::string& opticKey)
  /*!
    Builds the variables for the moveable apperature table
    containing two movable aperatures, pumping and bellows
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
    containing two movable aperatures, pumping and bellows
    \param Control :: Database
    \param frontKey :: prename
  */
{
  ELog::RegMethod RegA("speciesVariables[F]","moveAperatureTable");

  setVariable::BellowGenerator BellowGen;
  setVariable::PipeGenerator PipeGen;
  setVariable::CrossGenerator CrossGen;

  PipeGen.setNoWindow();   // no window
  PipeGen.setMat("Stainless304");
  PipeGen.setCF<CF40>();
  PipeGen.setBFlangeCF<CF63>();
  PipeGen.generatePipe(Control,frontKey+"PipeB",0,15.0);

  BellowGen.setCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,frontKey+"BellowE",0,14.0);

  // Aperature pipe is movable:
  PipeGen.setCF<CF63>();
  PipeGen.generatePipe(Control,frontKey+"AperturePipe",14.0,24.0);
  collimatorVariables(Control,frontKey+"MoveCollA");
  
  BellowGen.setCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,frontKey+"BellowF",0,14.0);

  // Stepped 420mm from pipeB so bellows/aperaturePipe can move freely
  CrossGen.setMat("Stainless304");
  CrossGen.setPlates(0.5,2.0,2.0);  // wall/Top/base
  CrossGen.setTotalPorts(7.0,7.0);     // len of ports (after main)
  CrossGen.generateDoubleCF<setVariable::CF63,setVariable::CF100>
    (Control,frontKey+"IonPC",52.0,15.74,28.70);   // height/depth

  // [FREE FLOATING]
  BellowGen.setCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,frontKey+"BellowG",0,14.0);

  // Aperature pipe is movable:
  PipeGen.setCF<CF63>();
  PipeGen.generatePipe(Control,frontKey+"AperturePipeB",14.0,24.0);
  collimatorVariables(Control,frontKey+"MoveCollB");
  Control.addVariable(frontKey+"MoveCollBYAngle",180.0);
  
  // [FREE FLOATING]
  BellowGen.setCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,frontKey+"BellowH",0,14.0);

  // [End fix for BellowH]
  PipeGen.setCF<CF40>();
  PipeGen.setAFlangeCF<CF63>();
  PipeGen.generatePipe(Control,frontKey+"PipeC",52.0,10.0);
  
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
  ELog::RegMethod RegA("speciesVariables[F]","heatDumpTable");

  setVariable::BellowGenerator BellowGen;
  setVariable::PipeGenerator PipeGen;
  setVariable::CrossGenerator CrossGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::PortItemGenerator PItemGen;
    
  PipeGen.setNoWindow();   // no window
  PipeGen.setMat("Stainless304");
  
  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,frontKey+"BellowC",0,10.0);
  
  PipeGen.setCF<CF40>();
  PipeGen.generatePipe(Control,frontKey+"HeatPipe",0,115.0);

  heatDumpVariables(Control,frontKey);

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,frontKey+"BellowD",0,10.0);

  // will be rotated vertical
  const std::string gateName=frontKey+"GateTubeA";
  SimpleTubeGen.setCF<CF63>();
  SimpleTubeGen.setCap();
  SimpleTubeGen.generateTube(Control,frontKey+"GateTubeA",0.0,20.0);

  // beam ports
  Control.addVariable(gateName+"NPorts",2);
  const Geometry::Vec3D ZVec(0,0,1);
  PItemGen.setCF<setVariable::CF40>(0.45);
  PItemGen.setPlate(0.0,"Void");  
  PItemGen.generatePort(Control,gateName+"Port0",Geometry::Vec3D(0,0,0),ZVec);
  PItemGen.generatePort(Control,gateName+"Port1",Geometry::Vec3D(0,0,0),-ZVec);

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
  ELog::RegMethod RegA("speciesVariables","heatDumpVariables");

  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::HeatDumpGenerator HeatGen;

  SimpleTubeGen.setMat("Stainless304");
  SimpleTubeGen.setCF<CF150>();
  SimpleTubeGen.setCap(1,0);
  SimpleTubeGen.generateTube(Control,frontKey+"HeatBox",0.0,20.0);
  Control.addVariable(frontKey+"HeatBoxNPorts",2);

  // beam ports
  PItemGen.setCF<setVariable::CF40>(5.0);
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
shutterTable(FuncDataBase& Control,
	     const std::string& frontKey)
  /*!
    Set the variables for the shutter table (number 3)
    \param Control :: DataBase to use
    \param frontKey :: name before part names
  */
{
  ELog::RegMethod RegA("speciesVariables[F]","shutterTable");

  setVariable::BellowGenerator BellowGen;
  setVariable::GateValveGenerator GateGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::PipeGenerator PipeGen;
  setVariable::BeamMountGenerator BeamMGen;
    
  // joined and open
  GateGen.setLength(3.5);
  GateGen.setCF<setVariable::CF40>();
  GateGen.generateValve(Control,frontKey+"GateA",0.0,0);
  
  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,frontKey+"BellowI",0,10.0);
  
  SimpleTubeGen.setCF<CF100>();
  SimpleTubeGen.setCap();
  SimpleTubeGen.generateTube(Control,frontKey+"FlorTubeA",0.0,16.0);

  // beam ports
  const std::string florName(frontKey+"FlorTubeA");
  Control.addVariable(florName+"NPorts",4);
  const Geometry::Vec3D XVec(1,0,0);
  const Geometry::Vec3D ZVec(0,0,1);

  PItemGen.setCF<setVariable::CF40>(1.0);
  PItemGen.setPlate(0.0,"Void");  
  PItemGen.generatePort(Control,florName+"Port0",Geometry::Vec3D(0,0,0),ZVec);
  PItemGen.generatePort(Control,florName+"Port1",Geometry::Vec3D(0,0,0),-ZVec);
  PItemGen.generatePort(Control,florName+"Port2",Geometry::Vec3D(0,0,0),XVec);
  PItemGen.generatePort(Control,florName+"Port3",Geometry::Vec3D(0,0,0),-XVec);

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,frontKey+"BellowJ",0,10.0);

  // will be rotated vertical
  const std::string gateName=frontKey+"GateTubeB";
  SimpleTubeGen.setCF<CF63>();
  SimpleTubeGen.setCap();
  SimpleTubeGen.generateTube(Control,frontKey+"GateTubeB",0.0,20.0);
  // beam ports
  Control.addVariable(gateName+"NPorts",2);
  PItemGen.setCF<setVariable::CF40>(0.45);
  PItemGen.setPlate(0.0,"Void");  
  PItemGen.generatePort(Control,gateName+"Port0",Geometry::Vec3D(0,0,0),ZVec);
  PItemGen.generatePort(Control,gateName+"Port1",Geometry::Vec3D(0,0,0),-ZVec);
  
  PipeGen.setMat("Stainless304");
  PipeGen.setNoWindow();   // no window
  PipeGen.setCF<setVariable::CF40>();
  PipeGen.setBFlangeCF<setVariable::CF150>();
  PipeGen.generatePipe(Control,frontKey+"OffPipeA",0,6.8);
  Control.addVariable(frontKey+"OffPipeAFlangeBackZStep",3.0);


  const std::string shutterName=frontKey+"ShutterBox";
  const double sBoxLen(51.0);
  SimpleTubeGen.setCF<CF150>();
  SimpleTubeGen.generateTube(Control,shutterName,0.0,sBoxLen);
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
  PipeGen.generatePipe(Control,frontKey+"OffPipeB",0,21.0);
  Control.addVariable(frontKey+"OffPipeBFlangeFrontZStep",3.0);
  Control.addVariable(frontKey+"OffPipeBZStep",-3.0);

  Control.addVariable(frontKey+"BremBlockRadius",3.0);
  Control.addVariable(frontKey+"BremBlockLength",20.0);
  Control.addVariable(frontKey+"BremBlockHoleWidth",2.0);
  Control.addVariable(frontKey+"BremBlockHoleHeight",2.0);
  Control.addVariable(frontKey+"BremBlockMainMat","Tungsten");

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.setAFlangeCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,frontKey+"BellowK",0,11.05);
  
  
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
  PipeGen.generatePipe(Control,transKey+"JoinPipe",0,185.0);

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
frontEndVariables(FuncDataBase& Control,
		  const std::string& frontKey)
/*!
    Set the variables for the frontend
    \param Control :: DataBase to use
    \param frontKey :: name before part names
  */
{
  ELog::RegMethod RegA("speciesVariables[F]","frontEndVariables");

  setVariable::BellowGenerator BellowGen;
  setVariable::PipeGenerator PipeGen;
  setVariable::CrossGenerator CrossGen;
  setVariable::VacBoxGenerator VBoxGen;
  setVariable::SqrFMaskGenerator CollGen;
  setVariable::PortTubeGenerator PTubeGen;
  setVariable::PortItemGenerator PItemGen;

  Control.addVariable(frontKey+"OuterRadius",50.0);
  
  PipeGen.setNoWindow();   // no window
  PipeGen.setMat("Stainless304");

  undulatorVariables(Control,frontKey);
   
  Control.addVariable(frontKey+"ECutDiskYStep",2.0);
  Control.addVariable(frontKey+"ECutDiskLength",0.1);
  Control.addVariable(frontKey+"ECutDiskRadius",0.50);
  Control.addVariable(frontKey+"ECutDiskDefMat","H2Gas#0.1");

  Control.addVariable(frontKey+"ECutMagDiskYStep",2.0);
  Control.addVariable(frontKey+"ECutMagDiskDepth",0.1);
  Control.addVariable(frontKey+"ECutMagDiskWidth",4.6);
  Control.addVariable(frontKey+"ECutMagDiskHeight",1.8);
  Control.addVariable(frontKey+"ECutMagDiskDefMat","H2Gas#0.1");

  Control.addVariable(frontKey+"ECutWallDiskYStep",20.0);
  Control.addVariable(frontKey+"ECutWallDiskDepth",0.1);
  Control.addVariable(frontKey+"ECutWallDiskWidth",20.0);
  Control.addVariable(frontKey+"ECutWallDiskHeight",20.0);
  Control.addVariable(frontKey+"ECutWallDiskDefMat","H2Gas#0.1");

  setVariable::QuadUnitGenerator PGen;
  PGen.generatePipe(Control,frontKey+"QuadUnit",0.0);

  setVariable::DipoleChamberGenerator DCGen;
  DCGen.generatePipe(Control,frontKey+"DipoleChamber",0.0);

  // this reaches 454.5cm from the middle of the undulator
  PipeGen.setCF<CF40>();
  PipeGen.setAFlangeCF<CF63>();
  //  PipeGen.generatePipe(Control,frontKey+"DipolePipe",0,291.1+7.5);
  PipeGen.generatePipe(Control,frontKey+"DipolePipe",0,88.0);

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.setBFlangeCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,frontKey+"BellowA",0,10.0);

  // collimator block
  CollGen.setCF<CF63>();
  CollGen.setBFlangeCF<CF40>();
  CollGen.setFrontGap(3.99,1.97);  //1033.8
  CollGen.setBackGap(0.71,0.71);
  CollGen.setMinSize(10.2,0.71,0.71);
  CollGen.generateColl(Control,frontKey+"CollA",0.0,15.0);
  

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,frontKey+"BellowB",0,10.0);

  // flange if possible
  CrossGen.setPlates(0.5,2.0,2.0);  // wall/Top/base
  CrossGen.setTotalPorts(10.0,10.0);     // len of ports (after main)
  CrossGen.setMat("Stainless304");
  // height/depth
  CrossGen.generateDoubleCF<setVariable::CF40,setVariable::CF100>
    (Control,frontKey+"IonPA",0.0,26.6,26.6);

  heatDumpTable(Control,frontKey);
  moveApertureTable(Control,frontKey);
  shutterTable(Control,frontKey);

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
  RingDoorGenerator RGen;
  
  Control.addVariable("sdefType","Wiggler");
  // add ring door to our sector
  RGen.generateDoor(Control,"R1RingRingDoor",0.0);
  Control.addVariable("R1RingRingDoorWallID",8);
  
  speciesVar::frontEndVariables(Control,"SpeciesFrontBeam");  
  speciesVar::wallVariables(Control,"SpeciesWallLead");
  speciesVar::transferVariables(Control,"Species");
  speciesVar::opticsHutVariables(Control,"SpeciesOpticsHut");
  speciesVar::opticsBeamVariables(Control,"SpeciesOpticsBeam");

  return;
}

}  // NAMESPACE setVariable
