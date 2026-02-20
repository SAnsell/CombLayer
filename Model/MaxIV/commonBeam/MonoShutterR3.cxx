/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/MonoShutterR3.cxx
 *
 * Copyright (c) 2026 by Udo Friman-Gayer
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
#include "surfRegister.h"
#include "objectRegister.h"
#include "Vec3D.h"
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
#include "FixedGroup.h"
#include "FixedRotateGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "portItem.h"
#include "VirtualTube.h"
#include "PipeTube.h"
#include "FlangePlate.h"
#include "ShutterUnit.h"
#include "MonoShutterR3.h"

namespace xraySystem
{

MonoShutterR3::MonoShutterR3(const std::string& Key) :
  attachSystem::FixedRotate(Key,3),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut(),
  attachSystem::SurfMap(),
  attachSystem::CellMap(),
  
  entryAdapter(std::make_shared<constructSystem::FlangePlate>(keyName+"EntryAdapter")),
  exitAdapter(std::make_shared<constructSystem::FlangePlate>(keyName+"ExitAdapter")),
  shutterPipe(std::make_shared<constructSystem::PipeTube>(keyName+"Pipe")),
  monoShutterA(std::make_shared<xraySystem::ShutterUnit>(keyName+"UnitA")),
  monoShutterB(std::make_shared<xraySystem::ShutterUnit>(keyName+"UnitB"))
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  
  OR.addObject(shutterPipe);
  OR.addObject(monoShutterA);
  OR.addObject(monoShutterB);
}

void
MonoShutterR3::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("MonoShutterR3","populate");

  FixedRotate::populate(Control);

  height=Control.EvalVar<double>(keyName+"Height");
  length=Control.EvalVar<double>(keyName+"Length");

  beamPortInnerRadius=Control.EvalVar<double>(keyName+"BeamPortInnerRadius");
  beamPortWallThick=Control.EvalVar<double>(keyName+"BeamPortWallThick");
  beamPortFlangeRadius=Control.EvalVar<double>(keyName+"BeamPortFlangeRadius");
  beamPortFlangeLength=Control.EvalVar<double>(keyName+"BeamPortFlangeLength");

  vesselInnerRadius=Control.EvalVar<double>(keyName+"VesselInnerRadius");
  vesselWallThick=Control.EvalVar<double>(keyName+"VesselWallThick");
  vesselFlangeRadius=Control.EvalVar<double>(keyName+"VesselFlangeRadius");
  vesselFlangeLength=Control.EvalVar<double>(keyName+"VesselFlangeLength");

  apertureBackLength=Control.EvalVar<double>(keyName+"ApertureBackLength");
  apertureInnerRadius=Control.EvalVar<double>(keyName+"ApertureInnerRadius");
  apertureOuterRadius=Control.EvalVar<double>(keyName+"ApertureOuterRadius");
  apertureThick=Control.EvalVar<double>(keyName+"ApertureThick");
  apertureMat=ModelSupport::EvalMat<int>(Control,keyName+"ApertureMat");
  apertureToBlockGap=Control.EvalVar<double>(keyName+"ApertureToBlockGap");

  blockHeight=Control.EvalVar<double>(keyName+"BlockHeight");
  blockLength=Control.EvalVar<double>(keyName+"BlockLength");
  blockWidth=Control.EvalVar<double>(keyName+"BlockWidth");
  shutterDistance=Control.EvalVar<double>(keyName+"ShutterDistance");

  return;
}


