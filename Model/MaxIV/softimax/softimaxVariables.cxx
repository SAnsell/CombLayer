/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   softimax/softimaxVariables.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell/Konstantin Batkov
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
// #include <fstream>
#include <iomanip>
// #include <iostream>
// #include <sstream>
#include <cmath>
// #include <complex>
// #include <list>
#include <vector>
#include <set>
#include <map>
// #include <string>
// #include <algorithm>

// #include "Exception.h"
// #include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
// #include "GTKreport.h"
// #include "OutputLog.h"
// #include "support.h"
// #include "MatrixBase.h"
// #include "Matrix.h"
#include "Vec3D.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
// #include "variableSetup.h"
#include "maxivVariables.h"

#include "CFFlanges.h"
#include "PipeGenerator.h"
#include "SplitPipeGenerator.h"
#include "BellowGenerator.h"
#include "BremCollGenerator.h"
#include "BremMonoCollGenerator.h"
#include "LeadPipeGenerator.h"
#include "CrossGenerator.h"
#include "GateValveGenerator.h"
#include "JawValveGenerator.h"
#include "PipeTubeGenerator.h"
#include "PortTubeGenerator.h"
#include "PortItemGenerator.h"
#include "PipeShieldGenerator.h"
#include "VacBoxGenerator.h"
#include "GratingMonoGenerator.h"
#include "GratingUnitGenerator.h"
#include "TankMonoVesselGenerator.h"
#include "MonoBoxGenerator.h"
#include "FlangeMountGenerator.h"
#include "BeamPairGenerator.h"
#include "MirrorGenerator.h"
#include "CollGenerator.h"
#include "JawFlangeGenerator.h"
// #include "MazeGenerator.h"
// #include "RingDoorGenerator.h"
#include "PortChicaneGenerator.h"
#include "WallLeadGenerator.h"
#include "MonoShutterGenerator.h"

// #include "PreDipoleGenerator.h"
// #include "DipoleChamberGenerator.h"

// #include "R3ChokeChamberGenerator.h"
#include "DiffPumpGenerator.h"

