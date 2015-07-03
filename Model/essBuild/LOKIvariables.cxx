/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/LOKIvariables.cxx
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
LOKIvariables(FuncDataBase& Control)
  /*!
    Create all the beamline variables for loki
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("LOKIvariables[F]","LOKIvariables");

  Control.addVariable("lokiBAXStep",0.0);       
  Control.addVariable("lokiBAYStep",0.0);       
  Control.addVariable("lokiBAZStep",0.0);       
  Control.addVariable("lokiBAXYAngle",0.0);       
  Control.addVariable("lokiBAZAngle",0.0);       
  Control.addVariable("lokiBALength",220.0);       
  Control.addVariable("lokiBAHeight",50.0);       
  Control.addVariable("lokiBADepth",50.0);       
  Control.addVariable("lokiBALeftWidth",20.0);       
  Control.addVariable("lokiBARightWidth",20.0);       
  Control.addVariable("lokiBAFeMat","Void");       
  Control.addVariable("lokiBANShapes",1);       
  Control.addVariable("lokiBANShapeLayers",3);

  Control.addVariable("lokiBALayerThick1",0.4);  // glass thick
  Control.addVariable("lokiBALayerThick2",1.5);

  Control.addVariable("lokiBALayerMat0","Void");
  Control.addVariable("lokiBALayerMat1","Glass");
  Control.addVariable("lokiBALayerMat2","Void");       

  Control.addVariable("lokiBA0TypeID","Tapper");
  Control.addVariable("lokiBA0HeightStart",3.0);
  Control.addVariable("lokiBA0HeightEnd",3.0);
  Control.addVariable("lokiBA0WidthStart",7.0);
  Control.addVariable("lokiBA0WidthEnd",10.0);
  Control.addVariable("lokiBA0Length",220.0);
  Control.addVariable("lokiBA0ZAngle",0.0);


  return;
}

}  // NAMESPACE setVariable