void
MonoShutterR3::createSurfaces()
{
  ELog::RegMethod RegA("MonoShutterR3","createSurfaces");

  // Plane along the optical axis that divides the shutter into two (almost equal)
  // parts.
  // const int optAxisCentralPlane = shutterPipe->getPort(1).getLinkSurf(1);

  // Upstream and downstream aperture have the nominal 4 mm distance to the shutter 
  // blocks (Fig. 2.5 in [1]).
  // For the central aperture, the distance is slightly larger by default, because 
  // it is determined by the distance of the two shutter blocks. In order to position
  // them at 4 mm distance, the blocks would need to be so close to each other that 
  // their flanges overlap.
  // In Tab. 2.3 in Ref. [1], "clearance between blocks and apertures in z-dir <= 5 mm"
  // is given, showing that the uncertainty of the 4 mm is relatively large.

  if (!isActive("front"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*length/2.0,Y);
      ExternalCut::setCutSurf("front",buildIndex+1);
    }
  if (!isActive("back"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*length/2.0,Y);
      ExternalCut::setCutSurf("back",buildIndex+2);
    }

  const double width = 28.0;

  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*width/2.0,X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*width/2.0,X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*height/2.0,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*height/2.0,Z);

  ModelSupport::buildPlane(SMap,buildIndex+11,Origin-Y*(length/2.0-beamPortFlangeLength),Y);
  ModelSupport::buildPlane(SMap,buildIndex+22,Origin+Y*(length/2.0-beamPortFlangeLength),Y);

  ModelSupport::buildCylinder(SMap,buildIndex+7,Y,Y,beamPortFlangeRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Y,Y,beamPortInnerRadius+beamPortWallThick);
  ModelSupport::buildCylinder(SMap,buildIndex+27,Y,Y,beamPortInnerRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+37,Y,Y,apertureInnerRadius);

  ModelSupport::buildPlane(SMap,buildIndex+15,Origin-Z*(height/2.0-vesselFlangeLength),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*(height/2.0-vesselFlangeLength),Z);
  ModelSupport::buildCylinder(SMap,buildIndex+107,Z,Z,vesselFlangeRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+117,Z,Z,vesselInnerRadius+vesselWallThick);
  ModelSupport::buildCylinder(SMap,buildIndex+127,Z,Z,vesselInnerRadius);

  return; 
}

void
MonoShutterR3::createObjects(Simulation& System)
  /*!
    \param System :: Simulation to add results
  */
{
  ELog::RegMethod RegA("MonoShutterR3","createObjects");

  const HeadRule front = ExternalCut::getRule("front");
  const HeadRule back = ExternalCut::getRule("back");
  const HeadRule bottom = ModelSupport::getHeadRule(SMap,buildIndex,"5");
  const HeadRule top = ModelSupport::getHeadRule(SMap,buildIndex,"6");
  
  addOuterSurf(
    ModelSupport::getHeadRule(SMap,buildIndex,"3 -4")
    *front*back.complement()*bottom*top.complement()
  );

  makeCell("EntryFlange",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"-11 -7 27")*front);
  makeCell("EntryExitPipe",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"11 -22 -17 27 117"));
  makeCell("ExitFlange",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"22 -7 27")*back.complement());

  makeCell("VesselBottomFlange",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"-15 -107")*bottom);
  makeCell("VesselBottomVoid",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"3 -4 -15 107")
    *bottom*front*back.complement()
  );
  makeCell("VesselTopFlange",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"16 -107")*top.complement());
  makeCell("VesselTopVoid",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"3 -4 16 107")
    *top.complement()*front*back.complement()
  );
  

  makeCell("Vessel",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"15 -16 -117 127 27"));

  return; 
}

void
MonoShutterR3::buildComponents(Simulation& System)
  /*!
    Construct the object to be built
    \param System :: Simulation
  */
{
  ELog::RegMethod RegA("MonoShutterR3","buildComponents");
  return;
}

void
MonoShutterR3::createLinks()
{
  ELog::RegMethod RControl("MonoShutterR3","createLinks");

  return;
}

void
MonoShutterR3::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    External build everything
    \param System :: Simulation
    \param FC :: FixedComp for beam origin
    \param sideIndex :: link point of centre [and axis]
   */
{
  ELog::RegMethod RegA("MonoShutterR3","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);

  buildComponents(System);

  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // NAMESPACE xraySystem
