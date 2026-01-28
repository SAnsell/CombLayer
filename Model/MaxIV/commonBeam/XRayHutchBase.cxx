/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonBeam/XRayHutchBase.cxx
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
#include <array>

#include "FileReport.h"
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
#include "objectRegister.h"
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
#include "SurfMap.h"
#include "ExternalCut.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "forkHoles.h"

#include "XRayHutchBase.h"


namespace xraySystem
{
  XRayHutchBase::XRayHutchBase(const std::string& Key) :
  attachSystem::FixedRotate(Key,18),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  forks(keyName+"Fork")
  /*!
    \param Key :: hutch name
  */
  {
    // Must explicitly set since not always populated:
    frontPlateThick = 0.0;
    frontPlateWidth = 0.0;
    frontPlateHeight= 0.0;

    backPlateInnerThick = 0.0;
    backPlateInnerWidth = 0.0;
    backPlateInnerHeight= 0.0;
  }

  void
  XRayHutchBase::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
  {
    ELog::RegMethod RegA("XRayHutchBase","populate");

    FixedRotate::populate(Control);

    height=Control.EvalVar<double>(keyName+"Height");
    length=Control.EvalVar<double>(keyName+"Length");
    outWidth=Control.EvalVar<double>(keyName+"OutWidth");

    innerThick=Control.EvalVar<double>(keyName+"InnerThick");
    pbWallThick=Control.EvalVar<double>(keyName+"PbWallThick");
    pbBackThick=Control.EvalVar<double>(keyName+"PbBackThick");
    pbRoofThick=Control.EvalVar<double>(keyName+"PbRoofThick");
    outerThick=Control.EvalVar<double>(keyName+"OuterThick");

    innerOutVoid=Control.EvalDefVar<double>(keyName+"InnerOutVoid",0.0);
    outerOutVoid=Control.EvalDefVar<double>(keyName+"OuterOutVoid",0.0);
    outerBackVoid=Control.EvalDefVar<double>(keyName+"OuterBackVoid",0.0);

    frontPlateActive=Control.EvalDefVar<int>(keyName+"FrontPlateActive",false);
    if (frontPlateActive) {
      frontPlateThick=Control.EvalVar<double>(keyName+"FrontPlateThick");
      frontPlateWidth=Control.EvalVar<double>(keyName+"FrontPlateWidth");
      frontPlateHeight=Control.EvalVar<double>(keyName+"FrontPlateHeight");
    }

    backPlateInnerActive=Control.EvalDefVar<int>(keyName+"BackPlateInnerActive",false);
    if (backPlateInnerActive) {
      backPlateInnerThick=Control.EvalVar<double>(keyName+"BackPlateInnerThick");
      backPlateInnerWidth=Control.EvalVar<double>(keyName+"BackPlateInnerWidth");
      backPlateInnerHeight=Control.EvalVar<double>(keyName+"BackPlateInnerHeight");
    }

    floorShineThick=Control.EvalVar<double>(keyName+"FloorShineThick");
    floorShineLength=Control.EvalVar<double>(keyName+"FloorShineLength");
    floorShineFrontLength=Control.EvalDefVar<double>(keyName+"FloorShineFrontLength", floorShineLength);
    floorShineBackLength=Control.EvalDefVar<double>(keyName+"FloorShineBackLength", floorShineLength);

    double holeRad(0.0);
    size_t holeIndex(0);
    do {
      const std::string iStr("Hole"+std::to_string(holeIndex));
      const double holeXStep=Control.EvalDefVar<double>(keyName+iStr+"XStep",0.0);
      const double holeZStep=Control.EvalDefVar<double>(keyName+iStr+"ZStep",0.0);
      holeRad=Control.EvalDefVar<double>(keyName+iStr+"Radius",-1.0);

      if (holeRad>Geometry::zeroTol) {
	holeOffset.push_back(Geometry::Vec3D(holeXStep,0.0,holeZStep));
	holeRadius.push_back(holeRad);
	holeIndex++;
      }
    } while(holeRad>Geometry::zeroTol);

    voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
    skinMat=ModelSupport::EvalMat<int>(Control,keyName+"SkinMat");
    pbMat=ModelSupport::EvalMat<int>(Control,keyName+"PbMat");
    floorShineMat=ModelSupport::EvalDefMat(Control,keyName+"FloorShineMat",pbMat);

    forks.populate(Control);
  }
}
