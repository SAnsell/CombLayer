/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   singleItemBuild/singleItemVariables.cxx
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
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
#include <iterator>
#include <memory>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"

#include "CFFlanges.h"
#include "CryoGenerator.h"
#include "BladeGenerator.h"
#include "PipeGenerator.h"
#include "CornerPipeGenerator.h"
#include "LeadPipeGenerator.h"
#include "TwinBaseGenerator.h"
#include "TwinGenerator.h"
#include "TwinFlatGenerator.h"
#include "ChopperGenerator.h"
#include "DipoleGenerator.h"
#include "DipoleExtractGenerator.h"
#include "QuadrupoleGenerator.h"
#include "LinacQuadGenerator.h"
#include "LinacSexuGenerator.h"
#include "SexupoleGenerator.h"
#include "OctupoleGenerator.h"
#include "EPSeparatorGenerator.h"
#include "EPCombineGenerator.h"
#include "EPContinueGenerator.h"
#include "PreDipoleGenerator.h"
#include "DipoleChamberGenerator.h"
#include "DipoleSndBendGenerator.h"
#include "R3ChokeChamberGenerator.h"
#include "MagnetM1Generator.h"
#include "MagnetU1Generator.h"
#include "MagnetBlockGenerator.h"
#include "CorrectorMagGenerator.h"
#include "QuadUnitGenerator.h"
#include "CurveMagGenerator.h"
#include "CylGateValveGenerator.h"
#include "GateValveGenerator.h"
#include "DipoleDIBMagGenerator.h"
#include "EArrivalMonGenerator.h"
#include "StriplineBPMGenerator.h"
#include "ButtonBPMGenerator.h"
#include "CeramicGapGenerator.h"
#include "BeamDividerGenerator.h"
#include "MainBeamDumpGenerator.h"
#include "EBeamStopGenerator.h"
#include "ScrapperGenerator.h"
#include "FlatPipeGenerator.h"
#include "SixPortGenerator.h"
#include "CrossWayGenerator.h"
#include "VacBoxGenerator.h"
#include "DCMTankGenerator.h"
#include "GaugeGenerator.h"
#include "BremBlockGenerator.h"
#include "PrismaChamberGenerator.h"
#include "TriPipeGenerator.h"
#include "TriGroupGenerator.h"
#include "subPipeUnit.h"
#include "MultiPipeGenerator.h"
#include "PitGenerator.h"
#include "ScreenGenerator.h"
#include "CooledScreenGenerator.h"
#include "YagScreenGenerator.h"
#include "BeamScrapperGenerator.h"
#include "YagUnitGenerator.h"
#include "YagUnitBigGenerator.h"
#include "TWCavityGenerator.h"
#include "UndVacGenerator.h"
#include "FMUndulatorGenerator.h"
#include "CollGenerator.h"
#include "SqrFMaskGenerator.h"
#include "BellowGenerator.h"
#include "PipeTubeGenerator.h"
#include "PortTubeGenerator.h"
#include "PortItemGenerator.h"
#include "JawFlangeGenerator.h"
#include "CleaningMagnetGenerator.h"
#include "IonPTubeGenerator.h"
#include "IonGaugeGenerator.h"
#include "CollTubeGenerator.h"
#include "CollUnitGenerator.h"
#include "TriggerGenerator.h"
#include "NBeamStopGenerator.h"
#include "BremTubeGenerator.h"
#include "HPJawsGenerator.h"
#include "BoxJawsGenerator.h"
#include "DiffXIADP03Generator.h"
#include "CRLTubeGenerator.h"
#include "ViewScreenGenerator.h"
#include "PortChicaneGenerator.h"
#include "ConnectorGenerator.h"
#include "LocalShieldingGenerator.h"
#include "FlangeDomeGenerator.h"
#include "DomeConnectorGenerator.h"
#include "BeamBoxGenerator.h"
#include "MonoShutterGenerator.h"
#include "FocusGenerator.h"
#include "MLMDetailGenerator.h"
#include "M1DetailGenerator.h"
#include "HeimdalCaveGenerator.h"

#include "RoundShutterGenerator.h"
#include "TubeDetBoxGenerator.h"

