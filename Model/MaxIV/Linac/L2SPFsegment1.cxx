/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File: Linac/L2SPFsegment1.cxx
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "CopiedComp.h"
#include "InnerZone.h"
#include "World.h"
#include "AttachSupport.h"
#include "generateSurf.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"

#include "VacuumPipe.h"
#include "SplitFlangePipe.h"
#include "Bellows.h"
#include "portItem.h"
#include "PipeTube.h"
#include "LQuad.h"
#include "CorrectorMag.h"

#include "L2SPFsegment1.h"

namespace tdcSystem
{

// Note currently uncopied:
  
L2SPFsegment1::L2SPFsegment1(const std::string& Key) :
  attachSystem::FixedOffset(Key,2),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  buildZone(*this,cellIndex),

  pipeA(new constructSystem::VacuumPipe(keyName+"PipeA")),
  bellowA(new constructSystem::Bellows(keyName+"BellowA")),
  pipeB(new constructSystem::VacuumPipe(keyName+"PipeB")),
  cMagHorrA(new tdcSystem::CorrectorMag(keyName+"CMagHorrA")),
  cMagVertA(new tdcSystem::CorrectorMag(keyName+"CMagVertA")),
  pipeC(new constructSystem::VacuumPipe(keyName+"PipeC")),
  pipeD(new constructSystem::VacuumPipe(keyName+"PipeD")),
  cMagHorrB(new tdcSystem::CorrectorMag(keyName+"CMagHorrB")),
  cMagVertB(new tdcSystem::CorrectorMag(keyName+"CMagVertB")),
  QuadA(new tdcSystem::LQuad(keyName+"QuadA")),
  pipeE(new constructSystem::VacuumPipe(keyName+"PipeE")),
  cMagHorrC(new tdcSystem::CorrectorMag(keyName+"CMagHorrC")),
  cMagVertC(new tdcSystem::CorrectorMag(keyName+"CMagVertC")),
  pumpA(new constructSystem::PipeTube(keyName+"PumpA"))
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(pipeA);
  OR.addObject(pipeB);
  OR.addObject(pipeC);
  OR.addObject(pipeD);
}
  
L2SPFsegment1::~L2SPFsegment1()
  /*!
    Destructor
   */
{}

void
L2SPFsegment1::populate(const FuncDataBase& Control)
  /*!
    Populate the intial values [movement]
    \param Control :: Database of variables
  */
{
  ELog::RegMethod RegA("L2SPFsegment1","populate");
  FixedOffset::populate(Control);

  outerLeft=Control.EvalDefVar<double>(keyName+"OuterLeft",0.0);
  outerRight=Control.EvalDefVar<double>(keyName+"OuterRight",0.0);
  outerHeight=Control.EvalDefVar<double>(keyName+"OuterHeight",0.0);

  const int voidMat=ModelSupport::EvalDefMat<int>(Control,keyName+"VoidMat",0);
  buildZone.setInnerMat(voidMat);

  return;
}


void
L2SPFsegment1::createSurfaces()
  /*!
    Create surfaces for the buildZone [if used]
  */
{
  ELog::RegMethod RegA("L2SPFsegment1","createSurfaces");

  const double totalLength(400.0);
  
  if (outerLeft>Geometry::zeroTol && isActive("floor"))
    {
      std::string Out;
      ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*outerLeft,X);
      ELog::EM<<"L3 == "<<Origin-X*outerLeft<<ELog::endDiag;
      ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*outerRight,X);
      ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*outerHeight,Z);
      Out=ModelSupport::getComposite(SMap,buildIndex," 3 -4 -6");
      const HeadRule HR(Out+getRuleStr("floor"));
      buildZone.setSurround(HR);
    }

  if (!isActive("front"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*100.0,Y);
      setCutSurf("front",SMap.realSurf(buildIndex+1));
    }

  if (!isActive("back"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*totalLength,Y);
      setCutSurf("back",-SMap.realSurf(buildIndex+2));
    }
  
    
  return;
}

void
L2SPFsegment1::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("L2SPFsegment1","buildObjects");

  int outerCell;
  buildZone.setFront(getRule("front"));
  buildZone.setBack(getRule("back"));
  
  MonteCarlo::Object* masterCell=
    buildZone.constructMasterCell(System,*this);

  pipeA->createAll(System,*this,0);
  // dump cell for joinPipe
  //outerCell=buildZone.createOuterVoidUnit(System,masterCell,*pipeInit,-1);
  //if (preInsert)
  //preInsert->insertInCell(System,outerCell);

  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*pipeA,2);
  
  return;
}

void
L2SPFsegment1::createLinks()
  /*!
    Create a front/back link
   */
{
  //  setLinkSignedCopy(0,*bellowA,1);
  //  setLinkSignedCopy(1,*lastComp,2);
  return;
}

void 
L2SPFsegment1::createAll(Simulation& System,
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
  ELog::RegMethod RControl("L2SPFsegment1","build");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  
  buildObjects(System);
  createLinks();
  return;
}


}   // NAMESPACE tdcSystem
