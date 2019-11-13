/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File: balder/balderExptBeamline.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#include "FixedRotate.h"
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
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"

#include "insertObject.h"
#include "insertCylinder.h"
#include "VacuumPipe.h"
#include "SplitFlangePipe.h"

#include "balderExptBeamline.h"

namespace xraySystem
{

// Note currently uncopied:

balderExptBeamline::balderExptBeamline(const std::string& Key) :
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

balderExptBeamline::~balderExptBeamline()
  /*!
    Destructor
   */
{}

void
balderExptBeamline::populate(const FuncDataBase& Control)
  /*!
    Populate the intial values [movement]
    \param Control :: Database of variables
  */
{
  ELog::RegMethod RegA("balderExptBeamline","populate");
  FixedOffset::populate(Control);

  sampleRadius=Control.EvalDefVar<double>(keyName+"SampleRadius",0.0);
  sampleYStep=Control.EvalDefVar<double>(keyName+"SampleYStep",0.0);

  beamStopYStep=Control.EvalDefVar<double>(keyName+"BeamStopYStep",0.0);
  beamStopThick=Control.EvalVar<double>(keyName+"BeamStopThick");
  beamStopRadius=Control.EvalVar<double>(keyName+"BeamStopRadius");

  sampleMat=ModelSupport::EvalDefMat<int>(Control,keyName+"SampleMat",0);
  beamStopMat=ModelSupport::EvalMat<int>(Control,keyName+"BeamStopMat");

  return;
}

void
balderExptBeamline::createSurfaces()
  /*!
    Build all the surfaces
  */
{
  ELog::RegMethod RegA("balderExptBeamline","createSurfaces");

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
balderExptBeamline::buildObjects(Simulation& System)
  /*!
    Build all the objects
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("balderExptBeamline","buildObjects");

  std::string Out;

  if (sampleRadius>Geometry::zeroTol)
    {
      Out=ModelSupport::getComposite(SMap,buildIndex," -7 ");
      CellMap::makeCell("Sample",System,cellIndex++,sampleMat,0.0,Out);
      addOuterSurf(Out);
    }

  Out=ModelSupport::getComposite(SMap,buildIndex," 101 -102 -107 ");
  CellMap::makeCell("BeamStop",System,cellIndex++,beamStopMat,0.0,Out);
  addOuterUnionSurf(Out);


  return;
}

void
balderExptBeamline::createLinks()
  /*!
    Create a front/back link
   */
{
  return;
}

void
balderExptBeamline::createAll(Simulation& System,
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
  ELog::RegMethod RControl("balderExptBeamline","build");

  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  buildObjects(System);
  createLinks();
  insertObjects(System);
  return;
}


}   // NAMESPACE xraySystem
