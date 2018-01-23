/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   photon/balderVariables.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
balderVariables(FuncDataBase& Control)
  /*!
    Function to set the control variables and constants
    -- This version is for Photon Moderator
    \param Control :: Function data base to add constants too
  */
{
  ELog::RegMethod RegA("balderVariables[F]","balderVariables");

  Control.addVariable("OpticsDepth",100.0);
  Control.addVariable("OpticsHeight",200.0);
  Control.addVariable("OpticsLength",800.0);
  Control.addVariable("OpticsOutWidth",120.0);
  Control.addVariable("OpticsRingWidth",60.0);
  Control.addVariable("OpticsRingLength",200.0);
  Control.addVariable("OpticsRingWallLen",200.0);
  Control.addVariable("OpticsRingWallAngle",20.0);
  Control.addVariable("OpticsInnerThick",0.5);
  Control.addVariable("OpticsPbThick",5.0);
  Control.addVariable("OpticsOuterThick",0.5);
  Control.addVariable("OpticsFloorThick",50.0);

  Control.addVariable("OpticsSkinMat","Stainless304");
  Control.addVariable("OpticsPbMat","Lead");
  Control.addVariable("OpticsFloorMat","Concrete");

  Control.addVariable("TriggerPipeYStep",22.0);
  Control.addVariable("TriggerPipeHorrRadius",1.60);
  Control.addVariable("TriggerPipeVertRadius",5.0);
  Control.addVariable("TriggerPipeHeight",10.0);
  Control.addVariable("TriggerPipeDepth",26.5);
  Control.addVariable("TriggerPipeLength",21.5);

  Control.addVariable("TriggerPipeFeThick",0.5);

  Control.addVariable("TriggerPipeBasePlate",1.0);
  Control.addVariable("TriggerPipeTopPlate",1.0);
  Control.addVariable("TriggerPipeFlangeLength",0.6);
  Control.addVariable("TriggerPipeFlangeRadius",1.0);
  
  Control.addVariable("TriggerPipeFeMat","Stainless304");

  return;
}

}  // NAMESPACE setVariable
