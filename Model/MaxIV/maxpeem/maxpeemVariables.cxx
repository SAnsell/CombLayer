/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   maxpeem/maxpeemVariables.cxx
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
#include "JawValveGenerator.h"
#include "PipeTubeGenerator.h"
#include "PortTubeGenerator.h"
#include "PortItemGenerator.h"
#include "VacBoxGenerator.h"
#include "FlangeMountGenerator.h"
#include "MirrorGenerator.h"
#include "CollGenerator.h"
#include "SqrFMaskGenerator.h"
#include "PortChicaneGenerator.h"
#include "LeadBoxGenerator.h"

namespace setVariable
{

namespace maxpeemVar
{
  void moveApertureTable(FuncDataBase&,const std::string&);
  void heatDumpVariables(FuncDataBase&,const std::string&);
  void shutterTable(FuncDataBase&,const std::string&);
  void transferVariables(FuncDataBase&,const std::string&);
  void opticsHutVariables(FuncDataBase&,const std::string&);
  void opticsBeamVariables(FuncDataBase&,const std::string&);

void
opticsBeamVariables(FuncDataBase& Control,
		    const std::string& opticKey)
  /*!
    Builds the variables for the moveable apperature table
    containing two movable aperatures, pumping and bellows
    \param Control :: Database
    \param frontKey :: prename
  */
{
  ELog::RegMethod RegA("maxpeemVariables[F]","opticsBeamVariables");

  setVariable::BellowGenerator BellowGen;
  setVariable::PipeGenerator PipeGen;
  setVariable::CrossGenerator CrossGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::PortItemGenerator PItemGen;
  
  PipeGen.setWindow(-2.0,0.0);   // no window
  PipeGen.setMat("Stainless304");
  
  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,opticKey+"BellowA",0,16.0);

    // flange if possible
  CrossGen.setPlates(0.5,2.0,2.0);       // wall/Top/base
  CrossGen.setTotalPorts(10.0,10.0);     // len of ports (after main)
  CrossGen.setMat("Stainless304");
  // height/depth
  CrossGen.generateDoubleCF<setVariable::CF40,setVariable::CF100>
    (Control,opticKey+"IonPA",0.0,24.4,36.6);

  // will be rotated vertical
  const std::string gateName=opticKey+"GateTubeA";
  SimpleTubeGen.setCF<CF63>();
  SimpleTubeGen.generateTube(Control,gateName,0.0,20.0);
  Control.addVariable(gateName+"NPorts",2);   // beam ports
  const Geometry::Vec3D ZVec(0,0,1);
  PItemGen.setCF<setVariable::CF40>(0.45);
  PItemGen.setPlate(0.0,"Void");  
  PItemGen.generatePort(Control,gateName+"Port0",Geometry::Vec3D(0,0,0),ZVec);
  PItemGen.generatePort(Control,gateName+"Port1",Geometry::Vec3D(0,0,0),-ZVec);

  BellowGen.generateBellow(Control,opticKey+"BellowB",0,16.0);

  PipeGen.setCF<CF40>();
  PipeGen.generatePipe(Control,opticKey+"PipeA",0,50.0);

  // will be rotated vertical
  const std::string florName=opticKey+"FlorTubeA";
  SimpleTubeGen.setCF<CF150>();
  SimpleTubeGen.generateTube(Control,florName,0.0,27.0);  // centre 13.5cm
  Control.addVariable(florName+"NPorts",2);   // beam ports
  PItemGen.setCF<setVariable::CF40>(1.9);
  PItemGen.setPlate(0.0,"Void");  
  PItemGen.generatePort(Control,florName+"Port0",Geometry::Vec3D(0,0,0),ZVec);
  PItemGen.generatePort(Control,florName+"Port1",Geometry::Vec3D(0,0,0),-ZVec);

  BellowGen.generateBellow(Control,opticKey+"BellowC",0,16.0);
  
