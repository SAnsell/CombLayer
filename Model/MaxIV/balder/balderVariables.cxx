/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   balder/balderVariables.cxx
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
#include <cmath>
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
#include "maxivVariables.h"

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
#include "MonoShutterGenerator.h"
#include "CollGenerator.h"
#include "SqrFMaskGenerator.h"
#include "PortChicaneGenerator.h"
#include "LeadBoxGenerator.h"
#include "PipeShieldGenerator.h"
#include "WallLeadGenerator.h"
#include "OpticsHutGenerator.h"
#include "TriggerGenerator.h"
#include "CylGateValveGenerator.h"

namespace setVariable
{

namespace balderVar
{

void wigglerVariables(FuncDataBase&,const std::string&);
void frontMaskVariables(FuncDataBase&,const std::string&);
void wallVariables(FuncDataBase&,const std::string&);
void monoShutterVariables(FuncDataBase&,const std::string&);
void exptHutVariables(FuncDataBase&,const std::string&);
  

void
wigglerVariables(FuncDataBase& Control,
		   const std::string& frontKey)
  /*!
    Builds the variables for the wiggler
    \param Control :: Database
    \param frontKey :: prename
  */
{
  ELog::RegMethod RegA("balderVariables[F]","wigglerVariables");

  setVariable::VacBoxGenerator VBoxGen;
  
  VBoxGen.setMat("Stainless304");
  VBoxGen.setWallThick(1.0);
  VBoxGen.setCF<CF40>();
  VBoxGen.setPortLength(5.0,5.0); // La/Lb
  // ystep/width/height/depth/length  (yste =498)
  VBoxGen.generateBox(Control,frontKey+"WigglerBox",
		      30.0,15.0,15.0,210.0);

  // Wiggler
  Control.addVariable(frontKey+"WigglerYStep",0.0);
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
  ELog::RegMethod RegA("balderVariables[F]","frontMaskVariables");
  setVariable::SqrFMaskGenerator FMaskGen;
  
  const double FM1dist(1172.60);
  const double FM2dist(1624.2);

  FMaskGen.setCF<CF100>();
  FMaskGen.setFrontGap(2.62,1.86);       // 1033.8
  FMaskGen.setBackGap(1.54,1.42);
  FMaskGen.setMinAngleSize(29.0,FM1dist,1000.0,1000.0);  // Approximated to get 1mrad 
  FMaskGen.generateColl(Control,preName+"CollA",FM1dist,35.0);


  FMaskGen.setFrontGap(2.13,2.146);
  FMaskGen.setBackGap(0.756,0.432);
  // approx for 800uRad x 200uRad  
  FMaskGen.setMinAngleSize(29.0,FM2dist,800.0,200.0);
  FMaskGen.generateColl(Control,preName+"CollB",FM2dist,34.2);

  // FM 3:
  FMaskGen.setMain(1.20,"Copper","Void");
  FMaskGen.setFrontGap(0.84,0.582);
  FMaskGen.setBackGap(0.750,0.357);
  // approx for 100uRad x 100uRad
  FMaskGen.setMinAngleSize(12.0,1600.0, 400.0, 100.0);
  FMaskGen.generateColl(Control,preName+"CollC",17.0/2.0,17.0);

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

  OpticsHutGenerator OGen; 

  OGen.addHole(Geometry::Vec3D(0,0,4.0),3.5);
  OGen.generateHut(Control,hutName,1034.0);


  Control.addVariable(hutName+"NChicane",4);
  PortChicaneGenerator PGen;
  PGen.generatePortChicane(Control,hutName+"Chicane0",470.0,-25.0);
  PGen.generatePortChicane(Control,hutName+"Chicane1",370.0,-25.0);
  PGen.generatePortChicane(Control,hutName+"Chicane2",-70.0,-25.0);
  PGen.generatePortChicane(Control,hutName+"Chicane3",-280.0,-25.0);


  return;
}

void
exptHutVariables(FuncDataBase& Control,
		 const std::string& beamName)
/*!
    Optics hut variables
    \param Control :: DataBase to add
    \param beamName :: Balder name
  */
{
  ELog::RegMethod RegA("balderVariables","opticsHutVariables");

  const double beamOffset(4.0);

  const std::string hutName(beamName+"ExptHut");
  
  Control.addVariable(hutName+"YStep",1850.0);

  Control.addVariable(hutName+"Height",200.0);
  Control.addVariable(hutName+"Length",858.4);
  Control.addVariable(hutName+"OutWidth",260);
  Control.addVariable(hutName+"RingWidth",200);
  Control.addVariable(hutName+"InnerThick",0.3);
  Control.addVariable(hutName+"PbFrontThick",0.5);
  Control.addVariable(hutName+"PbBackThick",0.5);
  Control.addVariable(hutName+"PbRoofThick",0.5);
  Control.addVariable(hutName+"PbWallThick",0.5);
  Control.addVariable(hutName+"OuterThick",0.3);

  Control.addVariable(hutName+"InnerOutVoid",10.0);
  Control.addVariable(hutName+"OuterOutVoid",10.0);

  Control.addVariable(hutName+"VoidMat","Void");
  Control.addVariable(hutName+"SkinMat","Stainless304");
  Control.addVariable(hutName+"PbMat","Lead");
  
  Control.addVariable(hutName+"HoleXStep",0.0);
  Control.addVariable(hutName+"HoleZStep",beamOffset);
  Control.addVariable(hutName+"HoleRadius",7.0);
  Control.addVariable(hutName+"HoleMat","Lead");


  Control.addVariable(hutName+"NChicane",4);
  PortChicaneGenerator PGen;
  PGen.generatePortChicane(Control,hutName+"Chicane0","Right",270.0,-25.0);
  PGen.generatePortChicane(Control,hutName+"Chicane1","Right",170.0,-25.0);
  PGen.setSize(4.0,40.0,30.0);
  PGen.generatePortChicane(Control,hutName+"Chicane2","Right",-70.0,-25.0);
  PGen.setSize(4.0,30.0,90.0);
  PGen.generatePortChicane(Control,hutName+"Chicane3","Right",70.0,15.0);

  return;
}
  
void
monoShutterVariables(FuncDataBase& Control,
		     const std::string& preName)
  /*!
    Construct Mono Shutter variables
    \param Control :: Database for variables
    \param preName :: Control ssytem
   */
{
  ELog::RegMethod RegA("balderVariables","monoShutterVariables");

  setVariable::GateValveGenerator GateGen;
  setVariable::BellowGenerator BellowGen;
  setVariable::MonoShutterGenerator MShutterGen;
  
  // up / up (true)
  MShutterGen.generateShutter(Control,preName+"MonoShutter",1,1);  
  
  // bellows on shield block
  BellowGen.setCF<setVariable::CF40>();
  BellowGen.setAFlangeCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,preName+"BellowG",10.0);    

