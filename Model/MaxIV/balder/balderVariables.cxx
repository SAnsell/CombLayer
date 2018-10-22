/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   balder/balderVariables.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#include "FlangeMountGenerator.h"
#include "BeamMountGenerator.h"
#include "MirrorGenerator.h"
#include "CollGenerator.h"
#include "PortChicaneGenerator.h"
#include "MazeGenerator.h"
#include "RingDoorGenerator.h"
#include "LeadBoxGenerator.h"

namespace setVariable
{

namespace balderVar
{

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
    \param mazeFlag :: door is present
  */
{
  ELog::RegMethod RegA("balderVariables[F]","frontCaveVariables");

  MazeGenerator MGen;
  RingDoorGenerator RGen;
  
  Control.addVariable(preName+"Length",2100.0);
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
  Control.addVariable(preName+"SegmentLength",1365.0);
  Control.addVariable(preName+"SegmentThick",100.0);

  Control.addVariable(preName+"FrontHoleRadius",7.0);

  
  Control.addVariable(preName+"FrontWallMat","Concrete");
  Control.addVariable(preName+"WallMat","Concrete");
  Control.addVariable(preName+"FloorMat","Concrete");
  Control.addVariable(preName+"RoofMat","Concrete");


  if (mazeFlag)
    MGen.generateMaze(Control,preName+"Maze",0.0);
  if (doorFlag)
    RGen.generateDoor(Control,preName+"RingDoor",800.0);
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
  ELog::RegMethod RegA("maxpeemVariables[F]","shutterTable");

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
  ELog::RegMethod RegA("maxpeemVariables[F]","collimatorVariables");

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
  ELog::RegMethod RegA("balderVariables[F]","heatDumpTable");

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
  ELog::RegMethod RegA("balderVariables","heatDumpVariables");
  
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
wallVariables(FuncDataBase& Control,
	      const std::string& wallKey)
/*!
    Set the variables for the frontend wall
    \param Control :: DataBase to use
    \param frontKey :: name before part names
  */
{
  ELog::RegMethod RegA("maxpeemVariables[F]","wallVariables");

  Control.addVariable(wallKey+"FrontHeight",40.0);
  Control.addVariable(wallKey+"FrontWidth",60.0);
  Control.addVariable(wallKey+"FrontLength",20.0);
  
  Control.addVariable(wallKey+"BackWidth",20.0);
  Control.addVariable(wallKey+"BackHeight",20.0);
  
  Control.addVariable(wallKey+"VoidRadius",3.0);
  Control.addVariable(wallKey+"WallMat","Lead");
  Control.addVariable(wallKey+"VoidMat","Void");
  return;
}

void
frontEndVariables(FuncDataBase& Control,
		  const std::string& frontKey)
/*!
    Set the variables for the mono
    \param Control :: DataBase to use
    \param frontKey :: name before part names
  */
{
  ELog::RegMethod RegA("balderVariables[F]","frontEndVariables");

  setVariable::BellowGenerator BellowGen;
  setVariable::PipeGenerator PipeGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::VacBoxGenerator VBoxGen;
  setVariable::CollGenerator CollGen;
  setVariable::PortTubeGenerator PTubeGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::FlangeMountGenerator FlangeGen;

  Control.addVariable(frontKey+"OuterRadius",60.0);  

  PipeGen.setWindow(-2.0,0.0);   // no window
  PipeGen.setMat("Stainless304");
  
  VBoxGen.setMat("Stainless304");
  VBoxGen.setWallThick(1.0);
  VBoxGen.setCF<CF40>();
  VBoxGen.setPortLength(5.0,5.0); // La/Lb
  // ystep/width/height/depth/length
  VBoxGen.generateBox(Control,frontKey+"WigglerBox",
		      115.0,30.0,15.0,15.0,210.0);

  // Wiggler
  Control.addVariable(frontKey+"WigglerLength",200.0);
  Control.addVariable(frontKey+"WigglerBlockWidth",8.0);
  Control.addVariable(frontKey+"WigglerBlockHeight",8.0);
  Control.addVariable(frontKey+"WigglerBlockDepth",8.0);
  Control.addVariable(frontKey+"WigglerBlockHGap",0.2);
  Control.addVariable(frontKey+"WigglerBlockVGap",0.96);

  Control.addVariable(frontKey+"WigglerBlockVCorner",1.0);
  Control.addVariable(frontKey+"WigglerBlockHCorner",2.0);

  
  Control.addVariable(frontKey+"WigglerVoidMat",0);
  Control.addVariable(frontKey+"WigglerBlockMat","Iron_10H2O");

  PipeGen.setCF<CF40>();
  PipeGen.generatePipe(Control,frontKey+"DipolePipe",0,806.0);

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
  CollGen.setMinSize(29.0,1.2,1.24);
  CollGen.generateColl(Control,frontKey+"CollA",0.0,34.0);

  BellowGen.setCF<setVariable::CF63>();
  BellowGen.setAFlangeCF<setVariable::CF100>();
  BellowGen.generateBellow(Control,frontKey+"BellowB",0,16.0);

  
  PipeGen.setCF<CF100>();
  PipeGen.generatePipe(Control,frontKey+"CollABPipe",0,432.0);

  Control.addVariable(frontKey+"ECutDiskYStep",2.0);
  Control.addVariable(frontKey+"ECutDiskLength",0.1);
  Control.addVariable(frontKey+"ECutDiskRadius",0.11);
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
  CollGen.setMinSize(12.0,0.730,0.16);
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
  ELog::RegMethod RegA("balderVariables","opticsHutVariables");

  Control.addVariable(hutName+"BeamTubeRadius",50.0);
  
  Control.addVariable(hutName+"Depth",100.0);
  Control.addVariable(hutName+"Height",200.0);
  Control.addVariable(hutName+"Length",1034.6);
  Control.addVariable(hutName+"OutWidth",250.0);
  Control.addVariable(hutName+"RingWidth",60.0);
  Control.addVariable(hutName+"RingWallLen",105.0);
  Control.addVariable(hutName+"RingWallAngle",18.50);
  Control.addVariable(hutName+"RingConcThick",100.0);
  
  Control.addVariable(hutName+"InnerThick",0.3);
  
  Control.addVariable(hutName+"PbWallThick",2.0);
  Control.addVariable(hutName+"PbRoofThick",2.0);
  Control.addVariable(hutName+"PbFrontThick",2.0);
  Control.addVariable(hutName+"PbBackThick",10.0);

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
  Control.addVariable(hutName+"HoleRadius",3.5);

  Control.addVariable(hutName+"InletXStep",0.0);
  Control.addVariable(hutName+"InletZStep",0.0);
  Control.addVariable(hutName+"InletRadius",5.0);


  Control.addVariable(hutName+"NChicane",2);
  PortChicaneGenerator PGen;
  PGen.generatePortChicane(Control,hutName+"Chicane0",470.0,-25.0);
  PGen.generatePortChicane(Control,hutName+"Chicane1",370.0,-25.0);


  return;
}


void
shieldVariables(FuncDataBase& Control)
  /*!
    Shield variables
    \param Control :: DataBase to add
  */
{
  ELog::RegMethod RegA("balderVariables","shieldVariables");

  const std::string preName("Balder");
  
  Control.addVariable(preName+"PShieldLength",10.0);
  Control.addVariable(preName+"PShieldWidth",80.0);
  Control.addVariable(preName+"PShieldHeight",80.0);
  Control.addVariable(preName+"PShieldWallThick",0.5);
  Control.addVariable(preName+"PShieldClearGap",1.0);
  Control.addVariable(preName+"PShieldWallMat","Stainless304");
  Control.addVariable(preName+"PShieldMat","Lead");
   
  Control.addVariable(preName+"NShieldYStep",10.2);
  Control.addVariable(preName+"NShieldLength",7.0);
  Control.addVariable(preName+"NShieldWidth",80.0);
  Control.addVariable(preName+"NShieldHeight",80.0);
  Control.addVariable(preName+"NShieldWallThick",0.5);
  Control.addVariable(preName+"NShieldClearGap",0.2);
  Control.addVariable(preName+"NShieldWallMat","Stainless304");
  Control.addVariable(preName+"NShieldMat","Poly");

  Control.addVariable(preName+"OuterShieldYStep",10.2);
  Control.addVariable(preName+"OuterShieldLength",5.0);
  Control.addVariable(preName+"OuterShieldWidth",80.0);
  Control.addVariable(preName+"OuterShieldHeight",80.0);
  Control.addVariable(preName+"OuterShieldWallThick",0.5);
  Control.addVariable(preName+"OuterShieldClearGap",0.2);
  Control.addVariable(preName+"OuterShieldWallMat","Stainless304");
  Control.addVariable(preName+"OuterShieldMat","Poly");
  return;
}
  
void
monoVariables(FuncDataBase& Control,const double YStep)
  /*!
    Set the variables for the mono
    \param Control :: DataBase to use
    \param YStep :: Distance to step
  */
{
  ELog::RegMethod RegA("balderVariables[F]","monoVariables");
  setVariable::PortItemGenerator PItemGen;
    
  const std::string preName("BalderOpticsLine");
  
  Control.addVariable(preName+"MonoVacYStep",YStep);
  Control.addVariable(preName+"MonoVacZStep",2.0);
  Control.addVariable(preName+"MonoVacRadius",33.0);
  Control.addVariable(preName+"MonoVacRingWidth",21.5);
  Control.addVariable(preName+"MonoVacOutWidth",16.5);
  Control.addVariable(preName+"MonoVacWallThick",1.0);

  Control.addVariable(preName+"MonoVacDoorThick",2.54);
  Control.addVariable(preName+"MonoVacBackThick",2.54);
  Control.addVariable(preName+"MonoVacDoorFlangeRad",4.0);
  Control.addVariable(preName+"MonoVacRingFlangeRad",4.0);
  Control.addVariable(preName+"MonoVacDoorFlangeLen",2.54);
  Control.addVariable(preName+"MonoVacRingFlangeLen",2.54);

  Control.addVariable(preName+"MonoVacInPortZStep",-2.0);
  Control.addVariable(preName+"MonoVacOutPortZStep",2.0);
  
  Control.addVariable(preName+"MonoVacPortRadius",5.0);
  Control.addVariable(preName+"MonoVacPortLen",4.65);
  Control.addVariable(preName+"MonoVacPortThick",0.3);
  Control.addVariable(preName+"MonoVacPortFlangeLen",1.5);
  Control.addVariable(preName+"MonoVacPortFlangeRad",2.7);

  Control.addVariable(preName+"MonoVacWallMat","Stainless304");
  // CRYSTALS:
  Control.addVariable(preName+"MonoXtalZStep",-2.0);
  Control.addVariable(preName+"MonoXtalGap",4.0);
  Control.addVariable(preName+"MonoXtalTheta",10.0);
  Control.addVariable(preName+"MonoXtalPhiA",0.0);
  Control.addVariable(preName+"MonoXtalPhiA",0.0);
  Control.addVariable(preName+"MonoXtalWidth",10.0);
  Control.addVariable(preName+"MonoXtalLengthA",8.0);
  Control.addVariable(preName+"MonoXtalLengthB",12.0);
  Control.addVariable(preName+"MonoXtalThickA",4.0);
  Control.addVariable(preName+"MonoXtalThickB",3.0);
  Control.addVariable(preName+"MonoXtalBaseThick",5.0);
  Control.addVariable(preName+"MonoXtalBaseExtra",2.0);
  
  Control.addVariable(preName+"MonoXtalMat","Silicon80K");
  Control.addVariable(preName+"MonoXtalBaseMat","Copper");

  // 20cm above port tube
  const Geometry::Vec3D XVecMinus(-1,0,0);
  PItemGen.setCF<setVariable::CF50>(10.0);
  PItemGen.setPlate(1.0,"Glass");

  PItemGen.generatePort(Control,preName+"MonoVacPort0",
			Geometry::Vec3D(0,0,0),XVecMinus);
  return;
}

void
opticsVariables(FuncDataBase& Control,
		const std::string& beamName)
  /*
    Vacuum optics components in the optics hutch
    \param Control :: Function data base
    \param preName :: beamline name
  */
{
  ELog::RegMethod RegA("balderVariables[F]","balderVariables");

  const std::string opticsName(beamName+"OpticsLine");
  
  setVariable::PipeGenerator PipeGen;
  setVariable::BellowGenerator BellowGen;
  setVariable::CrossGenerator CrossGen;
  setVariable::VacBoxGenerator VBoxGen;
  setVariable::PortTubeGenerator PTubeGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::GateValveGenerator GateGen;
  setVariable::JawValveGenerator JawGen;
  setVariable::FlangeMountGenerator FlangeGen;
  setVariable::MirrorGenerator MirrGen;

  PipeGen.setWindow(-2.0,0.0);   // no window

  // addaptor flange at beginning: [check]
  PipeGen.setCF<CF40>();
  PipeGen.setAFlangeCF<CF63>(); 
  PipeGen.generatePipe(Control,opticsName+"InitPipe",0,10.0);
  
  // flange if possible
  CrossGen.setPlates(0.5,2.0,2.0);  // wall/Top/base
  CrossGen.setPorts(5.75,5.75);     // len of ports (after main)
  CrossGen.setMat("Stainless304");

  CrossGen.generateDoubleCF<setVariable::CF40,setVariable::CF63>
    (Control,opticsName+"IonPA",0.0,10.0,26.5);

  // flange if possible
  CrossGen.setPlates(0.5,2.0,2.0);  // wall/Top/base
  CrossGen.setPorts(5.75,5.75);     // len of ports (after main)
  CrossGen.generateDoubleCF<setVariable::CF40,setVariable::CF63>
    (Control,opticsName+"TriggerPipe",0.0,15.0,10.0);  // ystep/height/depth

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.setBFlangeCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,opticsName+"BellowA",0,16.0);

  // ACTUAL ROUND PIPE + 4 filter tubles and 1 base tube [large]
  
  PTubeGen.setMat("Stainless304");
  PTubeGen.setCF<CF63>();
  PTubeGen.setPortLength(10.7,10.7);
  // ystep/width/height/depth/length
  PTubeGen.generateTube(Control,opticsName+"FilterBox",0.0,9.0,54.0);
  Control.addVariable(opticsName+"FilterBoxNPorts",4);

  PItemGen.setCF<setVariable::CF50>(20.0);
  PItemGen.setPlate(0.0,"Void");  
  FlangeGen.setCF<setVariable::CF50>();
  FlangeGen.setBlade(3.0,5.0,0.5,22.0,"Tungsten",1);  // 22 rotation

  // centre of mid point
  Geometry::Vec3D CPos(0,-1.5*11.0,0);
  const Geometry::Vec3D ZVec(0,0,1);
  for(size_t i=0;i<4;i++)
    {
      const std::string name=opticsName+"FilterBoxPort"+std::to_string(i);
      const std::string fname=opticsName+"Filter"+std::to_string(i);      
      PItemGen.generatePort(Control,name,CPos,ZVec);
      CPos+=Geometry::Vec3D(0,11,0);
      const int upFlag((i) ? 0 : 1);
      FlangeGen.generateMount(Control,fname,upFlag);  // in beam
    }

  // filters:
  
  BellowGen.setCF<setVariable::CF40>();
  BellowGen.setAFlangeCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,opticsName+"BellowB",0,10.0);    

