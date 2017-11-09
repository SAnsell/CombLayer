/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonVar/TwinGenerator.cxx
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
#include "TwinGenerator.h"

namespace setVariable
{

TwinGenerator::TwinGenerator() :
  TwinBaseGenerator(),
  
  portRadius(10.0),portOuter(12.65),
  portNBolt(24),portBoltRadius(0.4),

  viewWidth(11.6),viewHeight(11.6),
  viewLength(1.0),viewWindowThick(0.3),
  viewBoltStep(1.0),viewNBolt(8),
  viewBoltRadius(0.3),
  viewWindowMat("SiCrystal"),viewMat("Aluminium"),
  viewBoltMat("ChipIRSteel")
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
  portRadius*=R/mainRadius;
  portOuter*=R/mainRadius;
  viewWidth*=R/mainRadius;
  viewHeight*=R/mainRadius;
  viewBoltStep*=R/mainRadius;
  TwinBaseGenerator::setMainRadius(R);
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

  TwinBaseGenerator::generateChopper
    (Control,keyName,yStep,length,voidLength);
  

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

  Control.addVariable(keyName+"BoltMat","ChipIRSteel");
  
  Control.addVariable(keyName+"PortRadius",portRadius); 
  Control.addVariable(keyName+"PortOuter",portOuter); // [5691.2]
  Control.addVariable(keyName+"PortStep",0.0); // estimate
  Control.addVariable(keyName+"PortNBolt",portNBolt); 
  Control.addVariable(keyName+"PortBoltRadius",portBoltRadius); //M8 inc
  Control.addVariable(keyName+"PortBoltAngOff",180.0/portNBolt);
  Control.addVariable(keyName+"PortSealThick",0.2);
  Control.addVariable(keyName+"PortSealMat","Poly");


  const std::string kItem=
    "-("+keyName+"Length+"+keyName+"InnerVoid)/4.0";
  Control.addParse<double>(keyName+"IPortAYStep",kItem);
  
  Control.addVariable(keyName+"IPortAWidth",viewWidth);  
  Control.addVariable(keyName+"IPortAHeight",viewHeight);
  Control.addVariable(keyName+"IPortALength",viewLength);
  Control.addVariable(keyName+"IPortAMat",viewMat);
  Control.addVariable(keyName+"IPortASealStep",0.5);
  Control.addVariable(keyName+"IPortASealThick",0.3); 
  Control.addVariable(keyName+"IPortASealMat","Poly");
  Control.addVariable(keyName+"IPortAWindow",viewWindowThick);
  Control.addVariable(keyName+"IPortAWindowMat",viewWindowMat);

  Control.addVariable(keyName+"IPortANBolt",viewNBolt);
  Control.addVariable(keyName+"IPortABoltStep",viewBoltStep);
  Control.addVariable(keyName+"IPortABoltRadius",viewBoltRadius);
  Control.addVariable(keyName+"IPortABoltMat",viewBoltMat);
  
  const std::string lItem=
    "("+keyName+"Length+"+keyName+"InnerVoid)/4.0";
  Control.addParse<double>(keyName+"IPortBYStep",lItem);

  Control.addVariable(keyName+"IPortBWidth",viewWidth);  
  Control.addVariable(keyName+"IPortBHeight",viewHeight);
  Control.addVariable(keyName+"IPortBLength",viewLength);
  Control.addVariable(keyName+"IPortBMat",viewMat);
  Control.addVariable(keyName+"IPortBSealStep",0.5);
  Control.addVariable(keyName+"IPortBSealThick",0.3); 
  Control.addVariable(keyName+"IPortBSealMat","Poly");
  Control.addVariable(keyName+"IPortBWindow",viewWindowThick);
  Control.addVariable(keyName+"IPortBWindowMat",viewWindowMat);

  Control.addVariable(keyName+"IPortBNBolt",viewNBolt);
  Control.addVariable(keyName+"IPortBBoltStep",viewBoltStep);
  Control.addVariable(keyName+"IPortBBoltRadius",viewBoltRadius);
  Control.addVariable(keyName+"IPortBBoltMat",viewBoltMat);

  return;
}


  
}  // NAMESPACE setVariable
