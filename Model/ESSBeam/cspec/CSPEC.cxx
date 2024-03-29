/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   ESSBeam/cspec/CSPEC.cxx
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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
#include <utility>
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
#include "FixedRotate.h"
#include "FixedRotateUnit.h"
#include "FixedGroup.h"
#include "FixedOffsetGroup.h"
#include "FixedRotateGroup.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "CopiedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "World.h"
#include "beamlineSupport.h"
#include "GuideItem.h"
#include "GuideUnit.h"
#include "PlateUnit.h"
#include "BenderUnit.h"
#include "DiskChopper.h"
#include "GeneralPipe.h"
#include "VacuumPipe.h"
#include "Bunker.h"
#include "SingleChopper.h"

#include "CSPEC.h"

namespace essSystem
{

CSPEC::CSPEC(const std::string& keyName) :
  attachSystem::CopiedComp("cspec",keyName),
  stopPoint(0),
  cspecAxis(new attachSystem::FixedRotateUnit(newName+"Axis",4)),

  FocusA(new beamlineSystem::PlateUnit(newName+"FA")),

  VPipeB(new constructSystem::VacuumPipe(newName+"PipeB")),
  FocusB(new beamlineSystem::PlateUnit(newName+"FB")),

  VPipeC(new constructSystem::VacuumPipe(newName+"PipeC")),
  FocusC(new beamlineSystem::PlateUnit(newName+"FC")),

  ChopperA(new essConstruct::SingleChopper(newName+"ChopperA")),
  BWDiskA(new essConstruct::DiskChopper(newName+"BWDiskA")),

  VPipeD(new constructSystem::VacuumPipe(newName+"PipeD")),
  BendD(new beamlineSystem::BenderUnit(newName+"BD"))


 /*!
    Constructor
    \param keyName :: keyname of beamline 
 */
{
  ELog::RegMethod RegA("CSPEC","CSPEC");
 
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(cspecAxis);

  OR.addObject(FocusA);

  OR.addObject(VPipeB);
  OR.addObject(FocusB);

  OR.addObject(VPipeC);
  OR.addObject(FocusC);

  OR.addObject(ChopperA);
  OR.addObject(BWDiskA);

  OR.addObject(VPipeD);
  OR.addObject(BendD);

}
  
CSPEC::~CSPEC()
  /*!
    Destructor
  */
{}
  
void 
CSPEC::build(Simulation& System,
	    const GuideItem& GItem,
	    const Bunker& bunkerObj,
	    const int)
  /*!
    Carry out the full build
    \param System :: Simulation system
    \param GItem :: Guide Item 
    \param BunkerObj :: Bunker component [for inserts]
    \param voidCell :: Void cell
   */
{
  // For output stream
  ELog::RegMethod RegA("CSPEC","build");

  const Geometry::Vec3D& ZVert(World::masterOrigin().getZ());
  
  ELog::EM<<"\nBuilding CSPEC on : "<<GItem.getKeyName()<<ELog::endDiag;

  const FuncDataBase& Control=System.getDataBase();
  CopiedComp::process(System.getDataBase());
  stopPoint=Control.EvalDefVar<int>(newName+"StopPoint",0);
  ELog::EM<<"GItem == "<<GItem.getKey("Beam").getLinkPt(-1)
	  <<ELog::endDiag;
  
  essBeamSystem::setBeamAxis(*cspecAxis,Control,GItem,1);

  FocusA->addInsertCell(GItem.getCells("Void"));
  FocusA->setFront(GItem.getKey("Beam"),-1);
  FocusA->setBack(GItem.getKey("Beam"),-2);
  FocusA->createAll(System,*cspecAxis,-3);

  if (stopPoint==1) return;                      // STOP At monolith
                                                 // edge
  VPipeB->addAllInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeB->createAll(System,*FocusA,2);

  FocusB->addInsertCell(VPipeB->getCells("Void"));
  FocusB->createAll(System,*VPipeB,0);

  VPipeC->addAllInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeC->createAll(System,*FocusB,2);

  
  FocusC->addInsertCell(VPipeC->getCells("Void"));
  FocusC->createAll(System,*VPipeC,0);

  ChopperA->addInsertCell(bunkerObj.getCell("MainVoid"));
  ChopperA->getKey("Main").setAxisControl(3,ZVert);
  ChopperA->getKey("BuildBeam").setAxisControl(3,ZVert);
  ChopperA->createAll(System,*FocusC,2);

  BWDiskA->addInsertCell(ChopperA->getCell("Void"));
  BWDiskA->createAll(System,ChopperA->getKey("Main"),0);
  ChopperA->insertAxle(System,*BWDiskA);
  
  VPipeD->addAllInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeD->createAll(System,ChopperA->getKey("Beam"),2);

  BendD->addInsertCell(VPipeD->getCells("Void"));
  BendD->createAll(System,*VPipeD,0);
  
  return;
}

}   // NAMESPACE essSystem

