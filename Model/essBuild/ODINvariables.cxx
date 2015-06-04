/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/ODINvariables.cxx
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#include "variableSetup.h"

namespace setVariable
{

void
ODINvariables(FuncDataBase& Control)
  /*!
    Create all the beamline variables
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("ODINvariables[F]","ODINvariables");

  Control.addVariable("ODINCollAXStep",0.0);
  Control.addVariable("ODINCollAYStep",50.0);
  Control.addVariable("ODINCollAZStep",0.0);
  Control.addVariable("ODINCollAXYangle",0.0);
  Control.addVariable("ODINCollAZangle",0.0);

  Control.addVariable("ODINCollAZOpen",1.5);
  Control.addVariable("ODINCollAZThick",4.0);
  Control.addVariable("ODINCollAZCross",15.0);
  Control.addVariable("ODINCollAZLen",8.0);

  Control.addVariable("ODINCollAGap",0.5);  
  
  Control.addVariable("ODINCollAXOpen",1.5);
  Control.addVariable("ODINCollAXThick",4.0);
  Control.addVariable("ODINCollAXCross",15.0);
  Control.addVariable("ODINCollAXLen",8.0);  

  Control.addVariable("ODINCollAXHeight",18.0);
  Control.addVariable("ODINCollAYHeight",9.0);
  Control.addVariable("ODINCollAZHeight",18.0);
  Control.addVariable("ODINCollAWallThick",2.0);

  Control.addVariable("ODINCollAzJawMat","Aluminium");
  Control.addVariable("ODINCollAxJawMat","Aluminium");
  Control.addVariable("ODINCollAWallMat","Aluminium");

  Control.addVariable("ODINCollAzJawMat","Aluminium");

  Control.addVariable("ODINg1XStep",0.0);       
  Control.addVariable("ODINg1YStep",0.0);       
  Control.addVariable("ODINg1ZStep",0.0);       
  Control.addVariable("ODINg1XYAngle",0.0);       
  Control.addVariable("ODINg1ZAngle",0.0);       
  Control.addVariable("ODINg1Length",3190.0);       
  Control.addVariable("ODINg1Height",100.0);       
  Control.addVariable("ODINg1Depth",100.0);       
  Control.addVariable("ODINg1LeftWidth",30.0);       
  Control.addVariable("ODINg1RightWidth",30.0);       
  Control.addVariable("ODINg1FeMat","Stainless304");       
  Control.addVariable("ODINg1NShapes",2);       
  Control.addVariable("ODINg1NShapeLayers",3);       

  return;
}

}  // NAMESPACE setVariable
