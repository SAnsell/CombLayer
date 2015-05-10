/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   pipeBuild/pipeVariables.cxx
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
PipeVariables(FuncDataBase& Control)
  /*!
    Create all the beamline variabes
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("pipeVariables[F]","PipeVariables");
  
  Control.addVariable("TubeObjLength",300.0);
  Control.addVariable("TubeObjWidth",100.0);
  Control.addVariable("TubeObjHeight",100.0);
  Control.addVariable("TubeObjInnerWidth",30.0);
  Control.addVariable("TubeObjInnerHeight",30.0);
  Control.addVariable("TubeObjWallMat","Stainless304");

  Control.addVariable("TubeObjNWallLayers",3);
  Control.addVariable("TubeObjWLayerThick0",3.0);
  Control.addVariable("TubeObjWLayerThick1",4.0);
  Control.addVariable("TubeObjWLayerThick2",5.0);
  Control.addVariable("TubeObjWLayerThick3",5.0);

  Control.addVariable("pointSourceXStep",0.0);
  Control.addVariable("pointSourceYStep",1.0);
  Control.addVariable("pointSourceZStep",0.0);
  Control.addVariable("pointSourceRadius",0.2);
  Control.addVariable("pointSourceParticleType",1);  // neutron
  Control.addVariable("pointSourceASpread",90.0);
  Control.addVariable("pointSourceEnergy",3.0);
  Control.addVariable("pointSourceEProb",1.0);
  

  return;
}

}  // NAMESPACE setVariable
 