namespace setVariable
{

namespace softimaxVar
{
  void undulatorVariables(FuncDataBase&,const std::string&);
  void frontMaskVariables(FuncDataBase&,const std::string&);
  void wallVariables(FuncDataBase&,const std::string&);
  //  void monoShutterVariables(FuncDataBase&,const std::string&);
  void monoVariables(FuncDataBase&,const std::string&);

void
undulatorVariables(FuncDataBase& Control,
		   const std::string& undKey)
  /*!
    Builds the variables for the undulator
    \param Control :: Database
    \param undKey :: prename
  */
{
  ELog::RegMethod RegA("softimaxVariables[F]","undulatorVariables");
  setVariable::PipeGenerator PipeGen;

  const double L(372.0);
  PipeGen.setMat("Aluminium");
  PipeGen.setNoWindow();   // no window
  PipeGen.setCF<setVariable::CF63>();
  PipeGen.generatePipe(Control,undKey+"UPipe",0,L+7.0);

  Control.addVariable(undKey+"UPipeWidth",6.0);
  Control.addVariable(undKey+"UPipeHeight",0.6);
  Control.addVariable<double>(undKey+"UPipeYStep",20.0);
  Control.addVariable(undKey+"UPipeFeThick",0.2);

  // undulator I Vacuum
  Control.addVariable(undKey+"UndulatorVGap",1.1);  // mininum 11mm
  Control.addVariable(undKey+"UndulatorLength",L);
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
  ELog::RegMethod RegA("softimaxVariables[F]","frontMaskVariables");

  setVariable::CollGenerator CollGen;

  CollGen.setFrontGap(2.62,1.86);       //1033.8
  CollGen.setBackGap(1.54,1.42);
  //  CollGen.setMinSize(29.0,0.55,0.55);  // Approximated to get 1mrad x 1mrad
  CollGen.setMinAngleSize(29.0,1033.0,1000.0,1000.0);  // Approximated to get 1mrad x 1mrad
  CollGen.generateColl(Control,preName+"CollA",0.0,34.0);

  CollGen.setFrontGap(2.13,2.146);
  CollGen.setBackGap(0.756,0.432);

  // approx for 100uRad x 100uRad
  //  CollGen.setMinSize(32.0,0.680,0.358);

  CollGen.setMinAngleSize(32.0,1600.0,100.0,100.0);
  CollGen.generateColl(Control,preName+"CollB",0.0,34.2);

  // FM 3:
  CollGen.setMain(1.20,"Copper","Void");
  CollGen.setFrontGap(0.84,0.582);
  CollGen.setBackGap(0.750,0.357);

  // approx for 40uRad x 40uRad
  CollGen.setMinAngleSize(12.0,1600.0,40.0,40.0);
  CollGen.generateColl(Control,preName+"CollC",0.0,17.0);

  return;
}

void
wallVariables(FuncDataBase& Control,
	      const std::string& wallKey)
/*!
    Set the variables for the frontEnd wall
    \param Control :: DataBase to use
    \param wallKey :: name before part names
  */
{
  ELog::RegMethod RegA("softimaxVariables[F]","wallVariables");

  WallLeadGenerator LGen;
  LGen.setWidth(70,140.0);
  LGen.generateWall(Control,wallKey,2.0);

  return;
}

void
monoVariables(FuncDataBase& Control,
	      const std::string& monoKey)
  /*!
    Builds the variables for the mono packge
    \param Control :: Database
    \param slitKey :: prename
  */
{
  ELog::RegMethod RegA("softimaxVariables[F]","monoVariables");

  //  setVariable::PipeGenerator PipeGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::TankMonoVesselGenerator MBoxGen;
  setVariable::GratingMonoGenerator MXtalGen;
  setVariable::GratingUnitGenerator MUnitGen;

  // PipeGen.setMat("Stainless304");
  // PipeGen.setNoWindow();
  // PipeGen.setCF<setVariable::CF63>();
  // PipeGen.setBFlange(17.8,1.0);
  // PipeGen.generatePipe(Control,monoKey+"OffPipeA",0,3.0);
  // Control.addVariable(monoKey+"OffPipeAFlangeBackZStep",-7.0);

  // ystep/width/height/depth/length
  //
  MBoxGen.setCF<CF63>();   // set ports
  MBoxGen.setAFlange(17.8,1.0);
  MBoxGen.setBFlange(17.8,1.0);
  MBoxGen.setPortLength(7.5,7.5); // La/Lb
  MBoxGen.generateBox(Control,monoKey+"MonoVessel",0.0,54.91,36.45,36.45); // ystep,R,height,depth
  Control.addVariable(monoKey+"MonoVesselOuterSize",65);
  //  Control.addVariable(monoKey+"MonoVesselPortAZStep",-7);   //
  Control.addVariable(monoKey+"MonoVesselFlangeAZStep",-7);     //
  Control.addVariable(monoKey+"MonoVesselFlangeBZStep",-7);     //
  Control.addVariable(monoKey+"MonoVesselPortBZStep",3.2);      // from primary
  Control.addVariable(monoKey+"MonoVesselWallMat", "Aluminium");


  const std::string portName=monoKey+"MonoVessel";
  Control.addVariable(monoKey+"MonoVesselNPorts",1); // beam ports (lots!!)
  PItemGen.setCF<setVariable::CF120>(5.0);
  PItemGen.setPlate(0.0,"Void");
  PItemGen.generatePort(Control,portName+"Port0",
			Geometry::Vec3D(0,5.0,0.0),
			Geometry::Vec3D(1,0,0));

  // crystals
  MXtalGen.generateGrating(Control,monoKey+"MonoXtal",0.0,3.0);
  // monounit
  MUnitGen.generateGrating(Control,monoKey+"Grating",0.0,0.0);

  // PipeGen.setCF<setVariable::CF63>();
  // PipeGen.setAFlange(17.8,1.0);
  // PipeGen.generatePipe(Control,monoKey+"OffPipeB",0,3.0);
  // Control.addVariable(monoKey+"OffPipeBFlangeFrontZStep",-7.0);

  return;
}


// void
// monoShutterVariables(FuncDataBase& Control,
// 		     const std::string& preName)
//   /*!
//     Construct Mono Shutter variables
//     \param Control :: Database for variables
//     \param preName :: Control system
//    */
// {
//   ELog::RegMethod RegA("softimaxVariables","monoShutterVariables");

//   setVariable::GateValveGenerator GateGen;
//   setVariable::BellowGenerator BellowGen;
//   setVariable::MonoShutterGenerator MShutterGen;

//   // both shutters up
//   MShutterGen.generateShutter(Control,preName+"MonoShutter",1,1);

//   // bellows on shield block
//   BellowGen.setCF<setVariable::CF40>();
//   BellowGen.setAFlangeCF<setVariable::CF63>();
//   BellowGen.generateBellow(Control,preName+"BellowJ",0,10.0);

//     // joined and open
//   GateGen.setCF<setVariable::CF40>();
//   GateGen.generateValve(Control,preName+"GateJ",0.0,0);
//   return;
// }

void
opticsHutVariables(FuncDataBase& Control,
		   const std::string& preName)
  /*!
    Variable for the main optics hutch walls
    \param Control :: Database
    \param preName :: Beamline name
  */
{
  ELog::RegMethod RegA("softimaxVariables[F]","opticsCaveVariables");

  const std::string hutName(preName+"OpticsHut");

  Control.addVariable(hutName+"Height",250.0);
  Control.addVariable(hutName+"Length",1070); // outer length, measured by KB - check
  Control.addVariable(hutName+"OutWidth",200.0);
  Control.addVariable(hutName+"RingWidth",75.0);
  Control.addVariable(hutName+"RingWallLen",80.0);
  Control.addVariable(hutName+"RingWallAngle",18.50);
  Control.addVariable(hutName+"RingConcThick",100.0);

  Control.addVariable(hutName+"InnerThick",0.3);

  Control.addVariable(hutName+"PbWallThick",1.2);
  Control.addVariable(hutName+"PbRoofThick",1.2);
  Control.addVariable(hutName+"PbBackThick",2.0);
  Control.addVariable(hutName+"PbFrontThick",2.0);

  Control.addVariable(hutName+"OuterThick",0.3);

  Control.addVariable(hutName+"InnerOutVoid",10.0);  // side wall for chicane
  Control.addVariable(hutName+"OuterOutVoid",10.0);

  Control.addVariable(hutName+"SkinMat","Stainless304");
  Control.addVariable(hutName+"RingMat","Concrete");
  Control.addVariable(hutName+"PbMat","Lead");

  Control.addVariable(hutName+"HoleXStep",2.5);
  Control.addVariable(hutName+"HoleZStep",0.0);
  Control.addVariable(hutName+"HoleRadius",4.5);

  Control.addVariable(hutName+"InletXStep",0.0);
  Control.addVariable(hutName+"InletZStep",0.0);
  Control.addVariable(hutName+"InletRadius",5.0);

  Control.addVariable(preName+"OpticsNChicane",1);
  PortChicaneGenerator PGen;
  PGen.generatePortChicane(Control,preName+"OpticsChicane0",0,0);

  return;
}

// void
// exptHutVariables(FuncDataBase& Control,const std::string& preName)
//   /*!
//     Variable for the main expt hutch walls
//     \param Control :: Database
//     \param preName :: Beamline name
//   */
// {
//   ELog::RegMethod RegA("softimaxVariables[F]","exptHutVariables");

//   const std::string hutName(preName+"ExptHut");

//   Control.addVariable(hutName+"YStep",1000.0);
//   Control.addVariable(hutName+"Depth",120.0);
//   Control.addVariable(hutName+"Height",200.0);
//   Control.addVariable(hutName+"Length",858.4);
//   Control.addVariable(hutName+"OutWidth",198.50);
//   Control.addVariable(hutName+"RingWidth",248.6);
//   Control.addVariable(hutName+"InnerThick",0.2);
//   Control.addVariable(hutName+"PbThick",0.4);
//   Control.addVariable(hutName+"OuterThick",0.2);

//   Control.addVariable(hutName+"VoidMat","Void");
//   Control.addVariable(hutName+"SkinMat","Stainless304");
//   Control.addVariable(hutName+"PbMat","Lead");
//   Control.addVariable(hutName+"FloorMat","Concrete");

//   Control.addVariable(hutName+"HoleXStep",0.0);
//   Control.addVariable(hutName+"HoleZStep",5.0);
//   Control.addVariable(hutName+"HoleRadius",7.0);
//   Control.addVariable(hutName+"HoleMat","Lead");

//   return;
// }



// void
// mirrorBox(FuncDataBase& Control,const std::string& Name,
// 	  const std::string& Index,const std::string& vertFlag,
// 	  const double theta,const double phi)
//   /*!
//     Construct variables for the diagnostic units
//     \param Control :: Database
//     \param Name :: component name
//     \param Index :: Index designator
//     \param theta :: theta angle [beam angle in deg]
//     \param phi :: phi angle [rotation angle in deg]
//   */
// {
//   ELog::RegMethod RegA("softimaxVariables[F]","mirrorBox");

//   setVariable::MonoBoxGenerator VBoxGen;
//   setVariable::MirrorGenerator MirrGen;

//   const double normialAngle=0.2;
//   const double vAngle=(vertFlag[0]=='H') ? 90 : 0.0;
//   const double centreDist(55.0);
//   const double heightNormDelta=sin(2.0*normialAngle*M_PI/180.0)*centreDist;
//   const double heightDelta=sin(2.0*theta*M_PI/180.0)*centreDist;

//   if (vAngle>45)
//     VBoxGen.setBPortOffset(heightNormDelta,0.0);
//   else
//     VBoxGen.setBPortOffset(0.0,heightNormDelta);

//   VBoxGen.setMat("Stainless304");
//   VBoxGen.setWallThick(1.0);
//   VBoxGen.setCF<CF63>();
//   VBoxGen.setPortLength(5.0,5.0); // La/Lb
//   VBoxGen.setLids(3.0,1.0,1.0); // over/base/roof

//   // ystep/width/height/depth/length
//   VBoxGen.generateBox(Control,Name+"MirrorBox"+Index,
// 		      0.0,53.1,23.6,29.5,124.0);


//   // length thick width
//   MirrGen.setPlate(50.0,1.0,9.0);  //guess
//   MirrGen.setPrimaryAngle(0,vAngle,0);
//   // ystep : zstep : theta : phi : radius
//   MirrGen.generateMirror(Control,Name+"MirrorFront"+Index,
// 			 -centreDist/2.0,0.0,theta,phi,0.0);         // hits beam center
//   MirrGen.setPrimaryAngle(0,vAngle+180.0,0.0);
//   MirrGen.generateMirror(Control,Name+"MirrorBack"+Index,
// 			 centreDist/2.0,heightDelta,theta,phi,0.0);
//   return;
// }

// void
// diagUnit(FuncDataBase& Control,const std::string& Name)
//   /*!
//     Construct variables for the diagnostic units
//     \param Control :: Database
//     \param Name :: component name
//   */
// {
//   ELog::RegMethod RegA("softimaxVariables[F]","diagUnit");


//   const double DLength(55.0);         // diag length [checked]
//   setVariable::PortTubeGenerator PTubeGen;
//   setVariable::PortItemGenerator PItemGen;

//   PTubeGen.setMat("Stainless304");

//   // ports offset by 24.5mm in x direction
//   // length 425+ 75 (a) 50 b
//   PTubeGen.setCF<CF63>();
//   PTubeGen.setBPortCF<CF40>();
//   PTubeGen.setBFlangeCF<CF63>();
//   PTubeGen.setPortLength(-5.0,-7.5);
//   PTubeGen.setAPortOffset(2.45,0);
//   PTubeGen.setBPortOffset(2.45,0);

//   // ystep/radius length
//   PTubeGen.generateTube(Control,Name,0.0,7.5,DLength);
//   Control.addVariable(Name+"NPorts",7);

//   const std::string portName=Name+"Port";
//   const Geometry::Vec3D MidPt(0,0,0);
//   const Geometry::Vec3D XVec(1,0,0);
//   const Geometry::Vec3D ZVec(0,0,1);
//   const Geometry::Vec3D PPos(0.0,DLength/4.0,0);

//   PItemGen.setOuterVoid(1);
//   PItemGen.setCF<setVariable::CF40>(2.0);
//   PItemGen.generatePort(Control,portName+"0",-PPos,ZVec);
//   PItemGen.setCF<setVariable::CF63>(4.0);
//   PItemGen.generatePort(Control,portName+"1",MidPt,ZVec);
//   PItemGen.generatePort(Control,portName+"2",PPos,ZVec);
//   // view port
//   PItemGen.setCF<setVariable::CF63>(8.0);
//   PItemGen.generatePort(Control,portName+"3",
// 			Geometry::Vec3D(0,DLength/4.5,0),
// 			Geometry::Vec3D(-1,-1,0));

//     //  flange for diamond filter view
//   PItemGen.setCF<setVariable::CF40>(4.0);
//   PItemGen.generatePort(Control,portName+"4",
// 			Geometry::Vec3D(0,0.3*DLength,0),XVec);
//   PItemGen.generatePort(Control,portName+"5",
// 			Geometry::Vec3D(0,0.3*DLength,0),-XVec);

//   // ion pump port
//   PItemGen.setCF<setVariable::CF100>(7.5);
//   PItemGen.generatePort(Control,portName+"6",MidPt,-ZVec);

//   return;
// }

// void
// diagUnit2(FuncDataBase& Control,const std::string& Name)
//   /*!
//     Construct variables for the small diagnostic units
//     \param Control :: Database
//     \param Name :: component name
//   */
// {
//   ELog::RegMethod RegA("softimaxVariables[F]","diagUnit");


//   const double DLength(40.0);         // diag length [checked+5cm]
//   setVariable::PortTubeGenerator PTubeGen;
//   setVariable::PortItemGenerator PItemGen;

//   PTubeGen.setMat("Stainless304");

//   // ports offset by 24.5mm in x direction
//   // length 425+ 75 (a) 50 b
//   PTubeGen.setCF<CF63>();
//   PTubeGen.setPortLength(-5.0,-5.0);
//   // ystep/radius length
//   PTubeGen.generateTube(Control,Name,0.0,7.5,DLength);
//   Control.addVariable(Name+"NPorts",4);

//   const std::string portName=Name+"Port";
//   const Geometry::Vec3D MidPt(0,0,0);
//   const Geometry::Vec3D XVec(1,0,0);
//   const Geometry::Vec3D ZVec(0,0,1);
//   const Geometry::Vec3D PPos(0.0,DLength/6.0,0);

//   PItemGen.setOuterVoid(1);  // create boundary round flange
//   PItemGen.setCF<setVariable::CF63>(5.0);
//   PItemGen.generatePort(Control,portName+"0",-PPos,ZVec);
//   PItemGen.setCF<setVariable::CF63>(5.0);
//   PItemGen.generatePort(Control,portName+"1",MidPt,XVec);
//   PItemGen.generatePort(Control,portName+"2",PPos,ZVec);
//   // view port
//   PItemGen.setCF<setVariable::CF63>(8.0);
//   PItemGen.generatePort(Control,portName+"3",
// 			Geometry::Vec3D(0,DLength/5.0,0),
// 			Geometry::Vec3D(-1,-1,0));

//     //  flange for diamond filter view
//   PItemGen.setCF<setVariable::CF40>(4.0);
//   PItemGen.generatePort(Control,portName+"4",
// 			Geometry::Vec3D(0,0.3*DLength,0),XVec);
//   PItemGen.generatePort(Control,portName+"5",
// 			Geometry::Vec3D(0,0.3*DLength,0),-XVec);

//   // ion pump port
//   PItemGen.setCF<setVariable::CF100>(7.5);
//   PItemGen.generatePort(Control,portName+"6",MidPt,-ZVec);

//   JawFlangeGenerator JFlanGen;
//   JFlanGen.generateFlange(Control,Name+"JawUnit0");
//   JFlanGen.generateFlange(Control,Name+"JawUnit1");

//   return;
// }

void
m1MirrorVariables(FuncDataBase& Control,
		  const std::string& mirrorKey)
/*!
  Builds the variables for the M1 Mirror
  \param Control :: Database
  \param mirrorKey :: prename
*/
{
  ELog::RegMethod RegA("softimaxVariables[F]","m1MirrorVariables");

  setVariable::PipeGenerator PipeGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::GateValveGenerator GateGen;
  setVariable::MirrorGenerator MirrGen;

  PipeGen.setMat("Stainless304");
  PipeGen.setNoWindow();
  PipeGen.setCF<setVariable::CF63>();
  PipeGen.setBFlangeCF<CF150>();
  PipeGen.generatePipe(Control,mirrorKey+"OffPipeA",0,6.8);
  Control.addVariable(mirrorKey+"OffPipeAFlangeBackXYAngle",-4.0);
  Control.addVariable(mirrorKey+"OffPipeAFlangeBackXStep",-2.0);

  const std::string mName=mirrorKey+"M1Tube";
  const double centreOffset(sin(M_PI*4.0/180.0)*6.8/2);  // half 6.8
  SimpleTubeGen.setCF<CF150>();
  SimpleTubeGen.generateTube(Control,mName,0.0,36.0);  // centre 13.5cm
  Control.addVariable(mName+"XStep",centreOffset);
  Control.addVariable(mName+"NPorts",0);   // beam ports

  PipeGen.setCF<setVariable::CF63>();
  PipeGen.setAFlangeCF<setVariable::CF150>();
  PipeGen.generatePipe(Control,mirrorKey+"OffPipeB",0,13.8);
  Control.addVariable(mirrorKey+"OffPipeBFlangeFrontXStep",-2.0);
  Control.addVariable(mirrorKey+"OffPipeBXStep",2.0);



  // mirror in M1Tube
  MirrGen.setPlate(28.0,1.0,9.0);  //guess
  // y/z/theta/phi/radius
  MirrGen.generateMirror(Control,mirrorKey+"M1Mirror",0.0, 0.0, 2.0, 0.0,0.0);
  Control.addVariable(mirrorKey+"M1MirrorYAngle",90.0);

  Control.addVariable(mirrorKey+"M1StandHeight",110.0);
  Control.addVariable(mirrorKey+"M1StandWidth",30.0);
  Control.addVariable(mirrorKey+"M1StandLength",30.0);
  Control.addVariable(mirrorKey+"M1StandMat","SiO2");

  // joined and open
  GateGen.setLength(7.5);
  GateGen.setCF<setVariable::CF63>();
  GateGen.generateValve(Control,mirrorKey+"GateA",0.0,0);

  return;
}

void
opticsSlitPackage(FuncDataBase& Control,
		  const std::string& opticsName)
  /*!
    Builds the DM2 slit package
    \param Control :: Function data base for variables
    \param opticsName :: PreNaem
   */
{
  setVariable::PortTubeGenerator PortTubeGen;
  setVariable::JawValveGenerator JawGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::BeamPairGenerator BeamMGen;

    /// SLIT PACKAGE

  const std::string sName=opticsName+"SlitTube";
  const double tLen(48.2);
  PortTubeGen.setPipeCF<CF200>();

  PortTubeGen.setPortCF<CF40>();
  PortTubeGen.setPortLength(-5.0,-5.0);
  PortTubeGen.generateTube(Control,sName,0.0,tLen);

  Control.addVariable(sName+"NPorts",3);   // beam ports (lots!!)
  PItemGen.setCF<setVariable::CF150>(6.1);
  PItemGen.setPlate(setVariable::CF150::flangeLength,"Stainless304");

  // Top port 16.0: Side 20.0cm  from front :  Vacuum 1/2 way
  //
  const Geometry::Vec3D topJaw(0.0,16.0-tLen/2.0,0.0);
  const Geometry::Vec3D sideJaw(0.0,20.0-tLen/2.0,0.0);
  const Geometry::Vec3D vacPort(0.0,0.0,0.0);

  const Geometry::Vec3D XVec(1,0,0);
  const Geometry::Vec3D ZVec(0,0,1);

  PItemGen.setOuterVoid(1); ///
  PItemGen.generatePort(Control,sName+"Port0",topJaw,ZVec);
  PItemGen.generatePort(Control,sName+"Port1",sideJaw,XVec);
  PItemGen.generatePort(Control,sName+"Port2",vacPort,-ZVec);
  //  PItemGen.generatePort(Control,sName+"Port3",CPt,ZVec);

  // Jaw units:
  BeamMGen.setThread(0.5,"Nickel");
  BeamMGen.setLift(0.0,2.5);
  BeamMGen.setGap(1.0,1.0);
  BeamMGen.setXYStep(0.6,0.0,-0.6,0);
  BeamMGen.setBlock(4.0,2.0,1.0,0.0,"Copper");

  const std::string jawKey[]={"JawX","JawZ"};
  for(size_t i=0;i<2;i++)
    {
      const std::string fname=opticsName+jawKey[i];
      BeamMGen.generateMount(Control,fname,1);  // outer of beam
    }

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
  ELog::RegMethod RegA("softimaxVariables[F]","opticsVariables");

  const std::string preName(beamName+"OpticsLine");
  const Geometry::Vec3D XVec(1,0,0);
  const Geometry::Vec3D ZVec(0,0,1);
  std::string Name;

  Control.addVariable(preName+"OuterLeft",70.0);
  Control.addVariable(preName+"OuterRight",70.0);
  Control.addVariable(preName+"OuterTop",70.0);

  setVariable::PipeGenerator PipeGen;
  setVariable::BellowGenerator BellowGen;
  setVariable::CrossGenerator CrossGen;
  setVariable::PortTubeGenerator PTubeGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::GateValveGenerator GateGen;
  setVariable::VacBoxGenerator VBoxGen;
  setVariable::FlangeMountGenerator FlangeGen;
  setVariable::BremCollGenerator BremGen;
  setVariable::BremMonoCollGenerator BremMonoGen;
  setVariable::JawFlangeGenerator JawFlangeGen;
  setVariable::DiffPumpGenerator DiffGen;

  PipeGen.setNoWindow();   // no window

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,preName+"InitBellow",0,6.0);

