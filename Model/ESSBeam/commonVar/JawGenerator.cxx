/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonVar/JawGenerator.cxx
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
#include "JawGenerator.h"

namespace setVariable
{

JawGenerator::JawGenerator()
/*!
    Constructor and defaults
  */
{}


JawGenerator::~JawGenerator() 
 /*!
   Destructor
 */
{}

void
JawGenerator::generateJaws(FuncDataBase& Control,const std::string& keyName,
                            const double yStep) const
/*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param yStep :: y-offset 
  */
{
  ELog::RegMethod RegA("JawGenerator","generatorJaws");

  Control.addVariable(keyName+"YStep",yStep);
  Control.addVariable(keyName+"Length",25.0);
  Control.addVariable(keyName+"Radius",30.0);
  //
  // JAWS
  //
  // Jaw X
  Control.addVariable(keyName+"VertXStep",0.0);
  Control.addVariable(keyName+"VertYStep",-5.0);
  Control.addVariable(keyName+"VertZStep",0.0);
  Control.addVariable(keyName+"VertXYangle",180.0);
  Control.addVariable(keyName+"VertZangle",0.0);

  Control.addVariable(keyName+"VertZOpen",5.5);
  Control.addVariable(keyName+"VertZThick",2.0);
  Control.addVariable(keyName+"VertZCross",15.0);
  Control.addVariable(keyName+"VertZLen",8.0);

  Control.addVariable(keyName+"VertGap",0.5);  
  
  Control.addVariable(keyName+"VertXOpen",5.5);
  Control.addVariable(keyName+"VertXThick",2.0);
  Control.addVariable(keyName+"VertXCross",15.0);
  Control.addVariable(keyName+"VertXLen",8.0);  

  Control.addVariable(keyName+"VertXHeight",28.0);
  Control.addVariable(keyName+"VertYHeight",9.0);
  Control.addVariable(keyName+"VertZHeight",28.0);
  Control.addVariable(keyName+"VertWallThick",2.0);

  Control.addVariable(keyName+"VertxJawMat","Tungsten");
  Control.addVariable(keyName+"VertzJawMat","Tungsten");
  Control.addVariable(keyName+"VertWallMat","Aluminium");

  // Jaw XZ
  Control.addVariable(keyName+"DiagXStep",0.0);
  Control.addVariable(keyName+"DiagYStep",5.0);
  Control.addVariable(keyName+"DiagZStep",0.0);
  Control.addVariable(keyName+"DiagXAngle",0.0);
  Control.addVariable(keyName+"DiagYAngle",45.0);
  Control.addVariable(keyName+"DiagZAngle",0.0);

  Control.addVariable(keyName+"DiagZOpen",5.5);
  Control.addVariable(keyName+"DiagZThick",2.0);
  Control.addVariable(keyName+"DiagZCross",15.0);
  Control.addVariable(keyName+"DiagZLen",8.0);

  Control.addVariable(keyName+"DiagGap",0.5);  
  
  Control.addVariable(keyName+"DiagXOpen",5.5);
  Control.addVariable(keyName+"DiagXThick",2.0);
  Control.addVariable(keyName+"DiagXCross",15.0);
  Control.addVariable(keyName+"DiagXLen",8.0);  

  Control.addVariable(keyName+"DiagXHeight",28.0);
  Control.addVariable(keyName+"DiagYHeight",9.0);
  Control.addVariable(keyName+"DiagZHeight",28.0);
  Control.addVariable(keyName+"DiagWallThick",2.0);


  Control.addVariable(keyName+"DiagxJawMat","Tungsten");
  Control.addVariable(keyName+"DiagzJawMat","Tungsten");
  Control.addVariable(keyName+"DiagWallMat","Aluminium");
  
  return;

}

}  // NAMESPACE setVariable
