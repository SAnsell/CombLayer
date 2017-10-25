/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:    ESSBeam/nnbar/NNBARvariables.cxx
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
#include "LayerGenerator.h"
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
NNBARvariables(FuncDataBase& Control)
  /*!
    Create all the beamline variables for vor
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("NNBARvariables[F]","NNBARvariables");

  setVariable::ChopperGenerator CGen;
  setVariable::CryoGenerator CryGen;
  setVariable::FocusGenerator FGen;
  setVariable::LayerGenerator LGen;
  setVariable::PitGenerator PGen;
  setVariable::PipeGenerator PipeGen;
  setVariable::BladeGenerator BGen;
  setVariable::JawGenerator JawGen;

  Control.addVariable("nnbarStartPoint",0);
  Control.addVariable("nnbarStopPoint",0);
  
  LGen.setWall(100.0,{35.0,65.0}, {4,4}, {"CastIron","Concrete"});
  LGen.setRoof(100.0,{35.0,65.0}, {4,4}, {"CastIron","Concrete"});
  LGen.setFloor(100.0,{35.0,65.0}, {4,4}, {"CastIron","Concrete"});

  FGen.setLayer(1,0.5,"Copper");
  FGen.setLayer(2,0.5,"Void");
  FGen.setYOffset(2.0);
  FGen.generateTaper(Control,"nnbarFA",350.0, 30.0,50.0, 30.0,50.0);

  PipeGen.setPipe(35.0,1.0);
  PipeGen.setWindow(-4.0,1.0);
  PipeGen.setFlange(-8.0,4.0);
  PipeGen.generatePipe(Control,"nnbarPipeB",8.0,46.0);

  FGen.setLayer(1,0.5,"Aluminium");
  FGen.clearYOffset();
  FGen.generateTaper(Control,"nnbarFB",38.0, 50.0,55.0, 20.0,25.0);

  // Section from 6m to 10m
  PipeGen.setPipe(35.0,1.0);
  PipeGen.generatePipe(Control,"nnbarPipeC",4.0,400.0);

  FGen.clearYOffset();
  FGen.generateTaper(Control,"nnbarFC",392.0, 55.0,57.5, 25.0,27.5);

  // Section from 10m to 30m
  PipeGen.setPipe(35.0,1.0);
  PipeGen.generatePipe(Control,"nnbarPipeD",4.0,2000.0);

  FGen.clearYOffset();
  FGen.generateRectangle(Control,"nnbarFD",1992.0,57.5,27.5);

  // BEAM INSERT:
  Control.addVariable("nnbarBInsertHeight",90.0);
  Control.addVariable("nnbarBInsertWidth",90.0);
  Control.addVariable("nnbarBInsertTopWall",1.0);
  Control.addVariable("nnbarBInsertLowWall",1.0);
  Control.addVariable("nnbarBInsertLeftWall",1.0);
  Control.addVariable("nnbarBInsertRightWall",1.0);
  Control.addVariable("nnbarBInsertWallMat","Stainless304");       


  LGen.generateLayer(Control,"nnbarShieldA",3500.0,1);
  // Section from 30m to 50m
  PipeGen.generatePipe(Control,"nnbarPipeOutA",4.0,2000.0);

  FGen.setYOffset(12.0);
  FGen.generateRectangle(Control,"nnbarFOutA",1996.0,57.5,27.5);

  // Extra Section from 30m [Drift Tube]
  PipeGen.generatePipe(Control,"nnbarPipeOutA",8.0,3000.0);

    // HUT:  
  Control.addVariable("nnbarCaveYStep",25.0);
  Control.addVariable("nnbarCaveXStep",0.0);
  Control.addVariable("nnbarCaveVoidFront",60.0);
  Control.addVariable("nnbarCaveVoidHeight",270.0);
  Control.addVariable("nnbarCaveVoidDepth",225.0);
  Control.addVariable("nnbarCaveVoidWidth",480.0);  // max 5.8 full
  Control.addVariable("nnbarCaveVoidLength",960.0);

  Control.addVariable("nnbarCaveFeFront",25.0);
  Control.addVariable("nnbarCaveFeLeftWall",15.0);
  Control.addVariable("nnbarCaveFeRightWall",15.0);
  Control.addVariable("nnbarCaveFeRoof",15.0);
  Control.addVariable("nnbarCaveFeFloor",15.0);
  Control.addVariable("nnbarCaveFeBack",15.0);

  Control.addVariable("nnbarCaveConcFront",35.0);
  Control.addVariable("nnbarCaveConcLeftWall",35.0);
  Control.addVariable("nnbarCaveConcRightWall",35.0);
  Control.addVariable("nnbarCaveConcRoof",35.0);
  Control.addVariable("nnbarCaveConcFloor",50.0);
  Control.addVariable("nnbarCaveConcBack",35.0);

  Control.addVariable("nnbarCaveFeMat","Stainless304");
  Control.addVariable("nnbarCaveConcMat","Concrete");

  // Beam port through front of cave
  Control.addVariable("nnbarCaveCutShape","Circle");
  Control.addVariable("nnbarCaveCutRadius",10.0);

  // Beam port through front of cave
  Control.addVariable("nnbarCaveCutShape","Circle");
  Control.addVariable("nnbarCaveCutRadius",100.0);

  Control.addVariable("G1BLineTop11Width1",50.0);
  Control.addVariable("G1BLineTop11Width2",65.0);

  Control.addVariable("G1BLineTop11Depth1",40.0);
  Control.addVariable("G1BLineTop11Depth2",45.0);
  
  Control.addVariable("G1BLineTop11Height1",40.0);
  Control.addVariable("G1BLineTop11Height2",44.0);
  Control.addVariable("GuideBayInnerHeight",60.0);
  Control.addVariable("GuideBayHeight",70.0);

  return;
}
 
}  // NAMESPACE setVariable
 
