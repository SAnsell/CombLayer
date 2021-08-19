/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   flexpes/flexpesVariables.cxx
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
#include "TriggerGenerator.h"
#include "CylGateValveGenerator.h"
#include "GateValveGenerator.h"
#include "GratingMonoGenerator.h"
#include "JawValveGenerator.h"
#include "WallLeadGenerator.h"

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
#include "SqrFMaskGenerator.h"
#include "TwinPipeGenerator.h"
#include "VacBoxGenerator.h"
#include "GrateMonoBoxGenerator.h"

namespace setVariable
{

namespace flexpesVar
{
  void undulatorVariables(FuncDataBase&,const std::string&);
  void frontMaskVariables(FuncDataBase&,const std::string&);

  void transferVariables(FuncDataBase&,const std::string&);
  
  void opticsHutVariables(FuncDataBase&,const std::string&);
  void opticsBeamVariables(FuncDataBase&,const std::string&);
  void monoVariables(FuncDataBase&,const std::string&);
  void m1MirrorVariables(FuncDataBase&,const std::string&);
  void splitterVariables(FuncDataBase&,const std::string&);
  void slitPackageVariables(FuncDataBase&,const std::string&);
  void shieldVariables(FuncDataBase&,const std::string&);
  void wallVariables(FuncDataBase&,const std::string&);

   

void
undulatorVariables(FuncDataBase& Control,
		   const std::string& undKey)
  /*!
    Builds the variables for the collimator
    \param Control :: Database
    \param undKey :: prename
  */
{
  ELog::RegMethod RegA("flexpesVariables[F]","undulatorVariables");
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
  ELog::RegMethod RegA("flexpesVariables[F]","splitVariables");
  setVariable::TwinPipeGenerator TwinGen;
  setVariable::BellowGenerator BellowGen;
  setVariable::GateValveGenerator GateGen;
  setVariable::PipeGenerator PipeGen;
  setVariable::PortTubeGenerator PTubeGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::PipeShieldGenerator ShieldGen;

  
  TwinGen.setCF<CF40>();
  TwinGen.setJoinFlangeCF<CF100>();
  TwinGen.setAPos(-2.7,0);
  TwinGen.setBPos(2.7,0);
  TwinGen.setXYAngle(4.0,-4.0);
  TwinGen.generateTwin(Control,splitKey+"Splitter",0.0,42.0);  

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,splitKey+"BellowAA",16.0);
  BellowGen.generateBellow(Control,splitKey+"BellowBA",16.0);

  GateGen.setLength(3.5);
  GateGen.setCubeCF<setVariable::CF40>();
  GateGen.generateValve(Control,splitKey+"GateAA",0.0,0);
  GateGen.generateValve(Control,splitKey+"GateBA",0.0,0);

  PTubeGen.setMat("Stainless304");
  PTubeGen.setPipeCF<CF63>();
  PTubeGen.setPortCF<CF40>();  
  PTubeGen.setPortLength(2.5,2.5);

  const std::string pumpNameA=splitKey+"PumpTubeAA";
  const std::string pumpNameB=splitKey+"PumpTubeBA";
  const Geometry::Vec3D zVec(0,0,1);
  const Geometry::Vec3D centPoint(0,0,0);
  PTubeGen.generateTube(Control,pumpNameA,0.0,20.0);
  Control.addVariable(pumpNameA+"NPorts",1);
  PTubeGen.generateTube(Control,pumpNameB,0.0,20.0);
  Control.addVariable(pumpNameB+"NPorts",1);

  PItemGen.setCF<setVariable::CF63>(CF63::outerRadius+14.95);
  PItemGen.generatePort(Control,pumpNameA+"Port0",centPoint,zVec);
  PItemGen.generatePort(Control,pumpNameB+"Port0",centPoint,zVec);

  PipeGen.setMat("Stainless304");
  PipeGen.setNoWindow();   // no window
  PipeGen.setCF<setVariable::CF40>();
  PipeGen.generatePipe(Control,splitKey+"OutPipeA",82.5);
  PipeGen.generatePipe(Control,splitKey+"OutPipeB",82.5);

