/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/ODIN.cxx
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#include "SecondTrack.h"
#include "TwinComp.h"
#include "LayerComp.h"
#include "CellMap.h"
#include "World.h"
#include "AttachSupport.h"
#include "Jaws.h"
#include "GuideLine.h"
#include "DiskChopper.h"
#include "Bunker.h"
#include "BunkerInsert.h"
#include "ChopperPit.h"
#include "ODIN.h"

namespace essSystem
{

ODIN::ODIN() :
  BladeChopper(new constructSystem::DiskChopper("odinBlade")),
  GuideA(new beamlineSystem::GuideLine("odinGA")),
  T0Chopper(new constructSystem::DiskChopper("odinTZero")),
  GuideB(new beamlineSystem::GuideLine("odinGB")),
  BInsert(new BunkerInsert("odinBInsert")),
  GuideC(new beamlineSystem::GuideLine("odinGC")),
  GuideD(new beamlineSystem::GuideLine("odinGD")),
  PitA(new constructSystem::ChopperPit("odinPitA")),
  GuidePitAFront(new beamlineSystem::GuideLine("odinGPitAFront")),
  GuidePitABack(new beamlineSystem::GuideLine("odinGPitABack")),
  GuideE(new beamlineSystem::GuideLine("odinGE")),

  PitB(new constructSystem::ChopperPit("odinPitB")),
  GuidePitBFront(new beamlineSystem::GuideLine("odinGPitBFront")),
  GuidePitBBack(new beamlineSystem::GuideLine("odinGPitBBack"))

 /*!
    Constructor
 */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(BladeChopper);
  OR.addObject(GuideA);
  OR.addObject(T0Chopper);
  OR.addObject(GuideB);
  OR.addObject(BInsert);
  
  OR.addObject(GuideC);
  OR.addObject(GuideD);
  OR.addObject(PitA);
  OR.addObject(GuidePitAFront);
  OR.addObject(GuidePitABack);

  OR.addObject(GuideE);
  
}




ODIN::~ODIN()
  /*!
    Destructor
  */
{}


void 
ODIN::build(Simulation& System,const attachSystem::TwinComp& GItem,
	    const Bunker& bunkerObj,const int voidCell)
  /*!
    Carry out the full build
    \param System :: Simulation system
    \param GItem :: Guide Item 
    \param BunkerObj :: Bunker component [for inserts]
    \param voidCell :: Void cell
   */
{
  // For output stream
  ELog::RegMethod RegA("ODIN","build");
  
  BladeChopper->addInsertCell(bunkerObj.getCell("MainVoid"));
  BladeChopper->createAll(System,GItem,2);

  GuideA->addInsertCell(bunkerObj.getCell("MainVoid"));
  GuideA->createAll(System,BladeChopper->getKey("Main"),2,
		    BladeChopper->getKey("Beam"),2);
  
  T0Chopper->addInsertCell(bunkerObj.getCell("MainVoid"));
  T0Chopper->createAll(System,GuideA->getKey("Guide0"),2);

  GuideB->addInsertCell(bunkerObj.getCell("MainVoid"));
  GuideB->addEndCut(bunkerObj.getSignedLinkString(8));
  GuideB->createAll(System,T0Chopper->getKey("Main"),2,
		    T0Chopper->getKey("Beam"),2);

  BInsert->setInsertCell(bunkerObj.getCells("MainWall8"));
  BInsert->createAll(System,GuideB->getKey("Guide0"),2,bunkerObj);

  // Guide in the bunker insert
  GuideC->addInsertCell(BInsert->getCell("Void"));
  GuideC->addEndCut(bunkerObj.getSignedLinkString(-2));
  GuideC->createAll(System,*BInsert,-1,*BInsert,-1);

  // Guide leaving the bunker
  ELog::EM<<"GuideC exit point == "<<
    GuideC->getKey("Guide0").getSignedLinkPt(2)<<ELog::endDiag;


  GuideD->addInsertCell(voidCell);
  GuideD->createAll(System,*BInsert,2,GuideC->getKey("Guide0"),2);

  // First chopper pit out of bunker
  // Guide guide String
  const attachSystem::FixedComp& GOuter=GuideD->getKey("Shield");
  HeadRule GuideCut;
  for(size_t i=1;i<6;i++)
    GuideCut.addUnion(GOuter.getLinkString(i));
  PitA->addInsertCell(voidCell);
  PitA->createAll(System,GuideD->getKey("Guide0"),2,GuideCut.display());

  GuidePitAFront->addInsertCell(PitA->getCell("MidLayer"));
  GuidePitAFront->addEndCut(PitA->getKey("Inner").getSignedLinkString(1));
  GuidePitAFront->createAll(System,GuideD->getKey("Guide0"),2,
			    GuideD->getKey("Guide0"),2);

  GuideE->addInsertCell(voidCell);
  GuideE->addInsertCell(PitA->getCell("MidLayer"));
  GuideE->addInsertCell(PitA->getCell("Outer"));
  GuideE->createAll(System,PitA->getKey("Mid"),2,PitA->getKey("Mid"),2);

  // runs backwards from guide to chopper
  GuidePitABack->addInsertCell(PitA->getCell("MidLayer"));
  GuidePitABack->addEndCut(PitA->getKey("Inner").getSignedLinkString(2));
  GuidePitABack->createAll(System,GuideE->getKey("Guide0"),-1,
			    GuideE->getKey("Guide0"),-1);



  // SECOND CHOPPER PIT:
  
  GuideD->addInsertCell(voidCell);
  GuideD->createAll(System,*BInsert,2,GuideC->getKey("Guide0"),2);
  
  return;
}


}   // NAMESPACE essSystem

