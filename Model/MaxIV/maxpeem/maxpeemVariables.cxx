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
#include "GrateMonoBoxGenerator.h"
#include "GratingMonoGenerator.h"
#include "TwinPipeGenerator.h"

namespace setVariable
{

namespace maxpeemVar
{
  void undulatorVariables(FuncDataBase&,const std::string&);
  void collimatorVariables(FuncDataBase&,const std::string&);
  void moveApertureTable(FuncDataBase&,const std::string&);
  void heatDumpVariables(FuncDataBase&,const std::string&);
  void shutterTable(FuncDataBase&,const std::string&);
  void transferVariables(FuncDataBase&,const std::string&);
  void opticsHutVariables(FuncDataBase&,const std::string&);
  void opticsBeamVariables(FuncDataBase&,const std::string&);
  void monoVariables(FuncDataBase&,const std::string&);
  void m1MirrorVariables(FuncDataBase&,const std::string&);
  void splitterVariables(FuncDataBase&,const std::string&);
  void shieldVariables(FuncDataBase&,const std::string&,const double);
  void slitPackageVariables(FuncDataBase&,const std::string&);


void
shieldVariables(FuncDataBase& Control,
		const std::string& shieldKey,
		const double YStep)
  /*!
    Build the shield unit variables
    \param Control :: Database
    \param shieldKey :: prename
    \param shieldKey :: distance of step
  */
{
  ELog::RegMethod RegA("maxpeemVariables[F]","shieldVariables");

  Control.addVariable(shieldKey+"YStep",YStep);
  Control.addVariable(shieldKey+"Length",7.0);
  Control.addVariable(shieldKey+"Width",60.0);
  Control.addVariable(shieldKey+"Height",60.0);
  Control.addVariable(shieldKey+"WallThick",0.5);
  Control.addVariable(shieldKey+"ClearGap",0.2);
  Control.addVariable(shieldKey+"WallMat","Stainless304");
  Control.addVariable(shieldKey+"Mat","Lead");

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
undulatorVariables(FuncDataBase& Control,
		   const std::string& undKey)
  /*!
    Builds the variables for the collimator
    \param Control :: Database
    \param undKey :: prename
  */
{
  ELog::RegMethod RegA("maxpeemVariables[F]","collimatorVariables");
  setVariable::PipeGenerator PipeGen;

  const double L(280.0);
  PipeGen.setMat("Aluminium");
  PipeGen.setWindow(-2.0,0.0);   // no window
  PipeGen.setCF<setVariable::CF100>();
  PipeGen.generatePipe(Control,undKey+"UPipe",0,L);

  Control.addVariable(undKey+"UPipeWidth",6.0);
  Control.addVariable(undKey+"UPipeHeight",0.6);
  Control.addVariable<double>(undKey+"UPipeYStep",-L/2.0);
  Control.addVariable(undKey+"UPipeFeThick",0.2);

  // undulator  
  Control.addVariable(undKey+"UndulatorVGap",2.0);  // mininum 11mm
  Control.addVariable(undKey+"UndulatorLength",247.989);   // 46.2mm*30*2
  Control.addVariable(undKey+"UndulatorMagnetWidth",6.0);
  Control.addVariable(undKey+"UndulatorMagnetDepth",3.0);
  Control.addVariable(undKey+"UndulatorSupportWidth",12.0);
  Control.addVariable(undKey+"UndulatorSupportThick",8.0);
  Control.addVariable(undKey+"UndulatorSupportLength",4.0);  // extra
  Control.addVariable(undKey+"UndulatorSupportVOffset",2.0);
  Control.addVariable(undKey+"UndulatorVoidMat","Void");
  Control.addVariable(undKey+"UndulatorMagnetMat","Iron");
  Control.addVariable(undKey+"UndulatorSupportMat","Copper");

    
  return;
}

void
splitterVariables(FuncDataBase& Control,
  		  const std::string& splitKey)
  /*!
    Builds the variables for the slitter
    \param Control :: Database
    \param splitKey :: prename
  */
{
  ELog::RegMethod RegA("maxpeemVariables[F]","splitVariables");
  setVariable::TwinPipeGenerator TwinGen;
  setVariable::BellowGenerator BellowGen;
  setVariable::GateValveGenerator GateGen;
  setVariable::PipeGenerator PipeGen;
  setVariable::PortTubeGenerator PTubeGen;
  setVariable::PortItemGenerator PItemGen;

  
  TwinGen.setCF<CF40>();
  TwinGen.setJoinFlangeCF<CF100>();
  TwinGen.setAPos(-2.7,0);
  TwinGen.setBPos(2.7,0);
  TwinGen.setXYAngle(4.0,-4.0);
  TwinGen.generateTwin(Control,splitKey+"Splitter",0.0,42.0);  

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.generateBellow(Control,splitKey+"BellowAA",0,16.0);
  BellowGen.generateBellow(Control,splitKey+"BellowBA",0,16.0);

  GateGen.setLength(3.5);
  GateGen.setCF<setVariable::CF40>();
  GateGen.generateValve(Control,splitKey+"GateAA",0.0,0);
  GateGen.generateValve(Control,splitKey+"GateBA",0.0,0);

  PTubeGen.setMat("Stainless304");
  PTubeGen.setCF<CF40>();
  PTubeGen.setPortLength(2.5,2.5);

  const std::string pumpNameA=splitKey+"PumpTubeAA";
  const std::string pumpNameB=splitKey+"PumpTubeBA";
  const Geometry::Vec3D zVec(0,0,-1);
  const Geometry::Vec3D centPoint(0,0,0);
  PTubeGen.generateCFTube<CF63>(Control,pumpNameA,0.0,20.0);
  Control.addVariable(pumpNameA+"NPorts",1);
  PTubeGen.generateCFTube<CF63>(Control,pumpNameB,0.0,20.0);
  Control.addVariable(pumpNameB+"NPorts",1);

  PItemGen.setCF<setVariable::CF63>(14.95);
  PItemGen.generatePort(Control,pumpNameA+"Port0",centPoint,zVec);
  PItemGen.generatePort(Control,pumpNameB+"Port0",centPoint,zVec);

  PipeGen.setMat("Stainless304");
  PipeGen.setWindow(-2.0,0.0);   // no window
  PipeGen.setCF<setVariable::CF40>();
  PipeGen.generatePipe(Control,splitKey+"OutPipeA",0,82.5);
  PipeGen.generatePipe(Control,splitKey+"OutPipeB",0,82.5);

  shieldVariables(Control,splitKey+"ScreenB",10.0);

  return;
}
  
void
m3MirrorVariables(FuncDataBase& Control,
		  const std::string& mirrorKey)
  /*!
    Builds the variables for the M3 Mirror
    \param Control :: Database
    \param mirrorKey :: prename
  */
{
  ELog::RegMethod RegA("maxpeemVariables[F]","m3MirrorVariables");

  setVariable::PipeGenerator PipeGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::GateValveGenerator GateGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::JawValveGenerator JawGen;
  setVariable::MirrorGenerator MirrGen;
  
  // will be rotated vertical
  const std::string viewName=mirrorKey+"ViewTube";
  SimpleTubeGen.setCF<CF63>();
  SimpleTubeGen.setBFlangeCF<CF100>();
  SimpleTubeGen.generateTube(Control,viewName,0.0,15.0);
  Control.addVariable(viewName+"NPorts",1);   // beam ports

  const double wallThick=setVariable::CF63::innerRadius+
    setVariable::CF63::wallThick;
  PItemGen.setCF<setVariable::CF40>(5.95);
  PItemGen.setPlate(0.0,"Void");  
  const Geometry::Vec3D angVec(0,cos(M_PI*37.0/180.0),-sin(M_PI*37.0/180.0));
  const double DLen=14.0-wallThick/sin(M_PI*37.0/180.0);

  PItemGen.setCF<setVariable::CF40>(DLen);
  PItemGen.setOuterVoid(0);
  PItemGen.generatePort(Control,viewName+"Port0",
			Geometry::Vec3D(0,7.5,0),-angVec);
  
  // Simple slits
  JawGen.setCF<setVariable::CF100>();
  JawGen.setLength(4.0);
  JawGen.setSlits(3.0,2.0,0.2,"Tantalum");
  JawGen.generateSlits(Control,mirrorKey+"SlitsB",0.0,0.8,0.8);

  // will be rotated vertical
  const std::string pumpName=mirrorKey+"PumpTubeB";
  SimpleTubeGen.setCF<CF150>();
  SimpleTubeGen.generateTube(Control,pumpName,0.0,40.0);
  Control.addVariable(pumpName+"NPorts",3);   // beam ports

  const Geometry::Vec3D ZVec(0,0,1);
  PItemGen.setCF<setVariable::CF63>(5.4);
  PItemGen.setPlate(0.0,"Void");  
  PItemGen.generatePort(Control,pumpName+"Port0",Geometry::Vec3D(0,0,0),ZVec);

  PItemGen.setCF<setVariable::CF63>(5.4);
  PItemGen.setPlate(0.0,"Void");  
  PItemGen.generatePort(Control,pumpName+"Port1",Geometry::Vec3D(0,0,0),-ZVec);

  const Geometry::Vec3D pAngVec(0,sin(M_PI*37.0/180.0),-cos(M_PI*37.0/180.0));
  const double PLen=14.0-8.05/cos(M_PI*37.0/180.0);
  PItemGen.setCF<setVariable::CF40>(PLen);
  PItemGen.setOuterVoid(0);
  PItemGen.generatePort(Control,pumpName+"Port2",
			Geometry::Vec3D(0,0,0),-pAngVec);

  PipeGen.setMat("Stainless304");
  PipeGen.setWindow(-2.0,0.0);   // no window
  PipeGen.setCF<setVariable::CF63>();
  PipeGen.setBFlangeCF<setVariable::CF150>();
  PipeGen.generatePipe(Control,mirrorKey+"OffPipeC",0,18.1);

  const std::string mName=mirrorKey+"M3Tube";
  SimpleTubeGen.setCF<CF150>();
  SimpleTubeGen.generateTube(Control,mName,0.0,32.0);  // centre 13.5cm
  Control.addVariable(mName+"NPorts",0);   // beam ports

  // mirror in M3Tube 
  MirrGen.setPlate(28.0,1.0,9.0);  //guess
  // y/z/theta/phi/radius
  MirrGen.generateMirror(Control,mirrorKey+"M3Mirror",0.0, 0.0, 2.0, 0.0,0.0);
  Control.addVariable(mirrorKey+"M3MirrorYAngle",90.0);

  PipeGen.setCF<setVariable::CF100>();
  PipeGen.setAFlangeCF<setVariable::CF150>();
  PipeGen.generatePipe(Control,mirrorKey+"OffPipeD",0,20.6);

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
  ELog::RegMethod RegA("maxpeemVariables[F]","monoVariables");

  setVariable::BellowGenerator BellowGen;
  setVariable::GrateMonoBoxGenerator MBoxGen;
  setVariable::GratingMonoGenerator MXtalGen;
  setVariable::GateValveGenerator GateGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::PipeGenerator PipeGen;
  
  MBoxGen.setMat("Stainless304");
  MBoxGen.setWallThick(1.0);
  MBoxGen.setCF<CF63>();
  MBoxGen.setAPortCF<CF63>();
  MBoxGen.setPortLength(7.5,7.5); // La/Lb
  MBoxGen.setLid(3.0,1.0,1.0); // over/base/roof

  // ystep/width/height/depth/length
  // 
  MBoxGen.generateBox(Control,monoKey+"MonoBox",0.0,41.2,12.8,12.8,117.1);
  Control.addVariable(monoKey+"MonoBoxPortBZStep",3.1);   //

  
  Control.addVariable(monoKey+"MonoBoxNPorts",0);   // beam ports (lots!!)
  PItemGen.setCF<setVariable::CF63>(7.5);
  PItemGen.setPlate(0.0,"Void");

  MXtalGen.generateGrating(Control,monoKey+"MonoXtal",0.0,3.0);

  PipeGen.setMat("Stainless304");
  PipeGen.setWindow(-2.0,0.0);   // no window
  PipeGen.setCF<setVariable::CF63>();
  PipeGen.generatePipe(Control,monoKey+"PipeG",0,7.0);

  // joined and open
  GateGen.setLength(7.5);
  GateGen.setCF<setVariable::CF63>();
  GateGen.generateValve(Control,monoKey+"GateC",0.0,0);
  
  BellowGen.setCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,monoKey+"BellowE",0,7.5);
  
  return;
}

void
slitPackageVariables(FuncDataBase& Control,
		     const std::string& slitKey)
  /*!
    Builds the variables for the Slit package
    \param Control :: Database
    \param slitKey :: prename
  */
{
  ELog::RegMethod RegA("maxpeemVariables[F]","slitPackageVariables");

  setVariable::PipeGenerator PipeGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::GateValveGenerator GateGen;
  setVariable::BellowGenerator BellowGen;
  
  PipeGen.setMat("Stainless304");
  PipeGen.setWindow(-2.0,0.0);   // no window
  PipeGen.setCF<setVariable::CF63>();
  PipeGen.generatePipe(Control,slitKey+"PipeC",0,33.6);


  shieldVariables(Control,slitKey+"ScreenA",0.0);

  PipeGen.setCF<setVariable::CF63>();
  PipeGen.setBFlangeCF<setVariable::CF150>();
  PipeGen.generatePipe(Control,slitKey+"PipeD",0,9.9);

  const std::string sName=slitKey+"SlitTube";
  const double tLen(50.2);
  SimpleTubeGen.setCF<CF150>();
  SimpleTubeGen.generateTube(Control,sName,0.0,tLen);  

  Control.addVariable(sName+"NPorts",4);   // beam ports (lots!!)
  PItemGen.setCF<setVariable::CF63>(6.1);
  PItemGen.setPlate(0.0,"Void");

  // -1/5 missed
  const Geometry::Vec3D XVec(1,0,0);
  const Geometry::Vec3D ZVec(0,0,1);
  const Geometry::Vec3D PStep(0,tLen/10.0,0);
  Geometry::Vec3D CPt(0.0,-tLen/2.0,0.0);
  CPt+=PStep*1.5;
  PItemGen.generatePort(Control,sName+"Port0",CPt,-XVec);
  CPt+=PStep*2.0;
  PItemGen.generatePort(Control,sName+"Port1",CPt,XVec);
  CPt+=PStep*2.0;
  PItemGen.generatePort(Control,sName+"Port2",CPt,-ZVec);
  CPt+=PStep*2.0;
  PItemGen.setOuterVoid(0);
  PItemGen.generatePort(Control,sName+"Port3",CPt,ZVec);

  PipeGen.setCF<setVariable::CF63>();
  PipeGen.setAFlangeCF<setVariable::CF150>();
  PipeGen.generatePipe(Control,slitKey+"PipeE",0,5.4);

  // joined and open
  GateGen.setLength(7.5);
  GateGen.setCF<setVariable::CF63>();
  GateGen.generateValve(Control,slitKey+"GateB",0.0,0);

  BellowGen.setCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,slitKey+"BellowD",0,7.5);

