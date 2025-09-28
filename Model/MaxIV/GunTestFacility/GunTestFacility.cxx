/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File: Linac/GunTestFacility.cxx
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
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <iterator>
#include <memory>

#include "FileReport.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"

#include "BuildingB.h"


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
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "CopiedComp.h"
#include "ExternalCut.h"
#include "BlockZone.h"




#include "GTFLine.h"
#include "GunTestFacility.h"

namespace MAXIV::GunTestFacility
{

  GunTestFacility::GunTestFacility(const std::string& KN) :
    attachSystem::FixedOffset(KN,6),
    attachSystem::CellMap(),
    buildingB(new BuildingB("BldB")),
    beamLine(new GTFLine("GTFLine"))
    /*!
      Constructor
      \param KN :: Keyname
    */
  {
    ELog::RegMethod RegA("GunTestFacility","GunTestFacility");

    ModelSupport::objectRegister& OR=
      ModelSupport::objectRegister::Instance();

    OR.addObject(buildingB);
    OR.addObject(beamLine);
  }

  GunTestFacility::~GunTestFacility()
  /*!
    Destructor
  */
  {}

  void
  GunTestFacility::createAll(Simulation& System,
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
    ELog::RegMethod RControl("GunTestFacility","createAll");

    constexpr int voidCell(74123);
    buildingB->addInsertCell(voidCell);
    buildingB->createAll(System,FCOrigin,sideIndex);

    beamLine->setInnerMat(buildingB->getCellMat(System,"GunRoom"));
    beamLine->addInsertCell(buildingB->getCell("GunRoom"));

    beamLine->setCutSurf("front",-buildingB->getSurf("front"));
    beamLine->setCutSurf("back", buildingB->getSurf("back"));
    beamLine->setCutSurf("floor",buildingB->getSurf("bottom"));

    beamLine->createAll(System,*buildingB,buildingB->getSideIndex("BackWallEast"));



    return;
  }

}
