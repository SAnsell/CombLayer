/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   linac/magVariables.cxx
 *
 * Copyright (c) 2004-2021 by Stuart Ansell/Konstantin Batkov
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

#include "CFFlanges.h"
#include "PipeGenerator.h"
#include "SplitPipeGenerator.h"
#include "BellowGenerator.h"
#include "CorrectorMagGenerator.h"
#include "LinacQuadGenerator.h"
#include "PipeTubeGenerator.h"
#include "PortItemGenerator.h"
#include "StriplineBPMGenerator.h"
#include "DipoleDIBMagGenerator.h"
#include "YagScreenGenerator.h"
#include "YagUnitGenerator.h"
#include "FlatPipeGenerator.h"
#include "BeamDividerGenerator.h"
#include "IonPTubeGenerator.h"
#include "LBeamStopGenerator.h"
#include "NBeamStopGenerator.h"
#include "BeamBoxGenerator.h"

#include "magnetVar.h"

namespace setVariable
{

namespace linacVar
{

void
Segment1Magnet(FuncDataBase& Control,
	       const std::string& lKey)
  /*!
    This should be for the magnet unit but currently doing segment1
    to make fast compiles
    \param Control :: Variable Database
    \param lKey :: key name
  */
{
  setVariable::PipeGenerator PGen;
  setVariable::LinacQuadGenerator LQGen;
  setVariable::CorrectorMagGenerator CMGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::StriplineBPMGenerator BPMGen;
  setVariable::IonPTubeGenerator IonPGen;

  // exactly 1m from wall.
  const Geometry::Vec3D startPt(0,0,0);
  const Geometry::Vec3D endPt(0,395.2,0);
  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);

  Control.addVariable(lKey+"BeamOrg",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"BeamDelta",Geometry::Vec3D(0,0,0));

  PGen.setCF<setVariable::CF18_TDC>();
  PGen.setMat("Stainless316L");
  PGen.setNoWindow();
  PGen.setOuterVoid(1);

  PGen.generatePipe(Control,lKey+"PipeA",16.5); // No_1_00.pdf

  setBellow26(Control,lKey+"BellowA",7.5);

  //  corrector mag and pie
  PGen.setOuterVoid(0);
  PGen.generatePipe(Control,lKey+"PipeB",57.23); // No_1_00.pdf
  CMGen.generateMag(Control,lKey+"CMagHA",31.85,0); // No_1_00.pdf
  CMGen.generateMag(Control,lKey+"CMagVA",46.85,1); // No_1_00.pdf

  PGen.setCF<setVariable::CF16_TDC>();
  PGen.setMat("Stainless304L");
  PGen.setOuterVoid(1);
  PGen.generatePipe(Control,lKey+"PipeC",34.27); // No_1_00.pdf

  PGen.setCF<setVariable::CF18_TDC>();
  PGen.setMat("Stainless316L");
  PGen.setOuterVoid(0);
  PGen.generatePipe(Control,lKey+"PipeD",113.7);

  CMGen.generateMag(Control,lKey+"CMagHB",51.86,0);
  CMGen.generateMag(Control,lKey+"CMagVB",69.36,1);
  LQGen.generateQuad(Control,lKey+"QuadA",96.86);

  BPMGen.generateBPM(Control,lKey+"BPM",0.0);

  PGen.setCF<setVariable::CF18_TDC>();
  PGen.generatePipe(Control,lKey+"PipeE",128.0);

  CMGen.generateMag(Control,lKey+"CMagHC",101.20,0);
  CMGen.generateMag(Control,lKey+"CMagVC",117.0,1);


