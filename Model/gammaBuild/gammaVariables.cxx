/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   t1Upgrade/gammaVariables.cxx
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
gammaVariables(FuncDataBase& Control)
  /*!
    Function to set the control variables and constants
    -- This version is for TS1 (real version : non-model)
    \param Control :: Function data base to add constants too
  */
{
  ELog::RegMethod RegA("gammaVariables[F]","TS1upgrade");
  Control.addVariable("zero",0.0);      // Zero
  Control.addVariable("one",1.0);       // One

  Control.addVariable("BrickXStep",0.0);       
  Control.addVariable("BrickYStep",0.0);       
  Control.addVariable("BrickZStep",0.0);       
  Control.addVariable("BrickXYangle",0.0);       
  Control.addVariable("BrickZangle",0.0);       

  Control.addVariable("BrickNLayers",1);       
  Control.addVariable("BrickWidth",12.0);       
  Control.addVariable("BrickDepth",2.0);       
  Control.addVariable("BrickHeight",10.0);       
  Control.addVariable("BrickTemp",300);       
  Control.addVariable("BrickMat","Copper");       

  Control.addVariable("BrickNConic",0);       
  Control.addVariable("BrickNWedge",0);       

  Control.addVariable("gammaSourceXStep",0.0);       
  Control.addVariable("gammaSourceYStep",0.0);       
  Control.addVariable("gammaSourceZStep",0.0);       
  Control.addVariable("gammaSourceRadius",3.0);       
  Control.addVariable("gammaSourceASpread",3.0);       
  Control.addVariable("gammaSourceEStart",3.0);       
  Control.addVariable("gammaSourceNE",10);       
  Control.addVariable("gammaSourceEEnd",80.0);     


  Control.addVariable("VoidTallyNSegment",18);     
  Control.addVariable("VoidTallyRadius",10.0);     
  Control.addVariable("VoidTallyThick",1.0);     
  Control.addVariable("VoidTallyHeight",3.0);     

  Control.addVariable("gammaSetupNDet",16);     
  Control.addVariable("NordNFace",5);     

  const double xStep(16.0);
  const double zStep(16.0);
  Control.addVariable("Nord0XStep",-xStep);     
  Control.addVariable("Nord0ZStep",0.0*zStep);     
  Control.addVariable("Nord0XYangle",0.0);     

  Control.addVariable("Nord1XStep",-xStep);     
  Control.addVariable("Nord1ZStep",-zStep);     
  Control.addVariable("Nord1XYangle",-30.0);     

  Control.addVariable("Nord2XStep",xStep);     
  Control.addVariable("Nord2ZStep",-zStep);     
  Control.addVariable("Nord2XYangle",-30.0);     

  Control.addVariable("Nord3XStep",xStep);     
  Control.addVariable("Nord3ZStep",zStep);     
  Control.addVariable("Nord3XYangle",-30.0);     

  Control.addVariable("Nord4XStep",-xStep);     
  Control.addVariable("Nord4ZStep",zStep);     
  Control.addVariable("Nord4XYangle",-60.0);     

  Control.addVariable("Nord5XStep",-xStep);     
  Control.addVariable("Nord5ZStep",-zStep);     
  Control.addVariable("Nord5XYangle",-60.0);     

  Control.addVariable("Nord6XStep",xStep);     
  Control.addVariable("Nord6ZStep",-zStep);     
  Control.addVariable("Nord6XYangle",-60.0);     

  Control.addVariable("Nord7XStep",xStep);     
  Control.addVariable("Nord7ZStep",zStep);     
  Control.addVariable("Nord7XYangle",-60.0);    

  Control.addVariable("Nord8XStep",-xStep);     
  Control.addVariable("Nord8ZStep",zStep);     
  Control.addVariable("Nord8XYangle",-90.0);     

  Control.addVariable("Nord9XStep",-xStep);     
  Control.addVariable("Nord9ZStep",-zStep);     
  Control.addVariable("Nord9XYangle",-90.0);     

  Control.addVariable("Nord10XStep",xStep);     
  Control.addVariable("Nord10ZStep",-zStep);     
  Control.addVariable("Nord10XYangle",-90.0);     

  Control.addVariable("Nord11XStep",xStep);     
  Control.addVariable("Nord11ZStep",zStep);     
  Control.addVariable("Nord11XYangle",-90.0);     

  Control.addVariable("Nord12XStep",-xStep);     
  Control.addVariable("Nord12ZStep",zStep);     
  Control.addVariable("Nord12XYangle",-120.0);     

  Control.addVariable("Nord13XStep",-xStep);     
  Control.addVariable("Nord13ZStep",-zStep);     
  Control.addVariable("Nord13XYangle",-120.0);     

  Control.addVariable("Nord14XStep",xStep);     
  Control.addVariable("Nord14ZStep",-zStep);     
  Control.addVariable("Nord14XYangle",-120.0);     

  Control.addVariable("Nord15XStep",xStep);     
  Control.addVariable("Nord15ZStep",zStep);     
  Control.addVariable("Nord15XYangle",-120.0);     

  Control.addVariable("NordZangle",0.0);     
  Control.addVariable("NordYStep",150.0);     
  Control.addVariable("NordFrontLength",6.0);     
  Control.addVariable("NordBackLength",10.0);     
  Control.addVariable("NordFaceWidth",4.0);     
  Control.addVariable("NordBackWidth",8.0);     
  Control.addVariable("NordWallThick",0.5);     
  Control.addVariable("NordPlateRadius",12.0);     
  Control.addVariable("NordPlateThick",0.8);     
  Control.addVariable("NordPlateThick",0.8);     
  Control.addVariable("NordSupportThick",7.0);     
  Control.addVariable("NordElecThick",11.0);     
  Control.addVariable("NordElecRadius",9.0);     
  Control.addVariable("NordWallMat","Aluminium");     
  Control.addVariable("NordMat","H2O");     
  Control.addVariable("NordPlateMat","Lead");     
  Control.addVariable("NordSupportMat","Lead");     
  Control.addVariable("NordElecMat","Lead");     

  return;
}

}  // NAMESPACE setVariable
