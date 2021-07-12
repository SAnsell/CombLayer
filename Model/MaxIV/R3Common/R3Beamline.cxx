/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File: R3Common/R3Beamline.cxx
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
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <iterator>
#include <memory>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "CopiedComp.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "BlockZone.h"
#include "OpticsHutch.h"

#include "R3Ring.h"
#include "R3Beamline.h"

namespace xraySystem
{

R3Beamline::R3Beamline(const std::string& beamName,
			 const std::string& copiedName) :
  attachSystem::CopiedComp(beamName,copiedName)
  /*!
    Constructor
    \param beamName :: Full current naem
    \param copiedName :: copiedName
  */
{
}

R3Beamline::~R3Beamline()
  /*!
    Destructor
   */
{}

void
R3Beamline::buildOpticsHutch
(Simulation& System,
 const std::shared_ptr<xraySystem::OpticsHutch>& opticsHut,
 const size_t segmentIndex,
 const std::string& exitLink)
  /*!
    Build the optics hutch
    \param segmentIndex :: segment number     
   */
{
  ELog::RegMethod RegA("R3Beamline","buildHutch");

  const size_t NS=r3Ring->getNInnerSurf();
  const size_t prevSegment=(NS+segmentIndex-1) % NS;
  
  opticsHut->setCutSurf("Floor",r3Ring->getSurf("Floor"));
  opticsHut->setCutSurf("RingWall",
			r3Ring->getSurf("BeamOuter",segmentIndex));

  opticsHut->addInsertCell(r3Ring->getCell("OuterSegment",prevSegment));
  opticsHut->addInsertCell(r3Ring->getCell("OuterSegment",segmentIndex));

  opticsHut->setCutSurf("InnerSideWall",
			r3Ring->getSurf("FlatInner",segmentIndex));
  opticsHut->setCutSurf("SideWall",
			r3Ring->getSurf("FlatOuter",segmentIndex));

  const HeadRule cutHR=
    r3Ring->getSurfRule("FlatOuter",segmentIndex)+
    r3Ring->getSurfRule("FlatOuterCut",segmentIndex);
  opticsHut->setCutSurf("SideWallCut",cutHR);
  opticsHut->createAll(System,*r3Ring,exitLink);

  return;
}
  


}   // NAMESPACE xraySystem

