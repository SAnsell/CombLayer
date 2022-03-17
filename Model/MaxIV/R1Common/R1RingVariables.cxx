/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   R1Common/R1RingVariables.cxx
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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

#include "CylGateValveGenerator.h"
#include "QuadUnitGenerator.h"
#include "DipoleChamberGenerator.h"
#include "DipoleExtractGenerator.h"
#include "DipoleSndBendGenerator.h"
#include "PipeGenerator.h"
#include "HeatDumpGenerator.h"
#include "SplitPipeGenerator.h"
#include "BellowGenerator.h"
#include "CrossGenerator.h"
#include "BeamMountGenerator.h"
#include "PipeTubeGenerator.h"
#include "PortItemGenerator.h"
#include "GateValveGenerator.h"
#include "MagnetBlockGenerator.h"

#include "CFFlanges.h"

namespace setVariable
{

namespace R1Ring
{
  void electronCutVariables(FuncDataBase&,const std::string&);
  void collimatorVariables(FuncDataBase&,const std::string&);
  void heatDumpVariables(FuncDataBase&,const std::string&);
  void heatDumpTable(FuncDataBase&,const std::string&);
  void moveApertureTable(FuncDataBase&,const std::string&);
  void shutterTable(FuncDataBase&,const std::string&);
}
  
void
R1FrontEndVariables(FuncDataBase& Control,
		    const std::string& frontKey,
		    const double) 
  /*!
    Set the variables for the front end
    \param Control :: DataBase to use
    \param frontKey :: name before part names
    \param exitLen :: last exit pipe length
  */
{
  ELog::RegMethod RegA("R1RingVariables[F]","R1FrontEndVariables");
  
  setVariable::PipeGenerator PipeGen;
  setVariable::CrossGenerator CrossGen;
  setVariable::QuadUnitGenerator QPGen;
  setVariable::BellowGenerator BellowGen;
  setVariable::GateValveGenerator CGateGen;
  
  PipeGen.setNoWindow();   // no window
  PipeGen.setMat("Stainless304");

  // CGateGen.setYRotate(rotateAngle);

  // length / width / depth / height
  CGateGen.setOuter(3.4,13.0,20.0,20.0);
  CGateGen.setBladeMat("Stainless304L");
  CGateGen.setBladeThick(0.8);
  CGateGen.setPortPairCF<CF40,CF63>();
  CGateGen.generateValve(Control,frontKey+"ElecGateA",0.0,1);

  
  Control.addVariable(frontKey+"OuterLeft",25.0);
  Control.addVariable(frontKey+"OuterRight",25.0);
  Control.addVariable(frontKey+"OuterFront",300.0);

  // Block for new R1-M1
  setVariable::MagnetBlockGenerator MBGen;
  MBGen.generateBlock(Control,frontKey+"MagnetBlock",0.0);

  QPGen.setLength(98.0);  //73.4
  QPGen.generatePipe(Control,frontKey+"MagnetBlockQuadUnit",0.0);

  setVariable::DipoleChamberGenerator DCGen;  
  DCGen.generatePipe(Control,frontKey+"MagnetBlockDipoleChamber",15.0);

  setVariable::DipoleExtractGenerator DEGen;
  DEGen.generatePipe(Control,frontKey+"MagnetBlockDipoleExtract",0.0);

  DEGen.setLength(89.0);
  DEGen.generatePipe(Control,frontKey+"MagnetBlockDipoleOut",0.0);

  setVariable::DipoleSndBendGenerator DBGen;
  DBGen.generatePipe(Control,frontKey+"MagnetBlockDipoleSndBend",2.8,15.0);

  // this reaches 454.5cm from the middle of the undulator
  PipeGen.setCF<CF40>();
  PipeGen.setAFlangeCF<CF63>();
  PipeGen.generatePipe(Control,frontKey+"DipolePipe",88.0);
  
  BellowGen.setCF<setVariable::CF40>();
  BellowGen.setAFlangeCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,frontKey+"BellowA",10.0);
  
  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,frontKey+"BellowB",10.0);

  // flange if possible
  CrossGen.setPlates(0.5,2.0,2.0);  // wall/Top/base
  CrossGen.setTotalPorts(10.0,10.0);     // len of ports (after main)
  CrossGen.setMat("Stainless304");
  // height/depth
  CrossGen.generateDoubleCF<setVariable::CF40,setVariable::CF100>
    (Control,frontKey+"IonPA",0.0,26.6,26.6);

  R1Ring::heatDumpTable(Control,frontKey);
  R1Ring::moveApertureTable(Control,frontKey);
  R1Ring::shutterTable(Control,frontKey);
  R1Ring::electronCutVariables(Control,frontKey+"MagnetBlock");

  return;
}

