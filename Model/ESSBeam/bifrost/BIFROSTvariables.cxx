/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:    ESSBeam/bifrost/BIFROSTvariables.cxx
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
  Control.addVariable("bifrostAxisZStep",2.0);   // +/- height

  FGen.setLayer(1,0.5,"Copper");
  FGen.setYOffset(8.0);
  FGen.generateTaper(Control,"bifrostFA",350.0,8.0,5.0 ,10.0,5.0);
  
  // Pipe in gamma shield
  PipeGen.generatePipe(Control,"bifrostPipeB",8.0,46.0);
  FGen.setLayer(1,0.5,"Aluminium");
  FGen.clearYOffset();
  FGen.generateTaper(Control,"bifrostFB",44.0, 5.0,4.0, 5.0,4.0);

  Control.addVariable("bifrostAppAInnerWidth",4.0);
  Control.addVariable("bifrostAppAInnerHeight",4.0);
  Control.addVariable("bifrostAppAWidth",12.0);
  Control.addVariable("bifrostAppAHeight",12.0);
  Control.addVariable("bifrostAppAThick",5.0);
  Control.addVariable("bifrostAppAYStep",7.0);
  Control.addVariable("bifrostAppADefMat","Tungsten");

  // VACBOX A : 6.50m target centre
  //  Length 100.7 + Width [87.0] + Height [39.0] void Depth/2 + front
  CGen.setMainRadius(38.122);
  CGen.setFrame(86.5,86.5);
  CGen.generateChopper(Control,"bifrostChopperA", 8.0,12.0,6.55);

  // // T0 chopper if needed
  //  CGen.generateChopper(Control,"bifrostChopperA", 20.0, 36.0 ,32.0);
  // BGen.setMaterials("Inconnel","Tungsten");
  // BGen.setThick({20.0});
  // BGen.addPhase({95},{60.0});
  // BGen.generateBlades(Control,"bifrostDBlade",10.0,25.0,35.0);

  BGen.setThick({0.2,0.2});
  BGen.addPhase({95,275},{60.0,60.0});
  BGen.addPhase({95,275},{60.0,60.0});
  BGen.generateBlades(Control,"bifrostDBlade",0.0,20.0,35.0);

  // VACUUM PIPE: SDisk to T0 (A)
  PipeGen.setPipe(12.0,0.5);
  PipeGen.setWindow(-2.0,0.5);
  PipeGen.setFlange(-4.0,1.0);
  PipeGen.generatePipe(Control,"bifrostPipeC",2.0,400.0);
  Control.addVariable("bifrostPipeCNDivision",1);

  FGen.clearYOffset();
  FGen.generateTaper(Control,"bifrostFC",396.0, 5.0 ,13.0, 5.0,13.0);   
  //  Control.addVariable("bifrostFCBeamYStep",1.10); 

  CGen.setMainRadius(38.122);
  CGen.setFrame(86.5,86.5);
  CGen.generateChopper(Control,"bifrostChopperB",9.0,12.0,6.55);

  // Singe Blade chopper FOC
  BGen.setThick({0.2});
  BGen.addPhase({95},{60.0});
  BGen.generateBlades(Control,"bifrostFOC1Blade",0.0,20.0,35.0);

  // VACUUM PIPE: from ChoperB to 6m holding point
  PipeGen.setPipe(12.0,0.5);
  PipeGen.setWindow(-2.0,0.5);
  PipeGen.setFlange(-4.0,1.0);
  PipeGen.generatePipe(Control,"bifrostPipeD",2.0,500.0);

  FGen.clearYOffset();
  FGen.generateRectangle(Control,"bifrostFD",496.0, 13.0,13.0);   

  // VACUUM PIPE: from ChoperB to 6m holding point
  PipeGen.setPipe(12.0,0.5);
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
  BGen.addPhase({95},{60.0});
  BGen.generateBlades(Control,"bifrostFOC2Blade",0.0,20.0,35.0);

  // VACUUM PIPE: from ChoperC with 4m
  PipeGen.setPipe(12.0,0.5);
  PipeGen.setWindow(-2.0,0.5);
  PipeGen.setFlange(-4.0,1.0);
  PipeGen.generatePipe(Control,"bifrostPipeF",2.0,481.5);

  FGen.clearYOffset();
  FGen.generateTaper(Control,"bifrostFF",477.5, 13.0,4.0, 13.0,4.0);   

  Control.addVariable("bifrostAppBInnerWidth",3.7);
  Control.addVariable("bifrostAppBInnerHeight",3.7);
  Control.addVariable("bifrostAppBWidth",12.0);
  Control.addVariable("bifrostAppBHeight",12.0);
  Control.addVariable("bifrostAppBThick",5.0);
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



  // NEW BEAM INSERT:
  Control.addVariable("bifrostCInsertNBox",2);
  Control.addVariable("bifrostCInsertHeight",30.0);
  Control.addVariable("bifrostCInsertWidth",48.0);
  Control.addVariable("bifrostCInsertHeight1",20.0);
  Control.addVariable("bifrostCInsertWidth1",28.0);
  Control.addVariable("bifrostCInsertLength0",150.0);
  Control.addVariable("bifrostCInsertLength1",210.0);
  Control.addVariable("bifrostCInsertMat0","D2O");
  Control.addVariable("bifrostCInsertMat1","H2O");

  Control.addVariable("bifrostCInsertNWall",2);
  Control.addVariable("bifrostCInsertWallThick0",1.0);
  Control.addVariable("bifrostCInsertWallMat0","Nickel");
  Control.addVariable("bifrostCInsertWallThick1",2.0);
  Control.addVariable("bifrostCInsertWallMat1","Void");
  

  // VACUUM PIPE: in bunker wall
  PipeGen.setPipe(6.0,0.5);
  PipeGen.setWindow(-2.0,0.5);
  PipeGen.setFlange(-4.0,1.0);
  PipeGen.generatePipe(Control,"bifrostPipeWall",1.0,348.0);
  // Guide in wall
  FGen.generateTaper(Control,"bifrostFWall",344.0,4.0,5.232, 4.0,5.232);

  FGen.setLayer(1,0.5,"Glass");
  PipeGen.setMat("Stainless304");
  
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
  PGen.generatePit(Control,"bifrostOutPitA",6505.0,25.0,160.0,210.0,40.0);

  Control.addVariable("bifrostOutACutFrontShape","Square");
  Control.addVariable("bifrostOutACutFrontRadius",5.0);
  
  Control.addVariable("bifrostOutACutBackShape","Square");
  Control.addVariable("bifrostOutACutBackRadius",5.0);

  // FIRST out of bunker chopper
  CGen.setMainRadius(38.122);
  CGen.setFrame(86.5,86.5);
  CGen.generateChopper(Control,"bifrostChopperOutA",22.0,12.0,6.55);

  // Singe Blade chopper FOC
  BGen.setThick({0.2});
  BGen.addPhase({95},{60.0});
  BGen.generateBlades(Control,"bifrostFOCOutABlade",0.0,20.0,35.0);

  // Shield: leaving chopper pit A
  SGen.generateShield(Control,"bifrostShieldB",6275.0,40.0,40.0,40.0,4,8);

  // ARRAY SECTION
  double yStep(14.0);
  for(size_t i=0;i<7;i++)
    {
      const std::string strNum(StrFunc::makeString(i));
      PipeGen.generatePipe(Control,"bifrostPipeS"+strNum,yStep,600.0);
      FGen.generateRectangle(Control,"bifrostFOutS"+strNum,596.0,8.0,8.0);
      yStep=4.0;
    }

  FGen.setLayer(1,0.5,"Copper");
  double gap(8.0);
  for(size_t i=0;i<4;i++)
    {
      const std::string strNum(StrFunc::makeString(i));
      PipeGen.generatePipe(Control,"bifrostPipeE"+strNum,yStep,500.0);
      FGen.generateTaper(Control,"bifrostFOutE"+strNum,496.0,
                         gap,gap-1.0,gap,gap-1.0);
      gap-=1.0;
    }

  // HUT:
  
  Control.addVariable("bifrostCaveYStep",25.0);
  Control.addVariable("bifrostCaveXStep",100.0);
  Control.addVariable("bifrostCaveVoidFront",60.0);
  Control.addVariable("bifrostCaveVoidHeight",300.0);
  Control.addVariable("bifrostCaveVoidDepth",183.0);
  Control.addVariable("bifrostCaveVoidWidth",500.0);
  Control.addVariable("bifrostCaveVoidLength",600.0);

  Control.addVariable("bifrostCaveFeFront",25.0);
  Control.addVariable("bifrostCaveFeLeftWall",15.0);
  Control.addVariable("bifrostCaveFeRightWall",15.0);
  Control.addVariable("bifrostCaveFeRoof",15.0);
  Control.addVariable("bifrostCaveFeFloor",15.0);
  Control.addVariable("bifrostCaveFeBack",15.0);

  Control.addVariable("bifrostCaveConcFront",35.0);
  Control.addVariable("bifrostCaveConcLeftWall",35.0);
  Control.addVariable("bifrostCaveConcRightWall",35.0);
  Control.addVariable("bifrostCaveConcRoof",35.0);
  Control.addVariable("bifrostCaveConcFloor",50.0);
  Control.addVariable("bifrostCaveConcBack",35.0);

  Control.addVariable("bifrostCaveFeMat","Stainless304");
  Control.addVariable("bifrostCaveConcMat","Concrete");

  // Beam port through front of cave
  Control.addVariable("bifrostCaveCutShape","Circle");
  Control.addVariable("bifrostCaveCutRadius",10.0);

  // Second vacuum pipe out of bunker [before chopper pit]
  PipeGen.setPipe(6.0,0.5);
  PipeGen.setWindow(-2.0,0.5);
  PipeGen.setFlange(-4.0,1.0);
  PipeGen.generatePipe(Control,"bifrostPipeCave",2.0,250.0);

  return;
}
 
}  // NAMESPACE setVariable