  GateGen.setLength(2.5);
  GateGen.setCF<setVariable::CF40>();
  GateGen.generateValve(Control,opticsName+"GateA",0.0,0);
    
  VBoxGen.setMat("Stainless304");
  VBoxGen.setWallThick(1.0);
  VBoxGen.setCF<CF40>();
  VBoxGen.setPortLength(5.0,5.0); // La/Lb
  // ystep/width/height/depth/length
  // [length is 177.4cm total]
  VBoxGen.generateBox(Control,opticsName+"MirrorBox",0.0,54.0,15.3,31.3,167.4);

  // y/z/theta/phi/radius
  MirrGen.generateMirror(Control,opticsName+"Mirror",0.0, 0.0, -0.5, 0.0,0.0);

  // y/z/theta/phi/radius
  MirrGen.generateMirror(Control,opticsName+"MirrorB",0.0, 0.0, 0.5, 0.0,0.0);

  GateGen.generateValve(Control,opticsName+"GateB",0.0,0);

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.setBFlangeCF<setVariable::CF100>();
  BellowGen.generateBellow(Control,opticsName+"BellowC",0,10.0);    

  PipeGen.setCF<setVariable::CF100>(); // was 2cm (why?)
  // [length is 38.3cm total]
  PipeGen.generatePipe(Control,opticsName+"DriftA",0,38.3);
  // Length ignored  as joined front/back

