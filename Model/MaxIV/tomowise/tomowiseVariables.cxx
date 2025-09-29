/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   tomowise/tomowiseVariables.cxx
 *
 * Copyright (c) 2004-2025 by Konstantin Batkov
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
#include "LeadPipeGenerator.h"
#include "GateValveGenerator.h"
#include "JawValveGenerator.h"
#include "PipeTubeGenerator.h"
#include "PortTubeGenerator.h"
#include "PortItemGenerator.h"
#include "VacBoxGenerator.h"
#include "MonoBoxGenerator.h"
#include "RoundShutterGenerator.h"
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
#include "SixPortGenerator.h"
#include "CollTubeGenerator.h"
#include "CollUnitGenerator.h"
#include "TableGenerator.h"
#include "OpticsHutGenerator.h"
#include "ExptHutGenerator.h"
#include "MovableSafetyMaskGenerator.h"
#include "MLMDetailGenerator.h"
#include "YagScreenGenerator.h"
#include "ConnectorGenerator.h"
#include "BoxJawsGenerator.h"
#include "PowerFilterGenerator.h"
#include "HeatAbsorberToyamaGenerator.h"
#include "ProximityShieldingGenerator.h"
#include "BeamMountGenerator.h"
#include "BremBlockGenerator.h"
#include "MainBeamDumpGenerator.h"


// References
// see R3Common/R3RingVariables.cxx and:
// [4] TomoWISE hutch walls
//     http://localhost:8080/maxiv/work-log/tomowise/cad/250214-tomowise-hutch-255596-step.zst/view

namespace setVariable
{

namespace tomowiseVar
{

void undulatorVariables(FuncDataBase&,const std::string&);
void hdmmPackage(FuncDataBase&,const std::string&);
void hdcmPackage(FuncDataBase&,const std::string&);
void diagPackage(FuncDataBase&,const std::string&);
void diag2Package(FuncDataBase&,const std::string&);
void crlPackage(FuncDataBase&,const std::string&);
void safetyUnit(FuncDataBase&,const std::string&);

void monoShutterVariables(FuncDataBase&,const std::string&);
void mirrorBox(FuncDataBase&,const std::string&,const std::string&,
	       const double,const double);
void viewPackage(FuncDataBase&,const std::string&);
void detectorTubePackage(FuncDataBase&,const std::string&);

void opticsHutVariables(FuncDataBase&,const std::string&);
void exptHutVariables(FuncDataBase&,const std::string&,const double);
void opticsVariables(FuncDataBase&,const std::string&);
void exptLineVariables(FuncDataBase&,const std::string&);
  //void exptLineBVariables(FuncDataBase&,const std::string&);
void shieldVariables(FuncDataBase&,const std::string&);
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
  ELog::RegMethod RegA("tomowiseVariables[F]","undulatorVariables");
  setVariable::PipeGenerator PipeGen;

  const double undulatorLen(300.0);
  PipeGen.setMat("Aluminium");
  PipeGen.setNoWindow();   // no window
  PipeGen.setCF<setVariable::CF63>();
  PipeGen.generatePipe(Control,undKey+"UPipe",undulatorLen);
  Control.addVariable(undKey+"UPipeYStep",-undulatorLen/2.0);

  Control.addVariable(undKey+"UPipeWidth",6.0);
  Control.addVariable(undKey+"UPipeHeight",0.6);
  Control.addVariable(undKey+"UPipePipeThick",0.2);

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
  setVariable::BellowGenerator BellowGen;

  constexpr double FM1Length(40.0); // [3]
  constexpr double FM2Length(40.0); // [3]
  constexpr double MSMLength(40.0); // [3]

  const double FM1dist(1150.0+FM1Length/2.0); // [3]
  const double FM2dist(1500.0+FM2Length/2.0); // [3]
  const double MSMdist(1600.0); // [3]

  FMaskGen.setCF<CF100>();
  FMaskGen.setFrontGap(2.53, 2.53); // [3]
  double backWidth = 1.19; // [3]
  double backHeight = backWidth;
  FMaskGen.setBackGap(backWidth, backHeight);
  FMaskGen.setMinSize(FM1Length-CF100::flangeLength-Geometry::zeroTol,
		      backWidth, backHeight);
  FMaskGen.generateColl(Control,preName+"CollA",FM1dist,FM1Length);



  FMaskGen.setFrontGap(1.65, 1.65); // [3]
  backWidth = 1.54; // [3]
  backHeight = 0.154; // [3]
  FMaskGen.setBackGap(backWidth, backHeight);
  FMaskGen.setMinSize(FM2Length-CF100::flangeLength-Geometry::zeroTol,
		      backWidth, backHeight);
  FMaskGen.generateColl(Control,preName+"CollB",FM2dist,FM2Length);

;


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
  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,preName+"BellowPreMSM",14.0); // guess

  MovableSafetyMaskGenerator MSMGen;
  MSMGen.generate(Control,preName+"MSM",MSMLength, "undulator"); // [2]
  Control.addVariable(preName+"MSMYStep",MSMdist);

  BellowGen.generateBellow(Control,preName+"BellowPostMSM",14.0); // guess

