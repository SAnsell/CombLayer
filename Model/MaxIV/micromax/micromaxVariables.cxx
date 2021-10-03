/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   micromax/micromaxVariables.cxx
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
#include "PipeGenerator.h"
#include "SplitPipeGenerator.h"
#include "BellowGenerator.h"
#include "BremBlockGenerator.h"
#include "LeadPipeGenerator.h"
#include "GateValveGenerator.h"
#include "JawValveGenerator.h"
#include "PipeTubeGenerator.h"
#include "PortTubeGenerator.h"
#include "PortItemGenerator.h"
#include "VacBoxGenerator.h"
#include "MonoBoxGenerator.h"
#include "MonoShutterGenerator.h"
#include "BremMonoCollGenerator.h"
#include "BremTubeGenerator.h"
#include "HPJawsGenerator.h"
#include "FlangeMountGenerator.h"
#include "MirrorGenerator.h"
#include "CollGenerator.h"
#include "SqrFMaskGenerator.h"
#include "PortChicaneGenerator.h"
#include "JawFlangeGenerator.h"
#include "BremCollGenerator.h"
#include "WallLeadGenerator.h"
#include "CLRTubeGenerator.h"
#include "TriggerGenerator.h"
#include "CylGateValveGenerator.h"
#include "BeamPairGenerator.h"
#include "IonGaugeGenerator.h"
#include "DCMTankGenerator.h"
#include "MonoBlockXstalsGenerator.h"
#include "MLMonoGenerator.h"
#include "ViewScreenGenerator.h"
#include "YagScreenGenerator.h"
#include "SixPortGenerator.h"
#include "BoxJawsGenerator.h"
#include "PipeShieldGenerator.h"
#include "ConnectorGenerator.h"
#include "FlangeDomeGenerator.h"
#include "TableGenerator.h"
#include "AreaDetectorGenerator.h"
#include "OpticsHutGenerator.h"