  ShieldGen.setPlate(150.0,100.0,10.0);
  ShieldGen.generateShield(Control,splitKey+"ScreenB",
  			   Geometry::Vec3D(0,-15.50,0.0),0.0);
  return;
}
  
void
m3MirrorVariables(FuncDataBase& Control,
		  const std::string& mirrorKey)
  /*!
    Builds the variables for the M3 Mirror
    \param Control :: Database
    \param mirrorKey :: prename
  */
{
  ELog::RegMethod RegA("flexpesVariables[F]","m3MirrorVariables");

  setVariable::PipeGenerator PipeGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::GateValveGenerator GateGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::JawValveGenerator JawGen;
  setVariable::MirrorGenerator MirrGen;
  
  // will be rotated vertical
  const std::string viewName=mirrorKey+"ViewTube";
  SimpleTubeGen.setCF<CF63>();
  SimpleTubeGen.setBFlangeCF<CF100>();
  SimpleTubeGen.generateTube(Control,viewName,15.0);
  Control.addVariable(viewName+"NPorts",1);   // beam ports

  const double wallThick=setVariable::CF63::innerRadius+
    setVariable::CF63::wallThick;
  PItemGen.setCF<setVariable::CF40>(CF63::outerRadius+5.95);
  PItemGen.setPlate(0.0,"Void");  
  const Geometry::Vec3D angVec(0,cos(M_PI*37.0/180.0),-sin(M_PI*37.0/180.0));
  const double DLen=14.0-wallThick/sin(M_PI*37.0/180.0);

  PItemGen.setCF<setVariable::CF40>(CF63::outerRadius+DLen);
  PItemGen.setOuterVoid(0);
  PItemGen.generatePort(Control,viewName+"Port0",
			Geometry::Vec3D(0,7.5,0),-angVec);
  
  // Simple slits
  JawGen.setCF<setVariable::CF100>();
  JawGen.setLength(4.0);
  JawGen.setSlits(3.0,2.0,0.2,"Tantalum");
  JawGen.generateSlits(Control,mirrorKey+"SlitsB",0.0,0.8,0.8);

  // will be rotated vertical
  const std::string pumpName=mirrorKey+"PumpTubeB";
  SimpleTubeGen.setCF<CF150>();
  SimpleTubeGen.generateTube(Control,pumpName,40.0);
  Control.addVariable(pumpName+"NPorts",3);   // beam ports

  const Geometry::Vec3D ZVec(0,0,1);
  PItemGen.setCF<setVariable::CF63>(CF150::outerRadius+5.4);
  PItemGen.setPlate(0.0,"Void");  
  PItemGen.generatePort(Control,pumpName+"Port0",Geometry::Vec3D(0,0,0),ZVec);

  PItemGen.setCF<setVariable::CF63>(CF150::outerRadius+5.4);
  PItemGen.setPlate(0.0,"Void");  
  PItemGen.generatePort(Control,pumpName+"Port1",Geometry::Vec3D(0,0,0),-ZVec);

  const Geometry::Vec3D pAngVec(0,sin(M_PI*37.0/180.0),-cos(M_PI*37.0/180.0));
  const double PLen=14.0-8.05/cos(M_PI*37.0/180.0);
  PItemGen.setCF<setVariable::CF40>(CF150::outerRadius+PLen);
  PItemGen.setOuterVoid(0);
  PItemGen.generatePort(Control,pumpName+"Port2",
			Geometry::Vec3D(0,0,0),-pAngVec);

  PipeGen.setMat("Stainless304");
  PipeGen.setNoWindow();   // no window
  PipeGen.setCF<setVariable::CF63>();
  PipeGen.setBFlangeCF<setVariable::CF150>();
  PipeGen.generatePipe(Control,mirrorKey+"OffPipeC",18.1);

  const std::string mName=mirrorKey+"M3Tube";
  SimpleTubeGen.setCF<CF150>();
  SimpleTubeGen.generateTube(Control,mName,32.0);  // centre 13.5cm
  Control.addVariable(mName+"NPorts",0);   // beam ports

  // mirror in M3Tube 
  MirrGen.setPlate(28.0,1.0,9.0);  //guess
  // x/y/z/theta/phi/radius
  MirrGen.generateMirror(Control,mirrorKey+"M3Mirror",
			 0.0,0.0, 0.0, 2.0, 0.0,0.0);
  Control.addVariable(mirrorKey+"M3MirrorYAngle",90.0);

  PipeGen.setCF<setVariable::CF100>();
  PipeGen.setAFlangeCF<setVariable::CF150>();
  PipeGen.generatePipe(Control,mirrorKey+"OffPipeD",20.6);

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
  ELog::RegMethod RegA("flexpesVariables[F]","monoVariables");

  setVariable::BellowGenerator BellowGen;
  setVariable::GrateMonoBoxGenerator MBoxGen;
  setVariable::GratingMonoGenerator MXtalGen;
  setVariable::GateValveGenerator GateGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::PipeGenerator PipeGen;
  
  MBoxGen.setMat("Stainless304");
  MBoxGen.setWallThick(1.0);
  MBoxGen.setCF<CF63>();
  MBoxGen.setAPortCF<CF63>();
  MBoxGen.setPortLength(7.5,7.5); // La/Lb
  MBoxGen.setLid(3.0,1.0,1.0); // over/base/roof

  // width/height/depth/length
  // 
  MBoxGen.generateBox(Control,monoKey+"MonoBox",41.2,12.8,12.8,117.1);
  Control.addVariable(monoKey+"MonoBoxPortBZStep",3.1);   //
  
  Control.addVariable(monoKey+"MonoBoxNPorts",0);   // beam ports (lots!!)
  PItemGen.setCF<setVariable::CF63>(7.5);
  PItemGen.setPlate(0.0,"Void");

  MXtalGen.generateGrating(Control,monoKey+"MonoXtal",0.0,3.0);

  PipeGen.setMat("Stainless304");
  PipeGen.setNoWindow();   // no window
  PipeGen.setCF<setVariable::CF63>();
  PipeGen.generatePipe(Control,monoKey+"PipeG",7.0);

  // joined and open
  GateGen.setLength(7.5);
  GateGen.setCubeCF<setVariable::CF63>();
  GateGen.generateValve(Control,monoKey+"GateC",0.0,0);
  
  BellowGen.setCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,monoKey+"BellowE",7.5);
  
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
  ELog::RegMethod RegA("flexpesVariables[F]","slitPackageVariables");

