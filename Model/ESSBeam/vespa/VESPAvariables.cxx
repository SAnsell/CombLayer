/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:    ESSBeam/vespa/VESPAvariables.cxx
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
#include "JawGenerator.h"
#include "BladeGenerator.h"
#include "CryoGenerator.h"

namespace setVariable
{

 
  
void
VESPAvariables(FuncDataBase& Control)
  /*!
    Create all the beamline variables for vor
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("VESPAvariables[F]","VESPAvariables");

  setVariable::ChopperGenerator CGen;
  setVariable::CryoGenerator CryGen;
  setVariable::FocusGenerator FGen;
  setVariable::ShieldGenerator SGen;
  setVariable::PitGenerator PGen;
  setVariable::PipeGenerator PipeGen;
  setVariable::BladeGenerator BGen;
  setVariable::JawGenerator JawGen;

  Control.addVariable("vespaStartPoint",0);
  Control.addVariable("vespaStopPoint",0);

  Control.addVariable("vespaAxisZStep",0.0);
  
  PipeGen.setPipe(8.0,0.5);      // 8cm radius / 0.5cm wall
  PipeGen.setWindow(-2.0,0.5); 
  PipeGen.setFlange(-4.0,1.0);

  SGen.addWall(1,20.0,"CastIron");
  SGen.addRoof(1,20.0,"CastIron");
  SGen.addFloor(1,20.0,"CastIron");
  SGen.addFloorMat(4,"Concrete");
  SGen.addRoofMat(4,"Concrete");
  SGen.addWallMat(4,"Concrete");
  //  SGen.setAngle(10.0,10.0);
  
  FGen.setLayer(1,0.5,"Copper");
  FGen.setLayer(2,0.5,"Void");  
  FGen.setYOffset(2.0);
  FGen.generateTaper(Control,"vespaFA",350.0,3.5,4.0, 3.5,4.0);

  PipeGen.generatePipe(Control,"vespaPipeA",6.0,46.0);

  FGen.setLayer(1,0.5,"Aluminium");
  FGen.clearYOffset();
  FGen.generateRectangle(Control,"vespaFB",44.0,4.0,4.0);   
  
  // VACBOX A : 6.10m target centre
  //  Length 100.7 + Width [87.0] + Height [39.0] void Depth/2 + front
  CGen.setMainRadius(38.0);   // diameter 70.0 emali
  CGen.setFrame(85.0,85.0);
  CGen.generateChopper(Control,"vespaChopperA",8.0,12.0,5.55);

  // Double Blade chopper
  BGen.setThick({0.2,0.2});
  BGen.addPhase({0.0,90.0,220.0},{24.31,40.98,57.32});
  BGen.addPhase({30.0,120.0,250.0},{24.31,40.98,57.32});
  BGen.setGap(3.0);
  BGen.generateBlades(Control,"vespaWFMBladeA",0.0,20.0,35.5);

  // Pipe between choppers:
  PipeGen.generatePipe(Control,"vespaPipeC",2.0,20.0);
  FGen.generateTaper(Control,"vespaFC",16.0,3.7,4.0,8.0,8.0);   
  //  Control.addVariable("vespaFBBeamYStep",4.0);


  // VACBOX A : 6.10m target centre
  //  Length 100.7 + Width [87.0] + Height [39.0] void Depth/2 + front
  CGen.generateChopper(Control,"vespaChopperB",9.0,12.0,5.55);
  
  // Double Blade chopper
  BGen.setThick({0.2,0.2});
  BGen.addPhase({0.0,90.0,220.0},{24.31,40.98,57.32});
  BGen.addPhase({30.0,120.0,250.0},{24.31,40.98,57.32});
  BGen.setGap(3.0);
  BGen.generateBlades(Control,"vespaWFMBladeB",0.0,20.0,35.5);

  PipeGen.generatePipe(Control,"vespaPipeD",2.0,60.0);

  FGen.clearYOffset();
  FGen.generateRectangle(Control,"vespaFD",56.0,5.0,8.0);   
  //  Control.addVariable("vespaFBBeamYStep",4.0);

  // VACBOX A : 6.10m target centre
  //  Length 100.7 + Width [87.0] + Height [39.0] void Depth/2 + front
  CGen.generateChopper(Control,"vespaChopperC",9.0,12.0,5.55);
  
  // Double Blade chopper
  BGen.setThick({0.2,0.2});
  BGen.addPhase({0.0,90.0,220.0},{24.31,40.98,57.32});
  BGen.addPhase({30.0,120.0,250.0},{24.31,40.98,57.32});
  BGen.setGap(3.0);
  BGen.generateBlades(Control,"vespaWFMBladeC",0.0,20.0,35.5);

  PipeGen.generatePipe(Control,"vespaPipeE",2.0,270.0);
  FGen.generateTaper(Control,"vespaFE",266.0, 4.0,7.5, 8.0,7.5);   
  //  Control.addVariable("vespaFBBeamYStep",4.0);

  // VACBOX
  CGen.generateChopper(Control,"vespaChopperD",9.0,12.0,5.55);

  // Double Blade chopper
  BGen.setThick({0.2,0.2});
  BGen.addPhase({0.0},{320.0});
  BGen.addPhase({30.0},{320.0});
  BGen.generateBlades(Control,"vespaFOCBladeA",0.0,20.0,35.5);

  PipeGen.generatePipe(Control,"vespaPipeF",2.0,135.0);
  FGen.generateTaper(Control,"vespaFF",130.0,8.0,7.5,9.0,8.5);  // NOT CORRECT

  //  Control.addVariable("vespaFBBeamYStep",4.0);
  
  // BEAM INSERT:
  Control.addVariable("vespaBInsertHeight",20.0);
  Control.addVariable("vespaBInsertWidth",28.0);
  Control.addVariable("vespaBInsertTopWall",1.0);
  Control.addVariable("vespaBInsertLowWall",1.0);
  Control.addVariable("vespaBInsertLeftWall",1.0);
  Control.addVariable("vespaBInsertRightWall",1.0);
  Control.addVariable("vespaBInsertWallMat","Stainless304");       

  PGen.setFeLayer(15.0);
  PGen.setConcLayer(40.0);
  PGen.generatePit(Control,"vespaOutPitT0",0.0,25.0,220.0,210.0,40.0);

  CGen.setMainRadius(33.0);
  CGen.setFrame(80.0,80.0);
  CGen.generateChopper(Control,"vespaChopperT0",20.0,15.0,9.55);

  // T0 Chopper disk B
  BGen.setThick({3.4});
  BGen.setInnerThick({5.4});
  BGen.setMaterials("Inconnel","Tungsten");
  //  BGen.addPhase({95,275},{30.0,30.0});
  BGen.addPhase({-15,165},{30.0,30.0});   // chopper open
  BGen.generateBlades(Control,"vespaT0Disk",2.0,20.0,30.0);

  // Optional pipe in wall
  PipeGen.generatePipe(Control,"vespaPipeWall",4.0,348.0);
  // Guide in wall

  FGen.clearYOffset();
  FGen.generateTaper(Control,"vespaFWall",344.0,9.0,9.0,8.5,8.5);

  PGen.setFeLayer(15.0);
  PGen.setConcLayer(40.0);
  PGen.generatePit(Control,"vespaOutPitA",440.0,25.0,130.0,110.0,40.0);

  SGen.generateTriShield(Control,"vespaShieldA",350.0,100.0,100.0,100.0,4,9);

  PipeGen.setPipe(9.0,0.5);  // R/T
  PipeGen.generatePipe(Control,"vespaPipeOutA",55.0,395.0);  

  FGen.clearYOffset();
  FGen.generateTaper(Control,"vespaFOutA",385.0,9.0,11.0,8.5,10.0);

  Control.addVariable("vespaT0ExitPortShape","Circle");
  Control.addVariable("vespaT0ExitPortRadius",5.0);

  CGen.setMainRadius(38.0);
  CGen.setFrame(110.0,110.0);
  CGen.generateChopper(Control,"vespaChopperOutA",22.0,12.0,5.55);

  // Double Blade chopper
  BGen.setThick({0.2,0.2});
  BGen.setGap(3.0);
  BGen.addPhase({0.0},{320.0});
  BGen.addPhase({30.0},{320.0});
  BGen.generateBlades(Control,"vespaFOCBladeB",0.0,25.0,35.5);

  // Guide after wall [17.5m - 3.20] for wall
  PipeGen.generatePipe(Control,"vespaPipeOutB",2.0,760.0);  //
  Control.addVariable("vespaPipeOutBRadius",9.0);
  FGen.setYOffset(2.0);
  FGen.generateTaper(Control,"vespaFOutB",750.0,9.0,11.0,8.5,10.0);

  SGen.clearLayers();
  SGen.addWall(1,20.0,"CastIron");
  SGen.addRoof(1,20.0,"CastIron");
  SGen.addFloor(1,20.0,"CastIron");
  SGen.addFloorMat(3,"Concrete");
  SGen.addRoofMat(3,"Concrete");
  SGen.addWallMat(3,"Concrete");
  SGen.generateShield(Control,"vespaShieldB",770.0,80.0,80.0,80.0,4,8);

  const double slen(595.0);
  for(size_t i=0;i<4;i++)
    {
      const std::string
        shieldName("vespaShieldArray"+StrFunc::makeString(i));
      const std::string
        vacName("vespaVPipeArray"+StrFunc::makeString(i));
      const std::string
        focusName("vespaFocusArray"+StrFunc::makeString(i));

      SGen.generateShield(Control,shieldName,slen,40.0,40.0,40.0,4,8);
      PipeGen.generatePipe(Control,vacName,2.0,slen-2.0);  //
      FGen.clearYOffset();
      FGen.generateRectangle(Control,focusName,slen-6.0,8.0,8.0);
    }
  
  PGen.setFeLayer(15.0);
  PGen.setConcLayer(40.0);
  PGen.generatePit(Control,"vespaOutPitB",3143.0,25.0,130.0,90.0,40.0);
  
  Control.addVariable("vespaPitBPortAShape","Circle");
  Control.addVariable("vespaPitBPortARadius",5.0);
  Control.addVariable("vespaPitBPortBShape","Circle");
  Control.addVariable("vespaPitBPortBRadius",5.0);

  // VACBOX for FOC out B
  CGen.generateChopper(Control,"vespaChopperOutB",29.0,12.0,5.55);
  // Double Blade chopper
  BGen.setThick({0.2,0.2});
  BGen.addPhase({0.0},{320.0});
  BGen.addPhase({30.0},{320.0});
  BGen.generateBlades(Control,"vespaFOCBladeOutB",0.0,25.0,35.5);

  SGen.generateShield(Control,"vespaShieldC",200.0,40.0,40.0,40.0,4,8);  

  PipeGen.generatePipe(Control,"vespaPipeOutC",26.0,225.0);  //
  Control.addVariable("vespaPipeOutCRadius",9.0);
  FGen.clearYOffset();
  FGen.generateTaper(Control,"vespaFOutC",220.0,9.0,11.0,8.5,10.0);

  // HUT:  
  Control.addVariable("vespaCaveYStep",25.0);
  Control.addVariable("vespaCaveXStep",0.0);
  Control.addVariable("vespaCaveVoidFront",60.0);
  Control.addVariable("vespaCaveVoidHeight",270.0);
  Control.addVariable("vespaCaveVoidDepth",225.0);
  Control.addVariable("vespaCaveVoidWidth",480.0);  // max 5.8 full
  Control.addVariable("vespaCaveVoidLength",960.0);

  Control.addVariable("vespaCaveFeFront",25.0);
  Control.addVariable("vespaCaveFeLeftWall",15.0);
  Control.addVariable("vespaCaveFeRightWall",15.0);
  Control.addVariable("vespaCaveFeRoof",15.0);
  Control.addVariable("vespaCaveFeFloor",15.0);
  Control.addVariable("vespaCaveFeBack",15.0);

  Control.addVariable("vespaCaveConcFront",35.0);
  Control.addVariable("vespaCaveConcLeftWall",35.0);
  Control.addVariable("vespaCaveConcRightWall",35.0);
  Control.addVariable("vespaCaveConcRoof",35.0);
  Control.addVariable("vespaCaveConcFloor",50.0);
  Control.addVariable("vespaCaveConcBack",35.0);

  Control.addVariable("vespaCaveFeMat","Stainless304");
  Control.addVariable("vespaCaveConcMat","Concrete");

  // Beam port through front of cave
  Control.addVariable("vespaCaveCutShape","Circle");
  Control.addVariable("vespaCaveCutRadius",10.0);

  JawGen.generateJaws(Control,"vespaVJaws",75.0);

  Control.addVariable("vespaSampleYStep",0.0);
  Control.addVariable("vespaSampleNLayers",2);
  Control.addVariable("vespaSampleRadius1",1.0);
  Control.addVariable("vespaSampleRadius2",1.5);
  Control.addVariable("vespaSampleHeight1",5.0);
  Control.addVariable("vespaSampleHeight2",6.0);
  Control.addVariable("vespaSampleMaterial1","H2O");
  Control.addVariable("vespaSampleMaterial2","Aluminium");
  Control.addVariable("vespaSampleTemp2",20.0);
  
  // VESPA DETECTORS
  Control.addVariable("vespaNDet",32);
  
  Control.addVariable("vespaXStalWidth",12.0); // tappered to 20cm
  Control.addVariable("vespaXStalThick",1.0);
  Control.addVariable("vespaXStalLength",10.0);
  Control.addVariable("vespaXStalGap",0.3);
  Control.addVariable("vespaXStalWallThick",0.5);
  Control.addVariable("vespaXStalBaseThick",0.5);
                                         
  Control.addVariable("vespaXStalXtalMat","Silicon300K");
  Control.addVariable("vespaXStalWallMat","Aluminium");

  Control.addVariable("vespaDBoxNDetectors",20);
  Control.addVariable("vespaDBoxCentRadius",0.635);
  Control.addVariable("vespaDBoxTubeRadius",0.425);
  Control.addVariable("vespaDBoxWallThick",0.1);
  Control.addVariable("vespaDBoxHeight",20.0);
  Control.addVariable("vespaDBoxWallMat","Aluminium");
  Control.addVariable("vespaDBoxDetMat","He3_10Bar");

  Control.addVariable("vespaXStal6Active",0);
  Control.addVariable("vespaXStal22Active",0);

  Control.addVariable("vespaDBox6Active",0);
  Control.addVariable("vespaDBox22Active",0);

  
  const double braggAngle[]={-50,-30,-50,-30};
  const double braggStep[]={32.63,64.157,32.63,64.157};
  const double detStep[]={24.5,30.0,24.5,30.0};
  int detCnt(0);
  for(size_t i=0;i<4;i++)   // 4 rings
    {
      double aZ= 0.0;
      const double aZStep= 45.0;
      for(size_t j=0;j<8;j++)
        {
          const std::string xKey="vespaXStal"+StrFunc::makeString(detCnt);
          const std::string dKey="vespaDBox"+StrFunc::makeString(detCnt);
          Control.addVariable(xKey+"YStep",braggStep[i]);
          Control.addVariable(xKey+"PreXYAngle",braggAngle[i]);
          Control.addVariable(xKey+"PreZAngle",0.0);
          Control.addVariable(xKey+"XYAngle",-braggAngle[i]);
          Control.addVariable(xKey+"ZAngle",0.0);
          Control.addVariable(dKey+"YStep",detStep[i]);
          Control.addVariable(dKey+"XYAngle",braggAngle[i]);
          Control.addVariable(xKey+"YRotation",aZ);
          Control.addVariable(xKey+"ZRotation",(i>1 ? 180.0 : 0.0));
          aZ+=aZStep;
          detCnt++;
        }
    }
  // CRYOSTAT
  CryGen.generateFridge(Control,"vespaCryo",300.0,-10,4.5);
  return;
}
 
}  // NAMESPACE setVariable