namespace setVariable
{

namespace micromaxVar
{

void undulatorVariables(FuncDataBase&,const std::string&);
void mirrorMonoPackage(FuncDataBase&,const std::string&);
void hdcmPackage(FuncDataBase&,const std::string&);
void diagPackage(FuncDataBase&,const std::string&);
void diag2Package(FuncDataBase&,const std::string&);
void diag3Package(FuncDataBase&,const std::string&);
void diag4Package(FuncDataBase&,const std::string&);
void mirrorBox(FuncDataBase&,const std::string&,const std::string&,
	       const double,const double);
void viewPackage(FuncDataBase&,const std::string&);
void detectorTubePackage(FuncDataBase&,const std::string&);

void opticsHutVariables(FuncDataBase&,const std::string&);
void exptHutVariables(FuncDataBase&,const std::string&,const double);
void opticsVariables(FuncDataBase&,const std::string&);
void exptVariables(FuncDataBase&,const std::string&);
void shieldVariables(FuncDataBase&,const std::string&);
  

void
undulatorVariables(FuncDataBase& Control,
		   const std::string& undKey)
  /*!
    Builds the variables for the collimator
    \param Control :: Database
    \param undKey :: prename
  */
{
  ELog::RegMethod RegA("micromaxVariables[F]","undulatorVariables");
  setVariable::PipeGenerator PipeGen;

  const double undulatorLen(400.0);
  PipeGen.setMat("Aluminium");
  PipeGen.setNoWindow();   // no window
  PipeGen.setCF<setVariable::CF63>();
  PipeGen.generatePipe(Control,undKey+"UPipe",undulatorLen);
  Control.addVariable(undKey+"UPipeYStep",-undulatorLen/2.0);

  Control.addVariable(undKey+"UPipeWidth",6.0);
  Control.addVariable(undKey+"UPipeHeight",0.6);
  Control.addVariable(undKey+"UPipeFeThick",0.2);

  // undulator  
  Control.addVariable(undKey+"UndulatorVGap",1.1);  // mininum 11mm
  Control.addVariable(undKey+"UndulatorLength",370.0);   // 46.2mm*30*2
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

  Control.addVariable(undKey+"UndulatorFlipX",1); 
  
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

  const double FM1dist(1172.60);
  const double FM2dist(1624.2);
  
    // collimator block
  FMaskGen.setCF<CF100>();
  FMaskGen.setFrontGap(3.99,1.97);  // 1033.8
  //  FMaskGen.setBackGap(0.71,0.71);
  // Approximated to get 1mrad x 1mrad
  FMaskGen.setMinAngleSize(10.0,FM1dist,1000.0,1000.0); 
  FMaskGen.setBackAngleSize(FM1dist, 1200.0,1100.0);     // Approximated to get 1mrad x 1mrad  
  FMaskGen.generateColl(Control,preName+"CollA",FM1dist,15.0);

  FMaskGen.setFrontGap(2.13,2.146);
  FMaskGen.setBackGap(0.756,0.432);
  // Approximated to get 100urad x 100urad @16m
  FMaskGen.setMinAngleSize(32.0,FM2dist, 100.0,100.0 );
  // Approximated to get 150urad x 150urad @16m
  FMaskGen.setBackAngleSize(FM2dist, 150.0,150.0 );   
  FMaskGen.generateColl(Control,preName+"CollB",FM2dist,40.0);

  // NOT PRESENT ::: 
  // FMaskGen.setFrontGap(0.84,0.582);
  // FMaskGen.setBackGap(0.750,0.357);

  // FMaskGen.setMinAngleSize(12.0,1600.0, 100.0, 100.0);
  // FMaskGen.generateColl(Control,preName+"CollC",17/2.0,17.0);


  return;
}

  
void
mirrorMonoPackage(FuncDataBase& Control,
		  const std::string& monoKey)
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
    (Control,monoKey+"MLMVessel",57.0,12.5,31.0,94.0);

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
hdcmPackage(FuncDataBase& Control,
	    const std::string& monoKey)
  /*!
    Builds the variables for the hdcm packge
    \param Control :: Database
    \param slitKey :: prename
  */
{
  ELog::RegMethod RegA("micromaxVariables[F]","hdcmPackage");

  setVariable::PortItemGenerator PItemGen;
  setVariable::DCMTankGenerator MBoxGen;
  setVariable::MonoBlockXstalsGenerator MXtalGen;
  
  // ystep/width/height/depth/length
  // 
  MBoxGen.setCF<CF40>();   // set ports
  MBoxGen.setPortLength(7.5,7.5); // La/Lb
  MBoxGen.setBPortOffset(-0.6,0.0);    // note -1mm from crystal offset
  // radius : Height / depth  [need heigh = 0]
  MBoxGen.generateBox(Control,monoKey+"MonoVessel",30.0,0.0,16.0);

  Control.addVariable(monoKey+"MonoVesselPortBXStep",-0.6);      // from primary

  const double outerRadius(30.5);
  const std::string portName=monoKey+"MonoVessel";
  Control.addVariable(portName+"NPorts",1);   // beam ports (lots!!)
  
  PItemGen.setCF<setVariable::CF63>(outerRadius+5.0);
  PItemGen.setWindowPlate(2.5,2.0,-0.8,"Stainless304","LeadGlass");
  PItemGen.generatePort(Control,portName+"Port0",
  			Geometry::Vec3D(0,5.0,-10.0),
  			Geometry::Vec3D(1,0,0));

  // crystals gap 7mm
  MXtalGen.generateXstal(Control,monoKey+"MBXstals",0.0,3.0);
  

  return;
}

void
diag3Package(FuncDataBase& Control,
	     const std::string& diagKey)
  /*!
    Builds the variables for the second diagnostice/slit packge
    \param Control :: Database
    \param diagKey :: prename
  */
{
  ELog::RegMethod RegA("micromaxVariables[F]","diag3Package");

  setVariable::PortItemGenerator PItemGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::CylGateValveGenerator GVGen;
  setVariable::BellowGenerator BellowGen;
  setVariable::ViewScreenGenerator VSGen;
  setVariable::YagScreenGenerator YagGen;
  setVariable::BremTubeGenerator BTGen;
  setVariable::HPJawsGenerator HPGen;
  setVariable::PipeGenerator PipeGen;
  setVariable::BremBlockGenerator MaskGen;
  
  PipeGen.setNoWindow();   // no window
  PipeGen.setCF<setVariable::CF40>();

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,diagKey+"BellowG",15.0);

  GVGen.generateGate(Control,diagKey+"GateTubeE",0);  // open

  VSGen.generateView(Control,diagKey+"ViewTube");
  YagGen.generateScreen(Control,diagKey+"YagScreen",1);  // in beam
  Control.addVariable(diagKey+"YagScreenYAngle",-45.0);

   // will be rotated vertical
  const std::string viewName=diagKey+"BremTubeB";
  SimpleTubeGen.setCF<CF150>();
  SimpleTubeGen.setCap(1,1);
  SimpleTubeGen.setBFlangeCF<CF150>();
  SimpleTubeGen.generateTube(Control,viewName,25.0);
  Control.addVariable(viewName+"NPorts",2);   // beam ports

  PItemGen.setCF<setVariable::CF100>(CF150::outerRadius+5.0);
  PItemGen.setPlate(0.0,"Void");  
  PItemGen.setOuterVoid(0);
  PItemGen.generatePort(Control,viewName+"Port0",
			Geometry::Vec3D(0,0,0),Geometry::Vec3D(0,0,1));

  PItemGen.setCF<setVariable::CF150>(CF150::outerRadius+10.0);
  PItemGen.setPlate(0.0,"Void");  
  PItemGen.generatePort(Control,viewName+"Port1",
			Geometry::Vec3D(0,0,0),Geometry::Vec3D(0,0,-1));

  MaskGen.setAperature(-1,1.0,1.0,1.0,1.0,1.0,1.0);
  MaskGen.generateBlock(Control,diagKey+"BremCollC",-4.0);
  Control.addVariable(diagKey+"BremCollCPreXAngle",90);
  HPGen.generateJaws(Control,diagKey+"HPJawsB",0.3,0.3); 


  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,diagKey+"BellowH",15.0);

