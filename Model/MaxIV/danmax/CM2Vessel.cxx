/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   danmax/CM2Vessel.cxx
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
#include <memory>
#include <array>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
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
#include "generateSurf.h"
#include "LinkUnit.h"  
#include "FixedComp.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "portItem.h"

#include "VirtualTube.h"
#include "PipeTube.h"
#include "CM2Vessel.h"

namespace xraySystem
{

CM2Vessel::CM2Vessel(const std::string& Key) :
  constructSystem::PipeTube(Key)  
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

  
CM2Vessel::~CM2Vessel() 
  /*!
    Destructor
  */
{}

void
CM2Vessel::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("CM2Vessel","populate");

  // Refs. [1] and [3] give the distance from CM1 to CM2 along the beam axis as
  // 3395.0 mm (explicitly in [3], implicitly in [1]).
  // However, the center angle of 16.177 deg from Ref. [2], together with the
  // offset of 980 mm, results in a value of about 3378 mm that creates a visible
  // discrepancy in the geometry. For this reason, the optical-axis distance is
  // calculated here instead of taking it from [1] or [3].
  //
  // [1] SINCRYS layout CM1, Drawing 256569, 2025-05-06
  // [2] JJ X-RAY, SINCRYS beamline - MAXIV, Final Design Report v2, 23087, 2025-07-11
  // [3] JJ X-RAY, SINCRYS layout drawing, 23087, 2025-06-27 (Appendix to [2])

  const std::string beamName = "DanMAX";
  CM1Y = Control.EvalVar<double>(beamName+"CM1Y");
  CM2PortLength = Control.EvalVar<double>(beamName+"CM2PortLength");
  const double SINCRYSAngleDeg = Control.EvalVar<double>(beamName+"SINCRYSAngle");
  SINCRYSAngle = SINCRYSAngleDeg*M_PI/180.0;
  SINCRYSBranchShift = Control.EvalVar<double>(beamName+"SINCRYSBranchShift");

  const double sinCrysBranchMinAngleDeg = 15.041; // [2]
  const double sinCrysBranchMaxAngleDeg = 17.313; // [2]
  if(
    SINCRYSAngleDeg < sinCrysBranchMinAngleDeg - Geometry::zeroTol ||
    SINCRYSAngleDeg > sinCrysBranchMaxAngleDeg + Geometry::zeroTol
  ){
    ELog::EM << "SINCRYS branch angle of " + std::to_string(SINCRYSAngleDeg)
    + " deg is outside the nominal range [ " + std::to_string(sinCrysBranchMinAngleDeg)
    + " deg, " + std::to_string(sinCrysBranchMaxAngleDeg) + " deg ]" << ELog::endWarn;
  }

  xStep = SINCRYSBranchShift;
  yStep = (CM1Y+fabs(SINCRYSBranchShift)/tan(SINCRYSAngle))+CM2PortLength;
  zAngle=180.0;
  PipeTube::populate(Control);
}

void
CM2Vessel::createSurfaces(){
    PipeTube::createSurfaces();
}

}