  // TODO:
  // use PortTubeGenerator instead
  // and set FlangeLength to 1.27 cm (instead of 0.5)
  Name=preName+"TriggerPipe";
  CrossGen.setPlates(0.3,2.0,2.0);  // wall/Top/base ???
  CrossGen.setPorts(-9.7,-9.7);     // len of ports - measured in the STEP file
  CrossGen.generateDoubleCF<setVariable::CF40,setVariable::CF100>
    (Control,Name,0.0,15.5,22.0);  // ystep/height/depth - measured

  CrossGen.setPorts(1.2,1.2);     // len of ports (after main)
  CrossGen.generateDoubleCF<setVariable::CF40,setVariable::CF63>
    (Control,preName+"GaugeA",0.0,10.6,8.0);  // ystep/height/depth

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.setBFlangeCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,preName+"BellowA",0,17.6);

  // will be rotated vertical
  const std::string pumpName=preName+"PumpM1";
  SimpleTubeGen.setCF<CF150>();
  SimpleTubeGen.setCap();
  SimpleTubeGen.generateTube(Control,pumpName,0.0,39.2); // full length (+caps)
  Control.addVariable(pumpName+"NPorts",7);

  PItemGen.setCF<setVariable::CF40>(5.3); // port length
  PItemGen.setPlate(0.0,"Void");
  PItemGen.generatePort(Control,pumpName+"Port0",Geometry::Vec3D(0,0,0),ZVec);

