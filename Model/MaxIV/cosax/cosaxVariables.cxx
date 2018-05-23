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
monoVariables(FuncDataBase& Control,
	      const double YStep)
  /*!
    Set the variables for the mono
    \param Control :: DataBase to use
  */
{
  ELog::RegMethod RegA("cosaxVariables[F]","monoVariables");
  

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
  setVariable::VacBoxGenerator VBoxGen;
  setVariable::PortTubeGenerator PTubeGen;
  setVariable::PortItemGenerator PItemGen;
  setVariable::GateValveGenerator GateGen;
  setVariable::JawValveGenerator JawGen;
  setVariable::FlangeMountGenerator FlangeGen;
  setVariable::MirrorGenerator MirrGen;

  PipeGen.setWindow(-2.0,0.0);   // no window

  BellowGen.setCF<setVariable::CF40>();
  BellowGen.setAFlangeCF<setVariable::CF63>();
  BellowGen.generateBellow(Control,preName+"InitBellow",0,6.0);

  CrossGen.setPlates(0.5,2.0,2.0);  // wall/Top/base
  CrossGen.setPorts(5.75,5.75);     // len of ports (after main)
  CrossGen.generateDoubleCF<setVariable::CF40,setVariable::CF63>
    (Control,preName+"TriggerPipe",0.0,15.0,15.0);  // ystep/height/depth

  
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
  
  return;
}

}  // NAMESPACE setVariable
