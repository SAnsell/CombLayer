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
#include "magnetVar.h"

#include "CFFlanges.h"
#include "PipeGenerator.h"
#include "SplitPipeGenerator.h"
#include "BellowGenerator.h"
#include "FlangePlateGenerator.h"

#include "GateValveGenerator.h"
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

namespace setVariable
{

namespace linacVar
{
  void wallVariables(FuncDataBase&,const std::string&);
  void setIonPump1Port(FuncDataBase&,const std::string&);
  void setIonPump2Port(FuncDataBase&,const std::string&);
  //  void setIonPump3Port(FuncDataBase&,const std::string&);
  void setIonPump3OffsetPort(FuncDataBase&,const std::string&);
  void setPrismaChamber(FuncDataBase&,const std::string&);
  void setSlitTube(FuncDataBase&,const std::string&);
  void setRecGateValve(FuncDataBase&,const std::string&,
		       const bool);
  void setCylGateValve(FuncDataBase&,const std::string&,
		       const double,const bool);
  void setFlat(FuncDataBase&,const std::string&,
	       const double,const double);
  void setGauge(FuncDataBase&,const std::string&,
		const double);
  void setBellow26(FuncDataBase&,const std::string&,
		 const double);
  void setBellow37(FuncDataBase&,const std::string&,
		 const double);

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

  SimpleTubeGen.setCF<CF37_TDC>();
  SimpleTubeGen.setMat("Stainless304L"); // mat checked
  SimpleTubeGen.generateTube(Control,name,0.0,12.6); // measured

  Control.addVariable(name+"NPorts",1);
  PItemGen.setCF<setVariable::CF37_TDC>(5.1); // measured
  PItemGen.setPlate(setVariable::CF37_TDC::flangeLength, "Stainless304L"); // mat checked
  PItemGen.generatePort(Control,name+"Port0",OPos,-XVec);

  return;
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

  const double wallThick = 0.2;
  const double innerRadius = 3.3;
  const double outerR = innerRadius+wallThick; // main pipe outer radius

  const std::string mat = "Stainless304L";
  SimpleTubeGen.setMat(mat);

  SimpleTubeGen.setCF<setVariable::CF63>();

  SimpleTubeGen.generateBlank(Control,name,0.0,25.8);
  Control.addVariable(name+"Radius",innerRadius); // No_17_00.pdf
  Control.addVariable(name+"WallThick",wallThick); // No_17_00.pdf
  Control.addVariable(name+"NPorts",2);
  Control.addVariable(name+"FlangeCapThick",setVariable::CF63::flangeLength);
  Control.addVariable(name+"FlangeCapMat",mat);

  // length of ports 0 and 1
  // measured at Segment18 from front/back to the centre
  const double L0(8.5 - outerR);
  const double L1(7.5 - outerR);

  PItemGen.setCF<setVariable::CF35_TDC>(L0); // No_10_00.pdf
  PItemGen.setNoPlate();
  PItemGen.generatePort(Control,name+"Port0",OPos,-XVec);

  PItemGen.setLength(L1);
  PItemGen.generatePort(Control,name+"Port1",OPos,XVec);

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
setPrismaChamber(FuncDataBase& Control,
		const std::string& name)
/*!
  Set the Prisma Chamber (4 port pipe) variables
  \param Control :: DataBase to use
  \param name :: name prefix
 */
{
  ELog::RegMethod RegA("linacVariables[F]","setPrismaChamber");

  const Geometry::Vec3D OPos(0.0, 0.0, 0.0);
  const Geometry::Vec3D XVec(1,0,0);
  const Geometry::Vec3D ZVec(0,0,1);

  setVariable::PipeTubeGenerator SimpleTubeGen;
  SimpleTubeGen.setMat("Stainless304L");
  SimpleTubeGen.generateBlank(Control,name,0.0,33.2); // measured
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

  PItemGen.setCF<setVariable::CF40_22>(6.50778); // depends on pPos01
  PItemGen.setNoPlate();
  PItemGen.generatePort(Control,name+"Port0",pPos01,-XVec);

  PItemGen.generatePort(Control,name+"Port1",pPos01,XVec);

  PItemGen.setCF<setVariable::CF100>(3.9);
  PItemGen.generatePort(Control,name+"Port2",pPos23,ZVec);
  Control.addVariable(name+"Port2CapMat", "Stainless304L");

  PItemGen.setLength(3.9);
  PItemGen.generatePort(Control,name+"Port3",pPos23,-ZVec);
  Control.addVariable(name+"Port3CapMat", "Stainless304L");

  return;
}

void
setSlitTube(FuncDataBase& Control,
	    const std::string& name)
/*!
  Set the slit tibe variables
  \param Control :: DataBase to use
  \param name :: name prefix
 */
{
  ELog::RegMethod RegA("linacVariables[F]","setSlitTube");

  const double DLength(18.0); // slit tube length, without flanges[AB]

  setVariable::PortTubeGenerator PTubeGen;
  setVariable::PortItemGenerator PItemGen;

  PTubeGen.setMat("Stainless304");

  const double Radius(7.5);
  const double WallThick(0.5);
  const double PortRadius(Radius+WallThick+0.5);
  PTubeGen.setPipe(Radius,WallThick);
  PTubeGen.setPortCF<setVariable::CF40>();
  const double sideWallThick(1.0);
  PTubeGen.setPortLength(-sideWallThick,sideWallThick);
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
  PItemGen.setCF<setVariable::CF63>(5.0);
  PItemGen.generatePort(Control,portName+"0",-PPos,ZVec);
  PItemGen.setCF<setVariable::CF63>(10.0);
  PItemGen.generatePort(Control,portName+"1",MidPt,XVec);

  PItemGen.setCF<setVariable::CF63>(5.0);
  PItemGen.generatePort(Control,portName+"2",-PPos,-ZVec);
  PItemGen.setCF<setVariable::CF63>(10.0);
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

  return;
}

void
setFlat(FuncDataBase& Control,
	const std::string& name,
	const double length,
	const double rotateAngle)
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

  return;
}

void
setGauge(FuncDataBase& Control,
	 const std::string& name)
/*!
  Set the vacuum gauge variables
  \param Control :: DataBase to use
  \param name :: name prefix
 */
{
  ELog::RegMethod RegA("linacVariables[F]","setGauge");

  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::PortItemGenerator PItemGen;

  const Geometry::Vec3D OPos(0,0.0,0);
  const Geometry::Vec3D XVec(1,0,0);

  SimpleTubeGen.setCF<CF37_TDC>();
  SimpleTubeGen.setMat("Stainless304L");
  SimpleTubeGen.generateTube(Control,name,0.0,12.6); // 12.596

  Control.addVariable(name+"NPorts",1);
  PItemGen.setCF<setVariable::CF37_TDC>(5.1); // measured in segment 19
  PItemGen.setPlate(setVariable::CF37_TDC::flangeLength, "Stainless304L");
  PItemGen.generatePort(Control,name+"Port0",OPos,XVec);

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
	  const double length=16.0)
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
		const std::string& name)