  PItemGen.setCF<setVariable::CF63>(5.4); // port length
  PItemGen.setPlate(0.0,"Void");
  PItemGen.generatePort(Control,pumpName+"Port1",Geometry::Vec3D(0,0,0),-ZVec);

  const Geometry::Vec3D pAngVec(0.75,0.0,cos(M_PI*37.0/180.0));
  const double PLen=14.0-8.05/cos(M_PI*37.0/180.0);
  PItemGen.setCF<setVariable::CF40>(PLen);
  PItemGen.setOuterVoid(0);
  PItemGen.generatePort(Control,pumpName+"Port2",
			Geometry::Vec3D(0,0,0),-pAngVec);

  PItemGen.setCF<setVariable::CF40>(5.4); // port length
  PItemGen.generatePort(Control,pumpName+"Port3",Geometry::Vec3D(0,0,0),XVec);

  PItemGen.setCF<setVariable::CF40>(5.4); // port length
  PItemGen.generatePort(Control,pumpName+"Port4",Geometry::Vec3D(0,10,0),XVec);

  // above port 2
  PItemGen.setCF<setVariable::CF40>(5.4); // port length
  PItemGen.generatePort(Control,pumpName+"Port5",Geometry::Vec3D(0,10,0),-XVec);

  const double xyAngle6(70.0*M_PI/180.0);
  const Geometry::Vec3D pAngVec6(sin(xyAngle6),0.0,-cos(xyAngle6));
  const double PLen6=14.0-8.05/cos(M_PI*37.0/180.0);
  PItemGen.setCF<setVariable::CF40>(PLen6);
  PItemGen.setOuterVoid(0);
  PItemGen.generatePort(Control,pumpName+"Port6",
			Geometry::Vec3D(0,0,0),-pAngVec6);

