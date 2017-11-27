/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/NNBAR.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#include "FrontBackCut.h"
#include "World.h"
#include "AttachSupport.h"
#include "beamlineSupport.h"
#include "GuideItem.h"
#include "HoleShape.h"
#include "Jaws.h"
#include "GuideLine.h"
#include "DiskChopper.h"
#include "VacuumBox.h"
#include "VacuumPipe.h"
#include "Bunker.h"
#include "BunkerInsert.h"
#include "LineShield.h"
#include "DetectorChamber.h"

#include "NNBAR.h"

namespace essSystem
{

NNBAR::NNBAR(const std::string& keyName) :
  attachSystem::CopiedComp("nnbar",keyName),
  startPoint(0),stopPoint(0),
  nnbarAxis(new attachSystem::FixedOffset(newName+"Axis",4)),
  FocusA(new beamlineSystem::GuideLine(newName+"FA")),
  VPipeB(new constructSystem::VacuumPipe(newName+"PipeB")),
  FocusB(new beamlineSystem::GuideLine(newName+"FB")),
  VPipeC(new constructSystem::VacuumPipe(newName+"PipeC")),
  FocusC(new beamlineSystem::GuideLine(newName+"FC")),
  VPipeD(new constructSystem::VacuumPipe(newName+"PipeD")),
  FocusD(new beamlineSystem::GuideLine(newName+"FD")),
  BInsert(new BunkerInsert(newName+"BInsert")),
  