  BellowGen.setCF<setVariable::CF100>();
  BellowGen.generateBellow(Control,opticsName+"MonoBellowA",0,50.0);   
  BellowGen.generateBellow(Control,opticsName+"MonoBellowB",0,50.0);
  
  // [length is 72.9cm total]
  // [offset after mono is 119.1cm ]
  PipeGen.setCF<setVariable::CF100>();    
  PipeGen.generatePipe(Control,opticsName+"DriftB",119.1,72.5); 
  Control.addVariable(opticsName+"DriftBZStep",4.0);

  monoVariables(Control,119.1/2.0);  // mono middle of drift chambers A/B
  
  // joined and open
  GateGen.setCF<setVariable::CF100>();
  GateGen.generateValve(Control,opticsName+"GateC",0.0,0);

  // [length is 54.4cm total]
  PipeGen.setCF<setVariable::CF100>();    
  PipeGen.generatePipe(Control,opticsName+"DriftC",0,54.4); 

  Control.addVariable(opticsName+"BeamStopZStep",-2.0);
  Control.addVariable(opticsName+"BeamStopWidth",4.0);
  Control.addVariable(opticsName+"BeamStopHeight",3.2);
  Control.addVariable(opticsName+"BeamStopDepth",6.0);
  Control.addVariable(opticsName+"BeamStopDefMat","Tungsten");

  
  // SLITS
  JawGen.setCF<setVariable::CF100>();
  JawGen.setLength(4.0);
  JawGen.setSlits(3.0,2.0,0.2,"Tantalum");
  JawGen.generateSlits(Control,opticsName+"SlitsA",0.0,0.8,0.8);

