/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   formax/formaxVariables.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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

#include "CFFlanges.h"
#include "PipeGenerator.h"
#include "SplitPipeGenerator.h"
#include "BellowGenerator.h"
#include "LeadPipeGenerator.h"
#include "CrossGenerator.h"
#include "GateValveGenerator.h"
#include "HeatDumpGenerator.h"
#include "JawValveGenerator.h"
#include "PipeTubeGenerator.h"
#include "PortTubeGenerator.h"
#include "PortItemGenerator.h"
#include "VacBoxGenerator.h"
#include "MonoBoxGenerator.h"
#include "MonoShutterGenerator.h"
#include "FlangeMountGenerator.h"
#include "BeamMountGenerator.h"
#include "MirrorGenerator.h"
#include "CollGenerator.h"
#include "PortChicaneGenerator.h"
#include "MazeGenerator.h"
#include "RingDoorGenerator.h"
#include "LeadBoxGenerator.h"
#include "JawFlangeGenerator.h"
#include "BremCollGenerator.h"
#include "PipeShieldGenerator.h"
#include "WallLeadGenerator.h"

namespace setVariable
{

namespace formaxVar
{

void frontCaveVariables(FuncDataBase&,const std::string&,
			const bool,const bool);
void undulatorVariables(FuncDataBase&,const std::string&);
void wallVariables(FuncDataBase&,const std::string&);
void moveApertureTable(FuncDataBase&,const std::string&);
void collimatorVariables(FuncDataBase&,const std::string&);
void heatDumpTable(FuncDataBase&,const std::string&);
void heatDumpVariables(FuncDataBase&,const std::string&);
void shutterTable(FuncDataBase&,const std::string&);
  
void
frontCaveVariables(FuncDataBase& Control,
		   const std::string& preName,
		   const bool mazeFlag,
		   const bool doorFlag)
  /*!
    Variable for the main ring front shielding
    \param Control :: Database
    \param preName :: Name to describe system
    \param mazeFlag :: maze is present
    \param doorFlag :: door is present
  */
{
  ELog::RegMethod RegA("formaxVariables[F]","frontCaveVariables");

  MazeGenerator MGen;
  RingDoorGenerator RGen;
  
  Control.addVariable(preName+"Length",2400.0);
  Control.addVariable(preName+"OuterGap",140.0);
  Control.addVariable(preName+"RingGap",250.0);

  // If this is changed then need to change joinPipe as well
  Control.addVariable(preName+"FrontWallThick",160.0);
  Control.addVariable(preName+"OuterWallThick",100.0);
  Control.addVariable(preName+"RingWallThick",100.0);
  Control.addVariable(preName+"InnerRingWidth",400.0);

  Control.addVariable(preName+"FloorDepth",130.0);
  Control.addVariable(preName+"FloorThick",100.0);

  Control.addVariable(preName+"RoofHeight",180.0);
  Control.addVariable(preName+"RoofThick",100.0);

  Control.addVariable(preName+"SegmentAngle",18.0);
  Control.addParse<double>(preName+"SegmentLength",preName+"Length-735");
  Control.addVariable(preName+"SegmentThick",100.0);

  Control.addVariable(preName+"FrontHoleRadius",7.0);

  
  Control.addVariable(preName+"FrontWallMat","Concrete");
  Control.addVariable(preName+"WallMat","Concrete");
  Control.addVariable(preName+"FloorMat","Concrete");
  Control.addVariable(preName+"RoofMat","Concrete");


  if (mazeFlag)
    MGen.generateMaze(Control,preName+"Maze",0.0);
  if (doorFlag)
    RGen.generateDoor(Control,preName+"RingDoor",-800.0);
  return;
}

void
shutterTable(FuncDataBase& Control,
	     const std::string& frontKey)
  /*!
    Set the variables for the shutter table (number 3)
    \param Control :: DataBase to use
    \param frontKey :: name before part names
  */
{
  ELog::RegMethod RegA("formaxVariables[F]","shutterTable");

  setVariable::BellowGenerator BellowGen;
  setVariable::GateValveGenerator GateGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::PipeGenerator PipeGen;
  setVariable::BeamMountGenerator BeamMGen;
    
  // joined and open
  GateGen.setLength(3.5);
  GateGen.setCF<setVariable::CF40>();
  GateGen.generateValve(Control,frontKey+"GateA",0.0,0);
  
  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,frontKey+"BellowI",0,10.0);
  
  SimpleTubeGen.setCF<CF100>();
  SimpleTubeGen.setCap();
  SimpleTubeGen.generateTube(Control,frontKey+"FlorTubeA",0.0,16.0);

  // beam ports
  const std::string florName(frontKey+"FlorTubeA");
  Control.addVariable(florName+"NPorts",4);
  const Geometry::Vec3D XVec(1,0,0);
  const Geometry::Vec3D ZVec(0,0,1);

  PItemGen.setCF<setVariable::CF40>(1.0);
  PItemGen.setPlate(0.0,"Void");  
  PItemGen.generatePort(Control,florName+"Port0",Geometry::Vec3D(0,0,0),ZVec);
  PItemGen.generatePort(Control,florName+"Port1",Geometry::Vec3D(0,0,0),-ZVec);
  PItemGen.generatePort(Control,florName+"Port2",Geometry::Vec3D(0,0,0),XVec);
  PItemGen.generatePort(Control,florName+"Port3",Geometry::Vec3D(0,0,0),-XVec);

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,frontKey+"BellowJ",0,10.0);

