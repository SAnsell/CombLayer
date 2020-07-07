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
#include "LinacQuadGenerator.h"
#include "LinacSexuGenerator.h"
#include "PipeTubeGenerator.h"
#include "PortItemGenerator.h"
#include "BPMGenerator.h"
#include "CylGateValveGenerator.h"
#include "GateValveGenerator.h"
#include "DipoleDIBMagGenerator.h"
#include "EArrivalMonGenerator.h"
#include "YagScreenGenerator.h"
#include "YagUnitGenerator.h"
#include "YagUnitBigGenerator.h"
#include "FlatPipeGenerator.h"
#include "TriPipeGenerator.h"
#include "BeamDividerGenerator.h"
#include "ScrapperGenerator.h"
#include "SixPortGenerator.h"
#include "CeramicSepGenerator.h"
#include "EBeamStopGenerator.h"
#include "TWCavityGenerator.h"
#include "subPipeUnit.h"
#include "MultiPipeGenerator.h"
#include "ButtonBPMGenerator.h"

namespace setVariable
{

namespace linacVar
{
  void wallVariables(FuncDataBase&,const std::string&);
  void setIonPump1Port(FuncDataBase&,const std::string&);
  void setIonPump2Port(FuncDataBase&,const std::string&);
  void setIonPump3Port(FuncDataBase&,const std::string&);
  void setIonPump3OffsetPort(FuncDataBase&,const std::string&);

  void linac2SPFsegment1(FuncDataBase&,const std::string&);
  void linac2SPFsegment2(FuncDataBase&,const std::string&);
  void linac2SPFsegment3(FuncDataBase&,const std::string&);
  void linac2SPFsegment4(FuncDataBase&,const std::string&);
  void linac2SPFsegment5(FuncDataBase&,const std::string&);
  void linac2SPFsegment6(FuncDataBase&,const std::string&);
  void linac2SPFsegment7(FuncDataBase&,const std::string&);
  void linac2SPFsegment8(FuncDataBase&,const std::string&);
  void linac2SPFsegment9(FuncDataBase&,const std::string&);
  void linac2SPFsegment10(FuncDataBase&,const std::string&);
  void linac2SPFsegment11(FuncDataBase&,const std::string&);
  void linac2SPFsegment12(FuncDataBase&,const std::string&);
  void linac2SPFsegment13(FuncDataBase&,const std::string&);

  void Segment14(FuncDataBase&,const std::string&);
  void Segment15(FuncDataBase&,const std::string&);
  void Segment16(FuncDataBase&,const std::string&);
  void Segment17(FuncDataBase&,const std::string&);
  void Segment18(FuncDataBase&,const std::string&);
  void Segment19(FuncDataBase&,const std::string&);
  void Segment20(FuncDataBase&,const std::string&);
  void Segment21(FuncDataBase&,const std::string&);
  void Segment22(FuncDataBase&,const std::string&);
  void Segment23(FuncDataBase&,const std::string&);
  void Segment24(FuncDataBase&,const std::string&);
  void Segment25(FuncDataBase&,const std::string&);
  void Segment27(FuncDataBase&,const std::string&);
  void Segment28(FuncDataBase&,const std::string&);
  void Segment30(FuncDataBase&,const std::string&);
  void Segment31(FuncDataBase&,const std::string&);
  void Segment32(FuncDataBase&,const std::string&);
  void Segment33(FuncDataBase&,const std::string&);
  void Segment34(FuncDataBase&,const std::string&);
  void Segment35(FuncDataBase&,const std::string&);
  void Segment36(FuncDataBase&,const std::string&);
  void Segment37(FuncDataBase&,const std::string&);
  void Segment38(FuncDataBase&,const std::string&);
  void Segment39(FuncDataBase&,const std::string&);