namespace setVariable
{

void exptHutVariables(FuncDataBase&,const std::string&,const double);
void localShieldVariables(FuncDataBase&);
void m1chamberDetails(FuncDataBase&);
void targetShieldVariables(FuncDataBase&);



void
SingleItemVariables(FuncDataBase& Control)
  /*!
    Function to set the control variables and constants
    -- This version is for ESS ()
    \param Control :: Function data base to add constants too
  */
{
  ELog::RegMethod RegA("singleItemVariables[F]","SingleItemVariables");
// -----------
// GLOBAL stuff
// -----------
  Control.addVariable("zero",0.0);     // Zero
  Control.addVariable("one",1.0);      // one

  targetShieldVariables(Control);
  // photon test
  Control.addVariable("TargetZStep",0.0);
  Control.addVariable("TargetRadius",5.0);
  Control.addVariable("TargetMat","Stainless304");

  Control.addVariable("ShieldInnerRadius",50.0);
  Control.addVariable("ShieldOuterRadius",100.0);
  Control.addVariable("ShieldMat","Stainless304");

  Control.addVariable("TubeYStep",10.0);
  Control.addVariable("TubeRadius",5.0);
  Control.addVariable("TubeLength",30.0);
  Control.addVariable("TubeDepth",10.0);
  Control.addVariable("TubeWidth",20.0);
  Control.addVariable("TubeHeight",20.0);
  Control.addVariable("TubeMat","Stainless304");

  Control.addVariable("TubeARadius",5.0);
  Control.addVariable("TubeALength",25.0);
  Control.addVariable("TubeBRadius",5.0);
  Control.addVariable("TubeBLength",5.0);
  Control.addVariable("TubeAMat","Stainless304");
  Control.addVariable("TubeBMat","Lead");

  Control.addVariable("MagTubeRadius",3.0);
  Control.addVariable("MagTubeLength",25.0);
  Control.addVariable("MagTubeMat","Stainless304");

  Control.addVariable("MagTubeBXStep",-300.0);
  Control.addVariable("MagTubeBRadius",3.0);
  Control.addVariable("MagTubeBLength",25.0);
  Control.addVariable("MagTubeBMat","Stainless304");


  Control.addVariable("CryoBOuterRadius",20.0);

  Control.addVariable("CryoBNLayers",0);
  Control.addVariable("CryoBLRadius0",1.0);
  Control.addVariable("CryoBLRadius1",3.0);
  Control.addVariable("CryoBLRadius2",5.0);
  Control.addVariable("CryoBLRadius3",8.0);
  Control.addVariable("CryoBLThick",0.5);
  Control.addVariable("CryoBLTemp",300.0);

  Control.addVariable("CryoBTopOffset",1.0);
  Control.addVariable("CryoBBaseOffset",1.0);
  Control.addVariable("CryoBCutTopAngle",10.0);
  Control.addVariable("CryoBCutBaseAngle",10.0);

  Control.addVariable("CryoBTopThick",10.0);
  Control.addVariable("CryoBBaseThick",10.0);

  Control.addVariable("CryoBApertureWidth",2.0);
  Control.addVariable("CryoBApertureHeight",2.0);

  Control.addVariable("CryoBMat","Aluminium");

  setVariable::PitGenerator CPitGen;
  CPitGen.setFeLayer(6.0);
  CPitGen.setConcLayer(10.0);
  CPitGen.generatePit(Control,"ChopperPit",0.0,340.0,150.0,120.0,30.0);

  setVariable::EArrivalMonGenerator EMonGen;
  EMonGen.generateMon(Control,"BeamMon",0.0);

  setVariable::CryoGenerator CryGen;
  CryGen.generateFridge(Control,"singleCryo",3.0,-10,4.5);

  setVariable::PrismaChamberGenerator PCGen;
  PCGen.generateChamber(Control,"PrismaChamber");

  setVariable::TwinGenerator TGen;
  TGen.generateChopper(Control,"singleTwinB",0.0,16.0,10.0);

  setVariable::TwinFlatGenerator TCGen;
  TCGen.generateChopper(Control,"singleTwinC",0.0,16.0,10.0);

  TCGen.generateChopper(Control,"singleTwinD",80.0,16.0,10.0);

  setVariable::PipeGenerator RPipeGen;
  RPipeGen.setCF<CF100>();
  RPipeGen.setRectPipe(5.0,6.0);
  RPipeGen.generatePipe(Control,"singleBoxPipeA",80.0);
  RPipeGen.generatePipe(Control,"singleBoxPipeB",90.0);

  setVariable::CornerPipeGenerator CPipeGen;
  CPipeGen.generatePipe(Control,"CornerPipe",80.0);

  setVariable::ChopperGenerator CGen;
  CGen.setMainRadius(25.0);
  CGen.setFrame(65.0,65.0);
  CGen.generateChopper(Control,"singleChopper",38.0,10.0,4.55);
  /*
  CGen.setMotorRadius(10.0);
  CGen.generateChopper(Control,"singleChopper",10.0,12.0,5.55);
  Control.addVariable("singleChopperMotorBodyLength",15.0);
  */

  setVariable::BladeGenerator BGen;
  // Single Blade chopper
  BGen.setThick({0.2});
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.generateBlades(Control,"singleBBladeTop",-2.0,22.5,35.0);

  // Single Blade chopper
  BGen.setThick({0.2});
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.generateBlades(Control,"singleBBladeLow",2.0,22.5,35.0);


  // collimator block
  setVariable::SqrFMaskGenerator FMaskGen;

  // B is standard
  FMaskGen.generateColl(Control,"FMask",0.0,15.0);

  FMaskGen.setPipeRadius(-10.0);
  // FMaskGen.setCF<CF63>();
  // FMaskGen.setBFlangeCF<CF40>();
  // FMaskGen.setFrontGap(3.99,1.97);  //1033.8
  // FMaskGen.setBackGap(0.71,0.71);
  // FMaskGen.setMinSize(10.2,0.71,0.71);
  FMaskGen.generateColl(Control,"FMaskB",40.0,15.0);


  setVariable::EPSeparatorGenerator EPSGen;
  EPSGen.generatePipe(Control,"EPSeparator");

  setVariable::DipoleChamberGenerator DCGen;
  DCGen.generatePipe(Control,"DipoleChamber",0.0);

  setVariable::DipoleExtractGenerator DEGen;
  DEGen.generatePipe(Control,"DipoleExtract",0.0);

  setVariable::DipoleSndBendGenerator DBGen;
  DBGen.generatePipe(Control,"DipoleSndBend",0.0);

  setVariable::R3ChokeChamberGenerator CCGen;
  CCGen.generateChamber(Control,"R3Chamber");

  setVariable::MagnetM1Generator M1Gen;
  M1Gen.generateBlock(Control,"M1Block");

  setVariable::MagnetU1Generator U1Gen;
  U1Gen.generateBlock(Control,"U1Block");


  setVariable::EPCombineGenerator EPCGen;
  EPCGen.generatePipe(Control,"EPCombine");

  setVariable::EPContinueGenerator EPCCGen;
  EPCCGen.generatePipe(Control,"EPContinue");

  setVariable::QuadrupoleGenerator QGen;
  QGen.generateQuad(Control,"QFend",20.0,25.0);

  setVariable::OctupoleGenerator OGen;
  OGen.generateOcto(Control,"OXX",20.0,25.0);

  setVariable::DipoleGenerator DGen;
  DGen.generateDipole(Control,"DIM",0.0,60.0);

  setVariable::SexupoleGenerator SGen;
  SGen.generateHex(Control,"SXX",20.0,25.0);

  setVariable::SixPortGenerator SPGen;
  SPGen.generateSixPort(Control,"SixPort");
  SPGen.setCF<CF40>();
  SPGen.setSideCF<CF150>();
  SPGen.setXSideLength(70.0,70.0);
  SPGen.generateSixPort(Control,"FourPort");

  setVariable::CrossWayGenerator MSPGen;
  MSPGen.generateCrossWay(Control,"CrossWay");

  setVariable::GaugeGenerator GTGen;
  GTGen.generateGauge(Control,"GaugeTube",0.0,0.0);

  setVariable::BremBlockGenerator BBGen;
  BBGen.setAperature(-1,1.0,1.0,1.0,1.0,1.0,1.0);
  BBGen.generateBlock(Control,"BremBlock",0.0);

  setVariable::CrossWayGenerator CWBlankGen;
  CWBlankGen.setCF<CF63>();
  CWBlankGen.setMainLength(2.4,13.6);
  CWBlankGen.setPortLength(5.9,8.1);
  CWBlankGen.setCrossLength(6.25,6.25);
  CWBlankGen.setPortCF<CF35_TDC>();
  CWBlankGen.setCrossCF<CF35_TDC>();
  CWBlankGen.generateCrossWay(Control,"CrossBlank");

  setVariable::IonPTubeGenerator IonPGen;
  IonPGen.generateTube(Control,"IonPTube");

  setVariable::BremTubeGenerator BTGen;
  BTGen.generateTube(Control,"BremTube");

  setVariable::HPJawsGenerator HPGen;
  HPGen.generateJaws(Control,"HPJaws",0.3,0.3);

  setVariable::ConnectorGenerator CPGen;
  CPGen.generatePipe(Control,"ConnectorTube",20.0);

  setVariable::BoxJawsGenerator BJGen;
  BJGen.generateJaws(Control,"BoxJaws",0.3,0.3);

  setVariable::DiffXIADP03Generator DPXGen;
  DPXGen.generatePump(Control,"DiffXIA",54.4);

  setVariable::CRLTubeGenerator DPGen;
  DPGen.generateLens(Control,"CRLTube",1);

  setVariable::MonoShutterGenerator MSGen;
  MSGen.generateShutter(Control,"MS",1,1);

  setVariable::RoundShutterGenerator RMSGen;
  RMSGen.generateShutter(Control,"RMS",1,1);

  setVariable::ViewScreenGenerator VTGen;
  VTGen.generateView(Control,"ViewTube");

  setVariable::IonGaugeGenerator IonGGen;
  IonGGen.generateTube(Control,"IonGauge");

  setVariable::CollTubeGenerator CTGen;
  CTGen.generateTube(Control,"CollTube",25.0);

  setVariable::CollUnitGenerator CUGen;
  CUGen.generateScreen(Control,"CollUnit");

  setVariable::TriggerGenerator TrigGen;
  TrigGen.generateTube(Control,"TriggerTube");

  setVariable::DCMTankGenerator DCMGen;
  DCMGen.setCF<CF40>();   // set ports
  DCMGen.setPortLength(7.5,7.5); // La/Lb
  DCMGen.setBPortOffset(-0.6,0.0);    // note -1mm from crystal offset
  // radius : Height / depth  [need heigh = 0]
  Control.addVariable("DMCPortBXStep",-0.6);      // from primary
  DCMGen.generateBox(Control,"DCMTank",30.0,0.0,16.0);

  // multipipe
  setVariable::MultiPipeGenerator MPGen;
  MPGen.setPipe<CF40>(Geometry::Vec3D(0,0,5.0),45.0, 0.0, 3.7);
  MPGen.setPipe<CF40>(Geometry::Vec3D(0,0,0.0),41.0, 0.0, 0.0);
  MPGen.setPipe<CF40>(Geometry::Vec3D(0,0,-5.0),37.0, 0.0, -3.7);
  MPGen.generateMulti(Control,"MultiPipe");

  const double startWidth(2.33/2.0);
  const double endWidth(6.70/2.0);
  setVariable::TriPipeGenerator TPGen;
  TPGen.setBFlangeCF<CF100>();
  TPGen.setXYWindow(startWidth,startWidth,endWidth,endWidth);
  TPGen.generateTri(Control,"TriPipe");
  
  setVariable::TriGroupGenerator TGGen;
  //  TPGen.setBFlangeCF<CF100>();
  TGGen.generateTri(Control,"TriGroup");

  setVariable::FlatPipeGenerator FPGen;
  FPGen.generateFlat(Control,"FlatPipe",80.0);
  Control.addVariable("FlatPipeXYAngle",0);

  setVariable::LinacQuadGenerator LQGen;
  LQGen.generateQuad(Control,"LQ",20.0);

  setVariable::LinacSexuGenerator LSxGen;
  LSxGen.generateSexu(Control,"LS",20.0);

  // Block for new R1-M1
  setVariable::MagnetBlockGenerator MBGen;
  MBGen.generateBlock(Control,"MB1",0.0);

  setVariable::QuadUnitGenerator M1QGen;
  M1QGen.generatePipe(Control,"MB1QuadUnit",0.0);

  //  setVariable::DipoleChamberGenerator DCGen;
  DCGen.generatePipe(Control,"MB1DipoleChamber",0.0);

  DEGen.generatePipe(Control,"MB1DipoleExtract",0.0);

  DBGen.generatePipe(Control,"MB1DipoleSndBend",2.8);

  DEGen.setLength(82.0);
  DEGen.generatePipe(Control,"MB1DipoleOut",0.0);

  // Beam Stop
  setVariable::NBeamStopGenerator BS;
  BS.generateBStop(Control,"BeamStop",3.0);
  Control.addVariable("BeamStopYStep",3.0);
  // Beam Box
  setVariable::BeamBoxGenerator BX;
  BX.generateBox(Control,"BeamBox",3.0);

  // corrector mag
  setVariable::CorrectorMagGenerator CMGen;
  // last argument is vertical/horizontal switch
  CMGen.generateMag(Control,"CM",0.0,0);

  setVariable::PipeGenerator PGen;
  PGen.setNoWindow();
  PGen.setCF<setVariable::CF40_22>();
  PGen.setMat("Stainless316L");
  PGen.generatePipe(Control,"CorrectorMagPipe",40.0);
  Control.addVariable("CorrectorMagPipe",-20.0);

  LQGen.generateQuad(Control,"QF",20.0);

  PGen.setCF<setVariable::CF40_22>();
  PGen.generatePipe(Control,"QHVC",80.0);
  Control.addVariable("QHVC",-40.0);
  Control.addVariable("QHVCRadius",0.4);
  LQGen.setRadius(0.56, 2.31);
  LQGen.generateQuad(Control,"QH",20.0);
  Control.addVariable("QHLength",18.7);
  Control.addVariable("QHYokeOuter",9.5);
  Control.addVariable("QHPolePitch",26.0);

  // CylGateValve
  setVariable::CylGateValveGenerator GVGen;
  GVGen.generateGate(Control,"GV",0);

  // CylGateValve
  setVariable::GateValveGenerator CGateGen;

  CGateGen.setOuter(3.4,13.0,20.0,20.0);
  CGateGen.setBladeMat("Stainless304L");
  CGateGen.setBladeThick(0.8);
  CGateGen.setPortPairCF<CF40,CF63>();
  CGateGen.generateValve(Control,"GVCube",0.0,1);

  CGateGen.setCylCF<CF40>();
  CGateGen.generateValve(Control,"GVCylinder",0.0,1);

  //  dipole magnet DIB
  setVariable::DipoleDIBMagGenerator DIBGen;
  DIBGen.generate(Control,"DIB");

  PGen.setCF<setVariable::CF40_22>();
  PGen.generatePipe(Control,"VC",80.0);

  PGen.setWindow(1.5,0.2);
  PGen.generatePipe(Control,"VCWin",80.0);


  PGen.setCF<setVariable::CF40_22>();
  PGen.setNoWindow();
  PGen.generatePipe(Control,"DipolePipe",80.0);
  Control.addVariable("DipolePipeYStep",-40.0);

  setVariable::CooledScreenGenerator CoolGen;
  CoolGen.generateScreen(Control,"Cool",1);  // in beam
  Control.addVariable("CoolYAngle",-90.0);

  setVariable::BeamScrapperGenerator BeamSGen;
  BeamSGen.generateScreen(Control,"BeamScrapper");
  //  Control.addVariable("BeamScrapperXStep",2668.0016244323656);
  //  Control.addVariable("BeamScrapperYStep",0.0);
  //  Control.addVariable("BeamScrapperZAngle",-180.0);
  
  setVariable::YagScreenGenerator YagGen;
  YagGen.generateScreen(Control,"YAG",1);  // in beam
  Control.addVariable("YAGYAngle",-90.0);

  setVariable::CurveMagGenerator CMagGen;
  CMagGen.generateMag(Control,"CMag");

  setVariable::StriplineBPMGenerator BPMGen;
  BPMGen.generateBPM(Control,"BPM",0.0);

  setVariable::UndVacGenerator UVGen;
  UVGen.generateUndVac(Control,"UVac");
  setVariable::FMUndulatorGenerator UUGen;
  UUGen.generateUndulator(Control,"UVacUndulator",482.0);


  setVariable::CeramicGapGenerator CSGen;
  CSGen.generateCeramicGap(Control,"CerSep");

  setVariable::HeimdalCaveGenerator HCaveGen;
  HCaveGen.generateCave(Control,"HeimdalCave");

  setVariable::CF40 CF40unit;
  setVariable::BeamDividerGenerator BDGen(CF40unit);
  BDGen.generateDivider(Control,"BeamDiv",0.0);

  setVariable::MainBeamDumpGenerator MainBDGen;
  MainBDGen.generate(Control,"MainBeamDump");

  setVariable::EBeamStopGenerator EBGen;
  EBGen.generateEBeamStop(Control,"EBeam",0);

  setVariable::ScrapperGenerator SCGen;
  SCGen.generateScrapper(Control,"Scrapper",1.0);   // z lift


  setVariable::YagUnitGenerator YagUnitGen;
  YagUnitGen.generateYagUnit(Control,"YU");
  Control.addVariable("YUYAngle",90.0);

  setVariable::YagUnitBigGenerator YagUnitBigGen;
  YagUnitBigGen.generateYagUnit(Control,"YUBig");
  Control.addVariable("YUBigYAngle",90.0);

  // traveling wave cavity
  PGen.setNoWindow();
  PGen.setCF<setVariable::CF40_22>();
  PGen.setMat("Stainless304L","Stainless304L");
  setVariable::TWCavityGenerator TDCGen;

  const double flangeLength(3.7);

  setVariable::PipeGenerator TWPipeGen;
  TWPipeGen.setCF<CF100>();

  TWPipeGen.generatePipe(Control,"PipeA",4*flangeLength);
  Control.addVariable("PipeARadius",1.16);
  Control.addVariable("PipeAPipeThick",0.2);

  TDCGen.generate(Control,"TWCavity");

  TWPipeGen.generatePipe(Control,"PipeB",4.0*flangeLength);
  Control.addParse<double>("PipeBRadius","PipeARadius");
  Control.addParse<double>("PipeBPipeThick","PipeAPipeThick");

  // Bellow
  setVariable::BellowGenerator BellowGen;
  BellowGen.setCF<setVariable::CF40>();
  ELog::EM<<"HERE "<<ELog::endDiag;
  BellowGen.generateBellow(Control,"Bellow",7.5);

  // Lead Clad Pipe
  setVariable::LeadPipeGenerator LeadGen;
  LeadGen.setCF<setVariable::CF63>();
  LeadGen.generatePipe(Control,"LeadPipe",7.5);


  // Offset Pipe [use standard pipe and modify]
  setVariable::PipeGenerator OPipeGen;
  OPipeGen.setCF<setVariable::CF63>();
  OPipeGen.generatePipe(Control,"OFP",20.0);
  Control.addVariable("OFPFlangeAXYAngle",20.0);
  Control.addVariable("OFPFlangeAXStep",-1.5);
  Control.addVariable("OFPFlangeAZStep",1.3);
  Control.addVariable("OFPFlangeBXStep",1.0);
  Control.addVariable("OFPFlangeBZStep",2.0);


  // UTube Pipe
  setVariable::PipeGenerator UTubeGen;
  UTubeGen.setCF<setVariable::CF63>();
  UTubeGen.generatePipe(Control,"UTubePipe",20.0);
  Control.addVariable("UTubePipeWidth",6.0);
  Control.addVariable("UTubePipeHeight",0.6);
  Control.addVariable("UTubePipePipeThick",0.2);

  // Rectangle Pipe with different flanges
  setVariable::PipeGenerator RTubeGen;

  RTubeGen.setRectPipe(3.0,4.0,0.5);
  RTubeGen.setARectFlange(8.0,13.0,2.0);
  RTubeGen.setBRectFlange(12.0,8.0,3.0);
  RTubeGen.setRectWindow(6.0,7.0,0.3);
  RTubeGen.setOuterVoid();
  RTubeGen.generatePipe(Control,"RPipe",20.0);

  // PipeTube

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,"BellowTube",7.5);
  //  Control.addVariable("BellowTubeZAngle",30.0);

  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::PortItemGenerator PItemGen;
  SimpleTubeGen.setCF<CF63>();
  SimpleTubeGen.generateTube(Control,"PipeTube",20.0);

