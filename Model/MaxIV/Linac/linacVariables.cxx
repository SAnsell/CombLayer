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

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "variableSetup.h"
#include "maxivVariables.h"

#include "CFFlanges.h"
#include "PipeGenerator.h"
#include "SplitPipeGenerator.h"
#include "BellowGenerator.h"
#include "LeadPipeGenerator.h"
#include "CrossGenerator.h"
#include "GateValveGenerator.h"
#include "JawValveGenerator.h"
#include "FlangeMountGenerator.h"
#include "CorrectorMagGenerator.h"
#include "LinacQuadGenerator.h"
#include "PipeTubeGenerator.h"
#include "PortItemGenerator.h"
#include "BPMGenerator.h"
#include "CylGateValveGenerator.h"
#include "DipoleDIBMagGenerator.h"
#include "EArrivalMonGenerator.h"
#include "YagScreenGenerator.h"
#include "YagUnitGenerator.h"

namespace setVariable
{

namespace linacVar
{
  void wallVariables(FuncDataBase&,const std::string&);
  void linac2SPFsegment1(FuncDataBase&,const std::string&);
  void linac2SPFsegment2(FuncDataBase&,const std::string&);

  void linac2SPFsegment14(FuncDataBase&,const std::string&);
  void linac2SPFsegment15(FuncDataBase&,const std::string&);

  const double zeroX(152.0);   // coordiated offset to master
  const double zeroY(81.0);    // drawing README.pdf
  
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
  setVariable::LinacQuadGenerator LQGen;
  setVariable::BPMGenerator BPMGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::PortItemGenerator PItemGen;
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

  BellowGen.generateBellow(Control,lKey+"BellowC",0.0,7.58);
  
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
  setVariable::LinacQuadGenerator LQGen;
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
linac2SPFsegment14(FuncDataBase& Control,
		   const std::string& lKey)
  /*!
    Set the variables for the main walls
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{
  ELog::RegMethod RegA("linacVariables[F]","linac2SPFsegment14");
  setVariable::PipeGenerator PGen;
  setVariable::BellowGenerator BellowGen;
  setVariable::LinacQuadGenerator LQGen;
  setVariable::CorrectorMagGenerator CMGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::GateValveGenerator GateGen;

  // number form drawing
  Control.addVariable(lKey+"XStep",-622.286+linacVar::zeroX);   // include 1m offset
  Control.addVariable(lKey+"YStep",4226.013+linacVar::zeroY);        // 
  Control.addVariable(lKey+"XYAngle",0.0);   // this should be 3.1183 deg
    
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
  GateGen.setCubeCF<setVariable::CF40>();
  GateGen.generateValve(Control,lKey+"GateA",0.0,0);
  Control.addVariable(lKey+"GateAPortALen",2.0);

  BellowGen.generateBellow(Control,lKey+"BellowB",0.0,7.44); // measured

  return;
}

void
linac2SPFsegment15(FuncDataBase& Control,
		   const std::string& lKey)
  /*!
    Set the variables for the main walls
    \param Control :: DataBase to use
    \param lKey :: name before part names
  */
{
  ELog::RegMethod RegA("linacVariables[F]","linac2SPFsegment15");
  setVariable::PipeGenerator PGen;
  setVariable::BellowGenerator BellowGen;
  setVariable::LinacQuadGenerator LQGen;
  setVariable::CorrectorMagGenerator CMGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::GateValveGenerator GateGen;
  setVariable::YagScreenGenerator YagGen;

    // number form drawing
  Control.addVariable(lKey+"XStep",-637.608+linacVar::zeroX);   // include 1m offset
  Control.addVariable(lKey+"YStep",4730.259+linacVar::zeroY);        // 
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

  // Ion pump
  name=lKey+"IonPump";
  SimpleTubeGen.setMat("Stainless304");
  SimpleTubeGen.setCF<CF63>();
  PItemGen.setCF<setVariable::CF40_22>(6.6); // Port0 length // measured
  PItemGen.setNoPlate();

  SimpleTubeGen.generateBlank(Control,name,0.0,25.8); // measured
  Control.addVariable(name+"NPorts",3);
  Control.addVariable(name+"FlangeACapThick",setVariable::CF63::flangeLength);

  PItemGen.setPlate(setVariable::CF40_22::flangeLength, "Stainless304");
  PItemGen.generatePort(Control,name+"Port0",OPos,-ZVec);

  PItemGen.setLength(9.5); // measured
  PItemGen.setNoPlate();
  PItemGen.generatePort(Control,name+"Port1",OPos,-XVec);

  PItemGen.setLength(3.2); // measured
  PItemGen.generatePort(Control,name+"Port2",OPos,XVec);

  //YagGen.setCF<CF40_22>();
  YagGen.generateScreen(Control,lKey+"YAG",1);   // closed

  PGen.generatePipe(Control,lKey+"PipeB",0.0,130.0);

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
  ELog::RegMethod RegA("linacVariables[F]","linacVariables");



  linacVar::wallVariables(Control,"InjectionHall");

  // Segment 1-14
  Control.addVariable("TDCl2spfXStep",linacVar::zeroX);
  Control.addVariable("TDCl2spfYStep",linacVar::zeroY); 
  Control.addVariable("TDCl2spfOuterLeft",80.0);
  Control.addVariable("TDCl2spfOuterRight",140.0);
  Control.addVariable("TDCl2spfOuterTop",100.0);

  Control.addVariable("TDCtdcXStep",-622.286+linacVar::zeroX);   
  Control.addVariable("TDCtdcYStep",4226.013+linacVar::zeroY);   
  Control.addVariable("TDCtdcOuterLeft",100.0);
  Control.addVariable("TDCtdcOuterRight",100.0);
  Control.addVariable("TDCtdcOuterTop",100.0);


  linacVar::linac2SPFsegment1(Control,"L2SPF1");
  linacVar::linac2SPFsegment2(Control,"L2SPF2");

  /// Segment 14-28
  linacVar::linac2SPFsegment14(Control,"L2SPF14");
  linacVar::linac2SPFsegment15(Control,"L2SPF15");

  return;
}

}  // NAMESPACE setVariable