  const double zeroX(152.0);   // coordiated offset to master
  const double zeroY(481.0);    // drawing README.pdf
  const Geometry::Vec3D zeroOffset(zeroX,zeroY,0.0);

void
setIonPump1Port(FuncDataBase& Control,
		const std::string& name)
/*!
  Set the 1 port ion pump variables
  \param Control :: DataBase to use
  \param name :: name prefix
  \param nPorts :: number of ports
 */
{
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::PortItemGenerator PItemGen;

  const Geometry::Vec3D OPos(0,0.0,0);
  const Geometry::Vec3D XVec(1,0,0);

  SimpleTubeGen.setCF<CF40_22>();
  SimpleTubeGen.setMat("Stainless304L"); // mat checked
  SimpleTubeGen.generateTube(Control,name,0.0,12.6); // measured

  Control.addVariable(name+"NPorts",1);
  PItemGen.setCF<setVariable::CF40_22>(5.1); // measured
  PItemGen.setPlate(setVariable::CF40_22::flangeLength, "Stainless304L"); // mat checked
  PItemGen.generatePort(Control,name+"Port0",OPos,-XVec);

}

void
setIonPump2Port(FuncDataBase& Control,
		const std::string& name)
/*!
  Set the 2 port ion pump variables
  \param Control :: DataBase to use
  \param name :: name prefix
 */
{
  ELog::RegMethod RegA("linacVariables[F]","setIonPump2Port");

  setVariable::PortItemGenerator PItemGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;

  const Geometry::Vec3D OPos(0.0,0.0,0.0);
  const Geometry::Vec3D XVec(1,0,0);

  SimpleTubeGen.setMat("Stainless304L");
  SimpleTubeGen.setCF<setVariable::CF63>();

  SimpleTubeGen.generateBlank(Control,name,0.0,25.8);
  Control.addVariable(name+"NPorts",2);
  Control.addVariable(name+"FlangeCapThick",setVariable::CF63::flangeLength);
  Control.addVariable(name+"FlangeCapMat","Stainless304L");


  // Outer radius of the vertical pipe
  const double outerR =
    setVariable::CF63::innerRadius+setVariable::CF63::wallThick;
  // length of ports 0 and 1
  // measured at Segment18 from front/back to the centre
  const double L0(8.5 - outerR);
  const double L1(7.5 - outerR);

  PItemGen.setCF<setVariable::CF40_22>(L0); // Port0 length
  PItemGen.setNoPlate();
  PItemGen.generatePort(Control,name+"Port0",OPos,-XVec);

  PItemGen.setLength(L1);
  PItemGen.generatePort(Control,name+"Port1",OPos,XVec);

  return;
}

void
setIonPump3Port(FuncDataBase& Control,const std::string& name)
/*!
  Set the 3 port ion pump variables
  \param Control :: DataBase to use
  \param name :: name prefix
 */
{
  ELog::RegMethod RegA("linacVariables[F]","setIonPump3Port");

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
  Control.addVariable(name+"YAngle",180.0);

  PItemGen.setCF<setVariable::CF50>(10.0);
  PItemGen.setPlate(setVariable::CF50::flangeLength, "Stainless304");
  PItemGen.generatePort(Control,name+"Port0",OPos,ZVec);

  // total ion pump length
  const double totalLength(16.0); // measured
  // length of ports 1 and 2
  double L = totalLength -
    (setVariable::CF63::innerRadius+setVariable::CF63::wallThick)*2.0;
  L /= 2.0;

  PItemGen.setCF<setVariable::CF40_22>(L);
  PItemGen.setNoPlate();
  PItemGen.generatePort(Control,name+"Port1",OPos,-XVec);
  Control.addVariable(name+"Port1Radius",1.7); // measured

  PItemGen.setCF<setVariable::CF40_22>(L);
  PItemGen.setNoPlate();
  PItemGen.generatePort(Control,name+"Port2",OPos,XVec);
  Control.addVariable(name+"Port2Radius",1.7); // measured

  return;
}
void
setIonPump3OffsetPort(FuncDataBase& Control,const std::string& name)
/*!
  Set the 3 port ion pump variables (with screen etc)
  \param Control :: DataBase to use
  \param name :: name prefix
 */
{
  ELog::RegMethod RegA("linacVariables[F]","setIonPump5Port");

  setVariable::PortItemGenerator PItemGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;

  const double fullLen(25.8);

  const Geometry::Vec3D OPos(0.0,5.0,0.0);
  const Geometry::Vec3D ZVec(0,0,-1);
  const Geometry::Vec3D XVec(1,0,0);

  SimpleTubeGen.setMat("Stainless304L");
  SimpleTubeGen.setCF<setVariable::CF63>();

  SimpleTubeGen.setCapMat("Stainless304L");
  SimpleTubeGen.setCap(1,1);
  SimpleTubeGen.generateTube(Control,name,0.0,fullLen);

  Control.addVariable(name+"NPorts",3);
  Control.addVariable(name+"YAngle",180.0);

  PItemGen.setCF<setVariable::CF40>(10.0);

  // Outer radius of the vertical pipe
  const double outerR =
    setVariable::CF63::innerRadius+setVariable::CF63::wallThick;
  const double L0(9.5 - outerR);
  const double L1(10.5 - outerR);
  const double L2(12.5 - outerR);

  PItemGen.setNoPlate();
  PItemGen.setLength(L0);
  PItemGen.generatePort(Control,name+"Port0",OPos,-ZVec);

  PItemGen.setLength(L1);
  PItemGen.generatePort(Control,name+"Port1",OPos,ZVec);

  const Geometry::Vec3D CPos(0,0,0);
  PItemGen.setCF<setVariable::CF50>(L2);
  PItemGen.generatePort(Control,name+"Port2",CPos,XVec);

  return;
}

void
linac2SPFsegment1(FuncDataBase& Control,
		  const std::string& lKey)
  /*!
    Set the variables for segment 1
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{
  ELog::RegMethod RegA("linacVariables[F]","linac2SPFsegment1");
  setVariable::PipeGenerator PGen;
  setVariable::BellowGenerator BellowGen;
  setVariable::LinacQuadGenerator LQGen;
  setVariable::CorrectorMagGenerator CMGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::PortItemGenerator PItemGen;

  // exactly 1m from wall.
  const Geometry::Vec3D startPt(0,0,0);
  const Geometry::Vec3D endPt(0,395.2,0);
  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);

  PGen.setCF<setVariable::CF40_22>();
  PGen.setNoWindow();
  PGen.generatePipe(Control,lKey+"PipeA",16.15);
  // note larger unit
  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,lKey+"BellowA",7.5);

  //  corrector mag and pie
  PGen.generatePipe(Control,lKey+"PipeB",55.90);
  CMGen.generateMag(Control,lKey+"CMagHorrA",30.80,0);
  CMGen.generateMag(Control,lKey+"CMagVertA",46.3,1);

  PGen.generatePipe(Control,lKey+"PipeC",33.85);
  PGen.generatePipe(Control,lKey+"PipeD",114.3);

  CMGen.generateMag(Control,lKey+"CMagHorrB",51.50,0);
  CMGen.generateMag(Control,lKey+"CMagVertB",68.50,1);

  LQGen.generateQuad(Control,lKey+"QuadA",94.0);


  PGen.generatePipe(Control,lKey+"PipeE",21.30);
  PGen.generatePipe(Control,lKey+"PipeF",130.0);

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
    Set the variables for segment 2
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{
  ELog::RegMethod RegA("linacVariables[F]","linac2SPFsegment2");

  setVariable::PipeGenerator PGen;
  setVariable::BellowGenerator BellowGen;
  setVariable::LinacQuadGenerator LQGen;
  setVariable::BPMGenerator BPMGen;
  setVariable::CylGateValveGenerator CGateGen;
  setVariable::EArrivalMonGenerator EArrGen;
  setVariable::YagScreenGenerator YagScreenGen;
  setVariable::YagUnitGenerator YagUnitGen;

  const Geometry::Vec3D startPt(0,395.2,0);
  const Geometry::Vec3D endPt(0,881.06,0);
  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);

  PGen.setCF<setVariable::CF40_22>();
  PGen.setNoWindow();

  // lengthened to fit quad +2cm
  PGen.generatePipe(Control,lKey+"PipeA",35.0);

  LQGen.generateQuad(Control,lKey+"QuadA",35.0/2.0);

  BPMGen.setCF<setVariable::CF40>();
  BPMGen.generateBPM(Control,lKey+"BPMA",0.0);

  // note larger unit
  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,lKey+"BellowA",7.5);

  PGen.generatePipe(Control,lKey+"PipeB",114.0);

  LQGen.generateQuad(Control,lKey+"QuadB",72.0);

  CGateGen.setRotate(1);
  CGateGen.generateGate(Control,lKey+"GateTube",0);

  PGen.generatePipe(Control,lKey+"PipeC",31.0);

  EArrGen.generateMon(Control,lKey+"BeamArrivalMon",0.0);

  PGen.generatePipe(Control,lKey+"PipeD",75.0);

  // again not larger size
  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,lKey+"BellowB",7.5);

  BPMGen.generateBPM(Control,lKey+"BPMB",0.0);

  PGen.generatePipe(Control,lKey+"PipeE",133.34);

  LQGen.generateQuad(Control,lKey+"QuadC",23.54);
  LQGen.generateQuad(Control,lKey+"QuadD",73.0);
  LQGen.generateQuad(Control,lKey+"QuadE",113.2);


  YagUnitGen.generateYagUnit(Control,lKey+"YagUnit");
  YagScreenGen.generateScreen(Control,lKey+"YagScreen",1);   // closed
  Control.addVariable(lKey+"YagScreenYAngle",-90.0);


  return;
}

void
linac2SPFsegment3(FuncDataBase& Control,
		  const std::string& lKey)
  /*!
    Set the variables for segment 3
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

  const Geometry::Vec3D startPt(0,881.06,0);
  const Geometry::Vec3D endPt(-15.322,1155.107,0);
  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);


  PGen.setCF<setVariable::CF40_22>();
  PGen.setMat("Stainless316L");
  PGen.setNoWindow();

  // again not larger size
  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,lKey+"BellowA",7.3);

  FPGen.generateFlat(Control,lKey+"FlatA",83.0);
  Control.addVariable(lKey+"FlatAXYAngle",1.6);
  DIBGen.generate(Control,lKey+"DipoleA");

  PGen.generatePipe(Control,lKey+"PipeA",92.40); // measured
  Control.addVariable(lKey+"PipeAXYAngle",1.6);
  // Control.addVariable(lKey+"PipeAXStep",-1.2);
  // Control.addVariable(lKey+"PipeAFlangeFrontXStep",1.2);

  CMGen.generateMag(Control,lKey+"CMagHorA",64.0,0);
  CMGen.generateMag(Control,lKey+"CMagVertA",80.0,1);

  FPGen.generateFlat(Control,lKey+"FlatB",84.2);
  Control.addVariable(lKey+"FlatBXYAngle",1.6);

  DIBGen.generate(Control,lKey+"DipoleB");
  // again not larger size
  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,lKey+"BellowB",7.6);
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
  setVariable::LinacQuadGenerator LQGen;
  setVariable::LinacSexuGenerator LSGen;
  setVariable::CorrectorMagGenerator CMGen;
  setVariable::YagUnitGenerator YagUnitGen;
  setVariable::YagScreenGenerator YagScreenGen;


  const Geometry::Vec3D startPt(-15.322,1155.107,0);
  const Geometry::Vec3D endPt(-45.073,1420.334,0);
  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"XYAngle",6.4);

  PGen.setCF<setVariable::CF40_22>();
  PGen.setMat("Stainless316L");
  PGen.setNoWindow();
  PGen.generatePipe(Control,lKey+"PipeA",67.0); // measured

  BPMGen.setCF<setVariable::CF40_22>();
  BPMGen.generateBPM(Control,lKey+"BPMA",0.0);

  PGen.generatePipe(Control,lKey+"PipeB",80.1); // measured

  LQGen.generateQuad(Control,lKey+"QuadA",17.2);
  LSGen.generateSexu(Control,lKey+"SexuA",39.1);
  LQGen.generateQuad(Control,lKey+"QuadB",62.15);

  YagUnitGen.setCF<CF50>();
  YagUnitGen.generateYagUnit(Control,lKey+"YagUnit");

  YagScreenGen.generateScreen(Control,lKey+"YagScreen",1);   // closed
  Control.addVariable(lKey+"YagScreenYAngle",-90.0);

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,lKey+"BellowA",7.5);

  PGen.generatePipe(Control,lKey+"PipeC",70.0); // measured

  CMGen.generateMag(Control,lKey+"CMagHorC",13.1,0);
  CMGen.generateMag(Control,lKey+"CMagVertC",33.21,1);

  return;
}

void
linac2SPFsegment5(FuncDataBase& Control,
		  const std::string& lKey)
  /*!
    Set the variables for segment 5
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{
  ELog::RegMethod RegA("linacVariables[F]","linac2SPFsegment5");

  setVariable::CF40 CF40unit;
  setVariable::BeamDividerGenerator BDGen(CF40unit);
  setVariable::BellowGenerator BellowGen;
  setVariable::FlatPipeGenerator FPGen;
  setVariable::DipoleDIBMagGenerator DIBGen;

  const Geometry::Vec3D startPt(-45.073,1420.334,0);
  const Geometry::Vec3D endPt(-90.011,1683.523,0);
  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"XYAngle",6.4);

  FPGen.generateFlat(Control,lKey+"FlatA",82.0);
  Control.addVariable(lKey+"FlatAXYAngle",1.6);
  DIBGen.generate(Control,lKey+"DipoleA");

  BDGen.generateDivider(Control,lKey+"BeamA",1.6);

  FPGen.generateFlat(Control,lKey+"FlatB",82.0);
  Control.addVariable(lKey+"FlatBXYAngle",1.6);
  DIBGen.generate(Control,lKey+"DipoleB");

  // again not larger size
  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,lKey+"BellowA",7.5);
  Control.addVariable(lKey+"BellowAXYAngle",1.6);

  return;
}

void
linac2SPFsegment6(FuncDataBase& Control,
		  const std::string& lKey)
  /*!
    Set the variables for segment 6
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{
  ELog::RegMethod RegA("linacVariables[F]","linac2SPFsegment6");

  setVariable::PipeGenerator PGen;
  setVariable::EBeamStopGenerator EBGen;
  setVariable::ScrapperGenerator SCGen;
  setVariable::CeramicSepGenerator CSGen;

  const Geometry::Vec3D startPt(-90.011,1683.523,0.0);
  const Geometry::Vec3D endPt(-147.547,1936.770,0.0);
  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"XYAngle",12.8);

  PGen.setCF<setVariable::CF40_22>();
  PGen.setNoWindow();

  PGen.generatePipe(Control,lKey+"PipeA",61.75);

  PGen.generatePipe(Control,lKey+"PipeB",20.5);

  PGen.setBFlangeCF<setVariable::CF63>();
  PGen.generatePipe(Control,lKey+"PipeC",55.0);

  SCGen.generateScrapper(Control,lKey+"Scrapper",1.0);   // z lift

  PGen.setCF<setVariable::CF40_22>();
  PGen.setAFlangeCF<setVariable::CF63>();
  PGen.generatePipe(Control,lKey+"PipeD",19.50);

  CSGen.generateCeramicSep(Control,lKey+"CeramicA");

  EBGen.generateEBeamStop(Control,lKey+"EBeam",0);

  CSGen.generateCeramicSep(Control,lKey+"CeramicB");


  return;
}

void
linac2SPFsegment7(FuncDataBase& Control,
		  const std::string& lKey)
  /*!
    Set the variables for segment 7
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{
  ELog::RegMethod RegA("linacVariables[F]","linac2SPFsegment7");

  setVariable::PipeGenerator PGen;
  setVariable::BPMGenerator BPMGen;
  setVariable::LinacQuadGenerator LQGen;
  setVariable::CorrectorMagGenerator CMGen;

  const Geometry::Vec3D startPt(-147.547,1936.770,0.0);
  const Geometry::Vec3D endPt(-206.146,2194.697,0.0);

  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"XYAngle",12.8);

  PGen.setCF<setVariable::CF40_22>();
  PGen.setNoWindow();

  PGen.generatePipe(Control,lKey+"PipeA",102.31);
  CMGen.generateMag(Control,lKey+"CMagHorA",37.25,0);
  LQGen.generateQuad(Control,lKey+"QuadA",81.0);
  BPMGen.generateBPM(Control,lKey+"BPM",0.0);
  PGen.generatePipe(Control,lKey+"PipeB",140.26);
  CMGen.generateMag(Control,lKey+"CMagVertA",11.87,1);


  return;
}

void
linac2SPFsegment8(FuncDataBase& Control,
		  const std::string& lKey)
  /*!
    Set the variables for segment 8
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{
  ELog::RegMethod RegA("linacVariables[F]","linac2SPFsegment8");

  setVariable::BellowGenerator BellowGen;
  setVariable::EBeamStopGenerator EBGen;
  setVariable::PipeGenerator PGen;

  const Geometry::Vec3D startPt(-206.146,2194.697,0.0);
  const Geometry::Vec3D endPt(-288.452,2556.964,0.0);

  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"XYAngle",12.8);

  PGen.setCF<setVariable::CF40_22>();
  PGen.setNoWindow();

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,lKey+"BellowA",7.5);
  EBGen.generateEBeamStop(Control,lKey+"EBeam",0);
  BellowGen.generateBellow(Control,lKey+"BellowB",7.5);
  PGen.generatePipe(Control,lKey+"PipeA",308.5);


  return;
}

void
linac2SPFsegment9(FuncDataBase& Control,
		  const std::string& lKey)
  /*!
    Set the variables for segment 8
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{
  ELog::RegMethod RegA("linacVariables[F]","linac2SPFsegment9");

  setVariable::PipeGenerator PGen;
  setVariable::BellowGenerator BellowGen;
  setVariable::BPMGenerator BPMGen;
  setVariable::CorrectorMagGenerator CMGen;
  setVariable::LinacQuadGenerator LQGen;
  setVariable::CeramicSepGenerator CSGen;


  const Geometry::Vec3D startPt(-288.452,2556.964,0.0);
  const Geometry::Vec3D endPt(-323.368,2710.648,0.0);


  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"XYAngle",12.8);

  PGen.setCF<setVariable::CF40_22>();
  PGen.setNoWindow();
  BellowGen.setCF<setVariable::CF40>();

  CSGen.generateCeramicSep(Control,lKey+"CeramicBellowA");
  setIonPump2Port(Control, lKey+"PumpA");

  PGen.generatePipe(Control,lKey+"PipeA",56.6);

  CMGen.generateMag(Control,lKey+"CMagVertA",20.50,1);
  CMGen.generateMag(Control,lKey+"CMagHorA",48.50,0);

  BellowGen.generateBellow(Control,lKey+"BellowB",7.5);
  BPMGen.generateBPM(Control,lKey+"BPM",0.0);

  PGen.generatePipe(Control,lKey+"PipeB",34.0);
  LQGen.generateQuad(Control,lKey+"QuadA",17.50);

  BellowGen.generateBellow(Control,lKey+"BellowC",7.5);

  return;
}

void
linac2SPFsegment10(FuncDataBase& Control,
		   const std::string& lKey)
  /*!
    Set the variables for segment 10
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{
  ELog::RegMethod RegA("linacVariables[F]","linac2SPFsegment10");

  setVariable::PipeGenerator PGen;
  setVariable::BellowGenerator BellowGen;
  setVariable::CorrectorMagGenerator CMGen;
  setVariable::LinacQuadGenerator LQGen;
  setVariable::GateValveGenerator GateGen;

  const Geometry::Vec3D startPt(-323.368,2710.648,0.0);
  const Geometry::Vec3D endPt(-492.992,3457.251,0.0);

  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"XYAngle",12.8);

  Control.addVariable(lKey+"WallRadius",4.0);

  PGen.setCF<setVariable::CF40_22>();
  PGen.setNoWindow();
  BellowGen.setCF<setVariable::CF40>();
  GateGen.setLength(3.0);
  GateGen.setCubeCF<setVariable::CF40>();

  PGen.generatePipe(Control,lKey+"PipeA",452.0);
  BellowGen.generateBellow(Control,lKey+"BellowA",7.5);

  GateGen.generateValve(Control,lKey+"GateValve",0.0,0);
  setIonPump2Port(Control, lKey+"PumpA");

  PGen.generatePipe(Control,lKey+"PipeB",152.00);
  BellowGen.generateBellow(Control,lKey+"BellowB",7.5);  

  PGen.generatePipe(Control,lKey+"PipeC",125.0);

  LQGen.generateQuad(Control,lKey+"QuadA",33.5);
  CMGen.generateMag(Control,lKey+"CMagVertA",115.0,1);
  return;
}

void
linac2SPFsegment11(FuncDataBase& Control,
		   const std::string& lKey)
  /*!
    Set the variables for segment 11
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{
  ELog::RegMethod RegA("linacVariables[F]","linac2SPFsegment11");

  setVariable::PipeGenerator PGen;
  setVariable::BellowGenerator BellowGen;
  setVariable::BPMGenerator BPMGen;
  setVariable::CorrectorMagGenerator CMGen;
  setVariable::LinacQuadGenerator LQGen;
  setVariable::YagUnitGenerator YagUnitGen;
  setVariable::YagScreenGenerator YagScreenGen;

  const Geometry::Vec3D startPt(-492.992,3457.251,0.0);
  const Geometry::Vec3D endPt(-547.597,3697.597,0.0);

  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"XYAngle",12.8);

  PGen.setCF<setVariable::CF40_22>();
  PGen.setNoWindow();
  BellowGen.setCF<setVariable::CF40>();

  BellowGen.generateBellow(Control,lKey+"BellowA",7.5);
  BPMGen.setCF<setVariable::CF40>();
  BPMGen.generateBPM(Control,lKey+"BPM",0.0);

  PGen.generatePipe(Control,lKey+"PipeA",43.50);
  LQGen.generateQuad(Control,lKey+"QuadA",20.5);

  setIonPump3OffsetPort(Control,lKey+"PumpA");

  YagUnitGen.generateYagUnit(Control,lKey+"YagUnit");

  YagScreenGen.generateScreen(Control,lKey+"YagScreen",1);   // closed
  Control.addVariable(lKey+"YagScreenYAngle",-90.0);

  PGen.generatePipe(Control,lKey+"PipeB",153.30);

  CMGen.generateMag(Control,lKey+"CMagHorA",10.0,1);
  return;
}

void
linac2SPFsegment12(FuncDataBase& Control,
		   const std::string& lKey)
  /*!
    Set the variables for segment 12
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{
  ELog::RegMethod RegA("linacVariables[F]","linac2SPFsegment12");

  setVariable::CF63 CF63unit;
  setVariable::PipeGenerator PGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::BellowGenerator BellowGen;
  setVariable::FlatPipeGenerator FPGen;
  setVariable::DipoleDIBMagGenerator DIBGen;
  setVariable::BeamDividerGenerator BDGen(CF63unit);
  setVariable::PipeTubeGenerator SimpleTubeGen;

  const Geometry::Vec3D startPtA(-547.597,3697.597,0.0);
  const Geometry::Vec3D endPtA(-609.286,3969.122,0.0);  // to segment 13
  const Geometry::Vec3D endPtB(-593.379,3968.258,0.0);  // to segment 30

  Control.addVariable(lKey+"Tolerance",0.3);
  Control.addVariable(lKey+"Offset",startPtA+linacVar::zeroOffset);
  Control.addVariable(lKey+"OffsetA",startPtA+linacVar::zeroOffset);
  Control.addVariable(lKey+"OffsetB",startPtA+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffsetA",endPtA+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffsetB",endPtB+linacVar::zeroOffset);

  Control.addVariable(lKey+"FrontLinkA","front");
  Control.addVariable(lKey+"BackLinkA","straightExit");
  Control.addVariable(lKey+"FrontLinkB","front");
  Control.addVariable(lKey+"BackLinkB","magnetExit");

  Control.addVariable(lKey+"XYAngle",12.8);
  
  PGen.setCF<setVariable::CF40_22>();
  PGen.setNoWindow();

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,lKey+"BellowA",7.5);
  
  FPGen.generateFlat(Control,lKey+"FlatA",85.4);
  Control.addVariable(lKey+"FlatAXYAngle",-1.6);

  DIBGen.generate(Control,lKey+"DipoleA");

  //  BDGen.setMainSize(60.0,3.2);
  BDGen.setAFlangeCF<setVariable::CF50>();
  BDGen.setBFlangeCF<setVariable::CF40>();
  BDGen.setEFlangeCF<setVariable::CF40>();
  //Angle (1.6) / short on left /  -1: left side aligned
  // angle of 1.6 gets us to direct (12.8 against y for exitpipe)
  BDGen.generateDivider(Control,lKey+"BeamA",1.6,1,0);

  BellowGen.generateBellow(Control,lKey+"BellowLA",7.5);

  // small ion pump port:
  // -----------------------
  // horizontal off
  const Geometry::Vec3D OPos(0.0,2.2,0.0);
  const Geometry::Vec3D XVec(1,0,0);
  const double outerR =
    setVariable::CF63::innerRadius+setVariable::CF63::wallThick;
  const double L0(8.5 - outerR);
  const double L1(7.5 - outerR);

  SimpleTubeGen.setMat("Stainless304");
  SimpleTubeGen.setCF<setVariable::CF63>();

  SimpleTubeGen.generateBlank(Control,lKey+"IonPumpLA",0.0,12.5);
  Control.addVariable(lKey+"IonPumpLANPorts",2);
  Control.addVariable(lKey+"IonPumpLAFlangeCapThick",
		      setVariable::CF63::flangeLength);

  PItemGen.setCF<setVariable::CF40>(L0); // Port0 length
  PItemGen.setNoPlate();
  PItemGen.generatePort(Control,lKey+"IonPumpLAPort0",OPos,-XVec);

  PItemGen.setLength(L1);
  PItemGen.setNoPlate();
  PItemGen.generatePort(Control,lKey+"IonPumpLAPort1",OPos,XVec);

  // -----------
  PGen.generatePipe(Control,lKey+"PipeLA",93.3);
  BellowGen.generateBellow(Control,lKey+"BellowLB",7.5);  

  // RIGHT SIDE

  FPGen.generateFlat(Control,lKey+"FlatB",85.4);
  Control.addVariable(lKey+"FlatBXYAngle",1.6);
  DIBGen.generate(Control,lKey+"DipoleB");
  Control.addVariable(lKey+"DipoleBXStep",6.0);
  
  BellowGen.generateBellow(Control,lKey+"BellowRB",7.5);

  Control.addVariable(lKey+"BellowRBXYAngle",-1.6);
  return;
}

void
linac2SPFsegment13(FuncDataBase& Control,
		   const std::string& lKey)
  /*!
    Set the variables for segment 13
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{
  ELog::RegMethod RegA("linacVariables[F]","linac2SPFsegment13");

  setVariable::PipeGenerator PGen;
  setVariable::BPMGenerator BPMGen;
  setVariable::LinacQuadGenerator LQGen;
  setVariable::CorrectorMagGenerator CMGen;
  setVariable::LinacSexuGenerator LSGen;
  setVariable::YagUnitGenerator YagUnitGen;
  setVariable::YagScreenGenerator YagScreenGen;

  const Geometry::Vec3D startPt(-593.379,3968.258,0.0);
  const Geometry::Vec3D endPt(-622.286,4226.013,0.0);

  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"XYAngle",6.4);

  PGen.setCF<setVariable::CF40_22>();
  PGen.setMat("Stainless316L");
  PGen.setNoWindow();
  PGen.generatePipe(Control,lKey+"PipeA",69.6); // measured

  CMGen.generateMag(Control,lKey+"CMagHorA",56.0,0);

  BPMGen.setCF<setVariable::CF40_22>();
  BPMGen.generateBPM(Control,lKey+"BPMA",0.0);


  PGen.generatePipe(Control,lKey+"PipeB",78.0);
  LQGen.generateQuad(Control,lKey+"QuadA",17.1);
  LSGen.generateSexu(Control,lKey+"SexuA",39.0);
  LQGen.generateQuad(Control,lKey+"QuadB",60.9);

  YagUnitGen.generateYagUnit(Control,lKey+"YagUnit");
  YagScreenGen.generateScreen(Control,lKey+"YagScreen",1);   // closed
  Control.addVariable(lKey+"YagScreenYAngle",-90.0);

  PGen.generatePipe(Control,lKey+"PipeC",69.6);
  CMGen.generateMag(Control,lKey+"CMagVerC",12.5,1);
  return;
}


void
Segment14(FuncDataBase& Control,
		   const std::string& lKey)
  /*!
    Set the variables for the main walls
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{
  ELog::RegMethod RegA("linacVariables[F]","Segment14");

  setVariable::BellowGenerator BellowGen;
  setVariable::PipeGenerator PGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::CylGateValveGenerator CGateGen;

  const Geometry::Vec3D startPt(-622.286,4226.013,0.0);
  const Geometry::Vec3D endPt(-637.608,4507.2590,0.0);
  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"XYAngle",3.1183);

  BellowGen.setCF<setVariable::CF40_22>();
  BellowGen.setMat("Stainless304L", "Stainless304L%Void%3.0");
  BellowGen.generateBellow(Control,lKey+"BellowA",8.82); // measured yStep, length
  PGen.setCF<setVariable::CF40_22>();
  PGen.setMat("Stainless316L");
  PGen.setNoWindow();
  PGen.generatePipe(Control,lKey+"PipeA",82.5); // measured

  setVariable::DipoleDIBMagGenerator DIBGen;
  DIBGen.generate(Control,lKey+"DM1");

  PGen.setMat("Stainless316L","Stainless304L");
  PGen.generatePipe(Control,lKey+"PipeB",94.4); // measured

  PGen.setMat("Stainless316L","Stainless316L");
  PGen.generatePipe(Control,lKey+"PipeC",82.5); // measured

  DIBGen.generate(Control,lKey+"DM2");

  CGateGen.generateGate(Control,lKey+"GateA",0);  // length 7.3 cm checked
  Control.addVariable(lKey+"GateAWallThick",0.3);
  Control.addVariable(lKey+"GateAPortThick",0.1);
  Control.addVariable(lKey+"GateAYAngle",-90.0);
  // email from Karl Åhnberg, 2 Jun 2020
  Control.addVariable(lKey+"GateAWallMat","Stainless316L");
  Control.addVariable(lKey+"GateABladeMat","Stainless316L"); // guess

  BellowGen.generateBellow(Control,lKey+"BellowB",7.44); // measured


  return;
}

void
Segment15(FuncDataBase& Control,
		   const std::string& lKey)
  /*!
    Set the variables for TDC segment 15
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{
  ELog::RegMethod RegA("linacVariables[F]","Segment15");

  setVariable::PipeGenerator PGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::YagUnitGenerator YagUnitGen;
  setVariable::YagScreenGenerator YagScreenGen;

  const Geometry::Vec3D startPt(-637.608,4507.259,0.0);
  const Geometry::Vec3D endPt(-637.608,4730.259,0.0);
  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);
  ELog::EM << "YStep increased by 10 cm to avoid cutting segment 14"
  << ELog::endCrit;
  Control.addVariable(lKey+"XYAngle",0.0);

  PGen.setCF<setVariable::CF40_22>();
  PGen.setMat("Stainless316L","Stainless304L");
  PGen.setNoWindow();
  PGen.generatePipe(Control,lKey+"PipeA",22.0); // measured

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



  YagUnitGen.setCF<CF63>();
  YagUnitGen.generateYagUnit(Control,lKey+"YagUnit");

  YagScreenGen.generateScreen(Control,lKey+"YagScreen",1);   // closed
  Control.addVariable(lKey+"YagScreenYAngle",-90.0);

  PGen.generatePipe(Control,lKey+"PipeB",167.0);

  return;
}

void
Segment16(FuncDataBase& Control,
		   const std::string& lKey)
  /*!
    Set the variables for TDC segment 16
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{
  ELog::RegMethod RegA("linacVariables[F]","Segment16");

  setVariable::BellowGenerator BellowGen;
  setVariable::BPMGenerator BPMGen;

  setVariable::PipeGenerator PGen;
  setVariable::LinacQuadGenerator LQGen;
  PGen.setNoWindow();
  setVariable::CorrectorMagGenerator CMGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::GateValveGenerator GateGen;
  setVariable::YagScreenGenerator YagGen;

  const Geometry::Vec3D startPt(-637.608,4730.259,0.0);
  const Geometry::Vec3D endPt(-637.608,4983.291,0.0);
  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"XYAngle",0.0);

  BellowGen.setCF<setVariable::CF40_22>();
  BellowGen.setMat("Stainless304L", "Stainless304L%Void%3.0");
  BellowGen.generateBellow(Control,lKey+"BellowA",7.5);

  BPMGen.setCF<setVariable::CF40_22>();
  BPMGen.generateBPM(Control,lKey+"BPM",0.0);

  const double pipeALength(34.0);
  PGen.setCF<setVariable::CF40_22>();
  PGen.setMat("Stainless316L","Stainless304L");
  PGen.generatePipe(Control,lKey+"PipeA",34.0); // measured
  Control.addVariable(lKey+"PipeARadius",0.4); // inner radius - measured
  Control.addVariable(lKey+"PipeAFeThick",0.1); // wall thick - measured

  // QG (QH) type quadrupole magnet
  LQGen.setRadius(0.56, 2.31); // 0.56 - measured
  LQGen.generateQuad(Control,lKey+"Quad",pipeALength/2.0);

  Control.addVariable(lKey+"QuadLength",18.7); // measured - inner box lengh
  // measured - inner box half width/height
  Control.addVariable(lKey+"QuadYokeOuter",9.5);
  // adjusted so that nose is 1 cm thick as in the STEP file
  Control.addVariable(lKey+"QuadPolePitch",26.0);

  PGen.setCF<setVariable::CF40_22>();
  PGen.generatePipe(Control,lKey+"PipeB",40.0); // measured

  CMGen.generateMag(Control,lKey+"CMagH",10.0,0); // measured
  CMGen.generateMag(Control,lKey+"CMagV",28.0,1); // measured

  BellowGen.generateBellow(Control,lKey+"BellowB",7.5); // measured

  setIonPump2Port(Control,lKey+"IonPump");
  Control.addVariable(lKey+"IonPumpYAngle",90.0);

  PGen.generatePipe(Control,lKey+"PipeC",126.0); // measured

  return;
}

void
Segment17(FuncDataBase& Control,
		   const std::string& lKey)
  /*!
    Set the variables for TDC segment 17
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{
  ELog::RegMethod RegA("linacVariables[F]","Segment17");
  setVariable::PipeGenerator PGen;
  setVariable::BellowGenerator BellowGen;

  const Geometry::Vec3D startPt(-637.608,4983.291,0.0);
  const Geometry::Vec3D endPt(-637.608,5780.261,0.0);
  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);

  ELog::EM << "### TDCsegmen17 cad file is missing => dimensions are dummy"
	   << ELog::endWarn;

  PGen.setCF<setVariable::CF40_22>();
  PGen.setMat("Stainless316L","Stainless304L");
  PGen.setNoWindow();
  PGen.generatePipe(Control,lKey+"PipeA",100.0); // guess

  BellowGen.setCF<setVariable::CF40_22>();
  BellowGen.generateBellow(Control,lKey+"BellowA",7.5); // guess

  const std::string pumpName=lKey+"IonPump";
  setIonPump2Port(Control,pumpName);
  Control.addVariable(lKey+"IonPumpYAngle",90.0);
  // Control.addVariable(pumpName+"Port1Length",9.5); // guess
  //  Control.addVariable(pumpName+"Port2Length",3.2); // guess

  PGen.generatePipe(Control,lKey+"PipeB",100.0); // guess

  return;
}

void
Segment18(FuncDataBase& Control,
	     const std::string& lKey)
  /*!
    Set the variables for TDC segment 18
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{
  ELog::RegMethod RegA("linacVariables[F]","Segment18");

  setVariable::BellowGenerator BellowGen;
  setVariable::BPMGenerator BPMGen;

  setVariable::PipeGenerator PGen;
  PGen.setNoWindow();
  setVariable::LinacQuadGenerator LQGen;
  setVariable::CorrectorMagGenerator CMGen;

  // coordinates form drawing
  const Geometry::Vec3D startPt(-637.608,5780.261,0.0);
  const Geometry::Vec3D endPt(-637.608,5994.561,0.0);
  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);

  BellowGen.setCF<setVariable::CF40_22>();
  BellowGen.setMat("Stainless304L", "Stainless304L%Void%3.0");
  BellowGen.generateBellow(Control,lKey+"BellowA",7.5); // measured

  setIonPump2Port(Control, lKey+"IonPump");
  Control.addVariable(lKey+"IonPumpYAngle",90.0);

  BellowGen.generateBellow(Control,lKey+"BellowB",7.5); // measured

  BPMGen.setCF<setVariable::CF40_22>();
  BPMGen.generateBPM(Control,lKey+"BPM",0.0);

  /// Quad and PipeA
  const double pipeALength(34.0); // measured
  PGen.setCF<setVariable::CF40_22>();
  PGen.setMat("Stainless316L","Stainless304L");
  PGen.generatePipe(Control,lKey+"PipeA",34.0); //
  Control.addVariable(lKey+"PipeARadius",0.4); // inner radius -
  Control.addVariable(lKey+"PipeAFeThick",0.1); // wall thick -

  // QG (QH) type quadrupole magnet
  LQGen.setRadius(0.56, 2.31); // 0.56 -> measured (QH)
  LQGen.generateQuad(Control,lKey+"Quad",pipeALength/2.0);

  Control.addVariable(lKey+"QuadLength",18.7); //  - inner box lengh
  //  - inner box half width/height
  Control.addVariable(lKey+"QuadYokeOuter",9.5);
  // adjusted so that nose is 1 cm thick as in the STEP file
  Control.addVariable(lKey+"QuadPolePitch",26.0);

  PGen.setCF<setVariable::CF40_22>();
  PGen.generatePipe(Control,lKey+"PipeB",127.3); //

  CMGen.generateMag(Control,lKey+"CMagH",10.0,0); //
  CMGen.generateMag(Control,lKey+"CMagV",28.0,1); //

  return;
}

void
Segment19(FuncDataBase& Control,
		   const std::string& lKey)
  /*!
    Set the variables for TDC segment 19
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{
  ELog::RegMethod RegA("linacVariables[F]","Segment19");

  setVariable::BellowGenerator BellowGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::GateValveGenerator RGateGen;
  setVariable::CylGateValveGenerator CGateGen;

  const Geometry::Vec3D startPt(-637.608,5994.561,0.0);
  const Geometry::Vec3D endPt(-637.608,6045.428,0.0);
  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);

  const Geometry::Vec3D OPos(0,0.0,0);
  const Geometry::Vec3D XVec(1,0,0);

  // Bellow
  BellowGen.setCF<setVariable::CF40_22>();
  BellowGen.setMat("Stainless304L", "Stainless304L%Void%3.0");
  BellowGen.generateBellow(Control,lKey+"BellowA",7.5); // measured

  // Vacuum gauge
  std::string name=lKey+"Gauge";
  SimpleTubeGen.setCF<CF40_22>();
  SimpleTubeGen.generateTube(Control,name,0.0,12.6); // measured

  Control.addVariable(name+"NPorts",1);
  PItemGen.setCF<setVariable::CF40_22>(5.1); // measured
  PItemGen.setPlate(setVariable::CF40_22::flangeLength, "Stainless304L");
  PItemGen.generatePort(Control,name+"Port0",OPos,XVec);

  // Fast closing valve
  RGateGen.setLength(3.5-2.0*setVariable::CF40_22::flangeLength);
  RGateGen.setCubeCF<setVariable::CF40_22>();
  RGateGen.generateValve(Control,lKey+"GateA",0.0,0);
  // BladeMat is guess as VAT (the manufacturer) does not want to give more details
  // (email from Marek 2020-06-17)
  Control.addVariable(lKey+"GateABladeMat","Stainless304");
  Control.addVariable(lKey+"GateABladeThick",0.5); // guess (based on the gap measured)
  Control.addVariable(lKey+"GateAWallMat","Stainless304"); // email from Karl Åhnberg, 2 Jun 2020

  setIonPump1Port(Control,lKey+"IonPump");

  CGateGen.generateGate(Control,lKey+"GateB",0);
  Control.addVariable(lKey+"GateBWallThick",0.3);
  Control.addVariable(lKey+"GateBPortThick",0.1);
  Control.addVariable(lKey+"GateBYAngle",180.0);
  Control.addVariable(lKey+"GateBWallMat","Stainless316L"); // email from Karl Åhnberg, 2 Jun 2020
  // BladeMat is guess as VAT (the manufacturer) does not want to give more details
  // (email from Marek 2020-06-17)
  Control.addVariable(lKey+"GateBBladeMat","Stainless316L");

  BellowGen.generateBellow(Control,lKey+"BellowB",7.5);

  return;
}

void
Segment20(FuncDataBase& Control,
		   const std::string& lKey)
  /*!
    Set the variables for TDC segment 20
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{
  ELog::RegMethod RegA("linacVariables[F]","Segment20");

  setVariable::PipeGenerator PGen;
  PGen.setNoWindow();
  PGen.setCF<setVariable::CF40_22>();
  PGen.setMat("Stainless304L","Stainless304L");
  setVariable::TWCavityGenerator TDCGen;

  const Geometry::Vec3D startPt(-637.608,6045.428,0.0);
  const Geometry::Vec3D endPt(-637.608,6358.791,0.0);
  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);

  const double flangeLength(3.7);  // measured
  PGen.generatePipe(Control,lKey+"PipeA",flangeLength);
  Control.addVariable(lKey+"PipeARadius",1.16); // inner radius -
  Control.addVariable(lKey+"PipeAFeThick",0.2); // wall thick -

  TDCGen.generate(Control,lKey+"Cavity");

  PGen.generatePipe(Control,lKey+"PipeB",flangeLength);
  Control.addParse<double>(lKey+"PipeBRadius",lKey+"PipeARadius");
  Control.addParse<double>(lKey+"PipeBFeThick",lKey+"PipeAFeThick");
  return;
}

void
Segment21(FuncDataBase& Control,
		   const std::string& lKey)
  /*!
    Set the variables for TDC segment 21
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{
  ELog::RegMethod RegA("linacVariables[F]","Segment21");

  setVariable::BellowGenerator BellowGen;
  setVariable::BPMGenerator BPMGen;

  setVariable::PipeGenerator PGen;
  PGen.setNoWindow();

  setVariable::LinacQuadGenerator LQGen;
  setVariable::CorrectorMagGenerator CMGen;
  setVariable::YagUnitGenerator YagUnitGen;
  setVariable::YagScreenGenerator YagGen;

  const Geometry::Vec3D startPt(-637.608,6358.791,0.0);
  const Geometry::Vec3D endPt(-637.608,6495.428,0.0);
  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"XYAngle",0.0);

  BellowGen.setCF<setVariable::CF40_22>();
  BellowGen.setMat("Stainless304L", "Stainless304L%Void%3.0");
  BellowGen.setPipe(1.3, 0.2, 1.0, 1.0);  // measured
  BellowGen.generateBellow(Control,lKey+"BellowA",7.5); // OK


  BPMGen.setCF<setVariable::CF40_22>();
  BPMGen.generateBPM(Control,lKey+"BPM",0.0);
  Control.addVariable(lKey+"BPMRadius", 1.3);

  const double pipeALength(34.0);
  PGen.setCF<setVariable::CF40_22>();
  PGen.setMat("Stainless316L","Stainless304L");
  PGen.generatePipe(Control,lKey+"PipeA",34.0); // OK
  Control.addVariable(lKey+"PipeARadius",0.4); // inner radius - OK
  Control.addVariable(lKey+"PipeAFeThick",0.1); // wall thick - measured

  // QG (QH) type quadrupole magnet
  LQGen.setRadius(0.56, 2.31);
  LQGen.generateQuad(Control,lKey+"Quad",pipeALength/2.0);

  Control.addVariable(lKey+"QuadLength",18.7); // inner box lengh
  // inner box half width/height
  Control.addVariable(lKey+"QuadYokeOuter",9.5);
  // adjusted so that nose is 1 cm thick as in the STEP file
  Control.addVariable(lKey+"QuadPolePitch",26.0);

  YagUnitGen.generateYagUnit(Control,lKey+"YagUnit");
  Control.addVariable(lKey+"YagUnitMainMat","Stainless304L");
  Control.addVariable(lKey+"YagUnitPortRadius",1.7); // measured
  Control.addVariable(lKey+"YagUnitPortThick",0.2);  // measured

  YagGen.generateScreen(Control,lKey+"YagScreen",0);
  Control.addVariable(lKey+"YagScreenYAngle",-90.0);

  PGen.setCF<setVariable::CF40_22>();
  // measured 45.7, adjusted to have correct length
  PGen.generatePipe(Control,lKey+"PipeB",45.437);

  CMGen.generateMag(Control,lKey+"CMagH",10.0,0);
  CMGen.generateMag(Control,lKey+"CMagV",28.0,1);

  BellowGen.generateBellow(Control,lKey+"BellowB",7.5);

  return;
}

void
Segment22(FuncDataBase& Control,
		   const std::string& lKey)
  /*!
    Set the variables for TDC segment 22
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{
  ELog::RegMethod RegA("linacVariables[F]","Segment22");

  Segment20(Control, lKey);

  const Geometry::Vec3D startPt(-637.608,6495.428,0.0);
  const Geometry::Vec3D endPt(-637.608,6808.791,0.0);
  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);

  return;
}

void
Segment23(FuncDataBase& Control,
		   const std::string& lKey)
  /*!
    Set the variables for TDC segment 23
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{
  ELog::RegMethod RegA("linacVariables[F]","Segment23");

  setVariable::BellowGenerator BellowGen;
  setVariable::BPMGenerator BPMGen;

  setVariable::PipeGenerator PGen;
  PGen.setNoWindow();

  setVariable::LinacQuadGenerator LQGen;
  setVariable::CorrectorMagGenerator CMGen;
  setVariable::YagUnitGenerator YagUnitGen;
  setVariable::YagScreenGenerator YagGen;
  setVariable::CylGateValveGenerator CGateGen;

  const Geometry::Vec3D startPt(-637.608, 6808.791, 0.0);
  const Geometry::Vec3D endPt(-637.608, 6960.961, 0.0);

  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"XYAngle",0.0);

  BellowGen.setCF<setVariable::CF40_22>();
  BellowGen.setMat("Stainless304L", "Stainless304L%Void%3.0");
  BellowGen.setPipe(1.3, 0.2, 1.0, 1.0);
  BellowGen.generateBellow(Control,lKey+"BellowA",7.5); // OK

  const double pipeALength(34.0); // OK
  PGen.setCF<setVariable::CF40_22>();
  PGen.setMat("Stainless316L","Stainless304L");
  PGen.generatePipe(Control,lKey+"PipeA",pipeALength);
  Control.addVariable(lKey+"PipeARadius",0.4); // inner radius
  Control.addVariable(lKey+"PipeAFeThick",0.1); // wall thick

  // QG (QH) type quadrupole magnet
  LQGen.setRadius(0.56, 2.31);
  LQGen.generateQuad(Control,lKey+"Quad",pipeALength/2.0);
  Control.addVariable(lKey+"QuadLength",18.7); // inner box lengh
  // inner box half width/height
  Control.addVariable(lKey+"QuadYokeOuter",9.5);
  // adjusted so that nose is 1 cm thick as in the STEP file
  Control.addVariable(lKey+"QuadPolePitch",26.0);

  BPMGen.setCF<setVariable::CF40_22>();
  BPMGen.generateBPM(Control,lKey+"BPM",0.0);
  Control.addVariable(lKey+"BPMRadius", 1.3);

  BellowGen.generateBellow(Control,lKey+"BellowB",7.5); // OK

  const double pipeBLength(40.0); // OK
  PGen.setCF<setVariable::CF40_22>();
  PGen.generatePipe(Control,lKey+"PipeB",pipeBLength);

  CMGen.generateMag(Control,lKey+"CMagH",10.0,0);
  CMGen.generateMag(Control,lKey+"CMagV",28.0,1);

  YagUnitGen.generateYagUnit(Control,lKey+"YagUnit");
  Control.addVariable(lKey+"YagUnitMainMat","Stainless304L");
  Control.addVariable(lKey+"YagUnitPortRadius",1.7);
  Control.addVariable(lKey+"YagUnitPortThick",0.2);
  Control.addVariable(lKey+"YagUnitFrontLength",13.0);
  Control.addVariable(lKey+"YagUnitBackLength",7.0);
  Control.addVariable(lKey+"YagUnitDepth",13.0); // measured
  Control.addVariable(lKey+"YagUnitHeight",5.9); // measured
  Control.addVariable(lKey+"YagUnitViewZStep",-3.2); // guess
  Control.addVariable(lKey+"YagUnitYAngle",90);
  YagGen.generateScreen(Control,lKey+"YagScreen",0);
  Control.addVariable(lKey+"YagScreenYAngle",-90.0);

  PGen.setCF<setVariable::CF40_22>();
  PGen.generatePipe(Control,lKey+"PipeC",6.5); // OK

  // gate length is 7.2
  CGateGen.generateGate(Control,lKey+"Gate",0);
  Control.addVariable(lKey+"GateWallThick",0.3);
  Control.addVariable(lKey+"GatePortThick",0.1);
  Control.addVariable(lKey+"GateYAngle",-90.0);
  Control.addVariable(lKey+"GateWallMat","Stainless316L"); // email from Karl Åhnberg, 2 Jun 2020
  Control.addVariable(lKey+"GateBladeMat","Stainless316L"); // guess

  BellowGen.generateBellow(Control,lKey+"BellowC",7.5);

  return;
}

void
Segment24(FuncDataBase& Control,
		   const std::string& lKey)
  /*!
    Set the variables for TDC segment 24
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{
  ELog::RegMethod RegA("linacVariables[F]","Segment24");

  setVariable::BellowGenerator BellowGen;
  setVariable::BPMGenerator BPMGen;

  setVariable::PipeGenerator PGen;
  PGen.setNoWindow();

  setVariable::LinacQuadGenerator LQGen;
  setVariable::CorrectorMagGenerator CMGen;
  setVariable::YagUnitGenerator YagUnitGen;
  setVariable::YagScreenGenerator YagGen;
  setVariable::CylGateValveGenerator CGateGen;

  const Geometry::Vec3D startPt(-637.608, 6960.961, 0.0);
  const Geometry::Vec3D endPt(-637.608, 7406.261, 0.0);

  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"XYAngle",0.0);

  PGen.setCF<setVariable::CF40_22>();
  PGen.setMat("Stainless316L","Stainless304L");
  PGen.generatePipe(Control,lKey+"PipeA",325.8); // measured

  setIonPump2Port(Control,lKey+"IonPump");
  Control.addVariable(lKey+"IonPumpYAngle",90.0);
  Control.addVariable(lKey+"IonPumpPort0Length",5.0); // measured
  Control.addVariable(lKey+"IonPumpPort1Length",3.8); // measured

  BellowGen.setCF<setVariable::CF40_22>();
  BellowGen.setMat("Stainless304L", "Stainless304L%Void%3.0");
  BellowGen.setPipe(1.3, 0.2, 1.0, 1.0);
  BellowGen.generateBellow(Control,lKey+"Bellow",7.5);

  const double pipeBLength(40.0);
  PGen.setCF<setVariable::CF40_22>();
  PGen.generatePipe(Control,lKey+"PipeB",pipeBLength);

  CMGen.generateMag(Control,lKey+"CMagH",10.0,1);
  CMGen.generateMag(Control,lKey+"CMagV",28.0,0);

  BPMGen.setCF<setVariable::CF40_22>();
  BPMGen.generateBPM(Control,lKey+"BPM",0.0);
  Control.addVariable(lKey+"BPMRadius", 1.3);

  const double pipeCLength(34.0);
  PGen.setCF<setVariable::CF40_22>();
  PGen.generatePipe(Control,lKey+"PipeC",pipeCLength);

  // QG (QH) type quadrupole magnet
  LQGen.setRadius(0.56, 2.31);
  LQGen.generateQuad(Control,lKey+"Quad",pipeCLength/2.0);
  Control.addVariable(lKey+"QuadLength",18.7); // inner box lengh
  // inner box half width/height
  Control.addVariable(lKey+"QuadYokeOuter",9.5);
  // adjusted so that nose is 1 cm thick as in the STEP file
  Control.addVariable(lKey+"QuadPolePitch",26.0);

  return;
}

void
Segment25(FuncDataBase& Control,
	  const std::string& lKey)
  /*!
    Set the variables for TDC segment 25
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{

  ELog::RegMethod RegA("linacVariables[F]","Segment25");

  setVariable::BellowGenerator BellowGen;
  setVariable::PipeGenerator PGen;
  setVariable::TriPipeGenerator TPGen;
  setVariable::DipoleDIBMagGenerator DIBGen;
  setVariable::SixPortGenerator SPortGen;
  setVariable::YagScreenGenerator YagScreenGen;
  setVariable::YagUnitGenerator YagUnitGen;

 
  const Geometry::Vec3D startPt(-637.608,7406.261,0.0);

  const Geometry::Vec3D endPtA(-637.608,8173.261,0.0);
  const Geometry::Vec3D endPtB(-637.608,8180.263,-37.887);
  const Geometry::Vec3D endPtC(-637.608,8169.632,-73.976);

  // const Geometry::Vec3D startPt(-637.608,7618.484,0.0);
  // const Geometry::Vec3D endPtA(-637.608,7618.384,0.0);
  // const Geometry::Vec3D endPtB(-637.608,7612.436,-8.214);
  // const Geometry::Vec3D endPtC(-637.608,7607.463,-15.805);

  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"OffsetA",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"OffsetB",startPt+linacVar::zeroOffset);

  Control.addVariable(lKey+"EndOffset",endPtA+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffsetA",endPtB+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffsetB",endPtC+linacVar::zeroOffset);

  Control.addVariable(lKey+"BackLink","Flat");
  Control.addVariable(lKey+"BackLinkA","Mid");
  Control.addVariable(lKey+"BackLinkB","Lower");

  BellowGen.setCF<setVariable::CF40>();

  BellowGen.generateBellow(Control,lKey+"BellowA",7.5);

  const double dipoleAngle(0.8);
  const double magAngle(0.8);
  const double multiAngle(magAngle+dipoleAngle);
  

  const double startWidth(2.33/2.0);
  const double endWidth(6.70/2.0);
  TPGen.setBFlangeCF<CF100>();
  TPGen.setXYWindow(startWidth,startWidth,endWidth,endWidth);
  TPGen.generateTri(Control,lKey+"TriPipeA");
  Control.addVariable(lKey+"TriPipeAYAngle",90);
  Control.addVariable(lKey+"TriPipeAXAngle",-dipoleAngle);

  DIBGen.generate(Control,lKey+"DipoleA");

  PGen.setCF<CF100>();
  PGen.setBFlangeCF<CF150>();
  PGen.setNoWindow();
  PGen.generatePipe(Control,lKey+"PipeB",16.15);
  Control.addVariable(lKey+"PipeBPreYAngle",-90);
  Control.addVariable(lKey+"PipeBXAngle",-magAngle);
  SPortGen.setCF<CF150>();
  SPortGen.generateSixPort(Control,lKey+"SixPortA");

  // multipipe
  setVariable::MultiPipeGenerator MPGen;
  MPGen.setPipe<CF40>(Geometry::Vec3D(0,0,5.0),45.0, 0.0,multiAngle);
  MPGen.setPipe<CF40>(Geometry::Vec3D(0,0,0.0),41.0, 0.0, -3.03406+multiAngle);
  MPGen.setPipe<CF40>(Geometry::Vec3D(0,0,-5.0),37.0, 0.0, -5.82659+multiAngle);
  MPGen.generateMulti(Control,lKey+"MultiPipe");

  BellowGen.generateBellow(Control,lKey+"BellowAA",7.5);
  BellowGen.generateBellow(Control,lKey+"BellowBA",7.5);
  BellowGen.generateBellow(Control,lKey+"BellowCA",7.5);

  PGen.setCF<CF40>();
  PGen.generatePipe(Control,lKey+"PipeAA",300.0);
  PGen.generatePipe(Control,lKey+"PipeBA",321.5);
  PGen.generatePipe(Control,lKey+"PipeCA",326.5);

  BellowGen.generateBellow(Control,lKey+"BellowAB",7.5);
  BellowGen.generateBellow(Control,lKey+"BellowBB",7.5);
  BellowGen.generateBellow(Control,lKey+"BellowCB",7.5);

  YagUnitGen.generateYagUnit(Control,lKey+"YagUnitA");
  YagScreenGen.generateScreen(Control,lKey+"YagScreenA",1);   // closed
  Control.addVariable(lKey+"YagUnitAYAngle",90.0);

  YagUnitGen.generateYagUnit(Control,lKey+"YagUnitB");
  YagScreenGen.generateScreen(Control,lKey+"YagScreenB",1);   // closed
  Control.addVariable(lKey+"YagUnitBYAngle",90.0);

  PGen.setCF<CF40>();
  PGen.generatePipe(Control,lKey+"PipeAB",216.5);
  PGen.generatePipe(Control,lKey+"PipeBB",209.2);
  PGen.generatePipe(Control,lKey+"PipeCB",221.0);

  return;
}
  
void
Segment27(FuncDataBase& Control,
	  const std::string& lKey)
  /*!
    Set the variables for segment 26
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{

  ELog::RegMethod RegA("linacVariables[F]","Segment27");

  setVariable::BellowGenerator BellowGen;
  setVariable::PipeGenerator PGen;
  setVariable::YagScreenGenerator YagScreenGen;
  setVariable::YagUnitGenerator YagUnitGen;

  const Geometry::Vec3D startPtA(-637.608,8173.261,0.0);
  const Geometry::Vec3D startPtB(-637.608,8180.263,-37.887);
  const Geometry::Vec3D startPtC(-637.608,8169.632,-73.976);

  const Geometry::Vec3D endPtA(-637.608,8458.411,0.0);
  const Geometry::Vec3D endPtB(-637.608,8458.379,-52.649);
  const Geometry::Vec3D endPtC(-637.608,8442.393,-101.956);

  Control.addVariable(lKey+"OffsetA",startPtA+linacVar::zeroOffset);
  Control.addVariable(lKey+"OffsetB",startPtB+linacVar::zeroOffset);
  Control.addVariable(lKey+"OffsetC",startPtC+linacVar::zeroOffset);
      
  Control.addVariable(lKey+"EndOffsetA",endPtA+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffsetB",endPtB+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffsetC",endPtC+linacVar::zeroOffset);

  Control.addVariable(lKey+"FrontLinkA","frontFlat");
  Control.addVariable(lKey+"BackLinkA","backFlat");
  Control.addVariable(lKey+"FrontLinkB","frontMid");
  Control.addVariable(lKey+"BackLinkB","backMid");
  Control.addVariable(lKey+"FrontLinkC","frontLower");
  Control.addVariable(lKey+"BackLinkC","backLower");



  BellowGen.setCF<setVariable::CF40>();
  PGen.setCF<CF40>();
  PGen.setNoWindow();
      
  BellowGen.generateBellow(Control,lKey+"BellowAA",16.0);
  BellowGen.generateBellow(Control,lKey+"BellowBA",16.0);
  BellowGen.generateBellow(Control,lKey+"BellowCA",16.0);
  
  Control.addVariable(lKey+"BellowAAOffset",startPtA+linacVar::zeroOffset);
  Control.addVariable(lKey+"BellowBAOffset",startPtB+linacVar::zeroOffset);
  Control.addVariable(lKey+"BellowCAOffset",startPtC+linacVar::zeroOffset);

  Control.addVariable(lKey+"BellowAAXAngle",
		      std::asin((endPtA-startPtA).unit()[2])*180.0/M_PI);
  Control.addVariable(lKey+"BellowBAXAngle",
		      std::asin((endPtB-startPtB).unit()[2])*180.0/M_PI);
  Control.addVariable(lKey+"BellowCAXAngle",
		      std::asin((endPtC-startPtC).unit()[2])*180.0/M_PI);
  
  PGen.generatePipe(Control,lKey+"PipeAA",216.95);
  PGen.generatePipe(Control,lKey+"PipeBA",210.4);
  PGen.generatePipe(Control,lKey+"PipeCA",222.0);
  
  BellowGen.generateBellow(Control,lKey+"BellowAB",16.0);
  BellowGen.generateBellow(Control,lKey+"BellowBB",16.0);
  BellowGen.generateBellow(Control,lKey+"BellowCB",16.0);

  YagUnitGen.generateYagUnit(Control,lKey+"YagUnitA");
  YagScreenGen.generateScreen(Control,lKey+"YagScreenA",1);   // closed
  Control.addVariable(lKey+"YagUnitAYAngle",90.0);

  YagUnitGen.generateYagUnit(Control,lKey+"YagUnitB");
  YagScreenGen.generateScreen(Control,lKey+"YagScreenB",1);   // closed
  Control.addVariable(lKey+"YagUnitBYAngle",90.0);

  YagUnitGen.generateYagUnit(Control,lKey+"YagUnitC");
  YagScreenGen.generateScreen(Control,lKey+"YagScreenC",1);   // closed
  Control.addVariable(lKey+"YagUnitCYAngle",90.0);
  
  BellowGen.generateBellow(Control,lKey+"BellowAC",16.0);
  BellowGen.generateBellow(Control,lKey+"BellowBC",16.0);
  BellowGen.generateBellow(Control,lKey+"BellowCC",16.0);

  
  return;
}

void
Segment28(FuncDataBase& Control,
	  const std::string& lKey)
  /*!
    Set the variables for segment 28
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{

  ELog::RegMethod RegA("linacVariables[F]","Segment28");

  setVariable::BellowGenerator BellowGen;
  setVariable::PipeGenerator PGen;

  const Geometry::Vec3D startPtA(-637.608,8458.411,0.0);
  const Geometry::Vec3D startPtB(-637.608,8458.379,-52.649);


  const Geometry::Vec3D endPtA(-637.608,9073.611,0.0);
  const Geometry::Vec3D endPtB(-637.608,9073.535,-84.888);

  Control.addVariable(lKey+"OffsetA",startPtA+linacVar::zeroOffset);
  Control.addVariable(lKey+"OffsetB",startPtB+linacVar::zeroOffset);
      
  Control.addVariable(lKey+"EndOffsetA",endPtA+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffsetB",endPtB+linacVar::zeroOffset);

  Control.addVariable(lKey+"FrontLinkA","frontFlat");
  Control.addVariable(lKey+"BackLinkA","backFlat");
  Control.addVariable(lKey+"FrontLinkB","frontMid");
  Control.addVariable(lKey+"BackLinkB","backMid");

  BellowGen.setCF<setVariable::CF40>();
  PGen.setCF<CF40>();
  PGen.setNoWindow();
      
  PGen.generatePipe(Control,lKey+"PipeAA",290.0);
  PGen.generatePipe(Control,lKey+"PipeBA",290.0);

  Control.addVariable(lKey+"PipeAAOffset",startPtA+linacVar::zeroOffset);
  Control.addVariable(lKey+"PipeBAOffset",startPtB+linacVar::zeroOffset);

  Control.addVariable(lKey+"PipeAAXAngle",
		      std::atan((endPtA-startPtA).unit()[2])*180.0/M_PI);
  Control.addVariable(lKey+"PipeBAXAngle",
		      std::atan((endPtB-startPtB).unit()[2])*180.0/M_PI);
  
  BellowGen.generateBellow(Control,lKey+"BellowAA",16.0);
  BellowGen.generateBellow(Control,lKey+"BellowBA",16.0);

  PGen.generatePipe(Control,lKey+"PipeAB",290.0);
  PGen.generatePipe(Control,lKey+"PipeBB",290.0);

  BellowGen.generateBellow(Control,lKey+"BellowAB",16.0);
  BellowGen.generateBellow(Control,lKey+"BellowBB",16.0);

  
  return;
}

void
Segment30(FuncDataBase& Control,
	  const std::string& lKey)
  /*!
    Set the variables for SPF segment 30
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{
  ELog::RegMethod RegA("linacVariables[F]","Segment30");
  setVariable::PipeGenerator PGen;
  setVariable::BellowGenerator BellowGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::CorrectorMagGenerator CMGen;

  const Geometry::Vec3D startPt(-609.286, 3969.122, 0.0);
  const Geometry::Vec3D endPt(-827.249, 4928.489, 0.0);
  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"XYAngle",
		      atan((startPt.X()-endPt.X())/(endPt.Y()-startPt.Y()))*180.0/M_PI);

  const Geometry::Vec3D OPos(0,0.0,0);
  const Geometry::Vec3D XVec(1,0,0);

  // Gauge
  std::string name=lKey+"Gauge";
  SimpleTubeGen.setCF<CF40_22>();
  SimpleTubeGen.generateTube(Control,name,0.0,12.6); // measured
  Control.addVariable(name+"YAngle", 180.0);

  Control.addVariable(name+"NPorts",1);
  PItemGen.setCF<setVariable::CF40_22>(5.1); //
  PItemGen.setPlate(setVariable::CF40_22::flangeLength, "Stainless304L");
  PItemGen.generatePort(Control,name+"Port0",OPos,XVec);

  PGen.setCF<setVariable::CF40_22>();
  PGen.setMat("Stainless316L","Stainless304L");
  PGen.setNoWindow();
  PGen.generatePipe(Control,lKey+"PipeA",436.5-0.084514221); // measured

  BellowGen.setCF<setVariable::CF40_22>();
  BellowGen.setMat("Stainless304L", "Stainless304L%Void%3.0");
  BellowGen.generateBellow(Control,lKey+"Bellow",7.5); // measured

  // IonPump
  const std::string pumpName=lKey+"IonPump";
  setIonPump2Port(Control,pumpName);
  Control.addVariable(pumpName+"Length",10.0+setVariable::CF63::flangeLength);
  const double portOffset(1.7);
  Control.addVariable(pumpName+"Port0Centre", Geometry::Vec3D(0, portOffset, 0));
  Control.addVariable(pumpName+"Port1Centre", Geometry::Vec3D(0, portOffset, 0));

  // CMagV
  const double pipeBLength(511.3); // measured
  PGen.generatePipe(Control,lKey+"PipeB",pipeBLength);
  CMGen.generateMag(Control,lKey+"CMagV",pipeBLength-12.0,1);

  return;
}

void
Segment31(FuncDataBase& Control,
		   const std::string& lKey)
  /*!
    Set the variables for SPF segment 31
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{
  ELog::RegMethod RegA("linacVariables[F]","Segment31");

  setVariable::PipeGenerator PGen;
  PGen.setNoWindow();
  setVariable::BellowGenerator BellowGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::CorrectorMagGenerator CMGen;
  setVariable::CylGateValveGenerator CGateGen;
  setVariable::BPMGenerator BPMGen;
  setVariable::LinacQuadGenerator LQGen;

  const Geometry::Vec3D startPt(-827.249, 4928.489, 0.0);
  const Geometry::Vec3D endPt(-921.651, 5344.0, 0.0);

  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"XYAngle",
		      atan((startPt.X()-endPt.X())/(endPt.Y()-startPt.Y()))*180.0/M_PI);

  // BellowA
  BellowGen.setCF<setVariable::CF40_22>();
  BellowGen.setMat("Stainless304L", "Stainless304L%Void%3.0");
  BellowGen.setPipe(1.3, 0.2, 1.0, 1.0);
  BellowGen.generateBellow(Control,lKey+"BellowA",7.5); // OK

  // IonPumpA
  setIonPump2Port(Control,lKey+"IonPumpA"); // length 16 cm checked
  Control.addVariable(lKey+"IonPumpALength",
		      11.4+setVariable::CF63::flangeLength);
  const double portOffset(1.7);
  Control.addVariable(lKey+"IonPumpAPort0Centre",
		      Geometry::Vec3D(0, portOffset, 0));
  Control.addVariable(lKey+"IonPumpAPort1Centre",
		      Geometry::Vec3D(0, portOffset, 0));
  Control.addVariable(lKey+"IonPumpAYAngle",90.0);

  CGateGen.generateGate(Control,lKey+"Gate",0);  // length 7.3 cm checked
  Control.addVariable(lKey+"GateWallThick",0.3);
  Control.addVariable(lKey+"GatePortThick",0.1);
  Control.addVariable(lKey+"GateYAngle",-90.0);
  // email from Karl Åhnberg, 2 Jun 2020
  Control.addVariable(lKey+"GateWallMat","Stainless316L"); 
  Control.addVariable(lKey+"GateBladeMat","Stainless316L"); // guess

  BellowGen.generateBellow(Control,lKey+"BellowB",7.5); // OK

  BPMGen.setCF<setVariable::CF40_22>();
  BPMGen.generateBPM(Control,lKey+"BPM",0.0);
  Control.addVariable(lKey+"BPMRadius", 1.3);

  // PipeA and Quadrupole
  const double pipeALength(42.5); // OK
  PGen.setCF<setVariable::CF40_22>();
  PGen.setMat("Stainless316L","Stainless304L");
  PGen.generatePipe(Control,lKey+"PipeA",pipeALength);
  Control.addVariable(lKey+"PipeARadius",0.4); // inner radius
  Control.addVariable(lKey+"PipeAFeThick",0.1); // wall thick

  // QF type quadrupole magnet
  LQGen.generateQuad(Control,lKey+"Quad",pipeALength/2.0);
  Control.addVariable(lKey+"QuadLength",18.7); // inner box lengh OK
  // inner box half width/height
  Control.addVariable(lKey+"QuadYokeOuter",9.5);

  // BellowC
  BellowGen.generateBellow(Control,lKey+"BellowC",7.5); // OK

  // CMagH
  const double pipeBLength(232.7); // OK
  PGen.generatePipe(Control,lKey+"PipeB",pipeBLength);
  CMGen.generateMag(Control,lKey+"CMagH",25.0,0); // 25 is approx

  // IonPumpB
  setIonPump2Port(Control,lKey+"IonPumpB");
  const double outerR =
    setVariable::CF63::innerRadius+setVariable::CF63::wallThick;
  Control.addVariable(lKey+"Port0Length",10.0-outerR); // OK
  Control.addVariable(lKey+"Port1Length",10.0-outerR); // OK
  Control.addVariable(lKey+"IonPumpBLength",11.4+setVariable::CF63::flangeLength); // approx
  Control.addVariable(lKey+"IonPumpBPort0Centre", Geometry::Vec3D(0, portOffset, 0));
  Control.addVariable(lKey+"IonPumpBPort1Centre", Geometry::Vec3D(0, portOffset, 0));


  // PipeC
  PGen.generatePipe(Control,lKey+"PipeC",55.6); // OK

  // BellowD
  BellowGen.generateBellow(Control,lKey+"BellowD",7.5);

  return;
}

void
Segment32(FuncDataBase& Control,
		   const std::string& lKey)
  /*!
    Set the variables for SPF segment 32
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{
  ELog::RegMethod RegA("linacVariables[F]","Segment32");

  setVariable::PipeGenerator PGen;
  setVariable::DipoleDIBMagGenerator DIBGen;
  setVariable::BellowGenerator BellowGen;

  const Geometry::Vec3D startPt(-921.651,5344.0,0.0);
  const Geometry::Vec3D endPt(-965.763,5607.319,0.0);

  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"XYAngle",
		      atan((startPt.X()-endPt.X())/(endPt.Y()-startPt.Y()))*180.0/M_PI);

  PGen.setCF<setVariable::CF40_22>();
  PGen.setMat("Stainless316L");
  PGen.setNoWindow();
  PGen.generatePipe(Control,lKey+"PipeA",82.5); // measured

  DIBGen.generate(Control,lKey+"DMA");

  PGen.setMat("Stainless316L","Stainless304L");
  PGen.generatePipe(Control,lKey+"PipeB",94.4); // measured

  PGen.setMat("Stainless316L","Stainless316L");
  PGen.generatePipe(Control,lKey+"PipeC",82.5); // measured

  DIBGen.generate(Control,lKey+"DMB");

  BellowGen.setCF<setVariable::CF40_22>();
  BellowGen.setMat("Stainless304L", "Stainless304L%Void%3.0");
  BellowGen.generateBellow(Control,lKey+"Bellow",7.5); // measured

  return;
}

void
Segment33(FuncDataBase& Control,
		   const std::string& lKey)
  /*!
    Set the variables for SPF segment 33
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{
  ELog::RegMethod RegA("linacVariables[F]","Segment33");

  setVariable::PipeGenerator PGen;
  setVariable::BPMGenerator BPMGen;
  setVariable::LinacQuadGenerator LQGen;
  setVariable::CorrectorMagGenerator CMGen;
  setVariable::LinacSexuGenerator LSGen;
  setVariable::YagUnitGenerator YagUnitGen;
  setVariable::YagScreenGenerator YagScreenGen;
  setVariable::BellowGenerator BellowGen;

  const Geometry::Vec3D startPt(-965.763,5607.319,0.0);
  const Geometry::Vec3D endPt(-995.514,5872.556,0.0);

  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"XYAngle",
		      atan((startPt.X()-endPt.X())/(endPt.Y()-startPt.Y()))*180.0/M_PI);

  PGen.setCF<setVariable::CF40_22>();
  PGen.setMat("Stainless316L","Stainless304L");
  PGen.setNoWindow();
  PGen.generatePipe(Control,lKey+"PipeA",67.0); // measured

  CMGen.generateMag(Control,lKey+"CMagHorA",56.0,0);

  BPMGen.setCF<setVariable::CF40_22>();
  BPMGen.generateBPM(Control,lKey+"BPMA",0.0); // 22 cm length OK
  Control.addVariable(lKey+"BPMRadius", 1.3); // ????

  PGen.generatePipe(Control,lKey+"PipeB",81.5); // measured: 81.6, but adjusted to keep total length
  LQGen.generateQuad(Control,lKey+"QuadA",17.1);
  LSGen.generateSexu(Control,lKey+"SexuA",39.0);
  LQGen.generateQuad(Control,lKey+"QuadB",60.9);

  YagUnitGen.generateYagUnit(Control,lKey+"YagUnit"); // length 20.2 != 20
  YagScreenGen.generateScreen(Control,lKey+"YagScreen",1);   // closed
  Control.addVariable(lKey+"YagUnitYAngle",90.0);

  PGen.generatePipe(Control,lKey+"PipeC",68.7);
  CMGen.generateMag(Control,lKey+"CMagVerC",12.5,0);

  BellowGen.setCF<setVariable::CF40_22>();
  BellowGen.setMat("Stainless304L", "Stainless304L%Void%3.0");
  BellowGen.generateBellow(Control,lKey+"Bellow",7.5); // measured

  return;
}


void
Segment34(FuncDataBase& Control,
		   const std::string& lKey)
  /*!
    Set the variables for SPF segment 34
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{
  ELog::RegMethod RegA("linacVariables[F]","Segment34");

  Segment32(Control, lKey);

  const Geometry::Vec3D startPt(-995.514,5872.556,0.0);
  const Geometry::Vec3D endPt(-1010.0,6139.149,0.0);

  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"XYAngle",
		      atan((startPt.X()-endPt.X())/(endPt.Y()-startPt.Y()))*180.0/M_PI);

  return;
}

void
Segment35(FuncDataBase& Control,
		   const std::string& lKey)
  /*!
    Set the variables for SPF segment 35
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{
  ELog::RegMethod RegA("linacVariables[F]","Segment35");

  setVariable::PipeGenerator PGen;
  setVariable::LinacQuadGenerator LQGen;
  setVariable::CorrectorMagGenerator CMGen;
  setVariable::YagUnitBigGenerator YagUnitGen;
  setVariable::YagScreenGenerator YagScreenGen;
  setVariable::BellowGenerator BellowGen;
  setVariable::ButtonBPMGenerator BPMGen;

  const Geometry::Vec3D startPt(-1010.0,6139.149,0.0);
  const Geometry::Vec3D endPt(-1010.0,6310.949,0.0);

  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"XYAngle",
		      atan((startPt.X()-endPt.X())/(endPt.Y()-startPt.Y()))*180.0/M_PI);

  YagUnitGen.generateYagUnit(Control,lKey+"YagUnit");
  Control.addVariable(lKey+"YagUnitYAngle",90.0);

  YagScreenGen.generateScreen(Control,lKey+"YagScreen",1);
  Control.addVariable(lKey+"YagScreenYAngle",-90.0);
  Control.addVariable(lKey+"YagScreenZStep",-3.3);

  const double pipeALength(37.5); // measured
  PGen.setCF<setVariable::CF40_22>();
  PGen.setMat("Stainless316L","Stainless304L");
  PGen.setNoWindow();
  PGen.generatePipe(Control,lKey+"PipeA",pipeALength);

  LQGen.generateQuad(Control,lKey+"QuadA",pipeALength/2.0); // approx

  BPMGen.setCF<setVariable::CF40_22>();
  BPMGen.generate(Control,lKey+"BPM");

  PGen.generatePipe(Control,lKey+"PipeB",75.2); // measured
  LQGen.generateQuad(Control,lKey+"QuadB",19.0); // approx
  CMGen.generateMag(Control,lKey+"CMagH",45.0,1);
  CMGen.generateMag(Control,lKey+"CMagV",65.0,0);

  setIonPump2Port(Control, lKey+"MirrorChamber");
  Control.addVariable(lKey+"MirrorChamberYAngle", 180.0);
  Control.addVariable(lKey+"MirrorChamberLength", 16.0);
  const double outerR =
    setVariable::CF63::innerRadius+setVariable::CF63::wallThick;
  Control.addVariable(lKey+"MirrorChamberPort0Length", 5.9-outerR);
  Control.addVariable(lKey+"MirrorChamberPort1Length", 8.1-outerR);
  const Geometry::Vec3D pPos(0.0,3.0+2.6,0.0);
  Control.addVariable(lKey+"MirrorChamberPort0Centre", pPos);
  Control.addVariable(lKey+"MirrorChamberPort1Centre", pPos);

  PGen.generatePipe(Control,lKey+"PipeC",12.6); // measured
  Control.addVariable(lKey+"PipeCFeMat", "Stainless304L"); // PDF

  BellowGen.setCF<setVariable::CF40_22>();
  BellowGen.setMat("Stainless304L", "Stainless304L%Void%3.0");
  BellowGen.generateBellow(Control,lKey+"Bellow",7.5);

  return;
}

void
Segment36(FuncDataBase& Control,
		   const std::string& lKey)
  /*!
    Set the variables for SPF segment 36
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{
  ELog::RegMethod RegA("linacVariables[F]","Segment36");

  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::PipeGenerator PGen;
  setVariable::LinacQuadGenerator LQGen;
  setVariable::CorrectorMagGenerator CMGen;
  setVariable::BPMGenerator BPMGen;
  setVariable::EArrivalMonGenerator EArrGen;
  setVariable::CylGateValveGenerator CGateGen;

  const Geometry::Vec3D startPt(-1010.0,6310.949,0.0);
  const Geometry::Vec3D endPt(-1010.0,6729.589,0.0);

  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"XYAngle",
		      atan((startPt.X()-endPt.X())/(endPt.Y()-startPt.Y()))*180.0/M_PI);

  // Gauge
  std::string name=lKey+"Gauge";
  SimpleTubeGen.setCF<CF40_22>();
  SimpleTubeGen.generateTube(Control,name,0.0,12.6); // measured

  Control.addVariable(name+"NPorts",1);
  PItemGen.setCF<setVariable::CF40_22>(5.1); //
  PItemGen.setPlate(setVariable::CF40_22::flangeLength, "Stainless304L");
  PItemGen.generatePort(Control,name+"Port0",
			Geometry::Vec3D(0,0,0),
			Geometry::Vec3D(1,0,0));

  // Pipes
  PGen.setCF<setVariable::CF40_22>();
  PGen.setMat("Stainless316L","Stainless304L");
  PGen.setNoWindow();
  const double pipeALength(146.9); // measured
  PGen.generatePipe(Control,lKey+"PipeA",pipeALength);
  PGen.generatePipe(Control,lKey+"PipeB",98.7); // measured
  PGen.generatePipe(Control,lKey+"PipeC",30.0); // measured
  PGen.generatePipe(Control,lKey+"PipeD",33.3); // measured
  PGen.generatePipe(Control,lKey+"PipeE",35.0); // measured

  // magnet locations based on front surfaces of yokes
  // Quadrupole magnets
  LQGen.generateQuad(Control,lKey+"QuadA",18.55); // measured
  Control.addVariable(lKey+"QuadALength",18.7); // measured
  LQGen.generateQuad(Control,lKey+"QuadB",128.55); // measured
  Control.addVariable(lKey+"QuadBLength",18.7); // measured

  // Corrector magnets
  CMGen.generateMag(Control,lKey+"CMagH",42.35+1.3,0); // measured with wrong CMagH length
  // Control.addVariable(lKey+"CMagHMagInnerLength",10.3); // 11.5
  // Control.addVariable(lKey+"CMagHMagLength",14.2-1.2); //
  CMGen.generateMag(Control,lKey+"CMagV",61.05+1.1,1); // measured with wrong CMagV length

  // Beam position monitors
  BPMGen.setCF<setVariable::CF40_22>();
  BPMGen.generateBPM(Control,lKey+"BPMA",0.0);
  BPMGen.generateBPM(Control,lKey+"BPMB",0.0);

  // Beam arrival monitor
  EArrGen.setCF<setVariable::CF40_22>();
  EArrGen.generateMon(Control,lKey+"BeamArrivalMon",0.0);
  Control.addVariable(lKey+"BeamArrivalMonLength",4.75);

  // Gate
  CGateGen.generateGate(Control,lKey+"Gate",0);  // length 7.3 cm checked
  Control.addVariable(lKey+"GateWallThick",0.3);
  Control.addVariable(lKey+"GatePortThick",0.1);
  Control.addVariable(lKey+"GateYAngle",180.0);
  Control.addVariable(lKey+"GateWallMat","Stainless316L"); // email from Karl Åhnberg, 2 Jun 2020
  Control.addVariable(lKey+"GateBladeMat","Stainless316L"); // guess

  return;
}

void
Segment37(FuncDataBase& Control,
		  const std::string& lKey)
  /*!
    Set the variables for SPF segment 37
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{
  ELog::RegMethod RegA("linacVariables[F]","Segment37");

  setVariable::CeramicSepGenerator CSGen;
  setVariable::EBeamStopGenerator EBGen;
  setVariable::PipeGenerator PGen;

  const Geometry::Vec3D startPt(-1010.0,6729.589,0.0);
  const Geometry::Vec3D endPt(-1010.0,6825.849,0.0);
  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"XYAngle",
  		      atan((startPt.X()-endPt.X())/(endPt.Y()-startPt.Y()))*180.0/M_PI);

  CSGen.generateCeramicSep(Control,lKey+"CeramicA");
  CSGen.generateCeramicSep(Control,lKey+"CeramicB");

  EBGen.generateEBeamStop(Control,lKey+"BeamStop",0);
  Control.addVariable(lKey+"BeamStopWallThick",1.0); // measured
  Control.addVariable(lKey+"BeamStopLength",41.0); // measured
  Control.addVariable(lKey+"BeamStopWidth",11.0); // measured
  Control.addVariable(lKey+"BeamStopStopRadius",5.2); // measured

  PGen.setCF<setVariable::CF40_22>();
  PGen.setNoWindow();

  PGen.setMat("Stainless316L","Stainless304L");
  PGen.generatePipe(Control,lKey+"Pipe",20.26);

  return;
}

void
Segment38(FuncDataBase& Control,
		   const std::string& lKey)
  /*!
    Set the variables for SPF segment 38
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{
  ELog::RegMethod RegA("linacVariables[F]","Segment17");
  setVariable::PipeGenerator PGen;
  setVariable::BellowGenerator BellowGen;

  const Geometry::Vec3D startPt(-1010.0,6825.849,0.0);
  const Geometry::Vec3D endPt(-1010.0,7355.379,0.0);
  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"XYAngle",
  		      atan((startPt.X()-endPt.X())/(endPt.Y()-startPt.Y()))*180.0/M_PI);


  PGen.setCF<setVariable::CF40_22>();
  PGen.setMat("Aluminium","Aluminium");
  PGen.setNoWindow();
  PGen.generatePipe(Control,lKey+"PipeA",285.0); // measured
  PGen.setMat("Stainless316L","Stainless304L");
  PGen.generatePipe(Control,lKey+"PipeB",221.0); // measured


  const std::string pumpName=lKey+"IonPump";
  setIonPump2Port(Control,pumpName);
  Control.addVariable(lKey+"IonPumpYAngle",180.0);

  BellowGen.setCF<setVariable::CF40_22>();
  BellowGen.setMat("Stainless304L", "Stainless304L%Void%3.0");
  BellowGen.generateBellow(Control,lKey+"BellowA",7.5); // measured

  return;
}

void
Segment39(FuncDataBase& Control,
		   const std::string& lKey)
  /*!
    Set the variables for SPF segment 39
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{
  ELog::RegMethod RegA("linacVariables[F]","Segment17");

  ELog::EM << "SPH39: start/end of SPF38 are used to avoid geometry error" << ELog::endWarn;
  // SPF39
  // const Geometry::Vec3D startPt(-1010.0,7355.379,0.0);
  // const Geometry::Vec3D endPt(-1010.0,7449.099,0.0);

  // SPF38
  const Geometry::Vec3D startPt(-1010.0,6825.849,0.0);
  const Geometry::Vec3D endPt(-1010.0,7355.379,0.0);

  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"XYAngle",
  		      atan((startPt.X()-endPt.X())/(endPt.Y()-startPt.Y()))*180.0/M_PI);

  // Stripline BPM
  setVariable::BPMGenerator BPMGen;
  BPMGen.setCF<setVariable::CF40_22>();
  BPMGen.generateBPM(Control,lKey+"BPM",0.0);

  // Yag screen and its unit
  setVariable::YagUnitBigGenerator YagUnitGen;
  YagUnitGen.generateYagUnit(Control,lKey+"YagUnit");
  Control.addVariable(lKey+"YagUnitYAngle",90.0);

  setVariable::YagScreenGenerator YagScreenGen;
  YagScreenGen.generateScreen(Control,lKey+"YagScreen",1);
  Control.addVariable(lKey+"YagScreenYAngle",-90.0);
  Control.addVariable(lKey+"YagScreenZStep",-3.3);

  // Gate
  setVariable::CylGateValveGenerator CGateGen;
  CGateGen.generateGate(Control,lKey+"Gate",0);  //7.3?
  Control.addVariable(lKey+"GateWallThick",0.3);
  Control.addVariable(lKey+"GatePortThick",0.1);
  Control.addVariable(lKey+"GateYAngle",90.0);
  Control.addVariable(lKey+"GateWallMat","Stainless316L"); // email from Karl Åhnberg, 2 Jun 2020
  Control.addVariable(lKey+"GateBladeMat","Stainless316L"); // guess

  // Pipe
  setVariable::PipeGenerator PGen;
  PGen.setCF<setVariable::CF40_22>();
  PGen.setMat("Stainless316L","Stainless304L");
  PGen.setNoWindow();
  PGen.generatePipe(Control,lKey+"Pipe",37.0);

  // Bellow
  setVariable::BellowGenerator BellowGen;
  BellowGen.setCF<setVariable::CF40_22>();
  BellowGen.setMat("Stainless304L", "Stainless304L%Void%3.0");
  BellowGen.generateBellow(Control,lKey+"Bellow",7.5);

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

  const double extraYLen(82.7);

  Control.addVariable(wallKey+"MainLength",9880.0);
  Control.addVariable(wallKey+"LinearRCutLength",3812.0+extraYLen);
  Control.addVariable(wallKey+"LinearLTurnLength",3672.0+extraYLen);
  Control.addVariable(wallKey+"RightWallStep",145.0);
  Control.addVariable(wallKey+"SPFAngleLength",4124.0);
  Control.addVariable(wallKey+"SPFLongLength",200.0); // extra divider
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
  Control.addVariable(wallKey+"MidTYStep",3357.0+extraYLen);  // to flat of T
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

  ELog::EM<<"LINAC VARIABLES"<<ELog::endDiag;
  linacVar::wallVariables(Control,"InjectionHall");

  // Segment 1-14
  Control.addVariable("l2spfXStep",linacVar::zeroX);
  Control.addVariable("l2spfYStep",linacVar::zeroY);
  Control.addVariable("l2spfOuterLeft",80.0);
  Control.addVariable("l2spfOuterRight",140.0);
  Control.addVariable("l2spfOuterTop",100.0);

  Control.addVariable("l2spfTurnXStep",linacVar::zeroX-80.0);
  Control.addVariable("l2spfTurnYStep",linacVar::zeroY);
  Control.addVariable("l2spfTurnOuterLeft",80.0);
  Control.addVariable("l2spfTurnOuterRight",140.0);
  Control.addVariable("l2spfTurnOuterTop",100.0);

  Control.addVariable("l2spfAngleXStep",linacVar::zeroX-155.0);
  Control.addVariable("l2spfAngleYStep",linacVar::zeroY+2000.0);
  Control.addVariable("l2spfAngleOuterLeft",100.0);
  Control.addVariable("l2spfAngleOuterRight",100.0);
  Control.addVariable("l2spfAngleOuterTop",100.0);
  Control.addVariable("l2spfAngleXYAngle",12.0);

  Control.addVariable("tdcFrontXStep",-419.0+linacVar::zeroX);
  Control.addVariable("tdcFrontYStep",3152.0+linacVar::zeroY);
  Control.addVariable("tdcFrontOuterLeft",50.0);
  Control.addVariable("tdcFrontOuterRight",50.0);
  Control.addVariable("tdcFrontOuterTop",100.0);
  Control.addVariable("tdcFrontXYAngle",12.0);

  Control.addVariable("tdcMainXStep",-419.0+linacVar::zeroX);
  Control.addVariable("tdcMainYStep",3152.0+linacVar::zeroY);
  Control.addVariable("tdcMainOuterLeft",50.0);
  Control.addVariable("tdcMainOuterRight",50.0);
  Control.addVariable("tdcMainOuterTop",100.0);
  Control.addVariable("tdcMainXYAngle",12.0);

  Control.addVariable("tdcXStep",-622.286+linacVar::zeroX);
  Control.addVariable("tdcYStep",4226.013+linacVar::zeroY);
  Control.addVariable("tdcOuterLeft",70.0);
  Control.addVariable("tdcOuterRight",50.0);
  Control.addVariable("tdcOuterTop",100.0);

  // segments 25-29
  Control.addVariable("spfMidXStep",-622.286+linacVar::zeroX);
  Control.addVariable("spfMidYStep",4226.013+linacVar::zeroY);
  Control.addVariable("spfMidOuterLeft",50.0);
  Control.addVariable("spfMidOuterRight",50.0);
  Control.addVariable("spfMidOuterTop",100.0);

  
  // start/endPt of Segment30
  Control.addVariable("spfAngleXStep",-609.286+linacVar::zeroX);
  Control.addVariable("spfAngleYStep",3969.122+linacVar::zeroY);
  Control.addVariable("spfAngleOuterLeft",50.0);
  Control.addVariable("spfAngleOuterRight",100.0);
  Control.addVariable("spfAngleOuterTop",100.0);
  Control.addVariable("spfAngleXYAngle",12.8);

  // start/endPt of Segment34
  Control.addVariable("spfXStep",-995.514+linacVar::zeroX);
  Control.addVariable("spfYStep",5872.556+linacVar::zeroY);
  Control.addVariable("spfOuterLeft",50.0);
  Control.addVariable("spfOuterRight",50.0);
  Control.addVariable("spfOuterTop",100.0);

  Control.addVariable("spfLongXStep",-622.286+linacVar::zeroX);
  Control.addVariable("spfLongYStep",4226.013+linacVar::zeroY);
  Control.addVariable("spfLongOuterLeft",50.0);
  Control.addVariable("spfLongOuterRight",50.0);
  Control.addVariable("spfLongOuterTop",100.0);


  linacVar::linac2SPFsegment1(Control,"L2SPF1");
  linacVar::linac2SPFsegment2(Control,"L2SPF2");
  linacVar::linac2SPFsegment3(Control,"L2SPF3");
  linacVar::linac2SPFsegment4(Control,"L2SPF4");
  linacVar::linac2SPFsegment5(Control,"L2SPF5");
  linacVar::linac2SPFsegment6(Control,"L2SPF6");
  linacVar::linac2SPFsegment7(Control,"L2SPF7");
  linacVar::linac2SPFsegment8(Control,"L2SPF8");
  linacVar::linac2SPFsegment9(Control,"L2SPF9");
  linacVar::linac2SPFsegment10(Control,"L2SPF10");
  linacVar::linac2SPFsegment11(Control,"L2SPF11");
  linacVar::linac2SPFsegment12(Control,"L2SPF12");
  linacVar::linac2SPFsegment13(Control,"L2SPF13");

  linacVar::Segment14(Control,"TDC14");
  linacVar::Segment15(Control,"TDC15");
  linacVar::Segment16(Control,"TDC16");
  linacVar::Segment17(Control,"TDC17");
  linacVar::Segment18(Control,"TDC18");
  linacVar::Segment19(Control,"TDC19");
  linacVar::Segment20(Control,"TDC20");
  linacVar::Segment21(Control,"TDC21");
  linacVar::Segment22(Control,"TDC22");
  linacVar::Segment23(Control,"TDC23");
  linacVar::Segment24(Control,"TDC24");
  linacVar::Segment25(Control,"TDC25");

  linacVar::Segment27(Control,"TDC27");
  linacVar::Segment28(Control,"TDC28");
  linacVar::Segment30(Control,"SPF30");
  linacVar::Segment31(Control,"SPF31");
  linacVar::Segment32(Control,"SPF32");
  linacVar::Segment33(Control,"SPF33");
  linacVar::Segment34(Control,"SPF34");
  linacVar::Segment35(Control,"SPF35");
  linacVar::Segment36(Control,"SPF36");
  linacVar::Segment37(Control,"SPF37");
  linacVar::Segment38(Control,"SPF38");
  linacVar::Segment39(Control,"SPF39");

  return;
}

}  // NAMESPACE setVariable
