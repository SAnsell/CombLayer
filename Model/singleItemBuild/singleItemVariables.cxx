/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   singleItemBuild/singleItemVariables.cxx
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#include "OutputLog.h"
#include "Vec3D.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"

#include "CFFlanges.h"
#include "CryoGenerator.h"
#include "BladeGenerator.h"
#include "PipeGenerator.h"
#include "RectPipeGenerator.h"
#include "TwinBaseGenerator.h"
#include "TwinGenerator.h"
#include "TwinFlatGenerator.h"
#include "ChopperGenerator.h"
#include "DipoleGenerator.h"
#include "QuadrupoleGenerator.h"
#include "LinacQuadGenerator.h"
#include "LinacSexuGenerator.h"
#include "SexupoleGenerator.h"
#include "OctupoleGenerator.h"
#include "EPSeparatorGenerator.h"
#include "EPCombineGenerator.h"
#include "PreDipoleGenerator.h"
#include "DipoleChamberGenerator.h"
#include "R3ChokeChamberGenerator.h"
#include "MagnetM1Generator.h"
#include "MagnetBlockGenerator.h"
#include "CorrectorMagGenerator.h"
#include "QuadUnitGenerator.h"
#include "CylGateValveGenerator.h"
#include "DipoleDIBMagGenerator.h"
#include "EArrivalMonGenerator.h"
#include "BPMGenerator.h"
#include "CeramicSepGenerator.h"
#include "BeamDividerGenerator.h"
#include "EBeamStopGenerator.h"
#include "ScrapperGenerator.h"
#include "TriPipeGenerator.h"
#include "YagScreenGenerator.h"
#include "YagUnitGenerator.h"

namespace setVariable
{


void
SingleItemVariables(FuncDataBase& Control)
  /*!
    Function to set the control variables and constants
    -- This version is for ESS ()
    \param Control :: Function data base to add constants too
  */
{
// -----------
// GLOBAL stuff
// -----------
  Control.addVariable("zero",0.0);     // Zero
  Control.addVariable("one",1.0);      // one

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

  setVariable::EArrivalMonGenerator EMonGen;
  EMonGen.generateMon(Control,"BeamMon",0.0);

  setVariable::CryoGenerator CryGen;
  CryGen.generateFridge(Control,"singleCryo",3.0,-10,4.5);

  setVariable::TwinGenerator TGen;
  TGen.generateChopper(Control,"singleTwinB",0.0,16.0,10.0);

  setVariable::TwinFlatGenerator TCGen;
  TCGen.generateChopper(Control,"singleTwinC",0.0,16.0,10.0);

  TCGen.generateChopper(Control,"singleTwinD",80.0,16.0,10.0);

  setVariable::RectPipeGenerator PipeGen;
  PipeGen.generatePipe(Control,"singleBoxPipeA",0.0,80.0);
  PipeGen.generatePipe(Control,"singleBoxPipeB",0.0,80.0);

  setVariable::ChopperGenerator CGen;
  CGen.setMotorRadius(10.0);
  CGen.generateChopper(Control,"singleChopper",10.0,12.0,5.55);
  Control.addVariable("singleChopperMotorBodyLength",15.0);


  setVariable::BladeGenerator BGen;
  // Single Blade chopper
  BGen.setThick({0.2});
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.generateBlades(Control,"singleBBladeTop",-2.0,22.5,35.0);

  // Single Blade chopper
  BGen.setThick({0.2});
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.generateBlades(Control,"singleBBladeLow",2.0,22.5,35.0);


  setVariable::EPSeparatorGenerator EPSGen;
  EPSGen.generatePipe(Control,"EPSeparator");

  setVariable::DipoleChamberGenerator DCGen;
  DCGen.generatePipe(Control,"DipoleChamber",0.0);

  setVariable::R3ChokeChamberGenerator CCGen;
  CCGen.generateChamber(Control,"R3Chamber");

  setVariable::MagnetM1Generator M1Gen;
  M1Gen.generateBlock(Control,"M1Block");

  setVariable::PreDipoleGenerator PBGen;
  PBGen.generatePipe(Control,"PreDipole");

  setVariable::EPCombineGenerator EPCGen;
  EPCGen.generatePipe(Control,"EPCombine");

  setVariable::QuadrupoleGenerator QGen;
  QGen.generateQuad(Control,"QFend",20.0,25.0);

  setVariable::OctupoleGenerator OGen;
  OGen.generateOcto(Control,"OXX",20.0,25.0);

  setVariable::DipoleGenerator DGen;
  DGen.generateDipole(Control,"M1BlockDIM",0.0,60.0);

  setVariable::SexupoleGenerator SGen;
  SGen.generateHex(Control,"SXX",20.0,25.0);

  const double startWidth(2.33/2.0);
  const double endWidth(6.70/2.0);
  setVariable::TriPipeGenerator TPGen;
  TPGen.setBFlangeCF<CF100>();
  TPGen.setXYWindow(startWidth,startWidth,endWidth,endWidth);
  TPGen.generateTri(Control,"TriPipe");

  setVariable::LinacQuadGenerator LQGen;
  LQGen.generateQuad(Control,"LQ",20.0);

  setVariable::LinacSexuGenerator LSGen;
  LSGen.generateSexu(Control,"LS",20.0);

  // Block for new R1-M1
  setVariable::MagnetBlockGenerator MBGen;
  MBGen.generateBlock(Control,"M1",0.0);

  setVariable::QuadUnitGenerator M1QGen;
  M1QGen.generatePipe(Control,"M1QuadUnit",0.0);

  //  setVariable::DipoleChamberGenerator DCGen;
  DCGen.generatePipe(Control,"M1DipoleChamber",0.0);

  //  corrector mag
  setVariable::CorrectorMagGenerator CMGen;
  CMGen.generateMag(Control,"CM",0.0,0);
  setVariable::PipeGenerator PGen;
  PGen.setCF<setVariable::CF40_22>();
  PGen.generatePipe(Control,"VC",-40.0,80.0);


  // CylGateValve
  setVariable::CylGateValveGenerator GVGen;
  GVGen.generateGate(Control,"GV",1);

  //  dipole magnet DIB
  setVariable::DipoleDIBMagGenerator DIBGen;
  DIBGen.generate(Control,"DIB");

  PGen.setCF<setVariable::CF25>();
  PGen.generatePipe(Control,"VC",-40.0,80.0);

  setVariable::YagScreenGenerator YagGen;
  YagGen.generateScreen(Control,"YAG",1);  // in beam

  setVariable::BPMGenerator BPMGen;
  BPMGen.generateBPM(Control,"BPM",0.0);

  setVariable::CeramicSepGenerator CSGen;
  CSGen.generateCeramicSep(Control,"CerSep");

  setVariable::CF40 CF40unit;
  setVariable::BeamDividerGenerator BDGen(CF40unit);
  BDGen.generateDivider(Control,"BeamDiv",0.0);  

  setVariable::EBeamStopGenerator EBGen;
  EBGen.generateEBeamStop(Control,"EBeam",0);  

  setVariable::ScrapperGenerator SCGen;
  SCGen.generateScrapper(Control,"Scrapper",1.0);   // z lift
  
  setVariable::YagUnitGenerator YagUnitGen;
  YagUnitGen.generateYagUnit(Control,"YU");

  return;
}

}  // NAMESPACE setVariable
