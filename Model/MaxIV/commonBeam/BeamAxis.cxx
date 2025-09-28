/*********************************************************************
  CombLayer : MCNP(X) Input builder

  * File:   Model/MaxIV/commonBeam/BeamAxis.cxx
  *
  * Copyright (c) 2004-2025 by Konstantin Batkov
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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "Line.h"
#include "surfRegister.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "BeamAxis.h"

namespace constructSystem {

  BeamAxis::BeamAxis() :
    beamAxis(std::make_unique<Geometry::Line>())
  {
  }

  BeamAxis::BeamAxis(const BeamAxis&)
  {
  }

  BeamAxis&
  BeamAxis::operator=(const BeamAxis&A)
  {
    if (this != &A) {
      BeamAxis::operator=(A);
    }
    return *this;
  }

  void
  BeamAxis::setBeamAxis(const attachSystem::FixedComp& FC,
			const long int sIndex)
  /*!
    Set the beam axis
    \param FC :: FixedComp to use
    \param sIndex :: Link point index
  */
  {
    ELog::RegMethod RegA("BeamAxis","setBeamAxis(FC)");

    *beamAxis=Geometry::Line(FC.getLinkPt(sIndex),
			     FC.getLinkAxis(sIndex));

    return;
  }

  void
  BeamAxis::setBeamAxis(const Geometry::Vec3D& Org,
			const Geometry::Vec3D& Axis)
  /*!
    Set the beam axis
    \param Org :: Origin point for line
    \param Axis :: Axis of line
  */
  {
    ELog::RegMethod RegA("BeamAxis","setBeamAxis(Vec3D)");

    *beamAxis=Geometry::Line(Org,Axis);
    return;
  }

}