  // will be rotated vertical
  const std::string gateName=frontKey+"GateTubeB";
  SimpleTubeGen.setCF<CF63>();
  SimpleTubeGen.setCap();
  SimpleTubeGen.generateTube(Control,frontKey+"GateTubeB",0.0,20.0);
  // beam ports
  Control.addVariable(gateName+"NPorts",2);
  PItemGen.setCF<setVariable::CF40>(0.45);
  PItemGen.setPlate(0.0,"Void");  
  PItemGen.generatePort(Control,gateName+"Port0",Geometry::Vec3D(0,0,0),ZVec);
  PItemGen.generatePort(Control,gateName+"Port1",Geometry::Vec3D(0,0,0),-ZVec);
  
  PipeGen.setMat("Stainless304");
  PipeGen.setWindow(-2.0,0.0);   // no window
  PipeGen.setCF<setVariable::CF40>();
  PipeGen.setBFlangeCF<setVariable::CF150>();
  PipeGen.generatePipe(Control,frontKey+"OffPipeA",0,6.8);
  Control.addVariable(frontKey+"OffPipeAFlangeBackZStep",3.0);


  const std::string shutterName=frontKey+"ShutterBox";
  const double sBoxLen(51.0);
  SimpleTubeGen.setCF<CF150>();
  SimpleTubeGen.generateTube(Control,shutterName,0.0,sBoxLen);
  Control.addVariable(frontKey+"ShutterBoxNPorts",2);
  
  // 20cm above port tube
  PItemGen.setCF<setVariable::CF50>(14.0);
  PItemGen.setPlate(setVariable::CF50::flangeLength,"Stainless304");
  // lift is actually 60mm [check]
  BeamMGen.setThread(1.0,"Nickel");
  BeamMGen.setLift(5.0,0.0);
  BeamMGen.setCentreBlock(6.0,6.0,20.0,0.0,"Tungsten");  

  // centre of mid point
  Geometry::Vec3D CPos(0,-sBoxLen/4.0,0);
  for(size_t i=0;i<2;i++)
    {
      const std::string name=frontKey+"ShutterBoxPort"+std::to_string(i);
      const std::string fname=frontKey+"Shutter"+std::to_string(i);
      
      PItemGen.generatePort(Control,name,CPos,ZVec);
      BeamMGen.generateMount(Control,fname,1);      // out of beam:upflag=1
      CPos+=Geometry::Vec3D(0,sBoxLen/2.0,0);
    }

  PipeGen.setCF<setVariable::CF63>();
  PipeGen.setAFlangeCF<setVariable::CF150>();
  PipeGen.generatePipe(Control,frontKey+"OffPipeB",0,21.0);
  Control.addVariable(frontKey+"OffPipeBFlangeFrontZStep",3.0);
  Control.addVariable(frontKey+"OffPipeBZStep",-3.0);

  Control.addVariable(frontKey+"BremBlockRadius",3.0);
  Control.addVariable(frontKey+"BremBlockLength",20.0);
  Control.addVariable(frontKey+"BremBlockHoleWidth",2.0);
  Control.addVariable(frontKey+"BremBlockHoleHeight",2.0);
  Control.addVariable(frontKey+"BremBlockMainMat","Tungsten");

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.setAFlangeCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,frontKey+"BellowK",0,11.05);
  
  
  return;
}


  
void
collimatorVariables(FuncDataBase& Control,
		    const std::string& collKey)
  /*!
    Builds the variables for the collimator
    \param Control :: Database
    \param collKey :: prename
  */
{
  ELog::RegMethod RegA("formaxVariables[F]","collimatorVariables");

  Control.addVariable(collKey+"Width",4.0);
  Control.addVariable(collKey+"Height",4.0);
  Control.addVariable(collKey+"Length",16.0);
  Control.addVariable(collKey+"InnerAWidth",1.2);
  Control.addVariable(collKey+"InnerAHeight",1.2);
  Control.addVariable(collKey+"InnerBWidth",1.2);
  Control.addVariable(collKey+"InnerBHeight",1.2);
  Control.addVariable(collKey+"Mat","Tantalum");

  return;
}