  // Gate valve A
  GateGen.setLength(2.5);
  GateGen.setCF<setVariable::CF40>();
  GateGen.generateValve(Control,preName+"GateA",0.0,0);

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.setBFlangeCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,preName+"BellowB",0,17.6);

  m1MirrorVariables(Control,preName);

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.setAFlangeCF<setVariable::CF63>();
  BellowGen.setBFlangeCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,preName+"BellowC",0,12.0);

  // will be rotated vertical
  const std::string collName=preName+"PumpTubeA";
  SimpleTubeGen.setCF<CF100>(); // counted 16 bolts
  SimpleTubeGen.setCap();
  SimpleTubeGen.generateTube(Control,collName,0.0,40.0);
  Control.addVariable(collName+"NPorts",2);   // beam ports

  PItemGen.setCF<setVariable::CF63>(5.95);
  PItemGen.setPlate(0.0,"Void");
  PItemGen.generatePort(Control,collName+"Port0",Geometry::Vec3D(0,0,0),ZVec);

  PItemGen.setCF<setVariable::CF63>(4.95);
  PItemGen.setPlate(0.0,"Void");
  PItemGen.generatePort(Control,collName+"Port1",Geometry::Vec3D(0,0,0),-ZVec);

  // Absolutely dummy Tungsten collimator.
  // TODO: ask about geometry
  BremGen.setCF<CF63>();
  BremGen.generateColl(Control,preName+"BremCollA",0,5.4);
  Control.addVariable(preName+"BremCollAExtLength", 0.8); // !!! UGLY
  Control.addVariable(preName+"BremCollAHoleMidDist", 3.78);

  GateGen.setLength(2.5);
  GateGen.setCF<setVariable::CF40>();
  GateGen.generateValve(Control,preName+"GateB",0.0,0);

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.setAFlangeCF<setVariable::CF63>();
  BellowGen.setBFlangeCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,preName+"BellowD",0,12.0);

  opticsSlitPackage(Control,preName);

  monoVariables(Control,preName);

  GateGen.setLength(2.5);
  GateGen.setCF<setVariable::CF40>();
  GateGen.generateValve(Control,preName+"GateC",0.0,0);

  BellowGen.setCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,preName+"BellowE",0,12.0);

  // will be rotated vertical
  const std::string pumpTubeBname=preName+"PumpTubeB";
  SimpleTubeGen.setCF<CF100>(); // counted 16 bolts
  SimpleTubeGen.setCap();
  SimpleTubeGen.generateTube(Control,pumpTubeBname,0.0,40.0);
  Control.addVariable(pumpTubeBname+"NPorts",2);   // beam ports

  PItemGen.setCF<setVariable::CF63>(5.95);
  PItemGen.setPlate(0.0,"Void");
  PItemGen.generatePort(Control,pumpTubeBname+"Port0",Geometry::Vec3D(0,0,0),ZVec);

  PItemGen.setCF<setVariable::CF63>(4.95);
  PItemGen.setPlate(0.0,"Void");
  PItemGen.generatePort(Control,pumpTubeBname+"Port1",Geometry::Vec3D(0,0,0),-ZVec);

  // GateGen.setLength(2.5);
  // GateGen.setCF<setVariable::CF40>();
  // GateGen.generateValve(Control,preName+"GateA",0.0,0);

  // PTubeGen.setMat("Stainless304");
  // PTubeGen.setCF<CF63>();
  // PTubeGen.setBPortCF<CF40>();
  // PTubeGen.setPortLength(-6.0,-5.0);
  // // ystep/radius length
  // PTubeGen.generateTube(Control,preName+"FilterBoxA",0.0,7.5,25.0);
  // Control.addVariable(preName+"FilterBoxANPorts",4);

  // PItemGen.setCF<setVariable::CF40>(4.0);
  // // 1/4 and 3/4 in main length: [total length 25.0-11.0]
  // Geometry::Vec3D PPos(0,3.5,0);
  // const Geometry::Vec3D XVec(-1,0,0);
  // const std::string portName=preName+"FilterBoxAPort";
  // PItemGen.generatePort(Control,portName+"0",PPos,XVec);
  // PItemGen.generatePort(Control,portName+"1",-PPos,XVec);

  // // ion pump port
  // PItemGen.setCF<setVariable::CF100>(7.5);
  // PItemGen.generatePort(Control,portName+"2",
  // 			Geometry::Vec3D(0,0,0),
  // 			Geometry::Vec3D(0,0,-1));
  // // Main flange for diamond filter
  // PItemGen.setCF<setVariable::CF63>(5.0);
  // PItemGen.generatePort(Control,portName+"3",
  // 			Geometry::Vec3D(0,0,0),
  // 			Geometry::Vec3D(0,0,1));

  // FlangeGen.setCF<setVariable::CF63>();
  // FlangeGen.setPlate(0.0,0.0,"Void");
  // FlangeGen.setBlade(3.0,5.0,0.5,0.0,"Graphite",1);
  // FlangeGen.generateMount(Control,preName+"FilterStick",1);  // in beam


  // SimpleTubeGen.setCF<CF40>();
  // SimpleTubeGen.setBFlangeCF<CF63>();
  // SimpleTubeGen.generateTube(Control,preName+"ScreenPipeA",0.0,12.5);
  // Control.addVariable(preName+"ScreenPipeANPorts",1);
  // PItemGen.setCF<setVariable::CF40>(4.0);
  // PItemGen.generatePort(Control,preName+"ScreenPipeAPort0",
  // 			Geometry::Vec3D(0,0,0),Geometry::Vec3D(1,0,0));


  // SimpleTubeGen.setCF<CF63>();
  // SimpleTubeGen.generateTube(Control,preName+"ScreenPipeB",0.0,14.0);
  // Control.addVariable(preName+"ScreenPipeBNPorts",2);
  // PItemGen.setCF<setVariable::CF63>(4.0);
  // PItemGen.setOuterVoid(0);
  // PItemGen.generatePort(Control,preName+"ScreenPipeBPort0",
  // 			Geometry::Vec3D(0,0,0),Geometry::Vec3D(-1,0,0));
  // PItemGen.generatePort(Control,preName+"ScreenPipeBPort1",
  // 			Geometry::Vec3D(0,0,0),Geometry::Vec3D(0,0,-1));


  // // Now add addaptor pipe:
  // PipeGen.setMat("Stainless304");
  // PipeGen.setCF<CF63>();
  // PipeGen.setBFlangeCF<CF150>();
  // PipeGen.generatePipe(Control,preName+"AdaptorPlateA",0.0,6.0);

  // // length
  // DiffGen.generatePump(Control,preName+"DiffPumpA",53.24);

  // VBoxGen.setMat("Stainless304");
  // VBoxGen.setWallThick(1.0);
  // VBoxGen.setCF<CF63>();
  // VBoxGen.setPortLength(2.5,2.5); // La/Lb
  // // ystep/width/height/depth/length
  // VBoxGen.generateBox(Control,preName+"PrimeJawBox",
  // 		      0.0,30.0,15.0,15.0,53.15);

  // GateGen.setCF<setVariable::CF63>();
  // GateGen.generateValve(Control,preName+"GateC",0.0,0);

  // GateGen.setCF<setVariable::CF63>();
  // GateGen.generateValve(Control,preName+"GateD",0.0,0);

  // BellowGen.setCF<setVariable::CF63>();
  // BellowGen.generateBellow(Control,preName+"BellowD",0,18.0);

  // softimaxVar::diagUnit(Control,preName+"DiagBoxA");
  // BremMonoGen.generateColl(Control,preName+"BremMonoCollA",0.0,10.0);

  // GateGen.setCF<setVariable::CF63>();
  // GateGen.generateValve(Control,preName+"GateE",0.0,0);

  // softimaxVar::mirrorBox(Control,preName,"A","Horrizontal",-0.2,0.0);

  // GateGen.setCF<setVariable::CF63>();
  // GateGen.generateValve(Control,preName+"GateF",0.0,0);

  // BellowGen.setCF<setVariable::CF63>();
  // BellowGen.generateBellow(Control,preName+"BellowF",0,12.0);

  // softimaxVar::diagUnit2(Control,preName+"DiagBoxB");

  // BellowGen.setCF<setVariable::CF63>();
  // BellowGen.generateBellow(Control,preName+"BellowG",0,12.0);

  // GateGen.setCF<setVariable::CF63>();
  // GateGen.generateValve(Control,preName+"GateG",0.0,0);

  // softimaxVar::mirrorBox(Control,preName,"B","Vertial",-0.2,0);

  // GateGen.setCF<setVariable::CF63>();
  // GateGen.generateValve(Control,preName+"GateH",0.0,0);

  // BellowGen.setCF<setVariable::CF63>();
  // BellowGen.generateBellow(Control,preName+"BellowH",0,18.0);

  // softimaxVar::diagUnit2(Control,preName+"DiagBoxC");

  // GateGen.setCF<setVariable::CF63>();
  // GateGen.generateValve(Control,preName+"GateI",0.0,0);

  // BellowGen.setCF<setVariable::CF63>();
  // BellowGen.generateBellow(Control,preName+"BellowI",0,18.0);

  // softimaxVar::monoShutterVariables(Control,preName);

  return;
}

