/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:    ESSBeam/magic/MAGICvariables.cxx
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
#include "FocusGenerator.h"
#include "ShieldGenerator.h"
#include "ChopperGenerator.h"
#include "PitGenerator.h"
#include "PipeGenerator.h"
#include "JawGenerator.h"
#include "BladeGenerator.h"
#include "essVariables.h"


namespace setVariable
{
  
void
MAGICvariables(FuncDataBase& Control)
  /*!
    Create all the beamline variables for vor
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("MAGICvariables[F]","MAGICvariables");

  setVariable::ChopperGenerator CGen;
  setVariable::FocusGenerator FGen;
  setVariable::ShieldGenerator SGen;
  setVariable::PitGenerator PGen;
  setVariable::PipeGenerator PipeGen;
  setVariable::BladeGenerator BGen;

  PipeGen.setPipe(8.0,0.5);
  PipeGen.setWindow(-2.0,0.5);
  PipeGen.setFlange(-4.0,1.0);

  SGen.addWall(1,20.0,"CastIron");
  SGen.addRoof(1,20.0,"CastIron");
  SGen.addFloor(1,20.0,"CastIron");
  SGen.addFloorMat(5,"Concrete");
  SGen.addRoofMat(5,"Concrete");
  SGen.addWallMat(5,"Concrete");

  // extent of beamline
  Control.addVariable("magicStopPoint",0);
  Control.addVariable("magicAxisXYAngle",0.0);   // rotation
  Control.addVariable("magicAxisZAngle",0.5);   // rotation
  Control.addVariable("magicAxisZStep",0.0);   // rotation

  FGen.setGuideMat("Copper");
  FGen.setYOffset(8.0);
  FGen.generateTaper(Control,"magicFA",350.0,11.0,3.0 ,4.0,3.0);

  PipeGen.generatePipe(Control,"magicPipeB",2.0,46.0);

  Control.addVariable("magicAppAInnerWidth",4.0);
  Control.addVariable("magicAppAInnerHeight",4.0);
  Control.addVariable("magicAppAWidth",12.0);
  Control.addVariable("magicAppAHeight",12.0);
  Control.addVariable("magicAppADepth",5.0);
  Control.addVariable("magicAppAYStep",7.0);
  Control.addVariable("magicAppADefMat","Tungsten");

  return;
}
 
}  // NAMESPACE setVariable