    // joined and open
  GateGen.setCubeCF<setVariable::CF40>();
  GateGen.generateValve(Control,preName+"GateE",0.0,0);
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
  
  Control.addVariable(preName+"PShieldYStep",10.1);
  Control.addVariable(preName+"PShieldLength",10.0);
  Control.addVariable(preName+"PShieldWidth",80.0);
  Control.addVariable(preName+"PShieldHeight",80.0);
  Control.addVariable(preName+"PShieldWallThick",0.5);
  Control.addVariable(preName+"PShieldClearGap",1.0);
  Control.addVariable(preName+"PShieldWallMat","Stainless304");
  Control.addVariable(preName+"PShieldMat","Lead");
   
  Control.addVariable(preName+"NShieldYStep",0.2);
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
  Control.addVariable(preName+"MonoXtalBaseALength",10.0);
  Control.addVariable(preName+"MonoXtalBaseBLength",14.0);
  Control.addVariable(preName+"MonoXtalBaseGap",0.3);
  Control.addVariable(preName+"MonoXtalBaseThick",1.0);
  Control.addVariable(preName+"MonoXtalBaseExtra",2.0);
  
  Control.addVariable(preName+"MonoXtalMat","Silicon80K");
  Control.addVariable(preName+"MonoXtalBaseMat","Copper");

  // 20cm above port tube
  const Geometry::Vec3D XVecMinus(-1,0,0);
  PItemGen.setCF<setVariable::CF50>(10.0);
  PItemGen.setPlate(1.0,"LeadGlass");

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

  Control.addVariable(opticsName+"OuterLeft",70.0);
  Control.addVariable(opticsName+"OuterRight",50.0);
  Control.addVariable(opticsName+"OuterTop",60.0);