void
exptVariables(FuncDataBase& Control,
		      const std::string& beamName)
  /*
    Components in the experimental hutch
    \param Control :: Function data base
    \param beamName :: Name of beamline
  */
{
  const std::string preName(beamName+"ExptLine");

  Control.addVariable(preName+"OuterLength",2300.0);
  Control.addVariable(preName+"OuterLeft",85.0);
  Control.addVariable(preName+"OuterRight",85.0);
  Control.addVariable(preName+"OuterTop",85.0);

  setVariable::BellowGenerator BellowGen;
  setVariable::GateValveGenerator GateGen;
  setVariable::JawValveGenerator JawGen;
  setVariable::PipeGenerator PipeGen;
  setVariable::MonoBoxGenerator VBoxGen;
  setVariable::DiffPumpGenerator DiffGen;
  setVariable::PortItemGenerator PItemGen;

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,preName+"InitBellow",0,6.0);

  // Gate valve A - round
  GateGen.setLength(2.5);
  GateGen.setCF<setVariable::CF40>();
  GateGen.generateValve(Control,preName+"GateA",0.0,0);

   // Double slits A and B
  JawGen.setCF<setVariable::CF100>();
  JawGen.setAPortCF<setVariable::CF40>();
  JawGen.setLength(4.0);
  JawGen.setRadius(4.0);
  JawGen.setSlits(3.0,2.0,0.2,"Tantalum");
  JawGen.generateSlits(Control,preName+"DoubleSlitA",0.0,0.8,0.8);

  JawGen.setCF<setVariable::CF100>();
  JawGen.setBPortCF<setVariable::CF40>();
  JawGen.setLength(4.0);
  JawGen.setRadius(4.0);
  JawGen.setSlits(3.0,2.0,0.2,"Tungsten");
  JawGen.generateSlits(Control,preName+"DoubleSlitB",0.0,0.8,0.8);

  VBoxGen.setMat("Stainless304");
  VBoxGen.setWallThick(1.0); // measured
  VBoxGen.setCF<CF63>();
  VBoxGen.setAPortCF<CF40>();
  VBoxGen.setPortLength(2.5,2.5); // La/Lb
  VBoxGen.setLids(3.5,1.5,1.5); // over/base/roof - all values are measured

  const std::string duName(preName+"DiagnosticUnit");

  // arguments: ystep/width/height/depth/length
  VBoxGen.generateBox(Control,duName,
		      0.0,22.0,8.5,8.5,43.0); // measured

  Control.addVariable(duName+"FilterHolder1YStep",8.2);
  Control.addVariable(duName+"FilterHolder1Thick",0.8); // measured
  Control.addVariable(duName+"FilterHolder1Height",1.8);
  Control.addVariable(duName+"FilterHolder1Depth",1.4);
  Control.addVariable(duName+"FilterHolder1Width",5.75);
  Control.addVariable(duName+"FilterHolder1Mat","Stainless304");
  Control.addVariable(duName+"FilterHolder1LegHeight",1.3);
  Control.addVariable(duName+"FilterHolder1LegWidth",1.5);
  Control.addVariable(duName+"FilterHolder1BaseHeight",1.0);
  Control.addVariable(duName+"FilterHolder1BaseWidth",6.5);
  Control.addVariable(duName+"FilterHolder1FoilThick",1.0); // arbitrary
  Control.addVariable(duName+"FilterHolder1FoilMat","Silicon300K"); // arbitrary
  Control.addVariable(duName+"FilterHolder1NWindows",5); // measured
  Control.addVariable(duName+"FilterHolder1WindowHeight",0.6); // measured
  Control.addVariable(duName+"FilterHolder1WindowDepth",0.6); // measured
  Control.addVariable(duName+"FilterHolder1WindowWidth",0.7); // measured

  Control.copyVarSet(duName+"FilterHolder1",duName+"FilterHolder2");
  Control.addVariable(duName+"FilterHolder2YStep",2.0);

  Control.copyVarSet(duName+"FilterHolder1",duName+"FilterHolder3");
  Control.addVariable(duName+"FilterHolder3YStep",2.0);

  // Gate valve B - flat
  GateGen.setLength(2.5);
  GateGen.setCF<setVariable::CF40>();
  GateGen.generateValve(Control,preName+"GateB",0.0,0);

  DiffGen.generatePump(Control,preName+"DiffPump",53.24);
  // NOTE: ACTIVE WINDOW:
  PipeGen.setCF<setVariable::CF40>();
  PipeGen.setWindow(2.7, 0.005);
  PipeGen.setAFlange(2.7,0.5);
  PipeGen.generatePipe(Control,preName+"TelescopicSystem",0,100.0);
  // In reality the window is made of 50 um diamond,
  // but void is a reasonable approximation for our needs:
  // Graphite#2 is Diamond (graphite with double density)
  Control.addVariable(preName+"TelescopicSystemWindowBackMat", "Diamond");
  Control.addVariable(preName+"TelescopicSystemWindowFrontMat", "Diamond");

  // sample area dimensions are arbitrary
  Control.addVariable(preName+"SampleAreaWidth",100.0);
  Control.addVariable(preName+"SampleAreaHeight",50.0);
  Control.addVariable(preName+"SampleAreaDepth",10.0);
  Control.addVariable(preName+"SampleAreaSampleRadius",0.0);  // sample not made
  Control.addVariable(preName+"SampleAreaAirMat","Air");
  Control.addVariable(preName+"SampleAreaSampleMat","Stainless304");

  const std::string tubeName(preName+"Tube");

  // X032_SoftiMAX_\(2019-02-11\)_dimensions.pdf:
  Control.addVariable(tubeName+"YStep", 454.748); // dummy

  const std::string noseName(tubeName+"NoseCone");

  Control.addVariable(noseName+"Length",35.0); // measured
  Control.addVariable(noseName+"MainMat","Void"); //
  Control.addVariable(noseName+"WallMat","Stainless304");
  Control.addVariable(noseName+"WallThick",1.0); // measured

  Control.addVariable(noseName+"FrontPlateWidth",12.0); // measured
  Control.addVariable(noseName+"FrontPlateHeight",12.0); // measured
  Control.addVariable(noseName+"FrontPlateThick",1.5); // measured

  Control.addVariable(noseName+"BackPlateWidth",38.0); // measured
  Control.addVariable(noseName+"BackPlateHeight",38.0); // measured
  Control.addVariable(noseName+"BackPlateThick",2.5); // measured
  Control.addVariable(noseName+"BackPlateRimThick",4.5); // measured

  Control.addVariable(noseName+"PipeRadius",4.0); // ??? guess
  Control.addVariable(noseName+"PipeLength",4.6); // measured
  Control.addVariable(noseName+"PipeWallThick",
		      static_cast<double>(setVariable::CF63::wallThick)); // guess ???
  Control.addVariable(noseName+"FlangeRadius",
		      static_cast<double>(setVariable::CF63::flangeRadius));
  Control.addVariable(noseName+"FlangeLength",2.6); // measured

  // front window
  Control.addVariable(noseName+"WindowRadius",setVariable::CF63::wallThick/2.0);
  Control.addVariable(noseName+"WindowThick",0.05);
  Control.addVariable(noseName+"WindowMat","Graphite");

  GateGen.setLength(10.0);
  GateGen.setCF<setVariable::CF40>();
  GateGen.generateValve(Control,tubeName+"GateA",0.0,0);
  Control.addVariable(tubeName+"GateARadius",17.0); // measured

  // [1] = x032_softimax_-2019-02-11-_dimensions.pdf
  // [2] = measured in X032_SoftiMAX_(2019-04-25).step
  Control.addVariable(tubeName+"StartPlateThick", 2.7); // [1]
  Control.addVariable(tubeName+"StartPlateRadius", 57.8);  // [1], 1156/2.0 mm

  // According to [1], the PortRadius is 50.2/2 = 25.1 cm, but here we set it to
  // 14.27 cm - the port radius of the gasket plate betwen GateA and StartPlate
  // (which we do not build)
  Control.addVariable(tubeName+"StartPlatePortRadius", 14.27);
  Control.addVariable(tubeName+"StartPlateMat", "Stainless304");


  const Geometry::Vec3D C(0,0,0);
  const Geometry::Vec3D C1(0,0.1,0);
  const Geometry::Vec3D C2(0,55.1,0);
  const Geometry::Vec3D C3(0,-50.7,0);

  const Geometry::Vec3D PX(1,0,0);
  const Geometry::Vec3D PY(0,1,0);
  const Geometry::Vec3D PZ(0,0,1);

  setVariable::PipeTubeGenerator SimpleTubeGen;
  SimpleTubeGen.setPipe(50.2,0.6,57.8,4.3);  // Rad,thick,Flange (Rad,len)

  std::string segName=tubeName+"Segment1";
  SimpleTubeGen.generateTube(Control,segName,0.0,167.2);
  Control.addVariable(segName+"NPorts",1);

  PItemGen.setCF<setVariable::CF350>(7.0);
  PItemGen.setPlate(CF350::flangeLength,"Stainless304");
  PItemGen.setOuterVoid(1);
  PItemGen.generatePort(Control,segName+"Port0",C1,PX);

  // segment 2:
  segName=tubeName+"Segment2";
  SimpleTubeGen.generateTube(Control,segName,0.0,176);
  Control.addVariable(segName+"NPorts",1);
  PItemGen.generatePort(Control,segName+"Port0",C,-PX);

  // segment 3: short without ports before the wall
  segName=tubeName+"Segment3";
  setVariable::PipeTubeGenerator WallTubeGen(SimpleTubeGen);
  WallTubeGen.setAFlange(57.8,3.7);
  WallTubeGen.setBFlange(70.0,1.0);
  // [2] 1 added to have distance 378.7 as in [1]
  WallTubeGen.generateTube(Control,segName,0.0,32.8+1.0);
  Control.addVariable(segName+"NPorts",0);


  // segment 4: longer with 2 ports right after the wall
  segName=tubeName+"Segment4";
  SimpleTubeGen.setAFlange(70.0,1.0);
  SimpleTubeGen.generateTube(Control,segName,0.0,238.2);

  Control.addVariable(segName+"NPorts",2);
  PItemGen.generatePort(Control,segName+"Port0",Geometry::Vec3D(0,38.2,0),PX);
  PItemGen.generatePort(Control,segName+"Port1",Geometry::Vec3D(0,-67.6,0),-PX);
  // segments 5-9 are the same length [5 has more ports]
  setVariable::PortItemGenerator PItemExtraGen(PItemGen);
  PItemExtraGen.setPort(19.0,17.8,0.6);          // len/rad/wall
  PItemExtraGen.setFlange(20.0,1.0);
  PItemExtraGen.setPlate(2.5,"Stainless304");

  // Segment 5
  segName=tubeName+"Segment5";
  SimpleTubeGen.setAFlange(57.8,4.3);   // set back to default
  SimpleTubeGen.generateTube(Control,segName,0.0,264.0);
  Control.addVariable(segName+"NPorts",5);

  const double alpha(30*M_PI/180);
  PItemGen.generatePort(Control,segName+"Port0",C2,PX);
  PItemGen.generatePort(Control,segName+"Port1",C3,-PX);
  PItemExtraGen.generatePort(Control,segName+"Port2",
			     Geometry::Vec3D(0,3.3,0),
			     Geometry::Vec3D(0,-sin(alpha),-cos(alpha)));
  PItemExtraGen.generatePort(Control,segName+"Port3",
			     Geometry::Vec3D(0,60.9,0),
			     Geometry::Vec3D(0,-sin(alpha),-cos(alpha)));

  PItemExtraGen.setPort(7.0,10.0,0.6);
  PItemExtraGen.setFlange(12.0,2.5);
  PItemExtraGen.generatePort(Control,segName+"Port4",
			     Geometry::Vec3D(0,-20.0,0),PX);

  // segments 6
  segName=tubeName+"Segment6";
  SimpleTubeGen.generateTube(Control,segName,0.0,264.0);
  Control.addVariable(segName+"NPorts",2);
  PItemGen.generatePort(Control,segName+"Port0",C2,PX);
  PItemGen.generatePort(Control,segName+"Port1",C3,-PX);

  // segments 7
  segName=tubeName+"Segment7";
  SimpleTubeGen.generateTube(Control,segName,0.0,264.0);
  Control.addVariable(segName+"NPorts",2);
  PItemGen.generatePort(Control,segName+"Port0",C2,PX);
  PItemGen.generatePort(Control,segName+"Port1",C3,-PX);

  // segments 8
  segName=tubeName+"Segment8";
  SimpleTubeGen.setAFlange(57.8,4.0);
  SimpleTubeGen.setBFlange(57.8,4.0);
  SimpleTubeGen.setFlangeCap(0.0,2.7);

  SimpleTubeGen.generateTube(Control,segName,0.0,264);
  //  SimpleTubeGen.setFlange(4.)
  Control.addVariable(segName+"NPorts",4);
  PItemGen.generatePort(Control,segName+"Port0",C2,PX);
  PItemGen.generatePort(Control,segName+"Port1",C3,-PX);

  PItemGen.setPort(6.6,4,1.0);  // len/rad/wall
  PItemGen.setFlange(8.3,2.0);  // rad/len
  PItemGen.setPlate(0.7,"Stainless304");
  PItemGen.generatePort(Control,segName+"Port2",
			Geometry::Vec3D(34.8,0.0,0),PY);
  PItemGen.generatePort(Control,segName+"Port3",
			Geometry::Vec3D(-34.8,0.0,0),PY);


  Control.addParse<double>(tubeName+"OuterRadius",
			   tubeName+"Segment3FlangeBRadius+10.0");
  Control.addParse<double>(tubeName+"OuterLength",
			   "SoftiMAXExptLineTubeNoseConeLength+"
			   "SoftiMAXExptLineTubeSegment1Length+"
			   "SoftiMAXExptLineTubeSegment2Length+"
			   "SoftiMAXExptLineTubeSegment3Length+"
			   "SoftiMAXExptLineTubeSegment4Length+"
			   "SoftiMAXExptLineTubeSegment5Length+"
			   "SoftiMAXExptLineTubeSegment6Length+"
			   "SoftiMAXExptLineTubeSegment7Length+"
			   "SoftiMAXExptLineTubeSegment8Length+"
			   "100.0");



  Control.addVariable(tubeName+"CableWidth",  20.0); // [2]
  Control.addVariable(tubeName+"CableHeight", 10.0); // [2]
  Control.addVariable(tubeName+"CableZStep",  -21.7); // [2]
  Control.addVariable(tubeName+"CableLength", 870.0); // dummy
  Control.addVariable(tubeName+"CableTailRadius", 17.0);
  Control.addVariable(tubeName+"CableMat", "StbTCABL"); // some generic cable material

  Control.addVariable(tubeName+"DetYStep", 0.0);

  Control.addVariable(tubeName+"BeamDumpLength", 0.6); // [2]
  Control.addVariable(tubeName+"BeamDumpRadius", 0.15); // [2]
  Control.addVariable(tubeName+"BeamDumpMat", "Tantalum");

  Control.addVariable(tubeName+"WAXSLength", 34.5); // [2]
  Control.addVariable(tubeName+"WAXSWidth", 23.52); // [2]
  Control.addVariable(tubeName+"WAXSHeight", 22.3); // [2]
  Control.addVariable(tubeName+"WAXSWallThick", 0.3); // [2]
  Control.addVariable(tubeName+"WAXSMainMat", "StbTCABL"); // guess
  Control.addVariable(tubeName+"WAXSWallMat", "Aluminium");
  Control.addVariable(tubeName+"WAXSYStep", 0.0);

  Control.addVariable(tubeName+"AirBoxLength", 32.0); // [2]
  Control.addVariable(tubeName+"AirBoxWidth", 30.8); // [2]
  Control.addVariable(tubeName+"AirBoxHeight", 53.0); // [2]
  Control.addVariable(tubeName+"AirBoxWallThick", 0.3); // [2]
  Control.addVariable(tubeName+"AirBoxMainMat", "Air");
  Control.addVariable(tubeName+"AirBoxWallMat", "Aluminium");

  Control.addVariable(tubeName+"CableWidth",  20.0); // [2]
  Control.addVariable(tubeName+"CableHeight", 10.0); // [2]
  Control.addVariable(tubeName+"CableZStep",  0.1);//-21.7); // [2]
  Control.addVariable(tubeName+"CableLength", 750.0); // dummy
  Control.addVariable(tubeName+"CableTailRadius", 17.0);
  Control.addParse<double>(tubeName+"CableDetYStep", tubeName+"WAXSYStep");
  Control.addVariable(tubeName+"CableMat", "StbTCABL"); // some generic cable material

  return;
}

}  // NAMESPACE softimaxVAR

