/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   linac/linacVariables.cxx
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
#include <cassert>
#include <numeric>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "magnetVar.h"

#include "CFFlanges.h"
#include "PipeGenerator.h"
#include "SplitPipeGenerator.h"
#include "BellowGenerator.h"
#include "FlangePlateGenerator.h"

#include "CorrectorMagGenerator.h"
#include "LinacQuadGenerator.h"
#include "LinacSexuGenerator.h"
#include "PortTubeGenerator.h"
#include "JawFlangeGenerator.h"
#include "PipeTubeGenerator.h"
#include "PortItemGenerator.h"
#include "StriplineBPMGenerator.h"
#include "CylGateValveGenerator.h"
#include "GateValveGenerator.h"
#include "DipoleDIBMagGenerator.h"
#include "EArrivalMonGenerator.h"
#include "YagScreenGenerator.h"
#include "YagUnitGenerator.h"
#include "YagUnitBigGenerator.h"
#include "FlatPipeGenerator.h"
#include "TriPipeGenerator.h"
#include "TriGroupGenerator.h"
#include "BeamDividerGenerator.h"
#include "ScrapperGenerator.h"
#include "SixPortGenerator.h"
#include "CeramicGapGenerator.h"
#include "EBeamStopGenerator.h"
#include "TWCavityGenerator.h"
#include "UndVacGenerator.h"
#include "FMUndulatorGenerator.h"
#include "subPipeUnit.h"
#include "MultiPipeGenerator.h"
#include "ButtonBPMGenerator.h"
#include "CurveMagGenerator.h"
#include "CleaningMagnetGenerator.h"
#include "CrossWayGenerator.h"
#include "PrismaChamberGenerator.h"
#include "IonPTubeGenerator.h"
#include "GaugeGenerator.h"
#include "LBeamStopGenerator.h"
#include "LocalShieldingGenerator.h"
#include "SPFCameraShieldGenerator.h"

namespace setVariable
{

namespace linacVar
{
  void wallVariables(FuncDataBase&,const std::string&);
  void setIonPump2Port(FuncDataBase&,const std::string&,const double);
  void setIonPump1Port(FuncDataBase&,const std::string&);
  void setGauge34(FuncDataBase&,const std::string&);
  void setGauge37(FuncDataBase&,const std::string&);


  //  void setIonPump3Port(FuncDataBase&,const std::string&);
  void setIonPump3OffsetPort(FuncDataBase&,const std::string&);
  void setPrismaChamber(FuncDataBase&,const std::string&);
  void setSlitTube(FuncDataBase&,const std::string&,const double);
  void setRecGateValve(FuncDataBase&,const std::string&,
		       const bool);
  void setCylGateValve(FuncDataBase&,const std::string&,
		       const double,const bool);

  void Segment1(FuncDataBase&,const std::string&);
  void Segment2(FuncDataBase&,const std::string&);
  void Segment3(FuncDataBase&,const std::string&);
  void Segment4(FuncDataBase&,const std::string&);
  void Segment5(FuncDataBase&,const std::string&);
  void Segment6(FuncDataBase&,const std::string&);
  void Segment7(FuncDataBase&,const std::string&);
  void segment8(FuncDataBase&,const std::string&);
  void Segment9(FuncDataBase&,const std::string&);
  void Segment10(FuncDataBase&,const std::string&);
  void Segment11(FuncDataBase&,const std::string&);
  void Segment12(FuncDataBase&,const std::string&);
  void Segment13(FuncDataBase&,const std::string&);

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
  void Segment26(FuncDataBase&,const std::string&);
  void Segment27(FuncDataBase&,const std::string&);
  void Segment28(FuncDataBase&,const std::string&);
  void Segment29(FuncDataBase&,const std::string&);
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
  void Segment40(FuncDataBase&,const std::string&);
  void Segment41(FuncDataBase&,const std::string&);
  void Segment42(FuncDataBase&,const std::string&);
  void Segment43(FuncDataBase&,const std::string&);
  void Segment44(FuncDataBase&,const std::string&);
  void Segment45(FuncDataBase&,const std::string&);
  void Segment46(FuncDataBase&,const std::string&);
  void Segment47(FuncDataBase&,const std::string&);
  void Segment48(FuncDataBase&,const std::string&);
  void Segment49(FuncDataBase&,const std::string&);

