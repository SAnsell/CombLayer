/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonBeam/DuctGenerator.cxx
 *
 * Copyright (c) 2004-2023 by Konstantin Batkov
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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"

#include "DuctGenerator.h"

namespace setVariable
{

DuctGenerator::DuctGenerator() :
  length(0.0),width(50.0),height(150.0),
  radius(1.0),voidMat(0),
  ductType("Cylinder"),
  shieldMat("Stainless304"),
  shieldType("None"),
  shieldPenetrationType("None"),
  shieldPenetrationZOffset(-2.0),
  shieldPenetrationXOffset(1.0),
  shieldPenetrationRadius(1.0),
  shieldPenetrationWidth(2.0),
  shieldPenetrationHeight(1.0),
  shieldPenetrationTiltXmin(0.0),
  shieldLedgeThick(5),
  shieldLedgeLength(10.0),
  shieldLedgeZOffset(0.0),
  shieldLedgeActive(0),
  shieldThick(5),
  shieldWidthRight(13),
  shieldWidthLeft(18),
  shieldDepth(18),
  shieldHeight(13),
  shieldWallOffset(0.0),
  ductMat("Void"),
  fillMat("Void"),
  fillDuctHeightRatio(0.0),
  fillOffset(0.0),
  fillLength(0.0)
  /*!
    Constructor and defaults
  */
{}

DuctGenerator::~DuctGenerator()
 /*!
   Destructor
 */
{}

void
DuctGenerator::generate(FuncDataBase& Control,
			const std::string& keyName,
			const std::string& ductType,
			const std::string& shieldType,
			const std::string& shieldPenetrationType,
			const double& zAngle,
			const double& xStep,
			const double& yStep,
			const double& zStep) const
/*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables
    \param keyName :: Head name for variable
  */
{
  ELog::RegMethod RegA("DuctGenerator","generate");

  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"Width",width);
  Control.addVariable(keyName+"Height",height);
  Control.addVariable(keyName+"Radius",radius);
  Control.addVariable(keyName+"DuctType",ductType);
  Control.addVariable(keyName+"ShieldType",shieldType);
  Control.addVariable(keyName+"ShieldPenetrationType",shieldPenetrationType);
  Control.addVariable(keyName+"ShieldPenetrationZOffset",shieldPenetrationZOffset);
  Control.addVariable(keyName+"ShieldPenetrationXOffset",shieldPenetrationXOffset);
  Control.addVariable(keyName+"ShieldPenetrationRadius",shieldPenetrationRadius);
  Control.addVariable(keyName+"ShieldPenetrationWidth",shieldPenetrationWidth);
  Control.addVariable(keyName+"ShieldPenetrationHeight",shieldPenetrationHeight);
  Control.addVariable(keyName+"ShieldPenetrationTiltXmin",shieldPenetrationTiltXmin);
  Control.addVariable(keyName+"ShieldLedgeThick",shieldLedgeThick);
  Control.addVariable(keyName+"ShieldLedgeLength",shieldLedgeLength);
  Control.addVariable(keyName+"ShieldLedgeZOffset",shieldLedgeZOffset);
  Control.addVariable(keyName+"ShieldLedgeActive",shieldLedgeActive);
  Control.addVariable(keyName+"ShieldThick",shieldThick);
  Control.addVariable(keyName+"ShieldWidthRight",shieldWidthRight);
  Control.addVariable(keyName+"ShieldWidthLeft",shieldWidthLeft);
  Control.addVariable(keyName+"ShieldDepth",shieldDepth);
  Control.addVariable(keyName+"ShieldHeight",shieldHeight);
  Control.addVariable(keyName+"ShieldWallOffset",shieldWallOffset);
  Control.addVariable(keyName+"DuctMat",ductMat);
  Control.addVariable(keyName+"FillMat",fillMat);
  Control.addVariable(keyName+"FillDuctHeightRatio",fillDuctHeightRatio);
  Control.addVariable(keyName+"FillOffset",fillOffset);
  Control.addVariable(keyName+"FillLength",fillLength);
  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"ShieldMat",shieldMat);
  Control.addVariable(keyName+"ZAngle",zAngle);
  Control.addVariable(keyName+"XStep",xStep);
  Control.addVariable(keyName+"YStep",yStep);
  Control.addVariable(keyName+"ZStep",zStep);

  return;

}

void
DuctGenerator::setSize(const double& r)
{
  ELog::RegMethod RegA("DuctGenerator","setSize(radius)");
  radius = r;
  width = 0.0;
  height = 0.0;
}

void
DuctGenerator::setSize(const double& w,const double& h)
{
  ELog::RegMethod RegA("DuctGenerator","setSize(width,height)");
  radius = 0.0;
  width = w;
  height = h;
}



}  // namespace setVariable