  PipeGen.setMat("Stainless304L"); // dummy
  PipeGen.generatePipe(Control,preName+"MSMExitPipe",100.0); // dummy

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
  ELog::RegMethod RegA("tomowiseVariables[F]","hdmmPackage");

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
    \param monoKey :: prename
  */
{
  ELog::RegMethod RegA("tomowiseVariables[F]","hdcmPackage");

  setVariable::PortItemGenerator PItemGen;
  setVariable::DCMTankGenerator MBoxGen;
  setVariable::MonoBlockXstalsGenerator MXtalGen;

  // ystep/width/height/depth/length
  //
  MBoxGen.setCF<CF40>();   // set ports
  MBoxGen.setPortLength(7.5,7.5); // La/Lb
  MBoxGen.setBPortOffset(0.0,0.0);    // Both ports without offset to allow two operation modes in TomoWISE (whte beam and mono beam)
  // radius : Height / depth  [need height = 0]
  MBoxGen.generateBox(Control,monoKey+"DCMVessel",30.0,0.0,16.0);

  //  Control.addVariable(monoKey+"MonoVesselPortAZStep",-7);   //
  //  Control.addVariable(monoKey+"MonoVesselFlangeAZStep",-7);     //
  //  Control.addVariable(monoKey+"MonoVesselFlangeBZStep",-7);     //

  const double outerRadius(30.5);
  const std::string portName=monoKey+"DCMVessel";
  Control.addVariable(monoKey+"MonoVesselNPorts",1);   // beam ports (lots!!)
  PItemGen.setCF<setVariable::CF63>(outerRadius+5.0);
  PItemGen.setWindowPlate(2.5,2.0,-0.8,"Stainless304","LeadGlass");
  PItemGen.generatePort(Control,portName+"Port0",
  			Geometry::Vec3D(0,5.0,-10.0),
  			Geometry::Vec3D(1,0,0));


  MXtalGen.setGap(1.0);
  MXtalGen.generateXstal(Control,monoKey+"MBXstals",0.0,3.0);
  Control.addVariable(monoKey+"MBXstalsYAngle",180.0);

  // pipe alternative to DCM
  setVariable::PipeGenerator PipeGen;
  PipeGen.setMat("Stainless304");
  PipeGen.setCF<CF40>();
  PipeGen.setOuterVoid(1);
  PipeGen.generatePipe(Control,monoKey+"DCMReplacementPipe",77.0);

  return;
}

void
diag2Package(FuncDataBase& Control,
	     const std::string& diagKey)
  /*!
    Builds the variables for the second diagnostice/slit packge
    \param Control :: Database
    \param diagKey :: prename
  */
{
  ELog::RegMethod RegA("tomowiseVariables[F]","diag2Package");

  setVariable::PortItemGenerator PItemGen;
  setVariable::BremTubeGenerator BTGen;
  setVariable::HPJawsGenerator HPGen;

  setVariable::BremBlockGenerator MaskGen;


  BTGen.generateTube(Control,diagKey+"BremTubeA");

  MaskGen.setLength(8.0);
  // x-gap size: 1 mrad (2.806 cm)    + 0.2 cm (safety margin) \approx 3.0
  //          we will vary the safety margin
  constexpr double xgap = 3.0;
  // y-gap size: 0.1 mrad (0.2806 cm) + 0.2 cm (safety margin) \approx 0.5
  constexpr double ygap = 0.5;
  MaskGen.setAperature(-1,xgap,ygap,xgap,ygap,xgap,ygap);
  MaskGen.generateBlock(Control,diagKey+"BremCollB",-4.0);
  Control.addVariable(diagKey+"BremCollBHeight",10.0);
  Control.addVariable(diagKey+"BremCollBWidth",10.0);

  // Monochromatic beam slits
  HPGen.generateJaws(Control,diagKey+"HPJawsA",xgap,ygap);
  // just a big gap to avoid cutting the beam. It does not exist in the CAD drawing
  Control.addVariable(diagKey+"HPJawsADividerGap",12);

  setVariable::BellowGenerator BellowGen;
  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,diagKey+"BellowFA",10.0+2.8);

  setVariable::PipeTubeGenerator SimpleTubeGen;

  const std::string viewName=diagKey+"ViewTubeA"; // BPM screen
  SimpleTubeGen.setCF<CF40>();
  SimpleTubeGen.setCap(0,0);
  //  SimpleTubeGen.setBFlangeCF<CF150>();
  SimpleTubeGen.generateTube(Control,viewName,25.0);
  Control.addVariable(viewName+"NPorts",4);   // beam ports

  PItemGen.setPlate(4.0,"Stainless304");

  PItemGen.setCF<setVariable::CF100>(CF100::outerRadius+5.0);
  PItemGen.setOuterVoid(1);
  PItemGen.generatePort(Control,viewName+"Port0",
			Geometry::Vec3D(0,0,0),Geometry::Vec3D(0,0,1));

  PItemGen.setCF<setVariable::CF100>(CF100::outerRadius+10.0);
  PItemGen.generatePort(Control,viewName+"Port1",
			Geometry::Vec3D(0,0,0),Geometry::Vec3D(0,0,-1));

  PItemGen.setCF<setVariable::CF40>(CF40::outerRadius+10.0);
  PItemGen.generatePort(Control,viewName+"Port2",
			Geometry::Vec3D(0,0,0),Geometry::Vec3D(-1,-1,0));

  PItemGen.setCF<setVariable::CF40>(CF40::outerRadius+10.0);
  PItemGen.generatePort(Control,viewName+"Port3",
			Geometry::Vec3D(0,0,0),Geometry::Vec3D(1,0,0));



  return;
}

