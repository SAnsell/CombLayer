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
#include "BeamScrapperGenerator.h"
#include "LeadPipeGenerator.h"
#include "GateValveGenerator.h"
#include "JawValveGenerator.h"
#include "PipeTubeGenerator.h"
#include "PortTubeGenerator.h"
#include "PortItemGenerator.h"
#include "VacBoxGenerator.h"
#include "MonoBoxGenerator.h"
#include "MonoShutterGenerator.h"
#include "RoundShutterGenerator.h"
#include "BremMonoCollGenerator.h"
#include "BremTubeGenerator.h"
#include "HPJawsGenerator.h"
#include "FlangeMountGenerator.h"
#include "FlangePlateGenerator.h"
#include "MirrorGenerator.h"
#include "CollGenerator.h"
#include "SqrFMaskGenerator.h"
#include "PortChicaneGenerator.h"
#include "JawFlangeGenerator.h"
#include "BremCollGenerator.h"
#include "WallLeadGenerator.h"
#include "CRLTubeGenerator.h"
#include "TriggerGenerator.h"
#include "CylGateValveGenerator.h"
#include "BeamPairGenerator.h"
#include "IonGaugeGenerator.h"
#include "DCMTankGenerator.h"
#include "MonoBlockXstalsGenerator.h"
#include "MLMonoGenerator.h"
#include "ViewScreenGenerator.h"
#include "ScreenGenerator.h"
#include "CooledScreenGenerator.h"
#include "YagScreenGenerator.h"
#include "SixPortGenerator.h"
#include "BoxJawsGenerator.h"
#include "PipeShieldGenerator.h"
#include "ConnectorGenerator.h"
#include "FlangeDomeGenerator.h"
#include "CollTubeGenerator.h"
#include "CollUnitGenerator.h"
#include "TableGenerator.h"
#include "AreaDetectorGenerator.h"
#include "OpticsHutGenerator.h"
#include "ExptHutGenerator.h"

namespace setVariable
{

namespace micromaxVar
{

void undulatorVariables(FuncDataBase&,const std::string&);
void hdmmPackage(FuncDataBase&,const std::string&);
void hdcmPackage(FuncDataBase&,const std::string&);
void diagPackage(FuncDataBase&,const std::string&);
void diag2Package(FuncDataBase&,const std::string&);
void crlPackage(FuncDataBase&,const std::string&);
void diag3Package(FuncDataBase&,const std::string&);
  
void monoShutterVariables(FuncDataBase&,const std::string&);
void mirrorBox(FuncDataBase&,const std::string&,const std::string&,
	       const double,const double);
void viewPackage(FuncDataBase&,const std::string&);
void detectorTubePackage(FuncDataBase&,const std::string&);

void opticsHutVariables(FuncDataBase&,const std::string&);
void exptHutVariables(FuncDataBase&,const std::string&,const double);
void exptHutBVariables(FuncDataBase&,const std::string&,const double);
void opticsVariables(FuncDataBase&,const std::string&);
void exptLineVariables(FuncDataBase&,const std::string&);
void exptLineBVariables(FuncDataBase&,const std::string&);
void shieldVariables(FuncDataBase&,const std::string&);
void monoShutterVariables(FuncDataBase&,const std::string&);
void monoShutterBVariables(FuncDataBase&,const std::string&);
  

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

  const double undulatorLen(300.0);
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
  Control.addVariable(undKey+"UndulatorLength",270.0);   // 46.2mm*30*2
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

  const double FM1Length(15.0);
  const double FM2Length(34.2);
  const double FM3Length(17.0);

  const double FM1dist(1172.60);
  const double FM2dist(1624.2);
  const double FM3dist(1624.2+FM2Length);  

  FMaskGen.setCF<CF100>();
  // Approximated to get 1mrad x 1mrad  
  FMaskGen.setFrontAngleSize(FM1dist,1300.0,1300.0); 
  FMaskGen.setMinAngleSize(10.0,FM1dist,1000.0,1000.0); 
  FMaskGen.setBackAngleSize(FM1dist, 1200.0,1100.0); 
  FMaskGen.generateColl(Control,preName+"CollA",FM1dist,FM1Length);


  // approx for 200uRad x 200uRad
  FMaskGen.setFrontAngleSize(FM2dist,300.0,300.0); 
  FMaskGen.setMinAngleSize(32.0,FM2dist, 200.0, 200.0 );
  FMaskGen.setBackAngleSize(FM2dist, 250.0,250.0 );   
  FMaskGen.generateColl(Control,preName+"CollB",FM2dist,FM2Length);
  
