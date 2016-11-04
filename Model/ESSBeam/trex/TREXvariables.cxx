/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   ESSBeam/trex/TREXvariables.cxx
 *
 * Copyright (c) 2004-2016 by Tsitohaina Randriamalala/Stuart Ansell
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
#include "ShieldGenerator.h"
#include "FocusGenerator.h"
#include "ChopperGenerator.h"
#include "PitGenerator.h"
#include "PipeGenerator.h"
#include "BladeGenerator.h"
#include "essVariables.h"


namespace setVariable
{
void TREXvariables(FuncDataBase& Control)
{
  ELog::RegMethod RegA("TREXvariables[F]","TREXvariables");

  setVariable::ChopperGenerator CGen;
  setVariable::FocusGenerator FGen;
  setVariable::ShieldGenerator SGen;
  setVariable::PitGenerator PGen;
  setVariable::PipeGenerator PipeGen;
  setVariable::BladeGenerator BGen;

  PipeGen.setPipe(12.0,0.5);
  PipeGen.setWindow(-2.0,0.5);
  PipeGen.setFlange(-4.0,1.0);

  SGen.addWall(1,20.0,"CastIron");
  SGen.addRoof(1,20.0,"CastIron");
  SGen.addFloor(1,20.0,"CastIron");
  SGen.addFloorMat(5,"Concrete");
  SGen.addRoofMat(5,"Concrete");
  SGen.addWallMat(5,"Concrete");

  // extent of beamline
  Control.addVariable("trexStopPoint",0);
  Control.addVariable("trexAxisXYAngle",0.0);   // rotation
  Control.addVariable("trexAxisZAngle",0.0);   // rotation
  Control.addVariable("trexAxisZStep",0.0);   // +/- height

  FGen.setGuideMat("Aluminium","CastIron");
  FGen.setThickness(0.8,0.5,0.5);
  FGen.clearYOffset();
  FGen.generateTaper(Control,"trexFA",350.0,9.0,6.38,3.0,4.43);


  //  Gamma blocker 
  PipeGen.generatePipe(Control,"trexPipeB",1.0,48.0);
  FGen.generateTaper(Control,"trexFB",46.0,6.38,6.0,4.43,4.59);

  //  4m first vac pipe:
  PipeGen.generatePipe(Control,"trexPipeC",4.0,400.0);
  // horrizontal bender
  FGen.generateBender(Control,"trexBC",396.0,6.0,6.0,4.59,5.64,
                      19000.0,0.0);

  

  return;
}
  
}//NAMESPACE setVariable
