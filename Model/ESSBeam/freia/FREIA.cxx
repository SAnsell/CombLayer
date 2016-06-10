/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/FREIA.cxx
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "debugMethod.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "stringCombine.h"
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
#include "Simulation.h"

#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedGroup.h"
#include "FixedOffsetGroup.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "CopiedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "World.h"
#include "AttachSupport.h"
#include "GuideItem.h"
#include "Jaws.h"
#include "GuideLine.h"
#include "DiskChopper.h"
#include "VacuumBox.h"
#include "VacuumPipe.h"
#include "ChopperHousing.h"
#include "Bunker.h"
#include "BunkerInsert.h"
#include "ChopperPit.h"
#include "ChopperUnit.h"
#include "DetectorTank.h"
#include "CylSample.h"
#include "LineShield.h"

#include "FREIA.h"

namespace essSystem
{

FREIA::FREIA(const std::string& keyName) :
  attachSystem::CopiedComp("freia",keyName),
  stopPoint(0),
  freiaAxis(new attachSystem::FixedOffset(newName+"Axis",4)),

  BendA(new beamlineSystem::GuideLine(newName+"BA")),

  VPipeB(new constructSystem::VacuumPipe(newName+"PipeB")),
  BendB(new beamlineSystem::GuideLine(newName+"BB")),

  VPipeC(new constructSystem::VacuumPipe(newName+"PipeC")),
  BendC(new beamlineSystem::GuideLine(newName+"BC")),

  ChopperA(new constructSystem::ChopperUnit(newName+"ChopperA")),
  DDisk(new constructSystem::DiskChopper(newName+"DBlade")),

  ChopperB(new constructSystem::ChopperUnit(newName+"ChopperB")),
  WFMDisk(new constructSystem::DiskChopper(newName+"WFMBlade")),

  VPipeD(new constructSystem::VacuumPipe(newName+"PipeD")),
  FocusD(new beamlineSystem::GuideLine(newName+"FD")),

  ChopperC(new constructSystem::ChopperUnit(newName+"ChopperC")),
  FOCDiskC(new constructSystem::DiskChopper(newName+"FOC1Blade"))

 /*!
    Constructor
 */
{
  ELog::RegMethod RegA("FREIA","FREIA");
 
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  // This is necessary as not directly constructed:
  OR.cell(newName+"Axis");
  OR.addObject(freiaAxis);

  OR.addObject(BendA);

  OR.addObject(VPipeB);
  OR.addObject(BendB);

  OR.addObject(VPipeC);
  OR.addObject(BendC);

  OR.addObject(ChopperA);
  OR.addObject(DDisk);  

  OR.addObject(ChopperB);
  OR.addObject(WFMDisk);  

  OR.addObject(VPipeD);
  OR.addObject(FocusD);

  OR.addObject(ChopperC);
  OR.addObject(FOCDiskC);  

}

FREIA::~FREIA()
  /*!
    Destructor
  */
{}

void
FREIA::setBeamAxis(const FuncDataBase& Control,
                   const GuideItem& GItem,
                   const bool reverseZ)
  /*!
    Set the primary direction object
    \param GItem :: Guide Item to 
    \param reverseZ :: Reverse axis
   */
{
  ELog::RegMethod RegA("FREIA","setBeamAxis");

  freiaAxis->populate(Control);
  freiaAxis->createUnitVector(GItem);
  freiaAxis->setLinkCopy(0,GItem.getKey("Main"),0);
  freiaAxis->setLinkCopy(1,GItem.getKey("Main"),1);
  freiaAxis->setLinkCopy(2,GItem.getKey("Beam"),0);
  freiaAxis->setLinkCopy(3,GItem.getKey("Beam"),1);

  if (reverseZ)
    freiaAxis->reverseZ();
  return;
}

  
void 
FREIA::build(Simulation& System,
	    const GuideItem& GItem,
	    const Bunker& bunkerObj,
	    const int voidCell)
  /*!
    Carry out the full build
    \param System :: Simulation system
    \param GItem :: Guide Item 
    \param BunkerObj :: Bunker component [for inserts]
    \param voidCell :: Void cell
   */
{
  ELog::RegMethod RegA("FREIA","build");

  ELog::EM<<"\nBuilding FREIA on : "<<GItem.getKeyName()<<ELog::endDiag;

  const FuncDataBase& Control=System.getDataBase();
  CopiedComp::process(System.getDataBase());
  stopPoint=Control.EvalDefVar<int>(newName+"StopPoint",0);
  ELog::EM<<"GItem == "<<GItem.getKey("Beam").getSignedLinkPt(-1)
	  <<ELog::endDiag;
  setBeamAxis(Control,GItem,0);
  
  BendA->addInsertCell(GItem.getCells("Void"));
  BendA->addEndCut(GItem.getKey("Beam").getSignedLinkString(-2));
  BendA->createAll(System,*freiaAxis,-3,*freiaAxis,-3);
  if (stopPoint==1) return;                      // STOP At monolith
                                                 // edge

  VPipeB->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeB->createAll(System,BendA->getKey("Guide0"),2);

  BendB->addInsertCell(VPipeB->getCells("Void"));
  BendB->createAll(System,*VPipeB,0,*VPipeB,0);
  
  VPipeC->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeC->createAll(System,BendB->getKey("Guide0"),2);

  BendC->addInsertCell(VPipeC->getCells("Void"));
  BendC->createAll(System,*VPipeC,0,*VPipeC,0);
  
  // First (green chopper)
  ChopperA->addInsertCell(bunkerObj.getCell("MainVoid"));
  ChopperA->createAll(System,BendC->getKey("Guide0"),2);

  // Double disk chopper
  DDisk->addInsertCell(ChopperA->getCell("Void"));
  DDisk->setCentreFlag(3);  // Z direction
  DDisk->setOffsetFlag(1);  // Z direction
  DDisk->createAll(System,ChopperA->getKey("Beam"),0);

    // First (green chopper)
  ChopperB->addInsertCell(bunkerObj.getCell("MainVoid"));
  ChopperB->createAll(System,ChopperA->getKey("Beam"),2);

  // Double disk chopper
  WFMDisk->addInsertCell(ChopperB->getCell("Void"));
  WFMDisk->setCentreFlag(3);  // Z direction
  WFMDisk->setOffsetFlag(1);  // Z direction
  WFMDisk->createAll(System,ChopperB->getKey("Beam"),0);

  VPipeD->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeD->createAll(System,ChopperB->getKey("Beam"),2);

  FocusD->addInsertCell(VPipeD->getCells("Void"));
  FocusD->createAll(System,*VPipeD,0,*VPipeD,0);

  // 8.5m FOC chopper
  ChopperC->addInsertCell(bunkerObj.getCell("MainVoid"));
  ChopperC->createAll(System,FocusD->getKey("Guide0"),2);
  // Double disk chopper
  FOCDiskC->addInsertCell(ChopperC->getCell("Void"));
  FOCDiskC->setCentreFlag(3);  // Z direction
  FOCDiskC->setOffsetFlag(1);  // Z direction
  FOCDiskC->createAll(System,ChopperC->getKey("Beam"),0);

  
  return;
}


}   // NAMESPACE essSystem