  // zeroX adjusted so that distance to KG wall is 1 m
  // const double zeroX(275.0);   // coordiated offset to master
  // const double zeroY(481.0);    // drawing README.pdf
  // const Geometry::Vec3D zeroOffset(zeroX,zeroY,0.0);

void
setIonPump1Port(FuncDataBase& Control,
		const std::string& name,
		const double angle=0.0)
/*!
  Set the 1 port ion pump variables [GaugeTube]
  \param Control :: DataBase to use
  \param name :: name prefix
 */
{
  setVariable::GaugeGenerator GGen;

  GGen.setCF<CF37_TDC>();
  GGen.setLength(12.6);
  GGen.setSidePortCF<setVariable::CF37_TDC>(6.95);
  GGen.setPlateThick(setVariable::CF37_TDC::flangeLength,"Stainless304L");

  GGen.generateGauge(Control,name,0.0,angle);

  return;
}

void
setIonPump2Port(FuncDataBase& Control,
		const std::string& pumpName,
		const double angle)
/*!
  Set the 2 port ion pump variables
  \param Control :: DataBase to use
  \param name :: name prefix
  \param angle :: Y Rotation angle
 */
{
  ELog::RegMethod RegA("linacVariables[F]","setIonPump2Port");

  setVariable::IonPTubeGenerator IonTGen;

  IonTGen.setCF<setVariable::CF66_TDC>();
  IonTGen.setPortCF<setVariable::CF35_TDC>();
  IonTGen.setVertical(11.9,5.0);  // d / h    5.0 is dummy
  IonTGen.generateTube(Control,pumpName);
  Control.addVariable(pumpName+"YAngle",angle);

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
  SimpleTubeGen.generateTube(Control,name,fullLen);

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
setPrismaChamber(FuncDataBase& Control,
		 const std::string& name,
		 const double angle)
/*!
  Set the Prisma Chamber (4 port pipe) variables
  \param Control :: DataBase to use
  \param name :: name prefix
 */
{
  ELog::RegMethod RegA("linacVariables[F]","setPrismaChamber");

  setVariable::PrismaChamberGenerator PCGen;

  PCGen.generateChamber(Control,name);
  Control.addVariable(name+"YAngle",angle);
  /*
  const Geometry::Vec3D OPos(0.0, 0.0, 0.0);
  const Geometry::Vec3D XVec(1,0,0);
  const Geometry::Vec3D ZVec(0,0,1);

  setVariable::PipeTubeGenerator SimpleTubeGen;
  SimpleTubeGen.setMat("Stainless304L");
  SimpleTubeGen.generateBlank(Control,name,33.2); // measured
  Control.addVariable(name+"Radius",15.0); // measured
  Control.addVariable(name+"WallThick",0.2); // measured
  Control.addVariable(name+"BlankThick",0.8);  // measured
  Control.addVariable(name+"FlangeRadius",17.8); // measured
  Control.addVariable(name+"FlangeLength",2.7); // measured
  Control.addVariable(name+"FlangeCapThick",setVariable::CF63::flangeLength); // guess

  Control.addVariable(name+"NPorts",4);
  Control.addVariable(name+"FlangeCapThick",setVariable::CF63::flangeLength);
  Control.addVariable(name+"FlangeCapMat","Stainless304L");

  setVariable::PortItemGenerator PItemGen;
  const double portZStep = 0.5; // measured
  const Geometry::Vec3D pPos01(0.0,portZStep,7.0);
  const Geometry::Vec3D pPos23(0.0,portZStep,0.0);

  PItemGen.setCF<setVariable::CF35_TDC>(6.50778); // depends on pPos01
  PItemGen.setNoPlate();
  PItemGen.generatePort(Control,name+"Port0",pPos01,-XVec);

  PItemGen.generatePort(Control,name+"Port1",pPos01,XVec);

  PItemGen.setCF<setVariable::CF100>(3.9);
  PItemGen.generatePort(Control,name+"Port2",pPos23,ZVec);
  Control.addVariable(name+"Port2CapMat", "Stainless304L");

  PItemGen.setLength(3.9);
  PItemGen.generatePort(Control,name+"Port3",pPos23,-ZVec);
  Control.addVariable(name+"Port3CapMat", "Stainless304L");
  */
  return;
}

void
setSlitTube(FuncDataBase& Control,const std::string& name,
	    const double length)
/*!
  Set the slit tibe variables
  \param Control :: DataBase to use
  \param name :: name prefix
  \param length :: total length
 */
{
  ELog::RegMethod RegA("linacVariables[F]","setSlitTube");

  const double sideWallThick(1.0);
  const double DLength(length-sideWallThick*2.0); // slit tube length, without flanges[AB]

  setVariable::PortTubeGenerator PTubeGen;
  setVariable::PortItemGenerator PItemGen;

  PTubeGen.setMat("Stainless304L");
  PItemGen.setPlate(setVariable::CF63::flangeLength, "Stainless304L");

  const double Radius(7.5);
  const double WallThick(0.5);
  const double PortRadius(Radius+WallThick+0.5);
  PTubeGen.setPipe(Radius,WallThick);
  PTubeGen.setPortCF<setVariable::CF40>();
  PTubeGen.setPortLength(-sideWallThick,sideWallThick); // avoid changing length
  PTubeGen.setAFlange(PortRadius,sideWallThick);
  PTubeGen.setBFlange(PortRadius,sideWallThick);
  PTubeGen.generateTube(Control,name,0.0,DLength);
  Control.addVariable(name+"NPorts",4);

  const std::string portName=name+"Port";
  const Geometry::Vec3D MidPt(0,1.5,0);
  const Geometry::Vec3D XVec(1,0,0);
  const Geometry::Vec3D ZVec(0,0,1);
  const Geometry::Vec3D PPos(0.0,DLength/8.0,0);

  // first 2 ports are with jaws, others - without jaws
  PItemGen.setOuterVoid(1);  // create boundary round flange
  PItemGen.setCF<setVariable::CF63>(5.0+Radius+WallThick);
  PItemGen.generatePort(Control,portName+"0",-PPos,ZVec);
  PItemGen.setCF<setVariable::CF63>(10.0+Radius+WallThick);
  PItemGen.generatePort(Control,portName+"1",MidPt,XVec);

  PItemGen.setCF<setVariable::CF63>(5.0+Radius+WallThick);
  PItemGen.generatePort(Control,portName+"2",-PPos,-ZVec);
  PItemGen.setCF<setVariable::CF63>(10.0+Radius+WallThick);
  PItemGen.generatePort(Control,portName+"3",MidPt,-XVec);

  // PItemGen.setCF<setVariable::CF63>(10.0);
  // PItemGen.generatePort(Control,portName+"4",MidPt,
  // 			Geometry::Vec3D(1,0,1));

  JawFlangeGenerator JFlanGen;
  JFlanGen.setSlits(2.5, 2.5, 0.4, "Tantalum"); // W,H,T,mat
  JFlanGen.generateFlange(Control,name+"JawUnit0");
  JFlanGen.generateFlange(Control,name+"JawUnit1");

  Control.addVariable(name+"JawUnit0JOpen",1.7);
  Control.addVariable(name+"JawUnit1JOpen",1.7);

}

void
setRecGateValve(FuncDataBase& Control,
		const std::string& name,
		const bool closedFlag)
/*!
  Set the rectangular gate valve variables
  \param Control :: DataBase to use
  \param name :: name prefix
  \param closedFlag :: closed flag {true,false}
 */
{
  ELog::RegMethod RegA("linacVariables[F]","setRecGateValve");

  setVariable::GateValveGenerator RGateGen;

  RGateGen.setCubeCF<setVariable::CF40_22>();
  RGateGen.setPort(0.95,1.15,2.55); // Rin, Length, T=Rout-Rin
  RGateGen.setLength(1.2); // length of inner void
  RGateGen.setWallThick(0.2); // measured in segment 10
  RGateGen.generateValve(Control,name,0.0, static_cast<int>(closedFlag));
  Control.addVariable(name+"WallMat","Stainless304"); // email from KÅ, 2020-06-02
  // BladeMat is guess as VAT (the manufacturer) does not want to give more details
  // (email from Marek 2020-06-17)
  Control.addVariable(name+"BladeMat","Stainless304");
  Control.addVariable(name+"BladeThick",0.5); // guess (based on the gap measured)

  return;
}

void
setCylGateValve(FuncDataBase& Control,
		const std::string& name,
		const double rotateAngle,
		const bool closedFlag)
/*!
  Set the cylindrical gate valve variables
  \param Control :: DataBase to use
  \param name :: name prefix
  \param rotateAngle :: rotation angle (e.g. 90/-90, 180.0)
  \param closedFlag :: closed flag {true,false}
 */
{
  ELog::RegMethod RegA("linacVariables[F]","setCylGateValve");

  setVariable::CylGateValveGenerator CGateGen;

  CGateGen.setYRotate(rotateAngle);

  CGateGen.generateGate(Control,name,closedFlag);  // length 7.3 cm checked
  Control.addVariable(name+"WallThick",0.3);
  Control.addVariable(name+"PortThick",0.1);
  Control.addVariable(name+"WallMat","Stainless316L");// email from KÅ 2020-06-02
  // BladeMat is guess as VAT (the manufacturer) does not want to give more details
  // (email from Marek 2020-06-17)
  Control.addVariable(name+"BladeMat","Stainless316L"); // guess
  // PortRadius as of No_49_00,
  // but it seems like same value for all gate valves
  Control.addVariable(name+"PortRadius",2.005);

  return;
}

void
setFlat(FuncDataBase& Control,const std::string& name,
	const double length,const double rotateAngle,
	const double xStep)
/*!
  Set the Flat pipe variables
  \param Control :: DataBase to use
  \param name :: name prefix
  \param length :: length [cm]
  \param rotateAngle :: XY angle [deg]
 */
{
  ELog::RegMethod RegA("linacVariables[F]","setFlat");

  setVariable::FlatPipeGenerator FPGen;

  FPGen.generateFlat(Control,name,length);
  Control.addVariable(name+"XYAngle",rotateAngle);
  Control.addVariable(name+"WallThick",0.15); // No_3_00.pdf
  Control.addVariable(name+"FrontWidth",3.656-1.344); // No_3_00.pdf
  Control.addVariable(name+"BackWidth",3.656-1.344); // No_3_00.pdf
  Control.addVariable(name+"XStep",xStep); // No_3_00.pdf

  return;
}

void
setGauge34(FuncDataBase& Control,
	 const std::string& name)
/*!
  Set the vacuum gauge variables with CF34_TDC flanges
  \param Control :: DataBase to use
  \param name :: name prefix
 */
{
  ELog::RegMethod RegA("linacVariables[F]","setGauge34");

  setVariable::GaugeGenerator GGen;
  GGen.setCF<CF34_TDC>();
  GGen.setLength(12.6);
  GGen.setSidePortCF<setVariable::CF34_TDC>(6.95);
  GGen.setPlateThick(setVariable::CF34_TDC::flangeLength,"Stainless304L");

  GGen.generateGauge(Control,name,0.0,0.0);
  return;
}

void
setGauge37(FuncDataBase& Control,
	 const std::string& name)
/*!
  Set the vacuum gauge variables with CF37_TDC flanges
  \param Control :: DataBase to use
  \param name :: name prefix
 */
{
  ELog::RegMethod RegA("linacVariables[F]","setGauge37");

  setVariable::GaugeGenerator GGen;

  GGen.setCF<CF37_TDC>();
  GGen.setLength(12.6);
  GGen.setSidePortCF<setVariable::CF37_TDC>(6.95);
  GGen.setPlateThick(setVariable::CF37_TDC::flangeLength,"Stainless304L");

  GGen.generateGauge(Control,name,0.0,0.0);
  return;
}

void
setBellow26(FuncDataBase& Control,
	  const std::string& name,
	  const double length=7.5)
/*!
  Set the Bellow variables with CF26_TDC flanges
  \param Control :: DataBase to use
  \param name :: name prefix
  \paral length :: length [cm]
 */
{
  ELog::RegMethod RegA("linacVariables[F]","setBellow");
  setVariable::BellowGenerator BellowGen;

  BellowGen.setCF<setVariable::CF26_TDC>();
  BellowGen.setMat("Stainless304L", "Stainless304L%Void%3.0");
  BellowGen.generateBellow(Control,name,length);

  return;
}

void
setBellow37(FuncDataBase& Control,
	  const std::string& name,
	  const double length)
/*!
  Set the Bellow variables with CF37_TDC flanges
  \param Control :: DataBase to use
  \param name :: name prefix
  \paral length :: length [cm]
 */
{
  ELog::RegMethod RegA("linacVariables[F]","setBellow");
  setVariable::BellowGenerator BellowGen;

  BellowGen.setCF<setVariable::CF37_TDC>();
  BellowGen.setMat("Stainless304L", "Stainless304L%Void%3.0");
  BellowGen.generateBellow(Control,name,length);

  return;
}

void
setMirrorChamber(FuncDataBase& Control,
		 const std::string& name,
		 const double angle)
/*!
  Set the Mirror Chamber (4 port pipe) variables
  \param Control :: DataBase to use
  \param name :: name prefix
 */
{
  ELog::RegMethod RegA("linacVariables[F]","setMirrorChamber");

  setVariable::CrossWayGenerator MSPGen;

  MSPGen.setCF<CF63>();
  MSPGen.setMainLength(7.4,13.4);    // 20.8
  MSPGen.setPortLength(6.3,6.3);
  MSPGen.setCrossLength(6.25,6.25);
  MSPGen.setPortCF<CF35_TDC>();
  MSPGen.setCrossCF<CF40_22>();
  MSPGen.setPlateThick(setVariable::CF34_TDC::flangeLength,"Stainless304L");
  MSPGen.generateCrossWay(Control,name);
  Control.addVariable(name+"YAngle", angle);

  return;
}

void
setMirrorChamberBlank(FuncDataBase& Control,
		      const std::string& name,
		      const double angle)
/*!
  Set the blank Mirror Chamber (4 port pipe) variables
  \param Control :: DataBase to use
  \param name :: name prefix
  \param angel :: rotation angle
 */
{
  ELog::RegMethod RegA("linacVariables[F]","setMirrorChamberBlank");

  setVariable::CrossWayGenerator MSPGen;

  MSPGen.setCF<CF63>();
  MSPGen.setMainLength(2.4,13.6);
  MSPGen.setPortLength(5.9,8.1);
  MSPGen.setCrossLength(6.25,6.25);
  MSPGen.setPortCF<CF35_TDC>();
  MSPGen.setCrossCF<CF35_TDC>();
  MSPGen.generateCrossWay(Control,name);
  Control.addVariable(name+"YAngle", angle);

  return;
}


void
Segment1(FuncDataBase& Control,
		  const std::string& lKey)
  /*!
    Set the variables for segment 1
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{
  ELog::RegMethod RegA("linacVariables[F]","Segment1");


  Segment1Magnet(Control,lKey);
  return;
}

void
Segment2(FuncDataBase& Control,
		   const std::string& lKey)
  /*!
    Set the variables for segment 2
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{
  ELog::RegMethod RegA("linacVariables[F]","Segment2");

  setVariable::PipeGenerator PGen;
  setVariable::LinacQuadGenerator LQGen;
  setVariable::StriplineBPMGenerator BPMGen;
  setVariable::EArrivalMonGenerator EArrGen;
  setVariable::YagScreenGenerator YagScreenGen;
  setVariable::YagUnitGenerator YagUnitGen;

  const Geometry::Vec3D startPt(0,395.2,0);
  const Geometry::Vec3D endPt(0,881.06,0);
  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);

  PGen.setCF<setVariable::CF18_TDC>();
  PGen.setMat("Stainless316L");
  PGen.setNoWindow();

  PGen.generatePipe(Control,lKey+"PipeA",34.0); // No_2_00.pdf
  LQGen.generateQuad(Control,lKey+"QuadA",17.16); // No_2_00.pdf
  BPMGen.generateBPM(Control,lKey+"BPMA",0.0);

  setBellow26(Control,lKey+"BellowA",7.5);

  PGen.generatePipe(Control,lKey+"PipeB",113.96); // No_2_00.pdf
  LQGen.generateQuad(Control,lKey+"QuadB",73.66); // No_2_00.pdf

  setCylGateValve(Control,lKey+"Gate", 180.0, false);

  PGen.setOuterVoid();
  PGen.generatePipe(Control,lKey+"PipeC",31.5); // No_2_00.pdf

  EArrGen.generateMon(Control,lKey+"BeamArrivalMon",0.0);

  PGen.generatePipe(Control,lKey+"PipeD",75.8); // No_2_00.pdf

  setBellow26(Control,lKey+"BellowB",7.5);

  BPMGen.generateBPM(Control,lKey+"BPMB",0.0);

  PGen.setOuterVoid(0);
  PGen.generatePipe(Control,lKey+"PipeE",133.4); // No_2_00.pdf
  Control.addVariable(lKey+"PipeEYAngle",90.0);

  LQGen.generateQuad(Control,lKey+"QuadC",24.7); // No_2_00.pdf
  LQGen.generateQuad(Control,lKey+"QuadD",74.7); // No_2_00.pdf
  LQGen.generateQuad(Control,lKey+"QuadE",114.7); // No_2_00.pdf

  YagUnitGen.generateYagUnit(Control,lKey+"YagUnit");
  YagScreenGen.generateScreen(Control,lKey+"YagScreen",0);   // closed
  Control.addVariable(lKey+"YagScreenYAngle",-90.0);

  setVariable::LocalShieldingGenerator LSGen;
  LSGen.setSize(40.0,10.0,15.0); // 62.jpg
  LSGen.generate(Control,lKey+"ShieldA");
  Control.addVariable(lKey+"ShieldAYStep",-50.0); // approx

  return;
}

void
Segment3(FuncDataBase& Control,
		  const std::string& lKey)
  /*!
    Set the variables for segment 3
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{
  ELog::RegMethod RegA("linacVariables[F]","Segment3");

  setVariable::PipeGenerator PGen;
  setVariable::FlatPipeGenerator FPGen;
  setVariable::DipoleDIBMagGenerator DIBGen;
  setVariable::CorrectorMagGenerator CMGen;

  const Geometry::Vec3D startPt(0,881.06,0);
  const Geometry::Vec3D endPt(-15.322,1155.107,0);
  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);

  PGen.setCF<setVariable::CF18_TDC>();
  PGen.setMat("Stainless316L");
  PGen.setNoWindow();

  setBellow26(Control,lKey+"BellowA",7.5);

  const double flatAXYAngle=1.6;
  setFlat(Control,lKey+"FlatA",82.549/cos(flatAXYAngle*M_PI/180.0),flatAXYAngle); // No_3_00.pdf

  DIBGen.generate(Control,lKey+"DipoleA");
  Control.addVariable(lKey+"DipoleAYStep",0.0065); // this centers DipoleA at 48.781 [No_3_00.pdf]

  const double pipeAXYAngle = 1.6;
  const double pipeAcos = cos((flatAXYAngle+pipeAXYAngle)*M_PI/180.0);
  PGen.generatePipe(Control,lKey+"PipeA",94.253/pipeAcos); // No_3_00.pdf
  Control.addVariable(lKey+"PipeAXYAngle",pipeAXYAngle);
  // Control.addVariable(lKey+"PipeAXStep",-1.2);
  // Control.addVariable(lKey+"PipeAFlangeFrontXStep",1.2);

  CMGen.generateMag(Control,lKey+"CMagHA",15.176/pipeAcos,0);  // No_3_00.pdf
  CMGen.generateMag(Control,lKey+"CMagVA",31.151/pipeAcos,1); // No_3_00.pdf

  const double flatBXYAngle = 1.6;  // No_3_00.pdf
  const double flatBcos = cos((flatAXYAngle+pipeAXYAngle+flatBXYAngle)*M_PI/180.0);  // No_3_00.pdf
  setFlat(Control,lKey+"FlatB",82.292/flatBcos,flatBXYAngle);  // No_3_00.pdf

  DIBGen.generate(Control,lKey+"DipoleB");
  Control.addVariable(lKey+"DipoleBYStep",0.02); // this centers DipoleB at 225.468 [No_3_00.pdf]

  setBellow26(Control,lKey+"BellowB",7.5);
  Control.addVariable(lKey+"BellowBXYAngle",1.6);

  // Local shielding wall
  // http://localhost:8080/maxiv/work-log/tdc/pictures/bc2/pictures-from-josefine/63.jpg/view
  // There are 7x4-8=28-8=20 bricks of 10 cm length
  // and 8 bricks of 15 cm length
  // in total there are 20+8 = 28 bricks
  // averge length is 10.0*20.0/28.0+15.0*8.0/28.0 = 11.428571 cm
  const double shieldALength(11.428571);
  setVariable::LocalShieldingGenerator LSGen;
  LSGen.setSize(shieldALength,80,35.0);
  LSGen.setMidHole(3.0, 5.0); // guess
  LSGen.generate(Control,lKey+"ShieldA");
  Control.addVariable(lKey+"ShieldAZStep",10.0); // center of the 2nd tier bricks
  Control.addVariable(lKey+"ShieldAYStep",80.0); // approx

  const double shieldBLength(10.0);
  const double shieldBWidth(140.0);
  LSGen.setSize(shieldBLength,shieldBWidth,35.0); // email from JR 210120
  LSGen.setMidHole(0.0, 0.0);
  LSGen.setCorner(120, 5.0, "right");
  LSGen.generate(Control,lKey+"ShieldB");
  Control.addVariable(lKey+"ShieldBYStep",shieldBLength/2.0);
  Control.addVariable(lKey+"ShieldBXStep",(shieldALength-shieldBWidth)/2.0);

  return;
}

void
Segment4(FuncDataBase& Control,
		  const std::string& lKey)
  /*!
    Set the variables for segment 4
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{
  ELog::RegMethod RegA("linacVariables[F]","Segment4");

  setVariable::PipeGenerator PGen;
  setVariable::StriplineBPMGenerator BPMGen;
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

  PGen.setCF<setVariable::CF18_TDC>();
  PGen.setMat("Stainless316L");
  PGen.setNoWindow();
  PGen.setOuterVoid();
  PGen.generatePipe(Control,lKey+"PipeA",67.0); // No_4_00.pdf

  BPMGen.generateBPM(Control,lKey+"BPMA",0.0);

  PGen.setOuterVoid(0);
  PGen.generatePipe(Control,lKey+"PipeB",80.2); // No_4_00.pdf

  LQGen.generateQuad(Control,lKey+"QuadA",19.7); // No_4_00.pdf
  LSGen.generateSexu(Control,lKey+"SexuA",40.7); // No_4_00.pdf
  LQGen.generateQuad(Control,lKey+"QuadB",61.7); // No_4_00.pdf

  YagUnitGen.generateYagUnit(Control,lKey+"YagUnit");
  Control.addVariable(lKey+"YagUnitYAngle",-90.0);

  YagScreenGen.generateScreen(Control,lKey+"YagScreen",0);   // closed
  Control.addVariable(lKey+"YagScreenYAngle",-90.0);

  setBellow26(Control,lKey+"BellowA",7.5);

  PGen.generatePipe(Control,lKey+"PipeC",70.2); // No_4_00.pdf

  CMGen.generateMag(Control,lKey+"CMagHA",14,1);
  CMGen.generateMag(Control,lKey+"CMagVA",34,0);

  setVariable::LocalShieldingGenerator LocalShieldingGen;
  LocalShieldingGen.setSize(40.0,10.0,15.0); // 64.jpg
  LocalShieldingGen.generate(Control,lKey+"ShieldA");
  Control.addVariable(lKey+"ShieldAYStep",-50.0); // approx

  return;
}

void
Segment5(FuncDataBase& Control,
		  const std::string& lKey)
  /*!
    Set the variables for segment 5
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{
  ELog::RegMethod RegA("linacVariables[F]","Segment5");

  const Geometry::Vec3D startPt(-45.073,1420.344,0);
  const Geometry::Vec3D endPt(-90.011,1683.523,0);
  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"XYAngle",6.4);

  Segment5Magnet(Control,lKey);

  return;
}

void
Segment6(FuncDataBase& Control,
		  const std::string& lKey)
  /*!
    Set the variables for segment 6
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{
  ELog::RegMethod RegA("linacVariables[F]","Segment6");

  setVariable::PipeGenerator PGen;
  setVariable::EBeamStopGenerator EBGen;
  setVariable::ScrapperGenerator SCGen;
  setVariable::CeramicGapGenerator CSGen;

  const Geometry::Vec3D startPt(-90.011,1683.523,0.0);
  const Geometry::Vec3D endPt(-147.547,1936.770,0.0);
  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"XYAngle",
  		      atan((startPt.X()-endPt.X())/(endPt.Y()-startPt.Y()))*180.0/M_PI);

  PGen.setCF<setVariable::CF18_TDC>();
  PGen.setMat("Stainless316L");
  PGen.setNoWindow();
  PGen.setOuterVoid();

  PGen.generatePipe(Control,lKey+"PipeA",61.7);

  PGen.generatePipe(Control,lKey+"PipeB",20.0);

  PGen.setBFlangeCF<setVariable::CF66_TDC>();
  PGen.generatePipe(Control,lKey+"PipeC",55.0);

  SCGen.setCF<setVariable::CF66_TDC>();
  SCGen.generateScrapper(Control,lKey+"Scrapper",1.0);   // z lift

  PGen.setCF<setVariable::CF18_TDC>();
  PGen.setAFlangeCF<setVariable::CF66_TDC>();
  PGen.setOuterVoid(0);
  PGen.generatePipe(Control,lKey+"PipeD",20.0);

  CSGen.generateCeramicGap(Control,lKey+"CeramicA");

  EBGen.generateEBeamStop(Control,lKey+"EBeam",0);
  Control.addVariable(lKey+"EBeamShieldActive",1);

  CSGen.generateCeramicGap(Control,lKey+"CeramicB");

  setVariable::LocalShieldingGenerator LSGen;
  LSGen.setSize(20.0,25,20.0);
  LSGen.setMidHole(15.0, 5.0); // guess
  LSGen.generate(Control,lKey+"ShieldA");
  Control.addVariable(lKey+"ShieldAOption","SideOnly");

  LSGen.setSize(5.0,25,20.0);
  LSGen.setMidHole(5.0, 5.0); // guess
  LSGen.generate(Control,lKey+"ShieldB");
  Control.addVariable(lKey+"ShieldBYStep",5.0); // approx
  Control.addVariable(lKey+"ShieldBOption","SideOnly");

  return;
}

void
Segment7(FuncDataBase& Control,
		  const std::string& lKey)
  /*!
    Set the variables for segment 7
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{
  ELog::RegMethod RegA("linacVariables[F]","Segment7");

  setVariable::PipeGenerator PGen;
  setVariable::StriplineBPMGenerator BPMGen;
  setVariable::LinacQuadGenerator LQGen;
  setVariable::CorrectorMagGenerator CMGen;

  const Geometry::Vec3D startPt(-147.547,1936.770,0.0);
  const Geometry::Vec3D endPt(-206.146,2194.697,0.0);

  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"XYAngle",12.8);

  PGen.setMat("Stainless316L","Stainless304L");
  PGen.setCF<setVariable::CF18_TDC>();
  PGen.setNoWindow();

  PGen.generatePipe(Control,lKey+"PipeA",102.3);
  CMGen.generateMag(Control,lKey+"CMagHA",39.0,0);
  LQGen.generateQuad(Control,lKey+"QuadA",79.0);
  BPMGen.generateBPM(Control,lKey+"BPM",0.0);
  PGen.generatePipe(Control,lKey+"PipeB",140.2);
  CMGen.generateMag(Control,lKey+"CMagVA",11.0,1);


  return;
}

void
Segment8(FuncDataBase& Control,
		  const std::string& lKey)
  /*!
    Set the variables for segment 8
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{
  ELog::RegMethod RegA("linacVariables[F]","Segment8");

  setVariable::EBeamStopGenerator EBGen;
  setVariable::PipeGenerator PGen;

  const Geometry::Vec3D startPt(-206.146,2194.697,0.0);
  const Geometry::Vec3D endPt(-288.452,2556.964,0.0);

  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"XYAngle",12.8);

  PGen.setCF<setVariable::CF18_TDC>();
  PGen.setMat("Stainless316L", "Stainless304L");
  PGen.setNoWindow();

  setBellow26(Control,lKey+"BellowA",7.5);

  EBGen.generateEBeamStop(Control,lKey+"EBeam",0);
  Control.addVariable(lKey+"EBeamShieldActive",1);

  setBellow26(Control,lKey+"BellowB",7.5);

  PGen.setOuterVoid();
  PGen.generatePipe(Control,lKey+"PipeA",308.5);


  return;
}

void
Segment9(FuncDataBase& Control,
		  const std::string& lKey)
  /*!
    Set the variables for segment 8
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{
  ELog::RegMethod RegA("linacVariables[F]","Segment9");

  setVariable::PipeGenerator PGen;
  setVariable::StriplineBPMGenerator BPMGen;
  setVariable::CorrectorMagGenerator CMGen;
  setVariable::LinacQuadGenerator LQGen;
  setVariable::CeramicGapGenerator CSGen;

  const Geometry::Vec3D startPt(-288.452,2556.964,0.0);
  const Geometry::Vec3D endPt(-323.368,2710.648,0.0);

  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"XYAngle",
  		      atan((startPt.X()-endPt.X())/(endPt.Y()-startPt.Y()))*180.0/M_PI); // 12.8

  PGen.setCF<setVariable::CF18_TDC>();
  PGen.setMat("Stainless316L","Stainless304L");
  PGen.setNoWindow();

  CSGen.generateCeramicGap(Control,lKey+"CeramicBellowA");

  const double yAngle(-90.0);
  setIonPump2Port(Control, lKey+"PumpA",yAngle);

  PGen.generatePipe(Control,lKey+"PipeA",57.8); // No_9_00.pdf
  Control.addVariable(lKey+"PipeAYAngle", -yAngle);

  CMGen.generateMag(Control,lKey+"CMagVA",22.0,1);
  CMGen.generateMag(Control,lKey+"CMagHA",42.0,0);

  setBellow26(Control,lKey+"BellowB",7.5);
  BPMGen.generateBPM(Control,lKey+"BPM",0.0);

  PGen.setBFlange(setVariable::CF18_TDC::innerRadius+
		  setVariable::CF18_TDC::wallThick,
		  setVariable::CF18_TDC::flangeLength);
  PGen.generatePipe(Control,lKey+"PipeB",32.8);
  LQGen.generateQuad(Control,lKey+"QuadA",19.7-1.0); // -1 cm to avoit cutting the PipeB flangeB

  setVariable::BellowGenerator BellowGen;
  BellowGen.setCF<setVariable::CF18_TDC>();
  BellowGen.setMat("Stainless316L", "Stainless316L%Void%3.0");
  BellowGen.setFlangePair(setVariable::CF18_TDC::innerRadius+setVariable::CF18_TDC::wallThick,
			  setVariable::CF18_TDC::flangeLength,
			  setVariable::CF18_TDC::flangeRadius,
			  setVariable::CF18_TDC::flangeLength);
  BellowGen.generateBellow(Control,lKey+"BellowC",7.5);

  return;
}

void
Segment10(FuncDataBase& Control,
		   const std::string& lKey)
  /*!
    Set the variables for segment 10
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{
  ELog::RegMethod RegA("linacVariables[F]","Segment10");

  setVariable::PipeGenerator PGen;
  setVariable::CorrectorMagGenerator CMGen;
  setVariable::LinacQuadGenerator LQGen;


  const Geometry::Vec3D startPt(-323.368,2710.648,0.0);
  const Geometry::Vec3D endPt(-492.992,3457.251,0.0);

  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"XYAngle",12.8);

  Control.addVariable(lKey+"WallRadius",3.0);

  PGen.setCF<setVariable::CF18_TDC>();
  PGen.setMat("Stainless316L","Stainless304L");
  PGen.setNoWindow();

  PGen.generatePipe(Control,lKey+"PipeA",453.0);
  setBellow26(Control,lKey+"BellowA",7.5);

  setRecGateValve(Control, lKey+"Gate", false);

  const double yAngle(-90.0);
  setIonPump2Port(Control, lKey+"PumpA",yAngle);

  PGen.setOuterVoid();
  PGen.generatePipe(Control,lKey+"PipeB",152.1);
  Control.addVariable(lKey+"PipeBYAngle", -yAngle);
  setBellow26(Control,lKey+"BellowB",7.5);

  PGen.setOuterVoid(0);
  PGen.generatePipe(Control,lKey+"PipeC",126.03);

  LQGen.generateQuad(Control,lKey+"QuadA",33.8);
  CMGen.generateMag(Control,lKey+"CMagVA",115.23,1);

  // Local shielding
  setVariable::LocalShieldingGenerator LSGen;
  LSGen.setSize(10.0,40.0,30.0);
  LSGen.setMidHole(5.0, 5.0); // guess
  LSGen.generate(Control,lKey+"ShieldA");
  Control.addVariable(lKey+"ShieldAYStep",7.0);

  LSGen.setSize(5.0,20.0,10.0);
  LSGen.setMidHole(0.0, 0.0);
  LSGen.generate(Control,lKey+"ShieldB");
  Control.addVariable(lKey+"ShieldBYStep",2.5);

  return;
}

void
Segment11(FuncDataBase& Control,
		   const std::string& lKey)
  /*!
    Set the variables for segment 11
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{
  ELog::RegMethod RegA("linacVariables[F]","Segment11");

  setVariable::PipeGenerator PGen;
  setVariable::StriplineBPMGenerator BPMGen;
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
  PGen.setMat("Stainless316L","Stainless304L");
  PGen.setNoWindow();

  setBellow26(Control,lKey+"BellowA",7.5);
  BPMGen.generateBPM(Control,lKey+"BPM",0.0);

  PGen.generatePipe(Control,lKey+"PipeA",42.50);
  LQGen.generateQuad(Control,lKey+"QuadA",24.7);

  setIonPump3OffsetPort(Control,lKey+"PumpA");

  YagUnitGen.generateYagUnit(Control,lKey+"YagUnit");
  Control.addVariable(lKey+"YagUnitYAngle",90.0);

  YagScreenGen.generateScreen(Control,lKey+"YagScreen",0);   // open
  Control.addVariable(lKey+"YagScreenYAngle",-90.0);

  PGen.generatePipe(Control,lKey+"PipeB",154.47);
  Control.addVariable(lKey+"PipeBYAngle",-90.0);

  CMGen.generateMag(Control,lKey+"CMagHA",10.0,0);

  // Local shielding wall
  // http://localhost:8080/maxiv/work-log/tdc/pictures/spf-hall/img_5404.mp4/view
  setVariable::LocalShieldingGenerator ShieldGen;
  ShieldGen.setSize(10,60,25);
  ShieldGen.setMidHole(2.5,5.0);
  ShieldGen.generate(Control,lKey+"ShieldA");
  Control.addVariable(lKey+"ShieldAYStep",47.0);
  Control.addVariable(lKey+"ShieldAZStep",5.0);

  return;
}

void
Segment12(FuncDataBase& Control,
		   const std::string& lKey)
  /*!
    Set the variables for segment 12
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{
  ELog::RegMethod RegA("linacVariables[F]","Segment12");

  setVariable::CF63 CF63unit;
  setVariable::PipeGenerator PGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::FlatPipeGenerator FPGen;
  setVariable::DipoleDIBMagGenerator DIBGen;
  setVariable::BeamDividerGenerator BDGen(CF63unit);
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::IonPTubeGenerator IonTGen;

  const Geometry::Vec3D startPtA(-547.597,3697.597,0.0);
  const Geometry::Vec3D endPtA(-609.286,3969.122,0.0);  // to segment 30
  const Geometry::Vec3D endPtB(-593.379,3968.258,0.0);  // to segment 13

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

  const double XYAngle(12.8);
  Control.addVariable(lKey+"XYAngle",XYAngle);

  Segment12Magnet(Control,lKey);

  // Placeholder for the local shielding wall
  // http://localhost:8080/maxiv/work-log/tdc/pictures/spf-hall/img_5409.mp4/view
  setVariable::LocalShieldingGenerator ShieldGen;
  // max length is 22 cm [img_5422]
  // max width is 86 cm [img_5428]
  ShieldGen.setSize(10,60,25); // length is 10, height/width arbitrary since it's a placeholder
  ShieldGen.setMidHole(10.0,4.0);
  ShieldGen.generate(Control,lKey+"ShieldA");
  Control.addVariable(lKey+"ShieldAYStep",17.0); // IMG_5423.JPG
  Control.addVariable(lKey+"ShieldAZStep",0.0); // dummy
  Control.addVariable(lKey+"ShieldAMainMat","Void"); // placeholder

  return;
}

void
Segment13(FuncDataBase& Control,
		   const std::string& lKey)
  /*!
    Set the variables for segment 13
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{
  ELog::RegMethod RegA("linacVariables[F]","Segment13");

  setVariable::PipeGenerator PGen;
  setVariable::StriplineBPMGenerator BPMGen;
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

  PGen.setCF<setVariable::CF18_TDC>();
  PGen.setMat("Stainless316L");
  PGen.setNoWindow();
  PGen.generatePipe(Control,lKey+"PipeA",67.001); // No_13_00.pdf

  CMGen.generateMag(Control,lKey+"CMagHA",56.701,0);

  BPMGen.generateBPM(Control,lKey+"BPMA",0.0);

  PGen.generatePipe(Control,lKey+"PipeB",81.7);
  LQGen.generateQuad(Control,lKey+"QuadA",19.7);
  LSGen.generateSexu(Control,lKey+"SexuA",40.7);
  LQGen.generateQuad(Control,lKey+"QuadB",61.7);

  YagUnitGen.generateYagUnit(Control,lKey+"YagUnit",true);
  Control.addVariable(lKey+"YagUnitYAngle",180);

  YagScreenGen.generateScreen(Control,lKey+"YagScreen",0); // 1=closed
  Control.addVariable(lKey+"YagScreenYAngle",-90.0);

  PGen.generatePipe(Control,lKey+"PipeC",68.7);
  CMGen.generateMag(Control,lKey+"CMagVA",11,1);

  // Placeholder for the local shielding wall
  // Part of SPF30ShieldA
  // http://localhost:8080/maxiv/work-log/tdc/pictures/spf-hall/tdc/img_5421.jpg/view
  setVariable::LocalShieldingGenerator ShieldGen;
  // max length is 14 cm [img_5420]
  // max width is 105 cm [img_5429]
  ShieldGen.setSize(10,40,25); // length is 10, height/width arbitrary since it's a placeholder
  ShieldGen.setMidHole(4.0,4.0); // dummy
  ShieldGen.generate(Control,lKey+"ShieldA");
  Control.addVariable(lKey+"ShieldAYStep",25.0); // IMG_5421.JPG
  Control.addVariable(lKey+"ShieldAXStep",7.5); // to avoid cutting SPF30
  Control.addVariable(lKey+"ShieldAZStep",0.0); // dummy
  Control.addVariable(lKey+"ShieldAMainMat","Void"); // placeholder

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

  setVariable::PipeGenerator PGen;
  setVariable::PortItemGenerator PItemGen;

  const Geometry::Vec3D startPt(-622.286,4226.013,0.0);
  const Geometry::Vec3D endPt(-637.608,4507.2590,0.0);
  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);
  const double XYAngle = 6.4;
  Control.addVariable(lKey+"XYAngle", XYAngle);

  setBellow26(Control,lKey+"BellowA",7.5);

  PGen.setCF<setVariable::CF18_TDC>();
  PGen.setMat("Stainless316L");
  PGen.setNoWindow();

  // length and angle are obtained from start/end coordinates of flatA
  const double flatAXYAngle=4.801-XYAngle; // 4.801 obtained from start/end coordinates of flatA
  setFlat(Control,lKey+"FlatA",82.581,flatAXYAngle); // No_14_00.pdf

  setVariable::DipoleDIBMagGenerator DIBGen;
  DIBGen.generate(Control,lKey+"DipoleA");

  PGen.setMat("Stainless316L","Stainless304L");
  PGen.setOuterVoid();
  PGen.generatePipe(Control,lKey+"PipeB",94.4); // No_14_00.pdf
  Control.addVariable(lKey+"PipeBXYAngle",-1.6); // No_14_00.pdf

  // length and angle are obtained from start/end coordinates of flatB
  // -0.2 to shift the BellowB end at the correct place
  setFlat(Control,lKey+"FlatB",82.581,-1.6-0.2);

  DIBGen.generate(Control,lKey+"DipoleB");

  setCylGateValve(Control,lKey+"GateA",-90,false);

  setBellow26(Control,lKey+"BellowB",7.5); // No_14_00.pdf

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
  Control.addVariable(lKey+"XYAngle",0.0);


  PGen.setCF<setVariable::CF40_22>();
  PGen.setMat("Stainless316L","Stainless304L");
  PGen.setNoWindow();
  PGen.generatePipe(Control,lKey+"PipeA",22.0); // measured

  // Mirror chamber
  setMirrorChamberBlank(Control, lKey+"MirrorChamber",0.0);

  YagUnitGen.setCF<CF63>();
  YagUnitGen.generateYagUnit(Control,lKey+"YagUnit");
  Control.addVariable(lKey+"YagUnitYAngle",90.0);

  YagScreenGen.generateScreen(Control,lKey+"YagScreen",0); // 1=closed
  Control.addVariable(lKey+"YagScreenYAngle",-90.0);

  PGen.setOuterVoid();
  PGen.generatePipe(Control,lKey+"PipeB",167.0);

  // Placeholder for the local shielding wall
  // http://localhost:8080/maxiv/work-log/tdc/pictures/spf-hall/img_????.mp4/view
  setVariable::LocalShieldingGenerator ShieldGen;
  // max length is 14 cm [img_5426.jpg]
  // max width is 91 cm [img_5430.jpg]
  ShieldGen.setSize(10,60,25); // length is 10, height/width arbitrary since it's a placeholder
  ShieldGen.setMidHole(10.0,4.0); // dummy
  ShieldGen.generate(Control,lKey+"ShieldA");
  Control.addVariable(lKey+"ShieldAYStep",11.0); // img_5427.jpg
  Control.addVariable(lKey+"ShieldAZStep",0.0); // dummy
  Control.addVariable(lKey+"ShieldAMainMat","Void"); // placeholder


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

  setVariable::StriplineBPMGenerator BPMGen;

  setVariable::PipeGenerator PGen;
  setVariable::LinacQuadGenerator LQGen;
  setVariable::CorrectorMagGenerator CMGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::YagScreenGenerator YagGen;
  setVariable::IonPTubeGenerator IonTGen;

  const Geometry::Vec3D startPt(-637.608,4730.259,0.0);
  const Geometry::Vec3D endPt(-637.608,4983.291,0.0);
  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"XYAngle",0.0);

  setBellow26(Control,lKey+"BellowA",7.5);

  BPMGen.generateBPM(Control,lKey+"BPM",0.0);

  PGen.setCF<setVariable::CF8_TDC>();
  PGen.setNoWindow();
  PGen.setMat("Stainless316L","Stainless304L");
  PGen.generatePipe(Control,lKey+"PipeA",34.0); // measured

  // QG (QH) type quadrupole magnet
  LQGen.setRadius(0.56, 2.31); // 0.56 - measured
  LQGen.generateQuad(Control,lKey+"Quad",19.0);
  // measured - inner box half width/height
  Control.addVariable(lKey+"QuadYokeOuter",9.5);
  // adjusted so that nose is 1 cm thick as in the STEP file
  Control.addVariable(lKey+"QuadPolePitch",26.0);

  PGen.setCF<setVariable::CF18_TDC>();
  PGen.generatePipe(Control,lKey+"PipeB",40.0); // measured

  CMGen.generateMag(Control,lKey+"CMagHA",10.0,0); // measured
  CMGen.generateMag(Control,lKey+"CMagVA",28.0,1); // measured

  setBellow26(Control,lKey+"BellowB",7.5);

  setIonPump2Port(Control,lKey+"IonPump",0.0);

  PGen.setOuterVoid();
  PGen.generatePipe(Control,lKey+"PipeC",126.03); // measured

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


  const Geometry::Vec3D startPt(-637.608,4983.291,0.0);
  const Geometry::Vec3D endPt(-637.608,5780.261,0.0);
  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);

  PGen.setCF<setVariable::CF18_TDC>();
  PGen.setMat("Stainless316L","Stainless304L");
  PGen.setNoWindow();

  PGen.setOuterVoid();
  PGen.generatePipe(Control,lKey+"PipeA",391.23); // No_17_00.pdf

  setBellow26(Control,lKey+"BellowA",7.5);

  setIonPump2Port(Control,lKey+"IonPump",0.0);

  PGen.generatePipe(Control,lKey+"PipeB",382.23); // No_17_00.pdf

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

  setVariable::StriplineBPMGenerator BPMGen;

  setVariable::PipeGenerator PGen;
  PGen.setNoWindow();
  setVariable::LinacQuadGenerator LQGen;
  setVariable::CorrectorMagGenerator CMGen;

  // coordinates form drawing
  const Geometry::Vec3D startPt(-637.608,5780.261,0.0);
  const Geometry::Vec3D endPt(-637.608,5994.561,0.0);
  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);

  setBellow26(Control,lKey+"BellowA",7.5);

  setIonPump2Port(Control, lKey+"IonPump",0.0);

  setBellow26(Control,lKey+"BellowB",7.5);

  BPMGen.generateBPM(Control,lKey+"BPM",0.0);

  /// Quad and PipeA
  PGen.setCF<setVariable::CF8_TDC>();
  PGen.setMat("Stainless316L","Stainless304L");
  PGen.generatePipe(Control,lKey+"PipeA",34.0);

  // QG (QH) type quadrupole magnet
  LQGen.setRadius(0.56, 2.31); // 0.56 -> measured (QH)
  LQGen.generateQuad(Control,lKey+"Quad",19.0); // No_18_00.pdf

  //  - inner box half width/height
  Control.addVariable(lKey+"QuadYokeOuter",9.5);
  // adjusted so that nose is 1 cm thick as in the STEP file
  Control.addVariable(lKey+"QuadPolePitch",26.0);

  PGen.setCF<setVariable::CF18_TDC>();
  PGen.generatePipe(Control,lKey+"PipeB",127.3);

  CMGen.generateMag(Control,lKey+"CMagHA",10.0,0);
  CMGen.generateMag(Control,lKey+"CMagVA",28.0,1);

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

  const Geometry::Vec3D startPt(-637.608,5994.561,0.0);
  const Geometry::Vec3D endPt(-637.608,6045.428,0.0);
  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);

  setBellow26(Control,lKey+"BellowA",7.5);

  setGauge37(Control,lKey+"Gauge");
  Control.addVariable(lKey+"GaugeYAngle", 180.0);

  // Fast closing valve
  setRecGateValve(Control,lKey+"GateA",false);
  Control.addVariable(lKey+"GateAYAngle", -180.0);

  setIonPump1Port(Control,lKey+"IonPump",-180.0);

  setCylGateValve(Control,lKey+"GateB",180.0,false);

  setBellow26(Control,lKey+"BellowB",7.5);

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
  PGen.setCF<setVariable::CF18_TDC>();
  PGen.setMat("Stainless304L","Stainless304L");
  setVariable::TWCavityGenerator TDCGen;

  const Geometry::Vec3D startPt(-637.608,6045.428,0.0);
  const Geometry::Vec3D endPt(-637.608,6358.791,0.0);
  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);

  const double flangeLength(3.652);  // No_20_00.pdf
  PGen.generatePipe(Control,lKey+"PipeA",flangeLength);
  Control.addVariable(lKey+"PipeARadius",1.16); // inner radius
  Control.addVariable(lKey+"PipeAFeThick",0.2); // wall thick

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
  setVariable::StriplineBPMGenerator BPMGen;

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

  setBellow26(Control,lKey+"BellowA",7.5);

  BPMGen.generateBPM(Control,lKey+"BPM",0.0);

  PGen.setCF<setVariable::CF8_TDC>();
  PGen.setMat("Stainless316L","Stainless304L");
  PGen.generatePipe(Control,lKey+"PipeA",34.0);

  // QG (QH) type quadrupole magnet
  LQGen.setRadius(0.56, 2.31);
  LQGen.generateQuad(Control,lKey+"Quad",18.0);
  // inner box half width/height
  Control.addVariable(lKey+"QuadYokeOuter",9.5);
  // adjusted so that nose is 1 cm thick as in the STEP file
  Control.addVariable(lKey+"QuadPolePitch",26.0);

  YagUnitGen.generateYagUnit(Control,lKey+"YagUnit",true);
  Control.addVariable(lKey+"YagUnitYAngle",90.0);

  YagGen.generateScreen(Control,lKey+"YagScreen",0);
  Control.addVariable(lKey+"YagScreenYAngle",-90.0);

  PGen.setCF<setVariable::CF18_TDC>();
  PGen.generatePipe(Control,lKey+"PipeB",45.7);

  CMGen.generateMag(Control,lKey+"CMagHA",10.3,1);
  CMGen.generateMag(Control,lKey+"CMagVA",28.3,0);

  setBellow26(Control,lKey+"BellowB",7.5);

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
  setVariable::StriplineBPMGenerator BPMGen;

  setVariable::PipeGenerator PGen;
  PGen.setNoWindow();
  PGen.setMat("Stainless316L","Stainless304L");

  setVariable::LinacQuadGenerator LQGen;
  setVariable::CorrectorMagGenerator CMGen;
  setVariable::YagUnitGenerator YagUnitGen;
  setVariable::YagScreenGenerator YagGen;

  const Geometry::Vec3D startPt(-637.608, 6808.791, 0.0);
  const Geometry::Vec3D endPt(-637.608, 6960.961, 0.0);

  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"XYAngle",0.0);

  setBellow26(Control,lKey+"BellowA",7.5);

  PGen.setCF<setVariable::CF8_TDC>();
  PGen.generatePipe(Control,lKey+"PipeA",34);

  // QG (QH) type quadrupole magnet
  LQGen.setRadius(0.56, 2.31);
  LQGen.generateQuad(Control,lKey+"Quad",18.0);
  // inner box half width/height
  Control.addVariable(lKey+"QuadYokeOuter",9.5);
  // adjusted so that nose is 1 cm thick as in the STEP file
  Control.addVariable(lKey+"QuadPolePitch",26.0);

  BPMGen.generateBPM(Control,lKey+"BPM",0.0);

  setBellow26(Control,lKey+"BellowB",7.5);

  PGen.setCF<setVariable::CF18_TDC>();
  PGen.generatePipe(Control,lKey+"PipeB",40.0);

  CMGen.generateMag(Control,lKey+"CMagHA",10.0,0); // guess
  CMGen.generateMag(Control,lKey+"CMagVA",29.0,1); // No_23_00.pdf

  YagUnitGen.generateYagUnit(Control,lKey+"YagUnit",true);
  Control.addVariable(lKey+"YagUnitPortRadius",1.7);
  Control.addVariable(lKey+"YagUnitPortThick",0.2);
  Control.addVariable(lKey+"YagUnitDepth",13.0); // measured
  Control.addVariable(lKey+"YagUnitHeight",5.9); // measured
  Control.addVariable(lKey+"YagUnitViewZStep",-3.2); // guess
  Control.addVariable(lKey+"YagUnitYAngle",90);

  YagGen.generateScreen(Control,lKey+"YagScreen",0);
  Control.addVariable(lKey+"YagScreenYAngle",-90.0);

  PGen.generatePipe(Control,lKey+"PipeC",6.5); // OK

  setCylGateValve(Control,lKey+"Gate",180.0,false);

  setBellow26(Control,lKey+"BellowC",7.5);

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
  setVariable::StriplineBPMGenerator BPMGen;

  setVariable::PipeGenerator PGen;
  PGen.setNoWindow();
  PGen.setCF<setVariable::CF18_TDC>();
  PGen.setMat("Stainless316L","Stainless304L");

  setVariable::LinacQuadGenerator LQGen;
  setVariable::CorrectorMagGenerator CMGen;
  setVariable::YagUnitGenerator YagUnitGen;
  setVariable::YagScreenGenerator YagGen;

  const Geometry::Vec3D startPt(-637.608, 6960.961, 0.0);
  const Geometry::Vec3D endPt(-637.608, 7406.261, 0.0);

  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"XYAngle",0.0);

  PGen.setOuterVoid();
  PGen.generatePipe(Control,lKey+"PipeA",325.8);

  setIonPump2Port(Control,lKey+"IonPump",0.0);

  setBellow26(Control,lKey+"Bellow",7.5);

  PGen.setOuterVoid(0);
  PGen.generatePipe(Control,lKey+"PipeB",40.0);

  CMGen.generateMag(Control,lKey+"CMagHA",10.0,0);
  CMGen.generateMag(Control,lKey+"CMagVA",28.0,1);

  BPMGen.generateBPM(Control,lKey+"BPM",0.0);

  PGen.setCF<setVariable::CF8_TDC>();
  PGen.generatePipe(Control,lKey+"PipeC",34);

  // QG (QH) type quadrupole magnet
  LQGen.setRadius(0.56, 2.31);
  LQGen.generateQuad(Control,lKey+"Quad",19.0);
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

  setVariable::PipeGenerator PGen;
  setVariable::TriPipeGenerator TPGen;
  setVariable::DipoleDIBMagGenerator DIBGen;
  setVariable::SixPortGenerator SPortGen;
  setVariable::BellowGenerator BellowGen;

  const Geometry::Vec3D startPt(-637.608,7406.261,0.0);

  const Geometry::Vec3D endPtA(-637.608,7618.484,0.0);
  const Geometry::Vec3D endPtB(-637.608,7612.436,-8.214);
  const Geometry::Vec3D endPtC(-637.608,7607.463,-15.805);

  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"OffsetA",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"OffsetB",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"OffsetC",startPt+linacVar::zeroOffset);

  Control.addVariable(lKey+"EndOffsetA",endPtA+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffsetB",endPtB+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffsetC",endPtC+linacVar::zeroOffset);


  Control.addVariable(lKey+"BackLinkA","backFlat");
  Control.addVariable(lKey+"BackLinkB","backMid");
  Control.addVariable(lKey+"BackLinkC","backLower");

  setBellow26(Control,lKey+"BellowA",7.5);

  const double dipoleAngle(0.8);
  const double magAngle(0.8);
  const double multiAngle(magAngle+dipoleAngle);

  const double startWidth(2.33/2.0);
  const double endWidth(6.70/2.0);
  TPGen.setBFlangeCF<CF100>();
  TPGen.setXYWindow(startWidth,startWidth,endWidth,endWidth);
  TPGen.generateTri(Control,lKey+"TriPipeA");
  Control.addVariable(lKey+"TriPipeALength",80.828); // No_25_00
  Control.addVariable(lKey+"TriPipeAYAngle",90);
  const double pipeAXAngle = -multiAngle;
  Control.addVariable(lKey+"TriPipeAXAngle",pipeAXAngle);

  DIBGen.generate(Control,lKey+"DipoleA");
  Control.addVariable(lKey+"DipoleAYStep",-0.4804); // No_25_00

  PGen.setCF<CF100>();
  PGen.setBFlangeCF<CF150>();
  PGen.setNoWindow();
  PGen.setMat("Stainless304L");
  PGen.setOuterVoid();

  PGen.generatePipe(Control,lKey+"PipeB",23.499); // No_25_00
  Control.addVariable(lKey+"PipeBRadius",5.0); // No_25_00
  Control.addVariable(lKey+"PipeBFeThick",0.2); // No_25_00
  Control.addVariable(lKey+"PipeBPreYAngle",-90);
  const double pipeBXAngle = multiAngle-2.83059;  // No_25_00
  Control.addVariable(lKey+"PipeBXAngle",pipeBXAngle);

  SPortGen.setCF<CF150>();
  SPortGen.generateSixPort(Control,lKey+"SixPortA");
  //  Control.addVariable(lKey+"SixPortAXAngle",multiAngle);
  Control.addVariable(lKey+"SixPortARadius",7.65); // No_25_00
  Control.addVariable(lKey+"SixPortAWallThick",0.3); // No_25_00
  Control.addVariable(lKey+"SixPortAFrontLength",16.7); // No_25_00
  Control.addVariable(lKey+"SixPortABackLength",16.7); // No_25_00

  // multipipe
  setVariable::MultiPipeGenerator MPGen;
  MPGen.setPipe<CF35_TDC>(Geometry::Vec3D(0,0,5.0), 50.820200, 0.0, -(pipeAXAngle+pipeBXAngle));
  MPGen.setPipe<CF35_TDC>(Geometry::Vec3D(0,0,0.0), 45.0957, 0.0, -0.1);
  MPGen.setPipe<CF35_TDC>(Geometry::Vec3D(0,0,-5.0), 40.662, 0.0, pipeAXAngle+pipeBXAngle-0.2);
  MPGen.generateMulti(Control,lKey+"MultiPipe");

  setBellow37(Control,lKey+"BellowAA");
  setBellow37(Control,lKey+"BellowBA");
  setBellow37(Control,lKey+"BellowCA");

  Control.addVariable(lKey+"BellowAABellowStep",4.0); // approx according to No_25_00
  Control.addVariable(lKey+"BellowBABellowStep",4.0); // approx according to No_25_00
  Control.addVariable(lKey+"BellowCABellowStep",4.0); // approx according to No_25_00

  return;
}


void
Segment26(FuncDataBase& Control,
	  const std::string& lKey)
  /*!
    Set the variables for segment 26
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{
  ELog::RegMethod RegA("linacVariables[F]","Segment26");

  setVariable::YagScreenGenerator YagScreenGen;
  setVariable::YagUnitGenerator YagUnitGen;

  const Geometry::Vec3D startPtA(-637.608,7618.484,0.0);
  const Geometry::Vec3D startPtB(-637.608,7612.436,-8.214);
  const Geometry::Vec3D startPtC(-637.608,7607.463,-15.805);

  const Geometry::Vec3D endPtA(-637.608,8173.261,0.0);
  const Geometry::Vec3D endPtB(-637.608,8180.263,-37.887);
  const Geometry::Vec3D endPtC(-637.608,8169.632,-73.976);

  Control.addVariable(lKey+"Offset",startPtA+linacVar::zeroOffset);
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

  setVariable::PipeGenerator PGen;
  PGen.setCF<CF35_TDC>();
  PGen.setNoWindow();
  PGen.setMat("Stainless304L");

  PGen.generatePipe(Control,lKey+"PipeAA",301.6);
  PGen.generatePipe(Control,lKey+"PipeBA",322.098);
  PGen.generatePipe(Control,lKey+"PipeCA",326.897);

  Control.addVariable(lKey+"PipeBAOffset",startPtB-startPtA);
  Control.addVariable(lKey+"PipeCAOffset",startPtC-startPtA);

  Control.addVariable(lKey+"PipeAAXAngle",
		      std::asin((endPtA-startPtA).unit()[2])*180.0/M_PI);
  const double pipeBAXAngle = std::asin((endPtB-startPtB).unit()[2])*180.0/M_PI;
  Control.addVariable(lKey+"PipeBAXAngle", pipeBAXAngle);
  Control.addVariable(lKey+"PipeCAXAngle",
		      std::asin((endPtC-startPtC).unit()[2])*180.0/M_PI);

  setBellow37(Control,lKey+"BellowAA",16.007);
  setBellow37(Control,lKey+"BellowBA",16.031);
  setBellow37(Control,lKey+"BellowCA",16.108);

  YagUnitGen.generateYagUnit(Control,lKey+"YagUnitA",true);
  Control.addVariable(lKey+"YagUnitAYAngle",90.0);

  YagScreenGen.generateScreen(Control,lKey+"YagScreenA",0);   // closed
  Control.addVariable(lKey+"YagScreenAYAngle",-90.0);

  YagUnitGen.generateYagUnit(Control,lKey+"YagUnitB",true);
  Control.addVariable(lKey+"YagUnitBYAngle",90.0);

  YagScreenGen.generateScreen(Control,lKey+"YagScreenB",0);   // closed
  Control.addVariable(lKey+"YagScreenBYAngle",-90.0);

  PGen.setCF<CF40>();
  PGen.setOuterVoid();
  PGen.generatePipe(Control,lKey+"PipeAB",217.2);
  PGen.generatePipe(Control,lKey+"PipeBB",210.473);
  PGen.generatePipe(Control,lKey+"PipeCB",222.207);

  // Local shielding walls
  setVariable::LocalShieldingGenerator ShieldGen;
  // SPF26ShieldA
  // http://localhost:8080/maxiv/work-log/tdc/pictures/spf-hall/tdc/img_5443.jpg/view
  // max length is 14 cm [img_5444.jpg]
  // max width is 99 cm [img_5445.jpg]
  // max height is 58 cm [img_5448.jpg]
  ShieldGen.setSize(14,99.0,58); // max dimensions
  ShieldGen.setMidHole(5.0,29.0); // approx
  ShieldGen.generate(Control,lKey+"ShieldA");
  Control.addVariable(lKey+"ShieldAYStep",20.0); // approx
  Control.addVariable(lKey+"ShieldAZStep",5.0); // approx
  Control.addVariable(lKey+"ShieldAXAngle",-pipeBAXAngle);

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

  setVariable::YagScreenGenerator YagScreenGen;
  setVariable::YagUnitGenerator YagUnitGen;
  setVariable::LBeamStopGenerator BSGen;

  const Geometry::Vec3D startPtA(-637.608,8173.261,0.0);
  const Geometry::Vec3D startPtB(-637.608,8180.263,-37.887);
  const Geometry::Vec3D startPtC(-637.608,8169.632,-73.976);

  const Geometry::Vec3D endPtA(-637.608,8458.411,0.0);
  const Geometry::Vec3D endPtB(-637.608,8458.379,-52.649);
  const Geometry::Vec3D endPtC(-637.608,8442.393,-101.956);

  Control.addVariable(lKey+"Offset",startPtA+linacVar::zeroOffset);
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

  setVariable::PipeGenerator PGen;
  PGen.setCF<CF35_TDC>();
  PGen.setNoWindow();
  PGen.setMat("Stainless304L");
  PGen.setOuterVoid();

  setBellow37(Control,lKey+"BellowAA",16.0);
  setBellow37(Control,lKey+"BellowBA",16.0);
  setBellow37(Control,lKey+"BellowCA",16.0);

  Control.addVariable(lKey+"BellowAAOffset");
  Control.addVariable(lKey+"BellowBAOffset",startPtB-startPtA);
  Control.addVariable(lKey+"BellowCAOffset",startPtC-startPtA);

  Control.addVariable(lKey+"BellowAAXAngle",
		      std::asin((endPtA-startPtA).unit()[2])*180.0/M_PI);
  Control.addVariable(lKey+"BellowBAXAngle",
		      std::asin((endPtB-startPtB).unit()[2])*180.0/M_PI);
  Control.addVariable(lKey+"BellowCAXAngle",
		      std::asin((endPtC-startPtC).unit()[2])*180.0/M_PI);

  PGen.generatePipe(Control,lKey+"PipeAA",217.2);
  PGen.generatePipe(Control,lKey+"PipeBA",210.5);
  PGen.generatePipe(Control,lKey+"PipeCA",222.2);

  setBellow37(Control,lKey+"BellowAB");
  setBellow37(Control,lKey+"BellowBB",16.008);
  setBellow37(Control,lKey+"BellowCB");

  YagUnitGen.generateYagUnit(Control,lKey+"YagUnitA",true);
  Control.addVariable(lKey+"YagUnitAYAngle",90.0);
  Control.addVariable(lKey+"YagUnitAFrontLength",12.95); // No_27_00

  YagScreenGen.generateScreen(Control,lKey+"YagScreenA",0);
  Control.addVariable(lKey+"YagScreenAYAngle",-90.0);

  YagUnitGen.generateYagUnit(Control,lKey+"YagUnitB",true);
  Control.addVariable(lKey+"YagUnitBYAngle",90.0);
  Control.addVariable(lKey+"YagUnitBFrontLength",13.008); // No_27_00

  YagScreenGen.generateScreen(Control,lKey+"YagScreenB",0);
  Control.addVariable(lKey+"YagScreenBYAngle",-90.0);

  YagUnitGen.generateYagUnit(Control,lKey+"YagUnitC",true);
  Control.addVariable(lKey+"YagUnitCYAngle",90.0);
  Control.addVariable(lKey+"YagUnitCFrontLength",12.993); // No_27_00

  YagScreenGen.generateScreen(Control,lKey+"YagScreenC",0);
  Control.addVariable(lKey+"YagScreenCYAngle",-90.0);

  setBellow37(Control,lKey+"BellowAC");
  setBellow37(Control,lKey+"BellowBC");

  BSGen.generateBStop(Control,lKey+"BeamStopC");
  Control.addVariable(lKey+"BeamStopCYStep",25.0);

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

  const Geometry::Vec3D startPtA(-637.608,8458.411,0.0);
  const Geometry::Vec3D startPtB(-637.608,8458.379,-52.649);

  const Geometry::Vec3D endPtA(-637.608,9073.611,0.0);
  const Geometry::Vec3D endPtB(-637.608,9073.535,-84.888);

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
  PGen.setOuterVoid();

  PGen.generatePipe(Control,lKey+"PipeAA",291.6);
  PGen.generatePipe(Control,lKey+"PipeBA",292.0);

  Control.addVariable(lKey+"PipeBAOffset",startPtB-startPtA);

  Control.addVariable(lKey+"PipeAAXAngle",
		      std::atan((endPtA-startPtA).unit()[2])*180.0/M_PI);
  Control.addVariable(lKey+"PipeBAXAngle",
		      std::atan((endPtB-startPtB).unit()[2])*180.0/M_PI);

  setBellow37(Control,lKey+"BellowAA");
  setBellow37(Control,lKey+"BellowBA");

  PGen.generatePipe(Control,lKey+"PipeAB",291.6);
  PGen.generatePipe(Control,lKey+"PipeBB",292.0);

  setBellow37(Control,lKey+"BellowAB");
  setBellow37(Control,lKey+"BellowBB");


  return;
}

void
Segment29(FuncDataBase& Control,
	  const std::string& lKey)
  /*!
    Set the variables for segment 29
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{

  ELog::RegMethod RegA("linacVariables[F]","Segment29");

  Segment29Magnet(Control,lKey);

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
  setVariable::CorrectorMagGenerator CMGen;
  setVariable::IonPTubeGenerator IonTGen;


  const Geometry::Vec3D startPt(-609.286, 3969.122, 0.0);
  const Geometry::Vec3D endPt(-827.249, 4928.489, 0.0);
  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"XYAngle",
		      atan((startPt.X()-endPt.X())/(endPt.Y()-startPt.Y()))*180.0/M_PI);

  const Geometry::Vec3D OPos(0,0.0,0);
  const Geometry::Vec3D XVec(1,0,0);

  // Gauge
  setGauge37(Control,lKey+"Gauge");
  Control.addVariable(lKey+"GaugeYAngle",0.0);

  PGen.setCF<setVariable::CF18_TDC>();
  PGen.setMat("Stainless316L","Stainless304L");
  PGen.setNoWindow();
  PGen.generatePipe(Control,lKey+"PipeA",436.5);

  setBellow26(Control,lKey+"Bellow",7.5);

  // IonPump
  setIonPump2Port(Control, lKey+"IonPump", 90.0); // TODO: actually, 1 port ion pump

  // CMagV
  PGen.generatePipe(Control,lKey+"PipeB",511.23);
  CMGen.generateMag(Control,lKey+"CMagVA",500.13,0);

  // Placeholder for the local shielding wall
  // Part of L2SPF13ShieldA
  // http://localhost:8080/maxiv/work-log/tdc/pictures/spf-hall/tdc/img_5421.jpg/view
  setVariable::LocalShieldingGenerator ShieldGen;
  // max length is 14 cm [img_5420]
  ShieldGen.setSize(10,30,25); // length is 10, height/width arbitrary since it's a placeholder
  ShieldGen.setMidHole(4.0,4.0); // dummy
  ShieldGen.generate(Control,lKey+"ShieldA");
  Control.addVariable(lKey+"ShieldAYStep",9.2); // IMG_5421.JPG
  Control.addVariable(lKey+"ShieldAXStep",-10.0); // to avoid cutting SPF30
  Control.addVariable(lKey+"ShieldAZStep",0.0); // dummy
  Control.addVariable(lKey+"ShieldAMainMat","Void"); // placeholder


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
  setVariable::CorrectorMagGenerator CMGen;
  setVariable::StriplineBPMGenerator BPMGen;
  setVariable::LinacQuadGenerator LQGen;
  setVariable::IonPTubeGenerator IonTGen;

  const Geometry::Vec3D startPt(-827.249, 4928.489, 0.0);
  const Geometry::Vec3D endPt  (-921.651, 5344.0, 0.0);

  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"XYAngle",
		      atan((startPt.X()-endPt.X())/(endPt.Y()-startPt.Y()))*180.0/M_PI);

  // BellowA
  setBellow26(Control,lKey+"BellowA",7.5);

  // IonPumpA

  setIonPump2Port(Control,lKey+"IonPumpA",0.0);

  setCylGateValve(Control,lKey+"Gate",0.0,false);

  setBellow26(Control,lKey+"BellowB",7.5);

  BPMGen.generateBPM(Control,lKey+"BPM",0.0);

  // PipeA and Quadrupole
  PGen.setCF<setVariable::CF18_TDC>();
  PGen.setMat("Stainless316L","Stainless304L");
  PGen.generatePipe(Control,lKey+"PipeA",42.5);

  // QF type quadrupole magnet
  LQGen.generateQuad(Control,lKey+"Quad",19.7);
  // inner box half width/height
  Control.addVariable(lKey+"QuadYokeOuter",9.5);

  setBellow26(Control,lKey+"BellowC",7.5);

  PGen.generatePipe(Control,lKey+"PipeB",232.7);
  CMGen.generateMag(Control,lKey+"CMagHA",24.7,0);

  // IonPumpB
  //  setIonPump2Port(Control,lKey+"IonPumpB", -90);
  IonTGen.setCF<setVariable::CF63>();
  IonTGen.setMainLength(10.0,10.0);
  IonTGen.setWallThick(0.2);      // No_17_00.pdf
  IonTGen.setRadius(3.3);         // No_17_00.pdf
  IonTGen.setWallThick(0.2);      // No_17_00.pdf
  IonTGen.setVertical(11.9,13.9);  // d / h
  IonTGen.generateTube(Control,lKey+"IonPumpB");
  Control.addVariable(lKey+"IonPumpBYAngle",-90.0);

  PGen.setOuterVoid();
  PGen.generatePipe(Control,lKey+"PipeC",55.7);

  setBellow26(Control,lKey+"BellowD",7.5);

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

  const Geometry::Vec3D startPt(-921.651, 5344.000, 0.0);
  const Geometry::Vec3D endPt  (-965.763, 5607.319, 0.0);

  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"XYAngle",11.2);  // from SPF32FlatA coordinates @ No_32_34_00.pdf
  Control.addVariable(lKey+"EndAngle",6.40);  // from drawing coord (seg33)

  Segment32Magnet(Control,lKey);

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
  setVariable::StriplineBPMGenerator BPMGen;
  setVariable::LinacQuadGenerator LQGen;
  setVariable::CorrectorMagGenerator CMGen;
  setVariable::LinacSexuGenerator LSGen;
  setVariable::YagUnitGenerator YagUnitGen;
  setVariable::YagScreenGenerator YagScreenGen;

  const Geometry::Vec3D startPt(-965.763,5607.319,0.0);
  const Geometry::Vec3D endPt(-995.514,5872.556,0.0);

  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"XYAngle",
		      atan((startPt.X()-endPt.X())/(endPt.Y()-startPt.Y()))*180.0/M_PI);

  PGen.setCF<setVariable::CF18_TDC>();
  PGen.setMat("Stainless316L","Stainless304L");
  PGen.setNoWindow();
  PGen.generatePipe(Control,lKey+"PipeA",67.0);

  CMGen.generateMag(Control,lKey+"CMagHA",56.7,0);

  BPMGen.generateBPM(Control,lKey+"BPMA",0.0); // 22 cm length OK

  PGen.generatePipe(Control,lKey+"PipeB",81.7);
  LQGen.generateQuad(Control,lKey+"QuadA",19.7);
  LSGen.generateSexu(Control,lKey+"SexuA",40.7);
  LQGen.generateQuad(Control,lKey+"QuadB",61.7);

  YagUnitGen.generateYagUnit(Control,lKey+"YagUnit"); // length 20.2 != 20
  YagScreenGen.generateScreen(Control,lKey+"YagScreen",0);   // open
  Control.addVariable(lKey+"YagUnitYAngle",90.0);
  Control.addVariable(lKey+"YagScreenYAngle",-90.0);

  PGen.generatePipe(Control,lKey+"PipeC",68.7);
  Control.addVariable(lKey+"PipeCYAngle",90.0);
  CMGen.generateMag(Control,lKey+"CMagVA",11.0,1);

  setBellow26(Control,lKey+"Bellow",7.5);

  // Local shielding walls
  setVariable::LocalShieldingGenerator ShieldGen;
  // SPF33ShieldA
  // http://localhost:8080/maxiv/work-log/tdc/pictures/spf-hall/img_5388.mp4/view
  ShieldGen.setSize(10,80,35);
  ShieldGen.setMidHole(3.0,5.0);
  ShieldGen.generate(Control,lKey+"ShieldA");
  Control.addVariable(lKey+"ShieldAYStep",14.0);

  // SPF33ShieldB
  // http://localhost:8080/maxiv/work-log/tdc/pictures/spf-hall/spf/img_5433.jpg/view
  ShieldGen.setSize(10,60,20);
  ShieldGen.setMidHole(3.0,5.0);
  ShieldGen.generate(Control,lKey+"ShieldB");
  Control.addVariable(lKey+"ShieldBYStep",40.0);

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

  const Geometry::Vec3D startPt(-995.514,5872.556,0.0);
  const Geometry::Vec3D endPt(-1010.0,6139.149,0.0);


  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"XYAngle",4.8);  // adjusted to get correct endPt

  Segment32Magnet(Control,lKey);

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
  setVariable::ButtonBPMGenerator BPMGen;

  const Geometry::Vec3D startPt(-1010.0,6139.149,0.0);
  const Geometry::Vec3D endPt(-1010.0,6310.949,0.0);

  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"XYAngle",
		      atan((startPt.X()-endPt.X())/(endPt.Y()-startPt.Y()))*180.0/M_PI);

  YagUnitGen.generateYagUnit(Control,lKey+"YagUnit");
  Control.addVariable(lKey+"YagUnitYAngle",90.0);

  YagScreenGen.generateScreen(Control,lKey+"YagScreen",0);
  Control.addVariable(lKey+"YagScreenYAngle",-90.0);
  Control.addVariable(lKey+"YagScreenZStep",-3.3);

  PGen.setCF<setVariable::CF18_TDC>();
  PGen.setMat("Stainless316L","Stainless304L");
  PGen.setNoWindow();
  PGen.generatePipe(Control,lKey+"PipeA",37.5);

  LQGen.generateQuad(Control,lKey+"QuadA",18.7);

  BPMGen.generate(Control,lKey+"BPM");

  PGen.generatePipe(Control,lKey+"PipeB",75.15);
  LQGen.generateQuad(Control,lKey+"QuadB",16.15);
  CMGen.generateMag(Control,lKey+"CMagHA",41.65,1);
  CMGen.generateMag(Control,lKey+"CMagVA",60.15,0);

  setMirrorChamberBlank(Control, lKey+"MirrorChamber",-90.0);

  PGen.setCF<setVariable::CF37_TDC>();
  PGen.setOuterVoid();
  PGen.generatePipe(Control,lKey+"PipeC",12.6);
  Control.addVariable(lKey+"PipeCFeMat", "Stainless304L");

  setBellow26(Control,lKey+"Bellow",7.5);

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

  setVariable::PipeGenerator PGen;
  setVariable::LinacQuadGenerator LQGen;
  setVariable::CorrectorMagGenerator CMGen;
  setVariable::StriplineBPMGenerator BPMGen;
  setVariable::EArrivalMonGenerator EArrGen;

  const Geometry::Vec3D startPt(-1010.0,6310.949,0.0);
  const Geometry::Vec3D endPt(-1010.0,6729.589,0.0);

  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"XYAngle",
		      atan((startPt.X()-endPt.X())/(endPt.Y()-startPt.Y()))*180.0/M_PI);

  setGauge34(Control,lKey+"Gauge");

  // Pipes
  PGen.setCF<setVariable::CF18_TDC>();
  PGen.setMat("Stainless316L","Stainless304L");
  PGen.setNoWindow();
  PGen.generatePipe(Control,lKey+"PipeA",146.84);
  PGen.setOuterVoid();
  PGen.generatePipe(Control,lKey+"PipeB",98.69);
  PGen.generatePipe(Control,lKey+"PipeC",30.0);
  PGen.generatePipe(Control,lKey+"PipeD",33.3);
  PGen.generatePipe(Control,lKey+"PipeE",35.0);

  // magnet locations based on front surfaces of yokes
  // Quadrupole magnets
  LQGen.generateQuad(Control,lKey+"QuadA",18.79);
  LQGen.generateQuad(Control,lKey+"QuadB",128.54);

  // Corrector magnets
  CMGen.generateMag(Control,lKey+"CMagHA",43.04,0);
  // Control.addVariable(lKey+"CMagHMagInnerLength",10.3); // 11.5
  // Control.addVariable(lKey+"CMagHMagLength",14.2-1.2); //
  CMGen.generateMag(Control,lKey+"CMagVA",61.54,1);

  // Beam position monitors
  BPMGen.generateBPM(Control,lKey+"BPMA",0.0);
  BPMGen.generateBPM(Control,lKey+"BPMB",0.0);

  // Beam arrival monitor
  EArrGen.setCF<setVariable::CF18_TDC>();
  EArrGen.generateMon(Control,lKey+"BeamArrivalMon",0.0);
  Control.addVariable(lKey+"BeamArrivalMonLength",4.75);

  // Gate
  setCylGateValve(Control,lKey+"Gate",180.0,false);  // length 7.3 cm checked

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

  setVariable::CeramicGapGenerator CSGen;
  setVariable::EBeamStopGenerator EBGen;
  setVariable::PipeGenerator PGen;

  const Geometry::Vec3D startPt(-1010.0,6729.589,0.0);
  const Geometry::Vec3D endPt(-1010.0,6825.849,0.0);
  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"XYAngle",
  		      atan((startPt.X()-endPt.X())/(endPt.Y()-startPt.Y()))*180.0/M_PI);

  CSGen.generateCeramicGap(Control,lKey+"CeramicA");
  Control.addVariable(lKey+"CeramicARadius",0.9);
  Control.addVariable(lKey+"CeramicAPipeThick",0.2);
  Control.addVariable(lKey+"CeramicALength",13.97); // No_37_00

  CSGen.generateCeramicGap(Control,lKey+"CeramicB");
  Control.addVariable(lKey+"CeramicBRadius",0.9);
  Control.addVariable(lKey+"CeramicBPipeThick",0.2);

  EBGen.generateEBeamStop(Control,lKey+"BeamStop",0);
  Control.addVariable(lKey+"BeamStopLength",39.2); // adjusted to have correct length according to No_37

  PGen.setCF<setVariable::CF18_TDC>();
  PGen.setNoWindow();

  PGen.setOuterVoid();
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
  ELog::RegMethod RegA("linacVariables[F]","Segment38");
  setVariable::PipeGenerator PGen;

  const Geometry::Vec3D startPt(-1010.0,6825.849,0.0);
  const Geometry::Vec3D endPt(-1010.0,7355.379,0.0);
  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"XYAngle",
  		      atan((startPt.X()-endPt.X())/(endPt.Y()-startPt.Y()))*180.0/M_PI);

  PGen.setCF<setVariable::CF16_TDC>();
  PGen.setMat("Aluminium","Aluminium");
  PGen.setNoWindow();
  PGen.setOuterVoid();
  PGen.generatePipe(Control,lKey+"PipeA",285.0); // No_38_00
  Control.addVariable(lKey+"PipeAFeThick",0.2);
  PGen.setCF<setVariable::CF34_TDC>();
  PGen.setMat("Stainless316L","Stainless304L");
  PGen.generatePipe(Control,lKey+"PipeB",221.0); // No_38_00


  setIonPump2Port(Control,lKey+"IonPump",90.0);

  setBellow26(Control,lKey+"BellowA",7.5);

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
  ELog::RegMethod RegA("linacVariables[F]","Segment39");

  const Geometry::Vec3D startPt(-1010.0,7355.379,0.0);
  const Geometry::Vec3D endPt(-1010.0,7449.099,0.0);

  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"XYAngle",
  		      atan((startPt.X()-endPt.X())/(endPt.Y()-startPt.Y()))*180.0/M_PI);

  // Stripline BPM
  setVariable::StriplineBPMGenerator BPMGen;
  BPMGen.generateBPM(Control,lKey+"BPM",0.0);

  // Yag screen and its unit
  setVariable::YagUnitBigGenerator YagUnitGen;
  YagUnitGen.generateYagUnit(Control,lKey+"YagUnit");
  Control.addVariable(lKey+"YagUnitYAngle",90.0);

  setVariable::YagScreenGenerator YagScreenGen;
  YagScreenGen.generateScreen(Control,lKey+"YagScreen",0);
  Control.addVariable(lKey+"YagScreenYAngle",-90.0);
  Control.addVariable(lKey+"YagScreenZStep",-3.3);

  // Gate
  setCylGateValve(Control,lKey+"Gate",90.0,false);

  // Pipe
  setVariable::PipeGenerator PGen;
  PGen.setCF<setVariable::CF40_22>();
  PGen.setOuterVoid();
  PGen.setMat("Stainless316L","Stainless304L");
  PGen.setNoWindow();
  PGen.generatePipe(Control,lKey+"Pipe",37.05);

  setBellow26(Control,lKey+"Bellow",7.5);

  return;
}

void
Segment40(FuncDataBase& Control,const std::string& lKey)
  /*!
    Set the variables for SPF segment 40
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{
  ELog::RegMethod RegA("linacVariables[F]","Segment40");

  setVariable::UndVacGenerator UVGen;
  setVariable::FMUndulatorGenerator UUGen;

  // SPF40
  const Geometry::Vec3D startPt(-1010.0,7449.099,0.0);
  const Geometry::Vec3D endPt(-1010.0,7971.099,0.0);


  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"XYAngle",
  		      atan((startPt.X()-endPt.X())/
			   (endPt.Y()-startPt.Y()))*180.0/M_PI);

  UVGen.generateUndVac(Control,lKey+"UVac");
  UUGen.generateUndulator(Control,lKey+"UVacUndulator",482.0);
  Control.addVariable(lKey+"UVacRadius",12.3);
  Control.addVariable(lKey+"UVacWallThick",0.4);

  return;
}

void
Segment41(FuncDataBase& Control,
		   const std::string& lKey)
  /*!
    Set the variables for SPF segment 41
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{
  ELog::RegMethod RegA("linacVariables[F]","Segment41");

  const Geometry::Vec3D startPt(-1010.0,7971.099,0.0);
  const Geometry::Vec3D endPt(-1010.0,8053.349,0.0);

  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"XYAngle",
  		      atan((startPt.X()-endPt.X())/(endPt.Y()-startPt.Y()))*180.0/M_PI);

  // Bellows
  setBellow26(Control,lKey+"BellowA",7.5);
  setBellow26(Control,lKey+"BellowB",7.5);

  // Stripline BPM
  setVariable::StriplineBPMGenerator BPMGen;
  BPMGen.generateBPM(Control,lKey+"BPM",0.0);

  // Gate
  setCylGateValve(Control,lKey+"Gate",-90.0,false);

  // Pipe
  setVariable::PipeGenerator PGen;
  PGen.setCF<setVariable::CF18_TDC>();
  PGen.setOuterVoid();
  PGen.setMat("Stainless316L","Stainless304L");
  PGen.setNoWindow();
  PGen.generatePipe(Control,lKey+"Pipe",38.05);

  return;
}

void
Segment42(FuncDataBase& Control,const std::string& lKey)
  /*!
    Set the variables for SPF segment 42
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{
  ELog::RegMethod RegA("linacVariables[F]","Segment40");

  setVariable::UndVacGenerator UVGen;
  setVariable::FMUndulatorGenerator UUGen;

  // SPF39
  const Geometry::Vec3D startPt(-1010.0,8053.349,0.0);
  const Geometry::Vec3D endPt(-1010.0,8575.349,0.0);


  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"XYAngle",
  		      atan((startPt.X()-endPt.X())/
			   (endPt.Y()-startPt.Y()))*180.0/M_PI);


  UVGen.generateUndVac(Control,lKey+"UVac");
  UUGen.generateUndulator(Control,lKey+"UVacUndulator",482.0);
  Control.addVariable(lKey+"UVacRadius",12.3);
  Control.addVariable(lKey+"UVacWallThick",0.4);

  return;
}

void
Segment43(FuncDataBase& Control,
		   const std::string& lKey)
  /*!
    Set the variables for SPF segment 43
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{
  ELog::RegMethod RegA("linacVariables[F]","Segment43");

  const Geometry::Vec3D startPt(-1010.0,8575.349,0.0);
  const Geometry::Vec3D endPt(-1010.0,8682.445,0.0);

  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"XYAngle", 0.0);

  // Stripline BPM
  setVariable::StriplineBPMGenerator BPMAGen;
  BPMAGen.generateBPM(Control,lKey+"BPMA",0.0);

  // Yag screen and its unit
  setVariable::YagUnitBigGenerator YagUnitGen;
  YagUnitGen.generateYagUnit(Control,lKey+"YagUnit");
  Control.addVariable(lKey+"YagUnitYAngle",90.0);

  setVariable::YagScreenGenerator YagScreenGen;
  YagScreenGen.generateScreen(Control,lKey+"YagScreen",0);
  Control.addVariable(lKey+"YagScreenYAngle",-90.0);
  Control.addVariable(lKey+"YagScreenZStep",-3.3);

  // Gate
  setCylGateValve(Control,lKey+"Gate",90.0,false);

  // Pipe
  setVariable::PipeGenerator PGen;
  PGen.setCF<setVariable::CF18_TDC>();
  PGen.setMat("Stainless316L","Stainless304L");
  PGen.setNoWindow();
  PGen.generatePipe(Control,lKey+"Pipe",39.9);

  setVariable::CorrectorMagGenerator CMGen;
  CMGen.generateMag(Control,lKey+"CMagHA",9.74,0);

  // Button pickup PBM
  setVariable::ButtonBPMGenerator BPMBGen;
  BPMBGen.setCF<setVariable::CF40_22>();
  BPMBGen.generate(Control,lKey+"BPMB");
  Control.addVariable(lKey+"BPMBLength",2.996); // No_43_00
  Control.addVariable(lKey+"BPMBNButtons",2);
  Control.addVariable(lKey+"BPMBButtonYAngle",0.0);
  Control.addVariable(lKey+"BPMBFlangeGap",0.0);
  Control.addVariable(lKey+"BPMBFlangeALength",0.5); // approx
  Control.addVariable(lKey+"BPMBFlangeBLength",0.5); // approx

  setBellow26(Control,lKey+"BellowA",7.5);
  setBellow26(Control,lKey+"BellowB",7.5);

  return;
}

void
Segment44(FuncDataBase& Control,const std::string& lKey)
  /*!
    Set the variables for SPF segment 44.
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{
  ELog::RegMethod RegA("linacVariables[F]","Segment44");

  // Note that the viewport branch is not an end point and nothing
  // currently connects.

  const Geometry::Vec3D startPt(-1010.0,8682.445,0.0);
  const Geometry::Vec3D endPtA(-1010.0,8825.445,0.0);
  const Geometry::Vec3D endPtB(-1010.0,8949.717,-60.951);

  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPtA+linacVar::zeroOffset);
  Control.addVariable(lKey+"XYAngle",
  		      atan((startPt.X()-endPtA.X())/
			   (endPtA.Y()-startPt.Y()))*180.0/M_PI);

  Control.addVariable(lKey+"OffsetA",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"OffsetB",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffsetA",endPtA+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffsetB",endPtB+linacVar::zeroOffset);

  Control.addVariable(lKey+"FrontLinkA","front");
  Control.addVariable(lKey+"BackLinkA","straightExit");
  Control.addVariable(lKey+"FrontLinkB","front");
  Control.addVariable(lKey+"BackLinkB","magnetExit");

  setVariable::TriGroupGenerator TGGen;
  setVariable::CurveMagGenerator CMagGen;

  // 267.75 / 168.0
  TGGen.setBend(313.40,110.4,58.46);
  TGGen.generateTri(Control,lKey+"TriBend");
  // \todo: UGLY FIX to avoid clipping with the bended pipe:
  Control.addVariable(lKey+"TriBendMidFlangeRadius",4.0);

  CMagGen.generateMag(Control,lKey+"CMag");
  Control.addVariable(lKey+"CMagYStep",9.0);
  Control.addVariable(lKey+"CMagZStep",-22.8);
  Control.addVariable(lKey+"CMagXAngle",16.0);

  return;
}

void
Segment45(FuncDataBase& Control,
		   const std::string& lKey)
  /*!
    Set the variables for SPF segment 45
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{
  ELog::RegMethod RegA("linacVariables[F]","Segment45");

  const Geometry::Vec3D startPt(-1010.0,8949.717,-60.951);
  const Geometry::Vec3D endPt(-1010.0,9227.315,-190.289);

  Geometry::Vec3D AB=(endPt-startPt).unit();

  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);
  Control.addVariable
    (lKey+"XYAngle",
     atan((startPt.X()-endPt.X())/(endPt.Y()-startPt.Y()))*180.0/M_PI);
  Control.addVariable
    (lKey+"XAngle",
     atan((endPt.Z()-startPt.Z())/(endPt.Y()-startPt.Y()))*180.0/M_PI);

  // floor gap
  Control.addVariable(lKey+"CutRadius",20.0);

  setVariable::CeramicGapGenerator CSGen;
  setVariable::YagUnitBigGenerator YagUnitGen;
  setVariable::YagScreenGenerator YagScreenGen;
  setVariable::FlangePlateGenerator FPGen;
  setVariable::PipeGenerator PGen;
  PGen.setMat("Stainless304L","Stainless304L");
  PGen.setNoWindow();
  PGen.setOuterVoid();

  CSGen.generateCeramicGap(Control,lKey+"Ceramic");

  PGen.setCF<setVariable::CF34_TDC>();
  PGen.generatePipe(Control,lKey+"PipeA",110.5);

  YagUnitGen.generateYagUnit(Control,lKey+"YagUnit");
  Control.addVariable(lKey+"YagUnitYAngle",90.0);

  YagScreenGen.generateScreen(Control,lKey+"YagScreen",0);
  Control.addVariable(lKey+"YagScreenYAngle",-90.0);
  Control.addVariable(lKey+"YagScreenZStep",-3.3);

  PGen.setCF<setVariable::CF66_TDC>();
  PGen.generatePipe(Control,lKey+"PipeB",160.0);

  FPGen.setCF<setVariable::CF63>(YagUnitGen.getPortRadius());
  FPGen.setFlangeLen(1.75);
  FPGen.generateFlangePlate(Control,lKey+"Adaptor");

  // additional stuff for beam dump - not present in the original
  // drawings
  PGen.generatePipe(Control,lKey+"PipeC",100.0); // approx
  Control.addVariable(lKey+"PipeCYAngle",-90);
  Control.addVariable(lKey+"PipeCFlangeFrontRadius",4.5); // to avoid cutting EBeam
  Control.addVariable(lKey+"PipeCFlangeBackRadius",4.5); // to avoid cutting EBeam
  setVariable::EBeamStopGenerator EBGen;
  EBGen.generateEBeamStop(Control,lKey+"EBeam",1);
  Control.addVariable(lKey+"EBeamShieldActive",1);
  Control.addVariable(lKey+"EBeamShieldInnerMat","Stainless304L"); // email from JR, 210120: "Iron"

  return;
}

void
Segment46(FuncDataBase& Control,
		   const std::string& lKey)
  /*!
    Set the variables for SPF segment 46
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{
  ELog::RegMethod RegA("linacVariables[F]","Segment46");

  // SPF46
  const Geometry::Vec3D startPt(-1010.0, 8825.445, 0.0);
  const Geometry::Vec3D   endPt(-1010.0, 9105.245, 0.0);

  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);
  Control.addVariable
    (lKey+"XYAngle",atan((startPt.X()-endPt.X())/
			 (endPt.Y()-startPt.Y()))*180.0/M_PI);

  // Pipes
  setVariable::PipeGenerator PGen;
  setVariable::CrossWayGenerator MSPGen;

  PGen.setNoWindow();

  MSPGen.setPortLength(6.3,6.3);
  //  MSPGen.setCF<CF35_TDC>();

  PGen.setCF<setVariable::CF35_TDC>();
  PGen.setMat("Stainless304L","Stainless304L");
  PGen.generatePipe(Control,lKey+"PipeA",96.8);

  setVariable::LocalShieldingGenerator LSGen;
  const double tmp = 4.0; // cut height to avoid overlap with SPF44
  LSGen.setSize(10,60,40-tmp);
  LSGen.setMidHole(10, 5);
  LSGen.setCorner(10, 5);
  LSGen.generate(Control,lKey+"ShieldA");
  Control.addVariable(lKey+"ShieldAYStep",89.0);
  Control.addVariable(lKey+"ShieldAZStep",-7.5+tmp/2.0);

  const double pipeBLength(40.0);
  PGen.setCF<setVariable::CF16_TDC>();
  PGen.setMat("Aluminium","Aluminium");
  PGen.generatePipe(Control,lKey+"PipeB",pipeBLength);
  Control.addVariable(lKey+"PipeBFeThick",0.15);
  Control.addVariable(lKey+"PipeBYAngle",-90.0);

  // Gate valves
  setVariable::CylGateValveGenerator CGateGen;
  CGateGen.setYRotate(180.0);
  CGateGen.setWallThick(0.3);
  CGateGen.setPortThick(0.10);
  CGateGen.generateGate(Control,lKey+"GateA",0);

  setBellow26(Control,lKey+"BellowA",7.5);

  // Prisma Chamber
  setVariable::PrismaChamberGenerator PCGen;
  PCGen.generateChamber(Control, lKey+"PrismaChamber");
  Control.addVariable(lKey+"PrismaChamberYAngle",180.0);

  // Mirror Chambers

  MSPGen.generateCrossWay(Control,lKey+"MirrorChamberA");
  Control.addVariable(lKey+"MirrorChamberAYAngle",90.0);

  // Cleaning magnet
  setVariable::CleaningMagnetGenerator ClMagGen;
  ClMagGen.generate(Control,lKey+"CleaningMagnet");
  Control.addVariable(lKey+"CleaningMagnetYStep",pipeBLength/2.0);

  // Slit tube and jaws
  setSlitTube(Control,lKey+"SlitTube",19.91);

  setBellow26(Control,lKey+"BellowB",10.0); // measured

  const double mcbLength(12.59);
  MSPGen.generateCrossWay(Control,lKey+"MirrorChamberB");
  Control.addVariable(lKey+"MirrorChamberBYAngle",90.0);
  Control.addVariable(lKey+"MirrorChamberBBackLength",mcbLength/2.0);
  Control.addVariable(lKey+"MirrorChamberBFrontLength",mcbLength/2.0);

  setBellow26(Control,lKey+"BellowC",10.0); // measured

  CGateGen.generateGate(Control,lKey+"GateB",0);
  Control.addVariable(lKey+"GateBYAngle",90.0);

  // Bellow D added: it is not on the drawing BUT the
  // error in lengths is 160mm and there should be a bellow
  // here.
  setBellow26(Control,lKey+"BellowD",16.0);

  return;
}

void
Segment47(FuncDataBase& Control,
		   const std::string& lKey)
  /*!
    Set the variables for SPF segment 49
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{
  ELog::RegMethod RegA("linacVariables[F]","Segment47");

  // SPF47
  const Geometry::Vec3D startPt(-1010.0,9105.245,0.0);
  const Geometry::Vec3D endPt(-1010.0,9327.140,0.0);

  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"XYAngle",
  		      atan((startPt.X()-endPt.X())/(endPt.Y()-startPt.Y()))*180.0/M_PI);

  setVariable::PipeGenerator PGen;
  PGen.setMat("Stainless304L","Stainless304L");
  PGen.setNoWindow();
  PGen.setOuterVoid();

  PGen.setCF<setVariable::CF35_TDC>();
  PGen.generatePipe(Control,lKey+"PipeA",87.4); // measured

  // Prisma Chamber
  setPrismaChamber(Control, lKey+"PrismaChamberA",180.0);

  // Mirror Chambers [note that 90 degree rotation carried through]
  setMirrorChamber(Control, lKey+"MirrorChamberA",90.0);

  PGen.setCF<setVariable::CF37_TDC>();
  PGen.generatePipe(Control,lKey+"PipeB",12.6); // measured


  setMirrorChamber(Control, lKey+"MirrorChamberB",0.0);

  PGen.generatePipe(Control,lKey+"PipeC",12.6); // measured

  setMirrorChamber(Control, lKey+"MirrorChamberC",0.0);

  PGen.setCF<setVariable::CF35_TDC>();
  PGen.generatePipe(Control,lKey+"PipeD",8.4); // measured
  Control.addVariable(lKey+"PipeDYAngle",90);

    // Gate valves
  setCylGateValve(Control,lKey+"GateA",180.0,false);


  setBellow26(Control,lKey+"BellowA",7.5);

  PGen.generatePipe(Control,lKey+"PipeE",8.4); // measured

  // Local shielding wall
  // http://localhost:8080/maxiv/work-log/tdc/pictures/spf-hall/spf/img_5457.jpg/view
  setVariable::LocalShieldingGenerator LSGen;
  LSGen.setSize(10.0,60,30.0);
  LSGen.generate(Control,lKey+"ShieldA");
  Control.addVariable(lKey+"ShieldAXStep",62.5);
  Control.addVariable(lKey+"ShieldAZStep",-10.0);
  Control.addVariable(lKey+"ShieldAYStep",1.1);

  // Mirror camera shield
  // http://localhost:8080/maxiv/work-log/tdc/pictures/spf-hall/spf/img_5457.jpg/view
  // http://localhost:8080/maxiv/work-log/tdc/pictures/spf-hall/spf/img_5384.jpg/view
  setVariable::SPFCameraShieldGenerator CSGen;
    //  CSGen.setSize(45.0,5,20.0);
  CSGen.generate(Control,lKey+"ShieldB");
  Control.addVariable(lKey+"ShieldBZStep",30.0);
  Control.addVariable(lKey+"ShieldBYAngle",90.0);
  Control.addVariable(lKey+"ShieldBYStep",-7.0);
  Control.addVariable(lKey+"ShieldBXStep",-7.5);
  return;
}

void
Segment48(FuncDataBase& Control,
		   const std::string& lKey)
  /*!
    Set the variables for SPF segment 49
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{
  ELog::RegMethod RegA("linacVariables[F]","Segment48");

  // SPF48
  const Geometry::Vec3D startPt(-1010.0,9327.140,0.0);
  const Geometry::Vec3D endPt(-1010.0,9495.745,0.0);


  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"XYAngle",
  		      atan((startPt.X()-endPt.X())/(endPt.Y()-startPt.Y()))*180.0/M_PI);

  setVariable::EBeamStopGenerator EBGen;
  setVariable::PipeGenerator PGen;

  PGen.setCF<setVariable::CF37_TDC>();
  PGen.setMat("Stainless304L","Stainless304L");
  PGen.setNoWindow();

  EBGen.generateEBeamStop(Control,lKey+"BeamStopA",0);
  setBellow26(Control,lKey+"BellowA",7.5);

  EBGen.generateEBeamStop(Control,lKey+"BeamStopB",0);

  PGen.generatePipe(Control,lKey+"PipeA",12.6); // measured

  // Slit tube and jaws
  setSlitTube(Control,lKey+"SlitTube",19.91);

  setBellow26(Control,lKey+"BellowB",10.0); // measured
  setBellow26(Control,lKey+"BellowC",10.0); // measured

  // Mirror Chamber
  setMirrorChamber(Control, lKey+"MirrorChamberA",90.0);

  // Local shielding wall
  // http://localhost:8080/maxiv/work-log/tdc/pictures/spf-hall/img_5378.mp4/view
  // time: 12:00
  setVariable::LocalShieldingGenerator LSGen;
  const double shieldAHeight = 35.0;
  // Average length estimate (dimension along the beam line):
  //   total height is made of:
  //    3 bricks of 5 cm height (10 cm length): 5*3=15
  //    2 bricks of 10 cm height (5 cm length): 10*2=20
  const double shieldALength = (15*10+20*5)/shieldAHeight;
  const double shieldAZStep = -2.5;
  LSGen.setSize(shieldALength,40,shieldAHeight);
  LSGen.setMidHole(4.1,4.1);
  LSGen.setCorner(10.0,5.0);
  LSGen.generate(Control,lKey+"ShieldA");
  Control.addVariable(lKey+"ShieldAYStep",6.0);
  Control.addVariable(lKey+"ShieldAZStep",shieldAZStep);

  LSGen.setSize(20.0,10.0,15.0);
  LSGen.setMidHole(0.0,0.0);
  LSGen.setCorner(0.0,0.0);
  LSGen.generate(Control,lKey+"ShieldB");
  Control.addVariable(lKey+"ShieldBXStep",1.5);
  Control.addVariable(lKey+"ShieldBYStep",10.0);

  const double shieldCWidth = 130;
  LSGen.setSize(shieldCWidth,5,20);
  LSGen.generate(Control,lKey+"ShieldC");
  Control.addVariable(lKey+"ShieldCYStep",shieldCWidth/2.0);
  Control.addVariable(lKey+"ShieldCZStep",-2.5);

  return;
}

void
Segment49(FuncDataBase& Control,
		   const std::string& lKey)
  /*!
    Set the variables for SPF segment 49
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{
  ELog::RegMethod RegA("linacVariables[F]","Segment49");

  const Geometry::Vec3D startPt(-1010.0,9495.745,0.0);
  const Geometry::Vec3D endPt(-1010.0,9791.435,0.0);

  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"XYAngle",
  		      atan((startPt.X()-endPt.X())/(endPt.Y()-startPt.Y()))*180.0/M_PI);

  Control.addVariable(lKey+"WallRadius",4.0); // K_20-2_354

  setCylGateValve(Control,lKey+"GateA",0.0,false);
  setCylGateValve(Control,lKey+"GateB",180.0,false);

  // Pipes
  setVariable::PipeGenerator PGen;
  PGen.setCF<setVariable::CF35_TDC>();
  PGen.setMat("Stainless304L","Stainless304L");
  PGen.setNoWindow();
  PGen.generatePipe(Control,lKey+"PipeA",51.29-15); // TODO -15 is artificial to avoid clipping with BackWall
  PGen.setOuterVoid();
  PGen.generatePipe(Control,lKey+"PipeB",230.0);

  // Local shielding wall
  // http://localhost:8080/maxiv/work-log/tdc/pictures/spf-hall/img_5378.mp4/view
  setVariable::LocalShieldingGenerator LSGen;
  LSGen.setSize(20,40,40);
  LSGen.setMidHole(4.0,5.0);
  LSGen.generate(Control,lKey+"ShieldA");
  // YStep is wrong: distance to the back wall is 20 cm, but
  // can't do it since gateA is too close to the wall
  // http://localhost:8080/maxiv/work-log/tdc/pictures/spf-hall/spf/img_5462.jpg/view
  Control.addVariable(lKey+"ShieldAYStep",20.0); // wrong, but reasonable
  Control.addVariable(lKey+"ShieldAZStep",-2.5);

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

  Control.addVariable(wallKey+"MainLength",13209.0); // K_20-1_08C6c1
  Control.addVariable(wallKey+"LinearRCutLength",3785.3358+extraYLen);
  Control.addVariable(wallKey+"LinearLTurnLength",3639.735+extraYLen);
  Control.addVariable(wallKey+"RightWallStep",145.0);
  Control.addVariable(wallKey+"SPFAngleLength",4181.5);
  Control.addVariable(wallKey+"SPFLongLength",200.0); // extra divider
  Control.addVariable(wallKey+"SPFAngle",12.8); // derived from K_20-1_08C6c1 and K_20-1_08C6b2

  Control.addVariable(wallKey+"SPFMazeWidthTDC",180.0); // K_20-1_08G6b3
  Control.addVariable(wallKey+"SPFMazeWidthSide",160.0); // K_20-1_08G6b3
  Control.addVariable(wallKey+"SPFMazeWidthSPF",160.0); // K_20-1_08G6b3
  Control.addVariable(wallKey+"SPFMazeLength",360.0); // derived from K_20-1_08G6b3: 690.0-330.0

  Control.addVariable(wallKey+"FKGDoorWidth",131.0); // K_20-1_08F6c1 and K_20-2_349
  Control.addVariable(wallKey+"FKGDoorHeight",211.0); // K_20-2_349
  Control.addVariable(wallKey+"FKGMazeWidth",100.0); // K_20-1_08F6c1
  Control.addVariable(wallKey+"FKGMazeLength",280.0); // derived from K_20-1_08F6c1 (110+170)
  Control.addVariable(wallKey+"FKGMazeWallThick",200.0); // K_20-1_08F6c1

  Control.addVariable(wallKey+"BTGThick",90.0); // calculated from K_20-1_08G6b4: 2700-1800
  Control.addVariable(wallKey+"BTGHeight",200.0); // derived from K_20-6_075
  Control.addVariable(wallKey+"BTGLength",1000.0); // K_20-1_08G6b[14]: 495000-489000-1416+5416
  Control.addVariable(wallKey+"BTGYOffset",180.0); // calculated from K_20-1_08G6b4: 5416-3616
  Control.addVariable(wallKey+"BTGMat","Concrete"); // AR 2020-11-17

  Control.addVariable(wallKey+"SPFParkingFrontWallLength",100.0); // K_20-1_08G6b3
  Control.addVariable(wallKey+"SPFParkingLength",335.0); // K_20-1_08G6b3
  Control.addVariable(wallKey+"SPFParkingWidth",290.0); // derived from K_20-1_08G6b3: 620-330

  Control.addVariable(wallKey+"SPFExitLength",250.0); // derived from K_20-1_08G6b3: 1446.6-1156.6-40
  Control.addVariable(wallKey+"SPFExitDoorLength",101.0); // K_20-2_353
  Control.addVariable(wallKey+"SPFExitDoorHeight",211.0); // K_20-2_353

  Control.addVariable(wallKey+"FemtoMAXWallThick",105.0); // K_01-0_010 IV1.13
  Control.addVariable(wallKey+"FemtoMAXWallOffset",405.0); // derived from K_20-1_08G6b[34]
  Control.addVariable(wallKey+"FemtoMAXWallIronThick",20.0); // K_20-1_08G6b4 + email from AR 2020-11-17

  Control.addVariable(wallKey+"BSPWallThick",100.0); // K_01-0_010
  Control.addVariable(wallKey+"BSP01WallOffset",910.0); // derived from K_20-1_08G6b[34]
  Control.addVariable(wallKey+"BSP01WallLength",1720.0); // derived from K_20-1_08G6b4
  Control.addVariable(wallKey+"BSPMazeWidth",100.0); // K_20-1_08G6c1
  Control.addVariable(wallKey+"BSPFrontMazeThick",100.0); // K_20-1_08G6b4: IV1.6 [K_01-0_010]
  // calculated K_20-1_08G6c1 + email from AR 2020-11-17
  Control.addVariable(wallKey+"BSPMidMazeThick",120.0);
  Control.addVariable(wallKey+"BSPMidMazeDoorHeight",250.0); // K_20-1_08G6b4: B-B [K_20-2_359]
  Control.addVariable(wallKey+"BSPBackMazeThick",50.0); // K_20-1_08G6c1: IV1.12 [K_01-0_010]
  Control.addVariable(wallKey+"BSPFrontMazeIronThick",20.0); // K_20-1_08G6b4 + email from AR 2020-11-17
  Control.addVariable(wallKey+"BSPMidMazeIronThick1",20.0); // Email from AR 2020-11-17
  Control.addVariable(wallKey+"BSPMidMazeIronThick2",30.0); // Email from AR 2020-11-17

  Control.addVariable(wallKey+"LinearWidth",990.0); // calculated based on K_20-1_08C6c1
  Control.addVariable(wallKey+"WallThick",40.0); // K_20-1_08C6c1

  const double totalHeight(360.0); // K_20-6_050
  const double floorDepth(130.0); // K_20-2_348
  Control.addVariable(wallKey+"FloorDepth",floorDepth);
  Control.addVariable(wallKey+"RoofHeight",totalHeight-floorDepth);

  Control.addVariable(wallKey+"RoofThick", 60.0); // K_20-6_053
  Control.addVariable(wallKey+"FloorThick",60.0); // K_20-6_050

  // Extra for boundary
  // 400 is enough, but we add more to allocate for soil
  Control.addVariable(wallKey+"BoundaryWidth",1000.0);
  //  Control.addVariable(wallKey+"BoundaryHeight",100.0);

  // Midwalls: MUST BE INFRONT OF LinearLTurnPoint
  Control.addVariable(wallKey+"MidTXStep",40.0); // derived from K_20-1_08F6c1
  Control.addVariable(wallKey+"MidTYStep",3325.33584+extraYLen);  // to flat of T
  Control.addVariable(wallKey+"MidTAngle",12.4695);  // derived from K_20-1_08F6c1
  Control.addVariable(wallKey+"MidTThick",200.0);  // K_20-1_08F6c1
  Control.addVariable(wallKey+"MidTThickX",150.0);  // K_20-1_08F6c1

  Control.addVariable(wallKey+"MidTLeft",427.1);  // from mid line
  Control.addVariable(wallKey+"MidTFrontAngleStep",277.0);  //  flat
  Control.addVariable(wallKey+"MidTBackAngleStep",301.0);  // out flat
  Control.addVariable(wallKey+"MidTRight",285.0);  // from mid line

  /////////////////////////////////////////////// DUCTS ///////////////
  const size_t nDucts = 20;
  Control.addVariable(wallKey+"MidTNDucts",nDucts);
  // Duct D1 is the TDC modulator klystron duct
  // This is the leftmost duct in K_20-2_354 [email from AR 2021-01-15].
  const double D1YStep = 7172.435; // calculated based on K_20-2_354 and K_20-1_08F6c1
  const double D1ZStep =  158.0; // measured on K_20-2_354

  // TDC modulator klystron duct
  Control.addVariable(wallKey+"MidTDuct1Radius",7.5); // measured with ruler
  Control.addVariable(wallKey+"MidTDuct1YStep",D1YStep-210.0); // measured with ruler
  Control.addVariable(wallKey+"MidTDuct1ZStep",D1ZStep);

  // D1 - D4
  // K_20-2_354, A-A view, leftmost ducts
  for (size_t i=0; i<=4; ++i)
    {
      const std::string name = wallKey+"MidTDuct" + std::to_string(i+2);
      Control.addVariable(name+"Radius",5.0); // K_20-2_354

      // K_20-2_354
      Control.addVariable(name+"YStep",D1YStep+30*static_cast<double>(i));
      Control.addVariable(name+"ZStep",D1ZStep); // K_20-2_354
    }

  // Ducts near the floor
  // They are not in the drawings, but should be approx. 2 meters to the right side after
  // D4 (last duct in the previous serie)
  // Water pipes go through them, but to be conservative we leave them empty
  // [email from AR 2021-01-19]
  const double floorDuctY = D1YStep+200.0; // approx
  for (size_t i=0; i<=4; ++i)
    {
      const std::string name = wallKey+"MidTDuct" + std::to_string(i+6);
      Control.addVariable(name+"Radius",5.0); // dummy      
      Control.addVariable(name+"YStep",floorDuctY+35*static_cast<double>(i));
      Control.addVariable(name+"ZStep",-115); // dummy
    }

  // Ducts above the BTG blocks
  // Photo of these ducts from the SPF hall:
  // http://localhost:8080/maxiv/work-log/tdc/pictures/spf-hall/img_5374.jpg/view
  // Video from the FKG:
  // http://localhost:8080/maxiv/work-log/tdc/pictures/fkg/img_5353.mov/view

  // Lower tier - AR: 210119: I would
  // assume there is also a concrete plug in each duct, but for now
  // start with void.
  const double dx = 60; // artificial offset based on img_5374
  const double BTGductY = 9839.035 - dx; // K_20-2_355
  for (size_t i=0; i<=5; ++i)
    {
      const std::string name = wallKey+"MidTDuct" + std::to_string(i+10);
      Control.addVariable(name+"Radius",7.5); // K_20-2_355
      Control.addVariable(name+"YStep",BTGductY+35*i); // distance: K_20-2_355
      Control.addVariable(name+"ZStep",86.0); // measured in K_20-2_355
    }
  // Upper tier: G1-G5
  // Electric cables, but now we put void to be conservative
  const double BTGductYup = BTGductY + 105.0 - dx; // K_20-2_355: 105 = 241.6-136.6
  for (size_t i=0; i<=5; ++i)
    {
      const std::string name = wallKey+"MidTDuct" + std::to_string(i+16);
      Control.addVariable(name+"Radius",5.0); // K_20-2_355
      // distance: K_20-2_355
      Control.addVariable
	(name+"YStep",BTGductYup+30.0*static_cast<double>(i));
      Control.addVariable(name+"ZStep",158.0); // measured in K_20-2_355
    }
  ///////////////////////////////////////////////

  Control.addVariable(wallKey+"KlysDivThick",100.0);

  Control.addVariable(wallKey+"MidGateOut",202.7); // K_20-1_08F6c1, 380-177.3
  Control.addVariable(wallKey+"MidGateWidth",440.0);// K_20-1_08F6c1
  Control.addVariable(wallKey+"MidGateWall",100.0); // K_20-1_08F6c1

  // back wall
  Control.addVariable(wallKey+"BackWallYStep",10044.0); // tdc-map.pdf
  Control.addVariable(wallKey+"BackWallThick",200.0); // K_20-1_08G6b3
  Control.addVariable(wallKey+"BackWallMat","Concrete"); // K_01-0_010: IV1.10 - Bvägg = 2000, Betong

  // K_20-1_08G6b[34] + email from AR 2020-11-1[79]
  Control.addVariable(wallKey+"BackWallIronThick",20.0);

  const double klystronSideWall(150.0);  // K_20-1_08F6b4
  // adjusted so that the corner is at the correct x coordinate
  Control.addVariable(wallKey+"KlystronXStep",klystronSideWall/2.0+100.0);
  Control.addVariable(wallKey+"KlystronLen",978.215);
  Control.addVariable(wallKey+"KlystronFrontWall",100.0); // K_20-1_08F6b4
  Control.addVariable(wallKey+"KlystronSideWall",klystronSideWall);

  Control.addVariable(wallKey+"VoidMat","Void");
  Control.addVariable(wallKey+"WallMat","Concrete");
  // WallIronMat is some unknown kind of steel with Co content <50 ppm [AR: 201120]
  Control.addVariable(wallKey+"WallIronMat","Stainless304L");
  Control.addVariable(wallKey+"BDRoofIronMat","Stainless304L");
  Control.addVariable(wallKey+"RoofMat","Concrete");
  Control.addVariable(wallKey+"FloorMat","Concrete");
  Control.addVariable(wallKey+"SoilMat","Earth");

  // Pillars
  /* numbering of pillars in the injection hall
     basis: 0 1 0 -1 0 0 (as in the construction drawings)

       1        |      6  8  10  |  12  14  16
         2 3 4  |  5   7  9  11  |  13  15  17
   */
  constexpr size_t nPillars(17); // 12 = 4 + 7 + 6 (different injection hall cells)
  Control.addVariable(wallKey+"NPillars",nPillars);
  Control.addVariable(wallKey+"PillarRadius",30.0); // measured in the PDF drawing
  Control.addVariable(wallKey+"PillarMat","Concrete");

  const std::array<double,nPillars>
    x{-208,57.9,17.9,-22.1,
      -485,-985,-485,-985,-485,-985,-485,
      -985,-485,-985,-485,-985,-485};

  assert(std::abs(std::accumulate
		  (x.begin(),x.end(), 0.0)+9459.3)<Geometry::zeroTol &&
	 "x-coordinates of pillars are wrong");

  const std::array<double,nPillars>
    y{1534.715,1621.215,2021.215,2421.215,
      5814.015,6364.015,6364.015,6964.015,6964.015,7564.015,7564.015,
      8213.215,8214.015,8864.015,8864.015,9464.015,9464.015};

  assert(std::abs(std::accumulate
		  (y.begin(), y.end(), 0.0)-108279.755)<Geometry::zeroTol &&
	 "y-coordinates of pillars are wrong");

  for (size_t i=0; i<nPillars; ++i)
    {
      const std::string n(std::to_string(i+1));
      Control.addVariable(wallKey+"Pillar"+n+"X",x[i]);
      Control.addVariable(wallKey+"Pillar"+n+"Y",y[i]);
    }

  // THz penetration
  Control.addVariable(wallKey+"THzHeight",5.0); // K_20-2_348
  Control.addVariable(wallKey+"THzWidth",30.0); // K_20-2_348
  Control.addVariable(wallKey+"THzXStep",127.0); // K_20-2_348
  Control.addVariable(wallKey+"THzZStep",-10.0); // K_20-2_348: 130-120 = 10
  Control.addVariable(wallKey+"THzZAngle",7.35); // measured with liner on K_20-1_08F6c1
  // AR 201106: currently it's lead or concrete, but eventually it will be empty
  // Update 210112: there is Lead (see photo)
  Control.addVariable(wallKey+"THzMat","Lead");

  // Main beam dump room
  // Top view: K_15-6_010
  const std::string bdRoom=wallKey+"BDRoom";
  Control.addVariable(bdRoom+"XStep",-735); // SPF line center
  Control.addVariable(bdRoom+"Height",200.0); // K_15-6_012 B-B
  Control.addVariable(bdRoom+"Length",540); // K_15-6_011
  Control.addVariable(bdRoom+"FloorThick",200.0); // K_15-6_012 B-B
  Control.addVariable(bdRoom+"RoofThick",50.0); // K_15-6_011
  Control.addVariable(bdRoom+"RoofSteelWidth",140.0); // measured with ruler
  Control.addVariable(bdRoom+"FrontWallThick",100.0); // K_15-6_011
  Control.addVariable(bdRoom+"SideWallThick",200.0); // K_15-6_010
  Control.addVariable(bdRoom+"BackSteelThick",50.0); // K_15-6_011
  Control.addVariable(bdRoom+"HatchLength",200.0); // measured on K_15-6_011
  Control.addVariable(bdRoom+"InnerWallThick",40.0); // K_15-6_010
  Control.addVariable(bdRoom+"InnerWallLength",365.0+2.0); // K_15-6_010, +2 just to avoid cutting SPF45PipeB
  Control.addVariable(bdRoom+"TDCWidth",380.0); // K_15-6_010
  Control.addVariable(bdRoom+"SPFWidth",460.0); // K_15-6_010
  Control.addVariable(bdRoom+"NewWidth",280.0); // K_15-6_010

  Control.addVariable(wallKey+"WasteRoomWidth",200.0); // derived from K_20-1_08G6b1:  2700-300-40
  Control.addVariable(wallKey+"WasteRoomLength",600.0); // derived from K_20-1_08G6b1: 10316-3516-40*2
  Control.addVariable(wallKey+"WasteRoomWallThick",40.0); // K_20-1_08G6b1
  Control.addVariable(wallKey+"WasteRoomYStep",7534.0); // derived from K_20-1_08G6b1

  // Local shielding at the MidT wall
  // Email from AR 2021-01-19 and 2021-01-20
  // K_20-1_08F6c1.pdf
  // K_20-2_348 - this wall marked as "STRÅLSKYDD ENL. SENARE BESKED"
  Control.addVariable(wallKey+"MidTFrontLShieldThick",10.0); // AR 2021-01-20
  Control.addVariable(wallKey+"MidTFrontLShieldHeight",100.0); // AR 2021-01-20
  Control.addVariable(wallKey+"MidTFrontLShieldWidth",100.0); // AR 2021-01-20
  Control.addVariable(wallKey+"MidTFrontLShieldMat","Lead"); // AR 2021-01-20

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
  ELog::RegMethod RegA("linacVariables[F]","LINACvariables");

  ELog::EM<<"LINAC VARIABLES"<<ELog::endDiag;
  linacVar::wallVariables(Control,"InjectionHall");

  // Segment 1-14
  Control.addVariable("l2spfXStep",linacVar::zeroX);
  Control.addVariable("l2spfYStep",linacVar::zeroY);
  Control.addVariable("l2spfOuterLeft",100.0);
  Control.addVariable("l2spfOuterRight",140.0);
  Control.addVariable("l2spfOuterTop",100.0);

  Control.addVariable("l2spfTurnXStep",linacVar::zeroX-80.0);
  Control.addVariable("l2spfTurnYStep",linacVar::zeroY);
  Control.addVariable("l2spfTurnOuterLeft",80.0);
  Control.addVariable("l2spfTurnOuterRight",140.0);
  Control.addVariable("l2spfTurnOuterTop",100.0);

  Control.addVariable("l2spfAngleXStep",linacVar::zeroX-155.0);
  Control.addVariable("l2spfAngleYStep",linacVar::zeroY+2000.0);
  Control.addVariable("l2spfAngleOuterLeft",50.0);
  Control.addVariable("l2spfAngleOuterRight",50.0);
  Control.addVariable("l2spfAngleOuterTop",100.0);
  Control.addVariable("l2spfAngleXYAngle",12.0);

  Control.addVariable("tdcFrontXStep",-419.0+linacVar::zeroX);
  Control.addVariable("tdcFrontYStep",3152.0+linacVar::zeroY);
  Control.addVariable("tdcFrontOuterLeft",50.0);
  Control.addVariable("tdcFrontOuterRight",50.0);
  Control.addVariable("tdcFrontOuterTop",100.0);
  Control.addVariable("tdcFrontXYAngle",12.0);

  Control.addVariable("tdcXStep",-622.286+linacVar::zeroX);
  Control.addVariable("tdcYStep",4226.013+linacVar::zeroY);
  Control.addVariable("tdcOuterLeft",70.0);
  Control.addVariable("tdcOuterRight",50.0);
  Control.addVariable("tdcOuterTop",100.0);

  Control.addVariable("tdcShortXStep",-622.286+linacVar::zeroX);
  Control.addVariable("tdcShortYStep",4226.013+linacVar::zeroY);
  Control.addVariable("tdcShortOuterLeft",70.0);
  Control.addVariable("tdcShortOuterRight",50.0);
  Control.addVariable("tdcShortOuterTop",100.0);

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
  Control.addVariable("spfAngleOuterRight",90.0);
  Control.addVariable("spfAngleOuterTop",100.0);
  Control.addVariable("spfAngleXYAngle",12.8);

  // start/endPt of Segment34
  Control.addVariable("spfXStep",-995.514+linacVar::zeroX);
  Control.addVariable("spfYStep",5872.556+linacVar::zeroY);
  Control.addVariable("spfOuterLeft",60.0);
  Control.addVariable("spfOuterRight",80.0);
  Control.addVariable("spfOuterTop",100.0);


  Control.addVariable("spfLongXStep",-622.286+linacVar::zeroX);
  Control.addVariable("spfLongYStep",4226.013+linacVar::zeroY);
  Control.addVariable("spfLongOuterLeft",250.0);
  Control.addVariable("spfLongOuterRight",250.0);
  Control.addVariable("spfLongOuterTop",100.0);


  // start/endPt of Segment40
  Control.addVariable("spfFarXStep",-995.514+linacVar::zeroX);
  Control.addVariable("spfFarYStep",7900.0+linacVar::zeroY);
  Control.addVariable("spfFarOuterLeft",60.0);
  Control.addVariable("spfFarOuterRight",80.0);
  Control.addVariable("spfFarOuterTop",100.0);

  // segment 49
  Control.addVariable("spfBehindBackWallXStep",-995.514+linacVar::zeroX); // dummy
  Control.addVariable("spfBehindBackWallYStep",9495.745+linacVar::zeroY); // start of segment49
  Control.addVariable("spfBehindBackWallOuterLeft",50.0);
  Control.addVariable("spfBehindBackWallOuterRight",50.0);
  Control.addVariable("spfBehindBackWallOuterTop",100.0);


  linacVar::Segment1(Control,"L2SPF1");
  linacVar::Segment2(Control,"L2SPF2");
  linacVar::Segment3(Control,"L2SPF3");
  linacVar::Segment4(Control,"L2SPF4");
  linacVar::Segment5(Control,"L2SPF5");
  linacVar::Segment6(Control,"L2SPF6");
  linacVar::Segment7(Control,"L2SPF7");
  linacVar::Segment8(Control,"L2SPF8");
  linacVar::Segment9(Control,"L2SPF9");
  linacVar::Segment10(Control,"L2SPF10");
  linacVar::Segment11(Control,"L2SPF11");
  linacVar::Segment12(Control,"L2SPF12");
  linacVar::Segment13(Control,"L2SPF13");

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
  linacVar::Segment26(Control,"TDC26");

  linacVar::Segment27(Control,"TDC27");
  linacVar::Segment28(Control,"TDC28");
  linacVar::Segment29(Control,"TDC29");
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
  linacVar::Segment40(Control,"SPF40");
  linacVar::Segment41(Control,"SPF41");
  linacVar::Segment42(Control,"SPF42");
  linacVar::Segment43(Control,"SPF43");
  linacVar::Segment44(Control,"SPF44");
  linacVar::Segment45(Control,"SPF45");
  linacVar::Segment46(Control,"SPF46");
  linacVar::Segment47(Control,"SPF47");
  linacVar::Segment48(Control,"SPF48");
  linacVar::Segment49(Control,"SPF49");

  return;
}

}  // NAMESPACE setVariable
