/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   cosaxs/cosaxsVariables.cxx
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
#include "BremCollGenerator.h"
#include "LeadPipeGenerator.h"
#include "CrossGenerator.h"
#include "GateValveGenerator.h"
#include "JawValveGenerator.h"
#include "PipeTubeGenerator.h"
#include "PortTubeGenerator.h"
#include "PortItemGenerator.h"
#include "VacBoxGenerator.h"
#include "MonoBoxGenerator.h"
#include "FlangeMountGenerator.h"
#include "MirrorGenerator.h"
#include "CollGenerator.h"
#include "JawFlangeGenerator.h"
#include "MazeGenerator.h"
#include "RingDoorGenerator.h"
#include "PortChicaneGenerator.h"
#include "WallLeadGenerator.h"
#include "MonoShutterGenerator.h"

#include "PreDipoleGenerator.h"
#include "DipoleChamberGenerator.h"

#include "R3ChokeChamberGenerator.h"
#include "DiffPumpGenerator.h"

namespace setVariable
{

namespace cosaxsVar
{
  void undulatorVariables(FuncDataBase&,const std::string&);
  void wallVariables(FuncDataBase&,const std::string&);
  void monoShutterVariables(FuncDataBase&,const std::string&);

void
undulatorVariables(FuncDataBase& Control,
		   const std::string& undKey)
  /*!
    Builds the variables for the undulator
    \param Control :: Database
    \param undKey :: prename
  */
{
  ELog::RegMethod RegA("cosaxsVariables[F]","undulatorVariables");
  setVariable::PipeGenerator PipeGen;

  const double L(210.0);
  PipeGen.setMat("Aluminium");
  PipeGen.setNoWindow();   // no window
  PipeGen.setCF<setVariable::CF63>();
  PipeGen.generatePipe(Control,undKey+"UPipe",0,L);

  Control.addVariable(undKey+"UPipeWidth",6.0);
  Control.addVariable(undKey+"UPipeHeight",0.6);
  Control.addVariable<double>(undKey+"UPipeYStep",20.0);
  Control.addVariable(undKey+"UPipeFeThick",0.2);

  // undulator I Vacuum
  Control.addVariable(undKey+"UndulatorVGap",1.1);  // mininum 11mm
  Control.addVariable(undKey+"UndulatorLength",203.0); 
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
wallVariables(FuncDataBase& Control,
	      const std::string& wallKey)
/*!
    Set the variables for the frontEnd wall
    \param Control :: DataBase to use
    \param wallKey :: name before part names
  */
{
  ELog::RegMethod RegA("cosaxsVariables[F]","wallVariables");

  WallLeadGenerator LGen;
  LGen.setWidth(140.0,70.0);
  LGen.generateWall(Control,wallKey,2.0);

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
  ELog::RegMethod RegA("cosaxsVariables","monoShutterVariables");

  setVariable::GateValveGenerator GateGen;
  setVariable::BellowGenerator BellowGen;
  setVariable::MonoShutterGenerator MShutterGen;
  

  MShutterGen.generateShutter(Control,preName+"MonoShutter",0,0);  
  
  // bellows on shield block
  BellowGen.setCF<setVariable::CF40>();
  BellowGen.setAFlangeCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,preName+"BellowJ",0,10.0);    

    // joined and open
  GateGen.setCF<setVariable::CF40>();
  GateGen.generateValve(Control,preName+"GateJ",0.0,0);
  return;
}

void
opticsHutVariables(FuncDataBase& Control,
		   const std::string& preName)
  /*!
    Variable for the main optics hutch walls
    \param Control :: Database
    \param preName :: Beamline name
  */
{
  ELog::RegMethod RegA("cosaxsVariables[F]","opticsCaveVariables");

  const std::string hutName(preName+"OpticsHut");

  Control.addVariable(hutName+"Height",250.0);
  Control.addVariable(hutName+"Length",886.1);  // changed from 906
  Control.addVariable(hutName+"OutWidth",200.0);
  Control.addVariable(hutName+"RingWidth",75.0);
  Control.addVariable(hutName+"RingWallLen",80.0);
  Control.addVariable(hutName+"RingWallAngle",18.50);
  Control.addVariable(hutName+"RingConcThick",100.0);

  Control.addVariable(hutName+"InnerThick",0.3);
    
  Control.addVariable(hutName+"PbWallThick",2.0);
  Control.addVariable(hutName+"PbRoofThick",2.0);
  Control.addVariable(hutName+"PbBackThick",10.0);
  Control.addVariable(hutName+"PbFrontThick",2.0);

  Control.addVariable(hutName+"OuterThick",0.3);
    
  Control.addVariable(hutName+"InnerOutVoid",10.0);  // side wall for chicane
  Control.addVariable(hutName+"OuterOutVoid",10.0);
  
  Control.addVariable(hutName+"SkinMat","Stainless304");
  Control.addVariable(hutName+"RingMat","Concrete");
  Control.addVariable(hutName+"PbMat","Lead");

  Control.addVariable(hutName+"HoleXStep",0.0);
  Control.addVariable(hutName+"HoleZStep",1.0);
  Control.addVariable(hutName+"HoleRadius",9.0);

  Control.addVariable(hutName+"InletXStep",0.0);
  Control.addVariable(hutName+"InletZStep",0.0);
  Control.addVariable(hutName+"InletRadius",5.0);

  Control.addVariable(preName+"OpticsNChicane",1);
  PortChicaneGenerator PGen;
  PGen.generatePortChicane(Control,preName+"OpticsChicane0",0,0);

  return;
}

void
exptHutVariables(FuncDataBase& Control,const std::string& preName)
  /*!
    Variable for the main expt hutch walls
    \param Control :: Database
    \param preName :: Beamline name
  */
{
  ELog::RegMethod RegA("cosaxsVariables[F]","exptHutVariables");

  const std::string hutName(preName+"ExptHut");
  
  Control.addVariable(hutName+"YStep",1000.0);
  Control.addVariable(hutName+"Depth",120.0);
  Control.addVariable(hutName+"Height",200.0);
  Control.addVariable(hutName+"Length",858.4);
  Control.addVariable(hutName+"OutWidth",198.50);
  Control.addVariable(hutName+"RingWidth",248.6);
  Control.addVariable(hutName+"InnerThick",0.2);
  Control.addVariable(hutName+"PbThick",0.4);
  Control.addVariable(hutName+"OuterThick",0.2);

  Control.addVariable(hutName+"VoidMat","Void");
  Control.addVariable(hutName+"SkinMat","Stainless304");
  Control.addVariable(hutName+"PbMat","Lead");
  Control.addVariable(hutName+"FloorMat","Concrete");

  Control.addVariable(hutName+"HoleXStep",0.0);
  Control.addVariable(hutName+"HoleZStep",5.0);
  Control.addVariable(hutName+"HoleRadius",7.0);
  Control.addVariable(hutName+"HoleMat","Lead");

  return;
}


void
monoVariables(FuncDataBase& Control)
  /*!
    Set the variables for the mono
    \param Control :: DataBase to use
  */
{
  ELog::RegMethod RegA("cosaxsVariables[F]","monoVariables");
  const std::string preName("CosaxsOpticsLine");
  
  setVariable::MonoBoxGenerator VBoxGen;

  VBoxGen.setMat("Stainless304");
  VBoxGen.setWallThick(1.0);
  VBoxGen.setCF<CF63>();
  VBoxGen.setAPortCF<CF40>();
  VBoxGen.setPortLength(5.0,5.0); // La/Lb
  VBoxGen.setLids(3.0,1.0,1.0); // over/base/roof

  // ystep/width/height/depth/length
  // height+depth == 452mm  -- 110/ 342
  VBoxGen.generateBox(Control,preName+"MonoBox",0.0,77.2,11.0,34.20,95.1);

    // CRYSTALS:
  Control.addVariable(preName+"MonoXtalYAngle",90.0);
  Control.addVariable(preName+"MonoXtalZStep",-1.25);
  Control.addVariable(preName+"MonoXtalGap",4.0);
  Control.addVariable(preName+"MonoXtalTheta",10.0);
  Control.addVariable(preName+"MonoXtalPhiA",0.0);
  Control.addVariable(preName+"MonoXtalPhiA",0.0);
  Control.addVariable(preName+"MonoXtalWidth",10.0);
  Control.addVariable(preName+"MonoXtalLengthA",8.0);
  Control.addVariable(preName+"MonoXtalLengthB",12.0);
  Control.addVariable(preName+"MonoXtalThickA",4.0);
  Control.addVariable(preName+"MonoXtalThickB",3.0);
  Control.addVariable(preName+"MonoXtalBaseThick",5.0);
  Control.addVariable(preName+"MonoXtalBaseExtra",2.0);
  
  Control.addVariable(preName+"MonoXtalMat","Silicon80K");
  Control.addVariable(preName+"MonoXtalBaseMat","Copper");

  return;
}

void
mirrorBox(FuncDataBase& Control,const std::string& Name,
	  const std::string& Index)
  /*!
    Construct variables for the diagnostic units
    \param Control :: Database
    \param Name :: component name
    \param Index :: Index designator
  */
{
  ELog::RegMethod RegA("cosaxsVariables[F]","mirrorBox");
  
  setVariable::MonoBoxGenerator VBoxGen;
  setVariable::MirrorGenerator MirrGen;
  
  VBoxGen.setMat("Stainless304");
  VBoxGen.setWallThick(1.0);
  VBoxGen.setCF<CF63>();
  VBoxGen.setPortLength(5.0,5.0); // La/Lb
  VBoxGen.setLids(3.0,1.0,1.0); // over/base/roof

  // ystep/width/height/depth/length
  VBoxGen.generateBox(Control,Name+"MirrorBox"+Index,
		      0.0,53.1,23.6,29.5,124.0);

  // mirror in mirror box
  MirrGen.setPlate(28.0,1.0,9.0);  //guess
  MirrGen.generateMirror(Control,Name+"Mirror"+Index,
			 0.0, 0.0, 2.0, 0.0,0.0);
  return;
}

void
diagUnit(FuncDataBase& Control,const std::string& Name)
  /*!
    Construct variables for the diagnostic units
    \param Control :: Database
    \param Name :: component name
  */
{
  ELog::RegMethod RegA("cosaxsVariables[F]","diagUnit");


  const double DLength(55.0);         // diag length [checked]
  setVariable::PortTubeGenerator PTubeGen;
  setVariable::PortItemGenerator PItemGen;
  
  PTubeGen.setMat("Stainless304");

  // ports offset by 24.5mm in x direction
  // length 425+ 75 (a) 50 b
  PTubeGen.setCF<CF63>();
  PTubeGen.setBPortCF<CF40>();
  PTubeGen.setBFlangeCF<CF63>();
  PTubeGen.setPortLength(-5.0,-7.5);
  PTubeGen.setAPortOffset(2.45,0);
  PTubeGen.setBPortOffset(2.45,0);
  
  // ystep/radius length
  PTubeGen.generateTube(Control,Name,0.0,7.5,DLength);
  Control.addVariable(Name+"NPorts",7);

  const std::string portName=Name+"Port";
  const Geometry::Vec3D MidPt(0,0,0);
  const Geometry::Vec3D XVec(1,0,0);
  const Geometry::Vec3D ZVec(0,0,1);
  const Geometry::Vec3D PPos(0.0,DLength/4.0,0);

  PItemGen.setOuterVoid(1);
  PItemGen.setCF<setVariable::CF40>(2.0);
  PItemGen.generatePort(Control,portName+"0",-PPos,ZVec);
  PItemGen.setCF<setVariable::CF63>(4.0);
  PItemGen.generatePort(Control,portName+"1",MidPt,ZVec);
  PItemGen.generatePort(Control,portName+"2",PPos,ZVec);
  // view port
  PItemGen.setCF<setVariable::CF63>(8.0);
  PItemGen.generatePort(Control,portName+"3",
			Geometry::Vec3D(0,DLength/4.5,0),
			Geometry::Vec3D(-1,-1,0));

    //  flange for diamond filter view
  PItemGen.setCF<setVariable::CF40>(4.0);
  PItemGen.generatePort(Control,portName+"4",
			Geometry::Vec3D(0,0.3*DLength,0),XVec);
  PItemGen.generatePort(Control,portName+"5",
			Geometry::Vec3D(0,0.3*DLength,0),-XVec);

  // ion pump port
  PItemGen.setCF<setVariable::CF100>(7.5);
  PItemGen.generatePort(Control,portName+"6",MidPt,-ZVec);

  return;
}

void
diagUnit2(FuncDataBase& Control,const std::string& Name)
  /*!
    Construct variables for the small diagnostic units
    \param Control :: Database
    \param Name :: component name
  */
{
  ELog::RegMethod RegA("cosaxsVariables[F]","diagUnit");


  const double DLength(40.0);         // diag length [checked+5cm]
  setVariable::PortTubeGenerator PTubeGen;
  setVariable::PortItemGenerator PItemGen;
  
  PTubeGen.setMat("Stainless304");

  // ports offset by 24.5mm in x direction
  // length 425+ 75 (a) 50 b
  PTubeGen.setCF<CF63>();
  PTubeGen.setPortLength(-5.0,-5.0);
  // ystep/radius length
  PTubeGen.generateTube(Control,Name,0.0,7.5,DLength);
  Control.addVariable(Name+"NPorts",4);

  const std::string portName=Name+"Port";
  const Geometry::Vec3D MidPt(0,0,0);
  const Geometry::Vec3D XVec(1,0,0);
  const Geometry::Vec3D ZVec(0,0,1);
  const Geometry::Vec3D PPos(0.0,DLength/6.0,0);

  PItemGen.setOuterVoid(1);  // create boundary round flange
  PItemGen.setCF<setVariable::CF63>(5.0);
  PItemGen.generatePort(Control,portName+"0",-PPos,ZVec);
  PItemGen.setCF<setVariable::CF63>(5.0);
  PItemGen.generatePort(Control,portName+"1",MidPt,XVec);
  PItemGen.generatePort(Control,portName+"2",PPos,ZVec);
  // view port
  PItemGen.setCF<setVariable::CF63>(8.0);
  PItemGen.generatePort(Control,portName+"3",
			Geometry::Vec3D(0,DLength/5.0,0),
			Geometry::Vec3D(-1,-1,0));

    //  flange for diamond filter view
  PItemGen.setCF<setVariable::CF40>(4.0);
  PItemGen.generatePort(Control,portName+"4",
			Geometry::Vec3D(0,0.3*DLength,0),XVec);
  PItemGen.generatePort(Control,portName+"5",
			Geometry::Vec3D(0,0.3*DLength,0),-XVec);

  // ion pump port
  PItemGen.setCF<setVariable::CF100>(7.5);
  PItemGen.generatePort(Control,portName+"6",MidPt,-ZVec);

  JawFlangeGenerator JFlanGen;
  JFlanGen.generateFlange(Control,Name+"JawUnit0");
  JFlanGen.generateFlange(Control,Name+"JawUnit1");

  return;
}

  
void
opticsVariables(FuncDataBase& Control,
		const std::string& beamName)
  /*
    Vacuum optics components in the optics hutch
    \param Control :: Function data base
    \param beamName :: Name of beamline
  */
{
  ELog::RegMethod RegA("cosaxsVariables[F]","opticsVariables");

  const std::string preName(beamName+"OpticsLine");

  Control.addVariable(preName+"OuterLeft",70.0);
  Control.addVariable(preName+"OuterRight",50.0);
  Control.addVariable(preName+"OuterTop",60.0);

  setVariable::PipeGenerator PipeGen;
  setVariable::BellowGenerator BellowGen;
  setVariable::CrossGenerator CrossGen;
  setVariable::PortTubeGenerator PTubeGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::GateValveGenerator GateGen;
  setVariable::JawValveGenerator JawGen;
  setVariable::VacBoxGenerator VBoxGen;
  setVariable::FlangeMountGenerator FlangeGen;
  setVariable::BremCollGenerator BremGen;
  setVariable::JawFlangeGenerator JawFlangeGen;
  setVariable::DiffPumpGenerator DiffGen;

  PipeGen.setWindow(-2.0,0.0);   // no window

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,preName+"InitBellow",0,6.0);