  setVariable::PipeGenerator PipeGen;
  setVariable::BellowGenerator BellowGen;
  setVariable::CrossGenerator CrossGen;
  setVariable::VacBoxGenerator VBoxGen;
  setVariable::PortTubeGenerator PTubeGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::GateValveGenerator GateGen;
  setVariable::JawValveGenerator JawGen;
  setVariable::FlangeMountGenerator FlangeGen;
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::MirrorGenerator MirrGen;
  setVariable::TriggerGenerator TGen;
  setVariable::CylGateValveGenerator GVGen;
  
  PipeGen.setNoWindow();

  // addaptor flange at beginning: [check]
  PipeGen.setCF<CF40>();
  PipeGen.setAFlangeCF<CF63>(); 
  PipeGen.generatePipe(Control,opticsName+"InitPipe",10.0);
  
    // will be rotated vertical
  TGen.setCF<CF100>();
  TGen.setVertical(15.0,25.0);
  TGen.setSideCF<setVariable::CF40>(10.0); // add centre distance?
  TGen.generateTube(Control,opticsName+"TriggerUnit");

  GVGen.generateGate(Control,opticsName+"GateTubeA",0);  // open
  
  BellowGen.setCF<setVariable::CF40>();
  BellowGen.setBFlangeCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,opticsName+"BellowA",16.0);

  // ACTUAL ROUND PIPE + 4 filter tubles and 1 base tube [large]
  
  PTubeGen.setMat("Stainless304");
  PTubeGen.setPipe(9.0,0.5);
  PTubeGen.setPortCF<CF63>();
  PTubeGen.setPortLength(10.7,10.7);
  // ystep/width/height/depth/length
  PTubeGen.generateTube(Control,opticsName+"FilterBox",0.0,54.0);
  Control.addVariable(opticsName+"FilterBoxNPorts",4);

  PItemGen.setCF<setVariable::CF50>(20.0);
  PItemGen.setPlate(CF50::flangeLength,"Stainless304");  

  FlangeGen.setCF<setVariable::CF50>();
  FlangeGen.setNoPlate();    
  FlangeGen.setBlade(0.3,5.0,0.5,22.0,"Graphite",1);  // 22 rotation

  const Geometry::Vec3D ZVec(0,0,1);
  // centre of mid point
  Geometry::Vec3D CPos(0,-1.5*11.0,0);
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
  BellowGen.generateBellow(Control,opticsName+"BellowB",10.0);    

  GateGen.setLength(2.5);
  GateGen.setCubeCF<setVariable::CF40>();
  GateGen.generateValve(Control,opticsName+"GateA",0.0,0);
    
  VBoxGen.setMat("Stainless304");
  VBoxGen.setWallThick(1.0);
  VBoxGen.setCF<CF40>();
  VBoxGen.setPortLength(5.0,5.0); // La/Lb
  // width/height/depth/length
  // [length is 177.4cm total]
  VBoxGen.generateBox(Control,opticsName+"MirrorBox",54.0,15.3,31.3,167.4);

  // x/y/z/theta/phi/radius
  MirrGen.generateMirror(Control,opticsName+"Mirror",0.0,0.0, 0.0, -0.5, 0.0,0.0);

  // x/y/z/theta/phi/radius
  MirrGen.generateMirror(Control,opticsName+"MirrorB",0.0,0.0, 0.0, 0.5, 0.0,0.0);

  GateGen.generateValve(Control,opticsName+"GateB",0.0,0);

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.setBFlangeCF<setVariable::CF100>();
  BellowGen.generateBellow(Control,opticsName+"BellowC",10.0);    

  PipeGen.setCF<setVariable::CF100>(); // was 2cm (why?)
  // [length is 38.3cm total]
  PipeGen.generatePipe(Control,opticsName+"DriftA",38.3);
  // Length ignored  as joined front/back

  BellowGen.setCF<setVariable::CF100>();
  BellowGen.generateBellow(Control,opticsName+"MonoBellowA",50.0);   
  BellowGen.generateBellow(Control,opticsName+"MonoBellowB",50.0);
  
  // [length is 72.9cm total]
  // [offset after mono is 119.1cm ]
  PipeGen.setCF<setVariable::CF100>();    
  PipeGen.generatePipe(Control,opticsName+"DriftB",62.5);
  Control.addVariable(opticsName+"DriftBYStep",119.1);
  Control.addVariable(opticsName+"DriftBZStep",4.0);