void
moveApertureTable(FuncDataBase& Control,
		  const std::string& frontKey)
  /*!
    Builds the variables for the moveable apperature table
    containing two movable aperatures, pumping and bellows
    \param Control :: Database
    \param frontKey :: prename
  */
{
  ELog::RegMethod RegA("maxpeemVariables[F]","moveAperatureTable");

  setVariable::BellowGenerator BellowGen;
  setVariable::PipeGenerator PipeGen;
  setVariable::CrossGenerator CrossGen;

  PipeGen.setWindow(-2.0,0.0);   // no window
  PipeGen.setMat("Stainless304");
  PipeGen.setCF<CF40>();
  PipeGen.setBFlangeCF<CF63>();
  PipeGen.generatePipe(Control,frontKey+"PipeB",0,15.0);

  BellowGen.setCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,frontKey+"BellowE",0,14.0);

  // Aperature pipe is movable:
  PipeGen.setCF<CF63>();
  PipeGen.generatePipe(Control,frontKey+"AperturePipe",14.0,24.0);
  collimatorVariables(Control,frontKey+"MoveCollA");
  
  BellowGen.setCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,frontKey+"BellowF",0,14.0);

  // Stepped 420mm from pipeB so bellows/aperaturePipe can move freely
  CrossGen.setMat("Stainless304");
  CrossGen.setPlates(0.5,2.0,2.0);  // wall/Top/base
  CrossGen.setTotalPorts(7.0,7.0);     // len of ports (after main)
  CrossGen.generateDoubleCF<setVariable::CF63,setVariable::CF100>
    (Control,frontKey+"IonPC",52.0,15.74,28.70);   // height/depth

  // [FREE FLOATING]
  BellowGen.setCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,frontKey+"BellowG",0,14.0);

  // Aperature pipe is movable:
  PipeGen.setCF<CF63>();
  PipeGen.generatePipe(Control,frontKey+"AperturePipeB",14.0,24.0);
  collimatorVariables(Control,frontKey+"MoveCollB");
  Control.addVariable(frontKey+"MoveCollBYAngle",180.0);
  
  // [FREE FLOATING]
  BellowGen.setCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,frontKey+"BellowH",0,14.0);

  // [End fix for BellowH]
  PipeGen.setCF<CF40>();
  PipeGen.setAFlangeCF<CF63>();
  PipeGen.generatePipe(Control,frontKey+"PipeC",52.0,10.0);
  
  return;
}

void
heatDumpTable(FuncDataBase& Control,
	      const std::string& frontKey)
  /*!
    Builds the variables for the heat dump table
    containing the heatdump and a gate valve [non-standard]
    \param Control :: Database
    \param frontKey :: prename
  */
{
  ELog::RegMethod RegA("formaxVariables[F]","heatDumpTable");

  setVariable::BellowGenerator BellowGen;
  setVariable::PipeGenerator PipeGen;
  setVariable::CrossGenerator CrossGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::PortItemGenerator PItemGen;
    
  PipeGen.setWindow(-2.0,0.0);   // no window
  PipeGen.setMat("Stainless304");
  
  heatDumpVariables(Control,frontKey);

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,frontKey+"BellowD",0,10.0);

  // will be rotated vertical
  const std::string gateName=frontKey+"GateTubeA";
  SimpleTubeGen.setCF<CF63>();
  SimpleTubeGen.setCap();
  SimpleTubeGen.generateTube(Control,frontKey+"GateTubeA",0.0,20.0);

  // beam ports
  Control.addVariable(gateName+"NPorts",2);
  const Geometry::Vec3D ZVec(0,0,1);
  PItemGen.setCF<setVariable::CF40>(0.45);
  PItemGen.setPlate(0.0,"Void");  
  PItemGen.generatePort(Control,gateName+"Port0",Geometry::Vec3D(0,0,0),ZVec);
  PItemGen.generatePort(Control,gateName+"Port1",Geometry::Vec3D(0,0,0),-ZVec);

  CrossGen.setMat("Stainless304");
  CrossGen.setPlates(0.5,2.0,2.0);  // wall/Top/base
  CrossGen.setTotalPorts(10.0,10.0);     // len of ports (after main)
  CrossGen.generateDoubleCF<setVariable::CF40,setVariable::CF100>
    (Control,frontKey+"IonPB",0.0,26.6,26.6);

  return;
}

 
void
heatDumpVariables(FuncDataBase& Control,const std::string& frontKey)
  /*!
    Build the heat dump variables
    \param Control :: Database
    \param frontKey :: prename
   */
{
  ELog::RegMethod RegA("formaxVariables","heatDumpVariables");
  
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::HeatDumpGenerator HeatGen;

  SimpleTubeGen.setMat("Stainless304");
  SimpleTubeGen.setCF<CF150>();
  SimpleTubeGen.setCap(1,0);
  SimpleTubeGen.generateTube(Control,frontKey+"HeatBox",0.0,20.0);
  Control.addVariable(frontKey+"HeatBoxNPorts",2);

  // beam ports
  PItemGen.setCF<setVariable::CF40>(5.0);
  PItemGen.setPlate(0.0,"Void");  

  const Geometry::Vec3D ZVec(0,0,1);
  const std::string heatName=frontKey+"HeatBoxPort";
  const std::string hName=frontKey+"HeatDumpFlange";
  PItemGen.generatePort(Control,heatName+"0",Geometry::Vec3D(0,0,0),ZVec);
  PItemGen.generatePort(Control,heatName+"1",Geometry::Vec3D(0,0,0),-ZVec);
  
  const std::string hDump(frontKey+"HeatDump");
  HeatGen.setCF<CF100>();
  HeatGen.setTopCF<CF150>();
  HeatGen.generateHD(Control,hDump,1);
  
  return;
}

