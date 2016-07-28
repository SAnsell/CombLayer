/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:    ESSBeam/bifrost/BIFROSTvariables.cxx
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#include "FocusGenerator.h"
#include "ShieldGenerator.h"
#include "ChopperGenerator.h"
#include "PitGenerator.h"
#include "PipeGenerator.h"
#include "JawGenerator.h"
#include "BladeGenerator.h"
#include "essVariables.h"


namespace setVariable
{
  
void
BIFROSTvariables(FuncDataBase& Control)
  /*!
    Create all the beamline variables for vor
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("BIFROSTvariables[F]","BIFROSTvariables");

  setVariable::ChopperGenerator CGen;
  setVariable::FocusGenerator FGen;
  setVariable::ShieldGenerator SGen;
  setVariable::PitGenerator PGen;
  setVariable::PipeGenerator PipeGen;
  setVariable::BladeGenerator BGen;

  PipeGen.setPipe(8.0,0.5);
  PipeGen.setWindow(-2.0,0.5);
  PipeGen.setFlange(-4.0,1.0);

  SGen.addWall(1,20.0,"CastIron");
  SGen.addRoof(1,20.0,"CastIron");
  SGen.addFloor(1,20.0,"CastIron");
  SGen.addFloorMat(5,"Concrete");
  SGen.addRoofMat(5,"Concrete");
  SGen.addWallMat(5,"Concrete");

  // extent of beamline
  Control.addVariable("bifrostStopPoint",0);
  Control.addVariable("bifrostAxisXYAngle",0.0);   // rotation
  Control.addVariable("bifrostAxisZAngle",0.0);   // rotation
  Control.addVariable("bifrostAxisZStep",2.0);   // rotation

  FGen.setGuideMat("Copper");
  FGen.setYOffset(0.0);
  FGen.setYOffset(8.0);
  FGen.generateTaper(Control,"bifrostFA",350.0,8.0,5.0 ,10.0,5.0);
  
  // Pipe in gamma shield
  PipeGen.generatePipe(Control,"bifrostPipeB",2.0,46.0);
  FGen.setGuideMat("Aluminium");
  FGen.clearYOffset();
  FGen.generateTaper(Control,"bifrostFB",44.0, 5.0,4.0, 5.0,4.0);

  Control.addVariable("bifrostAppAInnerWidth",4.0);
  Control.addVariable("bifrostAppAInnerHeight",4.0);
  Control.addVariable("bifrostAppAWidth",12.0);
  Control.addVariable("bifrostAppAHeight",12.0);
  Control.addVariable("bifrostAppADepth",5.0);
  Control.addVariable("bifrostAppAYStep",7.0);
  Control.addVariable("bifrostAppADefMat","Tungsten");

  // VACBOX A : 6.50m target centre
  //  Length 100.7 + Width [87.0] + Height [39.0] void Depth/2 + front
  CGen.setMainRadius(38.122);
  CGen.setFrame(86.5,86.5);
  CGen.generateChopper(Control,"bifrostChopperA",8.0,12.0,6.55);

  // Double Blade chopper
  BGen.setThick({0.2,0.2});
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.generateBlades(Control,"bifrostDBlade",0.0,25.0,35.0);

  // VACUUM PIPE: SDisk to T0 (A)
  PipeGen.setPipe(20.0,0.5);
  PipeGen.setWindow(-2.0,0.5);
  PipeGen.setFlange(-4.0,1.0);
  PipeGen.generatePipe(Control,"bifrostPipeC",2.0,400.0);

  FGen.clearYOffset();
  FGen.generateTaper(Control,"bifrostFC",396.0, 5.0 ,13.0, 5.0,13.0);   
  //  Control.addVariable("bifrostFCBeamYStep",1.10); 

  CGen.setMainRadius(38.122);
  CGen.setFrame(86.5,86.5);
  CGen.generateChopper(Control,"bifrostChopperB",9.0,12.0,6.55);

  // Singe Blade chopper FOC
  BGen.setThick({0.2});
  BGen.addPhase({95},{30.0});
  BGen.generateBlades(Control,"bifrostFOC1Blade",0.0,25.0,35.0);

  // VACUUM PIPE: from ChoperB to 6m holding point
  PipeGen.setPipe(20.0,0.5);
  PipeGen.setWindow(-2.0,0.5);
  PipeGen.setFlange(-4.0,1.0);
  PipeGen.generatePipe(Control,"bifrostPipeD",2.0,500.0);

  FGen.clearYOffset();
  FGen.generateRectangle(Control,"bifrostFD",496.0, 13.0,13.0);   

  // VACUUM PIPE: from ChoperB to 6m holding point
  PipeGen.setPipe(20.0,0.5);
  PipeGen.setWindow(-2.0,0.5);
  PipeGen.setFlange(-4.0,1.0);
  PipeGen.generatePipe(Control,"bifrostPipeE",2.0,400.0);

  FGen.clearYOffset();
  FGen.generateRectangle(Control,"bifrostFE",396.0, 13.0,13.0);   

  CGen.setMainRadius(38.122);
  CGen.setFrame(86.5,86.5);
  CGen.generateChopper(Control,"bifrostChopperC",9.0,12.0,6.55);

  // Singe Blade chopper FOC
  BGen.setThick({0.2});
  BGen.addPhase({95},{30.0});
  BGen.generateBlades(Control,"bifrostFOC2Blade",0.0,25.0,35.0);

  // VACUUM PIPE: from ChoperC with 4m
  PipeGen.setPipe(20.0,0.5);
  PipeGen.setWindow(-2.0,0.5);
  PipeGen.setFlange(-4.0,1.0);
  PipeGen.generatePipe(Control,"bifrostPipeF",2.0,487.5);

  FGen.clearYOffset();
  FGen.generateTaper(Control,"bifrostFF",483.5, 13.0,4.0, 13.0,4.0);   

  Control.addVariable("bifrostAppBInnerWidth",3.7);
  Control.addVariable("bifrostAppBInnerHeight",3.7);
  Control.addVariable("bifrostAppBWidth",12.0);
  Control.addVariable("bifrostAppBHeight",12.0);
  Control.addVariable("bifrostAppBDepth",5.0);
  Control.addVariable("bifrostAppBYStep",8.0);
  Control.addVariable("bifrostAppBDefMat","Tungsten");

  // BEAM INSERT:
  Control.addVariable("bifrostBInsertHeight",20.0);
  Control.addVariable("bifrostBInsertWidth",28.0);
  Control.addVariable("bifrostBInsertTopWall",1.0);
  Control.addVariable("bifrostBInsertLowWall",1.0);
  Control.addVariable("bifrostBInsertLeftWall",1.0);
  Control.addVariable("bifrostBInsertRightWall",1.0);
  Control.addVariable("bifrostBInsertWallMat","Stainless304");       


  // VACUUM PIPE: in bunker wall
  PipeGen.setPipe(6.0,0.5);
  PipeGen.setWindow(-2.0,0.5);
  PipeGen.setFlange(-4.0,1.0);
  PipeGen.generatePipe(Control,"bifrostPipeWall",1.0,318.0);
  // Guide in wall
  FGen.generateTaper(Control,"bifrostFWall",314.0,4.0,5.232, 4.0,5.232);

  
  // VACUUM PIPE: LEAVING BUNKER
  PipeGen.setPipe(6.0,0.5);
  PipeGen.setWindow(-2.0,0.5);
  PipeGen.setFlange(-4.0,1.0);
  PipeGen.generatePipe(Control,"bifrostPipe",1.0,318.0);
  // Guide in wall
  FGen.generateTaper(Control,"bifrostFWall",316.0,4.0,4.64, 4.0,4.64);

  // Shield: leaving bunker
  SGen.generateShield(Control,"bifrostShieldA",6500.0,40.0,40.0,40.0,4,8);
  // VACUUM PIPE: leaving bunker
  PipeGen.setPipe(6.0,0.5);
  PipeGen.setWindow(-2.0,0.5);
  PipeGen.setFlange(-4.0,1.0);
  PipeGen.generatePipe(Control,"bifrostPipeOutA",4.0,600);

  // Guide at 9.2m (part of a 4->8cm in 20m)
  FGen.clearYOffset();
  FGen.generateTaper(Control,"bifrostFOutA",596.0,5.232,5.84, 5.232,5.84);   

  // Second vacuum pipe out of bunker [before chopper pit]
  PipeGen.setPipe(6.0,0.5);
  PipeGen.setWindow(-2.0,0.5);
  PipeGen.setFlange(-4.0,1.0);
  PipeGen.generatePipe(Control,"bifrostPipeOutB",4.0,600);

  // Guide at 9.2m (part of a 4->8cm in 20m)
  FGen.clearYOffset();
  FGen.generateTaper(Control,"bifrostFOutB",596.0,5.84,7.04, 5.84,7.04);   

  // Second vacuum pipe out of bunker [before chopper pit]
  PipeGen.setPipe(7.0,0.5);
  PipeGen.setWindow(-2.0,0.5);
  PipeGen.setFlange(-4.0,1.0);
  PipeGen.generatePipe(Control,"bifrostPipeOutC",4.0,480.0);

  // Guide at 9.2m (part of a 4->8cm in 20m)
  FGen.clearYOffset();
  FGen.generateTaper(Control,"bifrostFOutC",476.0,7.04,8.0,7.04,8.0);   

  // ARRAY SECTION
  for(size_t i=0;i<8;i++)
    {
      const std::string strNum(StrFunc::makeString(i));
      PipeGen.generatePipe(Control,"bifrostPipeR"+strNum,4.0,600.0);
      FGen.generateRectangle(Control,"bifrostFOutR"+strNum,596.0,8.0,8.0);   
    }

  PGen.setFeLayer(6.0);
  PGen.setConcLayer(10.0);
  PGen.generatePit(Control,"bifrostOutPitA",6500.0,40.0,220.0,210.0,40.0);

  Control.addVariable("bifrostOutACutFrontShape","Square");
  Control.addVariable("bifrostOutACutFrontRadius",5.0);
  
  Control.addVariable("bifrostOutACutBackShape","Square");
  Control.addVariable("bifrostOutACutBackRadius",5.0);

  return;
}
 
}  // NAMESPACE setVariable
