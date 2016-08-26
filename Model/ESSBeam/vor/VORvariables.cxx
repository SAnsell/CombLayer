/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:    ESSBeam/vor/VORvariables.cxx
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
#include "BladeGenerator.h"
#include "essVariables.h"

namespace setVariable
{

void
VORvariables(FuncDataBase& Control)
  /*!
    Create all the beamline variables for vor
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("VORvariables[F]","VORvariables");

  setVariable::ChopperGenerator CGen;
  setVariable::FocusGenerator FGen;
  setVariable::ShieldGenerator SGen;
  setVariable::PitGenerator PGen;
  setVariable::PipeGenerator PipeGen;
  setVariable::BladeGenerator BGen;

  SGen.addWall(1,15.0,"CastIron");
  SGen.addRoof(1,15.0,"CastIron");
  SGen.addFloor(1,15.0,"CastIron");
  SGen.addFloorMat(5,"Concrete");
  SGen.addRoofMat(5,"Concrete");
  SGen.addWallMat(5,"Concrete");

  FGen.setGuideMat("Copper");
  FGen.setYOffset(0.1);

  PipeGen.setPipe(12.0,0.5);
  PipeGen.setWindow(-2.0,0.3);
  PipeGen.setFlange(-4.0,1.0);

    
  Control.addVariable("vorStopPoint",0);
  Control.addVariable("vorAxisXYAngle",0.0);   // rotation
  Control.addVariable("vorAxisZAngle",2.0);   // rotation 
  
  //  Control.addVariable("vorGABeamXYAngle",1.0);
  FGen.setGuideMat("Copper");
  FGen.setYOffset(0.1);
  FGen.generateTaper(Control,"vorFA",350.0,2.114,3.2417,3.16,3.9228);

  // VACUUM PIPE in Gamma shield
  PipeGen.generatePipe(Control,"vorPipeB",2.0,46.0);

  FGen.setGuideMat("Aluminium");
  FGen.clearYOffset();
  FGen.generateTaper(Control,"vorFB",44.0,3.30,3.4028,4.0,4.2);


  // VACUUM PIPE in Gamma shield
  PipeGen.generatePipe(Control,"vorPipeC",2.0,326.0);
  FGen.generateTaper(Control,"vorFC",322.0,3.4028,3.87,4.2,5.906);

  CGen.setMainRadius(26.0);
  CGen.setFrame(60.0,60.0);
  CGen.generateChopper(Control,"vorChopperA",8.0,10.0,4.55);    

  // Double Blade chopper
  BGen.setMaterials("Inconnel","Aluminium");
  BGen.setThick({0.3,0.3});
  BGen.setGap(1.0);
  BGen.addPhase({-15,165},{30.0,30.0});
  BGen.addPhase({-15,165},{30.0,30.0});
  BGen.generateBlades(Control,"vorDBlade",0.0,10.0,22.50);

  // VACUUM PIPE in Gamma shield
  PipeGen.generatePipe(Control,"vorPipeD",2.0,208.0);
  FGen.generateTaper(Control,"vorFD",204.0,3.4028,3.87,4.2,5.906);


  // BEAM INSERT:
  Control.addVariable("vorBInsertHeight",20.0);
  Control.addVariable("vorBInsertWidth",28.0);
  Control.addVariable("vorBInsertTopWall",1.0);
  Control.addVariable("vorBInsertLowWall",1.0);
  Control.addVariable("vorBInsertLeftWall",1.0);
  Control.addVariable("vorBInsertRightWall",1.0);
  Control.addVariable("vorBInsertWallMat","Stainless304");       

  // VACUUM PIPE: in bunker wall
  PipeGen.setPipe(6.0,0.5);
  PipeGen.setWindow(-2.0,0.5);
  PipeGen.setFlange(-4.0,1.0);
  PipeGen.generatePipe(Control,"vorPipeWall",1.0,318.0);

  // Guide in wall
  FGen.clearYOffset();
  FGen.generateTaper(Control,"vorFWall",316.0,6.0,6.0,6.0,6.0);

  // Shield/Pipe/Guide after bunker wall
  SGen.setRFLayers(3,8);
  SGen.generateShield(Control,"vorShieldA",1430.0,40.0,40.0,40.0,3,8);

  PipeGen.generatePipe(Control,"vorPipeOutA",2.0,208.0);
  FGen.generateTaper(Control,"vorFOutA",204.0,3.4028,3.87,4.2,5.906);

  return;
}
 
}  // NAMESPACE setVariable