  setVariable::BeamMountGenerator BeamMGen;
  setVariable::BellowGenerator BellowGen;
  setVariable::GateValveGenerator GateGen;
  setVariable::PipeGenerator PipeGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::PipeShieldGenerator ShieldGen;
  
  PipeGen.setMat("Stainless304");
  PipeGen.setNoWindow();   // no window
  PipeGen.setCF<setVariable::CF63>();
  PipeGen.generatePipe(Control,slitKey+"PipeC",26.6);

  // ystep : wing
  ShieldGen.setPlate(80.0,80.0,10.0);
  ShieldGen.generateShield(Control,slitKey+"ScreenA",
  			   Geometry::Vec3D(0,-4.0,0.0),30.0);
  
  PipeGen.setCF<setVariable::CF63>();
  PipeGen.setBFlangeCF<setVariable::CF150>();
  PipeGen.generatePipe(Control,slitKey+"PipeD",9.9);

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
      BeamMGen.generateMount(Control,fname,1);  // outer of beam
    }		       

  PipeGen.setCF<setVariable::CF63>();
  PipeGen.setAFlangeCF<setVariable::CF150>();
  PipeGen.generatePipe(Control,slitKey+"PipeE",5.4);

  // joined and open
  GateGen.setLength(7.5);
  GateGen.setCubeCF<setVariable::CF63>();
  GateGen.generateValve(Control,slitKey+"GateB",0.0,0);

  BellowGen.setCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,slitKey+"BellowD",7.5);

  PipeGen.setCF<setVariable::CF63>();
  PipeGen.generatePipe(Control,slitKey+"PipeF",14);

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
  ELog::RegMethod RegA("flexpesVariables[F]","m1MirrorVariables");

  setVariable::PipeGenerator PipeGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::GateValveGenerator GateGen;
  setVariable::MirrorGenerator MirrGen;
  
  PipeGen.setMat("Stainless304");
  PipeGen.setNoWindow();
  PipeGen.setCF<setVariable::CF63>();
  PipeGen.setBFlangeCF<CF150>();
  PipeGen.generatePipe(Control,mirrorKey+"OffPipeA",6.8);
  Control.addVariable(mirrorKey+"OffPipeAFlangeBackXYAngle",-4.0);
  Control.addVariable(mirrorKey+"OffPipeAFlangeBackXStep",-2.0);

  const std::string mName=mirrorKey+"M1Tube";
  const double centreOffset(sin(M_PI*4.0/180.0)*6.8/2);  // half 6.8
  SimpleTubeGen.setCF<CF150>();
  SimpleTubeGen.generateTube(Control,mName,36.0);  // centre 13.5cm
  Control.addVariable(mName+"XStep",centreOffset);   
  Control.addVariable(mName+"NPorts",0);   // beam ports

  PipeGen.setCF<setVariable::CF63>();
  PipeGen.setAFlangeCF<setVariable::CF150>();
  PipeGen.generatePipe(Control,mirrorKey+"OffPipeB",13.8);
  Control.addVariable(mirrorKey+"OffPipeBFlangeFrontXStep",-2.0);
  Control.addVariable(mirrorKey+"OffPipeBXStep",2.0);



  // mirror in M1Tube 
  MirrGen.setPlate(28.0,1.0,9.0);  //guess
  // y/z/theta/phi/radius
  MirrGen.generateMirror(Control,mirrorKey+"M1Mirror",0.0,0.0, 0.0, 2.0, 0.0,0.0);
  Control.addVariable(mirrorKey+"M1MirrorYAngle",90.0);

  Control.addVariable(mirrorKey+"M1StandHeight",110.0);
  Control.addVariable(mirrorKey+"M1StandWidth",30.0);
  Control.addVariable(mirrorKey+"M1StandLength",30.0);
  Control.addVariable(mirrorKey+"M1StandMat","SiO2");
  
  // joined and open
  GateGen.setLength(7.5);
  GateGen.setCubeCF<setVariable::CF63>();
  GateGen.generateValve(Control,mirrorKey+"GateA",0.0,0);

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
  ELog::RegMethod RegA("flexpesVariables[F]","opticsBeamVariables");

  setVariable::BellowGenerator BellowGen;
  setVariable::GateValveGenerator GateGen;
  setVariable::PipeGenerator PipeGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::PipeShieldGenerator ShieldGen;
  setVariable::TriggerGenerator TGen;
  setVariable::CylGateValveGenerator GVGen;


  Control.addVariable(opticKey+"OuterLeft",80.0);
  Control.addVariable(opticKey+"OuterRight",80.0);
  
  PipeGen.setNoWindow();   // no window
  PipeGen.setMat("Stainless304");
  
  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,opticKey+"BellowA",16.0);

  // will be rotated vertical
  TGen.setCF<CF100>();
  TGen.setVertical(15.0,25.0);
  TGen.setSideCF<setVariable::CF40>(10.0); // add centre distance?
  TGen.generateTube(Control,opticKey+"TriggerPipe");

  GVGen.generateGate(Control,opticKey+"GateTubeA",0);

  BellowGen.generateBellow(Control,opticKey+"BellowB",16.0);

  PipeGen.setCF<CF40>();
  PipeGen.generatePipe(Control,opticKey+"PipeA",50.0);

  // will be rotated vertical
  const Geometry::Vec3D ZVec(0,0,1);
  const std::string florName=opticKey+"FlorTubeA";
  SimpleTubeGen.setCF<CF150>();
  SimpleTubeGen.setCap();
  SimpleTubeGen.generateTube(Control,florName,27.0);  // centre 13.5cm

  Control.addVariable(florName+"NPorts",2);   // beam ports
  PItemGen.setCF<setVariable::CF40>(CF150::outerRadius+2.25);
  PItemGen.setPlate(0.0,"Void");  
  PItemGen.generatePort(Control,florName+"Port0",Geometry::Vec3D(0,0,0),ZVec);
  PItemGen.generatePort(Control,florName+"Port1",Geometry::Vec3D(0,0,0),-ZVec);

  BellowGen.generateBellow(Control,opticKey+"BellowC",16.0);
  
  PipeGen.setCF<CF40>();
  PipeGen.generatePipe(Control,opticKey+"PipeB",169.5);

  // ystep : wing
  ShieldGen.setMaterial("Stainless304","Stainless304","Stainless304");
  ShieldGen.setPlate(20.0,20.0,7.5);
  ShieldGen.generateShield(Control,opticKey+"ScreenExtra",
			   Geometry::Vec3D(0,-80.0,0.0),0.0);
  // will be rotated vertical
  const std::string collName=opticKey+"PumpTubeA";
  SimpleTubeGen.setCF<CF150>();
  SimpleTubeGen.setCap();
  SimpleTubeGen.generateTube(Control,collName,40.0);
  Control.addVariable(collName+"NPorts",3);   // beam ports
  
  PItemGen.setCF<setVariable::CF40>(CF150::outerRadius+5.95);
  PItemGen.setPlate(0.0,"Void");  
  PItemGen.generatePort(Control,collName+"Port0",Geometry::Vec3D(0,0,0),ZVec);

  PItemGen.setCF<setVariable::CF63>(CF150::outerRadius+4.95);
  PItemGen.setPlate(0.0,"Void");  
  PItemGen.generatePort(Control,collName+"Port1",Geometry::Vec3D(0,0,0),-ZVec);

  const Geometry::Vec3D angVec(0,sin(M_PI*35.0/180.0),-cos(M_PI*35.0/180.0));
  const double DLen=17.2-7.55/cos(M_PI*35.0/180.0);
  PItemGen.setCF<setVariable::CF40>(CF150::outerRadius+DLen);
  PItemGen.setOuterVoid(0);
  PItemGen.generatePort(Control,collName+"Port2",
			Geometry::Vec3D(0,0,0),-angVec);
  
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
    \param hutName :: Cave/Hut name (outer)
  */
{
  ELog::RegMethod RegA("balderVariables","opticsHutVariables");

  Control.addVariable(hutName+"Height",200.0);
  Control.addVariable(hutName+"Length",950.0);
  // total inner width [short] - 295.0cm 
  Control.addVariable(hutName+"OutWidth",207.81);
  Control.addVariable(hutName+"RingShortWidth",87.19);
  // total inner width [short] - 400.0cm 
  Control.addVariable(hutName+"RingLongWidth",192.19);
  Control.addVariable(hutName+"ShortLen",240.0);
  // length to first full width point
  Control.addVariable(hutName+"FullLen",348.0);
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

  Control.addVariable(hutName+"Hole0Offset",Geometry::Vec3D(30.0,0,4));
  Control.addVariable(hutName+"Hole0ZStep",1.0);
  Control.addVariable(hutName+"Hole0Radius",5.0);

  Control.addVariable(hutName+"Hole1Offset",Geometry::Vec3D(53.0,0,4));
  Control.addVariable(hutName+"Hole1Radius",5.0);


  Control.addVariable(hutName+"InnerFarVoid",15.0);
  Control.addVariable(hutName+"OuterFarVoid",15.0);
  Control.addVariable(hutName+"NChicane",1);

  SimpleChicaneGenerator PGen;
  PGen.generateSimpleChicane(Control,hutName+"Chicane0",170.0,-25.0);
  PGen.generateSimpleChicane(Control,hutName+"Chicane1",370.0,-25.0);


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
  ELog::RegMethod RegA("flexpesVariables[F]","transferVariables");
  setVariable::PipeGenerator PipeGen;

  PipeGen.setNoWindow();   // no window
  PipeGen.setMat("Stainless304");
  PipeGen.setCF<setVariable::CF40>(); // was 2cm (why?)
  PipeGen.generatePipe(Control,transKey+"JoinPipe",147.0);

  return;
}