  Control.addVariable("PipeTubeNPorts",1);
  PItemGen.setCF<setVariable::CF40>(10.0);
  PItemGen.setNoPlate();
  PItemGen.generatePort(Control,"PipeTubePort0",
			Geometry::Vec3D(0.0, -3.0, 0.0),
			Geometry::Vec3D(-1.0, 0.0, 0.0));

  PItemGen.setCF<setVariable::CF40>(12.0);
  PItemGen.generatePort(Control,"PipeTubePort1",
			Geometry::Vec3D(0.0, 3.0, 0.0),
			Geometry::Vec3D(0.5, -0.5, 0.866));

  // PortTube
  setVariable::PortTubeGenerator PortTubeGen;
  PortTubeGen.setPipeCF<CF63>();
  PortTubeGen.setPortCF<CF40>();
  PortTubeGen.setPortLength(5.0,6.0);
  PortTubeGen.generateTube(Control,"PortTube",0.0,20.0);

  Control.addVariable("PortTubeNPorts",2);
  PItemGen.setCF<setVariable::CF40>(12.0);
  PItemGen.generatePort(Control,"PortTubePort0",
			Geometry::Vec3D(0.0, 3.0, 0.0),
			Geometry::Vec3D(0.5, -0.5, 0.866));
  PItemGen.setNoPlate();
  PItemGen.setCF<setVariable::CF40>(10.0);
  PItemGen.generatePort(Control,"PortTubePort1",
			Geometry::Vec3D(0.0, -3.0, 0.0),
			Geometry::Vec3D(-1.0, 0.0, 0.0));

