/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:    ESSBeam/heimdal/HEIMDALvariables.cxx
 *
 * Copyright (c) 2004-2024 by Stuart Ansell
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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "CFFlanges.h"
#include "ShieldGenerator.h"
#include "FocusGenerator.h"
#include "ChopperGenerator.h"
#include "PitGenerator.h"
#include "PipeGenerator.h"
#include "JawGenerator.h"
#include "BladeGenerator.h"
#include "CryoGenerator.h"
#include "HeimdalCaveGenerator.h"

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
  setVariable::PipeGenerator RPipeGen;
  setVariable::BladeGenerator BGen;
  setVariable::JawGenerator JawGen;
  setVariable::HeimdalCaveGenerator CaveGen;

  Control.addVariable("heimdalStartPoint",0);
  Control.addVariable("heimdalStopPoint",0);

  Control.addVariable("heimdalAxisZStep",0.0);
  
  PipeGen.setPipe(14.0,0.5);
  PipeGen.setNoWindow();
  PipeGen.setFlange(-2.0,1.0);

  RPipeGen.setCF<CF200>();
  RPipeGen.setRectPipe(16.0,22.0);
  RPipeGen.setRectWindow(16.5,22.5,0.3); 
  RPipeGen.setRectFlange(20.0,26.0,1.0);

  SGen.addWall(1,20.0,"CastIron");
  SGen.addRoof(1,20.0,"CastIron");
  SGen.addFloor(1,20.0,"CastIron");
  SGen.addFloorMat(5,"Concrete");
  SGen.addRoofMat(5,"Concrete");
  SGen.addWallMat(5,"Concrete");

  // Double guide: increased wedge:
  Control.addVariable("G1BLineTop8Depth1",14.0);
  
  FGen.setLayer(1,0.8,"Copper");
  FGen.setLayer(2,0.2,"Void");
  FGen.setYOffset(2.0);
  FGen.generateGeneralTaper(Control,"heimdalFTA",334.819,
			    7.096,2.584,
			    3.154,2.584,
			    1.724,2.507,
			    1.724,2.507);

  FGen.generateRectangle(Control,"heimdalFCA",350.0,2.0,2.0);

  Control.addVariable("heimdalFTAXStep",2.35);
  Control.addVariable("heimdalFCAXStep",-2.85);
  Control.addVariable("heimdalFTAZStep",0.0);   
  Control.addVariable("heimdalFCAZStep",-4.71);   
  Control.addVariable("heimdalFTAXAngle",0.0);   
  Control.addVariable("heimdalFCAXAngle",-1.39);   
  Control.addVariable("heimdalFTAZAngle",1.1);  
  Control.addVariable("heimdalFCAZAngle",-0.4);  

  RPipeGen.generatePipe(Control,"heimdalPipeB",46.0);
  Control.addVariable("heimdalPipeBYStep",6.5);
  Control.addVariable("heimdalPipeBZStep",-6.0);

  
  FGen.setLayer(1,0.5,"Aluminium");
  FGen.setYOffset(7.5);
  // THERMAL ON MASTER LINE:
  // COLD offset by 
  FGen.generateTaper(Control,"heimdalFTB",44.0,4.0,4.0,4.0,4.0);
  FGen.generateTaper(Control,"heimdalFCB",44.0,2.0,2.0,2.0,2.0);

  //  RPipeGen.setPipe(7.0,24.0);
  RPipeGen.generatePipe(Control,"heimdalPipeC",46.0);
  Control.addVariable("heimdalPipeCYStep",3.5);
  
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
  PipeGen.generatePipe(Control,"heimdalPipeTD",130.0);
  Control.addVariable("heimdalPipeTDYStep",2.5);
  Control.addParse<double>("heimdalPipeTDZStep","-heimdalTChopAZStep");    
  
  FGen.clearYOffset();
  FGen.generateTaper(Control,"heimdalFTD",126.0,4.0,4.0,4.0,4.0);
  
  PipeGen.setPipe(3.0,0.5);
  PipeGen.generatePipe(Control,"heimdalPipeCD",530.0);
  Control.addVariable("heimdalPipeCDYStep",15.0);
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
  PipeGen.generatePipe(Control,"heimdalPipeTE",1170.0);
  Control.addVariable("heimdalPipeTEYStep",2.5);
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
  PipeGen.generatePipe(Control,"heimdalPipeTF",444.0);
  Control.addVariable("heimdalPipeTFYStep",2.5);
  Control.addParse<double>("heimdalPipeTFZStep","-heimdalChopperT0ZStep");    
    
  FGen.clearYOffset();
  FGen.generateTaper(Control,"heimdalFTF",440.0,4.0,4.0,4.0,4.0);

  
  CaveGen.generateCave(Control,"heimdalCave");
  Control.addVariable("heimdalCaveZAngle",4.7);
  
  return;
}
 
}  // NAMESPACE setVariable