void
undulatorVariables(FuncDataBase& Control,
		   const std::string& undKey)
  /*!
    Builds the variables for the collimator
    \param Control :: Database
    \param undKey :: prename
  */
{
  ELog::RegMethod RegA("formaxVariables[F]","undulatorVariables");
  setVariable::PipeGenerator PipeGen;

  const double L(400.0);
  PipeGen.setMat("Aluminium");
  PipeGen.setNoWindow();   // no window
  PipeGen.setCF<setVariable::CF63>();
  PipeGen.generatePipe(Control,undKey+"UPipe",0,L);

  Control.addVariable(undKey+"UPipeWidth",6.0);
  Control.addVariable(undKey+"UPipeHeight",0.6);
  Control.addVariable<double>(undKey+"UPipeYStep",20.0);
  Control.addVariable(undKey+"UPipeFeThick",0.2);

  // undulator  
  Control.addVariable(undKey+"UndulatorVGap",1.1);  // mininum 11mm
  Control.addVariable(undKey+"UndulatorLength",370.0);   // 46.2mm*30*2
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
wallVariables(FuncDataBase& Control,
	      const std::string& wallKey)
/*!
    Set the variables for the frontend wall
    \param Control :: DataBase to use
    \param wallKey :: name before part names
  */
{
  ELog::RegMethod RegA("formaxVariables[F]","wallVariables");

  WallLeadGenerator LGen;
  LGen.setWidth(140.0,70.0);
  LGen.generateWall(Control,wallKey,3.0);

  return;
}

void
frontEndVariables(FuncDataBase& Control,
		  const std::string& frontKey)
/*!
    Set the variables for the front end
    \param Control :: DataBase to use
    \param frontKey :: name before part names
  */
{
  ELog::RegMethod RegA("formaxVariables[F]","frontEndVariables");

  setVariable::BellowGenerator BellowGen;
  setVariable::PipeGenerator PipeGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::VacBoxGenerator VBoxGen;
  setVariable::CollGenerator CollGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::FlangeMountGenerator FlangeGen;

  Control.addVariable(frontKey+"OuterRadius",60.0);  

  PipeGen.setWindow(-2.0,0.0);   // no window
  PipeGen.setMat("Stainless304");

  undulatorVariables(Control,frontKey);
  

  PipeGen.setCF<CF40>();
  PipeGen.generatePipe(Control,frontKey+"DipolePipe",0,911.0);

  BellowGen.setCF<setVariable::CF63>();
  BellowGen.setBFlangeCF<setVariable::CF100>();
  BellowGen.generateBellow(Control,frontKey+"BellowA",0,16.0);

  SimpleTubeGen.setMat("Stainless304");
  SimpleTubeGen.setCF<CF100>();
  SimpleTubeGen.generateTube(Control,frontKey+"CollimatorTubeA",0.0,36.0);
  Control.addVariable(frontKey+"CollimatorTubeANPorts",0);
  // collimator block

  CollGen.setFrontGap(2.62,1.86);  //1033.8
  CollGen.setBackGap(1.54,1.42);
  CollGen.setMinSize(29.0,0.55,0.55);  // Approximated to get 1mrad x 1mrad

  CollGen.generateColl(Control,frontKey+"CollA",0.0,34.0);

  BellowGen.setCF<setVariable::CF63>();
  BellowGen.setAFlangeCF<setVariable::CF100>();
  BellowGen.generateBellow(Control,frontKey+"BellowB",0,16.0);

  
  PipeGen.setCF<CF100>();
  PipeGen.generatePipe(Control,frontKey+"CollABPipe",0,432.0);

  Control.addVariable(frontKey+"ECutDiskYStep",2.0);
  Control.addVariable(frontKey+"ECutDiskLength",0.1);
  Control.addVariable(frontKey+"ECutDiskRadius",1.0);
  Control.addVariable(frontKey+"ECutDiskDefMat","H2Gas#0.1");

  BellowGen.setCF<setVariable::CF63>();
  BellowGen.setBFlangeCF<setVariable::CF100>();
  BellowGen.generateBellow(Control,frontKey+"BellowC",0,16.0);

  SimpleTubeGen.setCF<CF100>();
  SimpleTubeGen.generateTube(Control,frontKey+"CollimatorTubeB",0.0,36.0);
  Control.addVariable(frontKey+"CollimatorTubeBNPorts",0);

  CollGen.setFrontGap(2.13,2.146);
  CollGen.setBackGap(0.756,0.432);
  CollGen.setMinSize(32.0,0.680,0.358);
  CollGen.generateColl(Control,frontKey+"CollB",0.0,34.2);

  // linked pipe tube
  SimpleTubeGen.setCF<CF40>();
  SimpleTubeGen.setAFlangeCF<CF100>();
  SimpleTubeGen.generateTube(Control,frontKey+"CollimatorTubeC",0.0,22.0);
  Control.addVariable(frontKey+"CollimatorTubeCNPorts",0);

  CollGen.setMain(1.20,"Copper","Void");
  CollGen.setFrontGap(0.84,0.582);
  CollGen.setBackGap(0.750,0.357);
  // approx for 100uRad x 100uRad
  CollGen.setMinSize(12.0,0.15,0.15);
  
  CollGen.generateColl(Control,frontKey+"CollC",0.0,17.0);

  PipeGen.setCF<setVariable::CF40>(); 
  PipeGen.generatePipe(Control,frontKey+"CollExitPipe",0,95.0);

  // Create HEAT DUMP
  heatDumpTable(Control,frontKey);
  moveApertureTable(Control,frontKey);
  shutterTable(Control,frontKey);
  
  PipeGen.setCF<setVariable::CF40>(); 
  PipeGen.generatePipe(Control,frontKey+"ExitPipe",0,50.0);

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
  ELog::RegMethod RegA("formaxVariables","opticsHutVariables");
  
  Control.addVariable(hutName+"Depth",100.0);
  Control.addVariable(hutName+"Height",200.0);
  Control.addVariable(hutName+"Length",834.6);
  Control.addVariable(hutName+"OutWidth",250.0);
  Control.addVariable(hutName+"RingWidth",60.0);
  Control.addVariable(hutName+"RingWallLen",105.0);
  Control.addVariable(hutName+"RingWallAngle",18.50);
  Control.addVariable(hutName+"RingConcThick",100.0);
  
  Control.addVariable(hutName+"InnerThick",0.3);
  
  Control.addVariable(hutName+"PbWallThick",1.6);
  Control.addVariable(hutName+"PbRoofThick",1.6);
  Control.addVariable(hutName+"PbFrontThick",1.2);
  Control.addVariable(hutName+"PbBackThick",9.0);

  Control.addVariable(hutName+"OuterThick",0.3);

  Control.addVariable(hutName+"FloorThick",50.0);
  Control.addVariable(hutName+"InnerOutVoid",10.0);
  Control.addVariable(hutName+"OuterOutVoid",10.0);

  Control.addVariable(hutName+"SkinMat","Stainless304");
  Control.addVariable(hutName+"RingMat","Concrete");
  Control.addVariable(hutName+"PbMat","Lead");
  Control.addVariable(hutName+"FloorMat","Concrete");

  Control.addVariable(hutName+"HoleXStep",0.0);
  Control.addVariable(hutName+"HoleZStep",5.0);
  //Control.addVariable(hutName+"HoleRadius",3.5);

  Control.addVariable(hutName+"InletXStep",0.0);
  Control.addVariable(hutName+"InletZStep",0.0);
  Control.addVariable(hutName+"InletRadius",5.0);


  Control.addVariable(hutName+"NChicane",2);
  PortChicaneGenerator PGen;
  PGen.generatePortChicane(Control,hutName+"Chicane0",270.0,-25.0);
  PGen.generatePortChicane(Control,hutName+"Chicane1",370.0,-25.0);



  return;
}

void
mirrorBox(FuncDataBase& Control,const std::string& Name)
  /*!
    Construct variables for the mirror boxes
    \param Control :: Database
    \param Name :: component name
  */
{
  ELog::RegMethod RegA("formaxVariables[F]","mirrorBox");
  
  setVariable::MonoBoxGenerator VBoxGen;

  VBoxGen.setMat("Stainless304");
  VBoxGen.setWallThick(1.0);
  VBoxGen.setCF<CF63>();
  VBoxGen.setPortLength(2.5,2.5); // La/Lb
  VBoxGen.setLids(3.0,1.0,1.0); // over/base/roof
  VBoxGen.setBPortOffset(2.5,0); // X/Z

  // ystep/width/height/depth/length
  VBoxGen.generateBox(Control,Name,0.0,53.1,23.6,29.5,124.0);


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
  ELog::RegMethod RegA("formaxVariables[F]","diagUnit");


  const double DLength(55.0);         // diag length [checked]
  setVariable::PortTubeGenerator PTubeGen;
  setVariable::PortItemGenerator PItemGen;
  
  PTubeGen.setMat("Stainless304");

  // ports offset by 24.5mm in x direction
  // length 425+ 75 (a) 50 b
  PTubeGen.setPipe(7.5,0.5);
  PTubeGen.setPortCF<CF63>();
  PTubeGen.setBPortCF<CF40>();
  PTubeGen.setBFlangeCF<CF63>();
  PTubeGen.setPortLength(-5.0,-7.5);
  PTubeGen.setAPortOffset(2.45,0);
  PTubeGen.setBPortOffset(2.45,0);
  
  // ystep/radius length
  PTubeGen.generateTube(Control,Name,0.0,DLength);
  Control.addVariable(Name+"NPorts",7);

  const std::string portName=Name+"Port";
  const Geometry::Vec3D MidPt(0,0,0);
  const Geometry::Vec3D XVec(1,0,0);
  const Geometry::Vec3D ZVec(0,0,1);
  const Geometry::Vec3D PPos(0.0,DLength/4.0,0);

  PItemGen.setOuterVoid(1);
  PItemGen.setCF<setVariable::CF40>(2.0);
  PItemGen.generatePort(Control,portName+"0",-PPos,ZVec);
  PItemGen.setCF<setVariable::CF63>(4.0);
  PItemGen.generatePort(Control,portName+"1",MidPt,ZVec);
  PItemGen.generatePort(Control,portName+"2",PPos,ZVec);
  // view port
  PItemGen.setCF<setVariable::CF63>(8.0);
  PItemGen.generatePort(Control,portName+"3",
			Geometry::Vec3D(0,DLength/4.5,0),
			Geometry::Vec3D(-1,-1,0));

    //  flange for diamond filter view
  PItemGen.setCF<setVariable::CF40>(4.0);
  PItemGen.generatePort(Control,portName+"4",
			Geometry::Vec3D(0,0.3*DLength,0),XVec);
  PItemGen.generatePort(Control,portName+"5",
			Geometry::Vec3D(0,0.3*DLength,0),-XVec);

  // ion pump port
  PItemGen.setCF<setVariable::CF100>(7.5);
  PItemGen.generatePort(Control,portName+"6",MidPt,-ZVec);

  return;
}

void
diagUnit2(FuncDataBase& Control,const std::string& Name)
  /*!
    Construct variables for the small diagnostic units
    \param Control :: Database
    \param Name :: component name
  */
{
  ELog::RegMethod RegA("formaxVariables[F]","diagUnit");


  const double DLength(35.0);         // diag length [checked]
  setVariable::PortTubeGenerator PTubeGen;
  setVariable::PortItemGenerator PItemGen;
  
  PTubeGen.setMat("Stainless304");

  // ports offset by 24.5mm in x direction
  // length 425+ 75 (a) 50 b
  PTubeGen.setPipe(7.5,0.5);
  PTubeGen.setPortCF<CF40>();
  PTubeGen.setAFlangeCF<CF63>();
  PTubeGen.setPortLength(-5.0,-5.0);
  // ystep/radius length
  PTubeGen.generateTube(Control,Name,0.0,DLength);
  Control.addVariable(Name+"NPorts",4);

  const std::string portName=Name+"Port";
  const Geometry::Vec3D MidPt(0,0,0);
  const Geometry::Vec3D XVec(1,0,0);
  const Geometry::Vec3D ZVec(0,0,1);
  const Geometry::Vec3D PPos(0.0,DLength/6.0,0);

  PItemGen.setOuterVoid(1);  // create boundary round flange
  PItemGen.setCF<setVariable::CF63>(5.0);
  PItemGen.generatePort(Control,portName+"0",-PPos,ZVec);
  PItemGen.setCF<setVariable::CF63>(5.0);
  PItemGen.generatePort(Control,portName+"1",MidPt,XVec);
  PItemGen.generatePort(Control,portName+"2",PPos,ZVec);
  // view port
  PItemGen.setCF<setVariable::CF63>(8.0);
  PItemGen.generatePort(Control,portName+"3",
			Geometry::Vec3D(0,DLength/5.0,0),
			Geometry::Vec3D(-1,-1,0));

    //  flange for diamond filter view
  PItemGen.setCF<setVariable::CF40>(4.0);
  PItemGen.generatePort(Control,portName+"4",
			Geometry::Vec3D(0,0.3*DLength,0),XVec);
  PItemGen.generatePort(Control,portName+"5",
			Geometry::Vec3D(0,0.3*DLength,0),-XVec);

  // ion pump port
  PItemGen.setCF<setVariable::CF100>(7.5);
  PItemGen.generatePort(Control,portName+"6",MidPt,-ZVec);

  JawFlangeGenerator JFlanGen;
  JFlanGen.generateFlange(Control,Name+"JawUnit0");
  JFlanGen.generateFlange(Control,Name+"JawUnit1");

  return;
}
  
void
monoVariables(FuncDataBase& Control,
	      const std::string& Name)
  /*!
    Set the variables for the mono
    \param Control :: DataBase to use
    \param Name :: component name
  */
{
  ELog::RegMethod RegA("formaxVariables[F]","monoVariables");
  
  
  setVariable::MonoBoxGenerator VBoxGen;

  VBoxGen.setMat("Stainless304");
  VBoxGen.setWallThick(1.0);
  VBoxGen.setCF<CF63>();
  VBoxGen.setAPortCF<CF40>();
  VBoxGen.setPortLength(2.5,2.5); // La/Lb
  VBoxGen.setBPortOffset(2.5,0); // La/Lb
  VBoxGen.setLids(3.0,1.0,1.0); // over/base/roof

  // ystep/width/height/depth/length
  // height+depth == 452mm  -- 110/ 342
  VBoxGen.generateBox(Control,Name+"MonoBox",0.0,77.2,11.0,34.20,95.1);

    // CRYSTALS:
  Control.addVariable(Name+"MonoXtalYAngle",90.0);
  Control.addVariable(Name+"MonoXtalZStep",-1.25);
  Control.addVariable(Name+"MonoXtalGap",4.0);
  Control.addVariable(Name+"MonoXtalTheta",10.0);
  Control.addVariable(Name+"MonoXtalPhiA",0.0);
  Control.addVariable(Name+"MonoXtalPhiA",0.0);
  Control.addVariable(Name+"MonoXtalWidth",10.0);
  Control.addVariable(Name+"MonoXtalLengthA",8.0);
  Control.addVariable(Name+"MonoXtalLengthB",12.0);
  Control.addVariable(Name+"MonoXtalThickA",4.0);
  Control.addVariable(Name+"MonoXtalThickB",3.0);
  Control.addVariable(Name+"MonoXtalBaseThick",5.0);
  Control.addVariable(Name+"MonoXtalBaseExtra",2.0);
  
  Control.addVariable(Name+"MonoXtalMat","Silicon80K");
  Control.addVariable(Name+"MonoXtalBaseMat","Copper");

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
  ELog::RegMethod RegA("formaxVariables[F]","opticsVariables");

  const std::string preName(beamName+"OpticsLine");

  Control.addVariable(preName+"OuterLeft",70.0);
  Control.addVariable(preName+"OuterRight",50.0);
  Control.addVariable(preName+"OuterTop",60.0);

  setVariable::PipeGenerator PipeGen;
  setVariable::BellowGenerator BellowGen;
  setVariable::CrossGenerator CrossGen;
  setVariable::PortTubeGenerator PTubeGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::GateValveGenerator GateGen;
  setVariable::JawValveGenerator JawGen;
  setVariable::VacBoxGenerator VBoxGen;
  setVariable::FlangeMountGenerator FlangeGen;
  setVariable::BremCollGenerator BremGen;
  setVariable::JawFlangeGenerator JawFlangeGen;
  setVariable::MonoShutterGenerator MShutterGen;
  
  PipeGen.setWindow(-2.0,0.0);   // no window

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.setAFlangeCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,preName+"InitBellow",0,6.0);

  CrossGen.setPlates(0.5,2.0,2.0);  // wall/Top/base
  CrossGen.setPorts(-9.0,-9.0);     // len of ports (after main)
  CrossGen.generateDoubleCF<setVariable::CF40,setVariable::CF100>
    (Control,preName+"TriggerPipe",0.0,15.0,15.0);  // ystep/height/depth
  
  CrossGen.setPorts(1.2,1.2);     // len of ports (after main)
  CrossGen.generateDoubleCF<setVariable::CF40,setVariable::CF63>
    (Control,preName+"GaugeA",0.0,11.0,11.0);  // ystep/height/depth

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.setBFlangeCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,preName+"BellowA",0,17.0);

  BremGen.setCF<CF63>();
  BremGen.setAperature(1.0,1.0,0.2,0.2,0.3,0.3);
  BremGen.generateColl(Control,preName+"BremCollA",0,8.0);

  PTubeGen.setMat("Stainless304");
  PTubeGen.setPipe(7.5,0.5);
  PTubeGen.setPortCF<CF63>();
  PTubeGen.setBPortCF<CF40>();
  PTubeGen.setPortLength(-6.0,-5.0);
  // ystep/radius length
  PTubeGen.generateTube(Control,preName+"FilterBoxA",0.0,25.0);
  Control.addVariable(preName+"FilterBoxANPorts",4);
  
  PItemGen.setCF<setVariable::CF40>(4.0);
  // 1/4 and 3/4 in main length: [total length 25.0-11.0] 
  Geometry::Vec3D PPos(0,3.5,0);
  const Geometry::Vec3D XVec(-1,0,0);
  const std::string portName=preName+"FilterBoxAPort";
  PItemGen.generatePort(Control,portName+"0",PPos,XVec);
  PItemGen.generatePort(Control,portName+"1",-PPos,XVec);

  // ion pump port
  PItemGen.setCF<setVariable::CF100>(7.5);
  PItemGen.generatePort(Control,portName+"2",
			Geometry::Vec3D(0,0,0),
			Geometry::Vec3D(0,0,-1));
  // Main flange for diamond filter
  PItemGen.setCF<setVariable::CF63>(5.0);
  PItemGen.generatePort(Control,portName+"3",
			Geometry::Vec3D(0,0,0),
			Geometry::Vec3D(0,0,1));

  FlangeGen.setCF<setVariable::CF63>();
  FlangeGen.setPlate(0.0,0.0,"Void");
  FlangeGen.setBlade(3.0,5.0,0.5,0.0,"Graphite",1);
  FlangeGen.generateMount(Control,preName+"FilterStick",1);  // in beam

  GateGen.setLength(2.5);
  GateGen.setCF<setVariable::CF40>();
  GateGen.generateValve(Control,preName+"GateA",0.0,0);

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,preName+"BellowB",0,12.0);

  SimpleTubeGen.setCF<CF40>();
  SimpleTubeGen.setBFlangeCF<CF63>();
  SimpleTubeGen.generateTube(Control,preName+"ScreenPipeA",0.0,12.5);
  Control.addVariable(preName+"ScreenPipeANPorts",1);
  PItemGen.setCF<setVariable::CF40>(4.0);
  PItemGen.generatePort(Control,preName+"ScreenPipeAPort0",
			Geometry::Vec3D(0,0,0),Geometry::Vec3D(1,0,0));

  
  SimpleTubeGen.setCF<CF63>();
  SimpleTubeGen.generateTube(Control,preName+"ScreenPipeB",0.0,14.0);
  Control.addVariable(preName+"ScreenPipeBNPorts",2);
  PItemGen.setCF<setVariable::CF63>(4.0);
  PItemGen.setOuterVoid(0);
  PItemGen.generatePort(Control,preName+"ScreenPipeBPort0",
			Geometry::Vec3D(0,0,0),Geometry::Vec3D(-1,0,0));
  PItemGen.generatePort(Control,preName+"ScreenPipeBPort1",
			Geometry::Vec3D(0,0,0),Geometry::Vec3D(0,0,-1));


  VBoxGen.setMat("Stainless304");
  VBoxGen.setWallThick(1.0);
  VBoxGen.setCF<CF63>();
  VBoxGen.setPortLength(2.5,2.5); // La/Lb
  // ystep/width/height/depth/length
  VBoxGen.generateBox(Control,preName+"PrimeJawBox",
		      0.0,30.0,15.0,15.0,53.15);

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.setAFlangeCF<setVariable::CF63>();
  BellowGen.setBFlangeCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,preName+"BellowC",0,12.0);
  
  GateGen.setCF<setVariable::CF63>();
  GateGen.generateValve(Control,preName+"GateB",0.0,0);

  formaxVar::monoVariables(Control,preName);

  GateGen.setCF<setVariable::CF63>();
  GateGen.generateValve(Control,preName+"GateC",0.0,0);

  BellowGen.setCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,preName+"BellowD",0,12.0);

  formaxVar::diagUnit(Control,preName+"DiagBoxA");

  
  BellowGen.setCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,preName+"BellowE",0,12.0);

  GateGen.setCF<setVariable::CF63>();
  GateGen.generateValve(Control,preName+"GateD",0.0,0);
  
  formaxVar::mirrorBox(Control,preName+"MirrorA");

  GateGen.setCF<setVariable::CF63>();
  GateGen.generateValve(Control,preName+"GateE",0.0,0);

  BellowGen.setCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,preName+"BellowF",0,12.0);

  formaxVar::diagUnit2(Control,preName+"DiagBoxB");

  BellowGen.setCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,preName+"BellowG",0,12.0);

  GateGen.setCF<setVariable::CF63>();
  GateGen.generateValve(Control,preName+"GateF",0.0,0);

  formaxVar::mirrorBox(Control,preName+"MirrorB");

  GateGen.setCF<setVariable::CF63>();
  GateGen.generateValve(Control,preName+"GateG",0.0,0);

  BellowGen.setCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,preName+"BellowH",0,12.0);

  formaxVar::diagUnit2(Control,preName+"DiagBoxC");

  MShutterGen.generateShutter(Control,preName+"MonoShutter",0,0);
  // ystep : wing
  setVariable::PipeShieldGenerator ShieldGen;
  ShieldGen.setPlate(100.0,100,9.0);
  ShieldGen.generateShield(Control,preName+"ScreenA",60.0,0.0);

  return;
}