  PTubeGen.setCF<CF100>();
  PTubeGen.setPortLength(1.0,1.0);
  // ystep/width/height/depth/length
  PTubeGen.generateTube(Control,opticsName+"ShieldPipe",0.0,9.0,54.0);

  Control.addVariable(opticsName+"ShieldPipeNPorts",4);

  // first Two ports are CF100 
  PItemGen.setCF<setVariable::CF100>(20.0);
  PItemGen.setPlate(0.0,"Void");  
  // centre of mid point
  CPos=Geometry::Vec3D(0,-15.0,0);
  const std::string nameShield=opticsName+"ShieldPipePort";

  PItemGen.generatePort(Control,nameShield+"0",CPos,ZVec);
  PItemGen.generatePort(Control,nameShield+"1",CPos,-ZVec);

  PItemGen.setCF<setVariable::CF40>(10.0);
  PItemGen.setPlate(0.0,"Void");
  
  PItemGen.generatePort(Control,nameShield+"2",
			Geometry::Vec3D(0,10,0),Geometry::Vec3D(-1,0,0));
  PItemGen.generatePort(Control,nameShield+"3",
			Geometry::Vec3D(0,15,0),Geometry::Vec3D(1,0,0));

  // bellows on shield block
  BellowGen.setCF<setVariable::CF40>();
  BellowGen.setAFlangeCF<setVariable::CF100>();
  BellowGen.generateBellow(Control,opticsName+"BellowD",0,10.0);    