namespace R1Ring
{
  
void
collimatorVariables(FuncDataBase& Control,
		    const std::string& collKey)
  /*!
    Builds the variables for the collimator
    \param Control :: Database
    \param collKey :: prename
  */
{
  ELog::RegMethod RegA("R1RingVariables[F]","collimatorVariables");

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
electronCutVariables(FuncDataBase& Control,
		     const std::string& frontKey)
  /*!
    Builds the variables for the electon cut cells
    \param Control :: Database
    \param frontKey :: prename
  */
{
  ELog::RegMethod RegA("R1RingVariables[F]","electronCutVariables");
   
  Control.addVariable(frontKey+"ECutDiskYStep",2.0);
  Control.addVariable(frontKey+"ECutDiskLength",0.1);
  Control.addVariable(frontKey+"ECutDiskRadius",0.11);
  Control.addVariable(frontKey+"ECutDiskDefMat","H2Gas#0.1");

  Control.addVariable(frontKey+"ECutMagDiskYStep",2.0);
  Control.addVariable(frontKey+"ECutMagDiskDepth",0.1);
  Control.addVariable(frontKey+"ECutMagDiskWidth",4.6);
  Control.addVariable(frontKey+"ECutMagDiskHeight",1.8);
  Control.addVariable(frontKey+"ECutMagDiskDefMat","H2Gas#0.1");

  Control.addVariable(frontKey+"ECutWallDiskYStep",20.0);
  Control.addVariable(frontKey+"ECutWallDiskDepth",0.1);
  Control.addVariable(frontKey+"ECutWallDiskWidth",20.0);
  Control.addVariable(frontKey+"ECutWallDiskHeight",20.0);
  Control.addVariable(frontKey+"ECutWallDiskDefMat","H2Gas#0.1");

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
  ELog::RegMethod RegA("R1RingVariables[F]","heatDumpTable");

  setVariable::BellowGenerator BellowGen;
  setVariable::PipeGenerator PipeGen;
  setVariable::CrossGenerator CrossGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::PortItemGenerator PItemGen;
    
  PipeGen.setNoWindow();   // no window
  PipeGen.setMat("Stainless304");

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,frontKey+"BellowC",10.0);
  
  PipeGen.setCF<CF40>();
  PipeGen.generatePipe(Control,frontKey+"HeatPipe",113.0);

  heatDumpVariables(Control,frontKey);

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,frontKey+"BellowD",10.0);

  
  // will be rotated vertical
  // length 8.1
  CylGateValveGenerator CGTGen;
  CGTGen.generateGate(Control,frontKey+"GateTubeA",0);
  
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
  ELog::RegMethod RegA("R1RingVariables[F]","heatDumpVariables");

  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::HeatDumpGenerator HeatGen;

  SimpleTubeGen.setMat("Stainless304");
  SimpleTubeGen.setCF<CF150>();
  SimpleTubeGen.setCap(1,0);
  SimpleTubeGen.generateTube(Control,frontKey+"HeatBox",20.0);
  Control.addVariable(frontKey+"HeatBoxNPorts",2);