  PipeGen.generatePipe(Control,diagKey+"PipeE",185.0);


  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,diagKey+"BellowI",15.0);

  return;
}

void
diag4Package(FuncDataBase& Control,
	     const std::string& diagKey)
  /*!
    Builds the variables for the second diagnostice/slit packge
    \param Control :: Database
    \param diagKey :: prename
  */
{
  ELog::RegMethod RegA("micromaxVariables[F]","diag4Package");

  setVariable::PortItemGenerator PItemGen;
  setVariable::CylGateValveGenerator GVGen;
  setVariable::ViewScreenGenerator VSGen;
  setVariable::BellowGenerator BellowGen;
  
  GVGen.generateGate(Control,diagKey+"GateTubeF",0);

  VSGen.setPortBCF<CF40>();
  VSGen.generateView(Control,diagKey+"ViewTubeB");

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,diagKey+"BellowJ",10.0);    
  
  return;
}

void
monoShutterVariables(FuncDataBase& Control,
		     const std::string& preName)
  /*!
    Construct Mono Shutter variables
    \param Control :: Database for variables
    \param preName :: Control system
   */
{
  ELog::RegMethod RegA("micromaxVariables","monoShutterVariables");

  setVariable::GateValveGenerator GateGen;
  setVariable::BellowGenerator BellowGen;
  setVariable::MonoShutterGenerator MShutterGen;
  setVariable::CylGateValveGenerator GVGen;
  setVariable::PipeGenerator PipeGen;

    // up / up (true)
  MShutterGen.generateShutter(Control,preName+"MonoShutter",1,1);  

  PipeGen.setMat("Stainless304");
  PipeGen.setNoWindow();
  PipeGen.setCF<setVariable::CF40>();
  PipeGen.setBFlangeCF<setVariable::CF63>(); 
  PipeGen.generatePipe(Control,preName+"MonoAdaptorA",7.5);
  PipeGen.setAFlangeCF<setVariable::CF63>();
  PipeGen.setBFlangeCF<setVariable::CF40>(); 
  PipeGen.generatePipe(Control,preName+"MonoAdaptorB",7.5);

  // bellows on shield block
  BellowGen.setCF<setVariable::CF40>();
  BellowGen.setAFlangeCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,preName+"BellowL",10.0);    


  PipeGen.setCF<setVariable::CF40>();
  PipeGen.generatePipe(Control,preName+"PipeF",85.0);  
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
  ELog::RegMethod RegA("micromaxVariables","opticsHutVariables");
  
  OpticsHutGenerator OGen; 
  OGen.generateHut(Control,hutName,1256.0);

  Control.addVariable(hutName+"NChicane",2);
  PortChicaneGenerator PGen;
  PGen.setSize(4.0,60.0,40.0);
  PGen.generatePortChicane(Control,hutName+"Chicane0",170.0,-25.0);
  PGen.generatePortChicane(Control,hutName+"Chicane1",270.0,-25.0);
  
  return;
}

void
exptHutVariables(FuncDataBase& Control,
		 const std::string& beamName,
		 const double beamXStep)
  /*!
    Optics hut variables
    \param Control :: DataBase to add
    \param beamName :: Beamline name
    \param bremXStep :: Offset of beam from main centre line
  */
{
  ELog::RegMethod RegA("micromaxVariables[F]","exptHutVariables");

  const double beamOffset(-0.6);
    
  const std::string hutName(beamName+"ExptHut");
  
  Control.addVariable(hutName+"YStep",0.0);
  Control.addVariable(hutName+"Height",200.0);
  Control.addVariable(hutName+"Length",1719.4);
  Control.addVariable(hutName+"OutWidth",198.50);
  Control.addVariable(hutName+"RingWidth",248.6);
  Control.addVariable(hutName+"InnerThick",0.1);
  Control.addVariable(hutName+"PbBackThick",0.6);
  Control.addVariable(hutName+"PbRoofThick",0.4);
  Control.addVariable(hutName+"PbWallThick",0.4);
  Control.addVariable(hutName+"OuterThick",0.1);

  Control.addVariable(hutName+"CornerLength",1719.4-120.0);  //
  Control.addVariable(hutName+"CornerAngle",45.0);
  
  Control.addVariable(hutName+"InnerOutVoid",10.0);
  Control.addVariable(hutName+"OuterOutVoid",10.0);

  Control.addVariable(hutName+"VoidMat","Void");
  Control.addVariable(hutName+"SkinMat","Stainless304");
  Control.addVariable(hutName+"PbMat","Lead");

  Control.addVariable(hutName+"HoleXStep",beamXStep-beamOffset);
  Control.addVariable(hutName+"HoleZStep",0.0);
  Control.addVariable(hutName+"HoleRadius",3.0);
  Control.addVariable(hutName+"HoleMat","Void");

  // lead shield on pipe
  Control.addVariable(hutName+"PShieldXStep",beamXStep-beamOffset);
  Control.addVariable(hutName+"PShieldYStep",0.3);
  Control.addVariable(hutName+"PShieldLength",1.0);
  Control.addVariable(hutName+"PShieldWidth",10.0);
  Control.addVariable(hutName+"PShieldHeight",10.0);
  Control.addVariable(hutName+"PShieldWallThick",0.2);
  Control.addVariable(hutName+"PShieldClearGap",0.3);
  Control.addVariable(hutName+"PShieldWallMat","Stainless304");
  Control.addVariable(hutName+"PShieldMat","Lead");

  Control.addVariable(hutName+"NChicane",2);
  PortChicaneGenerator PGen;
  PGen.setSize(4.0,40.0,30.0);
  PGen.generatePortChicane(Control,hutName+"Chicane0","Left",150.0,-5.0);
  PGen.generatePortChicane(Control,hutName+"Chicane1","Left",-270.0,-5.0);
  /*
  PGen.generatePortChicane(Control,hutName+"Chicane1",370.0,-25.0);
  PGen.generatePortChicane(Control,hutName+"Chicane2",-70.0,-25.0);
  PGen.generatePortChicane(Control,hutName+"Chicane3",-280.0,-25.0);
  */

  return;
}