  // joined and open
  GateGen.setCF<setVariable::CF40>();
  GateGen.generateValve(Control,opticsName+"GateD",0.0,0);

  VBoxGen.setCF<CF40>();
  VBoxGen.setPortLength(4.5,4.5); // La/Lb
  // [length is 177.4cm total]
  VBoxGen.generateBox(Control,opticsName+"MirrorBoxB",0.0,54.0,15.3,31.3,178.0);

  // small flange bellows
  BellowGen.setCF<setVariable::CF40>(); 
  BellowGen.setBFlangeCF<setVariable::CF100>(); 
  BellowGen.generateBellow(Control,opticsName+"BellowE",0,10.0);

  // SLITS [second pair]
  JawGen.setCF<setVariable::CF100>();
  JawGen.setLength(3.0);
  JawGen.setSlits(3.0,2.0,0.2,"Tantalum");
  JawGen.generateSlits(Control,opticsName+"SlitsB",0.0,0.8,0.8);

  PTubeGen.setCF<CF100>();
  PTubeGen.setPortLength(1.0,1.0);
  // ystep/radius/length
  PTubeGen.generateTube(Control,opticsName+"ViewTube",0.0,9.0,39.0);

  Control.addVariable(opticsName+"ViewTubeNPorts",4);

  const std::string nameView(opticsName+"ViewTubePort");
  const Geometry::Vec3D XAxis(1,0,0);
  const Geometry::Vec3D YAxis(0,1,0);
  const Geometry::Vec3D ZAxis(0,0,1);