void
safetyUnit(FuncDataBase& Control,
	     const std::string& preName)
  /*!
    Builds the variables for the second diagnostice/slit packge
    \param Control :: Database
    \param preName :: prename
  */
{
  ELog::RegMethod RegA("tomowiseVariables[F]","safetyUnit");

  setVariable::PortItemGenerator PItemGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::CylGateValveGenerator GVGen;
  setVariable::BellowGenerator BellowGen;
  setVariable::ViewScreenGenerator VSGen;
  setVariable::BremTubeGenerator BTGen;
  setVariable::HPJawsGenerator HPGen;
  setVariable::PipeGenerator PipeGen;
  setVariable::BremBlockGenerator MaskGen;
  setVariable::ProximityShieldingGenerator PSGen;
  setVariable::BeamMountGenerator BeamMGen;

  PipeGen.setNoWindow();   // no window
  PipeGen.setCF<setVariable::CF40>();

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,preName+"BellowG",15.0);
  BellowGen.generateBellow(Control,preName+"BellowH",13.97); // length adjusted to correctly place ProxiShieldA


  HeatAbsorberToyamaGenerator HAGen;
  constexpr double haLength = 30.0; // length: CAD 255589
  HAGen.generate(Control,preName+"HeatAbsorber",haLength);
  Control.addVariable(preName+"HeatAbsorberYStep",3075.1-haLength/2.0); // Excel table TomoWISE_components_feb25
  // 1 mrad + 0.2 cm safety margin
  // (1 mrad is same as the upstream FMs)
  Control.addVariable(preName+"HeatAbsorberGapWidth",3.3);
  Control.addVariable(preName+"HeatAbsorberGapHeight",0.6);



  GVGen.generateGate(Control,preName+"GateTubeE",0);  // open
  Control.addVariable(preName+"GateTubeEPortRadius", 1.9);


  constexpr double proxiShieldAPipeLength = 21.5;
  constexpr double proxiShieldBPipeLength = 20.0;
  constexpr double shutterLength = 57.8;
  constexpr double offPipeALength = 6.8;
  constexpr double shutterBoxLength = shutterLength - offPipeALength;
  constexpr double bremCollTotalLength = 21.0; //[2] (OffPipeB + BremCollPipe)
  constexpr double offPipeBLength = 2.6; // as small as possible
  constexpr double bremCollPipeLength = bremCollTotalLength - offPipeBLength;

  //  constexpr double proxiShieldBPipeEnd = 2110.0 - 2.97; // [2, page1]

  PipeGen.setMat("Stainless304");
  PipeGen.setNoWindow();   // no window
  PipeGen.setCF<setVariable::CF40>();
  PipeGen.setBFlangeCF<setVariable::CF150>();
  PipeGen.generatePipe(Control,preName+"OffPipeA",offPipeALength);
  Control.addVariable(preName+"OffPipeAFlangeBZStep",3.0);

  const std::string shutterName=preName+"ShutterBox";
  SimpleTubeGen.setCF<CF150>();
  SimpleTubeGen.setCap(0,0);
  SimpleTubeGen.generateTube(Control,shutterName,shutterBoxLength);
  Control.addVariable(shutterName+"NPorts",2);

  const Geometry::Vec3D XVec(1,0,0);
  const Geometry::Vec3D ZVec(0,0,1);


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
      const std::string name=preName+"ShutterBoxPort"+std::to_string(i);
      const std::string fname=preName+"BS"+std::to_string(i+1);

      PItemGen.generatePort(Control,name,CPos,ZVec);
      BeamMGen.generateMount(Control,fname,1);      // out of beam:upflag=1
      CPos+=Geometry::Vec3D(0,shutterBoxLength/2.0,0);
    }

  PipeGen.setCF<setVariable::CF63>();
  PipeGen.setAFlangeCF<setVariable::CF150>();
  PipeGen.generatePipe(Control,preName+"OffPipeB",offPipeBLength);
  Control.addVariable(preName+"OffPipeBFlangeAZStep",3.0);
  Control.addVariable(preName+"OffPipeBZStep",-3.0);
  Control.addVariable(preName+"OffPipeBFlangeBType",0);


  // Bremsstrahulung collimator
  setVariable::BremBlockGenerator BBGen;
  std::string name;
  name=preName+"BremCollPipe";
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
  // 1 mrad -> 2.2 + 0.6 cm safety margin
  BBGen.setAperature(-1.0, 3.8, 0.7,  3.8, 0.7,   3.8, 0.7);
  BBGen.generateBlock(Control,preName+"BremColl",0);
  Control.addVariable(preName+"BremCollYStep",(bremCollPipeLength-bremCollLength)/2.0);




  Control.addVariable(preName+"BremBlockRadius",3.0);
  Control.addVariable(preName+"BremBlockLength",20.0);
  Control.addVariable(preName+"BremBlockHoleWidth",2.0);
  Control.addVariable(preName+"BremBlockHoleHeight",2.0);
  Control.addVariable(preName+"BremBlockMainMat","Tungsten");



  PSGen.generate(Control,preName+"ProxiShieldA", 15.0); // CAD
  Control.addVariable(preName+"ProxiShieldAYStep",3.53); // CAD
  Control.addVariable(preName+"ProxiShieldABoreRadius",0.0);

  PipeGen.setCF<setVariable::CF40>();
  PipeGen.generatePipe(Control,preName+"ProxiShieldAPipe",proxiShieldAPipeLength);
  PSGen.generate(Control,preName+"ProxiShieldA", 15.0); // CAD
  Control.addVariable(preName+"ProxiShieldAYStep",3.53); // CAD
  Control.addVariable(preName+"ProxiShieldABoreRadius",0.0);

  Control.copyVarSet(preName+"ProxiShieldAPipe", preName+"ProxiShieldBPipe");
  Control.copyVarSet(preName+"ProxiShieldA", preName+"ProxiShieldB");
  Control.addVariable(preName+"ProxiShieldBPipeLength",proxiShieldBPipeLength);
  Control.addVariable(preName+"ProxiShieldBPipeFlangeARadius",7.5);
  Control.addVariable(preName+"ProxiShieldBYStep",setVariable::CF40::flangeLength+0.1); // approx




  // VSGen.generateView(Control,preName+"ViewTube");
  // YagGen.generateScreen(Control,preName+"YagScreen",1);  // in beam
  // Control.addVariable(preName+"YagScreenYAngle",-45.0);

  //  // will be rotated vertical
  // const std::string viewName=preName+"BremTubeB";
  // SimpleTubeGen.setCF<CF150>();
  // SimpleTubeGen.setCap(1,1);
  // SimpleTubeGen.setBFlangeCF<CF150>();
  // SimpleTubeGen.generateTube(Control,viewName,25.0);
  // Control.addVariable(viewName+"NPorts",2);   // beam ports

  // PItemGen.setCF<setVariable::CF100>(CF150::outerRadius+5.0);
  // PItemGen.setPlate(0.0,"Void");
  // PItemGen.setOuterVoid(0);
  // PItemGen.generatePort(Control,viewName+"Port0",
  // 			Geometry::Vec3D(0,0,0),Geometry::Vec3D(0,0,1));

  // PItemGen.setCF<setVariable::CF150>(CF150::outerRadius+10.0);
  // PItemGen.setPlate(0.0,"Void");
  // PItemGen.generatePort(Control,viewName+"Port1",
  // 			Geometry::Vec3D(0,0,0),Geometry::Vec3D(0,0,-1));

  // MaskGen.setAperature(-1,0.5,0.5,0.5,0.5,0.5,0.5);
  // MaskGen.generateBlock(Control,preName+"BremCollC",-4.0);
  // Control.addVariable(preName+"BremCollCPreXAngle",90);
  // HPGen.generateJaws(Control,preName+"HPJawsB",0.6,0.6);


  // BellowGen.setCF<setVariable::CF40>();
  // BellowGen.generateBellow(Control,preName+"BellowH",15.0);

  // PipeGen.generatePipe(Control,preName+"PipeE",208.0);


  // BellowGen.setCF<setVariable::CF40>();
  // BellowGen.generateBellow(Control,preName+"BellowI",15.0);

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
  ELog::RegMethod RegA("tomowiseVariables[F]","diag4Package");

  setVariable::PortItemGenerator PItemGen;
  setVariable::CylGateValveGenerator GVGen;
  setVariable::ViewScreenGenerator VSGen;
  setVariable::BellowGenerator BellowGen;

  GVGen.generateGate(Control,diagKey+"GateTubeF",0);
  Control.addVariable(diagKey+"GateTubeFPortRadius", 1.9);

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
  ELog::RegMethod RegA("tomowiseVariables","monoShutterVariables");

  setVariable::BellowGenerator BellowGen;
  setVariable::RoundShutterGenerator RShutterGen;
  setVariable::CylGateValveGenerator GVGen;
  setVariable::PipeGenerator PipeGen;

    // up / up (true)
  RShutterGen.generateShutter(Control,preName+"RMonoShutter",1,1);
  Control.addVariable(preName+"RMonoShutterYAngle",-90);
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
  PipeGen.generatePipe(Control,preName+"PipeF",34.0);
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
  ELog::RegMethod RegA("tomowiseVariables","monoShutterBVariables");

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
  ELog::RegMethod RegA("tomowiseVariables","opticsHutVariables");

  const double beamMirrorShift(4.3); //-1.0);

  OpticsHutGenerator OGen;
  PortChicaneGenerator PGen;

  OGen.setSkin(0.2);
  OGen.setBackLead(7.0);
  OGen.setWallLead(2.0);
  OGen.setRoofLead(2.0);

  OGen.addHole(Geometry::Vec3D(beamMirrorShift, 0.0, 3.5), 3.0); // last number is radius
  // Optics hutch length:
  // 250214-TomoWISE-hutch-255596.step: 995.0
  // if set to 995, then the Guillotine cuts ProxiShieldB, therefore
  // add ~10 cm to avoid geometry errors
  OGen.generateHut(Control,hutName,1012.4);
  Control.addVariable(hutName+"OutWidth",262.0);
  Control.addVariable(hutName+"Height",279.4);

  Control.addVariable(hutName+"RingStepLength",992.0);
  Control.addVariable(hutName+"RingStepWidth",200.0);

  Control.addVariable(hutName+"NChicane",2);

  PGen.setSize(4.0,60.0,40.0);
  PGen.generatePortChicane(Control,hutName+"Chicane0","Right",-230.0,-15.0);
  PGen.generatePortChicane(Control,hutName+"Chicane1","Right",400.0,-15.0);

  // Forklift truck holesa
  Control.addVariable(hutName+"ForkNHoles",0);
  Control.addVariable(hutName+"ForkWall","Outer");
  Control.addVariable(hutName+"ForkYStep",80.0);
  Control.addVariable(hutName+"ForkLength",60.0);
  Control.addVariable(hutName+"ForkHeight",10.0);
  Control.addVariable(hutName+"ForkZStep0",-115.0);
  Control.addVariable(hutName+"ForkZStep1",0.0);
  Control.addVariable(hutName+"ForkZStep2",80.0);

  //  Control.addVariable(hutName+"Hole0Offset",Geometry::Vec3D(23.0,0,4));
  Control.addVariable(hutName+"Hole0XStep",0.0);
  Control.addVariable(hutName+"Hole0ZStep",0.0);
  Control.addVariable(hutName+"Hole0Radius",5.0);


  return;
}