  // BlankTube
  SimpleTubeGen.setCF<CF63>();
  SimpleTubeGen.generateBlank(Control,"BlankTube",20.0);
  // Control.addVariable("BlankTubeFlangeCapThick",setVariable::CF63::flangeLength);
  // Control.addVariable("BlankTubeFlangeCapMat","Lead");
  //  Control.addVariable("BlankTubeYAngle", 30.0);
  Control.addVariable("BlankTubeNPorts",1);
  PItemGen.setCF<setVariable::CF40>(16.0);
  PItemGen.setPlate(setVariable::CF40_22::flangeLength, "Aluminium");
  PItemGen.generatePort(Control,"BlankTubePort0",
			Geometry::Vec3D(0.0, 3.0, 0.0),
			Geometry::Vec3D(1,0,0));
  //			Geometry::Vec3D(0.5, -0.5, 0.866));

  PItemGen.setNoPlate();
  //  PItemGen.setCF<setVariable::CF40>(7.0);
  PItemGen.setLength(17.0);
  PItemGen.generatePort(Control,"BlankTubePort1",
			Geometry::Vec3D(0.0, -3.0, 0.0),
			Geometry::Vec3D(-1.0, 0.0, 0.0));
  //  Control.addVariable("BlankTubePort1WallMat","Stainless316L");