  // approx for 100uRad x 100uRad
  FMaskGen.setFrontAngleSize(FM3dist,150.0,150.0);  
  FMaskGen.setMinAngleSize(12.0,FM3dist, 100.0, 100.0 );
  FMaskGen.setBackAngleSize(FM3dist, 160.0,160.0 );   
  FMaskGen.generateColl(Control,preName+"CollC",FM3Length/2.0,FM3Length);


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

  return;
}

void
hdmmPackage(FuncDataBase& Control,const std::string& monoKey)
  /*!
    Builds the variables for the hdmm (diffraction/mirror) packge
    \param Control :: Database
    \param monoKey :: prename
  */
{
  ELog::RegMethod RegA("micromaxVariables[F]","hdmmPackage");

  setVariable::PortItemGenerator PItemGen;
  setVariable::DCMTankGenerator MBoxGen;
  setVariable::MLMonoGenerator MXtalGen;
 
  // crystals gap 10mm [ystep,thetaA,thetaB]
  MXtalGen.setGap(1.0);
  MXtalGen.generateMono(Control,monoKey+"MLM",-10.0,1.3,1.3);

  // ystep/width/height/depth/length
  // 
  MBoxGen.setCF<CF40>();   // set ports
  MBoxGen.setPortLength(7.5,7.5); // La/Lb
  MBoxGen.setBPortOffset(0.0,0.0);    // note -1mm from crystal offset
  // radius : Height / depth  [need heigh = 0]
  MBoxGen.generateBox(Control,monoKey+"DMMVessel",30.0,0.0,16.0);

  const double outerRadius(30.5);
  const std::string portName=monoKey+"DMMVessel";
  Control.addVariable(portName+"NPorts",1);   // beam ports (lots!!)
  
  PItemGen.setCF<setVariable::CF63>(outerRadius+5.0);
  PItemGen.setWindowPlate(2.5,2.0,-0.8,"Stainless304","LeadGlass");
  PItemGen.generatePort(Control,portName+"Port0",
  			Geometry::Vec3D(0,5.0,-10.0),
  			Geometry::Vec3D(1,0,0));
  

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
  MBoxGen.setBPortOffset(-1.0,0.0);    
  // radius : Height / depth  [need heigh = 0]
  MBoxGen.generateBox(Control,monoKey+"DCMVessel",30.0,0.0,16.0);

  const double outerRadius(30.5);
  const std::string portName=monoKey+"DCMVessel";
  Control.addVariable(portName+"NPorts",1);   // beam ports (lots!!)
  
  PItemGen.setCF<setVariable::CF63>(outerRadius+5.0);
  PItemGen.setWindowPlate(2.5,2.0,-0.8,"Stainless304","LeadGlass");
  PItemGen.generatePort(Control,portName+"Port0",
  			Geometry::Vec3D(0,5.0,-10.0),
  			Geometry::Vec3D(1,0,0));

  // crystals gap 10mm
  MXtalGen.setGap(1.0);
  MXtalGen.generateXstal(Control,monoKey+"MBXstals",0.0,3.0);

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

  setVariable::ViewScreenGenerator VTGen;
  setVariable::BellowGenerator BellowGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::BremTubeGenerator BTGen;
  setVariable::HPJawsGenerator HPGen;
  setVariable::BremBlockGenerator MaskGen;
  setVariable::BeamScrapperGenerator ScrapperGen;
  setVariable::CylGateValveGenerator GVGen;
  setVariable::CooledScreenGenerator CoolGen;
  
  BTGen.generateTube(Control,Name+"MonoBremTube");

  ScrapperGen.generateScreen(Control,Name+"BremScrapper");
  
  MaskGen.setAperature(-1,1.0,1.0,1.0,1.0,1.0,1.0);
  MaskGen.generateBlock(Control,Name+"BremCollB",-4.0);

  HPGen.setMain(24.0);
  HPGen.generateJaws(Control,Name+"HPJawsA",0.3,0.3);

  const std::string portName=Name+"MonoBremTube";

  Control.addVariable(portName+"FrontNPorts",1);   // beam ports 
  PItemGen.setCF<setVariable::CF63>(CF63::outerRadius+9.1);
  PItemGen.setPlate(2.0,"Stainless304");  
  PItemGen.setOuterVoid(0);
  PItemGen.generatePort(Control,portName+"FrontPort0",
			Geometry::Vec3D(0,-11.5,0),
			Geometry::Vec3D(-1,0,0));

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,Name+"BellowI",7.50);

