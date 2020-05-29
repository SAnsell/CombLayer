/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   linac/linacVariables.cxx
 *
 * Copyright (c) 2004-2020 by Stuart Ansell/Konstantin Batkov
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
#include "GateValveGenerator.h"
#include "CorrectorMagGenerator.h"
#include "LinacQuadFGenerator.h"
#include "LinacSexuGenerator.h"
#include "PipeTubeGenerator.h"
#include "PortItemGenerator.h"
#include "BPMGenerator.h"
#include "CylGateValveGenerator.h"
#include "DipoleDIBMagGenerator.h"
#include "EArrivalMonGenerator.h"
#include "YagScreenGenerator.h"
#include "YagUnitGenerator.h"
#include "FlatPipeGenerator.h"
#include "BeamDividerGenerator.h"

namespace setVariable
{

namespace linacVar
{
  void wallVariables(FuncDataBase&,const std::string&);

  void linac2SPFsegment1(FuncDataBase&,const std::string&);
  void linac2SPFsegment2(FuncDataBase&,const std::string&);
  void linac2SPFsegment3(FuncDataBase&,const std::string&);
  void linac2SPFsegment4(FuncDataBase&,const std::string&);
  void linac2SPFsegment5(FuncDataBase&,const std::string&);

  void TDCsegment14(FuncDataBase&,const std::string&);
  void TDCsegment15(FuncDataBase&,const std::string&);
  void TDCsegment16(FuncDataBase&,const std::string&);
  //  void TDCsegment17(FuncDataBase&,const std::string&);
  void TDCsegment18(FuncDataBase&,const std::string&);