void
shieldVariables(FuncDataBase& Control,
		const std::string& preName)
  /*!
    Shield variables
    \param Control :: DataBase to add
  */
{
  ELog::RegMethod RegA("micromaxVariables","shieldVariables");
  
  Control.addVariable(preName+"PShieldLength",5.0);
  Control.addVariable(preName+"PShieldWidth",60.0);
  Control.addVariable(preName+"PShieldHeight",60.0);
  Control.addVariable(preName+"PShieldWallThick",0.5);
  Control.addVariable(preName+"PShieldClearGap",0.3);
  Control.addVariable(preName+"PShieldWallMat","Stainless304");
  Control.addVariable(preName+"PShieldMat","Lead");

  return;
}
  

void
mirrorBox(FuncDataBase& Control,
	  const std::string& Name,
	  const std::string& Index,
	  const std::string& vertFlag,
	  const double theta,const double phi)
  /*!
    Construct variables for the diagnostic units
    \param Control :: Database
    \param Name :: component name
    \param Index :: Index designator for mirror box (A/B etc)
    \param theta :: theta angle [beam angle in deg]
    \param phi :: phi angle [rotation angle in deg]
  */
{
  ELog::RegMethod RegA("micromaxVariables[F]","mirrorBox");
  
  setVariable::MonoBoxGenerator VBoxGen;
  setVariable::MirrorGenerator MirrGen;

  const double normialAngle=0.2; 
  const double vAngle=(vertFlag[0]=='H') ? 90 : 0.0;
  const double centreDist(55.0);
  const double heightNormDelta=sin(2.0*normialAngle*M_PI/180.0)*centreDist;
  const double heightDelta=sin(2.0*theta*M_PI/180.0)*centreDist;

  if (vAngle>45)
    VBoxGen.setBPortOffset(heightNormDelta,0.0);
  else
    VBoxGen.setBPortOffset(0.0,heightNormDelta);
  
  VBoxGen.setMat("Stainless304");
  VBoxGen.setWallThick(1.0);
  VBoxGen.setCF<CF63>();
  VBoxGen.setPortLength(5.0,5.0); // La/Lb
  VBoxGen.setLids(3.0,1.0,1.0); // over/base/roof

  // width/height/depth/length
  VBoxGen.generateBox(Control,Name+"MirrorBox"+Index,
		      53.1,23.6,29.5,168.0);

  // length thick width
  MirrGen.setPlate(50.0,1.0,9.0);  //guess  
  MirrGen.setPrimaryAngle(0,vAngle,0);  
  // ystep : zstep : theta : phi : radius
  MirrGen.generateMirror(Control,Name+"MirrorFront"+Index,
			 0.0,-centreDist/2.0,0.0,theta,phi,0.0);   // hits beam center
  MirrGen.setPrimaryAngle(0,vAngle+180.0,0.0);
  // x/y/z/theta/phi/
  MirrGen.generateMirror(Control,Name+"MirrorBack"+Index,
			 0.0,centreDist/2.0,heightDelta,theta,phi,0.0);
  return;
}

void
diag2Package(FuncDataBase& Control,const std::string& Name)
  /*!
    Construct variables for the diagnostic units
    \param Control :: Database
    \param Name :: component name
  */
{
  ELog::RegMethod RegA("micromaxVariables[F]","diag2Package");

  setVariable::PortItemGenerator PItemGen;
  setVariable::BremTubeGenerator BTGen;
  setVariable::HPJawsGenerator HPGen;
  setVariable::BremBlockGenerator MaskGen;

  BTGen.generateTube(Control,Name+"MonoBremTube");

  MaskGen.setAperature(-1,1.0,1.0,1.0,1.0,1.0,1.0);
  MaskGen.generateBlock(Control,Name+"BremCollB",-4.0);

  HPGen.generateJaws(Control,Name+"HPJawsA",0.3,0.3);

  const std::string portName=Name+"MonoBremTube";

  Control.addVariable(portName+"FrontNPorts",1);   // beam ports 
  PItemGen.setCF<setVariable::CF63>(CF63::outerRadius+9.1);
  PItemGen.setPlate(2.0,"Stainless304");  
  PItemGen.setOuterVoid(0);
  PItemGen.generatePort(Control,portName+"FrontPort0",
			Geometry::Vec3D(0,-11.5,0),
			Geometry::Vec3D(-1,0,0));
  

  
  return;
}