  VTGen.setPortBCF<setVariable::CF40>();
  VTGen.setPortBLen(3.0);
  VTGen.generateView(Control,Name+"ViewTubeB");

  CoolGen.generateScreen(Control,Name+"CooledScreenB",1);  // in beam
  Control.addVariable(Name+"CooledScreenBYAngle",-90.0);

  
  GVGen.generateGate(Control,Name+"GateTubeD",0);  // open
  
  
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

  setVariable::BellowGenerator BellowGen;
  setVariable::BremTubeGenerator BTGen;
  setVariable::BremBlockGenerator MaskGen;
  setVariable::PipeGenerator PipeGen;
  setVariable::CooledScreenGenerator CoolGen;  
  setVariable::CylGateValveGenerator GVGen;
  setVariable::ViewScreenGenerator VTGen;
  setVariable::HPJawsGenerator HPGen;
  setVariable::SixPortGenerator CrossGen;
  
    
  PipeGen.setNoWindow();   // no window
  PipeGen.setCF<setVariable::CF40>();
  PipeGen.generatePipe(Control,diagKey+"LongPipeA",160.3);  //drawing
  PipeGen.generatePipe(Control,diagKey+"LongPipeB",200.0);  //drawing

  GVGen.generateGate(Control,diagKey+"GateTubeE",0);  // open

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,diagKey+"BellowJ",7.5);    
  
  VTGen.setPortBCF<setVariable::CF40>();
  VTGen.setPortBLen(22.5);
  VTGen.generateView(Control,diagKey+"ViewTubeC");

  CoolGen.generateScreen(Control,diagKey+"CooledScreenC",1);  // in beam
  Control.addVariable(diagKey+"CooledScreenCYAngle",-90.0);

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,diagKey+"BellowK",7.5);    

  HPGen.setMain(24.0);
  HPGen.generateJaws(Control,diagKey+"HPJawsB",0.3,0.3);
  BellowGen.generateBellow(Control,diagKey+"BellowL",7.5);    
 
  //  BTGen.generateTube(Control,diagKey+"CRLBremTube");
  CrossGen.setCF<setVariable::CF40>();
  CrossGen.setSideCF<setVariable::CF120>();
  CrossGen.setLength(12.0,15.0);
  CrossGen.setSideLength(16.5);
  CrossGen.generateSixPort(Control,diagKey+"CRLBremTube");
  Control.addVariable(diagKey+"CRLBremTubeYAngle",90.0);
  
  MaskGen.setAperature(-1,1.0,1.0,1.0,1.0,1.0,1.0);
  MaskGen.generateBlock(Control,diagKey+"BremCollC",-4.0);
  
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

  setVariable::BellowGenerator BellowGen;
  setVariable::RoundShutterGenerator RShutterGen;
  setVariable::PipeGenerator PipeGen;
  
  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,preName+"BellowL",7.5);    

  // up / up (true)
  RShutterGen.generateShutter(Control,preName+"RMonoShutter",1,1);  
  Control.addVariable(preName+"RMonoShutterYAngle",180);

  return;
}

