/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonVar/TwinFlatGenerator.cxx
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
#include "TwinFlatGenerator.h"

namespace setVariable
{

TwinFlatGenerator::TwinFlatGenerator() :
  TwinBaseGenerator(),

  portIW(12.0),portIH(3.0),portOW(14.0),portOH(4.0),
  portNBolt(0),portBoltRadius(0.5)
  /*!
    Constructor : All variables are set for 35cm radius disks
    with an overlap of 5cm. Values are scaled appropiately for
    most changes
  */
{}


TwinFlatGenerator::~TwinFlatGenerator() 
 /*!
   Destructor
 */
{}

void
TwinFlatGenerator::setMainRadius(const double R)
  /*!
    Set the void space radius for the chopper
    \param R :: Radius
  */
{
  TwinBaseGenerator::setMainRadius(R);
  return;
}

void
TwinFlatGenerator::setMaterial(const std::string& wMat,
			   const std::string& )
  /*!
    Set material/port material
    \param wMat :: Main wall material
    \param pMat :: Port material
   */
{
  wallMat=wMat;
  //  viewWindowMat=pMat;
  return;
}
  
void
TwinFlatGenerator::generateChopper(FuncDataBase& Control,
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
  ELog::RegMethod RegA("TwinFlatGenerator","generateChopper");

  TwinBaseGenerator::generateChopper
    (Control,keyName,yStep,length,voidLength);

  Control.addVariable(keyName+"BoxPortFrontNBolts",24); 
  Control.addVariable(keyName+"BoxPortFrontBoltRadius",0.40); 
  Control.addVariable(keyName+"BoxPortFrontInnerWidth",portIW); 
  Control.addVariable(keyName+"BoxPortFrontInnerHeight",portIH);
  Control.addVariable(keyName+"BoxPortFrontOuterWidth",portOW); 
  Control.addVariable(keyName+"BoxPortFrontOuterHeight",portOH);
  Control.addVariable(keyName+"BoxPortFrontAngleOffset",180.0/24.0);
  Control.addVariable(keyName+"BoxPortFrontThickness",2.0); // estimate
  Control.addVariable(keyName+"BoxPortFrontSealThick",0.2);
  Control.addVariable(keyName+"BoxPortFrontMainMat",wallMat);
  Control.addVariable(keyName+"BoxPortFrontBoltMat","ChipIRSteel");  
  Control.addVariable(keyName+"BoxPortFrontSealMat","Poly");

  Control.addVariable(keyName+"BoxPortBackNBolts",24); 
  Control.addVariable(keyName+"BoxPortBackBoltRadius",0.40); 
  Control.addVariable(keyName+"BoxPortBackInnerWidth",portIW); 
  Control.addVariable(keyName+"BoxPortBackInnerHeight",portIH);
  Control.addVariable(keyName+"BoxPortBackOuterWidth",portOW); 
  Control.addVariable(keyName+"BoxPortBackOuterHeight",portOH);
  Control.addVariable(keyName+"BoxPortBackAngleOffset",180.0/24.0);
  Control.addVariable(keyName+"BoxPortBackThickness",2.0); // estimate
  Control.addVariable(keyName+"BoxPortBackSealThick",0.2);
  Control.addVariable(keyName+"BoxPortBackMainMat",wallMat);
  Control.addVariable(keyName+"BoxPortBackBoltMat","ChipIRSteel");  
  Control.addVariable(keyName+"BoxPortBackSealMat","Poly");

  Control.addVariable(keyName+"BoltMat","ChipIRSteel");


  return;
}


  
}  // NAMESPACE setVariable
