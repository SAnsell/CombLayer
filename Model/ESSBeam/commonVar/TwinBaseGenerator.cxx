/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonVar/TwinBaseGenerator.cxx
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
#include "TwinBaseGenerator.h"

namespace setVariable
{

TwinBaseGenerator::TwinBaseGenerator() :
  stepHeight(87.4),mainRadius(39.122),
  innerRadius(36.0),innerTopStep(25.0),
  innerLowStep(25.0),
  
  motorRadius(10.0),
  motorFlangeInner(10.20),motorFlangeOuter(12.20),
  motorOuter((motorFlangeInner+motorFlangeOuter)/2.0),
  motorLength(32.0),motorNBolt(24),
  motorBoltRadius(0.5),motorSealThick(0.2),
  motorRevFlagA(0),motorRevFlagB(0),
  motorSealMat("Poly"),

  ringNBolt(12),lineNBolt(8),outerStep(0.5),
  outerBoltRadius(0.8),outerBoltMat("ChipIRSteel"),
  wallMat("Aluminium")
  /*!
    Constructor : All variables are set for 35cm radius disks
    with an overlap of 5cm. Values are scaled appropiately for
    most changes
  */
{}


TwinBaseGenerator::~TwinBaseGenerator() 
 /*!
   Destructor
 */
{}

void
TwinBaseGenerator::setMainRadius(const double R)
  /*!
    Set the void space radius for the chopper
    \param R :: Radius
  */
{
  stepHeight*=R/mainRadius;
  motorFlangeInner*=R/mainRadius;
  motorFlangeOuter*=R/mainRadius;
  motorRadius*=R/mainRadius;
  motorLength*=R/mainRadius;
  mainRadius=R;
  return;
}

void
TwinBaseGenerator::setReverseMotors(const bool A,const bool B)
  /*!
    Reverse the motors from -ve Y direction
    \param A :: Reverse Motor A
    \param B :: Reverse Motor B
  */
{
  motorRevFlagA=A;
  motorRevFlagB=B;
  return;
}

void
TwinBaseGenerator::setMotorLength(const double L)
  /*!
    Set motor length
    \param L :: Motor length
   */
{
  motorLength=L;
  return;
}
  
void
TwinBaseGenerator::setWallMaterial(const std::string& wMat)
  /*!
    Set material/port material
    \param wMat :: Main wall material
   */
{
  wallMat=wMat;
  return;
}
  
void
TwinBaseGenerator::generateChopper(FuncDataBase& Control,
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
  ELog::RegMethod RegA("TwinBaseGenerator","generateChopper");


  Control.addVariable(keyName+"YStep",yStep);

  Control.addVariable(keyName+"StepHeight",stepHeight); 
  Control.addVariable(keyName+"Length",length);       
  Control.addVariable(keyName+"MainRadius",mainRadius);
  Control.addVariable(keyName+"InnerRadius",innerRadius);
  Control.addVariable(keyName+"InnerTopStep",innerTopStep);
  Control.addVariable(keyName+"InnerLowStep",innerLowStep);
  Control.addVariable(keyName+"InnerVoid",voidLength);    


  // MOTOR
  const double wallThick((length-voidLength)/2.0);
  for(const std::string itemName : {"MotorA","MotorB"})
    {
      Control.addVariable(keyName+itemName+"BodyLength",motorLength);
      Control.addVariable(keyName+itemName+"PlateThick",wallThick*1.2);
      Control.addVariable(keyName+itemName+"AxleRadius",2.0);
      Control.addVariable(keyName+itemName+"BodyRadius",motorRadius);
      Control.addVariable(keyName+itemName+"AxleMat","Nickel");
      Control.addVariable(keyName+itemName+"BodyMat","Copper");
      Control.addVariable(keyName+itemName+"PlateMat",wallMat);    
      Control.addVariable(keyName+itemName+"InnerRadius",motorFlangeInner); // [5691.2]
      Control.addVariable(keyName+itemName+"OuterRadius",motorFlangeOuter); // [5691.2]
      Control.addVariable(keyName+itemName+"BoltRadius",0.50);       //M10 inc thread
      Control.addVariable(keyName+itemName+"MainMat",wallMat);
      Control.addVariable(keyName+itemName+"BoltMat","ChipIRSteel");  
      Control.addVariable(keyName+itemName+"SealMat","Poly");
      Control.addVariable(keyName+itemName+"NBolts",24);
      Control.addVariable(keyName+itemName+"SealRadius",
			  (motorRadius+motorOuter)/2.0);
      Control.addVariable(keyName+itemName+"SealThick",0.2);  
      Control.addVariable(keyName+itemName+"SealMat",motorSealMat);
    }
  Control.addVariable(keyName+"MotorAReverse",static_cast<int>(motorRevFlagA));
  Control.addVariable(keyName+"MotorBReverse",static_cast<int>(motorRevFlagB));

  Control.addVariable(keyName+"OuterRingNBolt",ringNBolt);
  Control.addVariable(keyName+"OuterLineNBolt",lineNBolt);
  Control.addVariable(keyName+"OuterBoltStep",outerStep);
  Control.addVariable(keyName+"OuterBoltRadius",outerBoltRadius); 
  Control.addVariable(keyName+"OuterBoltMat",outerBoltMat);

  Control.addVariable(keyName+"WallMat","Aluminium");

  return;
}


  
}  // NAMESPACE setVariable
