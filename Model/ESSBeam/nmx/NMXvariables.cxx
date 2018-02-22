/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   ESSBeam/nmx/NMXvariables.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#include "PipeGenerator.h"
#include "essVariables.h"

namespace setVariable
{

void
NMXvariables(FuncDataBase& Control)
  /*!
    Create all the beamline variables for nmx
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("NMXvariables[F]","NMXvariables");

  
  setVariable::FocusGenerator FGen;
  setVariable::ShieldGenerator SGen;
  setVariable::PipeGenerator PipeGen;
  PipeGen.setPipe(12.0,1.0);
  PipeGen.setWindow(13.0,0.3);
  PipeGen.setFlange(16.0,1.0);
  
  SGen.addWall(1,10.0,"CastIron");
  SGen.addRoof(1,10.0,"CastIron");
  SGen.addFloor(1,10.0,"CastIron");
  SGen.addFloorMat(5,"Concrete");
  SGen.addRoofMat(5,"Concrete");
  SGen.addWallMat(5,"Concrete");

  Control.addVariable("nmxStopPoint",4);
  Control.addVariable("nmxAxisXStep",0.0);
  Control.addVariable("nmxAxisYStep",0.0);
  Control.addVariable("nmxAxisZStep",0.0);
  Control.addVariable("nmxAxisXYAngle",0.0);   // rotation 
  Control.addVariable("nmxAxisZAngle",1.0);

  FGen.setLayer(1,0.5,"Copper");
  FGen.setYOffset(0.1);
  FGen.generateTaper(Control,"nmxFA",350.0,3.0,3.0,3.0,4.5);
  Control.addVariable("nmxGABeamZAngle",0.0);         // -1.0 [BEND]

  FGen.setLayer(1,0.5,"Aluminium");
  FGen.setLayer(2,0.5,"B4C");
  FGen.setLayer(3,0.5,"Void");
  FGen.clearYOffset();
  
  const double bendAngle(90.0);
  const double bendRadius(120000.0);    // 1.2km 
  FGen.generateBender(Control,"nmxBA",394.0,3.0,3.0,3.0,4.5,
                      bendRadius,bendAngle);

  // VACUUM PIPES:
  PipeGen.generatePipe(Control,"nmxPipeA",50.0,400.0);
  
  FGen.setYOffset(4.5);
  FGen.generateBender(Control,"nmxBB",394.0,3.0,3.0,3.0,4.5,
                      bendRadius,bendAngle);
  PipeGen.generatePipe(Control,"nmxPipeB",0.0,400.0);

  FGen.generateBender(Control,"nmxBC",394.0,3.0,3.0,3.0,4.5,
                      bendRadius,bendAngle);
  PipeGen.generatePipe(Control,"nmxPipeC",0.0,400.0);

  FGen.generateBender(Control,"nmxBD",394.0,3.0,3.0,3.0,4.5,
                      bendRadius,bendAngle);
  PipeGen.generatePipe(Control,"nmxPipeD",0.0,400.0);

  FGen.generateBender(Control,"nmxBE",224.0,3.0,3.0,3.0,4.5,
                      bendRadius,bendAngle);
  PipeGen.generatePipe(Control,"nmxPipeE",0.0,230.0);

    
  Control.addVariable("nmxBInsertHeight",20.0);
  Control.addVariable("nmxBInsertWidth",28.0);
  Control.addVariable("nmxBInsertTopWall",1.0);
  Control.addVariable("nmxBInsertLowWall",1.0);
  Control.addVariable("nmxBInsertLeftWall",1.0);
  Control.addVariable("nmxBInsertRightWall",1.0);
  Control.addVariable("nmxBInsertWallMat","Stainless304");       

    // BEAM INSERT:
  FGen.clearYOffset();
  FGen.generateRectangle(Control,"nmxFWall",318.0,4.5,4.5);

  SGen.setRFLayers(3,8);
  SGen.generateShield(Control,"nmxShieldA",1750.0,40.0,40.0,40.0,8,8);

  // TEST OF COLLIMATORS IN PIPE:
  Control.addVariable("nmxCollALength",30.0);
  Control.addVariable("nmxCollAMat","Tungsten");

  Control.addVariable("nmxMainShutterYStep",3.0);
  Control.addVariable("nmxMainShutterLiftZStep",0.0);
  Control.addVariable("nmxMainShutterWidth",5.0);
  Control.addVariable("nmxMainShutterHeight",5.0);
  Control.addVariable("nmxMainShutterLength",20.0);
  Control.addVariable("nmxMainShutterNLayers",1);
  Control.addVariable("nmxMainShutterMat0","Stainless304");

  Control.addVariable("nmxMainShutterSurroundThick",1.0);
  Control.addVariable("nmxMainShutterSurroundMat","Aluminium");
  Control.addVariable("nmxMainShutterTopVoid",6.0);
  
  return;
}

}  // NAMESPACE setVariable