  const double zeroX(152.0);   // coordiated offset to master
  const double zeroY(481.0);    // drawing README.pdf

void
setIonPump2(FuncDataBase& Control,
	    const std::string& name)
/*!
  Set the 2 port ion pump variables
  \param Control :: DataBase to use
  \param name :: name prefix
  \param nPorts :: number of ports
 */
{
  ELog::RegMethod RegA("linacVariables[F]","setIonPump2");

  setVariable::PortItemGenerator PItemGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;

  const Geometry::Vec3D OPos(0.0,0.0,0.0);
  const Geometry::Vec3D XVec(1,0,0);

  SimpleTubeGen.setMat("Stainless304");
  SimpleTubeGen.setCF<setVariable::CF63>();
  PItemGen.setCF<setVariable::CF40_22>(6.6); // Port0 length
  PItemGen.setNoPlate();

  SimpleTubeGen.generateBlank(Control,name,0.0,25.8);
  Control.addVariable(name+"NPorts",2);
  Control.addVariable(name+"FlangeCapThick",setVariable::CF63::flangeLength);

  PItemGen.generatePort(Control,name+"Port0",OPos,-XVec);

  // total ion pump length
  const double totalLength(16.0); // measured
  // length of ports 1 and 2
  const double L = totalLength/2.0 -
    (setVariable::CF63::innerRadius+setVariable::CF63::wallThick);

  PItemGen.setLength(L);
  PItemGen.setNoPlate();
  PItemGen.generatePort(Control,name+"Port1",OPos,XVec);

  return;
}

void
setIonPump3(FuncDataBase& Control,
	    const std::string& name)
/*!
  Set the 3 port ion pump variables
  \param Control :: DataBase to use
  \param name :: name prefix
 */
{
  ELog::RegMethod RegA("linacVariables[F]","setIonPump");

  setVariable::PortItemGenerator PItemGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;

  const Geometry::Vec3D OPos(0.0,0.0,0.0);
  const Geometry::Vec3D ZVec(0,0,-1);
  const Geometry::Vec3D XVec(1,0,0);

  SimpleTubeGen.setMat("Stainless304");
  SimpleTubeGen.setCF<setVariable::CF63>();
  PItemGen.setCF<setVariable::CF40_22>(6.6); // Port0 length
  PItemGen.setNoPlate();

  SimpleTubeGen.generateBlank(Control,name,0.0,25.8);
  Control.addVariable(name+"NPorts",3);
  Control.addVariable(name+"FlangeACapThick",setVariable::CF63::flangeLength);
  Control.addVariable(name+"FlangeBCapThick",setVariable::CF63::flangeLength);

  PItemGen.setPlate(setVariable::CF40_22::flangeLength, "Stainless304");
  PItemGen.generatePort(Control,name+"Port0",OPos,-ZVec);

  // total ion pump length
  const double totalLength(16.0); // measured
  // length of ports 1 and 2
  double L = totalLength -
    (setVariable::CF63::innerRadius+setVariable::CF63::wallThick)*2.0;
  L /= 2.0;

  PItemGen.setLength(L);
  PItemGen.setNoPlate();
  PItemGen.generatePort(Control,name+"Port1",OPos,-XVec);

  PItemGen.setLength(L);
  PItemGen.generatePort(Control,name+"Port2",OPos,XVec);

  return;
}

void
linac2SPFsegment1(FuncDataBase& Control,
		  const std::string& lKey)
  /*!
    Set the variables for the main walls
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{
  ELog::RegMethod RegA("linacVariables[F]","linac2SPFsegment1");
  setVariable::PipeGenerator PGen;
  setVariable::BellowGenerator BellowGen;
  setVariable::LinacQuadFGenerator LQGen;
  setVariable::CorrectorMagGenerator CMGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::PortItemGenerator PItemGen;

  Control.addVariable(lKey+"XStep",linacVar::zeroX);   // exactly 1m from wall.
  Control.addVariable(lKey+"YStep",linacVar::zeroY);   // exactly 1m from wall.

  PGen.setCF<setVariable::CF40_22>();
  PGen.setNoWindow();
  PGen.generatePipe(Control,lKey+"PipeA",0.0,16.15);

  // note larger unit
  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,lKey+"BellowA",0.0,7.5);

  //  corrector mag and pie
  PGen.generatePipe(Control,lKey+"PipeB",0.0,55.73);
  CMGen.generateMag(Control,lKey+"CMagHorrA",30.80,0);
  CMGen.generateMag(Control,lKey+"CMagVertA",46.3,1);

  PGen.generatePipe(Control,lKey+"PipeC",0.0,33.85);
  PGen.generatePipe(Control,lKey+"PipeD",0.0,112.7);

  CMGen.generateMag(Control,lKey+"CMagHorrB",51.50,0);
  CMGen.generateMag(Control,lKey+"CMagVertB",68.50,1);

  LQGen.generateQuad(Control,lKey+"QuadA",94.0);


  PGen.generatePipe(Control,lKey+"PipeE",0.0,21.30);
  PGen.generatePipe(Control,lKey+"PipeF",0.0,128.0);

  CMGen.generateMag(Control,lKey+"CMagHorrC",101.20,0);
  CMGen.generateMag(Control,lKey+"CMagVertC",117.0,1);

  const Geometry::Vec3D OPos(0,2.0,0);
  const Geometry::Vec3D ZVec(0,0,-1);

  SimpleTubeGen.setMat("Stainless304");
  SimpleTubeGen.setCF<CF63>();
  PItemGen.setCF<setVariable::CF40>(6.5);
  PItemGen.setNoPlate();

  SimpleTubeGen.generateBlank(Control,lKey+"PumpA",0.0,12.4);
  Control.addVariable(lKey+"PumpANPorts",2);

  PItemGen.setLength(6.5);
  PItemGen.generatePort(Control,lKey+"PumpAPort0",OPos,-ZVec);
  PItemGen.setLength(2.5);
  PItemGen.generatePort(Control,lKey+"PumpAPort1",OPos,ZVec);


  return;
}

void
linac2SPFsegment2(FuncDataBase& Control,
		   const std::string& lKey)
  /*!
    Set the variables for the main walls
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{
  ELog::RegMethod RegA("linacVariables[F]","linac2SPFsegment2");

  setVariable::PipeGenerator PGen;
  setVariable::BellowGenerator BellowGen;
  setVariable::LinacQuadFGenerator LQGen;
  setVariable::BPMGenerator BPMGen;
  setVariable::CylGateValveGenerator CGateGen;
  setVariable::EArrivalMonGenerator EArrGen;
  setVariable::YagScreenGenerator YagGen;
  setVariable::YagUnitGenerator YagUnitGen;

  Control.addVariable(lKey+"XStep",linacVar::zeroX);   // exactly 1m from wall.
  Control.addVariable(lKey+"YStep",395.2+linacVar::zeroY);   // if segment 1 not built

  PGen.setCF<setVariable::CF40_22>();
  PGen.setNoWindow();

  // lengthened to fit quad +2cm
  PGen.generatePipe(Control,lKey+"PipeA",0.0,35.0);

  LQGen.generateQuad(Control,lKey+"QuadA",35.0/2.0);

  BPMGen.setCF<setVariable::CF40>();
  BPMGen.generateBPM(Control,lKey+"BPMA",0.0);

  // note larger unit
  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,lKey+"BellowA",0.0,7.58);

  PGen.generatePipe(Control,lKey+"PipeB",0.0,114.0);

  LQGen.generateQuad(Control,lKey+"QuadB",72.0);

  CGateGen.generateGate(Control,lKey+"GateTube",0);

  CGateGen.setRotate(1);
  CGateGen.generateGate(Control,lKey+"GateTube",0);

  PGen.generatePipe(Control,lKey+"PipeC",0.0,31.0);

  EArrGen.generateMon(Control,lKey+"BeamArrivalMon",0.0);

  PGen.generatePipe(Control,lKey+"PipeD",0.0,75.0);

  // again not larger size
  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,lKey+"BellowB",0.0,7.58);

  BPMGen.generateBPM(Control,lKey+"BPMB",0.0);

  PGen.generatePipe(Control,lKey+"PipeE",0.0,132.4);

  LQGen.generateQuad(Control,lKey+"QuadC",23.54);
  LQGen.generateQuad(Control,lKey+"QuadD",73.0);
  LQGen.generateQuad(Control,lKey+"QuadE",113.2);

  YagGen.setCF<CF40_22>();
  YagGen.generateScreen(Control,lKey+"YAG",1);   // closed

  YagUnitGen.generateYagUnit(Control,lKey+"YagUnit");



  return;
}

void
linac2SPFsegment3(FuncDataBase& Control,
		  const std::string& lKey)
  /*!
    Set the variables for segment3
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{
  ELog::RegMethod RegA("linacVariables[F]","linac2SPFsegment3");

  setVariable::PipeGenerator PGen;
  setVariable::BellowGenerator BellowGen;
  setVariable::FlatPipeGenerator FPGen;
  setVariable::DipoleDIBMagGenerator DIBGen;
  setVariable::CorrectorMagGenerator CMGen;

  Control.addVariable(lKey+"XStep",linacVar::zeroX);   // exactly 1m from wall.
  Control.addVariable(lKey+"YStep",881.06+linacVar::zeroY);

  PGen.setCF<setVariable::CF40_22>();
  PGen.setMat("Stainless316L");
  PGen.setNoWindow();

  // again not larger size
  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,lKey+"BellowA",0.0,7.58);

  FPGen.generateFlat(Control,lKey+"FlatA",83.8);
  Control.addVariable(lKey+"FlatAXYAngle",1.6);
  DIBGen.generate(Control,lKey+"DipoleA");

  PGen.generatePipe(Control,lKey+"PipeA",0.0,93.40); // measured
  Control.addVariable(lKey+"PipeAXYAngle",1.6);
  // Control.addVariable(lKey+"PipeAXStep",-1.2);
  // Control.addVariable(lKey+"PipeAFlangeFrontXStep",1.2);

  CMGen.generateMag(Control,lKey+"CMagHorA",64.0,0);
  CMGen.generateMag(Control,lKey+"CMagVertA",80.0,1);

  FPGen.generateFlat(Control,lKey+"FlatB",83.8);
  Control.addVariable(lKey+"FlatBXYAngle",1.6);
  DIBGen.generate(Control,lKey+"DipoleB");

  // again not larger size
  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,lKey+"BellowB",0.0,7.58);
  Control.addVariable(lKey+"BellowBXYAngle",1.6);
  return;
}

void
linac2SPFsegment4(FuncDataBase& Control,
		  const std::string& lKey)
  /*!
    Set the variables for segment 4
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{
  ELog::RegMethod RegA("linacVariables[F]","linac2SPFsegment4");

  setVariable::PipeGenerator PGen;
  setVariable::BellowGenerator BellowGen;
  setVariable::BPMGenerator BPMGen;
  setVariable::LinacQuadFGenerator LQGen;
  setVariable::LinacSexuGenerator LSGen;
  setVariable::CorrectorMagGenerator CMGen;
  setVariable::YagScreenGenerator YagGen;
  setVariable::YagUnitGenerator YagUnitGen;

  Control.addVariable(lKey+"XStep",-15.322+linacVar::zeroX);
  Control.addVariable(lKey+"YStep",1155.107+linacVar::zeroY);
  Control.addVariable(lKey+"XYAngle",6.4);

  PGen.setCF<setVariable::CF40_22>();
  PGen.setMat("Stainless316L");
  PGen.setNoWindow();
  PGen.generatePipe(Control,lKey+"PipeA",0.0,65.76); // measured

  BPMGen.setCF<setVariable::CF40_22>();
  BPMGen.generateBPM(Control,lKey+"BPMA",0.0);

  PGen.generatePipe(Control,lKey+"PipeB",0.0,79.2); // measured

  LQGen.generateQuad(Control,lKey+"QuadA",17.2);
  LSGen.generateSexu(Control,lKey+"SexuA",39.1);
  LQGen.generateQuad(Control,lKey+"QuadB",62.15);

  YagUnitGen.setCF<CF40_22>();
  YagUnitGen.generateYagUnit(Control,lKey+"YagUnit");

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,lKey+"BellowA",0.0,7.58);

  PGen.generatePipe(Control,lKey+"PipeC",0.0,69.2); // measured

  CMGen.generateMag(Control,lKey+"CMagHorC",13.1,0);
  CMGen.generateMag(Control,lKey+"CMagVertC",33.21,1);

  return;
}

void
linac2SPFsegment5(FuncDataBase& Control,
		  const std::string& lKey)
  /*!
    Set the variables for segment3
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{
  ELog::RegMethod RegA("linacVariables[F]","linac2SPFsegment5");

  setVariable::BeamDividerGenerator BDGen;
  setVariable::BellowGenerator BellowGen;
  setVariable::FlatPipeGenerator FPGen;
  setVariable::DipoleDIBMagGenerator DIBGen;

  Control.addVariable(lKey+"XStep",-45.073+linacVar::zeroX);
  Control.addVariable(lKey+"YStep",1420.344+linacVar::zeroY);
  Control.addVariable(lKey+"XYAngle",6.4);

  FPGen.generateFlat(Control,lKey+"FlatA",82.0);
  Control.addVariable(lKey+"FlatAXYAngle",1.6);
  DIBGen.generate(Control,lKey+"DipoleA");

  BDGen.generateDivider(Control,lKey+"BeamA");
  Control.addVariable(lKey+"BeamAXYAngle",1.6);

  FPGen.generateFlat(Control,lKey+"FlatB",82.0);
  Control.addVariable(lKey+"FlatBXYAngle",1.6);
  DIBGen.generate(Control,lKey+"DipoleB");

  // again not larger size
  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,lKey+"BellowA",0.0,7.58);
  Control.addVariable(lKey+"BellowAXYAngle",1.6);

  return;
}

void
TDCsegment14(FuncDataBase& Control,
		   const std::string& lKey)
  /*!
    Set the variables for the main walls
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{
  ELog::RegMethod RegA("linacVariables[F]","TDCsegment14");
  setVariable::PipeGenerator PGen;
  setVariable::BellowGenerator BellowGen;
  setVariable::LinacQuadFGenerator LQGen;
  setVariable::CorrectorMagGenerator CMGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::GateValveGenerator GateGen;

  // number form drawing
  Control.addVariable(lKey+"XStep",-622.286+linacVar::zeroX);   // include 1m offset
  Control.addVariable(lKey+"YStep",4226.013+linacVar::zeroY);        //
  Control.addVariable(lKey+"ZAngle",3.1183);   // this should be 3.1183 deg

  BellowGen.setCF<setVariable::CF40_22>();
  BellowGen.setMat("Stainless304L", "Stainless304L%Void%3.0");
  BellowGen.generateBellow(Control,lKey+"BellowA",0.0,8.82); // measured yStep, length

  PGen.setCF<setVariable::CF40_22>();
  PGen.setMat("Stainless316L");
  PGen.setNoWindow();
  PGen.generatePipe(Control,lKey+"PipeA",0.0,82.7); // measured

  setVariable::DipoleDIBMagGenerator DIBGen;
  DIBGen.generate(Control,lKey+"DM1");

  PGen.setMat("Stainless316L","Stainless304L");
  PGen.generatePipe(Control,lKey+"PipeB",0.0,94.4); // measured

  PGen.setMat("Stainless316L","Stainless316L");
  PGen.generatePipe(Control,lKey+"PipeC",0.0,82.7); // measured

  DIBGen.generate(Control,lKey+"DM2");

  GateGen.setLength(6.3);
  GateGen.setCubeCF<setVariable::CF40_22>();
  GateGen.generateValve(Control,lKey+"GateA",0.0,0);
  Control.addVariable(lKey+"GateAPortALen",2.0);

  BellowGen.generateBellow(Control,lKey+"BellowB",0.0,7.44); // measured

  return;
}

void
TDCsegment15(FuncDataBase& Control,
		   const std::string& lKey)
  /*!
    Set the variables for the main walls
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{
  ELog::RegMethod RegA("linacVariables[F]","TDCsegment15");
  setVariable::PipeGenerator PGen;
  setVariable::BellowGenerator BellowGen;
  setVariable::LinacQuadFGenerator LQGen;
  setVariable::CorrectorMagGenerator CMGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::GateValveGenerator GateGen;
  setVariable::YagScreenGenerator YagGen;

    // number form drawing
  Control.addVariable(lKey+"XStep",-637.608+linacVar::zeroX);   // include 1m offset
  ELog::EM << "YStep increased by 10 cm to avoid cutting segment 14" << ELog::endCrit;
  Control.addVariable(lKey+"YStep",10+4507.259+linacVar::zeroY);

  Control.addVariable(lKey+"XYAngle",0.0);

  PGen.setCF<setVariable::CF40_22>();
  PGen.setMat("Stainless316L","Stainless304L");
  PGen.setNoWindow();
  PGen.generatePipe(Control,lKey+"PipeA",0.0,22.0); // measured

  // Mirror chamber
  std::string name=lKey+"MirrorChamber";
  SimpleTubeGen.setMat("Stainless304");
  SimpleTubeGen.setCF<CF63>();
  PItemGen.setCF<setVariable::CF40_22>(10.0); // 10 is ummy since we set up all port lenght later
  PItemGen.setNoPlate();

  SimpleTubeGen.setFlangeCap(setVariable::CF63::flangeLength,setVariable::CF63::flangeLength);
  SimpleTubeGen.generateTube(Control,name,0.0,22.6); // measured but wrong - it's top/bottom asymmetric and the lower part does not have flange
  Control.addVariable(name+"NPorts",4);

  const Geometry::Vec3D OPos(0,0.0,0);
  const Geometry::Vec3D ZVec(0,0,-1);
  const Geometry::Vec3D XVec(1,0,0);

  PItemGen.setLength(2.2); // measured
  PItemGen.generatePort(Control,name+"Port0",OPos,-ZVec);
  PItemGen.setLength(4.6); // measured
  PItemGen.generatePort(Control,name+"Port1",OPos,ZVec);
  PItemGen.setLength(2.25); // measured
  PItemGen.setPlate(setVariable::CF40_22::flangeLength, "Stainless304");
  PItemGen.generatePort(Control,name+"Port2",OPos,-XVec);
  PItemGen.setLength(2.25); // measured
  PItemGen.generatePort(Control,name+"Port3",OPos,XVec);

  const std::string pumpName=lKey+"IonPump";
  setIonPump3(Control,pumpName);
  Control.addVariable(pumpName+"Port1Length",9.5);
  Control.addVariable(pumpName+"Port2Length",3.2);
  Control.addVariable(pumpName+"FlangeBCapThick",0.0);

  YagGen.generateScreen(Control,lKey+"YAG",1);   // closed

  PGen.generatePipe(Control,lKey+"PipeB",0.0,130.0);

  return;
}

void
TDCsegment16(FuncDataBase& Control,
		   const std::string& lKey)
  /*!
    Set the variables for the main walls
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{
  ELog::RegMethod RegA("linacVariables[F]","TDCsegment16");

  setVariable::BellowGenerator BellowGen;
  setVariable::BPMGenerator BPMGen;

  setVariable::PipeGenerator PGen;
  setVariable::LinacQuadFGenerator LQGen;
  PGen.setNoWindow();
  setVariable::CorrectorMagGenerator CMGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::GateValveGenerator GateGen;
  setVariable::YagScreenGenerator YagGen;

  // coordinates form drawing
  Control.addVariable(lKey+"XStep",-637.608+linacVar::zeroX);   // include 1m offset
  Control.addVariable(lKey+"YStep",4730.259+linacVar::zeroY);        //
  Control.addVariable(lKey+"XYAngle",0.0);

  BellowGen.setCF<setVariable::CF40_22>();
  BellowGen.setMat("Stainless304L", "Stainless304L%Void%3.0");
  BellowGen.generateBellow(Control,lKey+"BellowA",0.0,7.5);

  BPMGen.setCF<setVariable::CF40_22>();
  BPMGen.generateBPM(Control,lKey+"BPM",0.0);

  const double pipeALength(34.0);
  PGen.setCF<setVariable::CF40_22>();
  PGen.setMat("Stainless316L","Stainless304L");
  PGen.generatePipe(Control,lKey+"PipeA",0.0,34.0); // measured
  Control.addVariable(lKey+"PipeARadius",0.4); // inner radius - measured
  Control.addVariable(lKey+"PipeAFeThick",0.08); // wall thick - measured

  // QG (QH) type quadrupole magnet
  LQGen.setRadius(0.56, 2.31); // 0.56 - measured
  LQGen.generateQuad(Control,lKey+"Quad",pipeALength/2.0);

  Control.addVariable(lKey+"QuadLength",18.7); // measured - inner box lengh
  // measured - inner box half width/height
  Control.addVariable(lKey+"QuadYokeOuter",9.5);
  // adjusted so that nose is 1 cm thick as in the STEP file
  Control.addVariable(lKey+"QuadPolePitch",26.0);

  PGen.setCF<setVariable::CF40_22>();
  PGen.generatePipe(Control,lKey+"PipeB",0.0,40.0); // measured

  CMGen.generateMag(Control,lKey+"CMagH",10.0,0); // measured
  CMGen.generateMag(Control,lKey+"CMagV",28.0,1); // measured

  BellowGen.generateBellow(Control,lKey+"BellowB",0.0,7.5); // measured

  setIonPump2(Control,lKey+"IonPump");
  Control.addVariable(lKey+"IonPumpYAngle",90.0);

  PGen.generatePipe(Control,lKey+"PipeC",0.0,126.0); // measured

  return;
}

void
TDCsegment18(FuncDataBase& Control,
		   const std::string& lKey)
  /*!
    Set the variables for the main walls
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{
  ELog::RegMethod RegA("linacVariables[F]","TDCsegment18");

  setVariable::BellowGenerator BellowGen;
  setVariable::BPMGenerator BPMGen;

  setVariable::PipeGenerator PGen;
  PGen.setNoWindow();
  setVariable::LinacQuadFGenerator LQGen;
  setVariable::CorrectorMagGenerator CMGen;

  // coordinates form drawing
  Control.addVariable(lKey+"XStep",-637.608+linacVar::zeroX);
  Control.addVariable(lKey+"YStep",5780.261+linacVar::zeroY);
  Control.addVariable(lKey+"XYAngle",0.0);

  BellowGen.setCF<setVariable::CF40_22>();
  BellowGen.setMat("Stainless304L", "Stainless304L%Void%3.0");
  BellowGen.generateBellow(Control,lKey+"BellowA",0.0,7.5);

  setIonPump2(Control, lKey+"IonPump");
  Control.addVariable(lKey+"IonPumpYAngle",90.0);

  BellowGen.generateBellow(Control,lKey+"BellowB",0.0,7.5); // measured

  BPMGen.setCF<setVariable::CF40_22>();
  BPMGen.generateBPM(Control,lKey+"BPM",0.0);

  /// Quad and PipeA
  const double pipeALength(34.0); // measured
  PGen.setCF<setVariable::CF40_22>();
  PGen.setMat("Stainless316L","Stainless304L");
  PGen.generatePipe(Control,lKey+"PipeA",0.0,34.0); //
  Control.addVariable(lKey+"PipeARadius",0.4); // inner radius -
  Control.addVariable(lKey+"PipeAFeThick",0.08); // wall thick -

  // QG (QH) type quadrupole magnet
  LQGen.setRadius(0.56, 2.31); // 0.56 -> measured (QH)
  LQGen.generateQuad(Control,lKey+"Quad",pipeALength/2.0);

  // actually the pole width is 2.6, but then they cut with poleRadius of 0.56 cm
  // I think it's more important to have correct poleRadius since it defines aperture
  // of particles with high Lorentz factor
  Control.addVariable(lKey+"QuadPoleWidth",1.4);

  Control.addVariable(lKey+"QuadLength",18.7); //  - inner box lengh
  //  - inner box half width/height
  Control.addVariable(lKey+"QuadYokeOuter",9.5);

  PGen.setCF<setVariable::CF40_22>();
  PGen.generatePipe(Control,lKey+"PipeB",0.0,127.3); //

  CMGen.generateMag(Control,lKey+"CMagH",10.0,0); //
  CMGen.generateMag(Control,lKey+"CMagV",28.0,1); //



  return;
}

void
wallVariables(FuncDataBase& Control,
	      const std::string& wallKey)
  /*!
    Set the variables for the main walls
    \param Control :: DataBase to use
    \param wallKey :: name before part names
  */
{
  ELog::RegMethod RegA("linacVariables[F]","wallVariables");

  Control.addVariable(wallKey+"MainLength",9880.0);
  Control.addVariable(wallKey+"LinearRCutLength",3812.0);
  Control.addVariable(wallKey+"LinearLTurnLength",3672.0);
  Control.addVariable(wallKey+"RightWallStep",145.0);
  Control.addVariable(wallKey+"SPFAngleLength",4124.0);
  Control.addVariable(wallKey+"SPFAngle",12.7);

  Control.addVariable(wallKey+"LinearWidth",981.0);
  Control.addVariable(wallKey+"WallThick",39.0);

  Control.addVariable(wallKey+"FloorDepth",134.0);  // GUESS
  Control.addVariable(wallKey+"RoofHeight",140.0);  // GUESS

  Control.addVariable(wallKey+"RoofThick",90.0);
  Control.addVariable(wallKey+"FloorThick",50.0);

  // Extra for boundary
  Control.addVariable(wallKey+"BoundaryWidth",200.0);
  Control.addVariable(wallKey+"BoundaryHeight",100.0);

  // Midwalls: MUST BE INFRONT OF LinearLTurnPoint
  Control.addVariable(wallKey+"MidTThick",150.0);
  Control.addVariable(wallKey+"MidTXStep",43.0);
  Control.addVariable(wallKey+"MidTYStep",3357.0);  // to flat of T
  Control.addVariable(wallKey+"MidTAngle",13.0);  // slopes
  Control.addVariable(wallKey+"MidTThick",200.0);  // Thick of T

  Control.addVariable(wallKey+"MidTLeft",427.1);  // from mid line
  Control.addVariable(wallKey+"MidTFrontAngleStep",277.0);  //  flat
  Control.addVariable(wallKey+"MidTBackAngleStep",301.0);  // out flat
  Control.addVariable(wallKey+"MidTRight",283.0);  // from mid line

  Control.addVariable(wallKey+"KlysDivThick",100.0);

  Control.addVariable(wallKey+"MidGateOut",206.0);
  Control.addVariable(wallKey+"MidGateWidth",432.0);
  Control.addVariable(wallKey+"MidGateWall",100.0);


  Control.addVariable(wallKey+"KlystronXStep",28.0);
  Control.addVariable(wallKey+"KlystronLen",978.0);
  Control.addVariable(wallKey+"KlystronFrontWall",100.0);
  Control.addVariable(wallKey+"KlystronSideWall",150.0);


  Control.addVariable(wallKey+"VoidMat","Void");
  Control.addVariable(wallKey+"WallMat","Concrete");
  Control.addVariable(wallKey+"RoofMat","Concrete");
  Control.addVariable(wallKey+"FloorMat","Concrete");

  return;
}

}  // NAMESPACE linacVAR

void
LINACvariables(FuncDataBase& Control)
  /*!
    Function to set the control variables and constants
    \param Control :: Function data base to add constants too
  */
{
  ELog::RegMethod RegA("linacVariables[F]","LINACVariables");

  linacVar::wallVariables(Control,"InjectionHall");

  // Segment 1-14
  Control.addVariable("TDCl2spfXStep",linacVar::zeroX);
  Control.addVariable("TDCl2spfYStep",linacVar::zeroY);
  Control.addVariable("TDCl2spfOuterLeft",80.0);
  Control.addVariable("TDCl2spfOuterRight",140.0);
  Control.addVariable("TDCl2spfOuterTop",100.0);

  Control.addVariable("TDCl2spfTurnXStep",linacVar::zeroX-80.0);
  Control.addVariable("TDCl2spfTurnYStep",linacVar::zeroY);
  Control.addVariable("TDCl2spfTurnOuterLeft",80.0);
  Control.addVariable("TDCl2spfTurnOuterRight",140.0);
  Control.addVariable("TDCl2spfTurnOuterTop",100.0);

  Control.addVariable("TDCtdcXStep",-622.286+linacVar::zeroX);
  Control.addVariable("TDCtdcYStep",4226.013+linacVar::zeroY);
  Control.addVariable("TDCtdcOuterLeft",100.0);
  Control.addVariable("TDCtdcOuterRight",100.0);
  Control.addVariable("TDCtdcOuterTop",100.0);


  linacVar::linac2SPFsegment1(Control,"L2SPF1");
  linacVar::linac2SPFsegment2(Control,"L2SPF2");
  linacVar::linac2SPFsegment3(Control,"L2SPF3");
  linacVar::linac2SPFsegment4(Control,"L2SPF4");
  linacVar::linac2SPFsegment5(Control,"L2SPF5");

  /// TDC segments 14-28
  linacVar::TDCsegment14(Control,"TDC14");
  linacVar::TDCsegment15(Control,"TDC15");
  linacVar::TDCsegment16(Control,"TDC16");
  //  linacVar::TDCsegment17(Control,"TDC17");
  linacVar::TDCsegment18(Control,"TDC18");

  return;
}

}  // NAMESPACE setVariable