void
diagPackage(FuncDataBase& Control,const std::string& Name)
  /*!
    Construct variables for the diagnostic units
    \param Control :: Database
    \param Name :: component name
  */
{
  ELog::RegMethod RegA("micromaxVariables[F]","diagPackage");


  const double DLength(65.0);         // diag length [checked]
  setVariable::BellowGenerator BellowGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::PortTubeGenerator PTubeGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::JawValveGenerator JawGen;
  setVariable::BeamPairGenerator BeamMGen;
  setVariable::TableGenerator TableGen;

    
  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,Name+"BellowC",7.50);
  
  const std::string bremName(Name+"BremBlockTube");
  SimpleTubeGen.setCF<CF150>();
  SimpleTubeGen.setCap(1,1);
  SimpleTubeGen.generateTube(Control,bremName,25.0);
  Control.addVariable(bremName+"NPorts",2);   // beam ports

  PItemGen.setCF<setVariable::CF40>(CF150::outerRadius+2.2);
  PItemGen.setPlate(0.0,"Void");  
  PItemGen.setOuterVoid(0);
  PItemGen.generatePort(Control,bremName+"Port0",
			Geometry::Vec3D(0,0,0),Geometry::Vec3D(0,0,1));

  PItemGen.setCF<setVariable::CF40>(CF150::outerRadius+2.2);
  PItemGen.setPlate(0.0,"Void");  
  PItemGen.generatePort(Control,bremName+"Port1",
			Geometry::Vec3D(0,0,0),Geometry::Vec3D(0,0,-1));


  // View tube
  const std::string viewName(Name+"ViewTube");
  
  SimpleTubeGen.setCF<CF100>();
  SimpleTubeGen.setCap(1,1);
  SimpleTubeGen.generateTube(Control,viewName,44.0);
  Control.addVariable(viewName+"NPorts",2);   // beam ports

  PItemGen.setCF<setVariable::CF40>(12.5);  // include outerRadius
  PItemGen.setPlate(0.0,"Void");  
  PItemGen.generatePort(Control,viewName+"Port0",
			Geometry::Vec3D(0,-6,0),Geometry::Vec3D(0,0,-1));
  PItemGen.generatePort(Control,viewName+"Port1",
			Geometry::Vec3D(0,-6,0),Geometry::Vec3D(0,0,1));

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,Name+"BellowD",7.50);
  
  const std::string attnTubeName(Name+"AttnTube");
  PTubeGen.setPipe(9.0,0.5);
  PTubeGen.setPortCF<CF40>();
  PTubeGen.setPortLength(-3.0,-3.0);
  PTubeGen.generateTube(Control,attnTubeName,0.0,30.0);
  Control.addVariable(attnTubeName+"NPorts",3);   // beam ports

  PItemGen.setCF<setVariable::CF150>(CF150::outerRadius+22.5);
  PItemGen.setPlate(0.0,"Void");  
  PItemGen.generatePort(Control,attnTubeName+"Port0",
			Geometry::Vec3D(0,0,0),Geometry::Vec3D(0,0,-1));
  PItemGen.setCF<setVariable::CF150>(CF150::outerRadius+12.5);
  PItemGen.generatePort(Control,attnTubeName+"Port1",
			Geometry::Vec3D(0,0,0),Geometry::Vec3D(0,0,1));
  PItemGen.setCF<setVariable::CF40>(CF150::outerRadius+6.5);

  PItemGen.setOuterVoid(1);
  PItemGen.generateAnglePort(Control,attnTubeName+"Port2",
			     Geometry::Vec3D(0,0,0),
			     Geometry::Vec3D(1,0,0),
			     Geometry::Vec3D(0,0,1),
			     10.0);

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,Name+"BellowE",7.50);

  TableGen.generateTable(Control,Name+"TableA",-20,120.0);
  return;
}