  // beam ports
  PItemGen.setCF<setVariable::CF40>(13.05);
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
shutterTable(FuncDataBase& Control,
	     const std::string& frontKey)
  /*!
    Set the variables for the shutter table (number 3)
    \param Control :: DataBase to use
    \param frontKey :: name before part names
  */
{
  ELog::RegMethod RegA("R1RingVariables[F]","shutterTable");

  setVariable::BellowGenerator BellowGen;
  setVariable::GateValveGenerator GateGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::PipeGenerator PipeGen;
  setVariable::BeamMountGenerator BeamMGen;
    
  // joined and open
  GateGen.setLength(3.5);
  GateGen.setCubeCF<setVariable::CF40>();
  GateGen.generateValve(Control,frontKey+"GateA",0.0,0);
  
  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,frontKey+"BellowI",10.0);
  
  SimpleTubeGen.setCF<CF100>();
  SimpleTubeGen.setCap();
  SimpleTubeGen.generateTube(Control,frontKey+"FlorTubeA",16.0);

  // beam ports
  const std::string florName(frontKey+"FlorTubeA");
  Control.addVariable(florName+"NPorts",4);
  const Geometry::Vec3D XVec(1,0,0);
  const Geometry::Vec3D ZVec(0,0,1);

  PItemGen.setCF<setVariable::CF40>(CF100::outerRadius+2.0);
  PItemGen.setPlate(0.0,"Void");  
  PItemGen.generatePort(Control,florName+"Port0",Geometry::Vec3D(0,0,0),ZVec);
  PItemGen.generatePort(Control,florName+"Port1",Geometry::Vec3D(0,0,0),-ZVec);
  PItemGen.generatePort(Control,florName+"Port2",Geometry::Vec3D(0,0,0),XVec);
  PItemGen.generatePort(Control,florName+"Port3",Geometry::Vec3D(0,0,0),-XVec);

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,frontKey+"BellowJ",10.0);

  // will be rotated vertical
  CylGateValveGenerator CGTGen;
  CGTGen.generateGate(Control,frontKey+"GateTubeB",0);
  
  PipeGen.setMat("Stainless304");
  PipeGen.setNoWindow();   // no window
  PipeGen.setCF<setVariable::CF40>();
  PipeGen.setBFlangeCF<setVariable::CF150>();
  PipeGen.generatePipe(Control,frontKey+"OffPipeA",6.8);
  Control.addVariable(frontKey+"OffPipeAFlangeBackZStep",3.0);


  const std::string shutterName=frontKey+"ShutterBox";
  const double sBoxLen(51.0);
  SimpleTubeGen.setCF<CF150>();
  SimpleTubeGen.setCap(0,0);
  SimpleTubeGen.generateTube(Control,shutterName,sBoxLen);
  Control.addVariable(frontKey+"ShutterBoxNPorts",2);
  
  // 20cm above port tube
  PItemGen.setCF<setVariable::CF50>(22.0);
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
  PipeGen.generatePipe(Control,frontKey+"OffPipeB",21.0);
  Control.addVariable(frontKey+"OffPipeBFlangeFrontZStep",3.0);
  Control.addVariable(frontKey+"OffPipeBZStep",-3.0);

  Control.addVariable(frontKey+"BremBlockRadius",3.0);
  Control.addVariable(frontKey+"BremBlockLength",20.0);
  Control.addVariable(frontKey+"BremBlockHoleWidth",2.0);
  Control.addVariable(frontKey+"BremBlockHoleHeight",2.0);
  Control.addVariable(frontKey+"BremBlockMainMat","Tungsten");

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.setAFlangeCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,frontKey+"BellowK",11.05);
  
  return;
}


