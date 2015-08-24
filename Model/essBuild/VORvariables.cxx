/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/VORvariables.cxx
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
VORvariables(FuncDataBase& Control)
  /*!
    Create all the beamline variables for vor
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("VORvariables[F]","VORvariables");

  // Bender in section so use cut system
  Control.addVariable("G1BLine2Filled",1);
  Control.addVariable("G1BLine3Filled",1);
  
  Control.addVariable("vorFAXStep",0.0);       
  Control.addVariable("vorFAYStep",0.0);       
  Control.addVariable("vorFAZStep",0.0);       
  Control.addVariable("vorFAXYAngle",0.0);
  Control.addVariable("vorFAZAngle",0.0);
  Control.addVariable("vorFABeamXYAngle",0.0);       

  Control.addVariable("vorFALength",400.0);       
  Control.addVariable("vorFANShapes",1);       
  Control.addVariable("vorFANShapeLayers",3);
  Control.addVariable("vorFAActiveShield",0);

  Control.addVariable("vorFALayerThick1",0.4);  // glass thick
  Control.addVariable("vorFALayerThick2",1.5);

  Control.addVariable("vorFALayerMat0","Void");
  Control.addVariable("vorFALayerMat1","Glass");
  Control.addVariable("vorFALayerMat2","Void");       

  Control.addVariable("vorFA0TypeID","Tapper");
  Control.addVariable("vorFA0HeightStart",12.0);
  Control.addVariable("vorFA0HeightEnd",20.0);
  Control.addVariable("vorFA0WidthStart",12.0);
  Control.addVariable("vorFA0WidthEnd",20.0);
  Control.addVariable("vorFA0Length",800.0);

 
  return;
}

}  // NAMESPACE setVariable