void
wallVariables(FuncDataBase& Control,
	      const std::string& wallKey)
/*!
    Set the variables for the frontend wall
    \param Control :: DataBase to use
    \param wallKey :: name before part names
  */
{
  ELog::RegMethod RegA("flexpesVariables[F]","wallVariables");

  WallLeadGenerator LGen;

  LGen.generateWall(Control,wallKey,3.0);
  return;
}

void
shieldVariables(FuncDataBase& Control,
		const std::string& shieldKey)
  /*!
    Set the variables for the front end extra shield
    \param Control :: DataBase to use
    \param shieldKey :: name before part names
  */
{
  ELog::RegMethod RegA("flexpesVariables[F]","shieldVariables");


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
  ELog::RegMethod RegA("flexpesVariables[F]","frontMaskVariables");
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




}  // NAMESPACE flexpesVar

  
void
FLEXPESvariables(FuncDataBase& Control)
  /*!
    Function to set the control variables and constants
    -- This version is for Photon Moderator
    \param Control :: Function data base to add constants too
  */
{
  ELog::RegMethod RegA("flexpesVariables[F]","flexpesVariables");
  RingDoorGenerator RGen(1);  // (1) :: make R1 door
  
  Control.addVariable("sdefType","Wiggler");
  // add ring door to our sector
  RGen.generateDoor(Control,"R1RingRingDoor",50.0);
  Control.addVariable("R1RingRingDoorWallID",7);

  flexpesVar::undulatorVariables(Control,"FlexPesFrontBeam");
  setVariable::R1FrontEndVariables(Control,"FlexPesFrontBeam",25.0);
  flexpesVar::frontMaskVariables(Control,"FlexPesFrontBeam");

  flexpesVar::wallVariables(Control,"FlexPesWallLead");
  flexpesVar::shieldVariables(Control,"FlexPes");
  flexpesVar::transferVariables(Control,"FlexPes");
  flexpesVar::opticsHutVariables(Control,"FlexPesOpticsHut");
  flexpesVar::opticsBeamVariables(Control,"FlexPesOpticsBeam");

  return;
}

}  // NAMESPACE setVariable