  // Button pickup BPM
  setVariable::ButtonBPMGenerator ButtonBPMGen;
  ButtonBPMGen.setCF<setVariable::CF40_22>();
  ButtonBPMGen.generate(Control,"ButtonBPM");

  // Cleaning magnet
  setVariable::CleaningMagnetGenerator ClMagGen;
  ClMagGen.generate(Control,"CleaningMagnet");

  // Jaws
  const std::string Name="DiagnosticBox";
  const double DLength(16.0);

  setVariable::PortTubeGenerator PTubeGen;

  PTubeGen.setMat("Stainless304");

  const double Radius(7.5);
  const double WallThick(0.5);
  const double portRadius(Radius+WallThick+0.5);
  PTubeGen.setPipe(Radius,WallThick);
  PTubeGen.setPortCF<setVariable::CF40>();
  const double sideWallThick(1.0);
  PTubeGen.setPortLength(-sideWallThick,sideWallThick);
  PTubeGen.setAFlange(portRadius,sideWallThick);
  PTubeGen.setBFlange(portRadius,sideWallThick);
  PTubeGen.generateTube(Control,Name,0.0,DLength);
  Control.addVariable(Name+"NPorts",4);

  const std::string portName=Name+"Port";
  const Geometry::Vec3D MidPt(0,1.5,0);
  const Geometry::Vec3D XVec(1,0,0);
  const Geometry::Vec3D ZVec(0,0,1);
  const Geometry::Vec3D PPos(0.0,DLength/8.0,0);

