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
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ContainedComp.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "MonoShutterR3.h"

namespace xraySystem
{

MonoShutterR3::MonoShutterR3(const std::string& Key) :
  attachSystem::FixedRotate(Key,3),
  attachSystem::ContainedComp(),
  attachSystem::FrontBackCut(),
  attachSystem::SurfMap(),
  attachSystem::CellMap()
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{}

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
  width=Control.EvalVar<double>(keyName+"Width");

  adapterInnerRadius=Control.EvalVar<double>(keyName+"AdapterInnerRadius");

  beamPortInnerRadius=Control.EvalVar<double>(keyName+"BeamPortInnerRadius");
  beamPortWallThick=Control.EvalVar<double>(keyName+"BeamPortWallThick");
  beamPortFlangeRadius=Control.EvalVar<double>(keyName+"BeamPortFlangeRadius");
  beamPortFlangeLength=Control.EvalVar<double>(keyName+"BeamPortFlangeLength");

  vesselInnerRadius=Control.EvalVar<double>(keyName+"VesselInnerRadius");
  vesselWallThick=Control.EvalVar<double>(keyName+"VesselWallThick");
  vesselFlangeRadius=Control.EvalVar<double>(keyName+"VesselFlangeRadius");
  vesselFlangeLength=Control.EvalVar<double>(keyName+"VesselFlangeLength");
  vesselMat=ModelSupport::EvalMat<int>(Control,keyName+"VesselMat");

  apertureBackLength=Control.EvalVar<double>(keyName+"ApertureBackLength");
  apertureInnerRadius=Control.EvalVar<double>(keyName+"ApertureInnerRadius");
  apertureOuterRadius=Control.EvalVar<double>(keyName+"ApertureOuterRadius");
  apertureThick=Control.EvalVar<double>(keyName+"ApertureThick");
  apertureMat=ModelSupport::EvalMat<int>(Control,keyName+"ApertureMat");
  apertureToBlockGap=Control.EvalVar<double>(keyName+"ApertureToBlockGap");

  blockHeight=Control.EvalVar<double>(keyName+"BlockHeight");
  blockLength=Control.EvalVar<double>(keyName+"BlockLength");
  blockWidth=Control.EvalVar<double>(keyName+"BlockWidth");
  blockMat=ModelSupport::EvalMat<int>(Control,keyName+"BlockMat");

  shutterDistance=Control.EvalVar<double>(keyName+"ShutterDistance");
  shutterPortLength=Control.EvalVar<double>(keyName+"ShutterPortLength");
  shutterPortInnerRadius=Control.EvalVar<double>(keyName+"ShutterPortInnerRadius");
  shutterPortWallThick=Control.EvalVar<double>(keyName+"ShutterPortWallThick");
  shutterPortFlangeRadius=Control.EvalVar<double>(keyName+"ShutterPortFlangeRadius");
  shutterPortFlangeLength=Control.EvalVar<double>(keyName+"ShutterPortFlangeLength");
  threadLength=Control.EvalVar<double>(keyName+"ThreadLength");
  threadRadius=Control.EvalVar<double>(keyName+"ThreadRadius");
  threadMat=ModelSupport::EvalMat<int>(Control,keyName+"ThreadMat");
  lift=Control.EvalVar<double>(keyName+"Lift");

  entryShutterUpFlag=Control.EvalVar<int>(keyName+"EntryShutterUpFlag");
  exitShutterUpFlag=Control.EvalVar<int>(keyName+"ExitShutterUpFlag");

  return;
}


