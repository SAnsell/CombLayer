/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   pipeBuild/pipeVariables.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
  
  Control.addVariable("ATubeLength",350.0);
  Control.addVariable("ATubeWidth",500.0);
  Control.addVariable("ATubeHeight",500.0);
  Control.addVariable("ATubeInnerWidth",10.0);
  Control.addVariable("ATubeInnerHeight",10.0);
  Control.addVariable("ATubeWallMat","Stainless304");

  Control.addVariable("ATubeNWallLayers",0);
  Control.addVariable("ATubeWLayerThick0",3.0);
  Control.addVariable("ATubeWLayerThick1",4.0);
  Control.addVariable("ATubeWLayerThick2",5.0);
  Control.addVariable("ATubeWLayerThick3",5.0);

  Control.addVariable("BTubeLength",350.0);
  Control.addVariable("BTubeWidth",500.0);
  Control.addVariable("BTubeHeight",500.0);
  Control.addVariable("BTubeInnerWidth",10.0);
  Control.addVariable("BTubeInnerHeight",10.0);
  Control.addVariable("BTubeWallMat","H2O");

  Control.addVariable("BTubeNWallLayers",3);
  Control.addVariable("BTubeWLayerThick0",3.0);
  Control.addVariable("BTubeWLayerThick1",4.0);
  Control.addVariable("BTubeWLayerThick2",5.0);
  Control.addVariable("BTubeWLayerThick3",5.0);
  

  Control.addVariable("CTubeLength",350.0);
  Control.addVariable("CTubeWidth",500.0);
  Control.addVariable("CTubeHeight",500.0);
  Control.addVariable("CTubeInnerWidth",10.0);
  Control.addVariable("CTubeInnerHeight",10.0);
  Control.addVariable("CTubeWallMat","Stainless304");

  Control.addVariable("CTubeNWallLayers",6);
  Control.addVariable("CTubeWLayerThick0",3.0);
  Control.addVariable("CTubeWLayerThick1",4.0);
  Control.addVariable("CTubeWLayerThick2",5.0);
  Control.addVariable("CTubeWLayerThick3",5.0);
  Control.addVariable("CTubeWLayerThick4",5.0);
  Control.addVariable("CTubeWLayerThick5",5.0);
  
  Control.addVariable("pointSourceXStep",1.0);
  Control.addVariable("diskSourceXStep",0.0);
  Control.addVariable("diskSourceYStep",1.0);
  Control.addVariable("diskSourceZStep",0.0);
  Control.addVariable("diskSourceRadius",0.2);
  Control.addVariable("diskSourceParticleType",1);  // neutron
  Control.addVariable("diskSourceASpread",90.0);
  Control.addVariable("diskSourceEnergy",3.0);
  Control.addVariable("diskSourceEProb",1.0);
  
  
  Control.addVariable("ATubeLength",35.0);
  Control.addVariable("ATubeWidth",10.0);
  Control.addVariable("ATubeHeight",10.0);
  Control.addVariable("ATubeInnerWidth",5.0);
  Control.addVariable("ATubeInnerHeight",5.0);
  Control.addVariable("ATubeWallMat","Stainless304");

  return;
}

}  // NAMESPACE setVariable
 
