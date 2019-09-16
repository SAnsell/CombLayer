/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
  * File: softimax/SOFTIMAX.cxx
  *
  * Copyright (c) 2004-2019 by Konstantin Batkov
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
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "inputParam.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Rules.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedGroup.h"
#include "FixedOffsetGroup.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "CopiedComp.h"
#include "World.h"
#include "AttachSupport.h"
#include "InnerZone.h"

#include "VacuumPipe.h"
#include "SplitFlangePipe.h"
#include "Bellows.h"
#include "LeadPipe.h"

#include "balderOpticsHutch.h"
#include "ExperimentalHutch.h"
#include "JawFlange.h"
#include "FlangeMount.h"
#include "R3FrontEnd.h"
#include "softimaxFrontEnd.h"

#include "ConnectZone.h"
#include "PipeShield.h"
#include "WallLead.h"
#include "R3Ring.h"
#include "R3Beamline.h"
#include "SOFTIMAX.h"

namespace xraySystem
{

  SOFTIMAX::SOFTIMAX(const std::string& KN) :
    R3Beamline("Balder",KN),
    frontBeam(new softimaxFrontEnd(newName+"FrontBeam"))
    /*!
      Constructor
      \param KN :: Keyname
    */
  {
    ModelSupport::objectRegister& OR=
      ModelSupport::objectRegister::Instance();

    OR.addObject(frontBeam);

  }

  SOFTIMAX::~SOFTIMAX()
  /*!
    Destructor
  */
  {}

  void 
  SOFTIMAX::build(Simulation& System,
		  const attachSystem::FixedComp& FCOrigin,
		  const long int sideIndex)
  /*!
    Carry out the full build
    \param System :: Simulation system
    \param FCOrigin :: Start origin
    \param sideIndex :: link point for origin
  */
  {
    // For output stream
    ELog::RegMethod RControl("SOFTIMAX","build");

    const size_t NS=r3Ring->getNInnerSurf();
    const size_t PIndex=static_cast<size_t>(std::abs(sideIndex)-1);
    const size_t SIndex=(PIndex+1) % NS;
    const size_t prevIndex=(NS+PIndex-1) % NS;

    const std::string exitLink="ExitCentre"+std::to_string(PIndex);

    frontBeam->setStopPoint(stopPoint);
    frontBeam->addInsertCell(r3Ring->getCell("InnerVoid",SIndex));

    frontBeam->setBack(-r3Ring->getSurf("BeamInner",PIndex));
    frontBeam->createAll(System,FCOrigin,sideIndex);

    return;
  }


}   // NAMESPACE xraySystem

