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

  Control.addVariable("testTwinAStepHeight",87.4); 
  Control.addVariable("testTwinALength",30.0);
  Control.addVariable("testTwinAMainRadius",41.0);
  Control.addVariable("testTwinAInnerRadius",36.0);
  Control.addVariable("testTwinAInnerTopStep",25.0);
  Control.addVariable("testTwinAInnerLowStep",25.0);
  Control.addVariable("testTwinAInnerVoid",18.0);

  Control.addVariable("testTwinAPortRadius",10.0); // [5691.2]
  Control.addVariable("testTwinAPortOuter",12.65); // [5691.2]
  Control.addVariable("testTwinAPortStep",0.0); // estimate
  Control.addVariable("testTwinAPortNBolt",24); 
  Control.addVariable("testTwinAPortBoltRadius",0.40); //M8 inc
  Control.addVariable("testTwinAPortBoltAngOff",180.0/24.0);
  Control.addVariable("testTwinAPortSealThick",0.2);
  Control.addVariable("testTwinAPortSealMat","Poly");

  Control.addVariable("testTwinABoltMat","ChipIRSteel");
  //  const std::string kItem=
  //    "-("+keyName+"Length+"+keyName+"MainThick)/4.0";
  //  Control.addParse<double>(keyName+"IPortAYStep",kItem);
  Control.addVariable<double>("testTwinAIPortAYStep",-12.0);
  
  Control.addVariable("testTwinAIPortAWidth",11.6);  
  Control.addVariable("testTwinAIPortAHeight",11.6);
  Control.addVariable("testTwinAIPortALength",1.0);
  Control.addVariable("testTwinAIPortAMat","Aluminium");
  Control.addVariable("testTwinAIPortASealStep",0.5);
  Control.addVariable("testTwinAIPortASealThick",0.3); 
  Control.addVariable("testTwinAIPortASealMat","Poly");
  Control.addVariable("testTwinAIPortAWindow",0.3);
  Control.addVariable("testTwinAIPortAWindowMat","SiCrystal");

  Control.addVariable("testTwinAIPortANBolt",8);
  Control.addVariable("testTwinAIPortABoltStep",1.0);
  Control.addVariable("testTwinAIPortABoltRadius",0.3);
  Control.addVariable("testTwinAIPortABoltMat","ChipIRSteel");
  


  //  const std::string kItem=
  //    "-("+keyName+"Length+"+keyName+"MainThick)/4.0";
  //  Control.addParse<double>(keyName+"IPortBYStep",kItem);
  Control.addVariable<double>("testTwinAIPortBYStep",12.0);
  
  Control.addVariable("testTwinAIPortBWidth",11.6);  
  Control.addVariable("testTwinAIPortBHeight",11.6);
  Control.addVariable("testTwinAIPortBLength",1.0);
  Control.addVariable("testTwinAIPortBMat","Aluminium");
  Control.addVariable("testTwinAIPortBSealStep",0.5);
  Control.addVariable("testTwinAIPortBSealThick",0.3); 
  Control.addVariable("testTwinAIPortBSealMat","Poly");
  Control.addVariable("testTwinAIPortBWindow",0.3);
  Control.addVariable("testTwinAIPortBWindowMat","SiCrystal");

  Control.addVariable("testTwinAIPortBNBolt",8);
  Control.addVariable("testTwinAIPortBBoltStep",1.0);
  Control.addVariable("testTwinAIPortBBoltRadius",0.3);
  Control.addVariable("testTwinAIPortBBoltMat","ChipIRSteel");
  
  Control.addVariable("testTwinAMotorARadius",8.0); 
  Control.addVariable("testTwinAMotorAOuter",10.20); 
  Control.addVariable("testTwinAMotorAStep",0.0); // estimate
  Control.addVariable("testTwinAMotorANBolt",24); 
  Control.addVariable("testTwinAMotorABoltRadius",0.50); //M10 inc thread
  Control.addVariable("testTwinAMotorASealThick",0.2);  
  Control.addVariable("testTwinAMotorASealMat","Poly");

  Control.addVariable("testTwinAMotorBRadius",8.0); 
  Control.addVariable("testTwinAMotorBOuter",10.20); 
  Control.addVariable("testTwinAMotorBStep",0.0); // estimate
  Control.addVariable("testTwinAMotorBNBolt",24); 
  Control.addVariable("testTwinAMotorBBoltRadius",0.50); //M10 inc thread
  Control.addVariable("testTwinAMotorBSealThick",0.2);  
  Control.addVariable("testTwinAMotorBSealMat","Poly");

  Control.addVariable("testTwinAWallMat","Aluminium");
  
  
  return;
}
 
}  // NAMESPACE setVariable
