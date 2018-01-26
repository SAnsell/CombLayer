/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   photon/balderVariables.cxx
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

#include "PipeGenerator.h"
#include "CrossGenerator.h"
#include "VacBoxGenerator.h"

namespace setVariable
{

void
monoVariables(FuncDataBase& Control,
	      const double YStep)
  /*!
    Set the variables for the mono
    \param Control :: DataBase to use
  */
{
  ELog::RegMethod RegA("balderVariables[F]","monoVariables");
  
  Control.addVariable("MonoVacYStep",YStep);
  Control.addVariable("MonoVacZStep",2.0);
  Control.addVariable("MonoVacRadius",33.0);
  Control.addVariable("MonoVacRingWidth",21.5);
  Control.addVariable("MonoVacOutWidth",16.5);
  Control.addVariable("MonoVacWallThick",1.0);
  Control.addVariable("MonoVacDoorThick",2.54);
  Control.addVariable("MonoVacBackThick",2.54);
  Control.addVariable("MonoVacDoorFlangeRad",4.0);
  Control.addVariable("MonoVacRingFlangeRad",4.0);
  Control.addVariable("MonoVacDoorFlangeLen",2.54);
  Control.addVariable("MonoVacRingFlangeLen",2.54);

  Control.addVariable("MonoVacInPortZStep",-2.0);
  Control.addVariable("MonoVacOutPortZStep",2.0);
  
  Control.addVariable("MonoVacPortRadius",5.0);
  Control.addVariable("MonoVacPortLen",4.65);
  Control.addVariable("MonoVacPortThick",0.3);
  Control.addVariable("MonoVacPortFlangeLen",1.5);
  Control.addVariable("MonoVacPortFlangeRad",2.5);

  Control.addVariable("MonoVacWallMat","Stainless304");

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
  setVariable::PipeGenerator PipeGen;
  setVariable::CrossGenerator CrossGen;
  setVariable::VacBoxGenerator VBoxGen;
  
  Control.addVariable("OpticsDepth",100.0);
  Control.addVariable("OpticsHeight",200.0);
  Control.addVariable("OpticsLength",800.0);
  Control.addVariable("OpticsOutWidth",120.0);
  Control.addVariable("OpticsRingWidth",60.0);
  Control.addVariable("OpticsRingLength",200.0);
  Control.addVariable("OpticsRingWallLen",200.0);
  Control.addVariable("OpticsRingWallAngle",20.0);
  Control.addVariable("OpticsInnerThick",0.5);
  Control.addVariable("OpticsPbThick",5.0);
  Control.addVariable("OpticsOuterThick",0.5);
  Control.addVariable("OpticsFloorThick",50.0);

  Control.addVariable("OpticsSkinMat","Stainless304");
  Control.addVariable("OpticsPbMat","Lead");
  Control.addVariable("OpticsFloorMat","Concrete");

  CrossGen.setPlates(0.5,1.0,1.0);
  CrossGen.setPorts(5.75,5.75);
  CrossGen.setFlange(2.0,1.0);
  CrossGen.setMat("Stainless304");
  CrossGen.generateCross(Control,"TriggerPipe",22.0,
			  1.60,5.0,10.0,26.5);
  
  PipeGen.setPipe(1.6,0.5);      // 8cm radius / 0.5cm wall
  PipeGen.setWindow(-2.0,0.0); 
  PipeGen.setFlange(-2.7,1.0);

  PipeGen.setMat("Stainless304");
  PipeGen.generatePipe(Control,"BellowA",0,16.0);

  VBoxGen.setMat("Stainless304");
  VBoxGen.setPort(3.3,10.7,0.5);
  VBoxGen.setFlange(0.5,0.8);
  // ystep/width/height/depth/length
  VBoxGen.generateBox(Control,"FilterBox",0.0,48.5,4.7,4.7,54.0);

  PipeGen.generatePipe(Control,"BellowB",0,10.0);

  CrossGen.setPlates(0.5,1.0,1.0);
  CrossGen.setPorts(1.1,1.1);
  CrossGen.setFlange(2.0,0.3);
  CrossGen.generateCross(Control,"IonPumpA",0.0,2.0,3.5,10.0,7.5);

  
  VBoxGen.setMat("Stainless304");
  VBoxGen.setWallThick(1.0);
  VBoxGen.setPort(2.0,5.0,0.6);  // R/Len/thickness
  VBoxGen.setFlange(0.5,0.8);
  // ystep/width/height/depth/length
  // [length is 177.4cm total]
  VBoxGen.generateBox(Control,"MirrorBox",0.0,54.0,15.3,31.3,167.4);

  CrossGen.generateCross(Control,"IonPumpB",0.0,2.0,3.5,10.0,7.5);

  PipeGen.setPipe(2.0,0.5);      // 2cm radius / 0.5cm wall
  PipeGen.setFlangePair(-0.8,0.8,-5.7,1.0);
  // [length is 10cm total]
  PipeGen.generatePipe(Control,"BellowC",0,10.0); 

  PipeGen.setPipe(5.0,0.5);      // 2cm radius / 0.5cm wall
  PipeGen.setFlange(-2.7,1.0);
  // [length is 38.3cm total]
  PipeGen.generatePipe(Control,"DriftA",0,38.3);
  // Length ignored  as joined front/back
  PipeGen.generatePipe(Control,"MonoBellowA",0,50.0);   
  PipeGen.generatePipe(Control,"MonoBellowB",0,50.0);
  
  // [length is 72.9cm total]
  // [offset is 119.1cm ]
  PipeGen.generatePipe(Control,"DriftB",119.1,72.9); 
  //  Control.addVariable("DriftBZStep",4.0);

  monoVariables(Control,119.1/2.0);  // mono middle of drift chamber


  
  return;
}

}  // NAMESPACE setVariable