void
monoShutterBVariables(FuncDataBase& Control,
		      const std::string& preName)
  /*!
    Construct Mono Shutter variables
    \param Control :: Database for variables
    \param preName :: Control system
   */
{
  ELog::RegMethod RegA("micromaxVariables","monoShutterBVariables");

  setVariable::BellowGenerator BellowGen;
  setVariable::RoundShutterGenerator RShutterGen;
  setVariable::PipeGenerator PipeGen;
  
  // up / up (true)
  RShutterGen.generateShutter(Control,preName+"RMonoShutterB",1,1);  
  Control.addVariable(preName+"RMonoShutterBYAngle",270);
  Control.addVariable(preName+"RMonoShutterBYStep",495);

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

  const double beamMirrorShift(-1.0);
  
  OpticsHutGenerator OGen;
  PortChicaneGenerator PGen;
    
  OGen.setSkin(0.2);
  OGen.setBackLead(10.0);
  OGen.setWallLead(2.0);
  OGen.setRoofLead(2.0);

  OGen.generateHut(Control,hutName,1256.0);
  
  OGen.addHole(Geometry::Vec3D(beamMirrorShift,0,0),3.5);
  OGen.generateHut(Control,hutName,1256.0);

  Control.addVariable(hutName+"RingStepLength",840.0);
  Control.addVariable(hutName+"RingStepWidth",200.0);

  Control.addVariable(hutName+"NChicane",3);

  PGen.setSize(4.0,60.0,40.0);
  PGen.generatePortChicane(Control,hutName+"Chicane0","Right",-450.0,-5.0);
  PGen.generatePortChicane(Control,hutName+"Chicane1","Right",-300.0,-5.0);

  PGen.setSize(4.0,30.0,40.0);
  PGen.generatePortChicane(Control,hutName+"Chicane2","Right",-375.0,-5.0);
  
  // Forklift truck holes
  Control.addVariable(hutName+"ForkNHoles",0);
  Control.addVariable(hutName+"ForkWall","Outer");
  Control.addVariable(hutName+"ForkYStep",80.0);
  Control.addVariable(hutName+"ForkLength",60.0);
  Control.addVariable(hutName+"ForkHeight",10.0);
  Control.addVariable(hutName+"ForkZStep0",-115.0);
  Control.addVariable(hutName+"ForkZStep1",0.0);
  Control.addVariable(hutName+"ForkZStep2",80.0);

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

  setVariable::ExptHutGenerator EGen;
  
  const double beamOffset(-1.0);
  const std::string hutName(beamName+"ExptHut");

  EGen.setFrontHole(beamXStep-beamOffset,0.0,3.0);
  EGen.addHole(Geometry::Vec3D(beamOffset,0,0),3.5);
  EGen.generateHut(Control,hutName,0.0,901.0);

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

  Control.addVariable(hutName+"NChicane",4);
  PortChicaneGenerator PGen;
  PGen.setSize(4.0,60.0,40.0);
  PGen.generatePortChicane(Control,hutName+"Chicane0","Left",-350,-5.0);
  PGen.generatePortChicane(Control,hutName+"Chicane1","Left",-250.0,-5.0);
  PGen.generatePortChicane(Control,hutName+"Chicane2","Left",-150.0,-5.0);
  PGen.generatePortChicane(Control,hutName+"Chicane3","Left",-50.0,-5.0);
  /*
  PGen.generatePortChicane(Control,hutName+"Chicane1",370.0,-25.0);
  PGen.generatePortChicane(Control,hutName+"Chicane2",-70.0,-25.0);
  PGen.generatePortChicane(Control,hutName+"Chicane3",-280.0,-25.0);
  */

  return;
}

void
exptHutBVariables(FuncDataBase& Control,
		  const std::string& beamName,
		  const double beamXStep)
  /*!
    Optics hut variables
    \param Control :: DataBase to add
    \param beamName :: Beamline name
    \param bremXStep :: Offset of beam from main centre line
  */
{
  ELog::RegMethod RegA("micromaxVariables[F]","exptHutBVariables");

  setVariable::ExptHutGenerator EGen;
  
  const double beamOffset(-1.0);
  const std::string hutName(beamName+"ExptHutB");

  EGen.setFrontHole(beamXStep-beamOffset,0.0,3.0);
  EGen.generateHut(Control,hutName,0.0,478.0);

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
  PGen.generatePortChicane(Control,hutName+"Chicane1","Left",-170.0,-5.0);
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
	  const double theta,const double phi)
  /*!
    Construct variables for the diagnostic units
    \param Control :: Database
    \param Name :: component name
    \param Index :: Index designator for mirror box (A/B etc)
    \param theta :: theta angle [horrizontal rotation in deg]
    \param phi :: phi angle [vertical rotation in deg]
  */
{
  ELog::RegMethod RegA("formaxVariables[F]","mirrorBox");
  
  setVariable::MonoBoxGenerator VBoxGen;
  setVariable::MirrorGenerator MirrGen;

  const double mainLength(108.0);
  const double mainMirrorAngleA(0.0);
  const double mainMirrorAngleB(90.0);
  const double centreDist(55.0);
  const double aMirrorDist((mainLength+centreDist)/2.0);
  const double bMirrorDist((mainLength-centreDist)/2.0);

  const double heightDelta=tan(2.0*std::abs(phi)*M_PI/180.0)*aMirrorDist;
  const double widthDelta=tan(2.0*std::abs(theta)*M_PI/180.0)*bMirrorDist;

  VBoxGen.setBPortOffset(widthDelta,heightDelta);
  // if rotated through 90 then theta/phi reversed
  VBoxGen.setBPortAngle(2.0*phi,2.0*theta);

  VBoxGen.setMat("Stainless304");
  VBoxGen.setWallThick(1.0);
  VBoxGen.setCF<CF63>();
  VBoxGen.setPortLength(5.0,5.0); // La/Lb
  VBoxGen.setLids(3.0,1.0,1.0); // over/base/roof

  // width/height/depth/length
  VBoxGen.generateBox(Control,Name+"MirrorBox"+Index,
		      53.1,23.6,29.5,mainLength);

  // length thick width
  MirrGen.setPlate(50.0,1.0,9.0);  //guess  
  MirrGen.setPrimaryAngle(0,mainMirrorAngleA,0);  
  // xstep : ystep : zstep : theta : phi : radius
  MirrGen.generateMirror(Control,Name+"MirrorFront"+Index,
			 0.0,-centreDist/2.0,0.0,
			 -phi,0.0,0.0);   // hits beam center
  
  MirrGen.setPrimaryAngle(0,mainMirrorAngleB,0.0);
  // x/y/z/theta/phi/
  MirrGen.generateMirror(Control,Name+"MirrorBack"+Index,
			 0,centreDist/2.0,0,
			 theta,0.0,0.0);
  return;
}

