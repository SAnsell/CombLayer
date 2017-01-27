/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:    ESSBeam/miracles/MIRACLESvariables.cxx
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
MIRACLESvariables(FuncDataBase& Control)
  /*!
    Create all the beamline variables for vor
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("MIRACLESvariables[F]","MIRACLESvariables");

  setVariable::ChopperGenerator CGen;
  setVariable::FocusGenerator FGen;
  setVariable::ShieldGenerator SGen;
  setVariable::PitGenerator PGen;
  setVariable::PipeGenerator PipeGen;
  setVariable::BladeGenerator BGen;

  PipeGen.setPipe(8.0,0.5);
  PipeGen.setWindow(-2.0,0.5);
  PipeGen.setFlange(-2.0,1.0);

  SGen.addWall(1,20.0,"CastIron");
  SGen.addRoof(1,20.0,"CastIron");
  SGen.addFloor(1,20.0,"CastIron");
  SGen.addFloorMat(5,"Concrete");
  SGen.addRoofMat(5,"Concrete");
  SGen.addWallMat(5,"Concrete");

  // extent of beamline
  Control.addVariable("miraclesStopPoint",0);
  Control.addVariable("miraclesAxisXYAngle",0.0);   // rotation
  Control.addVariable("miraclesAxisZAngle",0.0);   // rotation
  Control.addVariable("miraclesAxisZStep",0.0);   // offset

  FGen.setGuideMat("Copper");
  FGen.setThickness(0.5,0.3);
  FGen.setYOffset(8.0);
  FGen.generateTaper(Control,"miraclesFA",350.0, 10.0,4.5 ,10.0,11.5);
  
  // Pipe in gamma shield
  PipeGen.generatePipe(Control,"miraclesPipeB",8.0,44.0);
  FGen.setGuideMat("Aluminium");
  FGen.clearYOffset();
  FGen.generateTaper(Control,"miraclesFB",42.0, 4.5,4.0, 11.5,12.0);

  // Pipe to collimator:
  PipeGen.generatePipe(Control,"miraclesPipeC",2.0,96.0);
  FGen.setYCentreOffset(-5.0);
  FGen.generateTaper(Control,"miraclesFC",84.0, 4.5,4.0, 11.5,12.0);

  Control.addVariable("miraclesAppAInnerWidth",4.0);
  Control.addVariable("miraclesAppAInnerHeight",4.0);
  Control.addVariable("miraclesAppAWidth",12.0);
  Control.addVariable("miraclesAppAHeight",12.0);
  Control.addVariable("miraclesAppADepth",5.0);
  Control.addVariable("miraclesAppAYStep",7.0);
  Control.addVariable("miraclesAppADefMat","Tungsten");

  // VACBOX A : 6.50m target centre
  //  Length 100.7 + Width [87.0] + Height [39.0] void Depth/2 + front
  CGen.setMainRadius(38.122);
  CGen.setFrame(86.5,86.5);
  CGen.generateChopper(Control,"miraclesChopperA",8.0,12.0,6.55);

  // Double Blade chopper
  BGen.setThick({0.2,0.2});
  BGen.addPhase({0,90,180,270},{7.95,40.0,7.95,40});
  BGen.addPhase({0,90,180,270},{7.95,40.0,7.95,40});
  BGen.generateBlades(Control,"miraclesDBlade",0.0,25.0,35.0);

  return;
  // VACUUM PIPE: SDisk to T0 (A)
  PipeGen.setPipe(20.0,0.5); 
  PipeGen.setWindow(-2.0,0.5);
  PipeGen.setFlange(-4.0,1.0);
  PipeGen.generatePipe(Control,"miraclesPipeCX",2.0,400.0);

  FGen.clearYOffset();
  FGen.generateTaper(Control,"miraclesFCX",396.0, 5.0 ,13.0, 5.0,13.0);   
  //  Control.addVariable("miraclesFCBeamYStep",1.10); 

  CGen.setMainRadius(38.122);
  CGen.setFrame(86.5,86.5);
  CGen.generateChopper(Control,"miraclesChopperB",9.0,12.0,6.55);

  // Singe Blade chopper FOC
  BGen.setThick({0.2});
  BGen.addPhase({95},{30.0});
  BGen.generateBlades(Control,"miraclesFOC1Blade",0.0,25.0,35.0);

  // VACUUM PIPE: from ChoperB to 6m holding point
  PipeGen.setPipe(20.0,0.5);
  PipeGen.setWindow(-2.0,0.5);
  PipeGen.setFlange(-4.0,1.0);
  PipeGen.generatePipe(Control,"miraclesPipeD",2.0,500.0);

  FGen.clearYOffset();
  FGen.generateRectangle(Control,"miraclesFD",496.0, 13.0,13.0);   

  // VACUUM PIPE: from ChoperB to 6m holding point
  PipeGen.setPipe(20.0,0.5);
  PipeGen.setWindow(-2.0,0.5);
  PipeGen.setFlange(-4.0,1.0);
  PipeGen.generatePipe(Control,"miraclesPipeE",2.0,400.0);

  FGen.clearYOffset();
  FGen.generateRectangle(Control,"miraclesFE",396.0, 13.0,13.0);   

  CGen.setMainRadius(38.122);
  CGen.setFrame(86.5,86.5);
  CGen.generateChopper(Control,"miraclesChopperC",9.0,12.0,6.55);

  // Singe Blade chopper FOC
  BGen.setThick({0.2});
  BGen.addPhase({95},{30.0});
  BGen.generateBlades(Control,"miraclesFOC2Blade",0.0,25.0,35.0);

  // VACUUM PIPE: from ChoperC with 4m
  PipeGen.setPipe(20.0,0.5);
  PipeGen.setWindow(-2.0,0.5);
  PipeGen.setFlange(-4.0,1.0);
  PipeGen.generatePipe(Control,"miraclesPipeF",2.0,487.5);

  FGen.clearYOffset();
  FGen.generateTaper(Control,"miraclesFF",483.5, 13.0,4.0, 13.0,4.0);   

  Control.addVariable("miraclesAppBInnerWidth",3.7);
  Control.addVariable("miraclesAppBInnerHeight",3.7);
  Control.addVariable("miraclesAppBWidth",12.0);
  Control.addVariable("miraclesAppBHeight",12.0);
  Control.addVariable("miraclesAppBDepth",5.0);
  Control.addVariable("miraclesAppBYStep",8.0);
  Control.addVariable("miraclesAppBDefMat","Tungsten");

  // BEAM INSERT:
  Control.addVariable("miraclesBInsertHeight",20.0);
  Control.addVariable("miraclesBInsertWidth",28.0);
  Control.addVariable("miraclesBInsertTopWall",1.0);
  Control.addVariable("miraclesBInsertLowWall",1.0);
  Control.addVariable("miraclesBInsertLeftWall",1.0);
  Control.addVariable("miraclesBInsertRightWall",1.0);
  Control.addVariable("miraclesBInsertWallMat","Stainless304");       



  // NEW BEAM INSERT:
  Control.addVariable("miraclesCInsertNBox",2);
  Control.addVariable("miraclesCInsertHeight",30.0);
  Control.addVariable("miraclesCInsertWidth",48.0);
  Control.addVariable("miraclesCInsertHeight1",20.0);
  Control.addVariable("miraclesCInsertWidth1",28.0);
  Control.addVariable("miraclesCInsertLength0",150.0);
  Control.addVariable("miraclesCInsertLength1",210.0);
  Control.addVariable("miraclesCInsertMat0","D2O");
  Control.addVariable("miraclesCInsertMat1","H2O");

  Control.addVariable("miraclesCInsertNWall",2);
  Control.addVariable("miraclesCInsertWallThick0",1.0);
  Control.addVariable("miraclesCInsertWallMat0","Nickel");
  Control.addVariable("miraclesCInsertWallThick1",2.0);
  Control.addVariable("miraclesCInsertWallMat1","Void");
  

  // VACUUM PIPE: in bunker wall
  PipeGen.setPipe(6.0,0.5);
  PipeGen.setWindow(-2.0,0.5);
  PipeGen.setFlange(-4.0,1.0);
  PipeGen.generatePipe(Control,"miraclesPipeWall",1.0,348.0);
  // Guide in wall
  FGen.generateTaper(Control,"miraclesFWall",344.0,4.0,5.232, 4.0,5.232);

  FGen.setGuideMat("Glass");
  PipeGen.setMat("Stainless304");
  
  // Shield: leaving bunker
  SGen.generateShield(Control,"miraclesShieldA",6500.0,40.0,40.0,40.0,4,8);
  // VACUUM PIPE: leaving bunker
  PipeGen.setPipe(6.0,0.5);
  PipeGen.setWindow(-2.0,0.5);
  PipeGen.setFlange(-4.0,1.0);
  PipeGen.generatePipe(Control,"miraclesPipeOutA",4.0,600);

  // Guide at 9.2m (part of a 4->8cm in 20m)
  FGen.clearYOffset();
  FGen.generateTaper(Control,"miraclesFOutA",596.0,5.232,5.84, 5.232,5.84);   

  // Second vacuum pipe out of bunker [before chopper pit]
  PipeGen.setPipe(6.0,0.5);
  PipeGen.setWindow(-2.0,0.5);
  PipeGen.setFlange(-4.0,1.0);
  PipeGen.generatePipe(Control,"miraclesPipeOutB",4.0,600);

  // Guide at 9.2m (part of a 4->8cm in 20m)

  FGen.clearYOffset();
  FGen.generateTaper(Control,"miraclesFOutB",596.0,5.84,7.04, 5.84,7.04);   

  // Second vacuum pipe out of bunker [before chopper pit]
  PipeGen.setPipe(7.0,0.5);
  PipeGen.setWindow(-2.0,0.5);
  PipeGen.setFlange(-4.0,1.0);
  PipeGen.generatePipe(Control,"miraclesPipeOutC",4.0,480.0);

  // Guide at 9.2m (part of a 4->8cm in 20m)
  FGen.clearYOffset();
  FGen.generateTaper(Control,"miraclesFOutC",476.0,7.04,8.0,7.04,8.0);   

  // ARRAY SECTION
  for(size_t i=0;i<8;i++)
    {
      const std::string strNum(StrFunc::makeString(i));
      PipeGen.generatePipe(Control,"miraclesPipeR"+strNum,4.0,600.0);
      FGen.generateRectangle(Control,"miraclesFOutR"+strNum,596.0,8.0,8.0);   
    }

  PGen.setFeLayer(6.0);
  PGen.setConcLayer(10.0);
  PGen.generatePit(Control,"miraclesOutPitA",6505.0,25.0,220.0,210.0,40.0);

  Control.addVariable("miraclesOutACutFrontShape","Square");
  Control.addVariable("miraclesOutACutFrontRadius",5.0);
  
  Control.addVariable("miraclesOutACutBackShape","Square");
  Control.addVariable("miraclesOutACutBackRadius",5.0);

  // FIRST out of bunker chopper
  CGen.setMainRadius(38.122);
  CGen.setFrame(86.5,86.5);
  CGen.generateChopper(Control,"miraclesChopperOutA",22.0,12.0,6.55);

  // Singe Blade chopper FOC
  BGen.setThick({0.2});
  BGen.addPhase({95},{30.0});
  BGen.generateBlades(Control,"miraclesFOCOutABlade",0.0,25.0,35.0);

  // Shield: leaving chopper pit A
  SGen.generateShield(Control,"miraclesShieldB",6275.0,40.0,40.0,40.0,4,8);

  // ARRAY SECTION
  double yStep(14.0);
  for(size_t i=0;i<7;i++)
    {
      const std::string strNum(StrFunc::makeString(i));
      PipeGen.generatePipe(Control,"miraclesPipeS"+strNum,yStep,600.0);
      FGen.generateRectangle(Control,"miraclesFOutS"+strNum,596.0,8.0,8.0);
      yStep=4.0;
    }

  FGen.setGuideMat("Copper");
  double gap(8.0);
  for(size_t i=0;i<4;i++)
    {
      const std::string strNum(StrFunc::makeString(i));
      PipeGen.generatePipe(Control,"miraclesPipeE"+strNum,yStep,500.0);
      FGen.generateTaper(Control,"miraclesFOutE"+strNum,496.0,
                         gap,gap-1.0,gap,gap-1.0);
      gap-=1.0;
    }

  // HUT:
  
  Control.addVariable("miraclesCaveYStep",25.0);
  Control.addVariable("miraclesCaveXStep",100.0);
  Control.addVariable("miraclesCaveVoidFront",60.0);
  Control.addVariable("miraclesCaveVoidHeight",300.0);
  Control.addVariable("miraclesCaveVoidDepth",183.0);
  Control.addVariable("miraclesCaveVoidWidth",500.0);
  Control.addVariable("miraclesCaveVoidLength",600.0);

  Control.addVariable("miraclesCaveFeFront",25.0);
  Control.addVariable("miraclesCaveFeLeftWall",15.0);
  Control.addVariable("miraclesCaveFeRightWall",15.0);
  Control.addVariable("miraclesCaveFeRoof",15.0);
  Control.addVariable("miraclesCaveFeFloor",15.0);
  Control.addVariable("miraclesCaveFeBack",15.0);

  Control.addVariable("miraclesCaveConcFront",35.0);
  Control.addVariable("miraclesCaveConcLeftWall",35.0);
  Control.addVariable("miraclesCaveConcRightWall",35.0);
  Control.addVariable("miraclesCaveConcRoof",35.0);
  Control.addVariable("miraclesCaveConcFloor",50.0);
  Control.addVariable("miraclesCaveConcBack",35.0);

  Control.addVariable("miraclesCaveFeMat","Stainless304");
  Control.addVariable("miraclesCaveConcMat","Concrete");

  // Beam port through front of cave
  Control.addVariable("miraclesCaveCutShape","Circle");
  Control.addVariable("miraclesCaveCutRadius",10.0);

  // Second vacuum pipe out of bunker [before chopper pit]
  PipeGen.setPipe(6.0,0.5);
  PipeGen.setWindow(-2.0,0.5);
  PipeGen.setFlange(-4.0,1.0);
  PipeGen.generatePipe(Control,"miraclesPipeCave",2.0,250.0);

  return;
}
 
}  // NAMESPACE setVariable
