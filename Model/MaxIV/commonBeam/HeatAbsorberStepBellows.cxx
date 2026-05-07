/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/commonBeam/HeatAbsorberStepBellows.cxx
 *
 * Copyright (c) 2026 by U. Friman-Gayer
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

#include <cmath>
#include <fstream>
#include <iomanip>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <memory>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "SurfMap.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Plane.h"
#include "MaterialSupport.h"
#include "Exception.h"

#include "StepBellows.h"
#include "HeatAbsorberStepBellows.h"

namespace xraySystem
{

HeatAbsorberStepBellows::HeatAbsorberStepBellows(
  const std::string& Key, const std::string& heatAbsorberKey):
    StepBellows(Key),heatAbsorberKey(heatAbsorberKey)
  /*!
    Constructor
    \param Key :: Key name
  */
{}

HeatAbsorberStepBellows::~HeatAbsorberStepBellows()
  /*!
    Destructor
  */
{}

void
HeatAbsorberStepBellows::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables and check input
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("HeatAbsorberStepBellows","populate");

  StepBellows::populate(Control);

  heatAbsorberClosed=static_cast<bool>(
    Control.EvalVar<int>(heatAbsorberKey+"Closed"));
  inOutRange=Control.EvalVar<double>(heatAbsorberKey+"InOutRange");

  const double sign = heatAbsorberClosed ? -1.0 : 1.0;

  yAngle = sign*90.0;
  zStep = sign*inOutRange/2.0;
}

}