  PipeGen.setCF<CF40>();
  PipeGen.generatePipe(Control,opticKey+"PipeB",0,174.0);

  
  return;
}


void
opticsHutVariables(FuncDataBase& Control,
		   const std::string& hutName)
  /*!
    Optics hut variables
    \param Control :: DataBase to add
    \param preName :: Beamline name
    \param caveName :: Cave name
  */
{
  ELog::RegMethod RegA("balderVariables","opticsHutVariables");

  Control.addVariable(hutName+"BeamTubeRadius",50.0);


  Control.addVariable(hutName+"Height",200.0);
  Control.addVariable(hutName+"Length",950.0);
  // total inner width [short] - 295.0cm 
  Control.addVariable(hutName+"OutWidth",207.81);
  Control.addVariable(hutName+"RingShortWidth",87.19);
  // total inner width [short] - 400.0cm 
  Control.addVariable(hutName+"RingLongWidth",192.19);
  Control.addVariable(hutName+"ShortLen",240.0);
  // length to first full width point
  Control.addVariable(hutName+"FullLen",348.0);

  // THIS IS WRONG but the diagram is a mess.
  Control.addVariable(hutName+"InnerSkin",0.3);
  Control.addVariable(hutName+"PbWallThick",0.4);
  Control.addVariable(hutName+"PbRoofThick",0.4);
  Control.addVariable(hutName+"PbFrontThick",0.4);
  Control.addVariable(hutName+"PbBackThick",0.4);
  Control.addVariable(hutName+"OuterSkin",0.3);

  Control.addVariable(hutName+"InnerMat","Stainless304");
  Control.addVariable(hutName+"PbMat","Lead");
  Control.addVariable(hutName+"OuterMat","Stainless304");
  
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
  ELog::RegMethod RegA("maxpeemVariables[F]","heatDumpTable");
  setVariable::BellowGenerator BellowGen;
  setVariable::PipeGenerator PipeGen;
  setVariable::CrossGenerator CrossGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::PortItemGenerator PItemGen;
    

  PipeGen.setWindow(-2.0,0.0);   // no window
  PipeGen.setMat("Stainless304");
  
  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,frontKey+"BellowC",0,10.0);
  
  PipeGen.setCF<CF40>();
  PipeGen.generatePipe(Control,frontKey+"HeatPipe",0,115.0);

