/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/CylGateTubeGenertor.cxx
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#include <stack>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <numeric>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"

#include "CFFlanges.h"
#include "CylGateTubeGenerator.h"

namespace setVariable
{

CylGateTubeGenerator::CylGateTubeGenerator() :
  radius(CF63::innerRadius),depth(7.8),height(10.0),
  wallThick(CF63::wallThick),portRadius(CF40_22::innerRadius),
  portFlangeRadius(CF40_22::flangeRadius),
  portInner(1.85),portThick(0.6),
  topRadius(CF63::flangeRadius),topHoleRadius(0.63),
  topThick(CF63::flangeLength),liftHeight(7.8),
  liftRadius(2.3),liftThick(0.2),liftPlate(1.2),
  driveRadius(0.6),bladeLift(),bladeThick(0.4),
  bladeRadius(2.2),voidMat("Void"),bladeMat("Copper"),
  driveMat("Nickel"),wallMat("Stainless304")
  /*!
    Constructor and defaults
  */
{}

CylGateTubeGenerator::~CylGateTubeGenerator() 
 /*!
   Destructor
 */
{}
				  
void
CylGateTubeGenerator::generateGate(FuncDataBase& Control,
				   const std::string& keyName,
				   const bool closedFlag) const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param upFlag :: true if item open/withdrawn
  */
{
  ELog::RegMethod RegA("CylGateTubeGenerator","generatorGate");

    ELog::EM<<"Depth == "<<depth<<ELog::endDiag;

  Control.addVariable(keyName+"Radius",radius);
  Control.addVariable(keyName+"Depth",depth);
  Control.addVariable(keyName+"Height",height);
  Control.addVariable(keyName+"WallThick",wallThick);

  Control.addVariable(keyName+"PortRadius",portRadius);
  Control.addVariable(keyName+"PortFlangeRadius",portFlangeRadius);
  Control.addVariable(keyName+"PortInner",portInner);
  Control.addVariable(keyName+"PortThick",portThick);

  Control.addVariable(keyName+"TopRadius",topRadius);
  Control.addVariable(keyName+"TopHoleRadius",topHoleRadius);
  Control.addVariable(keyName+"TopThick",topThick);

  Control.addVariable(keyName+"LiftHeight",liftHeight);
  Control.addVariable(keyName+"LiftRadius",liftRadius);
  Control.addVariable(keyName+"LiftThick",liftThick);
  Control.addVariable(keyName+"LiftPlate",liftPlate);

  Control.addVariable(keyName+"DriveRadius",driveRadius);
  Control.addVariable(keyName+"BladeLift",bladeLift);
  Control.addVariable(keyName+"BladeThick",bladeThick);
  Control.addVariable(keyName+"BladeRadius",bladeRadius);
  
  Control.addVariable(keyName+"Closed",static_cast<int>(closedFlag));

  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"BladeMat",bladeMat);
  Control.addVariable(keyName+"DriveMat",driveMat);
  Control.addVariable(keyName+"WallMat",wallMat);

         
  return;
}

  
}  // NAMESPACE setVariable
