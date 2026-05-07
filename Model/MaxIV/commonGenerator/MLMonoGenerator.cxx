/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonGenerator/MLMonoGenerator.cxx
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"

#include "MLMonoGenerator.h"

// References
// [1] Email from MJ 260506

namespace setVariable
{

MLMonoGenerator::MLMonoGenerator() :
  gap(0.4),phiA(0.0),phiB(0.0),
  widthA(4.5), // [1] "optic face width" in the attached picture
  heightA(4.5), // [1]
  lengthA(20.0), // [1]
  widthB(4.5), // [1] "optic face width" in the attached picture
  heightB(4.5), // [1]
  lengthB(20.0), // [1]
  supportAGap(0.3),supportAExtra(2.4),
  supportABackThick(0.8),supportABackLength(9.0),
  supportABase(1.1),supportAPillar(0.5),supportAPillarStep(0.8),
  supportBGap(0.3),supportBExtra(2.4),
  supportBBackThick(0.8),supportBBackLength(9.0),
  supportBBase(1.1),supportBPillar(0.5),supportBPillarStep(0.8),
  parked(1), // crystals are in the parked position by default
  parkedOffset(11.6), // [1]: can be anything from 116 to 698 mm
  parkedGap(0.6), // [1]: the gap is between 0.26 and 0.6 cm -> take the worst case
  mirrorAMat("Silicon300K"),mirrorBMat("Silicon300K"),
  baseAMat("Copper"),baseBMat("Copper")
  /*!
    Constructor and defaults
  */
{}


MLMonoGenerator::~MLMonoGenerator()
 /*!
   Destructor
 */
{}

void
MLMonoGenerator::generateMono(FuncDataBase& Control,
			      const std::string& keyName,
			      const double yStep,
			      const double thetaA,
			      const double thetaB) const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables
    \param keyName :: head name for variable
    \param yStep :: y-offset
    \param thetaA :: first crystal theta angle [mirror -xray 2T]
    \param thetaA :: second crystal theta angle [mirror -xray 2T]
  */
{
  ELog::RegMethod RegA("MLMonoGenerator","generate");

  Control.addVariable(keyName+"YAngle",180.0);
  Control.addVariable(keyName+"YStep",yStep);
  Control.addVariable(keyName+"ThetaA",thetaA);
  Control.addVariable(keyName+"ThetaB",thetaB);
  Control.addVariable(keyName+"PhiA",phiA);
  Control.addVariable(keyName+"PhiB",phiB);

  Control.addVariable(keyName+"Gap",gap);

  Control.addVariable(keyName+"WidthA",widthA);
  Control.addVariable(keyName+"HeightA",heightA);
  Control.addVariable(keyName+"LengthA",lengthA);

  Control.addVariable(keyName+"WidthB",widthB);
  Control.addVariable(keyName+"HeightB",heightB);
  Control.addVariable(keyName+"LengthB",lengthB);

  Control.addVariable(keyName+"SupportAGap",supportAGap);
  Control.addVariable(keyName+"SupportAExtra",supportAExtra);
  Control.addVariable(keyName+"SupportABackThick",supportABackThick);
  Control.addVariable(keyName+"SupportABackLength",supportABackLength);
  Control.addVariable(keyName+"SupportABase",supportABase);
  Control.addVariable(keyName+"SupportAPillar",supportAPillar);
  Control.addVariable(keyName+"SupportAPillarStep",supportAPillarStep);

  Control.addVariable(keyName+"SupportBGap",supportBGap);
  Control.addVariable(keyName+"SupportBExtra",supportBExtra);
  Control.addVariable(keyName+"SupportBBackThick",supportBBackThick);
  Control.addVariable(keyName+"SupportBBackLength",supportBBackLength);
  Control.addVariable(keyName+"SupportBBase",supportBBase);
  Control.addVariable(keyName+"SupportBPillar",supportBPillar);
  Control.addVariable(keyName+"SupportBPillarStep",supportBPillarStep);

  Control.addVariable(keyName+"Parked",parked);
  Control.addVariable(keyName+"ParkedOffset",parkedOffset);
  Control.addVariable(keyName+"ParkedGap",parkedGap);

  Control.addVariable(keyName+"MirrorAMat",mirrorAMat);
  Control.addVariable(keyName+"MirrorBMat",mirrorBMat);

  Control.addVariable(keyName+"BaseAMat",baseAMat);
  Control.addVariable(keyName+"BaseBMat",baseBMat);


  return;

}

}  // NAMESPACE setVariable