  heatDumpVariables(Control,frontKey);

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,frontKey+"BellowD",0,10.0);

  // will be rotated vertical
  const std::string gateName=frontKey+"GateTubeA";
  SimpleTubeGen.setCF<CF63>();
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
  ELog::RegMethod RegA("maxpeemVariables","heatDumpVariables");

  setVariable::PortTubeGenerator PTubeGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::FlangeMountGenerator FlangeGen;

  PTubeGen.setMat("Stainless304");
  PTubeGen.setCF<CF150>();
  PTubeGen.setPortLength(2.5,2.5);
  
  PTubeGen.generateCFTube<CF150>(Control,frontKey+"HeatBox",0.0,20.0);
  Control.addVariable(frontKey+"HeatBoxNPorts",2);

  // beam ports
  PItemGen.setCF<setVariable::CF40>(5.0);
  PItemGen.setPlate(0.0,"Void");  

  const Geometry::Vec3D ZVec(0,0,1);
  const std::string heatName=frontKey+"HeatBoxPort";
  const std::string hName=frontKey+"HeatDumpFlange";
  PItemGen.generatePort(Control,heatName+"0",Geometry::Vec3D(0,0,0),ZVec);
  PItemGen.generatePort(Control,heatName+"1",Geometry::Vec3D(0,0,0),-ZVec);

  FlangeGen.setCF<setVariable::CF150>();
  FlangeGen.setBlade(5.0,10.0,1.0,0.0,"Tungsten",0);     // W / H / T
  FlangeGen.generateMount(Control,frontKey+"HeatTopFlange",0);  // in beam
  
  const std::string hDump(frontKey+"HeatDump");
  Control.addVariable(hDump+"Height",10.0);
  Control.addVariable(hDump+"Width",3.0);
  Control.addVariable(hDump+"Thick",8.0);
  Control.addVariable(hDump+"CutHeight",10.0);
  Control.addVariable(hDump+"CutDepth",0.0);
  Control.addVariable(hDump+"Mat","Tungsten");

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
  setVariable::FlangeMountGenerator FlangeGen;
    
  // joined and open
  GateGen.setLength(3.5);
  GateGen.setCF<setVariable::CF40>();
  GateGen.generateValve(Control,frontKey+"GateA",0.0,0);
  
  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,frontKey+"BellowI",0,10.0);
  
  SimpleTubeGen.setCF<CF100>();
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
  PItemGen.setPlate(0.0,"Void");
  // lift is actually 60mm [check]
  FlangeGen.setCF<setVariable::CF50>();
  FlangeGen.setThread(1.0,30.0,"Nickel");
  // W / H / T / ang 
  FlangeGen.setBlade(6.0,6.0,20.0,0.0,"Tungsten",1);  

  // centre of mid point
  Geometry::Vec3D CPos(0,-sBoxLen/4.0,0);
  for(size_t i=0;i<2;i++)
    {
      const std::string name=frontKey+"ShutterBoxPort"+std::to_string(i);
      const std::string fname=frontKey+"Shutter"+std::to_string(i);

      PItemGen.generatePort(Control,name,CPos,ZVec);
      FlangeGen.generateMount(Control,fname,0);  // in beam
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
transferVariables(FuncDataBase& Control,
		  const std::string& transKey)
/*!
    Set the variables for the transfer regions
    \param Control :: DataBase to use
    \param transKey :: name before part names
  */
{
  ELog::RegMethod RegA("maxpeemVariables[F]","transferVariables");
  setVariable::PipeGenerator PipeGen;

  PipeGen.setWindow(-2.0,0.0);   // no window
  PipeGen.setMat("Stainless304");
  PipeGen.setCF<setVariable::CF40>(); // was 2cm (why?)
  PipeGen.generatePipe(Control,transKey+"JoinPipe",0,145.0);

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
    Set the variables for the frontend
    \param Control :: DataBase to use
    \param frontKey :: name before part names
  */
{
  ELog::RegMethod RegA("maxpeemVariables[F]","frontEndVariables");

  setVariable::BellowGenerator BellowGen;
  setVariable::PipeGenerator PipeGen;
  setVariable::CrossGenerator CrossGen;
  setVariable::VacBoxGenerator VBoxGen;
  setVariable::SqrFMaskGenerator CollGen;
  setVariable::PortTubeGenerator PTubeGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::FlangeMountGenerator FlangeGen;

  Control.addVariable(frontKey+"OuterRadius",50.0);
  
  PipeGen.setWindow(-2.0,0.0);   // no window
  PipeGen.setMat("Stainless304");
  
  VBoxGen.setMat("Stainless304");
  VBoxGen.setWallThick(1.0);
  VBoxGen.setCF<CF40>();
  VBoxGen.setPortLength(5.0,5.0); // La/Lb
  // ystep/width/height/depth/length
  VBoxGen.generateBox(Control,frontKey+"WigglerBox",
		      0.0,30.0,15.0,15.0,273.30);

  // Wiggler
  Control.addVariable(frontKey+"WigglerLength",251.508);
  Control.addVariable(frontKey+"WigglerBlockWidth",8.0);
  Control.addVariable(frontKey+"WigglerBlockHeight",8.0);
  Control.addVariable(frontKey+"WigglerBlockDepth",8.0);
  Control.addVariable(frontKey+"WigglerBlockHGap",0.2);
  Control.addVariable(frontKey+"WigglerBlockVGap",0.96);

  Control.addVariable(frontKey+"WigglerBlockVCorner",1.0);
  Control.addVariable(frontKey+"WigglerBlockHCorner",2.0);

  
  Control.addVariable(frontKey+"WigglerVoidMat",0);
  Control.addVariable(frontKey+"WigglerBlockMat","Iron_10H2O");

  Control.addVariable(frontKey+"ECutDiskYStep",2.0);
  Control.addVariable(frontKey+"ECutDiskLength",0.1);
  Control.addVariable(frontKey+"ECutDiskRadius",0.11);
  Control.addVariable(frontKey+"ECutDiskDefMat","H2Gas#0.1");

  // this reaches 454.5cm from the middle of the undulator
  PipeGen.setCF<CF40>();
  PipeGen.generatePipe(Control,frontKey+"DipolePipe",0,296.1);

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.setBFlangeCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,frontKey+"BellowA",0,10.0);

  // collimator block
  CollGen.setCF<CF63>();
  CollGen.setBFlangeCF<CF40>();
  CollGen.setFrontGap(3.99,1.97);  //1033.8
  CollGen.setBackGap(0.71,0.71);
  CollGen.setMinSize(10.2,0.71,0.71);
  CollGen.generateColl(Control,frontKey+"CollA",0.0,15.0);

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,frontKey+"BellowB",0,10.0);

  // flange if possible
  CrossGen.setPlates(0.5,2.0,2.0);  // wall/Top/base
  CrossGen.setTotalPorts(10.0,10.0);     // len of ports (after main)
  CrossGen.setMat("Stainless304");
  // height/depth
  CrossGen.generateDoubleCF<setVariable::CF40,setVariable::CF100>
    (Control,frontKey+"IonPA",0.0,26.6,26.6);

  heatDumpTable(Control,frontKey);
  moveApertureTable(Control,frontKey);
  shutterTable(Control,frontKey);

  return;
}

}  // NAMESPACE maxpeemVar

  
void
MAXPEEMvariables(FuncDataBase& Control)
  /*!
    Function to set the control variables and constants
    -- This version is for Photon Moderator
    \param Control :: Function data base to add constants too
  */
{
  ELog::RegMethod RegA("maxpeemVariables[F]","maxpeemVariables");

  Control.addVariable("sdefType","Wiggler");

  maxpeemVar::frontEndVariables(Control,"MaxPeemFrontBeam");  
  maxpeemVar::wallVariables(Control,"MaxPeemWallLead");
  maxpeemVar::transferVariables(Control,"MaxPeem");
  maxpeemVar::opticsHutVariables(Control,"MaxPeemOpticsHut");
  maxpeemVar::opticsBeamVariables(Control,"MaxPeemOpticsBeam");

  return;
}

}  // NAMESPACE setVariable