  CrossGen.setPlates(0.5,2.0,2.0);  // wall/Top/base
  CrossGen.setPorts(-9.0,-9.0);     // len of ports (after main)
  CrossGen.generateDoubleCF<setVariable::CF40,setVariable::CF100>
    (Control,preName+"TriggerPipe",0.0,15.0,15.0);  // ystep/height/depth
  
  CrossGen.setPorts(1.2,1.2);     // len of ports (after main)
  CrossGen.generateDoubleCF<setVariable::CF40,setVariable::CF63>
    (Control,preName+"GaugeA",0.0,11.0,11.0);  // ystep/height/depth

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.setBFlangeCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,preName+"BellowA",0,17.6);

  GateGen.setLength(2.5);
  GateGen.setCF<setVariable::CF40>();
  GateGen.generateValve(Control,preName+"GateA",0.0,0);
  
  BremGen.setCF<CF63>();
  BremGen.generateColl(Control,preName+"BremCollA",0,5.4);

  PTubeGen.setMat("Stainless304");
  PTubeGen.setCF<CF63>();
  PTubeGen.setBPortCF<CF40>();
  PTubeGen.setPortLength(-6.0,-5.0);
  // ystep/radius length
  PTubeGen.generateTube(Control,preName+"FilterBoxA",0.0,7.5,25.0);
  Control.addVariable(preName+"FilterBoxANPorts",4);
  