void
exptHutVariables(FuncDataBase& Control,
		 const std::string& beamName,
		 const double beamXStep)
  /*!
    Experimenta hutch variables
    \param Control :: DataBase to add
    \param beamName :: Beamline name
    \param bremXStep :: Offset of beam from main centre line
  */
{
  ELog::RegMethod RegA("tomowiseVariables[F]","exptHutVariables");

  setVariable::ExptHutGenerator EGen;

  const double beamOffset(-1.0);
  const std::string hutName(beamName+"ExptHut");

  EGen.setFrontHole(beamXStep-beamOffset,0.0,3.0);
  //  EGen.addHole(Geometry::Vec3D(beamOffset,0,0),3.5);
  EGen.setBackExt(10.0);
  EGen.setFrontExt(10.0);
  EGen.setCorner(30.0, 210.0);   // angle, step-y back - hutch length discussion 250919
  EGen.generateHut(Control,hutName,0.0,2037.6); // hutch length discussion 250919

  Control.addVariable(hutName+"Height",277.0);
  Control.addVariable(hutName+"FrontVoid",0.0);
  Control.addVariable(hutName+"OutWidth",260.0); // discussion with AR 250829 (we define the width)
  Control.addVariable(hutName+"RingWidth",260.0); // discussion with AR 250829 (we define the width)
  Control.addVariable(hutName+"PbWallThick",3.0); // optimised to be good enough
  Control.addVariable(hutName+"PbRoofThick",3.0); // same as the wall since at ~ the same distance to the beamline


  // // lead shield on pipe
  // Control.addVariable(hutName+"GuillotineXStep",beamXStep-beamOffset);
  // Control.addVariable(hutName+"GuillotineYStep",0.3);
  // Control.addVariable(hutName+"GuillotineLength",1.0);
  // Control.addVariable(hutName+"GuillotineWidth",10.0);
  // Control.addVariable(hutName+"GuillotineHeight",10.0);
  // Control.addVariable(hutName+"GuillotineWallThick",0.2);
  // Control.addVariable(hutName+"GuillotineClearGap",0.3);
  // Control.addVariable(hutName+"GuillotineWallMat","Stainless304");
  // Control.addVariable(hutName+"GuillotineMat","Lead");

  Control.addVariable(hutName+"NChicane",5);
  PortChicaneGenerator PGen;
  PGen.setSize(4.0,60.0,40.0);
  PGen.generatePortChicane(Control,hutName+"Chicane0","Left",0.0,-5.0);

  PGen.generatePortChicane(Control,hutName+"Chicane1","Right",50.0,-5.0);
  PGen.generatePortChicane(Control,hutName+"Chicane2","Right",-50.0,-5.0);
  PGen.generatePortChicane(Control,hutName+"Chicane3","Right",-150.0,-5.0);
  PGen.generatePortChicane(Control,hutName+"Chicane4","Right",-250.0,-5.0);

  PGen.setSize(4.0,40.0,40.0);
  //  PGen.generatePortChicane(Control,hutName+"Chicane5","Right",-150.0,-5.0);
  //  PGen.generatePortChicane(Control,hutName+"Chicane4","Back",-180,-5.0);

  // Forklift truck holes
  Control.addVariable(hutName+"ForkNHoles",0);
  Control.addVariable(hutName+"ForkWall","Ring");
  Control.addVariable(hutName+"ForkYStep",680.0);
  Control.addVariable(hutName+"ForkLength",60.0);
  Control.addVariable(hutName+"ForkHeight",10.0);
  Control.addVariable(hutName+"ForkZStep0",-115.0);
  Control.addVariable(hutName+"ForkZStep1",0.0);
  Control.addVariable(hutName+"ForkZStep2",80.0);
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
  ELog::RegMethod RegA("tomowiseVariables","shieldVariables");

  Control.addVariable(preName+"GuillotineLength",5.0);
  Control.addVariable(preName+"GuillotineWidth",60.0);
  Control.addVariable(preName+"GuillotineHeight",60.0);
  Control.addVariable(preName+"GuillotineWallThick",0.5);
  Control.addVariable(preName+"GuillotineClearGap",0.3);
  Control.addVariable(preName+"GuillotineWallMat","Stainless304");
  Control.addVariable(preName+"GuillotineMat","Lead");

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
  ELog::RegMethod RegA("tomowiseVariables[F]","mirrorBox");

  setVariable::MonoBoxGenerator VBoxGen;
  setVariable::MirrorGenerator MirrGen;

  const double mainLength(168.0);
  const double mainMirrorAngleA(0.0);
  const double mainMirrorAngleB(90.0);
  const double centreDist(55.0);
  const double aMirrorDist((mainLength+centreDist)/2.0);
  const double bMirrorDist((mainLength-centreDist)/2.0);

  const double heightDelta=tan(2.0*std::abs(phi)*M_PI/180.0)*aMirrorDist;
  //  const double mirrorDelta=tan(2.0*std::abs(phi)*M_PI/180.0)*centreDist;
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
  ELog::RegMethod RegA("tomowiseVariables[F]","crlPackage");
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
  ELog::RegMethod RegA("tomowiseVariables[F]","diagPackage");

  setVariable::BellowGenerator BellowGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::PortTubeGenerator PTubeGen;
  setVariable::PortItemGenerator PItemGen;
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
  ELog::RegMethod RegA("tomowiseVariables[F]","monoVariables");


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
diagUnit(FuncDataBase& Control,const std::string& Name)
  /*!
    Construct variables for the diagnostic units
    \param Control :: Database
    \param Name :: component name
  */
{
  ELog::RegMethod RegA("tomowiseVariables[F]","diagUnit");


  const double DLength(65.0);         // diag length [checked]
  setVariable::PortTubeGenerator PTubeGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::BeamPairGenerator BeamMGen;

  PTubeGen.setMat("Stainless304");

  // ports offset by 24.5mm in x direction
  // length 425+ 75 (a) 50 b
  PTubeGen.setPipe(9.0,0.5);
  PTubeGen.setPortCF<CF40>();
  PTubeGen.setPortLength(-3.0,-3.0);

  // ystep/radius length
  PTubeGen.generateTube(Control,Name,0.0,DLength);
  Control.addVariable(Name+"NPorts",5);

  const std::string portName=Name+"Port";
  const Geometry::Vec3D MidPt(0,0,0);
  const Geometry::Vec3D XVec(1,0,0);
  const Geometry::Vec3D ZVec(0,0,1);
  const Geometry::Vec3D PPosA(0.0,-DLength/3.2,0);
  const Geometry::Vec3D PPosB(0.0,-DLength/4.0,0);
  const Geometry::Vec3D PPosC(0.0,DLength/8.0,0);
  const Geometry::Vec3D PPosD(0.0,DLength/3.0,0);
  const Geometry::Vec3D PPosE(0.0,DLength/3.0,0);


  PItemGen.setOuterVoid(1);
  PItemGen.setCF<setVariable::CF150>(24.3);
  PItemGen.generatePort(Control,portName+"0",PPosA,ZVec);
  PItemGen.generatePort(Control,portName+"1",PPosB,-XVec);
  PItemGen.setCF<setVariable::CF40>(14.0);
  PItemGen.generatePort(Control,portName+"2",PPosC,-XVec);
  PItemGen.setCF<setVariable::CF63>(12.0);
  PItemGen.generatePort(Control,portName+"3",PPosD,-XVec);
  PItemGen.setCF<setVariable::CF100>(18.0);
  PItemGen.generatePort(Control,portName+"4",PPosE,ZVec);

  //

  const std::string jawKey[]={"JawX","JawZ"};
  for(size_t i=0;i<2;i++)
    {
      const std::string fname=Name+jawKey[i];
      if (i) BeamMGen.setXYStep(-1.2,-2.5,1.2,2.5);
      BeamMGen.generateMount(Control,fname,1);  // out of beam
    }

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
  ELog::RegMethod RegA("tomowiseVariables[F]","mirrorMonoPackage");

  setVariable::PortItemGenerator PItemGen;
  setVariable::VacBoxGenerator MBoxGen;
  setVariable::MLMonoGenerator MXtalGen;

  setVariable::MLMDetailGenerator MLGen;

  MLGen.generateMono(Control,monoKey+"MLM",0.4,-0.4);


  // ystep/width/height/depth/length
  //
  MBoxGen.setCF<CF40>();   // set ports
  MBoxGen.setAllThick(1.5,2.5,1.0,1.0,1.0); // Roof/Base/Width/Front/Back
  MBoxGen.setPortLength(7.5,7.5); // La/Lb
  MBoxGen.setBPortOffset(-0.4,0.0);    // note -1mm from crystal offset
  // width / heigh / depth / length
  MBoxGen.generateBox
    (Control,monoKey+"MLMVessel",54.7,12.5,31.0,92.7);  // 470mm height

  Control.addVariable(monoKey+"MLMVesselPortBXStep",0.0);   // from primary

  const std::string portName=monoKey+"MLMVessel";
  Control.addVariable(monoKey+"MLMVesselNPorts",0);   // beam ports (lots!!)
  PItemGen.setCF<setVariable::CF63>(5.0);
  PItemGen.setPlate(4.0,"LeadGlass");
  PItemGen.generatePort(Control,portName+"Port0",
			Geometry::Vec3D(0,5.0,-10.0),
			Geometry::Vec3D(1,0,0));

  // crystals gap 10mm
  //  MXtalGen.setGap(1.0);
  //  MXtalGen.generateMono(Control,monoKey+"MLM",-20.0,0.6,0.6);
  //  Control.addVariable(monoKey+"MLMYAngle",0.0);

  // pipe alternative to MLM
  setVariable::PipeGenerator PipeGen;
  PipeGen.setMat("Stainless304");
  PipeGen.setCF<CF40>();
  PipeGen.setOuterVoid(1);
  PipeGen.generatePipe(Control,monoKey+"MLMReplacementPipe",109.7);

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
  ELog::RegMethod RegA("tomowiseVariables[F]","opticsVariables");

  const std::string preName(beamName+"OpticsLine");

  Control.addVariable(preName+"OuterLeft",70.0);
  Control.addVariable(preName+"OuterRight",50.0);
  Control.addVariable(preName+"OuterTop",60.0);

  Control.addVariable(preName+"FM1Active",1);
  Control.addVariable(preName+"DCMActive",1);
  Control.addVariable(preName+"MLMActive",1);

  setVariable::PipeGenerator PipeGen;
  setVariable::BellowGenerator BellowGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::FlangeMountGenerator FlangeGen;
  setVariable::TriggerGenerator TGen;
  setVariable::CylGateValveGenerator GVGen;
  setVariable::SqrFMaskGenerator FMaskGen;
  setVariable::IonGaugeGenerator IGGen;
  setVariable::BremBlockGenerator MaskGen;

  PipeGen.setNoWindow();   // no window

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,preName+"InitBellow",6.0);

  // will be rotated vertical
  TGen.setCF<CF100>();
  TGen.setVertical(15.0,25.0);
  TGen.setSideCF<setVariable::CF40>(10.0); // add centre distance?
  TGen.generateTube(Control,preName+"TriggerUnit");

  constexpr double gateTubePortRadius = 1.9;
  GVGen.generateGate(Control,preName+"GateTubeA",0);  // open
  Control.addVariable(preName+"GateTubeAPortRadius", gateTubePortRadius);

  PipeGen.setMat("Stainless304");
  PipeGen.setCF<CF40>();
  PipeGen.setOuterVoid(1);
  PipeGen.generatePipe(Control,preName+"PipeA",24.4); // to put FM1 at its correct place

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,preName+"BellowA",12.6);


  constexpr double FM1dist = 2360.0;
  constexpr double FM1length = 12.0;
  FMaskGen.setCF<CF63>();
  FMaskGen.setFrontGap(2.13,2.146);
  FMaskGen.setBackGap(0.756,0.432);
  FMaskGen.setMinAngleSize(5.6,FM1dist, 60.0, 60.0); // last two arguments: opening angles in urad
  // step to +7.5 to make join with fixedComp:linkpt
  FMaskGen.generateColl(Control,preName+"FM1",7.5,FM1length);

  // pipe alternative to FM1
  PipeGen.setMat("Stainless304");
  PipeGen.setCF<CF40>();
  PipeGen.generatePipe(Control,preName+"FM1ReplacementPipe",FM1length);


  IGGen.setCF<CF150>();
  IGGen.setMainLength(12.0-1.65,8.0);
  IGGen.generateTube(Control,preName+"BremHolderA");

  constexpr double bcAlength = 8.0;
  MaskGen.setLength(bcAlength);
  // x-gap size: 1 mrad (2.386 cm)    + 0.2 cm (safety margin) \approx 2.6
  //          we will vary the safety margin
  // y-gap size: 0.1 mrad (0.2386 cm) + 0.1 cm (safety margin) \approx 0.35
  MaskGen.setAperatureAngle(bcAlength*2./3.,  2.6, 0.35, 2*1.5, 2*2.5);
  MaskGen.generateBlock(Control,preName+"BremCollA",-4.0);
  Control.addVariable(preName+"BremCollAHeight",10.0);
  Control.addVariable(preName+"BremCollAWidth",10.0);

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,preName+"BellowB",7.50);

  PipeGen.generatePipe(Control,preName+"BremPipeA",5.0);

  diagUnit(Control,preName+"DiagBoxA");

  PipeGen.setCF<CF40>();
  PipeGen.generatePipe(Control,preName+"PipeB",7.5);
  GVGen.generateGate(Control,preName+"GateTubeB",0);  // open
  Control.addVariable(preName+"GateTubeBPortRadius", gateTubePortRadius);
  BellowGen.generateBellow(Control,preName+"BellowC",15.0-0.45);
  BellowGen.generateBellow(Control,preName+"BellowCA",15.0-1.1769860+0.45); // length adjusted to place (TomoWISEOpticsLineMLMXstalA) at 2590 cm

  const std::string name=preName+"PowerFilterVessel";
  //  constexpr double pfLength = 44.223014; // dummy total power filter vessel length to place the first mirror correctly
  PipeGen.generatePipe(Control,preName+"PipeBA",5.0);
  Control.addVariable(preName+"PipeBAFlangeBRadius", CF200::outerRadius);
  PipeGen.generatePipe(Control,preName+"PipeBB",5.0);
  Control.addVariable(preName+"PipeBBFlangeARadius", CF200::outerRadius);
  SimpleTubeGen.setCF<CF200>();
  // There are no flanges -> set their radii to be the same as the outer radius
  // CL will not build their empty cells. The lenth parameter is irellevant.
  SimpleTubeGen.setAFlange(CF200::outerRadius, CF200::flangeLength);
  SimpleTubeGen.setBFlange(CF200::outerRadius, CF200::flangeLength);
  SimpleTubeGen.generateTube(Control,name,20.4);
  Control.addVariable(name+"NPorts",1);

  PItemGen.setCF<setVariable::CF150>(CF200::outerRadius+14.5); // measured
  PItemGen.setPlate(2.0,"Stainless304"); // dummy - another component above the port (TODO)
  PItemGen.generatePort(Control,name+"Port0",
			Geometry::Vec3D(0,0,0),
			Geometry::Vec3D(0,0,1));
  PItemGen.generatePort(Control,name+"Port1",
			Geometry::Vec3D(0,0,0),
			Geometry::Vec3D(0,0,-1));



  PowerFilterGenerator PFGen;
  PFGen.generate(Control,preName+"PowerFilter");


  mirrorMonoPackage(Control,preName);

  BellowGen.generateBellow(Control,preName+"BellowD",10.0);
  PipeGen.generatePipe(Control,preName+"PipeC",50.0+10-56.073014);
  GVGen.generateGate(Control,preName+"GateTubeC",0);  // open
  Control.addVariable(preName+"GateTubeCPortRadius", gateTubePortRadius);
  BellowGen.generateBellow(Control,preName+"BellowE",10.0);

  hdcmPackage(Control,preName);

  BellowGen.generateBellow(Control,preName+"BellowF",15.0-5-4); // length adjusted to place BremCollB
  PipeGen.generatePipe(Control,preName+"PipeD",12.5+5-14.45);// length adjusted to place BremCollB

  GVGen.generateGate(Control,preName+"GateTubeD",0);  // open
  Control.addVariable(preName+"GateTubeDPortRadius", gateTubePortRadius);

  diag2Package(Control,preName);

  PipeGen.generatePipe(Control,preName+"PipeDA",178.0);// dummy length


  // formaxVar::mirrorBox(Control,preName,"A",-0.17,0.17);
  mirrorBox(Control,preName,"A",-0.146,0.146);

  safetyUnit(Control,preName);

  diag4Package(Control,preName);

  monoShutterVariables(Control,preName);

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
  ELog::RegMethod RegA("tomowiseVariables[F]","viewPackage");

  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::BellowGenerator BellowGen;
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
exptVariables(FuncDataBase& Control,
	      const std::string& beamName)