  PItemGen.setCF<setVariable::CF40>(5.0);
  PItemGen.setPlate(0.0,"Void");  
  PItemGen.generatePort(Control,nameView+"0",YAxis*14.0,XAxis);
  PItemGen.generatePort(Control,nameView+"1",YAxis*10.0,-XAxis);
  PItemGen.setCF<setVariable::CF63>(10.0);
  PItemGen.setPlate(0.0,"Void");  
  PItemGen.generatePort(Control,nameView+"2",-YAxis*2.0,ZAxis);
  PItemGen.generatePort(Control,nameView+"3",-YAxis*2.0,
			Geometry::Vec3D(1,-1,0));


  // centre of mid point
  const std::string fname=opticsName+"ViewMount"+std::to_string(0);      
  FlangeGen.setCF<setVariable::CF40>();
  FlangeGen.generateMount(Control,fname,0);    // out of  beam

  // small flange bellows
  BellowGen.setCF<setVariable::CF63>(); 
  BellowGen.setAFlangeCF<setVariable::CF100>(); 
  BellowGen.generateBellow(Control,opticsName+"BellowF",0,23.0);

  // Shutter pipe
  CrossGen.setPlates(1.0,2.5,2.5);  // wall/Top/base
  CrossGen.setPorts(3.0,3.0);     // len of ports (after main)
  CrossGen.generateCF<setVariable::CF63>
    (Control,opticsName+"ShutterPipe",0.0,8.0,13.5,13.5);

  FlangeGen.setCF<setVariable::CF63>();
  FlangeGen.setBlade(5.0,5.0,5.0,0.0,"Tungsten",1);     // W / H / T
  FlangeGen.setNoPlate();
  FlangeGen.generateMount(Control,opticsName+"MonoShutter",0); 


  // bellows on shield block
  BellowGen.setCF<setVariable::CF40>();
  BellowGen.setAFlangeCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,opticsName+"BellowG",0,10.0);    

    // joined and open
  GateGen.setCF<setVariable::CF40>();
  GateGen.generateValve(Control,opticsName+"GateE",0.0,0);

  // pipe shield
  for(size_t i=0;i<4;i++)
    {
      const std::string NStr=opticsName+"NShield"+std::to_string(i);

      Control.addVariable(NStr+"Length",3.0);
      Control.addVariable(NStr+"Width",40.0);
      Control.addVariable(NStr+"Height",40.0);
      Control.addVariable(NStr+"WallThick",0.1);
      Control.addVariable(NStr+"ClearGap",0.3);
      Control.addVariable(NStr+"WallMat","Stainless304");
      Control.addVariable(NStr+"Mat","Poly");
    }
  Control.addVariable(opticsName+"NShield0YStep",1.0);
  Control.addVariable(opticsName+"NShield1YStep",3.0);
  Control.addVariable(opticsName+"NShield2YStep",1.0);
  return;
}