  PItemGen.setCF<setVariable::CF40>(4.0);
  // 1/4 and 3/4 in main length: [total length 25.0-11.0] 
  Geometry::Vec3D PPos(0,3.5,0);
  const Geometry::Vec3D XVec(-1,0,0);
  const std::string portName=preName+"FilterBoxAPort";
  PItemGen.generatePort(Control,portName+"0",PPos,XVec);
  PItemGen.generatePort(Control,portName+"1",-PPos,XVec);

  // ion pump port
  PItemGen.setCF<setVariable::CF100>(7.5);
  PItemGen.generatePort(Control,portName+"2",
			Geometry::Vec3D(0,0,0),
			Geometry::Vec3D(0,0,-1));
  // Main flange for diamond filter
  PItemGen.setCF<setVariable::CF63>(5.0);
  PItemGen.generatePort(Control,portName+"3",
			Geometry::Vec3D(0,0,0),
			Geometry::Vec3D(0,0,1));

  FlangeGen.setCF<setVariable::CF63>();
  FlangeGen.setPlate(0.0,0.0,"Void");
  FlangeGen.setBlade(3.0,5.0,0.5,0.0,"Graphite",1);
  FlangeGen.generateMount(Control,preName+"FilterStick",1);  // in beam

  GateGen.setLength(2.5);
  GateGen.setCF<setVariable::CF40>();
  GateGen.generateValve(Control,preName+"GateB",0.0,0);

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,preName+"BellowB",0,12.0);

  SimpleTubeGen.setCF<CF40>();
  SimpleTubeGen.setBFlangeCF<CF63>();
  SimpleTubeGen.generateTube(Control,preName+"ScreenPipeA",0.0,12.5);
  Control.addVariable(preName+"ScreenPipeANPorts",1);
  PItemGen.setCF<setVariable::CF40>(4.0);
  PItemGen.generatePort(Control,preName+"ScreenPipeAPort0",
			Geometry::Vec3D(0,0,0),Geometry::Vec3D(1,0,0));

  
  SimpleTubeGen.setCF<CF63>();
  SimpleTubeGen.generateTube(Control,preName+"ScreenPipeB",0.0,14.0);
  Control.addVariable(preName+"ScreenPipeBNPorts",2);
  PItemGen.setCF<setVariable::CF63>(4.0);
  PItemGen.setOuterVoid(0);
  PItemGen.generatePort(Control,preName+"ScreenPipeBPort0",
			Geometry::Vec3D(0,0,0),Geometry::Vec3D(-1,0,0));
  PItemGen.generatePort(Control,preName+"ScreenPipeBPort1",
			Geometry::Vec3D(0,0,0),Geometry::Vec3D(0,0,-1));


  // Now add addaptor pipe:
  PipeGen.setMat("Stainless304");
  PipeGen.setCF<CF63>();
  PipeGen.setBFlangeCF<CF150>();
  PipeGen.generatePipe(Control,preName+"AdaptorPlateA",0.0,6.0);

  // length
  DiffGen.generatePump(Control,preName+"DiffPumpA",53.24);
  
  VBoxGen.setMat("Stainless304");
  VBoxGen.setWallThick(1.0);
  VBoxGen.setCF<CF63>();
  VBoxGen.setPortLength(2.5,2.5); // La/Lb
  // ystep/width/height/depth/length
  VBoxGen.generateBox(Control,preName+"PrimeJawBox",
		      0.0,30.0,15.0,15.0,53.15);

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.setAFlangeCF<setVariable::CF63>();
  BellowGen.setBFlangeCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,preName+"BellowC",0,12.0);
  
  GateGen.setCF<setVariable::CF63>();
  GateGen.generateValve(Control,preName+"GateC",0.0,0);

  cosaxsVar::monoVariables(Control);

  GateGen.setCF<setVariable::CF63>();
  GateGen.generateValve(Control,preName+"GateD",0.0,0);

  BellowGen.setCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,preName+"BellowD",0,18.0);

  cosaxsVar::diagUnit(Control,preName+"DiagBoxA");

  
  BellowGen.setCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,preName+"BellowE",0,12.0);

  GateGen.setCF<setVariable::CF63>();
  GateGen.generateValve(Control,preName+"GateE",0.0,0);
  
  cosaxsVar::mirrorBox(Control,preName,"A");

  GateGen.setCF<setVariable::CF63>();
  GateGen.generateValve(Control,preName+"GateF",0.0,0);

  BellowGen.setCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,preName+"BellowF",0,12.0);

  cosaxsVar::diagUnit2(Control,preName+"DiagBoxB");

  BellowGen.setCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,preName+"BellowG",0,12.0);

  GateGen.setCF<setVariable::CF63>();
  GateGen.generateValve(Control,preName+"GateG",0.0,0);

  cosaxsVar::mirrorBox(Control,preName,"B");

  GateGen.setCF<setVariable::CF63>();
  GateGen.generateValve(Control,preName+"GateH",0.0,0);

  BellowGen.setCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,preName+"BellowH",0,18.0);

  cosaxsVar::diagUnit2(Control,preName+"DiagBoxC");

  GateGen.setCF<setVariable::CF63>();
  GateGen.generateValve(Control,preName+"GateI",0.0,0);
  
  BellowGen.setCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,preName+"BellowI",0,18.0);

  cosaxsVar::monoShutterVariables(Control,preName);
  
  return;
}

