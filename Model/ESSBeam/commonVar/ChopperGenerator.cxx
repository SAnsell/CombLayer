/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonVar/ChopperGenerator.cxx
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
#include "ChopperGenerator.h"

namespace essSystem
{

ChopperGenerator::ChopperGenerator() :
  mainZStep(28.0),height(86.5),width(86.5),
  shortWidth(50.5),mainRadius(38.122),
  windowThick(0.3),
  wallMat("Aluminium"),
  sealMat("Poly"),windowMat("Aluminium")
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

ChopperGenerator::~ChopperGenerator() 
 /*!
   Destructor
 */
{}


void
ChopperGenerator::generateChopper(FuncDataBase& Control,
                                  const std::string& keyName,
                                  const double yStep,
                                  const double length,
                                  const double voidLength)
  /*!
    Generate the chopper variables
    \param Control :: Functional data base
    \param yStep :: main y-step
    \param length :: total length
    \param voidLength :: inner space length
   */
{
  ELog::RegMethod RegA("ChopperGenerator","generateChopper");
  
  Control.addVariable(keyName+"YStep",yStep);
  Control.addVariable(keyName+"MainZStep",mainZStep);   // drawing [5962.2]
  Control.addVariable(keyName+"Height",width);
  Control.addVariable(keyName+"Width",height);
  Control.addVariable(keyName+"Length",length);  // drawing [5960.2]
  Control.addVariable(keyName+"ShortWidth",shortWidth);
  Control.addVariable(keyName+"MainRadius",mainRadius); // estimate
  Control.addVariable(keyName+"MainThick",voidLength);  // estimate
  
  Control.addVariable(keyName+"MotorRadius",12.00); // [5691.2]
  Control.addVariable(keyName+"MotorOuter",15.20); // [5691.2]
  Control.addVariable(keyName+"MotorStep",0.0); // estimate
  Control.addVariable(keyName+"MotorNBolt",24); 
  Control.addVariable(keyName+"MotorBoltRadius",0.50); //M10 inc thread
  Control.addVariable(keyName+"MotorSealThick",0.2);  
  Control.addVariable(keyName+"MortorSealMat","Poly");
  
  Control.addVariable(keyName+"PortRadius",10.0); // [5691.2]
  Control.addVariable(keyName+"PortOuter",12.65); // [5691.2]
  Control.addVariable(keyName+"PortStep",0.0); // estimate
  Control.addVariable(keyName+"PortNBolt",24); 
  Control.addVariable(keyName+"PortBoltRadius",0.40); //M8 inc
  Control.addVariable(keyName+"PortBoltAngOff",180.0/24.0);
  Control.addVariable(keyName+"PortSealThick",0.2);
  Control.addVariable(keyName+"PortSealMat","Poly");

  Control.addVariable(keyName+"RingNSection",12);
  Control.addVariable(keyName+"RingNTrack",12);
  Control.addVariable(keyName+"RingThick",0.4);
  Control.addVariable(keyName+"RingRadius",40.0);  
  Control.addVariable(keyName+"RingMat",sealMat); 

  // strange /4 because it is average of 1/2 lengths
  const std::string kItem=
    "-("+keyName+"Length+"+keyName+"MainThick)/4.0";
  Control.addParse<double>(keyName+"IPortAYStep",kItem);
 
  Control.addVariable(keyName+"IPortAWidth",11.6);  
  Control.addVariable(keyName+"IPortAHeight",11.6);
  Control.addVariable(keyName+"IPortALength",1.0);
  Control.addVariable(keyName+"IPortAMat","Aluminium");
  Control.addVariable(keyName+"IPortASealStep",0.5);
  Control.addVariable(keyName+"IPortASealThick",0.3); 
  Control.addVariable(keyName+"IPortASealMat",sealMat);
  Control.addVariable(keyName+"IPortAWindow",windowThick);
  Control.addVariable(keyName+"IPortAWindowMat",windowMat);

  Control.addVariable(keyName+"IPortANBolt",8);
  Control.addVariable(keyName+"IPortABoltStep",1.0);
  Control.addVariable(keyName+"IPortABoltRadius",0.3);
  Control.addVariable(keyName+"IPortABoltMat","Stainless304");
  
  // PORT B
  Control.addParse<double>(keyName+"IPortBYStep",
        "("+keyName+"Length+"+keyName+"MainThick)/4.0");
  Control.addVariable(keyName+"IPortBWidth",12.0);  
  Control.addVariable(keyName+"IPortBHeight",12.0);
  Control.addVariable(keyName+"IPortBLength",1.0);
  Control.addVariable(keyName+"IPortBMat","Aluminium");
  Control.addVariable(keyName+"IPortBSealStep",0.5);
  Control.addVariable(keyName+"IPortBSealThick",0.3); 
  Control.addVariable(keyName+"IPortBSealMat","Poly");
  Control.addVariable(keyName+"IPortBWindow",windowThick);
  Control.addVariable(keyName+"IPortBWindowMat",windowMat);

  Control.addVariable(keyName+"IPortBNBolt",8);
  Control.addVariable(keyName+"IPortBBoltStep",1.0);
  Control.addVariable(keyName+"IPortBBoltRadius",0.3);
  Control.addVariable(keyName+"IPortBBoltMat","Stainless304");
    
  Control.addVariable(keyName+"BoltMat","Stainless304");
  Control.addVariable(keyName+"WallMat",wallMat);
  Control.addVariable(keyName+"VoidMat","Void");
  return;
}


  
}  // NAMESPACE essSystem