void
monoVariables(FuncDataBase& Control,
	      const std::string preName)
  /*!
    Set the variables for the mono
    \param Control :: DataBase to use
  */
{
  ELog::RegMethod RegA("micromaxVariables[F]","monoVariables");

  
  setVariable::MonoBoxGenerator VBoxGen;

  VBoxGen.setMat("Stainless304");
  VBoxGen.setWallThick(1.0);
  VBoxGen.setCF<CF63>();
  VBoxGen.setAPortCF<CF40>();
  VBoxGen.setPortLength(5.0,5.0); // La/Lb
  VBoxGen.setLids(3.0,1.0,1.0); // over/base/roof

  VBoxGen.setBPortOffset(2.5,0.0);
  // ystep/width/height/depth/length
  // height+depth == 452mm  -- 110/ 342
  VBoxGen.generateBox(Control,preName+"MonoBox",77.2,11.0,34.20,95.1);

    // CRYSTALS:

  Control.addVariable(preName+"MonoXtalYAngle",90.0);
  Control.addVariable(preName+"MonoXtalZStep",0.0);
  Control.addVariable(preName+"MonoXtalGap",2.5);
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
  ELog::RegMethod RegA("micromaxVariables[F]","opticsVariables");

  const std::string preName(beamName+"OpticsLine");

  Control.addVariable(preName+"OuterLeft",70.0);
  Control.addVariable(preName+"OuterRight",50.0);
  Control.addVariable(preName+"OuterTop",60.0);

  setVariable::PipeGenerator PipeGen;
  setVariable::BellowGenerator BellowGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::GateValveGenerator GateGen;
  setVariable::VacBoxGenerator VBoxGen;
  setVariable::FlangeMountGenerator FlangeGen;
  setVariable::BremCollGenerator BremGen;
  setVariable::BremMonoCollGenerator BremMonoGen;
  setVariable::JawFlangeGenerator JawFlangeGen;
  setVariable::CLRTubeGenerator DiffGen;
  setVariable::TriggerGenerator TGen;
  setVariable::CylGateValveGenerator GVGen;
  setVariable::SqrFMaskGenerator FMaskGen;
  setVariable::IonGaugeGenerator IGGen;
    
  PipeGen.setNoWindow();   // no window

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,preName+"InitBellow",6.0);

  // will be rotated vertical
  TGen.setCF<CF100>();
  TGen.setVertical(15.0,25.0);
  TGen.setSideCF<setVariable::CF40>(10.0); // add centre distance?
  TGen.generateTube(Control,preName+"TriggerUnit");

  GVGen.generateGate(Control,preName+"GateTubeA",0);  // open
  
  PipeGen.setMat("Stainless304");
  PipeGen.setCF<CF40>();
  PipeGen.generatePipe(Control,preName+"PipeA",20.0);

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,preName+"BellowA",17.6);

  const double FM2dist(1624.2);
  FMaskGen.setCF<CF63>();
  FMaskGen.setFrontGap(2.13,2.146);
  FMaskGen.setBackGap(0.756,0.432);
  FMaskGen.setMinAngleSize(10.0,FM2dist, 100.0,100.0 );
  // step to +7.5 to make join with fixedComp:linkpt
  FMaskGen.generateColl(Control,preName+"BremCollA",7.5,15.0);

  IGGen.generateTube(Control,preName+"IonGaugeA");
 
  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,preName+"BellowB",7.50);

  PipeGen.generatePipe(Control,preName+"BremPipeA",5.0);

  micromaxVar::diagPackage(Control,preName);

  micromaxVar::hdcmPackage(Control,preName);

  micromaxVar::diag2Package(Control,preName);
  /*
  PipeGen.setCF<CF40>();  
  PipeGen.generatePipe(Control,preName+"PipeB",7.5);
  GVGen.generateGate(Control,preName+"GateTubeB",0);  // open
  BellowGen.generateBellow(Control,preName+"BellowC",15.0);

  micromaxVar::mirrorMonoPackage(Control,preName);
  
  BellowGen.generateBellow(Control,preName+"BellowD",15.0);
  PipeGen.generatePipe(Control,preName+"PipeC",50.0);  
  GVGen.generateGate(Control,preName+"GateTubeC",0);  // open
  BellowGen.generateBellow(Control,preName+"BellowE",15.0);


  
  BellowGen.generateBellow(Control,preName+"BellowF",15.0);
  PipeGen.generatePipe(Control,preName+"PipeD",12.5);  
  GVGen.generateGate(Control,preName+"GateTubeD",0);  // open

  
  micromaxVar::diag2Package(Control,preName);

  micromaxVar::mirrorBox(Control,preName,"A","Horrizontal",-0.2,0.0);

  micromaxVar::diag3Package(Control,preName);

  micromaxVar::diag4Package(Control,preName);
  
  micromaxVar::monoShutterVariables(Control,preName);
  */
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
  ELog::RegMethod RegA("micromaxVariables[F]","viewPackage");

  setVariable::PipeTubeGenerator SimpleTubeGen;  
  setVariable::PortItemGenerator PItemGen;
  setVariable::BellowGenerator BellowGen;
  setVariable::GateValveGenerator GateGen;
  setVariable::FlangeMountGenerator FlangeGen;
  
  // will be rotated vertical
  const std::string pipeName=viewKey+"ViewTube";
  SimpleTubeGen.setCF<CF40>();
  // up 15cm / 32.5cm down : Measured
  SimpleTubeGen.generateTube(Control,pipeName,21.0);


  Control.addVariable(pipeName+"NPorts",4);   // beam ports (lots!!)

  PItemGen.setCF<setVariable::CF40>(CF40::outerRadius+5.0);
  PItemGen.setPlate(0.0,"Void");  
  PItemGen.generatePort(Control,pipeName+"Port0",
			Geometry::Vec3D(0,0,0),
			Geometry::Vec3D(0,0,1));
  PItemGen.generatePort(Control,pipeName+"Port1",
			Geometry::Vec3D(0,0,0),
			Geometry::Vec3D(0,0,-1));
  PItemGen.setPlate(0.0,"Stainless304");
  
  PItemGen.setCF<setVariable::CF40>(sqrt(2.0)*CF40::outerRadius+12.0);
  PItemGen.generatePort(Control,pipeName+"Port2",
			Geometry::Vec3D(0,0,0),
			Geometry::Vec3D(-1,-1,0));

  PItemGen.generatePort(Control,pipeName+"Port3",
			Geometry::Vec3D(0,0,0),
			Geometry::Vec3D(1,0,0));

  FlangeGen.setNoPlate();
  FlangeGen.setBlade(2.0,2.0,0.3,-45.0,"Graphite",1);  
  FlangeGen.generateMount(Control,viewKey+"ViewTubeScreen",1);  // in beam

  return;
}

