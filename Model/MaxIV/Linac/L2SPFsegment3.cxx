/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File: Linac/L2SPFsegment3.cxx
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
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "InnerZone.h"
#include "World.h"
#include "AttachSupport.h"
#include "generateSurf.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generalConstruct.h"

#include "VacuumPipe.h"
#include "OffsetFlangePipe.h"
#include "SplitFlangePipe.h"
#include "Bellows.h"
#include "FlatPipe.h"
#include "CorrectorMag.h"
#include "DipoleDIBMag.h"
#include "CorrectorMag.h"

#include "LObjectSupport.h"
#include "TDCsegment.h"
#include "L2SPFsegment3.h"

namespace tdcSystem
{

// Note currently uncopied:
  
L2SPFsegment3::L2SPFsegment3(const std::string& Key) :
  TDCsegment(Key,2),

  bellowA(new constructSystem::Bellows(keyName+"BellowA")),
  flatA(new tdcSystem::FlatPipe(keyName+"FlatA")),
  dipoleA(new tdcSystem::DipoleDIBMag(keyName+"DipoleA")),
  pipeA(new constructSystem::OffsetFlangePipe(keyName+"PipeA")),
  cMagHorA(new tdcSystem::CorrectorMag(keyName+"CMagHorA")),
  cMagVertA(new tdcSystem::CorrectorMag(keyName+"CMagVertA")),
  flatB(new tdcSystem::FlatPipe(keyName+"FlatB")),
  dipoleB(new tdcSystem::DipoleDIBMag(keyName+"DipoleB")),
  bellowB(new constructSystem::Bellows(keyName+"BellowB"))
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(bellowA);
  OR.addObject(flatA);
  OR.addObject(dipoleA);
  OR.addObject(pipeA);
  OR.addObject(cMagHorA);
  OR.addObject(cMagVertA);  
  OR.addObject(flatB);
  OR.addObject(dipoleB);
}
  
L2SPFsegment3::~L2SPFsegment3()
  /*!
    Destructor
   */
{}

void
L2SPFsegment3::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("L2SPFsegment3","buildObjects");

  int outerCell;

  MonteCarlo::Object* masterCell=buildZone->getMaster();
  if (!masterCell)
    masterCell=buildZone->constructMasterCell(System);

  bellowA->createAll(System,*this,0);
  outerCell=buildZone->createOuterVoidUnit(System,masterCell,*bellowA,2);
  bellowA->insertInCell(System,outerCell);

  flatA->createAll(System,*bellowA,"back");
  pipeMagGroup(System,*buildZone,flatA,
     {"FlangeA","Pipe"},"Origin","outerPipe",dipoleA);
  pipeTerminateGroup(System,*buildZone,flatA,{"FlangeB","Pipe"});

  pipeA->setFront(*flatA,"back");
  pipeA->createAll(System,*flatA,"back");  
  correctorMagnetPair(System,*buildZone,pipeA,cMagHorA,cMagVertA);
  pipeTerminate(System,*buildZone,pipeA);

  flatB->createAll(System,*pipeA,"back");
  pipeMagGroup(System,*buildZone,flatB,
     {"FlangeA","Pipe"},"Origin","outerPipe",dipoleB);
  pipeTerminateGroup(System,*buildZone,flatB,{"FlangeB","Pipe"});

  constructSystem::constructUnit
    (System,*buildZone,masterCell,*flatB,"back",*bellowB);


  
  buildZone->removeLastMaster(System);  
  return;
}

void
L2SPFsegment3::createLinks()
  /*!
    Create a front/back link
   */
{
  setLinkSignedCopy(0,*bellowA,1);
  setLinkSignedCopy(1,*bellowB,2);

  TDCsegment::setLastSurf(FixedComp::getFullRule(2));
  return;
}

void 
L2SPFsegment3::createAll(Simulation& System,
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
  ELog::RegMethod RControl("L2SPFsegment3","build");

  FixedRotate::populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  
  buildObjects(System);
  createLinks();

  return;
}


}   // NAMESPACE tdcSystem

