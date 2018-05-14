/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:    ESSBeam/heimdal/HEIMDALvariables.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#include <iterator>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "support.h"
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "essVariables.h"
#include "ShieldGenerator.h"
#include "FocusGenerator.h"
#include "ChopperGenerator.h"
#include "PitGenerator.h"
#include "PipeGenerator.h"
#include "RectPipeGenerator.h"
#include "JawGenerator.h"
#include "BladeGenerator.h"
#include "CryoGenerator.h"

namespace setVariable
{

 
  
void
HEIMDALvariables(FuncDataBase& Control)
  /*!
    Create all the beamline variables for vor
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("HEIMDALvariables[F]","HEIMDALvariables");

  setVariable::ChopperGenerator CGen;
  setVariable::CryoGenerator CryGen;
  setVariable::FocusGenerator FGen;
  setVariable::ShieldGenerator SGen;
  setVariable::PitGenerator PGen;
  setVariable::PipeGenerator PipeGen;
  setVariable::RectPipeGenerator RPipeGen;
  setVariable::BladeGenerator BGen;
  setVariable::JawGenerator JawGen;

  Control.addVariable("heimdalStartPoint",0);
  Control.addVariable("heimdalStopPoint",0);

  Control.addVariable("heimdalAxisZStep",0.0);
  
  PipeGen.setPipe(14.0,0.5);
  PipeGen.setWindow(-1.0,0.5);
  PipeGen.setFlange(-2.0,1.0);

  RPipeGen.setPipe(6.0,22.0,0.5);
  RPipeGen.setWindow(-2.0,-2.0,0.5);
  RPipeGen.setFlange(-4.0,-4.0,1.0);

  SGen.addWall(1,20.0,"CastIron");
  SGen.addRoof(1,20.0,"CastIron");
  SGen.addFloor(1,20.0,"CastIron");
  SGen.addFloorMat(5,"Concrete");
  SGen.addRoofMat(5,"Concrete");
  SGen.addWallMat(5,"Concrete");

  FGen.setLayer(1,0.5,"Copper");
  FGen.setLayer(2,0.5,"Void");
  FGen.setYOffset(2.0);
  FGen.generateTaper(Control,"heimdalFTA",350.0, 3.0,3.0, 3.0,3.0);
  FGen.generateRectangle(Control,"heimdalFCA",350.0,2.0,2.0);
  
  Control.addVariable("heimdalFTAZStep",0.0);   
  Control.addVariable("heimdalFCAZStep",-5.0);   
  Control.addVariable("heimdalFTAZAngle",0.0);   
  Control.addVariable("heimdalFCAZAngle",-1.3);   
  Control.addVariable("heimdalFTAXYAngle",1.0);   
  Control.addVariable("heimdalFCAXYAngle",-1.0);   

  RPipeGen.generatePipe(Control,"heimdalPipeB",6.5,46.0);
  Control.addVariable("heimdalPipeBZStep",-8.0);

  
  FGen.setLayer(1,0.5,"Aluminium");
  FGen.setYOffset(7.5);
  // THERMAL ON MASTER LINE:
  // COLD offset by 
  FGen.generateTaper(Control,"heimdalFTB",44.0,4.0,4.0,4.0,4.0);
  FGen.generateTaper(Control,"heimdalFCB",44.0,2.0,2.0,2.0,2.0);

  RPipeGen.setPipe(7.0,24.0,0.5);
  RPipeGen.generatePipe(Control,"heimdalPipeC",3.5,46.0);
  
  FGen.setYOffset(5.5);
  FGen.generateTaper(Control,"heimdalFTC",44.0,4.0,4.0,4.0,4.0);
  FGen.generateTaper(Control,"heimdalFCC",44.0,2.0,2.0,2.0,2.0);   

  CGen.setMainRadius(38.122);
  CGen.setFrame(86.5,86.5);
  CGen.generateChopper(Control,"heimdalTChopA",8.0,12.0,6.55);
  Control.addVariable("heimdalTChopAZStep",4.0);  // care must be put back
  
  // Single Blade chopper
  BGen.setThick({0.2});
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.generateBlades(Control,"heimdalADiskOne",-1.0,22.5,35.0);

  // Second Blade chopper
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.generateBlades(Control,"heimdalADiskTwo",1.0,22.5,35.0);

  PipeGen.setPipe(4.0,0.5);
  PipeGen.generatePipe(Control,"heimdalPipeTD",2.5,130.0);
  Control.addParse<double>("heimdalPipeTDZStep","-heimdalTChopAZStep");    
  
  FGen.clearYOffset();
  FGen.generateTaper(Control,"heimdalFTD",126.0,4.0,4.0,4.0,4.0);
  
  PipeGen.setPipe(3.0,0.5);
  PipeGen.generatePipe(Control,"heimdalPipeCD",15.0,530.0);
  FGen.setYOffset(18.0);
  FGen.generateTaper(Control,"heimdalFCD",126.0,2.0,2.0,2.0,2.0);   

  CGen.setMainRadius(38.122);
  CGen.setFrame(86.5,86.5);
  CGen.generateChopper(Control,"heimdalTChopB",8.0,6.0,3.55);
  Control.addVariable("heimdalTChopBZStep",4.0);
  
  // Single Blade chopper
  BGen.setThick({0.2});
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.generateBlades(Control,"heimdalBDisk",0.0,22.5,35.0);

  PipeGen.setPipe(4.5,0.5);
  PipeGen.generatePipe(Control,"heimdalPipeTE",2.5,1170.0);
  Control.addParse<double>("heimdalPipeTEZStep","-heimdalTChopBZStep");    
    
  FGen.clearYOffset();
  FGen.generateTaper(Control,"heimdalFTE",1166.0,4.0,4.0,4.0,4.0);

  // T0 chopper:
  CGen.setMaterial("Stainless304","Aluminium");
  CGen.generateChopper(Control,"heimdalChopperT0",12.0,16.0,12.0);
  Control.addVariable("heimdalChopperT0ZStep",4.0);
  
  // T0 Chopper disk 
  BGen.setMaterials("Inconnel","Tungsten");
  BGen.setThick({5.0});
  BGen.setInnerThick({5.4});
  BGen.addPhase({90,270},{30.0,30.0});

  Control.addVariable("heimdalT0MotorLength",20.0);
  Control.addVariable("heimdalT0MotorRadius",5.0);
  Control.addVariable("heimdalT0MotorMat","Copper");
  
  BGen.generateBlades(Control,"heimdalT0Disk",0.0,25.0,35.0);

  PipeGen.setPipe(4.5,0.5);
  PipeGen.generatePipe(Control,"heimdalPipeTF",2.5,444.0);
  Control.addParse<double>("heimdalPipeTFZStep","-heimdalChopperT0ZStep");    
    
  FGen.clearYOffset();
  FGen.generateTaper(Control,"heimdalFTF",440.0,4.0,4.0,4.0,4.0);

  return;
}
 
}  // NAMESPACE setVariable
