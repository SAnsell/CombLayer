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
    frontBeam(new softimaxFrontEnd(newName+"FrontBeam")),
    opticsHut(new balderOpticsHutch(newName+"OpticsHut")),
    joinPipe(new constructSystem::VacuumPipe(newName+"JoinPipe"))
    /*!
      Constructor
      \param KN :: Keyname
    */
  {
    ModelSupport::objectRegister& OR=
      ModelSupport::objectRegister::Instance();

    OR.addObject(frontBeam);
    OR.addObject(opticsHut);
    OR.addObject(joinPipe);

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

    if (stopPoint=="frontEnd" || stopPoint=="Dipole") return;

    opticsHut->setCutSurf("Floor",r3Ring->getSurf("Floor"));
    opticsHut->setCutSurf("RingWall",r3Ring->getSurf("BeamOuter",PIndex));

    opticsHut->addInsertCell(r3Ring->getCell("OuterSegment",prevIndex));
    opticsHut->addInsertCell(r3Ring->getCell("OuterSegment",PIndex));

    opticsHut->setCutSurf("SideWall",r3Ring->getSurf("FlatOuter",PIndex));
    opticsHut->setCutSurf("InnerSideWall",r3Ring->getSurf("FlatInner",PIndex));
    opticsHut->createAll(System,*r3Ring,r3Ring->getSideIndex(exitLink));

  // Ugly HACK to get the two objects to merge
  r3Ring->insertComponent
    (System,"OuterFlat",SIndex,
     *opticsHut,opticsHut->getSideIndex("frontCut"));

  // Inner space

  if (stopPoint=="opticsHut") return;

  joinPipe->addInsertCell(frontBeam->getCell("MasterVoid"));
  //  joinPipe->addInsertCell(wallLead->getCell("Void"));
  joinPipe->addInsertCell(opticsHut->getCell("Inlet"));
  joinPipe->createAll(System,*frontBeam,2);

  return;
  }


}   // NAMESPACE xraySystem

