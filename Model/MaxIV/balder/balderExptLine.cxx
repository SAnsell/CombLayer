/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File: balder/balderExptLine.cxx
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
#include "BaseVisit.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "CopiedComp.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"


#include "balderExptLine.h"

namespace xraySystem
{

// Note currently uncopied:

balderExptLine::balderExptLine(const std::string& Key) :
  attachSystem::CopiedComp(Key,Key),
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(newName,2),
  attachSystem::CellMap()
  /*!
    Constructor
    \param Key :: Name of construction key
    \param Index :: Index number
  */
{}

balderExptLine::~balderExptLine()
  /*!
    Destructor
   */
{}

void
balderExptLine::populate(const FuncDataBase& Control)
  /*!
    Populate the intial values [movement]
    \param Control :: Database of variables
  */
{
  ELog::RegMethod RegA("balderExptLine","populate");
  FixedOffset::populate(Control);

  sampleRadius=Control.EvalDefVar<double>(keyName+"SampleRadius",0.0);
  sampleYStep=Control.EvalDefVar<double>(keyName+"SampleYStep",0.0);

  beamStopYStep=Control.EvalDefVar<double>(keyName+"BeamStopYStep",0.0);
  beamStopThick=Control.EvalVar<double>(keyName+"BeamStopThick");
  beamStopRadius=Control.EvalVar<double>(keyName+"BeamStopRadius");

  sampleMat=ModelSupport::EvalDefMat(Control,keyName+"SampleMat",0);
  beamStopMat=ModelSupport::EvalMat<int>(Control,keyName+"BeamStopMat");

  return;
}

void
balderExptLine::createSurfaces()
  /*!
    Build all the surfaces
  */
{
  ELog::RegMethod RegA("balderExptLine","createSurfaces");

  if (sampleRadius>Geometry::zeroTol)
    ModelSupport::buildSphere
      (SMap,buildIndex+7,Origin+Y*sampleYStep,sampleRadius);

  ModelSupport::buildCylinder
    (SMap,buildIndex+107,Origin+Y*beamStopYStep,Y,beamStopRadius);
  ModelSupport::buildPlane
    (SMap,buildIndex+101,Origin+Y*(beamStopYStep-beamStopThick/2.0),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+102,Origin+Y*(beamStopYStep+beamStopThick/2.0),Y);

  return;
}


void
balderExptLine::buildObjects(Simulation& System)
  /*!
    Build all the objects
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("balderExptLine","buildObjects");

  HeadRule HR;

  if (sampleRadius>Geometry::zeroTol)
    {
      HR=ModelSupport::getHeadRule(SMap,buildIndex," -7 ");
      CellMap::makeCell("Sample",System,cellIndex++,sampleMat,0.0,HR);
      addOuterSurf(HR);
    }

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 101 -102 -107 ");
  CellMap::makeCell("BeamStop",System,cellIndex++,beamStopMat,0.0,HR);
  addOuterUnionSurf(HR);


  return;
}

void
balderExptLine::createLinks()
  /*!
    Create a front/back link
   */
{
  return;
}

void
balderExptLine::createAll(Simulation& System,
			  const attachSystem::FixedComp& FC,
			  const long int sideIndex)
  /*!
    Carry out the full build
    \param System :: Simulation system
    \param FC :: Fixed component
    \param sideIndex :: link point
   */
{
  // For output stream
  ELog::RegMethod RControl("balderExptLine","createAll");

  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  buildObjects(System);
  createLinks();
  insertObjects(System);
  return;
}


}   // NAMESPACE xraySystem
