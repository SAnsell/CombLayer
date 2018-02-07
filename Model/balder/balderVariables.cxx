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
#include "JawValveGenerator.h"
#include "PortTubeGenerator.h"
#include "PortItemGenerator.h"
#include "VacBoxGenerator.h"
#include "FlangeMountGenerator.h"
#include "MirrorGenerator.h"

namespace setVariable
{

void
frontEndVariables(FuncDataBase& Control,
		  const double YStep)
/*!
    Set the variables for the mono
    \param Control :: DataBase to use
  */
{
  ELog::RegMethod RegA("balderVariables[F]","frontEndVariables");
  setVariable::VacBoxGenerator VBoxGen;
  
  Control.addVariable("BalderFrontEndWallYStep",YStep);
  Control.addVariable("BalderFrontEndFrontWallThick",60.0);
  
  Control.addVariable("BalderFrontEndLength",200.0);
  Control.addVariable("BalderFrontEndRingGap",75.0);
  Control.addVariable("BalderFrontEndRingRadius",400.0);
  Control.addVariable("BalderFrontEndRingThick",80.0);

  Control.addVariable("BalderFrontEndOuterGap",75.0);
  Control.addVariable("BalderFrontEndOuterThick",80.0);

  Control.addVariable("BalderFrontEndFloorDepth",75.0);
  Control.addVariable("BalderFrontEndFloorThick",80.0);

  Control.addVariable("BalderFrontEndRoofHeight",75.0);
  Control.addVariable("BalderFrontEndRoofThick",80.0);

  Control.addVariable("BalderFrontEndFrontHoleRadius",7.0);

  
  Control.addVariable("BalderFrontEndFrontWallMat","Concrete");
  Control.addVariable("BalderFrontEndWallMat","Concrete");
  Control.addVariable("BalderFrontEndFloorMat","Concrete");
  Control.addVariable("BalderFrontEndRoofMat","Concrete");

  VBoxGen.setMat("Stainless304");
  VBoxGen.setWallThick(1.0);
  VBoxGen.setCF<CF63>();
  VBoxGen.setPortLength(5.0,5.0); // La/Lb
  // ystep/width/height/depth/length
  VBoxGen.generateBox(Control,"BalderWigglerBox",0.0,30.0,15.0,15.0,160.0);


  // Wiggler
  Control.addVariable("BalderWigglerLength",150.0);
  Control.addVariable("BalderWigglerBlockWidth",10.0);
  Control.addVariable("BalderWigglerBlockHeight",8.0);
  Control.addVariable("BalderWigglerBlockDepth",8.0);
  Control.addVariable("BalderWigglerBlockHGap",0.2);
  Control.addVariable("BalderWigglerBlockVGap",0.96);

  Control.addVariable("BalderWigglerBlockVCorner",1.0);
  Control.addVariable("BalderWigglerBlockHCorner",2.0);

  
  Control.addVariable("BalderWigglerVoidMat",0);
  Control.addVariable("BalderWigglerBlockMat","Iron_10H2O");
	

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
  ELog::RegMethod RegA("balderVariables[F]","monoVariables");
  
  Control.addVariable("BalderMonoVacYStep",YStep);
  Control.addVariable("BalderMonoVacZStep",2.0);
  Control.addVariable("BalderMonoVacRadius",33.0);
  Control.addVariable("BalderMonoVacRingWidth",21.5);
  Control.addVariable("BalderMonoVacOutWidth",16.5);
  Control.addVariable("BalderMonoVacWallThick",1.0);
  Control.addVariable("BalderMonoVacDoorThick",2.54);
  Control.addVariable("BalderMonoVacBackThick",2.54);
  Control.addVariable("BalderMonoVacDoorFlangeRad",4.0);
  Control.addVariable("BalderMonoVacRingFlangeRad",4.0);
  Control.addVariable("BalderMonoVacDoorFlangeLen",2.54);
  Control.addVariable("BalderMonoVacRingFlangeLen",2.54);

  Control.addVariable("BalderMonoVacInPortZStep",-2.0);
  Control.addVariable("BalderMonoVacOutPortZStep",2.0);
  
  Control.addVariable("BalderMonoVacPortRadius",5.0);
  Control.addVariable("BalderMonoVacPortLen",4.65);
  Control.addVariable("BalderMonoVacPortThick",0.3);
  Control.addVariable("BalderMonoVacPortFlangeLen",1.5);
  Control.addVariable("BalderMonoVacPortFlangeRad",2.7);

  Control.addVariable("BalderMonoVacWallMat","Stainless304");
  // CRYSTALS:
  Control.addVariable("BalderMonoXtalZStep",-2.0);
  Control.addVariable("BalderMonoXtalGap",4.0);
  Control.addVariable("BalderMonoXtalTheta",10.0);
  Control.addVariable("BalderMonoXtalPhiA",0.0);
  Control.addVariable("BalderMonoXtalPhiA",0.0);
  Control.addVariable("BalderMonoXtalWidth",10.0);
  Control.addVariable("BalderMonoXtalLengthA",8.0);
  Control.addVariable("BalderMonoXtalLengthB",12.0);
  Control.addVariable("BalderMonoXtalThickA",4.0);
  Control.addVariable("BalderMonoXtalThickB",3.0);
  Control.addVariable("BalderMonoXtalBaseThick",5.0);
  Control.addVariable("BalderMonoXtalBaseExtra",2.0);
  
  Control.addVariable("BalderMonoXtalMat","Silicon80K");
  Control.addVariable("BalderMonoXtalBaseMat","Copper");

  return;
}

void
opticsVariables(FuncDataBase& Control)
  /*
    Vacuum optics components in the 
    optics hutch
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
  PipeGen.generatePipe(Control,"BalderInitPipe",0,10.0);
  
  // flange if possible
  CrossGen.setPlates(0.5,2.0,2.0);  // wall/Top/base
  CrossGen.setPorts(5.75,5.75);     // len of ports (after main)
  CrossGen.setMat("Stainless304");

  CrossGen.generateDoubleCF<setVariable::CF40,setVariable::CF63>
    (Control,"BalderIonPA",0.0,10.0,26.5);

  // flange if possible
  CrossGen.setPlates(0.5,2.0,2.0);  // wall/Top/base
  CrossGen.setPorts(5.75,5.75);     // len of ports (after main)
  CrossGen.generateDoubleCF<setVariable::CF40,setVariable::CF63>
    (Control,"BalderTriggerPipe",0.0,15.0,10.0);  // ystep/height/depth

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.setBFlangeCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,"BalderBellowA",0,16.0);

  // ACTUALL ROUND PIPE + 4 filter tubles and 1 base tube [large]
  
  PTubeGen.setMat("Stainless304");
  PTubeGen.setCF<CF63>();
  PTubeGen.setPortLength(10.7,10.7);
  // ystep/width/height/depth/length
  PTubeGen.generateTube(Control,"BalderFilterBox",0.0,9.0,54.0);
  Control.addVariable("BalderFilterBoxNPorts",4);

  PItemGen.setCF<setVariable::CF50>(20.0);
  FlangeGen.setCF<setVariable::CF50>();
  FlangeGen.setBlade(3.0,5.0,0.5,22.0,"Tungsten");  // 22 rotation

  // centre of mid point
  Geometry::Vec3D CPos(0,-1.5*11.0,0);
  const Geometry::Vec3D ZVec(0,0,1);
  for(size_t i=0;i<4;i++)
    {
      const std::string name="BalderFilterBoxPort"+std::to_string(i);
      const std::string fname="BalderFilter"+std::to_string(i);      
      PItemGen.generatePort(Control,name,CPos,ZVec);
      CPos+=Geometry::Vec3D(0,11,0);
      const int upFlag((i) ? 0 : 1);
      FlangeGen.generateMount(Control,fname,upFlag);  // in beam
    }

  // filters:
  
  BellowGen.setCF<setVariable::CF40>();
  BellowGen.setAFlangeCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,"BalderBellowB",0,10.0);    

  GateGen.setLength(2.5);
  GateGen.setCF<setVariable::CF40>();
  GateGen.generateValve(Control,"BalderGateA",0.0,0);
    
  VBoxGen.setMat("Stainless304");
  VBoxGen.setWallThick(1.0);
  VBoxGen.setCF<CF40>();
  VBoxGen.setPortLength(5.0,5.0); // La/Lb
  // ystep/width/height/depth/length
  // [length is 177.4cm total]
  VBoxGen.generateBox(Control,"BalderMirrorBox",0.0,54.0,15.3,31.3,167.4);

  // y/z/theta/phi
  MirrGen.generateMirror(Control,"BalderMirror",0.0, 0.0, 5.0, 0.0);

  // y/z/theta/phi
  MirrGen.generateMirror(Control,"BalderMirrorB",0.0, 0.0, 5.0, 0.0);

  GateGen.generateValve(Control,"BalderGateB",0.0,0);

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.setBFlangeCF<setVariable::CF100>();
  BellowGen.generateBellow(Control,"BalderBellowC",0,10.0);    

  PipeGen.setCF<setVariable::CF100>(); // was 2cm (why?)
  // [length is 38.3cm total]
  PipeGen.generatePipe(Control,"BalderDriftA",0,38.3);
  // Length ignored  as joined front/back

  BellowGen.setCF<setVariable::CF100>();
  BellowGen.generateBellow(Control,"BalderMonoBellowA",0,50.0);   
  BellowGen.generateBellow(Control,"BalderMonoBellowB",0,50.0);
  
  // [length is 72.9cm total]
  // [offset after mono is 119.1cm ]
  PipeGen.setCF<setVariable::CF100>();    
  PipeGen.generatePipe(Control,"BalderDriftB",119.1,72.5); 
  Control.addVariable("BalderDriftBZStep",4.0);

  monoVariables(Control,119.1/2.0);  // mono middle of drift chambers A/B
  

  // joined and open
  GateGen.setCF<setVariable::CF100>();
  GateGen.generateValve(Control,"BalderGateC",0.0,0);

  // [length is 54.4cm total]
  PipeGen.setCF<setVariable::CF100>();    
  PipeGen.generatePipe(Control,"BalderDriftC",0,54.4); 

  // SLITS
  JawGen.setCF<setVariable::CF100>();
  JawGen.setLength(4.0);
  JawGen.setSlits(3.0,2.0,0.2,"Tantalum");
  JawGen.generateSlits(Control,"BalderSlitsA",0.0,0.8,0.8);


  PTubeGen.setCF<CF100>();
  PTubeGen.setPortLength(1.0,1.0);
  // ystep/width/height/depth/length
  PTubeGen.generateTube(Control,"BalderShieldPipe",0.0,9.0,54.0);

  Control.addVariable("BalderShieldPipeNPorts",4);

  // first Two ports are CF100 
  PItemGen.setCF<setVariable::CF100>(20.0);
  // centre of mid point
  CPos=Geometry::Vec3D(0,-15.0,0);
  const std::string nameShield="BalderShieldPipePort";

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
  BellowGen.generateBellow(Control,"BalderBellowD",0,10.0);    

  // joined and open
  GateGen.setCF<setVariable::CF40>();
  GateGen.generateValve(Control,"BalderGateD",0.0,0);

  VBoxGen.setCF<CF40>();
  VBoxGen.setPortLength(4.5,4.5); // La/Lb
  // [length is 177.4cm total]
  VBoxGen.generateBox(Control,"BalderMirrorBoxB",0.0,54.0,15.3,31.3,178.0);

  // small flange bellows
  BellowGen.setCF<setVariable::CF40>(); 
  BellowGen.setBFlangeCF<setVariable::CF100>(); 
  BellowGen.generateBellow(Control,"BalderBellowE",0,10.0);

  // SLITS [second pair]
  JawGen.setCF<setVariable::CF100>();
  JawGen.setLength(3.0);
  JawGen.setSlits(3.0,2.0,0.2,"Tantalum");
  JawGen.generateSlits(Control,"BalderSlitsB",0.0,0.8,0.8);

  PTubeGen.setCF<CF100>();
  PTubeGen.setPortLength(1.0,1.0);
  // ystep/radius/length
  PTubeGen.generateTube(Control,"BalderViewTube",0.0,9.0,39.0);

  Control.addVariable("BalderViewTubeNPorts",4);

  const std::string nameView("BalderViewTubePort");
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
    
  // small flange bellows
  BellowGen.setCF<setVariable::CF63>(); 
  BellowGen.setAFlangeCF<setVariable::CF100>(); 
  BellowGen.generateBellow(Control,"BalderBellowF",0,23.0);

  // Shuter pipe
  CrossGen.setPlates(1.0,2.5,2.5);  // wall/Top/base
  CrossGen.setPorts(2.0,2.0);     // len of ports (after main)
  CrossGen.setPorts(2.0,2.0);     // len of ports (after main)
  CrossGen.generateCF<setVariable::CF63>
    (Control,"BalderShutterPipe",0.0,8.0,13.5,13.5);

  // bellows on shield block
  BellowGen.setCF<setVariable::CF40>();
  BellowGen.setAFlangeCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,"BalderBellowG",0,10.0);    

    // joined and open
  GateGen.setCF<setVariable::CF40>();
  GateGen.generateValve(Control,"BalderGateE",0.0,0);

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
  PTubeGen.generateTube(Control,baseName+"IonPumpA",0.0,CF40::innerRadius,3.0);
  Control.addVariable(baseName+"IonPumpANPorts",1);
  PItemGen.generatePort(Control,baseName+"IonPumpAPort0",OPos,ZVec);

  LeadPipeGen.generateCladPipe(Control,baseName+"PipeB",0,188.0);
  
  BellowGen.generateBellow(Control,baseName+"BellowB",0,10.0);

  LeadPipeGen.generateCladPipe(Control,baseName+"PipeC",0,188.0);

  // ystep/width/height/depth/length
  PTubeGen.generateTube(Control,baseName+"IonPumpB",0.0,CF40::innerRadius,3.0);
  Control.addVariable(baseName+"IonPumpBNPorts",1);
  PItemGen.generatePort(Control,baseName+"IonPumpBPort0",OPos,ZVec);

  LeadPipeGen.generateCladPipe(Control,baseName+"PipeD",0,172.0);

  BellowGen.generateBellow(Control,baseName+"BellowC",0,10.0);

  return;
}
  
void
balderVariables(FuncDataBase& Control)
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


  
  Control.addVariable("BalderOpticsDepth",100.0);
  Control.addVariable("BalderOpticsHeight",200.0);
  Control.addVariable("BalderOpticsLength",1034.6);
  Control.addVariable("BalderOpticsOutWidth",250.0);
  Control.addVariable("BalderOpticsRingWidth",60.0);
  Control.addVariable("BalderOpticsRingWallLen",105.0);
  Control.addVariable("BalderOpticsRingWallAngle",18.50);
  Control.addVariable("BalderOpticsInnerThick",0.5);
  Control.addVariable("BalderOpticsPbThick",5.0);
  Control.addVariable("BalderOpticsOuterThick",0.5);
  Control.addVariable("BalderOpticsFloorThick",50.0);

  Control.addVariable("BalderOpticsSkinMat","Stainless304");
  Control.addVariable("BalderOpticsPbMat","Lead");
  Control.addVariable("BalderOpticsFloorMat","Concrete");

  Control.addVariable("BalderOpticsHoleXStep",0.0);
  Control.addVariable("BalderOpticsHoleZStep",5.0);
  Control.addVariable("BalderOpticsHoleRadius",7.0);

  frontEndVariables(Control,100.0);  // Set to middle

  PipeGen.setMat("Stainless304");
  PipeGen.setCF<setVariable::CF63>(); // was 2cm (why?)
  PipeGen.generatePipe(Control,"BalderJoinPipe",0,88.0);
  
  opticsVariables(Control);

  LeadPipeGen.setCF<setVariable::CF40>();
  LeadPipeGen.setCladdingThick(0.5);
  LeadPipeGen.generateCladPipe(Control,"BalderJoinPipeB",0,34.0);

  connectingVariables(Control);


  LeadPipeGen.generateCladPipe(Control,"BalderJoinPipeC",0,80.0);

  Control.addVariable("BalderExptYStep",1850.0);
  Control.addVariable("BalderExptDepth",120.0);
  Control.addVariable("BalderExptHeight",200.0);
  Control.addVariable("BalderExptLength",858.4);
  Control.addVariable("BalderExptOutWidth",198.50);
  Control.addVariable("BalderExptRingWidth",248.6);
  Control.addVariable("BalderExptInnerThick",0.5);
  Control.addVariable("BalderExptPbThick",5.0);
  Control.addVariable("BalderExptOuterThick",0.5);
  Control.addVariable("BalderExptFloorThick",50.0);

  Control.addVariable("BalderExptSkinMat","Stainless304");
  Control.addVariable("BalderExptPbMat","Lead");
  Control.addVariable("BalderExptFloorMat","Concrete");

  Control.addVariable("BalderExptHoleXStep",0.0);
  Control.addVariable("BalderExptHoleZStep",5.0);
  Control.addVariable("BalderExptHoleRadius",7.0);

  
  return;
}

}  // NAMESPACE setVariable