void
crlPackage(FuncDataBase& Control,const std::string& Name)
  /*!
    Construct variables for the compount refractive lens 
    (CRL) package.
    \param Control :: Database
    \param Name :: component name
  */
{
  ELog::RegMethod RegA("micromaxVariables[F]","crlPackage");
  setVariable::BellowGenerator BellowGen;
  setVariable::PipeGenerator PipeGen;
  setVariable::CRLTubeGenerator DPGen;
  
  PipeGen.setNoWindow();   // no window
  PipeGen.setCF<setVariable::CF40>();

  PipeGen.setCF<CF40>();
  PipeGen.setBFlangeCF<CF63>();
  PipeGen.generatePipe(Control,Name+"CRLPipeA",12.5);

  DPGen.setMain(65,34.0,25.0);  // length/width/height
  DPGen.setPortCF<CF63>(5.0); 
  DPGen.setLens(10,1.5,0.2);
  DPGen.setSecondary(0.8);
  DPGen.generateLens(Control,Name+"CRLTubeA",1);  // in beam

  PipeGen.setCF<CF63>();  
  PipeGen.generatePipe(Control,Name+"CRLPipeB",16.0);
  PipeGen.generatePipe(Control,Name+"CRLPipeC",25.0);

  PipeGen.setBFlangeCF<CF40>();
  DPGen.generateLens(Control,Name+"CRLTubeB",1);  // in beam

  PipeGen.generatePipe(Control,Name+"CRLPipeD",12.5);
    
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

  setVariable::BellowGenerator BellowGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::PortTubeGenerator PTubeGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::JawValveGenerator JawGen;
  setVariable::BeamPairGenerator BeamMGen;
  setVariable::TableGenerator TableGen;
  setVariable::BremBlockGenerator MaskGen;
  setVariable::IonGaugeGenerator IGGen;
  setVariable::ViewScreenGenerator VTGen;
  setVariable::CooledScreenGenerator CoolGen;
  setVariable::CollTubeGenerator CTGen;
  setVariable::CollUnitGenerator CUGen;
  
  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,Name+"BellowB",7.50);
  
  IGGen.setCF<CF150>();
  IGGen.setMainLength(12.0,8.0);
  IGGen.generateTube(Control,Name+"BremHolderA");

  MaskGen.setAperatureAngle(7.0,0.2,0.2,5.0,5.0);
  MaskGen.generateBlock(Control,Name+"BremCollA",-4.0);

  BellowGen.generateBellow(Control,Name+"BellowC",7.50);
 
  // View tube
  
  VTGen.setPortBCF<setVariable::CF40>();
  VTGen.setPortBLen(3.0);
  VTGen.generateView(Control,Name+"ViewTubeA");

  CoolGen.generateScreen(Control,Name+"CooledScreenA",1);  // in beam
  Control.addVariable(Name+"CooledScreenAYAngle",-90.0);

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,Name+"BellowD",7.50);
  
  const std::string attnTubeName(Name+"AttnTube");
  CTGen.setMainPort(3.0,3.0);
  CTGen.generateTube(Control,attnTubeName,24.0);
  CUGen.generateScreen(Control,Name+"AttnUnit");
  
  
  PTubeGen.setPipe(9.0,0.5);
  PTubeGen.setPortCF<CF40>();
  PTubeGen.setPortLength(-3.0,-3.0);
  PTubeGen.generateTube(Control,attnTubeName,0.0,30.0);
  Control.addVariable(attnTubeName+"NPorts",0);   // beam ports

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,Name+"BellowE",7.50);

  // ystep zstep , length
  TableGen.generateTable(Control,Name+"TableA",14.0,-20,125.0);
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
  setVariable::CRLTubeGenerator DiffGen;
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

  const double collDist(2400);
  FMaskGen.setCF<CF63>();
  FMaskGen.setFrontGap(2.13,2.146);
  FMaskGen.setBackGap(0.756,0.432);
  FMaskGen.setMinAngleSize(10.0,collDist,40.0,40.0);
  // step to +7.5 to make join with fixedComp:linkpt
  FMaskGen.generateColl(Control,preName+"WhiteCollA",7.5,15.0);

  IGGen.generateTube(Control,preName+"IonGaugeA");
 
  micromaxVar::diagPackage(Control,preName);

  micromaxVar::hdmmPackage(Control,preName);

  BellowGen.generateBellow(Control,preName+"BellowF",7.5);
  GVGen.generateGate(Control,preName+"GateTubeB",0);  // open
  BellowGen.generateBellow(Control,preName+"BellowG",7.5);
    
  micromaxVar::hdcmPackage(Control,preName);

  BellowGen.generateBellow(Control,preName+"BellowH",7.5);

  PipeGen.generatePipe(Control,preName+"PipeB",7.5);
  GVGen.generateGate(Control,preName+"GateTubeC",0);  // open
  
  micromaxVar::diag2Package(Control,preName);

  micromaxVar::crlPackage(Control,preName);

  micromaxVar::diag3Package(Control,preName);

  micromaxVar::monoShutterVariables(Control,preName);

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
exptLineVariables(FuncDataBase& Control,
	      const std::string& beamName)
  /*
    Components in the first experimental hutch
    \param Control :: Function data base
    \param beamName :: Name of beamline
  */
{
  ELog::RegMethod RegA("micromaxVariables[F]","exptVariables");

  setVariable::PipeGenerator PipeGen;
  setVariable::BellowGenerator BellowGen;
  setVariable::CRLTubeGenerator DPGen;
  setVariable::ViewScreenGenerator VTGen;
  setVariable::CooledScreenGenerator CoolGen;
  setVariable::CylGateValveGenerator GVGen;
  setVariable::HPJawsGenerator HPGen;
  setVariable::FlangePlateGenerator FPGen;
    
  PipeGen.setNoWindow();
  PipeGen.setMat("Stainless304");
  PipeGen.setCF<CF40>();
  
  const std::string preName(beamName+"ExptLine");

  GVGen.generateGate(Control,preName+"GateTubeA",0);  // open

  // ByPass / Sample / Diffraction [options]
  Control.addVariable(preName+"ExptType","ByPass");
  Control.addVariable(preName+"OuterLeft",50.0);
  Control.addVariable(preName+"OuterRight",50.0);
  Control.addVariable(preName+"OuterTop",60.0);

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,preName+"BellowA",7.5);

  VTGen.setPortBCF<setVariable::CF40>();
  VTGen.setPortBLen(22.5);
  VTGen.generateView(Control,preName+"ViewTube");

  CoolGen.generateScreen(Control,preName+"CooledScreen",1);  // in beam
  Control.addVariable(preName+"CooledScreenYAngle",-90.0);

  PipeGen.generatePipe(Control,preName+"PipeA",7.5);  

  HPGen.setMain(24.0);
  HPGen.generateJaws(Control,preName+"HPJaws",0.3,0.3);

  PipeGen.generatePipe(Control,preName+"PipeB",7.5);
  GVGen.generateGate(Control,preName+"GateTubeB",0);  // open
  PipeGen.generatePipe(Control,preName+"PipeC",15.0);
  
  crlPackage(Control,preName);

  PipeGen.generatePipe(Control,preName+"EndPipe",15.0);
  micromaxVar::mirrorBox(Control,preName,"A",-0.146,0.146);

  FPGen.setCF<setVariable::CF63>(1.0);
  FPGen.setWindow(2.0,0.3,"Diamond");
  FPGen.generateFlangePlate(Control,preName+"EndWindow");
    
  Control.addVariable(preName+"SampleYStep", 185.0); // [2]
  Control.addVariable(preName+"SampleRadius", 5.0); // [2]
  Control.addVariable(preName+"SampleDefMat", "Stainless304");

  monoShutterBVariables(Control,preName);
  PipeGen.generatePipe(Control,preName+"ExitPipe",35.0);  


  // Diffraciton stage
  PipeGen.setAFlangeCF<CF63>();
  PipeGen.generatePipe(Control,preName+"DiffractTube",125.0);
  // bypass stage

  PipeGen.generatePipe(Control,preName+"ByPassTube",435.0);
  return;
}

