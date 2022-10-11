/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   ESSBeam/nmx/NMX.cxx
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
#include "FixedRotateGroup.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "CopiedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "GuideItem.h"
#include "GuideUnit.h"
#include "PlateUnit.h"
#include "BenderUnit.h"
#include "GeneralPipe.h"
#include "VacuumPipe.h"
#include "WindowPipe.h"
#include "Bunker.h"
#include "BunkerInsert.h"
#include "BeamShutter.h"
#include "LineShield.h"
#include "PipeCollimator.h"
#include "AttachSupport.h"
#include "beamlineSupport.h"

#include "NMX.h"

namespace essSystem
{

NMX::NMX(const std::string& keyName) :
  attachSystem::CopiedComp("nmx",keyName),
  stopPoint(0),
  nmxAxis(new attachSystem::FixedRotateUnit(newName+"Axis",4)),
  FocusA(new beamlineSystem::PlateUnit(newName+"FA")),
  VPipeA(new constructSystem::WindowPipe(newName+"PipeA")),
  BendA(new beamlineSystem::BenderUnit(newName+"BA")),
  VPipeB(new constructSystem::WindowPipe(newName+"PipeB")),
  BendB(new beamlineSystem::BenderUnit(newName+"BB")),
  VPipeC(new constructSystem::WindowPipe(newName+"PipeC")),
  BendC(new beamlineSystem::BenderUnit(newName+"BC")),
  VPipeD(new constructSystem::WindowPipe(newName+"PipeD")),
  BendD(new beamlineSystem::BenderUnit(newName+"BD")),
  VPipeE(new constructSystem::WindowPipe(newName+"PipeE")),
  BendE(new beamlineSystem::BenderUnit(newName+"BE")),
  CollA(new constructSystem::PipeCollimator(newName+"CollA")),
  BInsert(new BunkerInsert(newName+"BInsert")),
  FocusWall(new beamlineSystem::PlateUnit(newName+"FWall")),
  MainShutter(new constructSystem::BeamShutter(newName+"MainShutter")),
  ShieldA(new constructSystem::LineShield(newName+"ShieldA"))
  /*!
    Constructor
 */
{
  ELog::RegMethod RegA("NMX","NMX");

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(nmxAxis);

  OR.addObject(FocusA);
  OR.addObject(VPipeA);
  OR.addObject(BendA);
  OR.addObject(VPipeB);
  OR.addObject(BendB);
  OR.addObject(VPipeC);
  OR.addObject(BendC);
  OR.addObject(VPipeD);
  OR.addObject(BendD);
  OR.addObject(VPipeE);
  OR.addObject(BendE);
  OR.addObject(BInsert);
  OR.addObject(FocusWall);

  OR.addObject(CollA);
  
  OR.addObject(MainShutter);
  OR.addObject(ShieldA);
}



NMX::~NMX()
  /*!
    Destructor
  */
{}

  
void 
NMX::build(Simulation& System,
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
  // For output stream
  ELog::RegMethod RegA("NMX","build");
  ELog::EM<<"\nBuilding NMX on : "<<GItem.getKeyName()<<ELog::endDiag;
  const FuncDataBase& Control=System.getDataBase();

  stopPoint=Control.EvalDefVar<int>(newName+"StopPoint",0);
  
  essBeamSystem::setBeamAxis(*nmxAxis,Control,GItem,1);


  FocusA->addInsertCell(GItem.getCells("Void"));
  FocusA->setFront(GItem.getKey("Beam"),-1);
  FocusA->setBack(GItem.getKey("Beam"),-2);
  FocusA->createAll(System,*nmxAxis,-3); // beam front reversed
  if (stopPoint==1) return;                  // STOP at Monolith
  
  return;

  // PIPE after gamma shield
  VPipeA->addAllInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeA->createAll(System,*FocusA,2);

  BendA->addInsertCell(VPipeA->getCells("Void"));
  BendA->createAll(System,*VPipeA,0);


  // PIPE from 10m to 14m
  VPipeB->addAllInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeB->setFront(*VPipeA,2);
  VPipeB->createAll(System,*BendA,2);

  BendB->addInsertCell(VPipeB->getCells("Void"));
  BendB->createAll(System,*BendA,2);

  // PIPE from 14m to 18m
  VPipeC->addAllInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeC->setFront(*VPipeB,2);
  VPipeC->createAll(System,*BendB,2);

  BendC->addInsertCell(VPipeC->getCells("Void"));
  BendC->createAll(System,*BendB,2);

  // PIPE from 18m to 22m
  VPipeD->addAllInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeD->setFront(*VPipeC,2);
  VPipeD->createAll(System,*BendC,2);

  BendD->addInsertCell(VPipeD->getCells("Void"));
  BendD->createAll(System,*BendC,2);

  // PIPE from 22m to Wall
  VPipeE->addAllInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeE->setFront(*VPipeD,2);
  VPipeE->createAll(System,*BendD,2);

  BendE->addInsertCell(VPipeE->getCells("Void"));
  BendE->createAll(System,*BendD,2);

  // EXPERIMENTAL WAY TO PLACE A SIMPLE COLLIMATOR   
  //  CollA->setInnerExclude(BendC->getXSectionOut());
  //  CollA->setOuter(VPipeC->getFullRule(-3));
  
  CollA->setCutSurf("Inner",BendC->getOuterSurf());
  CollA->setCutSurf("Outer",*VPipeC,-3);
  CollA->addInsertCell(VPipeC->getCell("Void"));
  CollA->createAll(System,*VPipeC,0);
  
  if (stopPoint==2) return;                      // STOP At bunker edge

  // First collimator [In WALL]
  BInsert->setBunkerObject(bunkerObj);
  BInsert->createAll(System,*BendE,2);
  attachSystem::addToInsertSurfCtrl(System,bunkerObj,"frontWall",*BInsert);

    // using 7 : mid point
  FocusWall->addInsertCell(BInsert->getCell("Void"));
  FocusWall->createAll(System,*BInsert,7);

  if (stopPoint==3) return;                  // STOP At bunker edge
  // Section to 24.5m
  ShieldA->addInsertCell(voidCell);
  ShieldA->setFront(bunkerObj,2);
  ShieldA->createAll(System,*FocusWall,2);

  MainShutter->addInsertCell(ShieldA->getCells("Void"));
  MainShutter->secondaryUnitVector(*ShieldA,-1);
  MainShutter->createAll(System,*ShieldA,-1);
  
                             
  return;
}


}   // NAMESPACE essSystem