void
detectorTubePackage(FuncDataBase& Control,
		    const std::string& beamName)
  /*!
    Builds the variables for the main detector tube.
    \param Control :: Database
    \param viewKey :: prename including view
  */
{
  ELog::RegMethod RegA("micromaxVariables[F]","detectorTubePackage");  


  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::FlangeDomeGenerator FDGen;
  setVariable::AreaDetectorGenerator WAXSGen;
  
  SimpleTubeGen.setCF<CF350>();
  
  const std::string tubeName(beamName+"DetectorTube");
  Control.addVariable(tubeName+"YStep", 800.748); // dummy
  Control.addVariable(tubeName+"OuterRadius",60.0);
  Control.addVariable(tubeName+"OuterMat","Void"); 
  // MAIN PIPE:

  for(size_t i=0;i<8;i++)
    {
      const std::string segName=tubeName+"Segment"+std::to_string(i);
      SimpleTubeGen.generateTube(Control,segName,111.0);
      Control.addVariable(segName+"NPorts",0);   // beam ports
    }


  PItemGen.setCF<setVariable::CF63>(10.0);
  PItemGen.setNoPlate();
  FDGen.generateDome(Control,tubeName+"FrontDome");
  Control.addVariable(tubeName+"FrontDomeNPorts",1);
  PItemGen.generatePort(Control,tubeName+"FrontDomePort0",
			Geometry::Vec3D(0.0, 0.0, 0.0),
			Geometry::Vec3D(0,1,0));

  PItemGen.setCF<setVariable::CF63>(10.0);
  PItemGen.setWindowPlate(CF63::flangeLength,0.8*CF63::flangeLength,
			  CF63::innerRadius*1.1,"Stainless304","SiO2");
  
  FDGen.generateDome(Control,tubeName+"BackDome");
  Control.addVariable(tubeName+"BackDomeNPorts",2);
  PItemGen.generatePort(Control,tubeName+"BackDomePort0",
			Geometry::Vec3D(-8.0, 0.0, 0.0),
			Geometry::Vec3D(0,1,0));
  PItemGen.generatePort(Control,tubeName+"BackDomePort1",
			Geometry::Vec3D(8.0, 0.0, 0.0),
			Geometry::Vec3D(0,1,0));

  Control.addVariable(tubeName+"BeamStopYStep", 500.0); // [2]
  Control.addVariable(tubeName+"BeamStop", 0.6); // [2]
  Control.addVariable(tubeName+"BeamStopLength", 0.6); // [2]
  Control.addVariable(tubeName+"BeamStopRadius", 0.15); // [2]
  Control.addVariable(tubeName+"BeamStopMat", "Tantalum");

  WAXSGen.generateDetector(Control,tubeName+"WAXS",600.0);
  
  return;
}
  

void
exptVariables(FuncDataBase& Control,
	      const std::string& beamName)