  monoVariables(Control,119.1/2.0);  // mono middle of drift chambers A/B
  
  // joined and open
  GateGen.setCubeCF<setVariable::CF100>();
  GateGen.generateValve(Control,opticsName+"GateC",0.0,0);

  // [length is 54.4cm total]
  PipeGen.setCF<setVariable::CF100>();    
  PipeGen.generatePipe(Control,opticsName+"DriftC",54.4); 

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

  PTubeGen.setPipe(9.0,0.5);
  PTubeGen.setPortCF<CF100>();
  PTubeGen.setPortLength(2.0,2.0);
  // ystep/width/height/depth/length
  PTubeGen.generateTube(Control,opticsName+"ShieldPipe",0.0,52.0);

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
  BellowGen.generateBellow(Control,opticsName+"BellowD",10.0);    

  // joined and open
  GateGen.setCubeCF<setVariable::CF40>();
  GateGen.generateValve(Control,opticsName+"GateD",0.0,0);

  VBoxGen.setCF<CF40>();
  VBoxGen.setPortLength(4.5,4.5); // La/Lb
  // [length is 177.4cm total]
  VBoxGen.generateBox(Control,opticsName+"MirrorBoxB",54.0,15.3,31.3,178.0);

  // small flange bellows
  BellowGen.setCF<setVariable::CF40>(); 
  BellowGen.setBFlangeCF<setVariable::CF100>(); 
  BellowGen.generateBellow(Control,opticsName+"BellowE",10.0);

  // SLITS [second pair]
  JawGen.setCF<setVariable::CF100>();
  JawGen.setLength(3.0);
  JawGen.setSlits(3.0,2.0,0.2,"Tantalum");
  JawGen.generateSlits(Control,opticsName+"SlitsB",0.0,0.8,0.8);

  const double viewOuterRad(9.5);
  PTubeGen.setPipe(9.0,0.5);
  PTubeGen.setPortCF<CF100>();
  PTubeGen.setPortLength(2.3,2.3);
  // ystep/radius/length
  PTubeGen.generateTube(Control,opticsName+"ViewTube",0.0,34.8);

  Control.addVariable(opticsName+"ViewTubeNPorts",4);

  const std::string nameView(opticsName+"ViewTubePort");
  const Geometry::Vec3D XAxis(1,0,0);
  const Geometry::Vec3D YAxis(0,1,0);
  const Geometry::Vec3D ZAxis(0,0,1);

 
  PItemGen.setCF<setVariable::CF40>(viewOuterRad+5.0);
  PItemGen.setPlate(CF40::flangeLength,"Copper");

  PItemGen.generatePort(Control,nameView+"0",YAxis*14.0,XAxis);
  PItemGen.generatePort(Control,nameView+"1",YAxis*10.0,-XAxis);
  PItemGen.setCF<setVariable::CF63>(viewOuterRad+10.0);
  PItemGen.generatePort(Control,nameView+"2",-YAxis*2.0,ZAxis);
  PItemGen.setCF<setVariable::CF63>(viewOuterRad/cos(M_PI/4)+10.0);
  PItemGen.generatePort(Control,nameView+"3",-YAxis*2.0,
			Geometry::Vec3D(1,-1,0));


  // centre of mid point
  const std::string fname=opticsName+"ViewMount"+std::to_string(0);
  FlangeGen.setBlade(2.2,5.0,0.5,22.0,"Graphite",1);  // 22 rotation
  FlangeGen.setCF<setVariable::CF40>();
  FlangeGen.setNoPlate();    
  FlangeGen.generateMount(Control,fname,0);    // out of  beam

  
  
  // small flange bellows
  BellowGen.setCF<setVariable::CF63>(); 
  BellowGen.setAFlangeCF<setVariable::CF100>(); 
  BellowGen.generateBellow(Control,opticsName+"BellowF",13.0);

  monoShutterVariables(Control,opticsName);
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
  Control.addVariable(opticsName+"NShield0YStep",1.5);
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
  setVariable::PipeTubeGenerator SimpleTubeGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::LeadBoxGenerator LBGen;
  setVariable::PipeShieldGenerator PSGen;
  
  
  BellowGen.setCF<CF40>();  
  BellowGen.generateBellow(Control,baseName+"BellowA",8.0);

  LBGen.setPlate(15.0,15.0,0.6);
  LBGen.generateBox(Control,baseName+"LeadA",4.5,12.0);
    
