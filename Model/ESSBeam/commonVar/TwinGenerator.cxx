/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonVar/TwinGenerator.cxx
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
#include <stack>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <numeric>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "TwinGenerator.h"

namespace setVariable
{

TwinGenerator::TwinGenerator() :
  stepHeight(87.4),mainRadius(38.122),
  innerRadius(36.0),innerTopStep(25.0),
  innerLowStep(25.0),
  
  portRadius(10.0),portOuter(12.65),
  portNBolt(24),portBoltRadius(0.4),

  viewWidth(11.6),viewHeight(11.6),
  viewLength(1.0),viewWindowThick(0.3),
  viewBoltStep(1.0),viewNBolt(8),
  viewBoltRadius(0.3),
  viewWindowMat("SiCrystal"),viewMat("Aluminium"),
  viewBoltMat("ChipIRSteel"),
  
  motorRadius(8.0),motorOuter(10.20),
  motorNBolt(24),motorBoltRadius(0.5),
  motorSealThick(0.2),motorSealMat("Poly"),
  wallMat("Aluminium")
  /*!
    Constructor : All variables are set for 35cm radius disks
    with an overlap of 5cm. Values are scaled appropiately for
    most changes
  */
{}


TwinGenerator::~TwinGenerator() 
 /*!
   Destructor
 */
{}

void
TwinGenerator::setMainRadius(const double R)
  /*!
    Set the void space radius for the chopper
    \param R :: Radius
  */
{
  stepHeight*=R/mainRadius;
  motorRadius*=R/mainRadius;
  motorOuter*=R/mainRadius;
  portRadius*=R/mainRadius;
  portOuter*=R/mainRadius;
  viewWidth*=R/mainRadius;
  viewHeight*=R/mainRadius;
  viewBoltStep*=R/mainRadius;
  mainRadius=R;
  return;
}

void
TwinGenerator::setMaterial(const std::string& wMat,
			   const std::string& pMat)
  /*!
    Set material/port material
    \param wMat :: Main wall material
    \param pMat :: Port material
   */
{
  wallMat=wMat;
  viewWindowMat=pMat;
  return;
}
  
void
TwinGenerator::generateChopper(FuncDataBase& Control,
                               const std::string& keyName,
                               const double yStep,
                               const double length,
                               const double voidLength)
  /*!
    Generate the chopper variables
    \param Control :: Functional data base
    \param keyName :: Base name for chopper variables
    \param yStep :: main y-step
    \param length :: total length
    \param voidLength :: inner space length
   */
{
  ELog::RegMethod RegA("TwinGenerator","generateChopper");

  Control.addVariable(keyName+"YStep",yStep);

  Control.addVariable("testTwinAStepHeight",stepHeight); 
  Control.addVariable("testTwinALength",length);       
  Control.addVariable("testTwinAMainRadius",mainRadius);
  Control.addVariable("testTwinAInnerRadius",innerRadius);
  Control.addVariable("testTwinAInnerTopStep",innerTopStep);
  Control.addVariable("testTwinAInnerLowStep",innerLowStep);
  Control.addVariable("testTwinAInnerVoid",voidLength);    

  Control.addVariable("testTwinAPortRadius",portRadius); 
  Control.addVariable("testTwinAPortOuter",portOuter); // [5691.2]
  Control.addVariable("testTwinAPortStep",0.0); // estimate
  Control.addVariable("testTwinAPortNBolt",portNBolt); 
  Control.addVariable("testTwinAPortBoltRadius",portBoltRadius); //M8 inc
  Control.addVariable("testTwinAPortBoltAngOff",180.0/portNBolt);
  Control.addVariable("testTwinAPortSealThick",0.2);
  Control.addVariable("testTwinAPortSealMat","Poly");

  Control.addVariable("testTwinABoltMat","ChipIRSteel");

  const std::string kItem=
    "-("+keyName+"Length+"+keyName+"MainThick)/4.0";
  Control.addParse<double>(keyName+"IPortAYStep",kItem);
  
  Control.addVariable("testTwinAIPortAWidth",viewWidth);  
  Control.addVariable("testTwinAIPortAHeight",viewHeight);
  Control.addVariable("testTwinAIPortALength",viewLength);
  Control.addVariable("testTwinAIPortAMat",viewMat);
  Control.addVariable("testTwinAIPortASealStep",0.5);
  Control.addVariable("testTwinAIPortASealThick",0.3); 
  Control.addVariable("testTwinAIPortASealMat","Poly");
  Control.addVariable("testTwinAIPortAWindow",viewWindowThick);
  Control.addVariable("testTwinAIPortAWindowMat",viewWindowMat);

  Control.addVariable("testTwinAIPortANBolt",viewNBolt);
  Control.addVariable("testTwinAIPortABoltStep",viewBoltStep);
  Control.addVariable("testTwinAIPortABoltRadius",viewBoltRadius);
  Control.addVariable("testTwinAIPortABoltMat",viewBoltMat);
  
  const std::string lItem=
    "("+keyName+"Length+"+keyName+"MainThick)/4.0";
  Control.addParse<double>(keyName+"IPortBYStep",kItem);

  Control.addVariable("testTwinAIPortBWidth",viewWidth);  
  Control.addVariable("testTwinAIPortBHeight",viewHeight);
  Control.addVariable("testTwinAIPortBLength",viewLength);
  Control.addVariable("testTwinAIPortBMat",viewMat);
  Control.addVariable("testTwinAIPortBSealStep",0.5);
  Control.addVariable("testTwinAIPortBSealThick",0.3); 
  Control.addVariable("testTwinAIPortBSealMat","Poly");
  Control.addVariable("testTwinAIPortBWindow",viewWindowThick);
  Control.addVariable("testTwinAIPortBWindowMat",viewWindowMat);

  Control.addVariable("testTwinAIPortBNBolt",viewNBolt);
  Control.addVariable("testTwinAIPortBBoltStep",viewBoltStep);
  Control.addVariable("testTwinAIPortBBoltRadius",viewBoltRadius);
  Control.addVariable("testTwinAIPortBBoltMat",viewBoltMat);


  // MOTOR
  
  Control.addVariable("testTwinAMotorAFlag",2);
  Control.addVariable("testTwinAMotorARadius",motorRadius); 
  Control.addVariable("testTwinAMotorAOuter",motorOuter); 
  Control.addVariable("testTwinAMotorAStep",0.0); // estimate
  Control.addVariable("testTwinAMotorANBolt",motorNBolt); 
  Control.addVariable("testTwinAMotorABoltRadius",motorBoltRadius); //M10 inc thread
  Control.addVariable("testTwinAMotorASealThick",motorSealThick);  
  Control.addVariable("testTwinAMotorASealMat",motorSealMat);

  Control.addVariable("testTwinAMotorBFlag",2);
  Control.addVariable("testTwinAMotorBRadius",motorRadius); 
  Control.addVariable("testTwinAMotorBOuter",motorOuter); 
  Control.addVariable("testTwinAMotorBStep",0.0); // estimate
  Control.addVariable("testTwinAMotorBNBolt",motorNBolt); 
  Control.addVariable("testTwinAMotorBBoltRadius",motorBoltRadius); //M10 inc thread
  Control.addVariable("testTwinAMotorBSealThick",motorSealThick);  
  Control.addVariable("testTwinAMotorBSealMat",motorSealMat);

  Control.addVariable("testTwinAWallMat","Aluminium");

  return;
}


  
}  // NAMESPACE setVariable