void
connectingVariables(FuncDataBase& Control)
  /*!
    Variables for the connecting region
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("formaxVariables[F]","connectingVariables");

  const std::string baseName="FormaxConnect";
  const Geometry::Vec3D OPos(0,0,0);
  const Geometry::Vec3D ZVec(0,0,-1);

  Control.addVariable(baseName+"OuterRadius",60.0);
  
  setVariable::BellowGenerator BellowGen;
  setVariable::LeadPipeGenerator LeadPipeGen;
  setVariable::PortTubeGenerator PTubeGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::LeadBoxGenerator LBGen;
  
  PItemGen.setCF<setVariable::CF40>(3.0);
  PItemGen.setPlate(0.0,"Void");  
  
  BellowGen.setCF<CF40>();  
  BellowGen.generateBellow(Control,baseName+"BellowA",0,10.0);

  LBGen.generateBox(Control,baseName+"LeadA",5.0,12.0);
    
  LeadPipeGen.setCF<CF40>();
  LeadPipeGen.setCladdingThick(0.5);
  LeadPipeGen.generateCladPipe(Control,baseName+"PipeA",10.0,152.0);
  
  PTubeGen.setMat("Stainless304");
  PTubeGen.setPipeCF<CF40>();
  PTubeGen.setPortCF<CF40>();
  PTubeGen.setPortLength(3.0,3.0);
  // ystep/width/height/depth/length
  PTubeGen.generateTube(Control,baseName+"IonPumpA",0.0,4.0);
  Control.addVariable(baseName+"IonPumpANPorts",1);
  PItemGen.generatePort(Control,baseName+"IonPumpAPort0",OPos,ZVec);


  // temp offset
  LBGen.generateBox(Control,baseName+"PumpBoxA",5.50,12.0);

  LeadPipeGen.generateCladPipe(Control,baseName+"PipeB",
			       PTubeGen.getTotalLength(4.0),188.0);
  
  BellowGen.generateBellow(Control,baseName+"BellowB",0,10.0);
  LBGen.generateBox(Control,baseName+"LeadB",5.0,12.0);
  
  LeadPipeGen.generateCladPipe(Control,baseName+"PipeC",10.0,188.0);

  // ystep/width/height/depth/length
  PTubeGen.generateTube(Control,baseName+"IonPumpB",0.0,4.0);
  LBGen.generateBox(Control,baseName+"PumpBoxB",5.5,12.0);
  
  Control.addVariable(baseName+"IonPumpBNPorts",1);
  PItemGen.generatePort(Control,baseName+"IonPumpBPort0",OPos,ZVec);
  
  LeadPipeGen.generateCladPipe(Control,baseName+"PipeD",
			       PTubeGen.getTotalLength(4.0),172.0);


  BellowGen.generateBellow(Control,baseName+"BellowC",0,10.0);
  LBGen.generateBox(Control,baseName+"LeadC",5.0,12.0);
  
  return;
}

}  // NAMESPACE formaxVar
  
void
FORMAXvariables(FuncDataBase& Control)
  /*!
    Function to set the control variables and constants
    -- This version is for Photon Moderator
    \param Control :: Function data base to add constants too
  */
{
  ELog::RegMethod RegA("formaxVariables[F]","formaxVariables");

  Control.addVariable("sdefType","Wiggler");
  
  setVariable::PipeGenerator PipeGen;
  setVariable::LeadPipeGenerator LeadPipeGen;

  PipeGen.setWindow(-2.0,0.0);   // no window


  formaxVar::frontCaveVariables(Control,"FormaxRingCaveA",1,1);
  formaxVar::frontCaveVariables(Control,"FormaxRingCaveB",0,0);
  formaxVar::frontEndVariables(Control,"FormaxFrontBeam");  
  formaxVar::wallVariables(Control,"FormaxWallLead");
  
  PipeGen.setMat("Stainless304");
  PipeGen.setCF<setVariable::CF40>(); 
  PipeGen.generatePipe(Control,"FormaxJoinPipe",0,170.0);

  formaxVar::opticsHutVariables(Control,"FormaxOpticsHut");
  formaxVar::opticsVariables(Control,"Formax");


  return;
}

}  // NAMESPACE setVariable