  // first 2 ports are with jaws, others - without jaws
  PItemGen.setOuterVoid(1);  // create boundary round flange
  PItemGen.setCF<setVariable::CF63>(portRadius+5.0);
  PItemGen.generatePort(Control,portName+"0",-PPos,ZVec);
  PItemGen.setCF<setVariable::CF63>(portRadius+10.0);
  PItemGen.generatePort(Control,portName+"1",MidPt,XVec);

  PItemGen.setCF<setVariable::CF63>(portRadius+5.0);
  PItemGen.generatePort(Control,portName+"2",-PPos,-ZVec);
  PItemGen.setCF<setVariable::CF63>(portRadius+10.0);
  PItemGen.generatePort(Control,portName+"3",MidPt,-XVec);

  // PItemGen.setCF<setVariable::CF63>(10.0);
  // PItemGen.generatePort(Control,portName+"4",MidPt,
  // 			Geometry::Vec3D(1,0,1));

  JawFlangeGenerator JFlanGen;
  JFlanGen.setSlits(2.5, 2.5, 0.4, "Tantalum"); // W,H,T,mat
  JFlanGen.generateFlange(Control,Name+"JawUnit0");
  JFlanGen.generateFlange(Control,Name+"JawUnit1");

  Control.addVariable(Name+"JawUnit0JOpen",1.7);
  Control.addVariable(Name+"JawUnit1JOpen",1.7);

