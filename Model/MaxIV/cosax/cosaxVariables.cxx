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
#include "LeadPipeGenerator.h"
#include "CrossGenerator.h"
#include "GateValveGenerator.h"
#include "JawValveGenerator.h"
#include "PortTubeGenerator.h"
#include "PortItemGenerator.h"
#include "VacBoxGenerator.h"
#include "FlangeMountGenerator.h"
#include "MirrorGenerator.h"
#include "CollGenerator.h"

namespace setVariable
{

void
frontCaveVariables(FuncDataBase& Control,
		   const double YStep)
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
    Set the variables for the mono
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
		      110.0,30.0,15.0,15.0,210.0);

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
monoVariables(FuncDataBase& Control,
	      const double YStep)
  /*!
    Set the variables for the mono
    \param Control :: DataBase to use
  */
{
  ELog::RegMethod RegA("cosaxVariables[F]","monoVariables");
  
  Control.addVariable("CosaxMonoVacYStep",YStep);
  Control.addVariable("CosaxMonoVacZStep",2.0);
  Control.addVariable("CosaxMonoVacRadius",33.0);
  Control.addVariable("CosaxMonoVacRingWidth",21.5);
  Control.addVariable("CosaxMonoVacOutWidth",16.5);
  Control.addVariable("CosaxMonoVacWallThick",1.0);

  Control.addVariable("CosaxMonoVacDoorThick",2.54);
  Control.addVariable("CosaxMonoVacBackThick",2.54);
  Control.addVariable("CosaxMonoVacDoorFlangeRad",4.0);
  Control.addVariable("CosaxMonoVacRingFlangeRad",4.0);
  Control.addVariable("CosaxMonoVacDoorFlangeLen",2.54);
  Control.addVariable("CosaxMonoVacRingFlangeLen",2.54);

  Control.addVariable("CosaxMonoVacInPortZStep",-2.0);
  Control.addVariable("CosaxMonoVacOutPortZStep",2.0);
  
  Control.addVariable("CosaxMonoVacPortRadius",5.0);
  Control.addVariable("CosaxMonoVacPortLen",4.65);
  Control.addVariable("CosaxMonoVacPortThick",0.3);
  Control.addVariable("CosaxMonoVacPortFlangeLen",1.5);
  Control.addVariable("CosaxMonoVacPortFlangeRad",2.7);

  Control.addVariable("CosaxMonoVacWallMat","Stainless304");
  // CRYSTALS:
  Control.addVariable("CosaxMonoXtalZStep",-2.0);
  Control.addVariable("CosaxMonoXtalGap",4.0);
  Control.addVariable("CosaxMonoXtalTheta",10.0);
  Control.addVariable("CosaxMonoXtalPhiA",0.0);
  Control.addVariable("CosaxMonoXtalPhiA",0.0);
  Control.addVariable("CosaxMonoXtalWidth",10.0);
  Control.addVariable("CosaxMonoXtalLengthA",8.0);
  Control.addVariable("CosaxMonoXtalLengthB",12.0);
  Control.addVariable("CosaxMonoXtalThickA",4.0);
  Control.addVariable("CosaxMonoXtalThickB",3.0);
  Control.addVariable("CosaxMonoXtalBaseThick",5.0);
  Control.addVariable("CosaxMonoXtalBaseExtra",2.0);
  
  Control.addVariable("CosaxMonoXtalMat","Silicon80K");
  Control.addVariable("CosaxMonoXtalBaseMat","Copper");

  return;
}