/*!
  Set the Mirror Chamber (4 port pipe) variables
  \param Control :: DataBase to use
  \param name :: name prefix
 */
{
  ELog::RegMethod RegA("linacVariables[F]","setMirrorChamber");

  const Geometry::Vec3D OPos(0.0, 3.0, 0.0);
  const Geometry::Vec3D XVec(1,0,0);
  const Geometry::Vec3D ZVec(0,0,1);

  setVariable::PipeTubeGenerator SimpleTubeGen;
  SimpleTubeGen.setCF<setVariable::CF63>();
  SimpleTubeGen.setMat("Stainless304L");
  SimpleTubeGen.generateTube(Control,name,0.0,20.8); // measured

  Control.addVariable(name+"NPorts",4);
  Control.addVariable(name+"FlangeACapThick",setVariable::CF63::flangeLength);
  Control.addVariable(name+"FlangeBCapThick",setVariable::CF63::flangeLength);
  Control.addVariable(name+"FlangeCapMat","Stainless304L");

  // Outer radius of the chamber
  const double outerR =
    setVariable::CF63::innerRadius+setVariable::CF63::wallThick;

  const double L0(12.6/2 - outerR); // measured
  const double L1(L0);
  const double L2(12.5/2 - outerR);
  const double L3(L2);

  setVariable::PortItemGenerator PItemGen;

  PItemGen.setCF<setVariable::CF40_22>(L0);
  PItemGen.setNoPlate();
  PItemGen.generatePort(Control,name+"Port0",OPos,-XVec);

  PItemGen.setLength(L1);
  PItemGen.generatePort(Control,name+"Port1",OPos,XVec);

  PItemGen.setPlate(setVariable::CF40_22::flangeLength,"Stainless304L");
  PItemGen.setCF<setVariable::CF40_22>(L2);
  PItemGen.generatePort(Control,name+"Port2",OPos,ZVec);

  PItemGen.setLength(L3);
  PItemGen.generatePort(Control,name+"Port3",OPos,-ZVec);

  return;
}

void
setMirrorChamberBlank(FuncDataBase& Control,
		const std::string& name)