  // Flange Dome components:
  FlangeDomeGenerator FDGen;
  PItemGen.setCF<setVariable::CF40>(10.0);
  PItemGen.setNoPlate();
  SimpleTubeGen.setCF<CF350>();
  SimpleTubeGen.generateTube(Control,"FlangeTube",20.0);
  Control.addVariable("FlangeTubeNPorts",0);

  FDGen.generateDome(Control,"FlangeDome");
  Control.addVariable("FlangeDomeNPorts",1);
  PItemGen.generatePort(Control,"FlangeDomePort0",
			Geometry::Vec3D(0.0, 0.0, 0.0),
			Geometry::Vec3D(0,1,0));

  // Flange DomeConnector  components:
  // uses FlangeTube above--
  DomeConnectorGenerator FDCGen;
  FDCGen.generateDome(Control,"DomeConnector",1);
  PItemGen.generatePort(Control,"DomeConnectorPort0",
			Geometry::Vec3D(0.0, 0.0, 0.0),
			Geometry::Vec3D(0,-1,0));



  // Double pipe
  SimpleTubeGen.setCF<CF63>();
  SimpleTubeGen.generateTube(Control,"DoublePipe",40.0);

  Control.addVariable("DoublePipeNPorts",1);
  PItemGen.setCF<setVariable::CF40>(30.0);
  PItemGen.setNoPlate();

  // second pipe
  setVariable::PortItemGenerator PItemGenB(PItemGen);
  PItemGenB.setCF<setVariable::CF63>(15.0);

  PItemGen.generateDoublePort
    (Control,"DoublePipePort0",
     PItemGenB,
     Geometry::Vec3D(0.0, -3.0, 0.0),
     Geometry::Vec3D(-1.0, 0.0, 0.0));

  MLMDetailGenerator MLGen;
  MLGen.generateMono(Control,"MLM",0.1,-0.1);

  m1chamberDetails(Control);

  TubeDetBoxGenerator TDBGen;
  TDBGen.generateBox(Control,"TDetBox",Geometry::Vec3D(0,3.15,0),8);

  // guideUnit variables
  setVariable::FocusGenerator FGen;
  FGen.setLayer(1,0.5,"Copper");
  FGen.setLayer(2,2.5,"Stainless304");
  FGen.setYOffset(2.0);
  //  FGen.generateRectangle(Control,"FA",100.0,5.0,8.0);
  //  FGen.generateTaper(Control,"FA",100.0,2,2,2,10);
  FGen.generateTaper(Control,"FA",26.0,2.63,2.14,3.75,4.76);

  const double bendAngle(0.0);   // relative to Z bend
  const double bendRadius(12000.0);    // 120m
  FGen.generateBender(Control,"BA",100.0,30,30,30,30,
                      bendRadius,bendAngle);
  //  FGen.generateBender(Control,"BA",100.0,3.0,3.0,3.0,3.0,
  //                      bendRadius,bendAngle);

  // expt hutch
  exptHutVariables(Control,"",0.0);
  localShieldVariables(Control);
  return;
}

