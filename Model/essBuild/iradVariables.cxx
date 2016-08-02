/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/iradVariables.cxx
 *
 * Copyright (c) 2004-2016 by Stuart Ansell/Konstantin Batkov
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
#include "essVariables.h"

namespace setVariable
{


void
EssIradVariables(FuncDataBase& Control)
  /*!
    Create all the irad variables
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("essVariables[F]","EssIradVariables");

  Control.addVariable("IradCylZStep",-0.2);
  Control.addVariable("IradCylRadius",0.65);
  Control.addVariable("IradCylLength",9.6); // inner length
  Control.addVariable("IradCylNLayer",4);
  Control.addVariable("IradCylWallThick0",0.1);
  Control.addVariable("IradCylWallThick1",0.1);
  Control.addVariable("IradCylWallThick2",0.2);
  Control.addVariable("IradCylWallThick3",0.2);
  Control.addVariable("IradCylTemp",0.0);
  Control.addVariable("IradCylMat","Aluminium");
  Control.addVariable("IradCylWallMat0","Aluminium");
  Control.addVariable("IradCylWallMat1","Zircaloy2");
  Control.addVariable("IradCylWallMat2","Void");
  Control.addVariable("IradCylWallMat3","Aluminium");
        

  Control.addVariable("IradCylSampleX",0.25);
  Control.addVariable("IradCylSampleY",1.0);
  Control.addVariable("IradCylSampleZ",0.25);
  return;
}

}  // NAMESPACE setVariable