  ShieldA(new constructSystem::LineShield(newName+"ShieldA")),
  VPipeOutA(new constructSystem::VacuumPipe(newName+"PipeOutA")),
  FocusOutA(new beamlineSystem::GuideLine(newName+"FOutA")),
  Cave(new DetectorChamber(newName+"Cave")),
  CaveCut(new constructSystem::HoleShape(newName+"CaveCut"))
 /*!
    Constructor
 */
{
  ELog::RegMethod RegA("NNBAR","NNBAR");
 
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  // This is necessary as not directly constructed:
  OR.cell(newName+"Axis");
  OR.addObject(nnbarAxis);

  OR.addObject(FocusA);

  OR.addObject(VPipeB);
  OR.addObject(FocusB);

  OR.addObject(VPipeC);
  OR.addObject(FocusC);

  OR.addObject(VPipeD);
  OR.addObject(FocusD);


  OR.addObject(BInsert);

  OR.addObject(ShieldA);
  OR.addObject(VPipeOutA);
  OR.addObject(FocusOutA);

  OR.addObject(Cave);
  OR.addObject(CaveCut);
}

  
NNBAR::~NNBAR()
  /*!
    Destructor
  */
{}


void
NNBAR::buildBunkerUnits(Simulation& System,
                        const attachSystem::FixedComp& FA,
                        const long int startIndex,
                        const int bunkerVoid)
  /*!
    Build all the components in the bunker space
    \param System :: simulation
    \param FA :: Fixed component to start build from [Mono guide]
    \param startIndex :: Fixed component link point
    \param bunkerVoid :: cell to place objects in
   */
{
  ELog::RegMethod RegA("NNBAR","buildBunkerUnits");
  
  VPipeB->addInsertCell(bunkerVoid);
  VPipeB->createAll(System,FA,startIndex);

  FocusB->addInsertCell(VPipeB->getCells("Void"));
  FocusB->createAll(System,*VPipeB,0,*VPipeB,0);

  // pipe from gamma shield to 10m
  VPipeC->addInsertCell(bunkerVoid);
  VPipeC->createAll(System,FocusB->getKey("Guide0"),2);

  FocusC->addInsertCell(VPipeC->getCells("Void"));
  FocusC->createAll(System,*VPipeC,0,*VPipeC,0);

  // pipe to 30m
  VPipeD->addInsertCell(bunkerVoid);
  VPipeD->createAll(System,FocusC->getKey("Guide0"),2);

  FocusD->addInsertCell(VPipeD->getCells("Void"));
  FocusD->createAll(System,*VPipeD,0,*VPipeD,0);

  return;
}


void
NNBAR::buildOutGuide(Simulation& System,
                     const attachSystem::FixedComp& FW,
                     const long int startIndex,
                     const int voidCell)
  /*!
    Build all the components that are outside of the wall
    \param System :: Simulation 
    \param FW :: Focus wall fixed axis
    \param startPoint :: link point 
    \param voidCell :: void cell nubmer
   */
{
  ELog::RegMethod RegA("NNBAR","buildOutGuide");

  ShieldA->addInsertCell(voidCell);
  ShieldA->createAll(System,FW,startIndex);

  VPipeD->addInsertCell(ShieldA->getCell("Void"));
  VPipeD->insertObjects(System);   

  VPipeOutA->addInsertCell(ShieldA->getCell("Void"));
  VPipeOutA->createAll(System,FW,startIndex);

  FocusOutA->addInsertCell(VPipeOutA->getCells("Void"));
  FocusOutA->createAll(System,FW,startIndex,FW,startIndex);
  
  return;
}

void
NNBAR::buildHut(Simulation& System,
		const attachSystem::FixedComp& connectFC,
		const long int connectIndex,
                const int voidCell)
  /*!
    Builds the hut connected to the FixedPoint given
    \param System :: Simulation to build with
    \param connectFC :: Connection point
    \param connectIndex :: Connection index
    \param voidCell :: Main void cell for this model
   */
{
  ELog::RegMethod RegA("NNBAR","buildHut");

  Cave->addInsertCell(voidCell);
  Cave->createAll(System,connectFC,connectIndex);

  ShieldA->addInsertCell(Cave->getCells("ConcFront"));
  ShieldA->insertObjects(System);

  CaveCut->addInsertCell(Cave->getCells("IronFront"));
  CaveCut->setFaces(Cave->getKey("Mid").getFullRule(-1),
                    Cave->getKey("Inner").getFullRule(1));
  CaveCut->createAll(System,Cave->getKey("Inner"),-1);

  return;
}

  
void
NNBAR::buildIsolated(Simulation& System,const int voidCell)
  /*!
    Carry out the build in isolation
    \param System :: Simulation system
    \param voidCell :: void cell
   */
{
  ELog::RegMethod RegA("NNBAR","buildIsolated");


  const FuncDataBase& Control=System.getDataBase();
  CopiedComp::process(System.getDataBase());
  startPoint=Control.EvalDefVar<int>(newName+"StartPoint",0);
  stopPoint=Control.EvalDefVar<int>(newName+"StopPoint",0);
  ELog::EM<<"BUILD ISOLATED Start/Stop:"
          <<startPoint<<" "<<stopPoint<<ELog::endDiag;

  const attachSystem::FixedComp* FStart(&(World::masterOrigin()));
  long int startIndex(0);
  
  if (startPoint<1)
    {
      startIndex= 2;
    }
  
  return;
}

  
void 
NNBAR::build(Simulation& System,
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
  ELog::RegMethod RegA("NNBAR","build");

  ELog::EM<<"\nBuilding NNBAR on : "<<GItem.getKeyName()<<ELog::endDiag;

  const FuncDataBase& Control=System.getDataBase();
  CopiedComp::process(System.getDataBase());
  stopPoint=Control.EvalDefVar<int>(newName+"StopPoint",0);
  ELog::EM<<"GItem == "<<GItem.getKey("Beam").getLinkPt(-1)
	  <<ELog::endDiag;
  
  essBeamSystem::setBeamAxis(*nnbarAxis,Control,GItem,1);

  FocusA->addInsertCell(GItem.getCells("Void"));
  FocusA->setFront(GItem.getKey("Beam"),-1);
  FocusA->setBack(GItem.getKey("Beam"),-2);
  FocusA->createAll(System,*nnbarAxis,-3,*nnbarAxis,-3);
  
  if (stopPoint==1) return;                      // STOP At monolith
                                                 // edge  
  buildBunkerUnits(System,FocusA->getKey("Guide0"),2,
                   bunkerObj.getCell("MainVoid"));

  // IN WALL
  // Make bunker insert
  BInsert->createAll(System,FocusC->getKey("Guide0"),2,bunkerObj);
  attachSystem::addToInsertSurfCtrl(System,bunkerObj,"frontWall",*BInsert);  


  VPipeD->addInsertCell(BInsert->getCell("Void"));
  VPipeD->insertObjects(System);   

  ShieldA->setFront(bunkerObj,2);
  buildOutGuide(System,FocusD->getKey("Guide0"),2,voidCell);

  buildHut(System,*ShieldA,2,voidCell);
  
  return;
}


}   // NAMESPACE essSystem