void
moveApertureTable(FuncDataBase& Control,
		  const std::string& frontKey)
  /*!
    Builds the variables for the moveable apperature table
    containing two movable apertures, pumping and bellows
    \param Control :: Database
    \param frontKey :: prename
  */
{
  ELog::RegMethod RegA("R1RingVariables[F]","moveApertureTable");

  setVariable::BellowGenerator BellowGen;
  setVariable::PipeGenerator PipeGen;
  setVariable::CrossGenerator CrossGen;

  PipeGen.setNoWindow();   // no window
  PipeGen.setMat("Stainless304");
  PipeGen.setCF<CF40>();
  PipeGen.setBFlangeCF<CF63>();
  PipeGen.generatePipe(Control,frontKey+"PipeB",15.0);

  BellowGen.setCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,frontKey+"BellowE",14.0);

  // Aperture pipe is movable:
  PipeGen.setCF<CF63>();
  PipeGen.generatePipe(Control,frontKey+"AperturePipe",24.0);
  Control.addVariable(frontKey+"AperturePipeYStep",14.0);
  collimatorVariables(Control,frontKey+"MoveCollA");
  
  BellowGen.setCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,frontKey+"BellowF",14.0);

  // Stepped 420mm from pipeB so bellows/aperturePipe can move freely
  CrossGen.setMat("Stainless304");
  CrossGen.setPlates(0.5,2.0,2.0);  // wall/Top/base
  CrossGen.setTotalPorts(7.0,7.0);     // len of ports (after main)
  CrossGen.generateDoubleCF<setVariable::CF63,setVariable::CF100>
    (Control,frontKey+"IonPC",52.0,15.74,28.70);   // height/depth

  // [FREE FLOATING]
  BellowGen.setCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,frontKey+"BellowG",14.0);

  // Aperture pipe is movable:
  PipeGen.setCF<CF63>();
  PipeGen.generatePipe(Control,frontKey+"AperturePipeB",24.0);
  Control.addVariable(frontKey+"AperturePipeBYStep",14.0);
  collimatorVariables(Control,frontKey+"MoveCollB");
  Control.addVariable(frontKey+"MoveCollBYAngle",180.0);
  
  // [FREE FLOATING]
  BellowGen.setCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,frontKey+"BellowH",14.0);

  // [End fix for BellowH]
  PipeGen.setCF<CF40>();
  PipeGen.setAFlangeCF<CF63>();
  PipeGen.generatePipe(Control,frontKey+"PipeC",10.0);
  Control.addVariable(frontKey+"PipeCYStep",52.0);
  
  return;
}
  
}  // NAMSPACE R1Ring

void
createR1Shielding(FuncDataBase& Control,
		  const std::string& preName)
  /*!
    Construct the extra R1 shielding pieces : SideWall and 
    freestanding
    \param Control :: DataBase for variables
  */
{
  ELog::RegMethod RegA("R1RingVariables[F]","createR1Shielding");
    // Construct SideWall

  typedef std::tuple<std::string,size_t,double> OSTYPE;
  const std::vector<OSTYPE> outUnits=
    {
     OSTYPE("MaxPeem",8,-230.0)
    };

  // name : opticsSector : nearOuter/farOuter
  typedef std::tuple<std::string,size_t,bool> STYPE;
  const std::vector<STYPE> wallUnits=
    {
     STYPE("MaxPeem",8,0),
     STYPE("Species",7,0),
     STYPE("FlexPes",6,1)
    };
  
  //  Control.addVariable(preName+"NSideWall",wallUnits.size());
  Control.addVariable(preName+"NSideWall",1);
  size_t index(0);
  for(const STYPE& tc : wallUnits)
    {
      const std::string wallName=preName+std::get<0>(tc)+"SideWall";
      Control.addVariable
	(preName+"SideWallName"+std::to_string(index),wallName);
      Control.addVariable(wallName+"ID",std::get<1>(tc));
      index++;
    }

  // Base units:
  const std::string sWall(preName+"SideWall");
  Control.addVariable(sWall+"XStep",506.0);
  Control.addVariable(sWall+"Length",175.0);
  Control.addVariable(sWall+"Depth",4.0);
  Control.addVariable(sWall+"Height",40.0);
  Control.addVariable(sWall+"OutStep",4.0);
  Control.addVariable(sWall+"Mat","Lead");


  // Outer standing Shield block:
  Control.addVariable(preName+"NOutShield",outUnits.size());
  index=0;
  for(const OSTYPE& tc : outUnits)
    {
      const std::string outName=preName+std::get<0>(tc)+"OutShield";
      Control.addVariable
	(preName+"OutShieldName"+std::to_string(index),outName);
      Control.addVariable(outName+"ID",std::get<1>(tc));
      Control.addVariable(outName+"XStep",std::get<2>(tc));
      index++;
    }
  const std::string outBaseKey=preName+"OutShield";
  Control.addVariable(outBaseKey+"YStep",10.0);
  Control.addVariable(outBaseKey+"ZStep",0.0);
  Control.addVariable(outBaseKey+"Width",340.0);
  Control.addVariable(outBaseKey+"Depth",5.0);
  Control.addVariable(outBaseKey+"Height",50.0);
  Control.addVariable(outBaseKey+"DefMat","Stainless304");
  
  // Free standing Shield block:
  Control.addVariable(preName+"NFreeShield",wallUnits.size());
  index=0;
  for(const STYPE& tc : wallUnits)
    {
      const std::string wallName=preName+std::get<0>(tc)+"FreeShield";
      Control.addVariable
	(preName+"FreeShieldName"+std::to_string(index),wallName);
      Control.addVariable(wallName+"ID",std::get<1>(tc));
      if (std::get<2>(tc))   //
	Control.addVariable(wallName+"XStep",-100.0);

      index++;
    }
  // Non-beamline version
  const std::string baseKey=preName+"FreeShield";
  Control.addVariable(baseKey+"YStep",600.0);
  Control.addVariable(baseKey+"XStep",-70.0);
  Control.addVariable(baseKey+"Width",14.0);
  Control.addVariable(baseKey+"Depth",340.0);
  Control.addVariable(baseKey+"Height",24.0);
  Control.addVariable(baseKey+"DefMat","Stainless304");

  // Beam-beamline version

  return;
}
  