/*!
  Set the blank Mirror Chamber (4 port pipe) variables
  \param Control :: DataBase to use
  \param name :: name prefix
 */
{
  ELog::RegMethod RegA("linacVariables[F]","setMirrorChamberBlank");

  const Geometry::Vec3D OPos(0.0, 5.6, 0.0);
  const Geometry::Vec3D XVec(1,0,0);
  const Geometry::Vec3D ZVec(0,0,1);

  setVariable::PipeTubeGenerator SimpleTubeGen;
  SimpleTubeGen.setCF<setVariable::CF63>();
  SimpleTubeGen.setMat("Stainless304L");
  SimpleTubeGen.generateBlank(Control,name,0.0,16.0); // measured seg 15

  Control.addVariable(name+"NPorts",4);
  Control.addVariable(name+"FlangeCapThick",setVariable::CF63::flangeLength);
  Control.addVariable(name+"FlangeCapMat","Stainless304L");

  // Outer radius of the chamber
  const double outerR =
    setVariable::CF63::innerRadius+setVariable::CF63::wallThick;

  const double L0(5.9-outerR); // measured seg 15
  const double L1(8.1-outerR); // measured seg 15
  const double L2(12.5/2 - outerR);
  const double L3(L2);

  setVariable::PortItemGenerator PItemGen;

  PItemGen.setCF<setVariable::CF40_22>(L0);
  PItemGen.setNoPlate();
  PItemGen.generatePort(Control,name+"Port0",OPos,-XVec);

  PItemGen.setLength(L1);
  PItemGen.generatePort(Control,name+"Port1",OPos,XVec);

  PItemGen.setPlate(setVariable::CF40_22::flangeLength,"Stainless304L");
  PItemGen.setCF<setVariable::CF40_22>(L2);
  PItemGen.generatePort(Control,name+"Port2",OPos,ZVec);

  PItemGen.setLength(L3);
  PItemGen.generatePort(Control,name+"Port3",OPos,-ZVec);

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
  setVariable::PipeGenerator PGen;
  setVariable::LinacQuadGenerator LQGen;
  setVariable::CorrectorMagGenerator CMGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::StriplineBPMGenerator BPMGen;

  // exactly 1m from wall.
  const Geometry::Vec3D startPt(0,0,0);
  const Geometry::Vec3D endPt(0,395.2,0);
  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);

  PGen.setCF<setVariable::CF18_TDC>();
  PGen.setMat("Stainless316L");
  PGen.setNoWindow();

  PGen.generatePipe(Control,lKey+"PipeA",16.5); // No_1_00.pdf

  setBellow26(Control,lKey+"BellowA");

  //  corrector mag and pie
  PGen.generatePipe(Control,lKey+"PipeB",57.23); // No_1_00.pdf
  CMGen.generateMag(Control,lKey+"CMagHorrA",31.85,0); // No_1_00.pdf
  CMGen.generateMag(Control,lKey+"CMagVertA",46.85,1); // No_1_00.pdf

  PGen.setCF<setVariable::CF16_TDC>();
  PGen.setMat("Stainless304L");
  PGen.generatePipe(Control,lKey+"PipeC",34.27); // No_1_00.pdf

  PGen.setCF<setVariable::CF18_TDC>();
  PGen.setMat("Stainless316L");
  PGen.generatePipe(Control,lKey+"PipeD",113.7);

  CMGen.generateMag(Control,lKey+"CMagHorrB",51.86, 0);
  CMGen.generateMag(Control,lKey+"CMagVertB",69.36, 1);
  LQGen.generateQuad(Control,lKey+"QuadA",96.86);

  BPMGen.generateBPM(Control,lKey+"BPM",0.0);

  PGen.setCF<setVariable::CF18_TDC>();
  PGen.generatePipe(Control,lKey+"PipeF",128.0);

  CMGen.generateMag(Control,lKey+"CMagHorrC",101.20,0);
  CMGen.generateMag(Control,lKey+"CMagVertC",117.0,1);

  const Geometry::Vec3D OPos(0,2.0,0);
  const Geometry::Vec3D ZVec(0,0,-1);

  SimpleTubeGen.setMat("Stainless304L");
  SimpleTubeGen.setCF<setVariable::CF66_TDC>();

  SimpleTubeGen.setFlangeCap(setVariable::CF66_TDC::flangeLength, 0.0);  // No_1_00.pdf
  SimpleTubeGen.generateBlank(Control,lKey+"PumpA",0.0,12.4);
  Control.addVariable(lKey+"PumpABlankThick",0.4); // No_1_00.pdf
  Control.addVariable(lKey+"PumpAFlangeCapMat","Stainless304L");

  Control.addVariable(lKey+"PumpANPorts",2);

  PItemGen.setCF<setVariable::CF35_TDC>(5.0);
  PItemGen.setNoPlate();
  PItemGen.generatePort(Control,lKey+"PumpAPort0",OPos,-ZVec);
  PItemGen.setLength(4.0);
  PItemGen.generatePort(Control,lKey+"PumpAPort1",OPos,ZVec);


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

  setBellow26(Control,lKey+"BellowA");

  PGen.generatePipe(Control,lKey+"PipeB",113.96); // No_2_00.pdf
  LQGen.generateQuad(Control,lKey+"QuadB",73.66); // No_2_00.pdf

  setCylGateValve(Control,lKey+"GateTube", 90.0, false);

  PGen.generatePipe(Control,lKey+"PipeC",31.5); // No_2_00.pdf

  EArrGen.generateMon(Control,lKey+"BeamArrivalMon",0.0);

  PGen.generatePipe(Control,lKey+"PipeD",75.8); // No_2_00.pdf

  setBellow26(Control,lKey+"BellowB");

  BPMGen.generateBPM(Control,lKey+"BPMB",0.0);

  PGen.generatePipe(Control,lKey+"PipeE",133.4); // No_2_00.pdf

  LQGen.generateQuad(Control,lKey+"QuadC",24.7); // No_2_00.pdf
  LQGen.generateQuad(Control,lKey+"QuadD",74.7); // No_2_00.pdf
  LQGen.generateQuad(Control,lKey+"QuadE",114.7); // No_2_00.pdf


  YagUnitGen.generateYagUnit(Control,lKey+"YagUnit");
  YagScreenGen.generateScreen(Control,lKey+"YagScreen",1);   // closed
  Control.addVariable(lKey+"YagScreenYAngle",-90.0);

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

  setBellow26(Control,lKey+"BellowA");

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

  CMGen.generateMag(Control,lKey+"CMagHorA",15.176/pipeAcos,0);  // No_3_00.pdf
  CMGen.generateMag(Control,lKey+"CMagVertA",31.151/pipeAcos,1); // No_3_00.pdf

  const double flatBXYAngle = 1.6;  // No_3_00.pdf
  const double flatBcos = cos((flatAXYAngle+pipeAXYAngle+flatBXYAngle)*M_PI/180.0);  // No_3_00.pdf
  setFlat(Control,lKey+"FlatB",82.292/flatBcos,flatBXYAngle);  // No_3_00.pdf

  DIBGen.generate(Control,lKey+"DipoleB");
  Control.addVariable(lKey+"DipoleBYStep",0.02); // this centers DipoleB at 225.468 [No_3_00.pdf]

  setBellow26(Control,lKey+"BellowB");
  Control.addVariable(lKey+"BellowBXYAngle",1.6);

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
  PGen.generatePipe(Control,lKey+"PipeA",67.0); // No_4_00.pdf

  BPMGen.generateBPM(Control,lKey+"BPMA",0.0);

  PGen.generatePipe(Control,lKey+"PipeB",80.2); // No_4_00.pdf

  LQGen.generateQuad(Control,lKey+"QuadA",19.7); // No_4_00.pdf
  LSGen.generateSexu(Control,lKey+"SexuA",40.7); // No_4_00.pdf
  LQGen.generateQuad(Control,lKey+"QuadB",61.7); // No_4_00.pdf

  YagUnitGen.generateYagUnit(Control,lKey+"YagUnit");
  Control.addVariable(lKey+"YagUnitYAngle",90.0);

  YagScreenGen.generateScreen(Control,lKey+"YagScreen",1);   // closed
  Control.addVariable(lKey+"YagScreenYAngle",-90.0);

  setBellow26(Control,lKey+"BellowA");

  PGen.generatePipe(Control,lKey+"PipeC",70.2); // No_4_00.pdf

  CMGen.generateMag(Control,lKey+"CMagHorC",14,1);
  CMGen.generateMag(Control,lKey+"CMagVertC",34,0);

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

  setVariable::CF40 CF40unit;
  setVariable::BeamDividerGenerator BDGen(CF40unit);
  setVariable::FlatPipeGenerator FPGen;
  setVariable::DipoleDIBMagGenerator DIBGen;

  const double angleDipole(1.6-0.12);
  //  const double bendDipole(1.6);
  const Geometry::Vec3D startPt(-45.073,1420.344,0);
  const Geometry::Vec3D endPt(-90.011,1683.523,0);
  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"XYAngle",6.4);
  // Control.addVariable(lKey+"XYAngle",
  // 		      atan((startPt.X()-endPt.X())/(endPt.Y()-startPt.Y()))*180.0/M_PI);

  const double flatAXYAngle = atan(117.28/817.51)*180/M_PI; // No_5_00.pdf

  setFlat(Control,lKey+"FlatA",81.751/cos(flatAXYAngle*M_PI/180.0),angleDipole);

  DIBGen.generate(Control,lKey+"DipoleA");
  Control.addVariable(lKey+"DipoleAYStep",-0.0091); // this centers DipoleA at 40.895 [No_5_00.pdf]

  BDGen.generateDivider(Control,lKey+"BeamA",angleDipole);
  Control.addVariable(lKey+"BeamAExitLength", 15);
  Control.addVariable(lKey+"BeamAMainLength", 34.4);

  setFlat(Control,lKey+"FlatB",81.009/cos((flatAXYAngle+angleDipole*2)*M_PI/180.0),
	  angleDipole);// No_5_00.pdf

  DIBGen.generate(Control,lKey+"DipoleB");
  Control.addVariable(lKey+"DipoleBYStep",0.037); // this centers DipoleB at 21.538 [No_5_00.pdf]

  setBellow26(Control,lKey+"BellowA");
  Control.addVariable(lKey+"BellowAXYAngle",angleDipole);

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

  PGen.generatePipe(Control,lKey+"PipeA",61.7);

  PGen.generatePipe(Control,lKey+"PipeB",20.0);

  PGen.setBFlangeCF<setVariable::CF66_TDC>();
  PGen.generatePipe(Control,lKey+"PipeC",55.0);

  SCGen.setCF<setVariable::CF66_TDC>();
  SCGen.generateScrapper(Control,lKey+"Scrapper",1.0);   // z lift

  PGen.setCF<setVariable::CF18_TDC>();
  PGen.setAFlangeCF<setVariable::CF66_TDC>();
  PGen.generatePipe(Control,lKey+"PipeD",20.0);

  CSGen.generateCeramicGap(Control,lKey+"CeramicA");

  EBGen.generateEBeamStop(Control,lKey+"EBeam",0);

  CSGen.generateCeramicGap(Control,lKey+"CeramicB");


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

  setBellow26(Control,lKey+"BellowA");

  EBGen.generateEBeamStop(Control,lKey+"EBeam",0);

  setBellow26(Control,lKey+"BellowB");

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
  setIonPump2Port(Control, lKey+"PumpA");

  PGen.generatePipe(Control,lKey+"PipeA",57.8); // No_9_00.pdf

  CMGen.generateMag(Control,lKey+"CMagVertA",22.0,1);
  CMGen.generateMag(Control,lKey+"CMagHorA",42.0,0);

  setBellow26(Control,lKey+"BellowB");
  BPMGen.generateBPM(Control,lKey+"BPM",0.0);

  PGen.setBFlange(setVariable::CF18_TDC::innerRadius+setVariable::CF18_TDC::wallThick,
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

  Control.addVariable(lKey+"WallRadius",4.0);

  PGen.setCF<setVariable::CF18_TDC>();
  PGen.setMat("Stainless316L","Stainless304L");
  PGen.setNoWindow();

  PGen.generatePipe(Control,lKey+"PipeA",453.0);
  setBellow26(Control,lKey+"BellowA");

  setRecGateValve(Control, lKey+"GateValve", false);

  setIonPump2Port(Control, lKey+"PumpA");

  PGen.generatePipe(Control,lKey+"PipeB",152.1);
  setBellow26(Control,lKey+"BellowB");

  PGen.generatePipe(Control,lKey+"PipeC",126.03);

  LQGen.generateQuad(Control,lKey+"QuadA",33.8);
  CMGen.generateMag(Control,lKey+"CMagVertA",115.23,1);

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

  setBellow26(Control,lKey+"BellowA");
  BPMGen.generateBPM(Control,lKey+"BPM",0.0);

  PGen.generatePipe(Control,lKey+"PipeA",42.50);
  LQGen.generateQuad(Control,lKey+"QuadA",24.7);

  setIonPump3OffsetPort(Control,lKey+"PumpA");

  YagUnitGen.generateYagUnit(Control,lKey+"YagUnit");
  Control.addVariable(lKey+"YagUnitYAngle",90.0);

  YagScreenGen.generateScreen(Control,lKey+"YagScreen",1);   // closed
  Control.addVariable(lKey+"YagScreenYAngle",-90.0);

  PGen.generatePipe(Control,lKey+"PipeB",154.47);

  CMGen.generateMag(Control,lKey+"CMagHorA",10.0,1);
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

  PGen.setCF<setVariable::CF18_TDC>();
  PGen.setMat("Stainless316L");
  PGen.setNoWindow();

  setBellow26(Control,lKey+"BellowA");

  // No_12_00.pdf
  const double flatAXYAngle = -1.6;
  setFlat(Control,lKey+"FlatA",
	  (935.83-73.14)/10.0/cos((XYAngle+flatAXYAngle)*M_PI/180.0),
	  flatAXYAngle);
  Control.addVariable(lKey+"FlatAFrontWidth",3.304-1.344); // outer width
  Control.addVariable(lKey+"FlatABackWidth",5.445-1.344); // outer width
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
  BDGen.generateDivider(Control,lKey+"BeamA",1.6,1,0);
  Control.addVariable(lKey+"BeamAWallThick",0.2); // No_12_00.pdf
  // + 0.313 in order to have correct coordinates of the [BE] flanges
  // according to No_12_00.pdf
  Control.addVariable(lKey+"BeamABoxLength",56.1+0.313);

  setBellow26(Control,lKey+"BellowLA");

  // small ion pump port:
  // -----------------------
  // horizontal off
  const Geometry::Vec3D OPos(0.0,2.2,0.0);
  const Geometry::Vec3D XVec(1,0,0);
  const double outerR =
    setVariable::CF63::innerRadius+setVariable::CF63::wallThick;
  const double L0(8.5 - outerR);
  const double L1(7.5 - outerR);

  SimpleTubeGen.setMat("Stainless304L");
  SimpleTubeGen.setCF<setVariable::CF63>();

  SimpleTubeGen.generateBlank(Control,lKey+"IonPumpLA",0.0,12.5);
  Control.addVariable(lKey+"IonPumpLANPorts",2);
  Control.addVariable(lKey+"IonPumpLAFlangeCapThick",
		      setVariable::CF63::flangeLength);
  Control.addVariable(lKey+"IonPumpLAFlangeCapMat","Stainless304L");

  PItemGen.setCF<setVariable::CF35_TDC>(L0); // Port0 length
  PItemGen.setNoPlate();
  PItemGen.generatePort(Control,lKey+"IonPumpLAPort0",OPos,-XVec);

  PItemGen.setLength(L1);
  PItemGen.setNoPlate();
  PItemGen.generatePort(Control,lKey+"IonPumpLAPort1",OPos,XVec);

  PGen.generatePipe(Control,lKey+"PipeLA",93.3-2.87);

  setBellow26(Control,lKey+"BellowLB");

  // RIGHT SIDE

  setFlat(Control,lKey+"FlatB",85.4-2.87,1.6);

  DIBGen.generate(Control,lKey+"DipoleB");
  Control.addVariable(lKey+"DipoleBXStep",6.0);

  setBellow26(Control,lKey+"BellowRB");

  Control.addVariable(lKey+"BellowRBXYAngle",-1.6);
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

  CMGen.generateMag(Control,lKey+"CMagHorA",56.701,0);

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
  CMGen.generateMag(Control,lKey+"CMagVerC",11,1);
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

  setBellow26(Control,lKey+"BellowA");

  PGen.setCF<setVariable::CF18_TDC>();
  PGen.setMat("Stainless316L");
  PGen.setNoWindow();

  // length and angle are obtained from start/end coordinates of flatA
  const double flatAXYAngle=4.801-XYAngle; // 4.801 obtained from start/end coordinates of flatA
  setFlat(Control,lKey+"FlatA",82.581,flatAXYAngle); // No_14_00.pdf

  setVariable::DipoleDIBMagGenerator DIBGen;
  DIBGen.generate(Control,lKey+"DM1");

  PGen.setMat("Stainless316L","Stainless304L");
  PGen.generatePipe(Control,lKey+"PipeB",94.4); // No_14_00.pdf
  Control.addVariable(lKey+"PipeBXYAngle",-1.6); // No_14_00.pdf

  // length and angle are obtained from start/end coordinates of flatB
  // -0.2 to shift the BellowB end at the correct place
  setFlat(Control,lKey+"FlatB",82.581,-1.6-0.2);

  DIBGen.generate(Control,lKey+"DM2");

  setCylGateValve(Control,lKey+"GateA",-90,false);

  setBellow26(Control,lKey+"BellowB"); // No_14_00.pdf

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
  setMirrorChamberBlank(Control, lKey+"MirrorChamber");
  Control.addVariable(lKey+"MirrorChamberYAngle", -90.0);

  YagUnitGen.setCF<CF63>();
  YagUnitGen.generateYagUnit(Control,lKey+"YagUnit");
  Control.addVariable(lKey+"YagUnitYAngle",180.0);

  YagScreenGen.generateScreen(Control,lKey+"YagScreen",0); // 1=closed
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

  setVariable::StriplineBPMGenerator BPMGen;

  setVariable::PipeGenerator PGen;
  setVariable::LinacQuadGenerator LQGen;
  setVariable::CorrectorMagGenerator CMGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::YagScreenGenerator YagGen;

  const Geometry::Vec3D startPt(-637.608,4730.259,0.0);
  const Geometry::Vec3D endPt(-637.608,4983.291,0.0);
  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"XYAngle",0.0);

  setBellow26(Control,lKey+"BellowA");

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

  CMGen.generateMag(Control,lKey+"CMagH",10.0,0); // measured
  CMGen.generateMag(Control,lKey+"CMagV",28.0,1); // measured

  setBellow26(Control,lKey+"BellowB");

  setIonPump2Port(Control,lKey+"IonPump");
  Control.addVariable(lKey+"IonPumpYAngle",90.0);

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

  PGen.generatePipe(Control,lKey+"PipeA",391.23); // No_17_00.pdf

  setBellow26(Control,lKey+"BellowA");

  const std::string pumpName=lKey+"IonPump";
  setIonPump2Port(Control,pumpName);
  Control.addVariable(pumpName+"YAngle",90.0);

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

  setBellow26(Control,lKey+"BellowA");

  setIonPump2Port(Control, lKey+"IonPump");
  Control.addVariable(lKey+"IonPumpYAngle",90.0);

  setBellow26(Control,lKey+"BellowB");

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

  CMGen.generateMag(Control,lKey+"CMagH",10.0,1);
  CMGen.generateMag(Control,lKey+"CMagV",28.0,0);

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

  setBellow26(Control,lKey+"BellowA");

  setGauge(Control,lKey+"Gauge");

  // Fast closing valve
  setRecGateValve(Control,lKey+"GateA",false);

  setIonPump1Port(Control,lKey+"IonPump");

  setCylGateValve(Control,lKey+"GateB",180.0,false);

  setBellow26(Control,lKey+"BellowB");

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

  setBellow26(Control,lKey+"BellowA");

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

  CMGen.generateMag(Control,lKey+"CMagH",10.3,1);
  CMGen.generateMag(Control,lKey+"CMagV",28.3,0);

  setBellow26(Control,lKey+"BellowB");

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

  setBellow26(Control,lKey+"BellowA");

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

  setBellow26(Control,lKey+"BellowB");

  PGen.setCF<setVariable::CF18_TDC>();
  PGen.generatePipe(Control,lKey+"PipeB",40.0);

  CMGen.generateMag(Control,lKey+"CMagH",10.0,0); // guess
  CMGen.generateMag(Control,lKey+"CMagV",29.0,1); // No_23_00.pdf

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

  setBellow26(Control,lKey+"BellowC");

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

  PGen.generatePipe(Control,lKey+"PipeA",325.8);

  setIonPump2Port(Control,lKey+"IonPump");
  Control.addVariable(lKey+"IonPumpYAngle",90.0);

  setBellow26(Control,lKey+"Bellow");

  PGen.generatePipe(Control,lKey+"PipeB",40.0);

  CMGen.generateMag(Control,lKey+"CMagH",10.0,1);
  CMGen.generateMag(Control,lKey+"CMagV",28.0,0);

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

  setBellow26(Control,lKey+"BellowA");

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

  Control.addVariable(lKey+"PipeAAOffset",startPtA+linacVar::zeroOffset);
  Control.addVariable(lKey+"PipeBAOffset",startPtB+linacVar::zeroOffset);
  Control.addVariable(lKey+"PipeCAOffset",startPtC+linacVar::zeroOffset);

  Control.addVariable(lKey+"PipeAAXAngle",
		      std::asin((endPtA-startPtA).unit()[2])*180.0/M_PI);
  Control.addVariable(lKey+"PipeBAXAngle",
		      std::asin((endPtB-startPtB).unit()[2])*180.0/M_PI);
  Control.addVariable(lKey+"PipeCAXAngle",
		      std::asin((endPtC-startPtC).unit()[2])*180.0/M_PI);

  setBellow37(Control,lKey+"BellowAA",16.007);
  setBellow37(Control,lKey+"BellowBA",16.031);
  setBellow37(Control,lKey+"BellowCA",16.108);

  YagUnitGen.generateYagUnit(Control,lKey+"YagUnitA",true);
  Control.addVariable(lKey+"YagUnitAYAngle",90.0);

  YagScreenGen.generateScreen(Control,lKey+"YagScreenA",1);   // closed
  Control.addVariable(lKey+"YagScreenAYAngle",-90.0);

  YagUnitGen.generateYagUnit(Control,lKey+"YagUnitB",true);
  Control.addVariable(lKey+"YagUnitBYAngle",90.0);

  YagScreenGen.generateScreen(Control,lKey+"YagScreenB",1);   // closed
  Control.addVariable(lKey+"YagScreenBYAngle",-90.0);

  PGen.setCF<CF40>();
  PGen.generatePipe(Control,lKey+"PipeAB",217.2);
  PGen.generatePipe(Control,lKey+"PipeBB",210.473);
  PGen.generatePipe(Control,lKey+"PipeCB",222.207);

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

  setVariable::PipeGenerator PGen;
  PGen.setCF<CF35_TDC>();
  PGen.setNoWindow();
  PGen.setMat("Stainless304L");

  setBellow37(Control,lKey+"BellowAA",16.0);
  setBellow37(Control,lKey+"BellowBA",16.0);
  setBellow37(Control,lKey+"BellowCA",16.0);

  Control.addVariable(lKey+"BellowAAOffset",startPtA+linacVar::zeroOffset);
  Control.addVariable(lKey+"BellowBAOffset",startPtB+linacVar::zeroOffset);
  Control.addVariable(lKey+"BellowCAOffset",startPtC+linacVar::zeroOffset);

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

  YagScreenGen.generateScreen(Control,lKey+"YagScreenA",1);   // closed
  Control.addVariable(lKey+"YagScreenAYAngle",-90.0);

  YagUnitGen.generateYagUnit(Control,lKey+"YagUnitB",true);
  Control.addVariable(lKey+"YagUnitBYAngle",90.0);
  Control.addVariable(lKey+"YagUnitBFrontLength",13.008); // No_27_00

  YagScreenGen.generateScreen(Control,lKey+"YagScreenB",1);   // closed
  Control.addVariable(lKey+"YagScreenBYAngle",-90.0);

  YagUnitGen.generateYagUnit(Control,lKey+"YagUnitC",true);
  Control.addVariable(lKey+"YagUnitCYAngle",90.0);
  Control.addVariable(lKey+"YagUnitCFrontLength",12.993); // No_27_00

  YagScreenGen.generateScreen(Control,lKey+"YagScreenC",1);   // closed
  Control.addVariable(lKey+"YagScreenCYAngle",-90.0);

  setBellow37(Control,lKey+"BellowAC");
  setBellow37(Control,lKey+"BellowBC");

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

  PGen.generatePipe(Control,lKey+"PipeAA",291.6);
  PGen.generatePipe(Control,lKey+"PipeBA",292.0);

  Control.addVariable(lKey+"PipeAAOffset",startPtA+linacVar::zeroOffset);
  Control.addVariable(lKey+"PipeBAOffset",startPtB+linacVar::zeroOffset);

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

  setVariable::YagScreenGenerator YagScreenGen;
  setVariable::YagUnitGenerator YagUnitGen;

  const Geometry::Vec3D startPtA(-637.608,9073.611,0.0);
  const Geometry::Vec3D startPtB(-637.608,9073.535,-84.888);

  const Geometry::Vec3D endPtA(-637.608,9401.161,0.0);
  const Geometry::Vec3D endPtB(-637.608,9401.151,-102.058);

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

  PGen.generatePipe(Control,lKey+"PipeAA",291.6);
  PGen.generatePipe(Control,lKey+"PipeBA",292.0);

  Control.addVariable(lKey+"PipeAAOffset",startPtA+linacVar::zeroOffset);
  Control.addVariable(lKey+"PipeBAOffset",startPtB+linacVar::zeroOffset);

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

  YagScreenGen.generateScreen(Control,lKey+"YagScreenA",1);   // closed
  Control.addVariable(lKey+"YagScreenAYAngle",-90.0);
  YagScreenGen.generateScreen(Control,lKey+"YagScreenB",1);   // closed
  Control.addVariable(lKey+"YagScreenBYAngle",-90.0);

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
  SimpleTubeGen.setMat("Stainless304L");
  SimpleTubeGen.setCF<CF37_TDC>();
  SimpleTubeGen.generateTube(Control,name,0.0,12.585); // No_30_00
  Control.addVariable(name+"YAngle", 180.0);

  Control.addVariable(name+"NPorts",1);
  PItemGen.setCF<setVariable::CF37_TDC>(5.1); //
  PItemGen.setPlate(setVariable::CF40_22::flangeLength, "Stainless304L");
  PItemGen.generatePort(Control,name+"Port0",OPos,XVec);

  PGen.setCF<setVariable::CF18_TDC>();
  PGen.setMat("Stainless316L","Stainless304L");
  PGen.setNoWindow();
  PGen.generatePipe(Control,lKey+"PipeA",436.5);

  setBellow26(Control,lKey+"Bellow");

  // IonPump
  const std::string pumpName=lKey+"IonPump";
  setIonPump2Port(Control,pumpName);
  Control.addVariable(pumpName+"Length",10.0+setVariable::CF63::flangeLength);
  const double portOffset(1.7);
  Control.addVariable(pumpName+"Port0Centre", Geometry::Vec3D(0, portOffset, 0));
  Control.addVariable(pumpName+"Port1Centre", Geometry::Vec3D(0, portOffset, 0));

  // CMagV
  PGen.generatePipe(Control,lKey+"PipeB",511.23);
  CMGen.generateMag(Control,lKey+"CMagV",500.13,1);

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
  setVariable::StriplineBPMGenerator BPMGen;
  setVariable::LinacQuadGenerator LQGen;

  const Geometry::Vec3D startPt(-827.249, 4928.489, 0.0);
  const Geometry::Vec3D endPt  (-921.651, 5344.0, 0.0);

  Control.addVariable(lKey+"Offset",startPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"EndOffset",endPt+linacVar::zeroOffset);
  Control.addVariable(lKey+"XYAngle",
		      atan((startPt.X()-endPt.X())/(endPt.Y()-startPt.Y()))*180.0/M_PI);

  // BellowA
  setBellow26(Control,lKey+"BellowA");

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

  setCylGateValve(Control,lKey+"Gate",-90.0,false);

  setBellow26(Control,lKey+"BellowB");

  BPMGen.generateBPM(Control,lKey+"BPM",0.0);

  // PipeA and Quadrupole
  PGen.setCF<setVariable::CF18_TDC>();
  PGen.setMat("Stainless316L","Stainless304L");
  PGen.generatePipe(Control,lKey+"PipeA",42.5);

  // QF type quadrupole magnet
  LQGen.generateQuad(Control,lKey+"Quad",19.7);
  // inner box half width/height
  Control.addVariable(lKey+"QuadYokeOuter",9.5);

  setBellow26(Control,lKey+"BellowC");

  PGen.generatePipe(Control,lKey+"PipeB",232.7);
  CMGen.generateMag(Control,lKey+"CMagH",24.7,0);

  // IonPumpB
  setIonPump2Port(Control,lKey+"IonPumpB");
  const double outerR = 3.5; // same as in setIonPump2Port
  Control.addVariable(lKey+"IonPumpBPort0Length",10.0-outerR);
  Control.addVariable(lKey+"IonPumpBPort1Length",10.0-outerR);
  Control.addVariable(lKey+"IonPumpBLength",11.4+setVariable::CF63::flangeLength); // approx
  Control.addVariable(lKey+"IonPumpBPort0Centre", Geometry::Vec3D(0, portOffset, 0));
  Control.addVariable(lKey+"IonPumpBPort1Centre", Geometry::Vec3D(0, portOffset, 0));

  PGen.generatePipe(Control,lKey+"PipeC",55.7);

  setBellow26(Control,lKey+"BellowD");

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

  PGen.setCF<setVariable::CF40_22>();
  PGen.setMat("Stainless316L","Stainless304L");
  PGen.setNoWindow();
  PGen.generatePipe(Control,lKey+"PipeA",67.0); // measured

  CMGen.generateMag(Control,lKey+"CMagHorA",56.0,0);

  BPMGen.generateBPM(Control,lKey+"BPMA",0.0); // 22 cm length OK

  PGen.generatePipe(Control,lKey+"PipeB",81.5); // measured: 81.6, but adjusted to keep total length
  LQGen.generateQuad(Control,lKey+"QuadA",17.1);
  LSGen.generateSexu(Control,lKey+"SexuA",39.0);
  LQGen.generateQuad(Control,lKey+"QuadB",60.9);

  YagUnitGen.generateYagUnit(Control,lKey+"YagUnit"); // length 20.2 != 20
  YagScreenGen.generateScreen(Control,lKey+"YagScreen",1);   // closed
  Control.addVariable(lKey+"YagUnitYAngle",90.0);

  PGen.generatePipe(Control,lKey+"PipeC",68.7);
  CMGen.generateMag(Control,lKey+"CMagVerC",12.5,0);

  setBellow26(Control,lKey+"Bellow");

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
  Control.addVariable(lKey+"XYAngle",6.4);  // Seg 33 angle (measured)

  /*
  setVariable::PipeGenerator PGen;
  setVariable::FlatPipeGenerator FPGen;
  setVariable::DipoleDIBMagGenerator DIBGen;

  FPGen.generateFlat(Control,lKey+"FlatA",82.5); // measured
  Control.addVariable(lKey+"FlatAXYAngle",0.0);

  DIBGen.generate(Control,lKey+"DMA");

  PGen.setMat("Stainless316L","Stainless304L");
  PGen.setNoWindow();
  PGen.setCF<CF40_22>();
  PGen.generatePipe(Control,lKey+"PipeA",94.4); // measured
  Control.addVariable(lKey+"PipeAXYAngle",-1.6);


  FPGen.generateFlat(Control,lKey+"FlatB",82.5); // measured
  Control.addVariable(lKey+"FlatBXYAngle",-1.6);

  DIBGen.generate(Control,lKey+"DMB");

  setBellow26(Control,lKey+"Bellow");
  Control.addVariable(lKey+"BellowXYAngle",-0.0);
  */

  Segment34Magnet(Control,lKey);

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

  YagScreenGen.generateScreen(Control,lKey+"YagScreen",1);
  Control.addVariable(lKey+"YagScreenYAngle",-90.0);
  Control.addVariable(lKey+"YagScreenZStep",-3.3);

  const double pipeALength(37.5); // measured
  PGen.setCF<setVariable::CF40_22>();
  PGen.setMat("Stainless316L","Stainless304L");
  PGen.setNoWindow();
  PGen.generatePipe(Control,lKey+"PipeA",pipeALength);

  LQGen.generateQuad(Control,lKey+"QuadA",pipeALength/2.0); // approx

  BPMGen.generate(Control,lKey+"BPM");

  PGen.generatePipe(Control,lKey+"PipeB",75.2); // measured
  LQGen.generateQuad(Control,lKey+"QuadB",19.0); // approx
  CMGen.generateMag(Control,lKey+"CMagH",45.0,1);
  CMGen.generateMag(Control,lKey+"CMagV",65.0,0);

  setMirrorChamberBlank(Control, lKey+"MirrorChamber");
  Control.addVariable(lKey+"MirrorChamberYAngle", 180.0);

  PGen.generatePipe(Control,lKey+"PipeC",12.6); // measured
  Control.addVariable(lKey+"PipeCFeMat", "Stainless304L"); // PDF

  setBellow26(Control,lKey+"Bellow");

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
  setVariable::StriplineBPMGenerator BPMGen;
  setVariable::EArrivalMonGenerator EArrGen;

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
  LQGen.generateQuad(Control,lKey+"QuadB",128.55); // measured

  // Corrector magnets
  CMGen.generateMag(Control,lKey+"CMagH",42.35+1.3,0); // measured with wrong CMagH length
  // Control.addVariable(lKey+"CMagHMagInnerLength",10.3); // 11.5
  // Control.addVariable(lKey+"CMagHMagLength",14.2-1.2); //
  CMGen.generateMag(Control,lKey+"CMagV",61.05+1.1,1); // measured with wrong CMagV length

  // Beam position monitors
  BPMGen.generateBPM(Control,lKey+"BPMA",0.0);
  BPMGen.generateBPM(Control,lKey+"BPMB",0.0);

  // Beam arrival monitor
  EArrGen.setCF<setVariable::CF40_22>();
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
  CSGen.generateCeramicGap(Control,lKey+"CeramicB");

  EBGen.generateEBeamStop(Control,lKey+"BeamStop",0);
  Control.addVariable(lKey+"BeamStopWallThick",1.0); // measured
  Control.addVariable(lKey+"BeamStopLength",41.0); // measured
  Control.addVariable(lKey+"BeamStopWidth",11.0); // measured
  Control.addVariable(lKey+"BeamStopStopRadius",5.2); // measured

  PGen.setCF<setVariable::CF40_22>();
  PGen.setNoWindow();

  PGen.setMat("Stainless316L","Stainless304L");
  PGen.generatePipe(Control,lKey+"Pipe",18.46);

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

  PGen.setCF<setVariable::CF40_22>();
  PGen.setMat("Aluminium","Aluminium");
  PGen.setNoWindow();
  PGen.generatePipe(Control,lKey+"PipeA",285.0); // measured
  PGen.setMat("Stainless316L","Stainless304L");
  PGen.generatePipe(Control,lKey+"PipeB",221.0); // measured


  const std::string pumpName=lKey+"IonPump";
  setIonPump2Port(Control,pumpName);
  Control.addVariable(lKey+"IonPumpYAngle",180.0);

  setBellow26(Control,lKey+"BellowA");

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
  YagScreenGen.generateScreen(Control,lKey+"YagScreen",1);
  Control.addVariable(lKey+"YagScreenYAngle",-90.0);
  Control.addVariable(lKey+"YagScreenZStep",-3.3);

  // Gate
  setCylGateValve(Control,lKey+"Gate",90.0,false);

  // Pipe
  setVariable::PipeGenerator PGen;
  PGen.setCF<setVariable::CF40_22>();
  PGen.setMat("Stainless316L","Stainless304L");
  PGen.setNoWindow();
  PGen.generatePipe(Control,lKey+"Pipe",37.0);

  setBellow26(Control,lKey+"Bellow");

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
  setBellow26(Control,lKey+"BellowA");
  setBellow26(Control,lKey+"BellowB");

  // Stripline BPM
  setVariable::StriplineBPMGenerator BPMGen;
  BPMGen.generateBPM(Control,lKey+"BPM",0.0);

  // Gate
  setCylGateValve(Control,lKey+"Gate",-90.0,false);

  // Pipe
  setVariable::PipeGenerator PGen;
  PGen.setCF<setVariable::CF40_22>();
  PGen.setMat("Stainless316L","Stainless304L");
  PGen.setNoWindow();
  PGen.generatePipe(Control,lKey+"Pipe",38.0);

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
  Control.addVariable(lKey+"XYAngle",
  		      atan((startPt.X()-endPt.X())/(endPt.Y()-startPt.Y()))*180.0/M_PI);

  // Stripline BPM
  setVariable::StriplineBPMGenerator BPMAGen;
  BPMAGen.setCF<setVariable::CF40_22>();
  BPMAGen.generateBPM(Control,lKey+"BPMA",0.0);

  // Yag screen and its unit
  setVariable::YagUnitBigGenerator YagUnitGen;
  YagUnitGen.generateYagUnit(Control,lKey+"YagUnit");
  Control.addVariable(lKey+"YagUnitYAngle",90.0);

  setVariable::YagScreenGenerator YagScreenGen;
  YagScreenGen.generateScreen(Control,lKey+"YagScreen",1);
  Control.addVariable(lKey+"YagScreenYAngle",-90.0);
  Control.addVariable(lKey+"YagScreenZStep",-3.3);

  // Gate
  setCylGateValve(Control,lKey+"Gate",90.0,false);

  // Pipe
  setVariable::PipeGenerator PGen;
  PGen.setCF<setVariable::CF40_22>();
  PGen.setMat("Stainless316L","Stainless304L");
  PGen.setNoWindow();
  PGen.generatePipe(Control,lKey+"Pipe",39.896); // measured 40, but adjusted to match start/end

  setVariable::CorrectorMagGenerator CMGen;
  CMGen.generateMag(Control,lKey+"CMagH",10.3,0);

  // Button pickup PBM
  setVariable::ButtonBPMGenerator BPMBGen;
  BPMBGen.setCF<setVariable::CF40_22>();
  BPMBGen.generate(Control,lKey+"BPMB");
  Control.addVariable(lKey+"BPMBLength",3.0); // measured
  Control.addVariable(lKey+"BPMBNButtons",2);
  Control.addVariable(lKey+"BPMBButtonYAngle",0.0);
  Control.addVariable(lKey+"BPMBFlangeGap",0.0);
  Control.addVariable(lKey+"BPMBFlangeALength",0.5); // approx
  Control.addVariable(lKey+"BPMBFlangeBLength",0.5); // approx

  setBellow26(Control,lKey+"BellowA");
  setBellow26(Control,lKey+"BellowB");

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
  TGGen.setBend(313.40,110.4,58.0);
  TGGen.generateTri(Control,lKey+"TriBend");

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


  CSGen.generateCeramicGap(Control,lKey+"Ceramic");

  PGen.setCF<setVariable::CF40_22>();
  PGen.generatePipe(Control,lKey+"PipeA",110.5);

  YagUnitGen.generateYagUnit(Control,lKey+"YagUnit");
  Control.addVariable(lKey+"YagUnitYAngle",90.0);

  YagScreenGen.generateScreen(Control,lKey+"YagScreen",1);
  Control.addVariable(lKey+"YagScreenYAngle",-90.0);
  Control.addVariable(lKey+"YagScreenZStep",-3.3);

  PGen.setCF<setVariable::CF63>();
  PGen.generatePipe(Control,lKey+"PipeB",161.75);

  FPGen.setCF<setVariable::CF63>();
  FPGen.setFlangeLen(1.75);
  FPGen.generateFlangePlate(Control,lKey+"Adaptor");
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


  PGen.setCF<setVariable::CF40_22>();
  PGen.setMat("Stainless304L","Stainless304L");
  PGen.setNoWindow();

  MSPGen.setMainLength(6.3,6.3);

  PGen.generatePipe(Control,lKey+"PipeA",96.8); // measured
  const double pipeBLength(40.0);  // measured
  PGen.generatePipe(Control,lKey+"PipeB",pipeBLength);

  // Gate valves
  setVariable::CylGateValveGenerator CGateGen;
  CGateGen.setYRotate(180.0);
  CGateGen.setWallThick(0.3);
  CGateGen.setPortThick(0.10);
  CGateGen.generateGate(Control,lKey+"GateA",0);

  setBellow26(Control,lKey+"BellowA");

  // Prisma Chamber
  setVariable::PrismaChamberGenerator PCGen;
  PCGen.generateChamber(Control, lKey+"PrismaChamber");
  Control.addVariable(lKey+"PrismaChamberYAngle",180.0);

  // Mirror Chambers
  MSPGen.generateCrossWay(Control,lKey+"MirrorChamberA");


  // Cleaning magnet
  setVariable::CleaningMagnetGenerator ClMagGen;
  ClMagGen.generate(Control,lKey+"CleaningMagnet");
  Control.addVariable(lKey+"CleaningMagnetYStep",pipeBLength/2.0);

  // Slit tube and jaws
  setSlitTube(Control,lKey+"SlitTube");

  setBellow26(Control,lKey+"BellowB",10.0); // measured

  MSPGen.generateCrossWay(Control,lKey+"MirrorChamberB");

  setBellow26(Control,lKey+"BellowC",10.0); // measured

  CGateGen.generateGate(Control,lKey+"GateB",0);

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

  // Pipes
  setVariable::PipeGenerator PGen;
  PGen.setCF<setVariable::CF40_22>();
  PGen.setMat("Stainless304L","Stainless304L");
  PGen.setNoWindow();

  PGen.generatePipe(Control,lKey+"PipeA",87.4); // measured
  PGen.generatePipe(Control,lKey+"PipeB",12.6); // measured
  PGen.generatePipe(Control,lKey+"PipeC",12.6); // measured
  PGen.generatePipe(Control,lKey+"PipeD",8.4); // measured
  PGen.generatePipe(Control,lKey+"PipeE",8.4); // measured

  // Gate valves
  setCylGateValve(Control,lKey+"GateA",180.0,false);

  setBellow26(Control,lKey+"BellowA");

  // Prisma Chamber
  setPrismaChamber(Control, lKey+"PrismaChamberA");
  Control.addVariable(lKey+"PrismaChamberAYAngle", -90.0);

  // Mirror Chambers
  setMirrorChamber(Control, lKey+"MirrorChamberA");
  Control.addVariable(lKey+"MirrorChamberAYAngle",90.0);
  setMirrorChamber(Control, lKey+"MirrorChamberB");
  //  Control.addVariable(lKey+"MirrorChamberBYAngle",90.0);
  setMirrorChamber(Control, lKey+"MirrorChamberC");

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
  EBGen.generateEBeamStop(Control,lKey+"BeamStopA",0);
  EBGen.generateEBeamStop(Control,lKey+"BeamStopB",0);

  setBellow26(Control,lKey+"BellowA");
  setBellow26(Control,lKey+"BellowB",10.0); // measured
  setBellow26(Control,lKey+"BellowC",10.0); // measured

  // Pipe
  setVariable::PipeGenerator PGen;
  PGen.setCF<setVariable::CF40_22>();
  PGen.setMat("Stainless304L","Stainless304L");
  PGen.setNoWindow();
  PGen.generatePipe(Control,lKey+"PipeA",12.5); // measured

  // Slit tube and jaws
  setSlitTube(Control,lKey+"SlitTube");

  // Mirror Chamber
  setMirrorChamber(Control, lKey+"MirrorChamberA");

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

  setCylGateValve(Control,lKey+"GateA",0.0,false);
  setCylGateValve(Control,lKey+"GateB",180.0,false);

  // Pipes
  setVariable::PipeGenerator PGen;
  PGen.setCF<setVariable::CF40_22>();
  PGen.setMat("Stainless304L","Stainless304L");
  PGen.setNoWindow();
  PGen.generatePipe(Control,lKey+"PipeA",51.29);
  PGen.generatePipe(Control,lKey+"PipeB",230.0);

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

  Control.addVariable(wallKey+"MainLength",12080.0);
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
  Control.addVariable(wallKey+"FloorThick",150.0);

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
  Control.addVariable("spfLongOuterLeft",60.0);
  Control.addVariable("spfLongOuterRight",50.0);
  Control.addVariable("spfLongOuterTop",100.0);


  // start/endPt of Segment40
  Control.addVariable("spfFarXStep",-995.514+linacVar::zeroX);
  Control.addVariable("spfFarYStep",7900.0+linacVar::zeroY);
  Control.addVariable("spfFarOuterLeft",50.0);
  Control.addVariable("spfFarOuterRight",50.0);
  Control.addVariable("spfFarOuterTop",100.0);


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