void
MonoShutterR3::createSurfaces()
{
  ELog::RegMethod RegA("MonoShutterR3","createSurfaces");

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
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
      setFront(SMap.realSurf(buildIndex+1));
    }
  if (!isActive("back"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*length,Y);
      setBack(-SMap.realSurf(buildIndex+2));
    }

  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*width/2.0,X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*width/2.0,X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*height/2.0,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*height/2.0,Z);

  ModelSupport::buildPlane(SMap,buildIndex+11,Origin+Y*beamPortFlangeLength,Y);
  ModelSupport::buildPlane(SMap,buildIndex+12,
    Origin+Y*(length-beamPortFlangeLength),Y);

  ModelSupport::buildPlane(SMap,buildIndex+21,
    Origin+Y*(
      (length-shutterDistance-blockLength)/2.0
      -apertureToBlockGap-apertureThick
    ),Y);
  ModelSupport::buildPlane(SMap,buildIndex+22,
    Origin+Y*((length-shutterDistance-blockLength)/2.0-apertureToBlockGap),Y);

  ModelSupport::buildPlane(SMap,buildIndex+31,
    Origin+Y*(length-apertureThick)/2.0,Y);
  ModelSupport::buildPlane(SMap,buildIndex+32,
    Origin+Y*(length+apertureThick)/2.0,Y);

  ModelSupport::buildPlane(SMap,buildIndex+41,
    Origin+Y*((length+shutterDistance+blockLength)/2.0+apertureToBlockGap),Y);
  ModelSupport::buildPlane(SMap,buildIndex+42,
    Origin+Y*(
      (length+shutterDistance+blockLength)/2.0+apertureThick+apertureToBlockGap),Y);
  ModelSupport::buildPlane(SMap,buildIndex+52,
    Origin+Y*(
      (length+shutterDistance+blockLength)/2.0+apertureToBlockGap+apertureBackLength),Y);

  ModelSupport::buildPlane(SMap,buildIndex+61,Origin+Y*(beamPortFlangeLength*2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+62,
    Origin+Y*(length-beamPortFlangeLength*2.0),Y);

  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin+Y,Y,beamPortFlangeRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,
    Origin+Y,Y,beamPortInnerRadius+beamPortWallThick);
  ModelSupport::buildCylinder(SMap,buildIndex+27,Origin+Y,Y,beamPortInnerRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+37,Origin+Y,Y,adapterInnerRadius);

  ModelSupport::buildPlane(SMap,buildIndex+15,
    Origin-Z*(height/2.0-vesselFlangeLength),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,
    Origin+Z*(height/2.0-vesselFlangeLength),Z);
  ModelSupport::buildCylinder(SMap,buildIndex+107,
    Origin+Y*length/2.0+Z,Z,vesselFlangeRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+117,
    Origin+Y*length/2.0+Z,Z,vesselInnerRadius+vesselWallThick);
  ModelSupport::buildCylinder(SMap,buildIndex+127,
    Origin+Y*length/2.0+Z,Z,vesselInnerRadius);

  ModelSupport::buildCylinder(SMap,buildIndex+207,Origin+Y,Y,apertureOuterRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+217,Origin+Y,Y,apertureInnerRadius);

  ModelSupport::buildPlane(SMap,buildIndex+301,
    Origin+Y*((length-shutterDistance-blockLength)/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+302,
    Origin+Y*((length-shutterDistance+blockLength)/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+311,
    Origin+Y*((length+shutterDistance-blockLength)/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+312,
    Origin+Y*((length+shutterDistance+blockLength)/2.0),Y);


  ModelSupport::buildPlane(SMap,buildIndex+303,Origin-X*blockWidth/2.0,X);
  ModelSupport::buildPlane(SMap,buildIndex+304,Origin+X*blockWidth/2.0,X);
  double blockLift = entryShutterUpFlag ? lift : 0.0;
  ModelSupport::buildPlane(SMap,buildIndex+305,
    Origin+Z*(-blockHeight/2.0+blockLift),Z);
  ModelSupport::buildPlane(SMap,buildIndex+306,Origin+Z*(blockHeight/2.0+blockLift),Z);

  blockLift = exitShutterUpFlag ? lift : 0.0;
  ModelSupport::buildPlane(SMap,buildIndex+315,
    Origin+Z*(-blockHeight/2.0+blockLift),Z);
  ModelSupport::buildPlane(SMap,buildIndex+316,Origin+Z*(blockHeight/2.0+blockLift),Z);

  ModelSupport::buildPlane(SMap,buildIndex+26,
    Origin+Z*(blockHeight/2.0+lift+threadLength),Z);
  ModelSupport::buildPlane(SMap,buildIndex+36,Origin+Z*shutterPortLength,Z);
  ModelSupport::buildPlane(SMap,buildIndex+46,
    Origin+Z*(shutterPortLength-shutterPortFlangeLength),Z);
  ModelSupport::buildPlane(SMap,buildIndex+56,
    Origin+Z*(blockHeight/2.0+threadLength),Z);
  ModelSupport::buildCylinder(SMap,buildIndex+307,
    Origin+Y*(length-shutterDistance)/2.0,Z,shutterPortFlangeRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+317,
    Origin+Y*(length-shutterDistance)/2.0,Z,
    shutterPortInnerRadius+shutterPortWallThick);
  ModelSupport::buildCylinder(SMap,buildIndex+327,
    Origin+Y*(length-shutterDistance)/2.0,Z,shutterPortInnerRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+337,
    Origin+Y*(length-shutterDistance)/2.0,Z,threadRadius);

  ModelSupport::buildCylinder(SMap,buildIndex+407,
    Origin+Y*(length+shutterDistance)/2.0,Z,shutterPortFlangeRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+417,
    Origin+Y*(length+shutterDistance)/2.0,Z,
    shutterPortInnerRadius+shutterPortWallThick);
  ModelSupport::buildCylinder(SMap,buildIndex+427,
    Origin+Y*(length+shutterDistance)/2.0,Z,shutterPortInnerRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+437,
    Origin+Y*(length+shutterDistance)/2.0,Z,threadRadius);

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
  const HeadRule frontBack = front*back;
  const HeadRule leftRight = ModelSupport::getHeadRule(SMap,buildIndex,"3 -4");
  const HeadRule bottom = ModelSupport::getHeadRule(SMap,buildIndex,"5");
  const HeadRule top = ModelSupport::getHeadRule(SMap,buildIndex,"-6");
  
  addOuterSurf(
    ModelSupport::getHeadRule(SMap,buildIndex,"3 -4 -26")
    *frontBack*bottom
  );

  makeCell("EntryAdapter",System,cellIndex++,vesselMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"-11 -7 37")*front);
  makeCell("EntryAdapterVoid",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"-11 -37")*front);  
  makeCell("EntryFlange",System,cellIndex++,vesselMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"11 -61 -7 27"));
  makeCell("EntryExitPipe",System,cellIndex++,vesselMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"61 -62 -17 27 117"));
  makeCell("EntryPipeVoid",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"11 -21 -27 127"));

  makeCell("ExitAdapter",System,cellIndex++,vesselMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"12 -7 37")*back);
  makeCell("ExitPipeVoid",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"52 -37 127")*back);
  makeCell("ExitPipeVoid",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"-12 52 -27 37"));
  makeCell("ExitFlange",System,cellIndex++,vesselMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"-12 62 -7 27"));
  makeCell("EntryExitPipeOuterVoid",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"-7 17 61 -62 117"));

  makeCell("VesselBottomFlange",System,cellIndex++,vesselMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"-15 -107")*bottom);
  makeCell("VesselBottomVoid",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"-15 107")
    *leftRight*bottom*frontBack
  );
  makeCell("VesselTopFlange",System,cellIndex++,vesselMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"16 -107 337 437")*top);
  makeCell("VesselTopVoid",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"16 107")
    *leftRight*top*frontBack
  );
  
  makeCell("Vessel",System,cellIndex++,vesselMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"15 -16 -117 127 27"));
  makeCell("VesselOuterVoid",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"15 -16 7 117")*leftRight*frontBack);
  makeCell("VesselFrontVoid",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"-21 15 -16 -127"));

  makeCell("EntryAperture",System,cellIndex++,apertureMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"21 -22 -207 217"));
  makeCell("EntryApertureHole",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"21 -22 -217"));
  makeCell("EntryApertureOuterVoid",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"15 -16 21 -22 207 -127"));
  makeCell("CenterAperture",System,cellIndex++,apertureMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"31 -32 -207 217"));
  makeCell("CenterApertureHole",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"31 -32 -217"));
  makeCell("CenterApertureOuterVoid",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"15 -16 31 -32 207 -127"));
  makeCell("ExitAperture",System,cellIndex++,apertureMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"41 -42 -207 217"));
  makeCell("ExitAperture",System,cellIndex++,apertureMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"42 -52 -27 217"));
  makeCell("ExitApertureHole",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"41 -52 -217"));
  makeCell("ExitApertureOuterVoid",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"15 -16 41 -42 207 -127"));
  makeCell("ExitApertureOuterVoid",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"15 -16 42 -52 27 -127"));

  makeCell("EntryShutterPortFlange",System,cellIndex++,vesselMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"-36 46 -307 337"));
  makeCell("EntryShutterPort",System,cellIndex++,vesselMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"6 -46 -317 327"));
  makeCell("EntryShutterPortOuterVoid",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"6 -46 -327 337"));
  makeCell("EntryShutterPortOuterVoid",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"6 -46 -307 317"));

  makeCell("EntryBlock",System,cellIndex++,blockMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"301 -302 303 -304 305 -306"));
  makeCell("EntryBlockFrontBackVoid",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"22 -31 15 -16 (-301:302) -127"));
  makeCell("EntryBlockLeftRightVoid",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"301 -302 (-303:304) 305 -306 -127"));
  makeCell("EntryBlockBottomVoid",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"301 -302 15 -305 -127"));
  makeCell("EntryBlockTopVoid",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"301 -302 -16 306 -127 337"));
  if(entryShutterUpFlag){
    makeCell("EntryBlockThread",System,cellIndex++,threadMat,0.0,
      ModelSupport::getHeadRule(SMap,buildIndex,"-26 306 -337"));
  } else {
    makeCell("EntryBlockThread",System,cellIndex++,threadMat,0.0,
      ModelSupport::getHeadRule(SMap,buildIndex,"-56 306 -337"));
    makeCell("EntryBlockThreadTopVoid",System,cellIndex++,0,0.0,
      ModelSupport::getHeadRule(SMap,buildIndex,"-26 56 306 -337"));
  }

  makeCell("ExitShutterPortFlange",System,cellIndex++,vesselMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"-36 46 -407 437"));
  makeCell("ExitShutterPort",System,cellIndex++,vesselMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"6 -46 -417 427"));
  makeCell("EntryShutterPortOuterVoid",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"6 -46 -427 437"));
  makeCell("EntryShutterPortOuterVoid",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"6 -46 -407 417"));

  makeCell("EntryExitShutterPortVoid",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"6 -36 307 407")*frontBack*leftRight);
  makeCell("ThreadVoid",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"-26 36 337 437")*frontBack*leftRight);

  makeCell("ExitBlock",System,cellIndex++,blockMat,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"311 -312 303 -304 315 -316"));
  makeCell("ExitBlockFrontBackVoid",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"32 -41 15 -16 (-311:312) -127"));
  makeCell("ExitBlockLeftRightVoid",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"311 -312 (-303:304) 315 -316 -127"));
  makeCell("ExitBlockBottomVoid",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"311 -312 15 -315 -127"));
  makeCell("ExitBlockTopVoid",System,cellIndex++,0,0.0,
    ModelSupport::getHeadRule(SMap,buildIndex,"311 -312 -16 316 -127 437"));
  if(exitShutterUpFlag){
    makeCell("ExitBlockThread",System,cellIndex++,threadMat,0.0,
      ModelSupport::getHeadRule(SMap,buildIndex,"-26 316 -437"));
  } else {
    makeCell("ExitBlockThread",System,cellIndex++,threadMat,0.0,
      ModelSupport::getHeadRule(SMap,buildIndex,"-56 316 -437"));
    makeCell("ExitBlockThreadTopVoid",System,cellIndex++,0,0.0,
      ModelSupport::getHeadRule(SMap,buildIndex,"-26 56 316 -437"));
  }
}

void
MonoShutterR3::createLinks()
{
  ELog::RegMethod RControl("MonoShutterR3","createLinks");

  FrontBackCut::createLinks(*this,Origin,Y);

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

  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // NAMESPACE xraySystem