void
exptVariables(FuncDataBase& Control,
		      const std::string& beamName)
  /*
    Components in the experimental hutch
    \param Control :: Function data base
    \param beamName :: Name of beamline
  */
{
  const std::string preName(beamName+"ExptLine");

  Control.addVariable(preName+"OuterLength",2300.0);
  Control.addVariable(preName+"OuterLeft",85.0);
  Control.addVariable(preName+"OuterRight",85.0);
  Control.addVariable(preName+"OuterTop",85.0);

  setVariable::BellowGenerator BellowGen;
  setVariable::GateValveGenerator GateGen;
  setVariable::JawValveGenerator JawGen;
  setVariable::PipeGenerator PipeGen;
  setVariable::MonoBoxGenerator VBoxGen;
  setVariable::DiffPumpGenerator DiffGen;


  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,preName+"InitBellow",0,6.0);

  // Gate valve A - round
  GateGen.setLength(2.5);
  GateGen.setCF<setVariable::CF40>();
  GateGen.generateValve(Control,preName+"GateA",0.0,0);

   // Double slits A and B
  JawGen.setCF<setVariable::CF100>();
  JawGen.setLength(4.0);
  JawGen.setSlits(3.0,2.0,0.2,"Tantalum");
  JawGen.generateSlits(Control,preName+"DoubleSlitA",0.0,0.8,0.8);

  JawGen.setCF<setVariable::CF100>();
  JawGen.setLength(4.0);
  JawGen.setSlits(3.0,2.0,0.2,"Tungsten");
  JawGen.generateSlits(Control,preName+"DoubleSlitB",0.0,0.8,0.8);

  VBoxGen.setMat("Stainless304");
  VBoxGen.setWallThick(1.0); // measured
  VBoxGen.setCF<CF63>();
  VBoxGen.setAPortCF<CF40>();
  VBoxGen.setPortLength(2.5,2.5); // La/Lb
  VBoxGen.setLids(3.5,1.5,1.5); // over/base/roof - all values are measured

  const std::string duName(preName+"DiagnosticUnit");

  // arguments: ystep/width/height/depth/length
  VBoxGen.generateBox(Control,duName,
		      0.0,22.0,8.5,8.5,43.0); // measured

  Control.addVariable(duName+"FilterHolder1YStep",8.2);
  Control.addVariable(duName+"FilterHolder1Thick",0.8); // measured
  Control.addVariable(duName+"FilterHolder1Height",1.8);
  Control.addVariable(duName+"FilterHolder1Depth",1.4);
  Control.addVariable(duName+"FilterHolder1Width",5.75);
  Control.addVariable(duName+"FilterHolder1Mat","Stainless304");
  Control.addVariable(duName+"FilterHolder1LegHeight",1.3);
  Control.addVariable(duName+"FilterHolder1LegWidth",1.5);
  Control.addVariable(duName+"FilterHolder1BaseHeight",1.0);
  Control.addVariable(duName+"FilterHolder1BaseWidth",6.5);
  Control.addVariable(duName+"FilterHolder1FoilThick",1.0); // arbitrary
  Control.addVariable(duName+"FilterHolder1FoilMat","Silicon300K"); // arbitrary
  Control.addVariable(duName+"FilterHolder1NWindows",5); // measured
  Control.addVariable(duName+"FilterHolder1WindowHeight",0.6); // measured
  Control.addVariable(duName+"FilterHolder1WindowDepth",0.6); // measured
  Control.addVariable(duName+"FilterHolder1WindowWidth",0.7); // measured

  Control.copyVarSet(duName+"FilterHolder1",duName+"FilterHolder2");
  Control.addVariable(duName+"FilterHolder2YStep",2.0);

  Control.copyVarSet(duName+"FilterHolder1",duName+"FilterHolder3");
  Control.addVariable(duName+"FilterHolder3YStep",2.0);

  // Gate valve B - flat
  GateGen.setLength(2.5);
  GateGen.setCF<setVariable::CF40>();
  GateGen.generateValve(Control,preName+"GateB",0.0,0);

  DiffGen.generatePump(Control,preName+"DiffPump",53.24);
  PipeGen.setCF<setVariable::CF40>();
  PipeGen.generatePipe(Control,preName+"TelescopicSystem",0,100.0);

  // sample area dimensions are arbitrary
  Control.addVariable(preName+"SampleAreaWidth",100.0);
  Control.addVariable(preName+"SampleAreaHeight",50.0);
  Control.addVariable(preName+"SampleAreaDepth",10.0);
  Control.addVariable(preName+"SampleAreaAirMat","Air");

  const std::string tubeName(preName+"Tube");

  // X032_CoSAXS_\(2019-02-11\)_dimensions.pdf:
  Control.addVariable(tubeName+"YStep", 453.748); // dummy

  const std::string noseName(tubeName+"NoseCone");
  
  Control.addVariable(noseName+"Length",35.0); // measured
  Control.addVariable(noseName+"MainMat","Void"); //
  Control.addVariable(noseName+"WallMat","Stainless304"); // ???
  Control.addVariable(noseName+"WallThick",1.0); // measured

  Control.addVariable(noseName+"FrontPlateWidth",12.0); // measured
  Control.addVariable(noseName+"FrontPlateHeight",12.0); // measured
  Control.addVariable(noseName+"FrontPlateThick",1.5); // measured

  Control.addVariable(noseName+"BackPlateWidth",38.0); // measured
  Control.addVariable(noseName+"BackPlateHeight",38.0); // measured
  Control.addVariable(noseName+"BackPlateThick",2.5); // measured
  Control.addVariable(noseName+"BackPlateRimThick",4.5); // measured

  Control.addVariable(noseName+"PipeRadius",4.0); // ??? guess
  Control.addVariable(noseName+"PipeLength",4.6); // measured
  Control.addVariable(noseName+"PipeWallThick",
		      static_cast<double>(setVariable::CF63::wallThick)); // guess ???
  Control.addVariable(noseName+"FlangeRadius",
		      static_cast<double>(setVariable::CF63::flangeRadius));
  Control.addVariable(noseName+"FlangeLength",2.6); // measured

  GateGen.setLength(10.0);
  GateGen.setCF<setVariable::CF40>();
  GateGen.generateValve(Control,tubeName+"GateA",0.0,0);
  Control.addVariable(tubeName+"GateARadius",17.0); // measured

  // [1] = x032_cosaxs_-2019-02-11-_dimensions.pdf
  // [2] = measured in X032_CoSAXS_(2019-04-25).step
  Control.addVariable(tubeName+"StartPlateThick", 2.7); // [1]
  Control.addVariable(tubeName+"StartPlateRadius", 57.8);  // [1], 1156/2.0 mm
  // According to [1], the PortRadius is 50.2/2 = 25.1 cm, but here we set it to
  // 14.27 cm - the port radius of the gasket plate betwen GateA and StartPlate
  // (which we do not build)
  Control.addVariable(tubeName+"StartPlatePortRadius", 14.27);
  Control.addVariable(tubeName+"StartPlateMat", "Stainless304");

  Control.addVariable(tubeName+"Segment1FlangeRadius", 57.8);
  Control.addVariable(tubeName+"Segment1FlangeLength", 4.3);

  Control.addVariable(tubeName+"Segment1Length", 167.2); // [2]
  Control.addVariable(tubeName+"Segment1Radius", 50.2); // [1] A-A
  Control.addVariable(tubeName+"Segment1WallThick", 0.6); // [2] A-A
  Control.addVariable(tubeName+"Segment1WallMat", "Stainless304");

  Control.addVariable(tubeName+"Segment1NPorts", 1);
  Control.addVariable(tubeName+"Segment1Port0Centre", "Vec3D(0,0.1,0)"); // [1]
  Control.addVariable(tubeName+"Segment1Port0Axis", "Vec3D(1,0,0)");
  Control.addVariable(tubeName+"Segment1Port0Length", 7.0); // dummy
  Control.addVariable(tubeName+"Segment1Port0Radius", 33.0); // [1, section C-C] 33 = 66.0/2
  Control.addParse<double>(tubeName+"Segment1Port0Wall",
			   tubeName+"Segment1WallThick");
  Control.addVariable(tubeName+"Segment1Port0FlangeRadius", 41.15); // [1, section C-C] 41.15 = 82.3/2
  Control.addVariable(tubeName+"Segment1Port0FlangeLength", 2.5); // [1, section C-C]
  Control.addVariable(tubeName+"Segment1PortCapThick", 2.5); // [1, section C-C]

  Control.copyVarSet(tubeName+"Segment1",tubeName+"Segment2");
  Control.addVariable(tubeName+"Segment2Length", 176.0); // [2]
  Control.addVariable(tubeName+"Segment2Port0Centre", "Vec3D(0,0,0)"); // [1]
  Control.addVariable(tubeName+"Segment2Port0Axis", "Vec3D(-1,0,0)");

  // segment 3: short without ports before the wall
  Control.copyVarSet(tubeName+"Segment1",tubeName+"Segment3");
  Control.addVariable(tubeName+"Segment3NPorts", 0);
  Control.addVariable(tubeName+"Segment3FlangeLength", 3.7); // [2]
  Control.addVariable(tubeName+"Segment3FlangeBRadius", 70.0); // [2]
  Control.addVariable(tubeName+"Segment3FlangeBLength", 1.0); // [2]
  Control.addVariable(tubeName+"Segment3Length", 32.8+1); // [2] 1 added to have distance 378.7 as in [1]

  // segment 4: longer with 2 ports right after the wall
  Control.copyVarSet(tubeName+"Segment1",tubeName+"Segment4");
  Control.addVariable(tubeName+"Segment4Length",238.2); // [2]
  Control.addVariable(tubeName+"Segment4NPorts", 2);
  Control.addParse<double>(tubeName+"Segment4FlangeARadius", tubeName+"Segment3FlangeBRadius");
  Control.addParse<double>(tubeName+"Segment4FlangeALength", tubeName+"Segment3FlangeBLength");

  Control.copyVarSet(tubeName+"Segment4Port0",tubeName+"Segment4Port1");
  Control.addVariable(tubeName+"Segment4Port0Centre", "Vec3D(0,38.2,0)"); // [1]
  Control.addVariable(tubeName+"Segment4Port1Axis", "Vec3D(-1,0,0)");
  Control.addVariable(tubeName+"Segment4Port1Centre", "Vec3D(0,-67.6,0)"); // [1]

  // segments 5-9 are the same
  Control.copyVarSet(tubeName+"Segment1",tubeName+"Segment5");
  Control.addVariable(tubeName+"Segment5Length",264.0); // [2]
  Control.addVariable(tubeName+"Segment5NPorts", 2);
  Control.copyVarSet(tubeName+"Segment5Port0",tubeName+"Segment5Port1");
  Control.addVariable(tubeName+"Segment5Port0Centre", "Vec3D(0,55.1,0)"); // [1]
  Control.addVariable(tubeName+"Segment5Port1Axis", "Vec3D(-1,0,0)");

  for (size_t i=6;i<=8;i++)
      Control.copyVarSet(tubeName+"Segment5",
			 tubeName+"Segment"+std::to_string(i));

  Control.addParse<double>(tubeName+"OuterRadius", tubeName+"Segment3FlangeBRadius+10");
  Control.addParse<double>(tubeName+"OuterLength",
			   "CosaxsExptLineTubeNoseConeLength+"
			   "CosaxsExptLineTubeSegment1Length+"
			   "CosaxsExptLineTubeSegment2Length+"
			   "CosaxsExptLineTubeSegment3Length+"
			   "CosaxsExptLineTubeSegment4Length+"
			   "CosaxsExptLineTubeSegment5Length+"
			   "CosaxsExptLineTubeSegment6Length+"
			   "CosaxsExptLineTubeSegment7Length+"
			   "CosaxsExptLineTubeSegment8Length+"
			   "100");
  Control.addVariable(tubeName+"Segment8FlangeLength", 4.0); // measured with ruler
  Control.addVariable(tubeName+"Segment8FlangeBCapThick", 2.7); // measured with ruler
  // adjust the tube length by the flange B length
  Control.addParse<double>(tubeName+"Segment8Length",tubeName+"Segment8Length+"+
			   tubeName+"Segment8FlangeBCapThick");
  Control.addVariable(tubeName+"Segment8NPorts", 4);
  Control.addVariable(tubeName+"Segment8Port2Length", 6.6); // [2]
  Control.addVariable(tubeName+"Segment8Port2Radius", 4.0); // [2]
  Control.addVariable(tubeName+"Segment8Port2Wall", 1.0); // [2] approx
  Control.addVariable(tubeName+"Segment8Port2FlangeRadius", 8.3); // [2] approx
  Control.addVariable(tubeName+"Segment8Port2FlangeLength", 2.0);
  Control.addVariable(tubeName+"Segment8Port2CapThick", 0.7); // [2] approx

  Control.addVariable(tubeName+"Segment8Port2Centre", "Vec3D(34.8,0,0)"); // measured with ruler
  Control.addVariable(tubeName+"Segment8Port2Axis", "Vec3D(0,1,0)");
  Control.copyVarSet(tubeName+"Segment8Port2", tubeName+"Segment8Port3");
  Control.addVariable(tubeName+"Segment8Port3Centre", "Vec3D(-34.8,0,0)"); // measured with ruler
  return;
}