/*
    Vacuum expt components in the optics hutch
    \param Control :: Function data base
    \param beamName :: Name of beamline
  */
{
  ELog::RegMethod RegA("micromaxVariables[F]","exptVariables");

  setVariable::BellowGenerator BellowGen;
  setVariable::SixPortGenerator CrossGen;
  setVariable::MonoBoxGenerator VBoxGen;
  setVariable::BoxJawsGenerator BJGen;
  setVariable::ConnectorGenerator CTGen;
  setVariable::CLRTubeGenerator DPGen;
  setVariable::PipeGenerator PipeGen;
  setVariable::SixPortGenerator SixGen;

  PipeGen.setNoWindow();
  PipeGen.setMat("Stainless304");
  PipeGen.setCF<CF40>();
  
  const std::string preName(beamName+"ExptLine");

  Control.addVariable(preName+"OuterLeft",50.0);
  Control.addVariable(preName+"OuterRight",50.0);
  Control.addVariable(preName+"OuterTop",60.0);
  Control.addVariable(preName+"OuterMat","Void");

  BellowGen.setCF<setVariable::CF40>();

  BellowGen.generateBellow(Control,preName+"BellowA",15.0);

  VBoxGen.setMat("Stainless304");
  VBoxGen.setCF<CF40>();
  VBoxGen.setPortLength(3.5,3.5); // La/Lb
  VBoxGen.setLids(3.5,1.5,1.5); // over/base/roof - all values are measured
  VBoxGen.generateBox(Control,preName+"FilterBoxA",5.8,4.5,4.5,20.0);

  BellowGen.generateBellow(Control,preName+"BellowB",7.5);

  CrossGen.setCF<setVariable::CF40>();
  CrossGen.setLength(7.0,7.0);
  CrossGen.setSideLength(6.5);
  CrossGen.generateSixPort(Control,preName+"CrossA");

  BellowGen.generateBellow(Control,preName+"BellowC",7.5);

  BJGen.generateJaws(Control,preName+"JawBox",0.8,0.8);

  CTGen.generatePipe(Control,preName+"ConnectA",20.0);

  DPGen.generatePump(Control,preName+"CLRTubeA",1);

  CTGen.generatePipe(Control,preName+"ConnectB",20.0);

  PipeGen.generatePipe(Control,preName+"PipeA",12.5);

  SixGen.setCF<CF40>();
  SixGen.setLength(6.0,6.0);
  SixGen.setSideLength(5.0);
  
  SixGen.generateSixPort(Control,preName+"SixPortA");

  
  setVariable::GateValveGenerator CGateGen;
  CGateGen.setBladeMat("Aluminium");
  CGateGen.setBladeThick(0.8);
  CGateGen.setLength(1.2);
  CGateGen.setCylCF<CF40>();
  CGateGen.generateValve(Control,preName+"CylGateA",0.0,0);

  PipeGen.generatePipe(Control,preName+"PipeB",118.0);

  BellowGen.generateBellow(Control,preName+"BellowD",15.0);
  
  SixGen.generateSixPort(Control,preName+"SixPortB");

  PipeGen.generatePipe(Control,preName+"PipeC",118.0);

  CGateGen.generateValve(Control,preName+"CylGateB",0.0,0);

  SixGen.generateSixPort(Control,preName+"SixPortC");
  
  PipeGen.generatePipe(Control,preName+"PipeD",12.5);

  CTGen.generatePipe(Control,preName+"ConnectC",20.0);

  DPGen.generatePump(Control,preName+"CLRTubeB",1);

  CTGen.generatePipe(Control,preName+"ConnectD",20.0);

  viewPackage(Control,preName);

  BellowGen.generateBellow(Control,preName+"BellowE",15.0);

  CrossGen.generateSixPort(Control,preName+"CrossB");

  PipeGen.generatePipe(Control,preName+"AdjustPipe",73.0);

  PipeGen.generatePipe(Control,preName+"PipeE",7.5);

  BJGen.generateJaws(Control,preName+"JawBoxB",0.8,0.8);

  CTGen.setOuter(2.5,0.5);
  CTGen.setPort(CF40::flangeRadius,0.5*CF40::flangeLength);
  CTGen.generatePipe(Control,preName+"ConnectE",5.0);

  PipeGen.setCF<CF16>();
  PipeGen.setAFlangeCF<CF40>();
  PipeGen.setBFlange(1.1,0.1);
  PipeGen.generatePipe(Control,preName+"EndPipe",61.0);

  Control.addVariable(preName+"SampleYStep", 25.0); // [2]
  Control.addVariable(preName+"SampleRadius", 5.0); // [2]
  Control.addVariable(preName+"SampleDefMat", "Stainless304");

  
  return;
}


}  // NAMESPACE micromaxVar
  
void
MICROMAXvariables(FuncDataBase& Control)
  /*!
    Function to set the control variables and constants
    -- This version is for Photon Moderator
    \param Control :: Function data base to add constants too
  */
{
  ELog::RegMethod RegA("micromaxVariables[F]","MICROMAXvariables");

  Control.addVariable("sdefType","Wiggler");
  
  setVariable::PipeGenerator PipeGen;
  setVariable::LeadPipeGenerator LeadPipeGen;
  PipeGen.setNoWindow();
  PipeGen.setMat("Aluminium");

  const std::string frontKey("MicroMaxFrontBeam");

  micromaxVar::undulatorVariables(Control,frontKey);
  setVariable::R3FrontEndVariables(Control,"MicroMax");
  micromaxVar::frontMaskVariables(Control,"MicroMaxFrontBeam");
    
  
  PipeGen.setMat("Stainless304");
  PipeGen.setCF<setVariable::CF40>(); 
  PipeGen.generatePipe(Control,"MicroMaxJoinPipe",150.0);
  Control.addVariable("MicroMaxJoinPipeFlipX",1);
    
  micromaxVar::opticsHutVariables(Control,"MicroMaxOpticsHut");
  micromaxVar::opticsVariables(Control,"MicroMax");

  PipeGen.setCF<setVariable::CF40>(); 
  PipeGen.generatePipe(Control,"MicroMaxJoinPipeB",20.0);

  micromaxVar::shieldVariables(Control,"MicroMax");
  
  micromaxVar::exptHutVariables(Control,"MicroMax",0.0);
  micromaxVar::exptVariables(Control,"MicroMax");
  micromaxVar::detectorTubePackage(Control,"MicroMax");



  return;
}

}  // NAMESPACE setVariable