void
connectingVariables(FuncDataBase& Control)
  /*!
    Variables for the connecting region
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("balderVariables[F]","connectingVariables");

  const std::string baseName="BalderConnect";
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
  PTubeGen.setCF<CF40>();
  PTubeGen.setPortLength(3.0,3.0);
  // ystep/width/height/depth/length
  PTubeGen.generateTube(Control,baseName+"IonPumpA",0.0,CF40::innerRadius,4.0);
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
  PTubeGen.generateTube(Control,baseName+"IonPumpB",0.0,CF40::innerRadius,4.0);
  LBGen.generateBox(Control,baseName+"PumpBoxB",5.5,12.0);
  
  Control.addVariable(baseName+"IonPumpBNPorts",1);
  PItemGen.generatePort(Control,baseName+"IonPumpBPort0",OPos,ZVec);
  
  LeadPipeGen.generateCladPipe(Control,baseName+"PipeD",
			       PTubeGen.getTotalLength(4.0),172.0);


  BellowGen.generateBellow(Control,baseName+"BellowC",0,10.0);
  LBGen.generateBox(Control,baseName+"LeadC",5.0,12.0);
  
  return;
}

}  // NAMESPACE balderVar
  
void
BALDERvariables(FuncDataBase& Control)
  /*!
    Function to set the control variables and constants
    -- This version is for Photon Moderator
    \param Control :: Function data base to add constants too
  */
{
  ELog::RegMethod RegA("balderVariables[F]","balderVariables");

  Control.addVariable("sdefType","Wiggler");

  
  setVariable::PipeGenerator PipeGen;
  setVariable::LeadPipeGenerator LeadPipeGen;

  PipeGen.setWindow(-2.0,0.0);   // no window


  balderVar::frontCaveVariables(Control,"BalderRingCaveA",1,1);
  balderVar::frontCaveVariables(Control,"BalderRingCaveB",0,0);
  balderVar::frontEndVariables(Control,"BalderFrontBeam");  
  balderVar::wallVariables(Control,"BalderWallLead");
  
  PipeGen.setMat("Stainless304");
  PipeGen.setCF<setVariable::CF40>(); 
  PipeGen.generatePipe(Control,"BalderJoinPipe",0,173.0);

  balderVar::opticsHutVariables(Control,"BalderOpticsHut");
  balderVar::opticsVariables(Control,"Balder");

  LeadPipeGen.setCF<setVariable::CF40>();
  LeadPipeGen.setCladdingThick(0.5);
  LeadPipeGen.generateCladPipe(Control,"BalderJoinPipeB",0,54.0);

  balderVar::shieldVariables(Control);
  balderVar::connectingVariables(Control);

  // note bellow skip
  LeadPipeGen.generateCladPipe(Control,"BalderJoinPipeC",10.0,80.0);

  
  Control.addVariable("BalderExptYStep",1850.0);
  Control.addVariable("BalderExptDepth",120.0);
  Control.addVariable("BalderExptHeight",200.0);
  Control.addVariable("BalderExptLength",858.4);
  Control.addVariable("BalderExptOutWidth",198.50);
  Control.addVariable("BalderExptRingWidth",248.6);
  Control.addVariable("BalderExptInnerThick",0.3);
  Control.addVariable("BalderExptPbThick",0.5);
  Control.addVariable("BalderExptOuterThick",0.3);
  Control.addVariable("BalderExptFloorThick",50.0);

  Control.addVariable("BalderExptVoidMat","Void");
  Control.addVariable("BalderExptSkinMat","Stainless304");
  Control.addVariable("BalderExptPbMat","Lead");
  Control.addVariable("BalderExptFloorMat","Concrete");

  Control.addVariable("BalderExptHoleXStep",0.0);
  Control.addVariable("BalderExptHoleZStep",5.0);
  Control.addVariable("BalderExptHoleRadius",7.0);

  const std::string exptName="BalderExptLine";
  Control.addVariable(exptName+"BeamStopYStep",800.0);
  Control.addVariable(exptName+"BeamStopRadius",4.0);
  Control.addVariable(exptName+"BeamStopLength",2.0);
  Control.addVariable(exptName+"BeamStopDefMat","Copper");

  return;
}

}  // NAMESPACE setVariable