void
exptLineBVariables(FuncDataBase& Control,
		   const std::string& beamName)
  /*
    Components in the second experimental hutch
    \param Control :: Function data base
    \param beamName :: Name of beamline
  */
{
  ELog::RegMethod RegA("micromaxVariables[F]","exptLineBVariables");

  setVariable::PipeGenerator PipeGen;
  setVariable::BellowGenerator BellowGen;
  setVariable::ViewScreenGenerator VTGen;
  setVariable::CooledScreenGenerator CoolGen;
  setVariable::CylGateValveGenerator GVGen;
  setVariable::HPJawsGenerator HPGen;
  
  PipeGen.setNoWindow();
  PipeGen.setMat("Stainless304");
  PipeGen.setCF<CF40>();
  
  const std::string exptName(beamName+"ExptLineB");
  Control.addVariable(exptName+"OuterLeft",50.0);
  Control.addVariable(exptName+"OuterRight",50.0);
  Control.addVariable(exptName+"OuterTop",60.0);

  GVGen.generateGate(Control,exptName+"GateTubeA",0);  // open
    
  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,exptName+"BellowA",7.5);

  VTGen.setPortBCF<setVariable::CF40>();
  VTGen.setPortBLen(22.5);
  VTGen.generateView(Control,exptName+"ViewTube");

  CoolGen.generateScreen(Control,exptName+"CooledScreen",1);  // in beam
  Control.addVariable(exptName+"CooledScreenYAngle",-90.0);

  PipeGen.generatePipe(Control,exptName+"PipeA",7.5);  

  HPGen.setMain(24.0);
  HPGen.generateJaws(Control,exptName+"HPJaws",0.3,0.3);

  PipeGen.generatePipe(Control,exptName+"PipeB",7.5);

  Control.addVariable(exptName+"SampleYStep", 185.0); // [2]
  Control.addVariable(exptName+"SampleRadius", 5.0); // [2]
  Control.addVariable(exptName+"SampleDefMat", "Stainless304");
  
  Control.addVariable(exptName+"BeamStopYStep",478.0-12.5);
  Control.addVariable(exptName+"BeamStopRadius",10.0);
  Control.addVariable(exptName+"BeamStopThick",3.0);
  Control.addVariable(exptName+"BeamStopMat","Stainless304");

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
  PipeGen.generatePipe(Control,"MicroMaxJoinPipe",130.0);
  Control.addVariable("MicroMaxJoinPipeFlipX",1);
    
  micromaxVar::opticsHutVariables(Control,"MicroMaxOpticsHut");
  micromaxVar::opticsVariables(Control,"MicroMax");

  PipeGen.setCF<setVariable::CF40>(); 
  PipeGen.generatePipe(Control,"MicroMaxJoinPipeB",70.0);

  micromaxVar::shieldVariables(Control,"MicroMax");
  
  micromaxVar::exptHutVariables(Control,"MicroMax",0.0);
  micromaxVar::exptHutBVariables(Control,"MicroMax",0.0);
  micromaxVar::exptLineVariables(Control,"MicroMax");
  micromaxVar::exptLineBVariables(Control,"MicroMax");
  //  micromaxVar::detectorTubePackage(Control,"MicroMax");

  PipeGen.setCF<setVariable::CF40>(); 
  PipeGen.generatePipe(Control,"MicroMaxJoinPipeC",20.0);


  return;
}

}  // NAMESPACE setVariable
