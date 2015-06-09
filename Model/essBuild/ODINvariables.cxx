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
  Control.addVariable("ODINCollAXYangle",180.0);
  Control.addVariable("ODINCollAZangle",180.0);

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

  Control.addVariable("ODINgAXStep",0.0);       
  Control.addVariable("ODINgAYStep",0.0);       
  Control.addVariable("ODINgAZStep",0.0);       
  Control.addVariable("ODINgAXYAngle",0.0);       
  Control.addVariable("ODINgAZAngle",0.0);       
  Control.addVariable("ODINgALength",220.0);       
  Control.addVariable("ODINgAHeight",100.0);       
  Control.addVariable("ODINgADepth",100.0);       
  Control.addVariable("ODINgALeftWidth",20.0);       
  Control.addVariable("ODINgARightWidth",20.0);       
  Control.addVariable("ODINgAFeMat","Void");       
  Control.addVariable("ODINgANShapes",1);       
  Control.addVariable("ODINgANShapeLayers",3);

  Control.addVariable("ODINgALayerThick1",0.4);  // glass thick
  Control.addVariable("ODINgALayerThick2",1.5);

  Control.addVariable("ODINgALayerMat0","Void");
  Control.addVariable("ODINgALayerMat1","Glass");
  Control.addVariable("ODINgALayerMat2","Void");       

  Control.addVariable("ODINgA0TypeID","Tapper");
  Control.addVariable("ODINgA0HeightStart",3.0);
  Control.addVariable("ODINgA0HeightEnd",3.0);
  Control.addVariable("ODINgA0WidthStart",7.0);
  Control.addVariable("ODINgA0WidthEnd",10.0);
  Control.addVariable("ODINgA0Length",220.0);
  Control.addVariable("ODINgA0ZAngle",0.0);       


  // T0 Chopper 

  Control.addVariable("odinTZeroXStep",0.0);
  Control.addVariable("odinTZeroYStep",2.0);
  Control.addVariable("odinTZeroZStep",15.0/2.0);
  Control.addVariable("odinTZeroXYangle",0.0);
  Control.addVariable("odinTZeroZangle",0.0);

  Control.addVariable("odinTZeroGap",3.0);
  Control.addVariable("odinTZeroInnerRadius",10.0);
  Control.addVariable("odinTZeroOuterRadius",25.0);
  Control.addVariable("odinTZeroNDisk",1);

  Control.addVariable("odinTZero0Thick",40.0);
  Control.addVariable("odinTZeroInnerMat","Aluminium");
  Control.addVariable("odinTZeroOuterMat","Inconnel");

  Control.addVariable("odinTZero0PhaseAngle",95.0);
  Control.addVariable("odinTZero0OpenAngle",30.0);
  
  

  
  return;
}

}  // NAMESPACE setVariable