  PipeGen.setCF<setVariable::CF63>();
  PipeGen.generatePipe(Control,slitKey+"PipeF",0,14);


  return;
}
 
void
m1MirrorVariables(FuncDataBase& Control,
		  const std::string& mirrorKey)
  /*!
    Builds the variables for the M1 Mirror
    \param Control :: Database
    \param mirrorKey :: prename
  */
{
  ELog::RegMethod RegA("maxpeemVariables[F]","m1MirrorVariables");

  setVariable::PipeGenerator PipeGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::GateValveGenerator GateGen;
  setVariable::MirrorGenerator MirrGen;
  
  PipeGen.setMat("Stainless304");
  PipeGen.setWindow(-2.0,0.0);   // no window
  PipeGen.setCF<setVariable::CF63>();
  PipeGen.setBFlangeCF<setVariable::CF150>();
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
  PipeGen.generatePipe(Control,mirrorKey+"OffPipeB",0,6.8);
  Control.addVariable(mirrorKey+"OffPipeBFlangeFrontXStep",-2.0);
  Control.addVariable(mirrorKey+"OffPipeBXStep",2.0);


  // mirror in M1Tube 
  MirrGen.setPlate(28.0,1.0,9.0);  //guess
  // y/z/theta/phi/radius
  MirrGen.generateMirror(Control,mirrorKey+"M1Mirror",0.0, 0.0, 2.0, 0.0,0.0);
  Control.addVariable(mirrorKey+"M1MirrorYAngle",90.0);
  
  // joined and open
  GateGen.setLength(7.5);
  GateGen.setCF<setVariable::CF63>();
  GateGen.generateValve(Control,mirrorKey+"GateA",0.0,0);

  return;
}

  
void
opticsBeamVariables(FuncDataBase& Control,
		    const std::string& opticKey)
  /*!
    Builds the variables for the moveable apperature table
    containing two movable aperatures, pumping and bellows
    \param Control :: Database
    \param opticKey :: prename
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

  // will be rotated vertical
  const std::string collName=opticKey+"PumpTubeA";
  SimpleTubeGen.setCF<CF150>();
  SimpleTubeGen.generateTube(Control,collName,0.0,40.0);
  Control.addVariable(collName+"NPorts",3);   // beam ports
  
  PItemGen.setCF<setVariable::CF40>(5.95);
  PItemGen.setPlate(0.0,"Void");  
  PItemGen.generatePort(Control,collName+"Port0",Geometry::Vec3D(0,0,0),ZVec);

  PItemGen.setCF<setVariable::CF63>(4.95);
  PItemGen.setPlate(0.0,"Void");  
  PItemGen.generatePort(Control,collName+"Port1",Geometry::Vec3D(0,0,0),-ZVec);

  const Geometry::Vec3D angVec(0,sin(M_PI*35.0/180.0),-cos(M_PI*35.0/180.0));
  const double DLen=17.2-7.55/cos(M_PI*35.0/180.0);
  PItemGen.setCF<setVariable::CF40>(DLen);
  PItemGen.setOuterVoid(0);
  PItemGen.generatePort(Control,collName+"Port2",
			Geometry::Vec3D(0,0,0),-angVec);
  
  m1MirrorVariables(Control,opticKey);
  slitPackageVariables(Control,opticKey);
  monoVariables(Control,opticKey);
  m3MirrorVariables(Control,opticKey);
  splitterVariables(Control,opticKey);
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

  Control.addVariable(hutName+"BeamTubeRadius",80.0);


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

  undulatorVariables(Control,frontKey);
   
  Control.addVariable(frontKey+"ECutDiskYStep",2.0);
  Control.addVariable(frontKey+"ECutDiskLength",0.1);
  Control.addVariable(frontKey+"ECutDiskRadius",0.11);
  Control.addVariable(frontKey+"ECutDiskDefMat","H2Gas#0.1");

  // this reaches 454.5cm from the middle of the undulator
  PipeGen.setCF<CF40>();
  PipeGen.generatePipe(Control,frontKey+"DipolePipe",0,291.1+7.5);

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