void
SOFTIMAXvariables(FuncDataBase& Control)
  /*!
    Function to set the control variables and constants
    -- This version is for Photon Moderator
    \param Control :: Function data base to add constants too
  */
{
  ELog::RegMethod RegA("softimaxVariables[F]","softimaxVariables");

  Control.addVariable("sdefType","Wiggler");

  setVariable::PipeGenerator PipeGen;
  setVariable::LeadPipeGenerator LeadPipeGen;
  setVariable::PipeShieldGenerator ShieldGen;

  PipeGen.setWindow(-2.0,0.0);   // no window

  softimaxVar::undulatorVariables(Control,"SoftiMAXFrontBeam");

  // ystep / dipole pipe / exit pipe
  setVariable::R3FrontEndVariables
    (Control,"SoftiMAXFrontBeam",141.0,724.0,40.0);
  softimaxVar::frontMaskVariables(Control,"SoftiMAXFrontBeam");

  softimaxVar::wallVariables(Control,"SoftiMAXWallLead");

  PipeGen.setMat("Stainless304");
  PipeGen.setCF<setVariable::CF40>(); // was 2cm (why?)
  PipeGen.generatePipe(Control,"SoftiMAXJoinPipe",0,126.0);

  softimaxVar::opticsHutVariables(Control,"SoftiMAX");
  softimaxVar::opticsVariables(Control,"SoftiMAX");
  //  softimaxVar::exptHutVariables(Control,"SoftiMAX");
  softimaxVar::exptVariables(Control,"SoftiMAX");

  PipeGen.generatePipe(Control,"SoftiMAXJoinPipeB",0,100.0);

  ShieldGen.setPlate(60.0,60.0,10.0);
  ShieldGen.generateShield(Control,"SoftiMAXScreenA",4.4,0.0);

  return;
}

}  // NAMESPACE setVariable
