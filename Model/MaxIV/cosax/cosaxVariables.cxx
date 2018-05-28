/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   cosax/cosaxVariables.cxx
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
#include "BremCollGenerator.h"
#include "LeadPipeGenerator.h"
#include "CrossGenerator.h"
#include "GateValveGenerator.h"
#include "JawValveGenerator.h"
#include "PipeTubeGenerator.h"
#include "PortTubeGenerator.h"
#include "PortItemGenerator.h"
#include "VacBoxGenerator.h"
#include "MonoBoxGenerator.h"
#include "FlangeMountGenerator.h"
#include "MirrorGenerator.h"
#include "CollGenerator.h"

namespace setVariable
{

namespace cosaxVar
{
  
void
frontCaveVariables(FuncDataBase& Control,const double YStep)
  /*!
    Variable for the main ring front shielding
    \param Control :: Database
    \param YStep :: offset
  */
{
  ELog::RegMethod RegA("cosaxVariables[F]","frontCaveVariables");

  Control.addVariable("CosaxFrontEndWallYStep",YStep);
  Control.addVariable("CosaxFrontEndFrontWallThick",160.0);
  
  Control.addVariable("CosaxFrontEndLength",2100.0);
  Control.addVariable("CosaxFrontEndRingGap",75.0);
  Control.addVariable("CosaxFrontEndRingRadius",4000.0);
  Control.addVariable("CosaxFrontEndRingThick",80.0);

  Control.addVariable("CosaxFrontEndOuterGap",75.0);
  Control.addVariable("CosaxFrontEndOuterThick",80.0);

  Control.addVariable("CosaxFrontEndFloorDepth",75.0);
  Control.addVariable("CosaxFrontEndFloorThick",80.0);

  Control.addVariable("CosaxFrontEndRoofHeight",75.0);
  Control.addVariable("CosaxFrontEndRoofThick",80.0);

  Control.addVariable("CosaxFrontEndFrontHoleRadius",7.0);
  
  Control.addVariable("CosaxFrontEndFrontWallMat","Concrete");
  Control.addVariable("CosaxFrontEndWallMat","Concrete");
  Control.addVariable("CosaxFrontEndFloorMat","Concrete");
  Control.addVariable("CosaxFrontEndRoofMat","Concrete");
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
  ELog::RegMethod RegA("cosaxVariables[F]","frontEndVariables");

  setVariable::PipeGenerator PipeGen;
  setVariable::PortTubeGenerator PTubeGen;
  setVariable::VacBoxGenerator VBoxGen;
  setVariable::CollGenerator CollGen;
  
  PipeGen.setWindow(-2.0,0.0);   // no window

  VBoxGen.setMat("Stainless304");
  VBoxGen.setWallThick(1.0);
  VBoxGen.setCF<CF120>();
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

  PipeGen.setCF<CF120>();
  PipeGen.generatePipe(Control,frontKey+"DipolePipe",0,700.0);
  
  PTubeGen.setMat("Stainless304");
  PTubeGen.setWallThick(1.0);
  PTubeGen.setCF<CF120>();
  PTubeGen.setPortLength(5.0,5.0); // La/Lb
  // ystep/width/height/depth/length
  PTubeGen.generateTube(Control,frontKey+"CollimatorTubeA",
		       0.0,30.0,200.0);
  Control.addVariable(frontKey+"CollimatorTubeANPorts",0);

  PipeGen.setCF<CF120>();
  PipeGen.generatePipe(Control,frontKey+"CollABPipe",0,400.0);

  // collimator block
  CollGen.generateColl(Control,frontKey+"CollA",0.0,34.0);
  CollGen.generateColl(Control,frontKey+"CollB",0.0,34.0);

  Control.addVariable(frontKey+"ECutDiskYStep",2.0);
  Control.addVariable(frontKey+"ECutDiskLength",1.0);
  Control.addVariable(frontKey+"ECutDiskRadius",6.0);
  Control.addVariable(frontKey+"ECutDiskDefMat","H2Gas#0.1");
  
  
  
  // ystep/width/height/depth/length
  PTubeGen.generateTube(Control,frontKey+"CollimatorTubeB",
		       0.0,30.0,200.0);
  Control.addVariable(frontKey+"CollimatorTubeBNPorts",0);

  PipeGen.setMat("Stainless304");
  PipeGen.setCF<setVariable::CF120>(); // was 2cm (why?)
  PipeGen.generatePipe(Control,frontKey+"FlightPipe",0,325.0);

  return;
}


void
opticsCaveVariables(FuncDataBase& Control)
  /*!
    Variable for the main optics hutch walls
    \param Control :: Database
  */
{
  ELog::RegMethod RegA("cosaxVariables[F]","opticsCaveVariables");

  const std::string preName("CosaxOpticsHut");

  Control.addVariable(preName+"Depth",132.0);
  Control.addVariable(preName+"Height",250.0);
  Control.addVariable(preName+"Length",1034.6);
  Control.addVariable(preName+"OutWidth",260.0);
  Control.addVariable(preName+"RingWidth",61.3);
  Control.addVariable(preName+"RingWallLen",109.0);
  Control.addVariable(preName+"RingWallAngle",18.50);
  Control.addVariable(preName+"InnerThick",0.3);
  Control.addVariable(preName+"PbWallThick",2.0);
  Control.addVariable(preName+"PbRoofThick",2.0);
  Control.addVariable(preName+"PbBackThick",10.0);
  Control.addVariable(preName+"OuterThick",0.3);
  Control.addVariable(preName+"FloorThick",50.0);

  Control.addVariable(preName+"SkinMat","Stainless304");
  Control.addVariable(preName+"PbMat","Lead");
  Control.addVariable(preName+"FloorMat","Concrete");

  Control.addVariable(preName+"HoleXStep",0.0);
  Control.addVariable(preName+"HoleZStep",5.0);
  Control.addVariable(preName+"HoleRadius",7.0);

  return;
}
  
void
monoVariables(FuncDataBase& Control)
  /*!
    Set the variables for the mono
    \param Control :: DataBase to use
  */
{
  ELog::RegMethod RegA("cosaxVariables[F]","monoVariables");
  const std::string preName("CosaxOpticsLine");
  
  setVariable::MonoBoxGenerator VBoxGen;

  VBoxGen.setMat("Stainless304");
  VBoxGen.setWallThick(1.0);
  VBoxGen.setCF<CF63>();
  VBoxGen.setAPortCF<CF40>();
  VBoxGen.setPortLength(2.5,2.5); // La/Lb
  VBoxGen.setLids(3.0,1.0,1.0); // over/base/roof

  // ystep/width/height/depth/length

  VBoxGen.generateBox(Control,preName+"MonoBox",0.0,30.0,15.0,15.0,53.15);


  return;
}

void
diagUnit(FuncDataBase& Control,
	 const std::string& postName)
  /*!
    Construct variables for the diagnostic units
    \param Control :: Database
    \param postName :: component name
  */
{
  ELog::RegMethod RegA("","diagUnit");

  const std::string preName("CosaxOpticsLine");
  const double DLength(55.0);         // diag length [checked]
  setVariable::PortTubeGenerator PTubeGen;
  setVariable::PortItemGenerator PItemGen;
  
  PTubeGen.setMat("Stainless304");

  // ports offset by 24.5mm in x direction
  // length 425+ 75 (a) 50 b
  PTubeGen.setCF<CF63>();
  PTubeGen.setBPortCF<CF40>();
  PTubeGen.setPortLength(-5.0,-7.5);
  PTubeGen.setAPortOffset(2.45,0);
  PTubeGen.setBPortOffset(2.45,0);
  
  // ystep/radius length
  PTubeGen.generateTube(Control,preName+postName,0.0,7.5,DLength);
  Control.addVariable(preName+postName+"NPorts",7);

  const std::string portName=preName+postName+"Port";
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

  return;
}

  
void
opticsVariables(FuncDataBase& Control)
  /*
    Vacuum optics components in the optics hutch
    \param Control :: Function data base
  */
{
  ELog::RegMethod RegA("cosaxVariables[F]","opticsVariables");

  const std::string preName("CosaxOpticsLine");

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
  BremGen.generateColl(Control,preName+"BremCollA",0,6.5);

  PTubeGen.setMat("Stainless304");
  PTubeGen.setCF<CF63>();
  PTubeGen.setBPortCF<CF40>();
  PTubeGen.setPortLength(-6.0,-5.0);
  // ystep/radius length
  PTubeGen.generateTube(Control,preName+"FilterBoxA",0.0,7.5,25.0);
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
  FlangeGen.setBlade(3.0,5.0,0.5,0.0,"Graphite");  // 22 rotation
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
  BellowGen.generateBellow(Control,preName+"BellowC",0,12.0);
  
  GateGen.setCF<setVariable::CF40>();
  GateGen.generateValve(Control,preName+"GateB",0.0,0);

  // SLITS
  JawGen.setCF<setVariable::CF63>();
  JawGen.setLength(4.0);
  JawGen.setSlits(3.0,2.0,0.2,"Tantalum");
  JawGen.generateSlits(Control,preName+"SlitsA",0.0,0.8,0.8);

  BellowGen.setCF<setVariable::CF63>();
  BellowGen.setBFlangeCF<setVariable::CF100>();
  BellowGen.generateBellow(Control,preName+"BellowD",0,12.0);

  // diag unit 1 :
  
  BellowGen.setCF<setVariable::CF40>();
  BellowGen.setBFlangeCF<setVariable::CF100>();
  BellowGen.generateBellow(Control,preName+"BellowE",0,12.0);

  // SLITS
  JawGen.setCF<setVariable::CF100>();
  JawGen.setLength(8.0);
  JawGen.setSlits(3.0,2.0,0.2,"Tantalum");
  JawGen.generateSlits(Control,preName+"SlitsB",0.0,0.8,0.8);
  

  return;
}

void
connectingVariables(FuncDataBase& Control)
  /*!
    Variables for the connecting region
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("cosaxVariables[F]","connectingVariables");
  const std::string baseName="CosaxConnect";
  const Geometry::Vec3D OPos(0,0,0);
  const Geometry::Vec3D ZVec(0,0,-1);


  return;
}

}  // NAMESPACE cosaxVAR
  
void
COSAXvariables(FuncDataBase& Control)
  /*!
    Function to set the control variables and constants
    -- This version is for Photon Moderator
    \param Control :: Function data base to add constants too
  */
{
  ELog::RegMethod RegA("cosaxVariables[F]","cosaxVariables");


  Control.addVariable("sdefType","Wiggler");

  setVariable::PipeGenerator PipeGen;
  setVariable::LeadPipeGenerator LeadPipeGen;

  PipeGen.setWindow(-2.0,0.0);   // no window
  
  cosaxVar::frontCaveVariables(Control,500.0);  // Set to middle
  cosaxVar::frontEndVariables(Control,"CosaxFrontBeam");  

  PipeGen.setMat("Stainless304");
  PipeGen.setCF<setVariable::CF63>(); // was 2cm (why?)
  PipeGen.setAFlangeCF<setVariable::CF120>(); 
  PipeGen.generatePipe(Control,"CosaxJoinPipe",0,195.0);

  cosaxVar::opticsCaveVariables(Control);
  cosaxVar::opticsVariables(Control);
  cosaxVar::monoVariables(Control);
  cosaxVar::diagUnit(Control,"DiagBoxA");  


  PipeGen.generatePipe(Control,"CosaxJoinPipeB",0,1195.0);

  return;
}

}  // NAMESPACE setVariable
