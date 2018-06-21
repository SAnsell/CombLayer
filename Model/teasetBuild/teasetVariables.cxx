/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   teaSetBuild/teaSetVariables.cxx
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
TeaSetVariables(FuncDataBase& Control)
  /*!
    Create all the beamline variabes
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("teaSetVariables[F]","TeaSetVariables");
  
  Control.addVariable("MugRadius",3.0);
  Control.addVariable("MugHeight",10.0);
  Control.addVariable("MugWallThick",0.5);
  Control.addVariable("MugHandleRadius",3.0);
  Control.addVariable("MugHandleOffset",-1.0);
  Control.addVariable("MugWallMat","Stainless304");

  Control.addVariable("pointSourceXStep",1.0);
  Control.addVariable("diskSourceXStep",0.0);
  Control.addVariable("diskSourceYStep",1.0);
  Control.addVariable("diskSourceZStep",0.0);
  Control.addVariable("diskSourceRadius",0.2);
  Control.addVariable("diskSourceParticleType",1);  // neutron
  Control.addVariable("diskSourceASpread",90.0);
  Control.addVariable("diskSourceEnergy",3.0);
  Control.addVariable("diskSourceEProb",1.0);
  

  return;
}

}  // NAMESPACE setVariable
 