  LeadPipeGen.setCF<CF40>();
  LeadPipeGen.setFlangeLength(CF40::flangeLength*0.5,
			      CF40::flangeLength*0.5);
  LeadPipeGen.setCladdingThick(0.5);
  LeadPipeGen.generatePipe(Control,baseName+"PipeA",150.6);
  Control.addVariable(baseName+"PipeAYStep",9.65);
  
  SimpleTubeGen.setMat("Stainless304");
  SimpleTubeGen.setCF<CF40>();
  SimpleTubeGen.setFlangeLength(CF40::flangeLength*0.5,CF40::flangeLength*0.5);
  // ystep/length
  SimpleTubeGen.generateTube(Control,baseName+"IonPumpA",3.8);

  Control.addVariable(baseName+"IonPumpANPorts",0);
  PItemGen.setCF<setVariable::CF40>(CF40::outerRadius+3.0);
  PItemGen.setNoPlate();

  PItemGen.generatePort(Control,baseName+"IonPumpAPort0",OPos,ZVec);
  
  // temp offset
  LBGen.setPlate(15.0,15.0,0.6);
  LBGen.generateBox(Control,baseName+"PumpBoxA",5.00,12.0);
  //  PSGen.generateShield(Control,baseName+"PumpBoxAFShield",0.0,0.0);

  LeadPipeGen.generatePipe(Control,baseName+"PipeB",188.0);
  Control.addVariable(baseName+"PipeBYStep",10.0);
  
  BellowGen.generateBellow(Control,baseName+"BellowB",10.0);
  LBGen.generateBox(Control,baseName+"LeadB",4.5,12.0);
  
  LeadPipeGen.generatePipe(Control,baseName+"PipeC",188.0);
  Control.addVariable(baseName+"PipeCYStep",9.0);
  
  // ystep/width/height/depth/length
  SimpleTubeGen.generateTube(Control,baseName+"IonPumpB",2.8);
  LBGen.generateBox(Control,baseName+"PumpBoxB",4.5,12.0);
  
  Control.addVariable(baseName+"IonPumpBNPorts",1);
  PItemGen.generatePort(Control,baseName+"IonPumpBPort0",OPos,ZVec);
  
  LeadPipeGen.generatePipe(Control,baseName+"PipeD",172.0);
  Control.addVariable(baseName+"PipeDYStep",9.0);
  // PTubeGen.getTotalLength(0.5));

  BellowGen.generateBellow(Control,baseName+"BellowC",8.0);
  LBGen.generateBox(Control,baseName+"LeadC",4.5,12.0);
  
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

  PipeGen.setNoWindow();

  balderVar::wigglerVariables(Control,"BalderFrontBeam");
  // ystep [0] / dipole pipe / exit pipe
  setVariable::R3FrontEndVariables(Control,"Balder");  
  balderVar::frontMaskVariables(Control,"BalderFrontBeam");  
  
  PipeGen.setMat("Stainless304");
  PipeGen.setCF<setVariable::CF40>(); 
  PipeGen.generatePipe(Control,"BalderJoinPipe",130.0);

  balderVar::opticsHutVariables(Control,"BalderOpticsHut");
  balderVar::opticsVariables(Control,"Balder");

  LeadPipeGen.setCF<setVariable::CF40>();
  LeadPipeGen.setCladdingThick(0.5);
  LeadPipeGen.generatePipe(Control,"BalderJoinPipeB",84.5);

  balderVar::shieldVariables(Control);
  balderVar::connectingVariables(Control);

  // note bellow skip
  LeadPipeGen.generatePipe(Control,"BalderJoinPipeC",81.0);
  Control.addVariable("BalderJoinPipeCYStep",9.0);

  balderVar::exptHutVariables(Control,"Balder");

  const std::string exptName="BalderExptLine";
  
  Control.addVariable(exptName+"BeamStopYStep",777.0);
  Control.addVariable(exptName+"BeamStopRadius",10.0);
  Control.addVariable(exptName+"BeamStopThick",5.0);
  Control.addVariable(exptName+"BeamStopMat","Stainless304");
  
  Control.addVariable(exptName+"SampleYStep",406.0);
  Control.addVariable(exptName+"SampleRadius",10.0);
  Control.addVariable(exptName+"SampleMat","Copper");

  return;
}

}  // NAMESPACE setVariable