void
m1chamberDetails(FuncDataBase& Control)
  /*!
    Construct the M1 detail
   */
{
  ELog::RegMethod RegA("singleItemVariable[F]","m1chamberDetails");

  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::M1DetailGenerator M1DGen;
  setVariable::PipeGenerator PipeGen;
  setVariable::DomeConnectorGenerator DCGen;
  setVariable::PortItemGenerator PItemGen;

  const double tubeLength(40.5);
  const double portXStep(2.0);
  const double theta(-0.0*M_PI/180.0);
  const double mExtra=30.0*sin(theta);


  const std::string frontName="M1TubeFront";
  const std::string tubeName="M1Tube";
  const std::string backName="M1TubeBack";

  PItemGen.setCF<setVariable::CF63>(6.0);
  PItemGen.setNoPlate();

  DCGen.setSphere(7.0,2.5);
  DCGen.setFlangeCF<CF150>(0.8);
  DCGen.setLengths(2.5,2.0,3.0);
  DCGen.generateDome(Control,frontName,1);
  PItemGen.generatePort(Control,frontName+"Port0",
			Geometry::Vec3D(portXStep, 0.0, 0.0),
			Geometry::Vec3D(0.0, -1.0, 0.0));
  //Control.addVariable(frontName+"ZAngle",-1.0);


  const std::string mName="M1Tube";
  SimpleTubeGen.setCF<CF150>();
  SimpleTubeGen.generateTube(Control,mName,tubeLength);
  Control.addVariable(mName+"WallMat","Titanium");
  Control.addVariable(mName+"NPorts",0);   // beam ports

  M1DGen.generateMirror(Control,"M1",0.0,0.0,0.0);
  Control.addVariable("M1XStep",portXStep+mExtra);

  DCGen.generateDome(Control,backName,1);
  PItemGen.generatePort(Control,backName+"Port0",
			Geometry::Vec3D(-portXStep, 0.0, 0.0),
			Geometry::Vec3D(0,-1,0));

}


void
targetShieldVariables(FuncDataBase& Control)
{
  // photon test
  Control.addVariable("TargetZStep",0.0);
  Control.addVariable("TargetRadius",5.0);
  Control.addVariable("TargetDefMat","Stainless304");

  Control.addVariable("TubeARadius",5.0);
  Control.addVariable("TubeALength",25.0);
  Control.addVariable("TubeBRadius",5.0);
  Control.addVariable("TubeBLength",5.0);
  Control.addVariable("TubeADefMat","Stainless304");
  Control.addVariable("TubeBDefMat","Lead");



  return;
}

void
localShieldVariables(FuncDataBase& Control)
  /*!
    Createa groupd of local shielding variables
  */
{
  ELog::RegMethod RegA("singleItemVariables[F]","localShieldVariables");

  setVariable::LocalShieldingGenerator LSGen;

  LSGen.setSize(10.0,60,30.0);
  LSGen.generate(Control,"ShieldA");
  Control.addVariable("ShieldAXStep",62.5);
  Control.addVariable("ShieldAZStep",-10.0);
  Control.addVariable("ShieldAYStep",1.1);

  // Walls near pipeC
  // http://localhost:8080/maxiv/work-log/tdc/pictures/spf-hall/spf/img_5457.jpg/view
  // http://localhost:8080/maxiv/work-log/tdc/pictures/spf-hall/spf/img_5384.jpg/view
  LSGen.setSize(45.0,5,20.0);
  LSGen.generate(Control,"ShieldB");
  Control.addVariable("ShieldBXStep",-25.0);
  Control.addVariable("ShieldBYStep",-27.0);
  Control.addVariable("ShieldBZStep",5.0);
  // floor
  LSGen.setSize(5.0,20,40.0);
  LSGen.generate(Control,"ShieldC");
  Control.addVariable("ShieldCZStep",-2.5);
  Control.addVariable("ShieldCXStep",-7.5);
  Control.addVariable("ShieldCYStep",2.5);
  // vertical wall
  LSGen.setSize(5.0,15,20.0);
  LSGen.generate(Control,"ShieldD");
  Control.addVariable("ShieldDXStep",10.0);
  Control.addVariable("ShieldDYStep",-2.5);
  // roof
  LSGen.setSize(5.0,10,20);
  LSGen.generate(Control,"ShieldE");
  Control.addVariable("ShieldEYStep",2.5); // Z
  Control.addVariable("ShieldEXStep",4); // X
  Control.addVariable("ShieldEZStep",-11); // Y
  Control.addVariable("ShieldEYAngle",-10);


  Control.addVariable("CellLength",100.0);
  Control.addVariable("CellWidth",100.0);
  Control.addVariable("CellHeight",190.0);

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
  ELog::RegMethod RegA("singleItemVariables[F]","exptHutVariables");

  const double beamOffset(-0.6);

  const std::string hutName(beamName+"ExptHutch");

  Control.addVariable(hutName+"Height",200.0);
  Control.addVariable(hutName+"Length",858.4);
  Control.addVariable(hutName+"OutWidth",198.50);
  Control.addVariable(hutName+"RingWidth",248.6);
  Control.addVariable(hutName+"InnerThick",1.1);
  Control.addVariable(hutName+"PbBackThick",1.0);
  Control.addVariable(hutName+"PbRoofThick",0.6);
  Control.addVariable(hutName+"PbWallThick",0.4);
  Control.addVariable(hutName+"OuterThick",1.1);
  Control.addVariable(hutName+"CornerLength",720.0);
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


}  // NAMESPACE setVariable