void
R1RingVariables(FuncDataBase& Control)
  /*!
    Function to set the control variables and constants
    for the R1 concrete shielding walls
    \param Control :: Function data base to add constants too
  */
{
  ELog::RegMethod RegA("R1RingVariables[F]","R1RingVariables");

  const std::string preName("R1Ring");
  
  Control.addVariable(preName+"FullOuterRadius",7000.0);
  Control.addVariable(preName+"HexRadius",800.0);
  Control.addVariable(preName+"HexWallThick",60.0);
  Control.addVariable(preName+"Height",160.0);
  Control.addVariable(preName+"Depth",130.0);
  Control.addVariable(preName+"RoofThick",60.0);
  Control.addVariable(preName+"RoofExtraVoid",40.0);
  Control.addVariable(preName+"FloorThick",100.0);

  Control.addVariable(preName+"WallMat","Concrete");  
  Control.addVariable(preName+"FloorMat","Concrete");
  Control.addVariable(preName+"RoofMat","Concrete");
  Control.addVariable(preName+"InnerMat","Void");
  Control.addVariable(preName+"OuterMat","Void");
  
  // construct points and inner
  Control.addVariable(preName+"NPoints",22);

  Control.addVariable(preName+"VPoint0",Geometry::Vec3D(7.94,1916.24,0.0));
  Control.addVariable(preName+"VPoint1",Geometry::Vec3D(158.46,1655.54,0.0));
  Control.addVariable(preName+"VPoint2",Geometry::Vec3D(443.6,1655.54,0.0));
  Control.addVariable(preName+"VPoint3",Geometry::Vec3D(1663.48,951.24,0.0));
  Control.addVariable(preName+"VPoint4",Geometry::Vec3D(1512.97,690.54,0.0));
  Control.addVariable(preName+"VPoint5",Geometry::Vec3D(1916.24,-7.94,0.0));
  Control.addVariable(preName+"VPoint6",Geometry::Vec3D(1655.54,-158.46,0.0));
  Control.addVariable(preName+"VPoint7",Geometry::Vec3D(1655.54,-965.0,0.0));
  Control.addVariable(preName+"VPoint8",Geometry::Vec3D(1354.51,-965.0,0.0));
  Control.addVariable(preName+"VPoint9",Geometry::Vec3D(1211.94,-1211.94,0.0));
  Control.addVariable(preName+"VPoint10",Geometry::Vec3D(-7.94,-1916.24,0.0));
  Control.addVariable(preName+"VPoint11",Geometry::Vec3D(-158.46,-1655.54,0.0));
  Control.addVariable(preName+"VPoint12",Geometry::Vec3D(-965.0,-1655.54,0.0));
  Control.addVariable(preName+"VPoint13",Geometry::Vec3D(-965.0,-1354.51,0.0));
  Control.addVariable(preName+"VPoint14",Geometry::Vec3D(-1663.48,-951.24,0.0));
  Control.addVariable(preName+"VPoint15",Geometry::Vec3D(-1512.97,-690.54,0.0));
  Control.addVariable(preName+"VPoint16",Geometry::Vec3D(-1916.24,7.94,0.0));
  Control.addVariable(preName+"VPoint17",Geometry::Vec3D(-1655.54,158.46,0.0));
  Control.addVariable(preName+"VPoint18",Geometry::Vec3D(-1655.54,965.0,0.0));
  Control.addVariable(preName+"VPoint19",Geometry::Vec3D(-1354.51,965.0,0.0));
  Control.addVariable(preName+"VPoint20",Geometry::Vec3D(-951.24,1663.48,0.0));
  Control.addVariable(preName+"VPoint21",Geometry::Vec3D(-690.54,1512.97,0.0));


  Control.addVariable(preName+"OPoint0",Geometry::Vec3D(103.83,  2110.16,0.0));
  Control.addVariable(preName+"OPoint1",Geometry::Vec3D(297.02,  1775.54,0.0));
  Control.addVariable(preName+"OPoint2",Geometry::Vec3D(475.75,  1775.54,0.0));
  Control.addVariable(preName+"OPoint3",Geometry::Vec3D(1878.37,  965.16,0.0));
  Control.addVariable(preName+"OPoint4",Geometry::Vec3D(1663.08,  590.54,0.0));
  Control.addVariable(preName+"OPoint5",Geometry::Vec3D(2055.52,  -89.19,0.0));
  Control.addVariable(preName+"OPoint6",Geometry::Vec3D(1735.54, -273.93,0.0));
  Control.addVariable(preName+"OPoint7",Geometry::Vec3D(1735.54, -1105.0,0.0));
  Control.addVariable(preName+"OPoint8",Geometry::Vec3D(1366.06, -1105.0,0.0));
  Control.addVariable(preName+"OPoint9",Geometry::Vec3D(1270.05, -1270.50,0.0));
  Control.addVariable(preName+"OPoint10",Geometry::Vec3D(-89.19,  -2055.52,0.0));
  Control.addVariable(preName+"OPoint11",Geometry::Vec3D(-273.93, -1735.54,0.0));
  Control.addVariable(preName+"OPoint12",Geometry::Vec3D(-1105.0, -1735.54,0.0));
  Control.addVariable(preName+"OPoint13",Geometry::Vec3D(-1105.0, -1366.06,0.0));
  Control.addVariable(preName+"OPoint14",Geometry::Vec3D(-1824.73, -950.52,0.0));
  Control.addVariable(preName+"OPoint15",Geometry::Vec3D(-1639.99, -630.54,0.0));
  Control.addVariable(preName+"OPoint16",Geometry::Vec3D(-2055.52,   89.19,0.0));
  Control.addVariable(preName+"OPoint17",Geometry::Vec3D(-1735.54,  273.93,0.0));
  Control.addVariable(preName+"OPoint18",Geometry::Vec3D(-1735.54, 1105.0,0.0));
  Control.addVariable(preName+"OPoint19",Geometry::Vec3D(-1366.06, 1105.0,0.0));
  Control.addVariable(preName+"OPoint20",Geometry::Vec3D(-950.52,  1824.73,0.0));
  Control.addVariable(preName+"OPoint21",Geometry::Vec3D(-670.54,  1663.08,0.0));

  createR1Shielding(Control,preName);
  return;

}

}  // NAMESPACE setVariable
