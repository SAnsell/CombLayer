/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:    ESSBeam/testBeam/TESTBEAMvariables.cxx
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
#include "TwinGenerator.h"
#include "PitGenerator.h"
#include "PipeGenerator.h"
#include "JawGenerator.h"
#include "BladeGenerator.h"

namespace setVariable
{
 
  
void
TESTBEAMvariables(FuncDataBase& Control)
  /*!
    Create all the beamline variables for vor
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("TESTBEAMvariables[F]","TESTBEAMvariables");

  setVariable::TwinGenerator TGen;
  setVariable::ChopperGenerator CGen;
  setVariable::FocusGenerator FGen;
  setVariable::ShieldGenerator SGen;
  setVariable::PitGenerator PGen;
  setVariable::PipeGenerator PipeGen;
  setVariable::BladeGenerator BGen;
  setVariable::JawGenerator JawGen;

  Control.addVariable("testBeamStartPoint",0);
  Control.addVariable("testBeamStopPoint",0);
  
  PipeGen.setPipe(8.0,0.5);
  PipeGen.setWindow(-2.0,0.5);
  PipeGen.setFlange(-4.0,1.0);

  SGen.addWall(1,20.0,"CastIron");
  SGen.addRoof(1,20.0,"CastIron");
  SGen.addFloor(1,20.0,"CastIron");
  SGen.addFloorMat(5,"Concrete");
  SGen.addRoofMat(5,"Concrete"); 
  SGen.addWallMat(5,"Concrete");

  FGen.setGuideMat("Copper");
  FGen.setYOffset(8.0);
  FGen.generateTaper(Control,"testBeamFA",350.0,7.6,4.02,5.0,7.6250);

  TGen.generateChopper(Control,"testTwinA",0.0,30.0,18.0);  

  // Single Blade chopper
  BGen.setThick({0.2});
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.generateBlades(Control,"testBladeA",-2.0,22.5,33.5);

  // Single Blade chopper
  BGen.setThick({0.2});
  BGen.addPhase({95,275},{30.0,30.0});
  BGen.generateBlades(Control,"testBladeB",2.0,22.5,33.5);


  Control.addVariable("testCryoASampleRadius",0.75);
  Control.addVariable("testCryoASampleHeight",2.0);
  
  Control.addVariable("testCryoAVoidRadius",5.0);
  Control.addVariable("testCryoAVoidDepth",12.0);
  Control.addVariable("testCryoAVoidHeight",6.0);
  Control.addVariable("testCryoAVoidWallThick",0.5);

  Control.addVariable("testCryoAStickLen",105.0);
  Control.addVariable("testCryoAStickRadius",0.6);

  Control.addVariable("testCryoAStickBoreRadius",1.8);
  Control.addVariable("testCryoAStickBoreHeight",107.0-12.0);
  Control.addVariable("testCryoAStickBoreThick",0.5);

  Control.addVariable("testCryoAHeatRadius",8.0);
  Control.addVariable("testCryoAHeatHeight",11.0);
  Control.addVariable("testCryoAHeatDepth",16.0);
  Control.addVariable("testCryoAHeatThick",0.5);

  Control.addVariable("testCryoATailRadius",11.0);
  Control.addVariable("testCryoATailHeight",7.0);
  Control.addVariable("testCryoATailDepth",18.5);
  Control.addVariable("testCryoATailThick",0.5);

  Control.addVariable("testCryoAHeatOuterRadius",13.5);
  Control.addVariable("testCryoAHeatOuterLift",15.0);

  Control.addVariable("testCryoALiqN2InnerRadius",15.6);
  Control.addVariable("testCryoALiqN2OuterRadius",21.2);
  Control.addVariable("testCryoALiqN2WallThick",0.5);
  Control.addVariable("testCryoALiqN2Height",53.0);
  
  Control.addVariable("testCryoALiqHeInnerRadius",5.7);
  Control.addVariable("testCryoALiqHeOuterRadius",12.85);
  Control.addVariable("testCryoALiqHeWallThick",0.5);
  Control.addVariable("testCryoALiqHeHeight",51.0);
  Control.addVariable("testCryoALiqHeExtraLift",2.0);

  Control.addVariable("testCryoATailOuterRadius",22.80);
  Control.addVariable("testCryoATailOuterHeight",89.0);
  Control.addVariable("testCryoATailOuterLift",12.0);

  Control.addVariable("testCryoASampleMat","H2O");
  Control.addVariable("testCryoAWallMat","Aluminium");
  Control.addVariable("testCryoAStickMat","Stainless304");
  Control.addVariable("testCryoALiqN2Mat","LiqN2");
  Control.addVariable("testCryoALiqHeMat","LiqHelium");



  return;
}
 
}  // NAMESPACE setVariable