/*
    Vacuum expt components in the optics hutch
    \param Control :: Function data base
    \param beamName :: Name of beamline
  */
{
  ELog::RegMethod RegA("formaxVariables[F]","exptVariables");

  setVariable::BellowGenerator BellowGen;
  setVariable::SixPortGenerator CrossGen;
  setVariable::MonoBoxGenerator VBoxGen;
  setVariable::BoxJawsGenerator BJGen;
  setVariable::ConnectorGenerator CTGen;
  setVariable::CRLTubeGenerator DPGen;
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
  CrossGen.setXSideLength(6.5,6.5);
  CrossGen.setZSideLength(6.5,6.5);
  CrossGen.generateSixPort(Control,preName+"CrossA");

  BellowGen.generateBellow(Control,preName+"BellowC",7.5);

  BJGen.generateJaws(Control,preName+"JawBox",1.6,1.6);

  CTGen.generatePipe(Control,preName+"ConnectA",20.0);

  DPGen.generateLens(Control,preName+"CRLTubeA",1);

  CTGen.generatePipe(Control,preName+"ConnectB",20.0);

  PipeGen.generatePipe(Control,preName+"PipeA",12.5);

  SixGen.setCF<CF40>();
  SixGen.setLength(6.0,6.0);
  SixGen.setXSideLength(5.0,5.0);
  SixGen.setZSideLength(5.0,5.0);
  SixGen.generateSixPort(Control,preName+"SixPortA");


  setVariable::GateValveGenerator CGateGen;
  CGateGen.setBladeMat("Aluminium");
  CGateGen.setBladeThick(0.8);
  CGateGen.setLength(1.2);
  CGateGen.setCylCF<CF40>();
  CGateGen.generateValve(Control,preName+"CylGateA",0.0,0);

  PipeGen.generatePipe(Control,preName+"PipeB",118.0-15);

  BellowGen.generateBellow(Control,preName+"BellowD",15.0);

  SixGen.generateSixPort(Control,preName+"SixPortB");

  PipeGen.generatePipe(Control,preName+"PipeC",118.0-15);

  CGateGen.generateValve(Control,preName+"CylGateB",0.0,0);

  SixGen.generateSixPort(Control,preName+"SixPortC");

  PipeGen.generatePipe(Control,preName+"PipeD",12.5);

  CTGen.generatePipe(Control,preName+"ConnectC",20.0);

  DPGen.generateLens(Control,preName+"CRLTubeB",1);

  CTGen.generatePipe(Control,preName+"ConnectD",20.0);

  viewPackage(Control,preName);

  BellowGen.generateBellow(Control,preName+"BellowE",15.0);

  CrossGen.generateSixPort(Control,preName+"CrossB");

  PipeGen.generatePipe(Control,preName+"AdjustPipe",33.0);

  PipeGen.generatePipe(Control,preName+"PipeE",7.5);

  BJGen.generateJaws(Control,preName+"JawBoxB",1.6,1.6);

  CTGen.setOuter(2.5,0.5);
  CTGen.setPort(CF40::flangeRadius,0.5*CF40::flangeLength);
  CTGen.generatePipe(Control,preName+"ConnectE",5.0);

  PipeGen.setCF<CF16>();
  PipeGen.setAFlangeCF<CF40>();
  PipeGen.setBFlange(1.1,0.1);
  PipeGen.generatePipe(Control,preName+"EndPipe",41.0);

  Control.addVariable(preName+"SampleYStep", 25.0); // [2]
  Control.addVariable(preName+"SampleRadius", 5.0); // [2]
  Control.addVariable(preName+"SampleMat", "Stainless304");


  return;
}

}  // NAMESPACE tomowiseVar