void
opticsVariables(FuncDataBase& Control)
  /*
    Vacuum optics components in the optics hutch
    \param Control :: Function data base
  */
{
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
  PipeGen.generatePipe(Control,"CosaxInitPipe",0,10.0);
  
  // flange if possible
  CrossGen.setPlates(0.5,2.0,2.0);  // wall/Top/base
  CrossGen.setPorts(5.75,5.75);     // len of ports (after main)
  CrossGen.setMat("Stainless304");

  CrossGen.generateDoubleCF<setVariable::CF40,setVariable::CF63>
    (Control,"CosaxIonPA",0.0,10.0,26.5);

  // flange if possible
  CrossGen.setPlates(0.5,2.0,2.0);  // wall/Top/base
  CrossGen.setPorts(5.75,5.75);     // len of ports (after main)
  CrossGen.generateDoubleCF<setVariable::CF40,setVariable::CF63>
    (Control,"CosaxTriggerPipe",0.0,15.0,10.0);  // ystep/height/depth

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.setBFlangeCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,"CosaxBellowA",0,16.0);

  // ACTUALL ROUND PIPE + 4 filter tubles and 1 base tube [large]
  
  PTubeGen.setMat("Stainless304");
  PTubeGen.setCF<CF63>();
  PTubeGen.setPortLength(10.7,10.7);
  // ystep/width/height/depth/length
  PTubeGen.generateTube(Control,"CosaxFilterBox",0.0,9.0,54.0);
  Control.addVariable("CosaxFilterBoxNPorts",4);

  PItemGen.setCF<setVariable::CF50>(20.0);
  FlangeGen.setCF<setVariable::CF50>();
  FlangeGen.setBlade(3.0,5.0,0.5,22.0,"Tungsten");  // 22 rotation

  // centre of mid point
  Geometry::Vec3D CPos(0,-1.5*11.0,0);
  const Geometry::Vec3D ZVec(0,0,1);
  for(size_t i=0;i<4;i++)
    {
      const std::string name="CosaxFilterBoxPort"+std::to_string(i);
      const std::string fname="CosaxFilter"+std::to_string(i);      
      PItemGen.generatePort(Control,name,CPos,ZVec);
      CPos+=Geometry::Vec3D(0,11,0);
      const int upFlag((i) ? 0 : 1);
      FlangeGen.generateMount(Control,fname,upFlag);  // in beam
    }

  // filters:
  
  BellowGen.setCF<setVariable::CF40>();
  BellowGen.setAFlangeCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,"CosaxBellowB",0,10.0);    

  GateGen.setLength(2.5);
  GateGen.setCF<setVariable::CF40>();
  GateGen.generateValve(Control,"CosaxGateA",0.0,0);
    
  VBoxGen.setMat("Stainless304");
  VBoxGen.setWallThick(1.0);
  VBoxGen.setCF<CF40>();
  VBoxGen.setPortLength(5.0,5.0); // La/Lb
  // ystep/width/height/depth/length
  // [length is 177.4cm total]
  VBoxGen.generateBox(Control,"CosaxMirrorBox",0.0,54.0,15.3,31.3,167.4);

  // y/z/theta/phi
  MirrGen.generateMirror(Control,"CosaxMirror",0.0, 0.0, 5.0, 0.0);

  // y/z/theta/phi
  MirrGen.generateMirror(Control,"CosaxMirrorB",0.0, 0.0, 5.0, 0.0);

  GateGen.generateValve(Control,"CosaxGateB",0.0,0);

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.setBFlangeCF<setVariable::CF100>();
  BellowGen.generateBellow(Control,"CosaxBellowC",0,10.0);    

  PipeGen.setCF<setVariable::CF100>(); // was 2cm (why?)
  // [length is 38.3cm total]
  PipeGen.generatePipe(Control,"CosaxDriftA",0,38.3);
  // Length ignored  as joined front/back

  BellowGen.setCF<setVariable::CF100>();
  BellowGen.generateBellow(Control,"CosaxMonoBellowA",0,50.0);   
  BellowGen.generateBellow(Control,"CosaxMonoBellowB",0,50.0);
  
  // [length is 72.9cm total]
  // [offset after mono is 119.1cm ]
  PipeGen.setCF<setVariable::CF100>();    
  PipeGen.generatePipe(Control,"CosaxDriftB",119.1,72.5); 
  Control.addVariable("CosaxDriftBZStep",4.0);

  monoVariables(Control,119.1/2.0);  // mono middle of drift chambers A/B
  
  // joined and open
  GateGen.setCF<setVariable::CF100>();
  GateGen.generateValve(Control,"CosaxGateC",0.0,0);

  // [length is 54.4cm total]
  PipeGen.setCF<setVariable::CF100>();    
  PipeGen.generatePipe(Control,"CosaxDriftC",0,54.4); 

  Control.addVariable("CosaxBeamStopZStep",-2.0);
  Control.addVariable("CosaxBeamStopWidth",4.0);
  Control.addVariable("CosaxBeamStopHeight",3.2);
  Control.addVariable("CosaxBeamStopDepth",6.0);
  Control.addVariable("CosaxBeamStopDefMat","Tungsten");

  
  // SLITS
  JawGen.setCF<setVariable::CF100>();
  JawGen.setLength(4.0);
  JawGen.setSlits(3.0,2.0,0.2,"Tantalum");
  JawGen.generateSlits(Control,"CosaxSlitsA",0.0,0.8,0.8);


  PTubeGen.setCF<CF100>();
  PTubeGen.setPortLength(1.0,1.0);
  // ystep/width/height/depth/length
  PTubeGen.generateTube(Control,"CosaxShieldPipe",0.0,9.0,54.0);

  Control.addVariable("CosaxShieldPipeNPorts",4);

  // first Two ports are CF100 
  PItemGen.setCF<setVariable::CF100>(20.0);
  // centre of mid point
  CPos=Geometry::Vec3D(0,-15.0,0);
  const std::string nameShield="CosaxShieldPipePort";

  PItemGen.generatePort(Control,nameShield+"0",CPos,ZVec);
  PItemGen.generatePort(Control,nameShield+"1",CPos,-ZVec);

  PItemGen.setCF<setVariable::CF40>(10.0);
  
  PItemGen.generatePort(Control,nameShield+"2",
			Geometry::Vec3D(0,10,0),Geometry::Vec3D(-1,0,0));
  PItemGen.generatePort(Control,nameShield+"3",
			Geometry::Vec3D(0,15,0),Geometry::Vec3D(1,0,0));

  // bellows on shield block
  BellowGen.setCF<setVariable::CF40>();
  BellowGen.setAFlangeCF<setVariable::CF100>();
  BellowGen.generateBellow(Control,"CosaxBellowD",0,10.0);    

  // joined and open
  GateGen.setCF<setVariable::CF40>();
  GateGen.generateValve(Control,"CosaxGateD",0.0,0);

  VBoxGen.setCF<CF40>();
  VBoxGen.setPortLength(4.5,4.5); // La/Lb
  // [length is 177.4cm total]
  VBoxGen.generateBox(Control,"CosaxMirrorBoxB",0.0,54.0,15.3,31.3,178.0);

  // small flange bellows
  BellowGen.setCF<setVariable::CF40>(); 
  BellowGen.setBFlangeCF<setVariable::CF100>(); 
  BellowGen.generateBellow(Control,"CosaxBellowE",0,10.0);

  // SLITS [second pair]
  JawGen.setCF<setVariable::CF100>();
  JawGen.setLength(3.0);
  JawGen.setSlits(3.0,2.0,0.2,"Tantalum");
  JawGen.generateSlits(Control,"CosaxSlitsB",0.0,0.8,0.8);

  PTubeGen.setCF<CF100>();
  PTubeGen.setPortLength(1.0,1.0);
  // ystep/radius/length
  PTubeGen.generateTube(Control,"CosaxViewTube",0.0,9.0,39.0);

  Control.addVariable("CosaxViewTubeNPorts",4);

  const std::string nameView("CosaxViewTubePort");
  const Geometry::Vec3D XAxis(1,0,0);
  const Geometry::Vec3D YAxis(0,1,0);
  const Geometry::Vec3D ZAxis(0,0,1);

  PItemGen.setCF<setVariable::CF40>(5.0);
  PItemGen.generatePort(Control,nameView+"0",YAxis*14.0,XAxis);
  PItemGen.generatePort(Control,nameView+"1",YAxis*10.0,-XAxis);
  PItemGen.setCF<setVariable::CF63>(10.0);
  PItemGen.generatePort(Control,nameView+"2",-YAxis*2.0,ZAxis);
  PItemGen.generatePort(Control,nameView+"3",-YAxis*2.0,
			Geometry::Vec3D(1,-1,0));


  // centre of mid point
  const std::string fname="CosaxViewMount"+std::to_string(0);      
  const int upFlag(1);
  FlangeGen.setCF<setVariable::CF40>();
  FlangeGen.generateMount(Control,fname,upFlag);  // in beam

  // small flange bellows
  BellowGen.setCF<setVariable::CF63>(); 
  BellowGen.setAFlangeCF<setVariable::CF100>(); 
  BellowGen.generateBellow(Control,"CosaxBellowF",0,23.0);

  // Shutter pipe
  CrossGen.setPlates(1.0,2.5,2.5);  // wall/Top/base
  CrossGen.setPorts(3.0,3.0);     // len of ports (after main)
  CrossGen.generateCF<setVariable::CF63>
    (Control,"CosaxShutterPipe",0.0,8.0,13.5,13.5);

  // bellows on shield block
  BellowGen.setCF<setVariable::CF40>();
  BellowGen.setAFlangeCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,"CosaxBellowG",0,10.0);    

    // joined and open
  GateGen.setCF<setVariable::CF40>();
  GateGen.generateValve(Control,"CosaxGateE",0.0,0);

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

  Control.addVariable(baseName+"OuterRadius",30.0);
  
  setVariable::BellowGenerator BellowGen;
  setVariable::LeadPipeGenerator LeadPipeGen;
  setVariable::PortTubeGenerator PTubeGen;
  setVariable::PortItemGenerator PItemGen;
  PItemGen.setCF<setVariable::CF40>(3.0);

  
  BellowGen.setCF<CF40>();  
  BellowGen.generateBellow(Control,baseName+"BellowA",0,10.0);

  LeadPipeGen.setCF<CF40>();
  LeadPipeGen.setCladdingThick(0.5);
  LeadPipeGen.generateCladPipe(Control,baseName+"PipeA",0,172.0);
  
  PTubeGen.setMat("Stainless304");
  PTubeGen.setCF<CF40>();
  PTubeGen.setPortLength(3.0,3.0);
  // ystep/width/height/depth/length
  PTubeGen.generateTube(Control,baseName+"IonPumpA",0.0,CF40::innerRadius,4.0);
  Control.addVariable(baseName+"IonPumpANPorts",1);
  PItemGen.generatePort(Control,baseName+"IonPumpAPort0",OPos,ZVec);

  LeadPipeGen.generateCladPipe(Control,baseName+"PipeB",0,188.0);
  
  BellowGen.generateBellow(Control,baseName+"BellowB",0,10.0);

  LeadPipeGen.generateCladPipe(Control,baseName+"PipeC",0,188.0);

  // ystep/width/height/depth/length
  PTubeGen.generateTube(Control,baseName+"IonPumpB",0.0,
			CF40::innerRadius,4.0);
  
  Control.addVariable(baseName+"IonPumpBNPorts",1);
  PItemGen.generatePort(Control,baseName+"IonPumpBPort0",OPos,ZVec);

  LeadPipeGen.generateCladPipe(Control,baseName+"PipeD",0,172.0);

  BellowGen.generateBellow(Control,baseName+"BellowC",0,10.0);

  return;
}
  
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


  
  Control.addVariable("CosaxOpticsDepth",100.0);
  Control.addVariable("CosaxOpticsHeight",200.0);
  Control.addVariable("CosaxOpticsLength",1034.6);
  Control.addVariable("CosaxOpticsOutWidth",250.0);
  Control.addVariable("CosaxOpticsRingWidth",60.0);
  Control.addVariable("CosaxOpticsRingWallLen",105.0);
  Control.addVariable("CosaxOpticsRingWallAngle",18.50);
  Control.addVariable("CosaxOpticsInnerThick",0.3);
  Control.addVariable("CosaxOpticsPbWallThick",2.0);
  Control.addVariable("CosaxOpticsPbRoofThick",2.0);
  Control.addVariable("CosaxOpticsPbBackThick",10.0);
  Control.addVariable("CosaxOpticsOuterThick",0.3);
  Control.addVariable("CosaxOpticsFloorThick",50.0);

  Control.addVariable("CosaxOpticsSkinMat","Stainless304");
  Control.addVariable("CosaxOpticsPbMat","Lead");
  Control.addVariable("CosaxOpticsFloorMat","Concrete");

  Control.addVariable("CosaxOpticsHoleXStep",0.0);
  Control.addVariable("CosaxOpticsHoleZStep",5.0);
  Control.addVariable("CosaxOpticsHoleRadius",7.0);

  frontCaveVariables(Control,500.0);  // Set to middle
  frontEndVariables(Control,"CosaxFrontBeam");  

  PipeGen.setMat("Stainless304");
  PipeGen.setCF<setVariable::CF63>(); // was 2cm (why?)
  PipeGen.setAFlangeCF<setVariable::CF120>(); 
  PipeGen.generatePipe(Control,"CosaxJoinPipe",0,195.0);
  
  return;
}

}  // NAMESPACE setVariable