  IonPGen.generateTube(Control,lKey+"PumpA");
  return;
}

void
Segment5Magnet(FuncDataBase& Control,
	       const std::string& lKey)
  /*!
    This should be for the magnet unit but currently doing segment5
    to make fast compiles
    \param Control :: Variable Database
    \param lKey :: key name
  */
{
  ELog::RegMethod RegA("linacVariables[F]","Segment5Magnet");

  setVariable::CF40 CF40unit;
  setVariable::BeamDividerGenerator BDGen(CF40unit);
  setVariable::FlatPipeGenerator FPGen;
  setVariable::DipoleDIBMagGenerator DIBGen;


  const double flatAXYAngle = atan(117.28/817.51)*180/M_PI; // No_5_00.pdf
  const double angleDipole(1.6-0.12);

  Control.addVariable(lKey+"BeamOrg",Geometry::Vec3D(229.927,1901.34,0));
  Control.addVariable(lKey+"BeamDelta",Geometry::Vec3D(0,0,0));
  Control.addVariable(lKey+"BeamXYAngle",-angleDipole);


  setFlat(Control,lKey+"FlatA",81.751/cos(flatAXYAngle*M_PI/180.0),
	  angleDipole,0.0);

  // this centers DipoleA at 40.895 [No_5_00.pdf]
  DIBGen.generate(Control,lKey+"DipoleA");
  Control.addVariable(lKey+"DipoleAYStep",-0.0091);

  BDGen.generateDivider(Control,lKey+"BeamA",angleDipole+0.3);
  Control.addVariable(lKey+"BeamAExitLength", 15);
  Control.addVariable(lKey+"BeamAMainLength", 34.4);
  Control.addVariable(lKey+"BeamAXStep", 0.0);

  setFlat(Control,lKey+"FlatB",
	  81.009/cos((flatAXYAngle+angleDipole*2)*M_PI/180.0),
	  angleDipole);// No_5_00.pdf

  DIBGen.generate(Control,lKey+"DipoleB");
  Control.addVariable(lKey+"DipoleBYStep",0.037); // this centers DipoleB at 21.538 [No_5_00.pdf]

  setBellow26(Control,lKey+"BellowA",7.5);
  Control.addVariable(lKey+"BellowAXYAngle",angleDipole);

  return;
}

void
Segment12Magnet(FuncDataBase& Control,
		const std::string& lKey)
  /*!
    This should be for the magnet unit but currently doing segment12
    to make fast compiles
    \param Control :: Variable Database
    \param lKey :: key name
  */
{
  ELog::RegMethod RegA("linacVariables[F]","Segment12Magnet");

  setVariable::CF63 CF63unit;
  setVariable::PipeGenerator PGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::FlatPipeGenerator FPGen;
  setVariable::DipoleDIBMagGenerator DIBGen;
  setVariable::BeamDividerGenerator BDGen(CF63unit);
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::IonPTubeGenerator IonTGen;

  const double XYAngle(12.8);

  PGen.setCF<setVariable::CF18_TDC>();
  PGen.setMat("Stainless316L");
  PGen.setNoWindow();

  setBellow26(Control,lKey+"BellowA",7.5);

  // No_12_00.pdf
  const double flatAXYAngle = -1.6;
  setFlat(Control,lKey+"FlatA",
	  (935.83-73.14)/10.0/cos((XYAngle+flatAXYAngle)*M_PI/180.0),0.0);
  //	  flatAXYAngle/2);
  Control.addVariable(lKey+"FlatAFrontWidth",3.304-1.344); // outer width
  Control.addVariable(lKey+"FlatABackWidth",1.0+5.445-1.344); // outer width
  Control.addVariable(lKey+"FlatAWallMat","Stainless316L");

  DIBGen.generate(Control,lKey+"DipoleA");
  // this fixes the Y horizontal coordinate but X is still wrong
  // => Wrong FlatA angle?
  Control.addVariable(lKey+"DipoleAYStep",-2.8321);

  //  BDGen.setMainSize(60.0,3.2);
  BDGen.setAFlangeCF<setVariable::CF50>();
  BDGen.setBFlangeCF<setVariable::CF18_TDC>();
  BDGen.setEFlangeCF<setVariable::CF18_TDC>();
  // Angle (1.6) / short on left /  -1: left side aligned
  // angle of 1.6 gets us to direct (12.8 against y for exitpipe)
  //  1.6-flatAXYAngle/2,
  //  BDGen.generateDivider(Control,lKey+"BeamA",-flatAXYAngle/2.0,1,0);
  BDGen.generateDivider(Control,lKey+"BeamA",0,1,-1);
  Control.addVariable(lKey+"BeamAWallThick",0.2); // No_12_00.pdf
  // + 0.313 in order to have correct coordinates of the [BE] flanges
  // according to No_12_00.pdf
  Control.addVariable(lKey+"BeamABoxLength",56.1+0.313);
  Control.addVariable(lKey+"BeamAXStep",0.0);

  setBellow26(Control,lKey+"BellowLA",7.5);

  // small ion pump port:
  // -----------------------
  // horizontal off


  IonTGen.setCF<setVariable::CF63>();
  IonTGen.setWallThick(0.2);      // No_17_00.pdf
  IonTGen.setVertical(8.45,4.050);  // d / h [2.2cm]
  IonTGen.setPortCF<setVariable::CF35_TDC>(); // Port
  IonTGen.generateTube(Control,lKey+"IonPumpLA");
  // remember to re-rotate next item:
  Control.addVariable(lKey+"IonPumpLAYAngle",-90.0);

  PGen.generatePipe(Control,lKey+"PipeLA",93.3-2.87);

  setBellow26(Control,lKey+"BellowLB",7.5);

  // RIGHT SIDE

  setFlat(Control,lKey+"FlatB",85.4-2.87,1.6);

  DIBGen.generate(Control,lKey+"DipoleB");
  Control.addVariable(lKey+"DipoleBXStep",6.0);
  Control.addVariable(lKey+"DipoleBYStep",-4.0);

  setBellow26(Control,lKey+"BellowRB",7.5);

  Control.addVariable(lKey+"BellowRBXYAngle",-1.6);
  return;
}

void
Segment29Magnet(FuncDataBase& Control,
		const std::string& lKey)
  /*!
    This should be for the magnet unit but currently doing segment29
    to make fast compiles
    \param Control :: Variable Database
    \param lKey :: key name
  */
{
  ELog::RegMethod RegA("linacVariables[F]","Segment29Magnet");

  setVariable::YagScreenGenerator YagScreenGen;
  setVariable::YagUnitGenerator YagUnitGen;
  setVariable::NBeamStopGenerator BSGen;
  setVariable::BeamBoxGenerator BXGen;

  const Geometry::Vec3D startPtA(-637.608,9073.611,0.0);
  const Geometry::Vec3D startPtB(-637.608,9073.535,-84.888);

  const Geometry::Vec3D endPtA(-637.608,9401.161,0.0);
  const Geometry::Vec3D endPtB(-637.608,9401.151,-102.058);

  // to allow main object to have an offset
  Control.addVariable(lKey+"Offset",startPtA+linacVar::zeroOffset);
  Control.addVariable(lKey+"OffsetA",startPtA+linacVar::zeroOffset);
  Control.addVariable(lKey+"OffsetB",startPtB+linacVar::zeroOffset);

  Control.addVariable(lKey+"EndOffsetA",endPtA+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffsetB",endPtB+linacVar::zeroOffset);

  Control.addVariable(lKey+"FrontLinkA","frontFlat");
  Control.addVariable(lKey+"BackLinkA","backFlat");
  Control.addVariable(lKey+"FrontLinkB","frontMid");
  Control.addVariable(lKey+"BackLinkB","backMid");

  setVariable::PipeGenerator PGen;
  PGen.setCF<CF35_TDC>();
  PGen.setNoWindow();
  PGen.setMat("Stainless304L");
  PGen.setOuterVoid(1);

  PGen.generatePipe(Control,lKey+"PipeAA",291.6);
  PGen.generatePipe(Control,lKey+"PipeBA",292.0);

  Control.addVariable(lKey+"PipeBAOffset",startPtB-startPtA);

  Control.addVariable(lKey+"PipeAAXAngle",
		      std::atan((endPtA-startPtA).unit()[2])*180.0/M_PI);
  Control.addVariable(lKey+"PipeBAXAngle",
		      std::atan((endPtB-startPtB).unit()[2])*180.0/M_PI);

  setBellow37(Control,lKey+"BellowAA");
  setBellow37(Control,lKey+"BellowBA", 16.066); // No_29_00

  YagUnitGen.generateYagUnit(Control,lKey+"YagUnitA",true);
  Control.addVariable(lKey+"YagUnitAYAngle",90.0);
  Control.addVariable(lKey+"YagUnitAFrontLength", 12.95);

  YagUnitGen.generateYagUnit(Control,lKey+"YagUnitB",true);
  Control.addVariable(lKey+"YagUnitBYAngle",90.0);

  YagScreenGen.generateScreen(Control,lKey+"YagScreenA",0);   // closed
  Control.addVariable(lKey+"YagScreenAYAngle",-90.0);
  YagScreenGen.generateScreen(Control,lKey+"YagScreenB",0);   // closed
  Control.addVariable(lKey+"YagScreenBYAngle",-90.0);

  BSGen.setFullLen(125.0);
  BSGen.generateBStop(Control,lKey+"BeamStopA",3.0);
  BSGen.setFullLen(122.0);
  BSGen.generateBStop(Control,lKey+"BeamStopB",3.0);
  Control.addVariable(lKey+"BeamStopAYAngle",-90.0);
  Control.addVariable(lKey+"BeamStopBYAngle",-90.0);
  
  Control.addVariable(lKey+"BeamWingAYStep",100.0);
  Control.addVariable(lKey+"BeamWingAXStep",25.0);
  Control.addVariable(lKey+"BeamWingAPreZAngle",70.0);
  Control.addVariable(lKey+"BeamWingAWidth",120.0);
  Control.addVariable(lKey+"BeamWingADepth",10.0);
  Control.addVariable(lKey+"BeamWingAHeight",200.0);
  Control.addVariable(lKey+"BeamWingAMainMat","Poly");
  
  Control.addVariable(lKey+"BeamWingBYStep",100.0);
  Control.addVariable(lKey+"BeamWingBXStep",-25.0);
  Control.addVariable(lKey+"BeamWingBPreZAngle",-70.0);
  Control.addVariable(lKey+"BeamWingBWidth",120.0);
  Control.addVariable(lKey+"BeamWingBDepth",10.0);
  Control.addVariable(lKey+"BeamWingBHeight",200.0);
  Control.addVariable(lKey+"BeamWingBMainMat","Poly");

    
  BXGen.generateBox(Control,lKey+"BeamBoxA",3.0);
  Control.addVariable(lKey+"BeamBoxAPreYAngle",-90.0);

  BXGen.generateBox(Control,lKey+"BeamBoxB",3.0);
  Control.addVariable(lKey+"BeamBoxBPreYAngle",-90.0);
  Control.addVariable(lKey+"BeamBoxBZStep",-100.0);

  // end void
  Control.addVariable(lKey+"EndVoidLength",3.0);

  return;
}


void
Segment32Magnet(FuncDataBase& Control,
		   const std::string& lKey)
  /*!
    This should be for the magnet unit but currently doing segment32
    to make fast compiles
    \param Control :: Variable Database
    \param lKey :: key name
  */
{
  ELog::RegMethod RegA("linacVariables[F]","Segment32Magnet");

  setVariable::PipeGenerator PGen;
  setVariable::FlatPipeGenerator FPGen;
  setVariable::DipoleDIBMagGenerator DIBGen;
  setVariable::BellowGenerator BellowGen;

  setFlat(Control,lKey+"FlatA",82.581,0.0);

  DIBGen.generate(Control,lKey+"DipoleA");
  Control.addVariable(lKey+"DipoleAXStep",-0.5198);
  Control.addVariable(lKey+"DipoleAYStep",0.0009);

  PGen.setMat("Stainless316L","Stainless304L");
  PGen.setNoWindow();
  PGen.setCF<CF18_TDC>();
  PGen.setOuterVoid(1);

  PGen.generatePipe(Control,lKey+"PipeA",94.401); // No_32_34_00
  Control.addVariable(lKey+"PipeAXYAngle",-1.6); // No_32_34_00

  setFlat(Control,lKey+"FlatB",82.582,-1.6); // measured

  DIBGen.generate(Control,lKey+"DipoleB");
  Control.addVariable(lKey+"DipoleBXStep",-0.23505);
  Control.addVariable(lKey+"DipoleAYStep",0.02628);

  setBellow26(Control,lKey+"Bellow",7.501);
  Control.addVariable(lKey+"BellowXYAngle",-1.6);

  return;
}

}  // NAMESPACE linacVAR

}   // NAMESPACE setVariable