void
TOMOWISEvariables(FuncDataBase& Control)
  /*!
    Function to set the control variables and constants
    -- This version is for Photon Moderator
    \param Control :: Function data base to add constants too
  */
{
  ELog::RegMethod RegA("tomowiseVariables[F]","TOMOWISEvariables");

  Control.addVariable("sdefType","Wiggler");

  setVariable::PipeGenerator PipeGen;
  PipeGen.setNoWindow();
  PipeGen.setMat("Aluminium");

  const std::string beamLineName("TomoWISE");
  const std::string frontKey(beamLineName+"FrontBeam");

  tomowiseVar::undulatorVariables(Control,frontKey);
  setVariable::R3FrontEndToyamaVariables(Control,beamLineName);
  tomowiseVar::frontMaskVariables(Control,frontKey);


  PipeGen.setMat("Stainless304");
  PipeGen.setCF<setVariable::CF40>();
  PipeGen.generatePipe(Control,"TomoWISEJoinPipe",145.0+31.97);
  Control.addVariable("TomoWISEJoinPipeFlipX",1);

  tomowiseVar::opticsHutVariables(Control,"TomoWISEOpticsHut");
  tomowiseVar::opticsVariables(Control,"TomoWISE");

  PipeGen.setCF<setVariable::CF40>();
  PipeGen.generatePipe(Control,"TomoWISEJoinPipeB",300.0);

  setVariable::MainBeamDumpGenerator MainBDGen;
  std::string name="TomoWISEBeamStop";
  MainBDGen.generate(Control,name);
  const double beamStopCoreLength = 15.0;
  const double beamStopWallThick = 10.0;
  const double beamStopWallDistance = 100.0;
  Control.addVariable(name+"Length",beamStopCoreLength);
  Control.addVariable(name+"Height",5.0);
  Control.addVariable(name+"Depth",5.0);
  Control.addVariable(name+"Width",15.0);
  Control.addVariable(name+"WallThick",beamStopWallThick);
  Control.addVariable(name+"YStep",-beamStopWallDistance-beamStopCoreLength-beamStopWallThick);
  Control.addVariable(name+"Mat","Tungsten");
  Control.addVariable(name+"WallMat","Void"); // poly is not needed
  Control.addVariable(name+"TargetHeight",5.0);
  Control.addVariable(name+"TargetWidth",5.0);
  Control.addVariable(name+"TargetMat","Tungsten");
  Control.addVariable(name+"TargetLength",0.5);
  Control.addVariable(name+"PortLength",15.0-3.9);
  Control.addVariable(name+"PortRadius",4.9);

  tomowiseVar::shieldVariables(Control,"TomoWISE");

  tomowiseVar::exptHutVariables(Control,"TomoWISE",0.0);
  tomowiseVar::exptVariables(Control,"TomoWISE");
  //  tomowiseVar::exptLineBVariables(Control,"TomoWISE");
  //  tomowiseVar::detectorTubePackage(Control,"TomoWISE");

  return;
}

}  // NAMESPACE setVariable
