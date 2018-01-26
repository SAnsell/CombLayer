/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonVar/ChopperGenerator.cxx
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
#include "ChopperGenerator.h"

namespace setVariable
{

ChopperGenerator::ChopperGenerator() :
  mainZStep(28.0),height(86.5),width(86.5),
  shortWidth(50.5),shortHeight(50.5),
  mainRadius(38.122),windowThick(0.3),
  ringRadius(40.0),motorRadius(10.0),
  motorInner(12.0),motorOuter(15.20),
  portRadius(10.0),
  portOuter(12.65),portWidth(11.6),
  portHeight(11.6),portBoltStep(1.0),
  wallMat("Aluminium"),portMat("Aluminium"),
  sealMat("Poly"),windowMat("Aluminium")
  /*!
    Constructor BUT ALL variable are left unpopulated.
  */
{}


ChopperGenerator::~ChopperGenerator() 
 /*!
   Destructor
 */
{}

ChopperGenerator::ChopperGenerator(const ChopperGenerator& A) : 
  mainZStep(A.mainZStep),height(A.height),width(A.width),
  shortWidth(A.shortWidth),shortHeight(A.shortHeight),
  mainRadius(A.mainRadius),windowThick(A.windowThick),
  ringRadius(A.ringRadius),motorRadius(A.motorRadius),
  motorInner(A.motorInner),
  motorOuter(A.motorOuter),portRadius(A.portRadius),
  portOuter(A.portOuter),portWidth(A.portWidth),
  portHeight(A.portHeight),portBoltStep(A.portBoltStep),
  wallMat(A.wallMat),portMat(A.portMat),sealMat(A.sealMat),
  windowMat(A.windowMat)
  /*!
    Copy constructor
    \param A :: ChopperGenerator to copy
  */
{}

ChopperGenerator&
ChopperGenerator::operator=(const ChopperGenerator& A)
  /*!
    Assignment operator
    \param A :: ChopperGenerator to copy
    \return *this
  */
{
  if (this!=&A)
    {
      mainZStep=A.mainZStep;
      height=A.height;
      width=A.width;
      shortWidth=A.shortWidth;
      shortHeight=A.shortHeight;
      mainRadius=A.mainRadius;
      windowThick=A.windowThick;
      ringRadius=A.ringRadius;
      motorRadius=A.motorRadius;
      motorInner=A.motorInner;
      motorOuter=A.motorOuter;
      portRadius=A.portRadius;
      portOuter=A.portOuter;
      portWidth=A.portWidth;
      portHeight=A.portHeight;
      portBoltStep=A.portBoltStep;
      wallMat=A.wallMat;
      portMat=A.portMat;
      sealMat=A.sealMat;
      windowMat=A.windowMat;
    }
  return *this;
}

void
ChopperGenerator::setMainRadius(const double R)
  /*!
    Set the void space radius for the chopper
    \param R :: Radius
  */
{
  ringRadius*=R/mainRadius;
  mainZStep*=R/mainRadius;
  motorRadius*=R/mainRadius;
  motorInner*=R/mainRadius;
  motorOuter*=R/mainRadius;
  portRadius*=R/mainRadius;
  portOuter*=R/mainRadius;
  portWidth*=R/mainRadius;
  portHeight*=R/mainRadius;
  portBoltStep*=R/mainRadius;
  mainRadius=R;
  return;
}

void
ChopperGenerator::setPortRadius(const double R)
  /*!
    Set the port radius for the chopper.
    Note that the motor radius should be reduce if the 
    main radius is not being increased
    \param R :: Radius
  */
{

  portOuter*=R/portRadius;
  portBoltStep*=R/portRadius;
  portOuter*=R/portRadius;
  portWidth*=R/portRadius;
  portRadius=R;
  return;
}

void
ChopperGenerator::setMotorRadius(const double R)
  /*!
    Set the motor radius for the chopper.
    Note that the port radius should be reduce if the 
    main radius is not being increased
    \param R :: Radius
  */
{
  motorInner*=R/motorRadius;
  motorOuter*=R/motorRadius;
  motorRadius=R;
  return;
}

void
ChopperGenerator::setReverseMotor(const bool A)
  /*!
    Reverse the motors from -ve Y direction
    \param A :: Reverse Motor 
  */
{
  motorRevFlag=A;
  return;
}
  
void
ChopperGenerator::setFrame(const double H,const double W)
  /*!
    Set the main width/height
    \param H :: height [largest vertical]
    \param W :: width [largest horizontal]
   */
{
  shortHeight*=H/height;
  shortWidth*=W/width;
  height=H;
  width=W;

  return;
}

void
ChopperGenerator::setMaterial(const std::string& wMat,
                              const std::string& pMat)
  /*!
    Set material/port material
    \param wMat :: Main wall material
    \param pMat :: Port material
   */
{
  wallMat=wMat;
  portMat=pMat;
  return;
}
  
void
ChopperGenerator::generateChopper(FuncDataBase& Control,
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
  ELog::RegMethod RegA("ChopperGenerator","generateChopper");

  Control.addVariable(keyName+"YStep",yStep);
  Control.addVariable(keyName+"MainZStep",mainZStep);   // drawing [5962.2]
  Control.addVariable(keyName+"Height",height);
  Control.addVariable(keyName+"Width",width);
  Control.addVariable(keyName+"Length",length);  // drawing [5960.2]
  Control.addVariable(keyName+"ShortHeight",shortHeight);
  Control.addVariable(keyName+"ShortWidth",shortWidth);
  Control.addVariable(keyName+"MainRadius",mainRadius); // estimate
  Control.addVariable(keyName+"MainThick",voidLength);  // estimate

  const double wallThick((length-voidLength)/2.0);
  Control.addVariable(keyName+"MotorBodyLength",5.0);
  Control.addVariable(keyName+"MotorPlateThick",wallThick*1.2);
  Control.addVariable(keyName+"MotorAxleRadius",0.5);
  Control.addVariable(keyName+"MotorBodyRadius",motorRadius);
  Control.addVariable(keyName+"MotorAxleMat","Nickel");
  Control.addVariable(keyName+"MotorBodyMat","Copper");
  Control.addVariable(keyName+"MotorPlateMat",wallMat);    
  Control.addVariable(keyName+"MotorInnerRadius",motorInner); // [5691.2]
  Control.addVariable(keyName+"MotorOuterRadius",motorOuter); // [5691.2]
  Control.addVariable(keyName+"MotorBoltRadius",0.50);       //M10 inc thread
  Control.addVariable(keyName+"MotorMainMat",wallMat);
  Control.addVariable(keyName+"MotorBoltMat","ChipIRSteel");  
  Control.addVariable(keyName+"MotorSealMat","Poly");
  Control.addVariable(keyName+"MotorNBolts",24);
  Control.addVariable(keyName+"MotorSealRadius",(motorInner+motorOuter)/2.0);
  Control.addVariable(keyName+"MotorSealThick",0.2);  
  Control.addVariable(keyName+"MotorSealMat","Poly");
  Control.addVariable(keyName+"MotorReverse",static_cast<int>(motorRevFlag));
  
  Control.addVariable(keyName+"FrontFlangeNBolts",24); 
  Control.addVariable(keyName+"FrontFlangeBoltRadius",0.40); 
  Control.addVariable(keyName+"FrontFlangeInnerRadius",portRadius); // [5691.2]
  Control.addVariable(keyName+"FrontFlangeOuterRadius",portOuter); // [5691.2]
  Control.addVariable(keyName+"FrontFlangeAngleOffset",180.0/24.0);
  Control.addVariable(keyName+"FrontFlangeThickness",2.0); // estimate
  Control.addVariable(keyName+"FrontFlangeSealThick",0.2);
  Control.addVariable(keyName+"FrontFlangeMainMat",wallMat);
  Control.addVariable(keyName+"FrontFlangeBoltMat","ChipIRSteel");  
  Control.addVariable(keyName+"FrontFlangeSealMat","Poly");

  Control.addVariable(keyName+"BackFlangeNBolts",24); 
  Control.addVariable(keyName+"BackFlangeBoltRadius",0.40); 
  Control.addVariable(keyName+"BackFlangeInnerRadius",portRadius); // [5691.2]
  Control.addVariable(keyName+"BackFlangeOuterRadius",portOuter); // [5691.2]
  Control.addVariable(keyName+"BackFlangeAngleOffset",180.0/24.0);
  Control.addVariable(keyName+"BackFlangeThickness",2.0); // estimate
  Control.addVariable(keyName+"BackFlangeSealThick",0.2);
  Control.addVariable(keyName+"BackFlangeMainMat",wallMat);
  Control.addVariable(keyName+"BackFlangeBoltMat","ChipIRSteel");  
  Control.addVariable(keyName+"BackFlangeSealMat","Poly");

  Control.addVariable(keyName+"RingNSection",12);
  Control.addVariable(keyName+"RingNTrack",12);
  Control.addVariable(keyName+"RingThick",0.4);
  Control.addVariable(keyName+"RingRadius",ringRadius);  
  Control.addVariable(keyName+"RingMat",sealMat); 

  // strange /4 because it is average of 1/2 lengths
  const std::string kItem=
    "-("+keyName+"Length+"+keyName+"MainThick)/4.0";
  Control.addParse<double>(keyName+"IPortAYStep",kItem);
 
  Control.addVariable(keyName+"IPortAWidth",portWidth);  
  Control.addVariable(keyName+"IPortAHeight",portHeight);
  Control.addVariable(keyName+"IPortALength",1.0);
  Control.addVariable(keyName+"IPortAMat",portMat);
  Control.addVariable(keyName+"IPortASealStep",0.5);
  Control.addVariable(keyName+"IPortASealThick",0.3); 
  Control.addVariable(keyName+"IPortASealMat",sealMat);
  Control.addVariable(keyName+"IPortAWindow",windowThick);
  Control.addVariable(keyName+"IPortAWindowMat",windowMat);

  Control.addVariable(keyName+"IPortANBolt",8);
  Control.addVariable(keyName+"IPortABoltStep",portBoltStep);
  Control.addVariable(keyName+"IPortABoltRadius",0.3);
  Control.addVariable(keyName+"IPortABoltMat","ChipIRSteel");
  
  // PORT B
  Control.addParse<double>(keyName+"IPortBYStep",
        "("+keyName+"Length+"+keyName+"MainThick)/4.0");
  Control.addVariable(keyName+"IPortBWidth",portWidth);  
  Control.addVariable(keyName+"IPortBHeight",portHeight);
  Control.addVariable(keyName+"IPortBLength",1.0);
  Control.addVariable(keyName+"IPortBMat",portMat);
  Control.addVariable(keyName+"IPortBSealStep",0.5);
  Control.addVariable(keyName+"IPortBSealThick",0.3); 
  Control.addVariable(keyName+"IPortBSealMat","Poly");
  Control.addVariable(keyName+"IPortBWindow",windowThick);
  Control.addVariable(keyName+"IPortBWindowMat",windowMat);

  Control.addVariable(keyName+"IPortBNBolt",8);
  Control.addVariable(keyName+"IPortBBoltStep",portBoltStep);
  Control.addVariable(keyName+"IPortBBoltRadius",0.3);
  Control.addVariable(keyName+"IPortBBoltMat","ChipIRSteel");
    
  Control.addVariable(keyName+"BoltMat","ChipIRSteel");
  Control.addVariable(keyName+"WallMat",wallMat);
  Control.addVariable(keyName+"VoidMat","Void");
  return;
}
  
}  // NAMESPACE setVariable
