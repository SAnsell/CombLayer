/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonBeam/CardanBellowsSINCRYS.cxx
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

#include "SmallAngleBellows.h"
#include "CardanBellowsSINCRYS.h"

namespace xraySystem
{

CardanBellowsSINCRYS::CardanBellowsSINCRYS(const std::string& Key):
    SmallAngleBellows(Key)
  /*!
    Constructor
    \param Key :: Key name
  */
{}

CardanBellowsSINCRYS::~CardanBellowsSINCRYS()
  /*!
    Destructor
  */
{}

void
CardanBellowsSINCRYS::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables and check input
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("CardanBellowsSINCRYS","populate");

  SmallAngleBellows::populate(Control);

  const std::string beamName = "DanMAX";
  CM1ToCardanBellowsUpstream = Control.EvalVar<double>(
    beamName+"SINCRYSCM1ToCardanBellowsUpstream");
  CM2PortLength = Control.EvalVar<double>(beamName+"CM2PortLength");
  SINCRYSAngle = Control.EvalVar<double>(beamName+"SINCRYSAngle")*M_PI/180.0;
  SINCRYSBranchShift = Control.EvalVar<double>(beamName+"SINCRYSBranchShift");
  SINCRYSCenterAngle = Control.EvalVar<double>(beamName+"SINCRYSCenterAngle")*M_PI/180.0;

  const Geometry::Vec3D SINCRYSBranchCenterVector(
    cos(SINCRYSCenterAngle),-sin(SINCRYSCenterAngle),0.0);
  const Geometry::Vec3D SINCRYSTransportPipeUpstreamJoint = (
    SINCRYSBranchCenterVector*(
      CM1ToCardanBellowsUpstream+0.5*length
    )
  );
  const Geometry::Vec3D SINCRYSTransportPipeDownstreamJoint = (
    Geometry::Vec3D(
      -SINCRYSBranchShift/tan(SINCRYSAngle),SINCRYSBranchShift,0.0
    )
    -SINCRYSBranchCenterVector*(CM2PortLength+0.5*length)
  );
  Geometry::Vec3D SINCRYSTransportPipeDirection = (
    SINCRYSTransportPipeDownstreamJoint-SINCRYSTransportPipeUpstreamJoint
  );
  SINCRYSTransportPipeDirection.makeUnit();
  const double cardanBellowsAngleSign = (
      SINCRYSTransportPipeDirection*SINCRYSBranchCenterVector).Z()
      /fabs((SINCRYSTransportPipeDirection*SINCRYSBranchCenterVector).Z()
    );
  angle = cardanBellowsAngleSign*acos(
    SINCRYSTransportPipeDirection.dotProd(SINCRYSBranchCenterVector));
}

}