void
connectingVariables(FuncDataBase& Control)
  /*!
    Variables for the connecting region
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("cosaxsVariables[F]","connectingVariables");
  const std::string baseName="CosaxsConnect";
  const Geometry::Vec3D OPos(0,0,0);
  const Geometry::Vec3D ZVec(0,0,-1);


  return;
}

}  // NAMESPACE cosaxsVAR
  
void
COSAXSvariables(FuncDataBase& Control)
  /*!
    Function to set the control variables and constants
    -- This version is for Photon Moderator
    \param Control :: Function data base to add constants too
  */
{
  ELog::RegMethod RegA("cosaxsVariables[F]","cosaxsVariables");


  Control.addVariable("sdefType","Wiggler");

  setVariable::PipeGenerator PipeGen;
  setVariable::LeadPipeGenerator LeadPipeGen;

  PipeGen.setWindow(-2.0,0.0);   // no window

  cosaxsVar::undulatorVariables(Control,"CosaxsFrontBeam");

  // ystep / dipole pipe / exit pipe
  setVariable::R3FrontEndVariables
    (Control,"CosaxsFrontBeam",310.0,724.0,40.0);  
  cosaxsVar::wallVariables(Control,"CosaxsWallLead");
  
  PipeGen.setMat("Stainless304");
  PipeGen.setCF<setVariable::CF40>(); // was 2cm (why?)
  PipeGen.generatePipe(Control,"CosaxsJoinPipe",0,126.0);

  cosaxsVar::opticsHutVariables(Control,"Cosaxs");
  cosaxsVar::opticsVariables(Control,"Cosaxs");
  cosaxsVar::exptHutVariables(Control,"Cosaxs");
  cosaxsVar::exptVariables(Control,"Cosaxs");

  PipeGen.generatePipe(Control,"CosaxsJoinPipeB",0,100.0);


  return;
}

}  // NAMESPACE setVariable
