/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/ESTIA.cxx
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
#include "Jaws.h"
#include "GuideLine.h"
#include "DiskChopper.h"
#include "VacuumBox.h"
#include "VacuumPipe.h"
#include "ChopperHousing.h"
#include "Bunker.h"
#include "BunkerInsert.h"
#include "ChopperPit.h"
#include "DHut.h"
#include "DetectorTank.h"
#include "CylSample.h"
#include "LineShield.h"

#include "ESTIA.h"

namespace essSystem
{

ESTIA::ESTIA(const std::string& keyName) :
  attachSystem::CopiedComp("estia",keyName),
  stopPoint(0),
  estiaAxis(new attachSystem::FixedOffset(newName+"Axis",4)),
  FocusMono(new beamlineSystem::GuideLine(newName+"FMono")),
  VPipeA(new constructSystem::VacuumPipe(newName+"PipeA")),
  FocusA(new beamlineSystem::GuideLine(newName+"FA")),

  VPipeB(new constructSystem::VacuumPipe(newName+"PipeB")),
  VacBoxA(new constructSystem::VacuumBox(newName+"VBoxA")),
  FocusB(new beamlineSystem::GuideLine(newName+"FB"))
  /*! 
    Constructor
    \param keyName :: main beamline name
  */
{
  ELog::RegMethod RegA("ESTIA","ESTIA");
 
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  // This is necessary:
  OR.cell(newName+"Axis");
  OR.addObject(estiaAxis);

  OR.addObject(FocusMono);
  OR.addObject(FocusA);
  OR.addObject(VPipeA);

  OR.addObject(FocusB);
  OR.addObject(VPipeB);
}

ESTIA::~ESTIA()
  /*!
    Destructor
  */
{}


void
ESTIA::buildChopperBlock(Simulation& System,
			 const Bunker& bunkerObj,
			 const attachSystem::FixedComp& prevFC,
			 const constructSystem::VacuumBox& prevVacBox,
			 constructSystem::VacuumBox& VacBox,
			 beamlineSystem::GuideLine& GL,
			 constructSystem::DiskChopper& Disk,
			 constructSystem::ChopperHousing& House,
			 constructSystem::VacuumPipe& Pipe)
  /*!
    Build a chopper block [about to move to some higher level]
    \param System :: Simulation 
    \param bunkerObj :: Object
    \param prevFC :: FixedComponent for like point [uses side 2]
    \param GL :: Guide Line 
  */
{
  ELog::RegMethod RegA("ESTIA","buildChopperBlock");
  
  // Box for BandA Disk
  VacBox.addInsertCell(bunkerObj.getCell("MainVoid"));
  VacBox.createAll(System,prevFC,2);

  // Double disk T0 chopper
  Disk.addInsertCell(VacBox.getCell("Void",0));
  Disk.setCentreFlag(3);  // Z direction
  Disk.createAll(System,VacBox,0);

  // Double disk chopper housing
  House.addInsertCell(VacBox.getCells("Void"));
  House.addInsertCell(VacBox.getCells("Box"));  // soon to become lid
  House.addInsertCell(bunkerObj.getCell("MainVoid"));
  House.createAll(System,Disk.getKey("Main"),0);
  House.insertComponent(System,"Void",Disk);

  Pipe.addInsertCell(bunkerObj.getCell("MainVoid"));
  Pipe.setFront(prevVacBox,2);
  Pipe.setBack(VacBox,1);
  Pipe.createAll(System,prevVacBox,2);
  
  GL.addInsertCell(Pipe.getCells("Void"));
  GL.addInsertCell(prevVacBox.getCells("Void"));
  GL.addInsertCell(VacBox.getCells("Void"));
  GL.createAll(System,prevFC,2,prevFC,2);
  return;
} 


  
void 
ESTIA::build(Simulation& System,
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
  ELog::RegMethod RegA("ESTIA","build");

  const FuncDataBase& Control=System.getDataBase();
  CopiedComp::process(System.getDataBase());  

  ELog::EM<<"\nBuilding ESTIA on : "<<GItem.getKeyName()<<ELog::endDiag;

  stopPoint=Control.EvalDefVar<int>(newName+"StopPoint",0);
  essBeamSystem::setBeamAxis(*estiaAxis,System.getDataBase(),GItem,1);

  FocusMono->addInsertCell(GItem.getCells("Void"));
  FocusMono->setBack(GItem.getKey("Beam").getLinkString(-2));
  FocusMono->createAll(System,*estiaAxis,-3,*estiaAxis,-3);

  // Shutter pipe [note gap front/back]
  VPipeA->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeA->createAll(System,FocusMono->getKey("Guide0"),2);

  FocusA->addInsertCell(VPipeA->getCells("Void"));
  FocusA->createAll(System,FocusMono->getKey("Guide0"),2,
		    FocusMono->getKey("Guide0"),2);

  // pipe for first section
  VPipeB->addInsertCell(bunkerObj.getCell("MainVoid"));
  VPipeB->createAll(System,FocusA->getKey("Guide0"),2);

  return;
  FocusB->addInsertCell(VPipeA->getCells("Void"));
  FocusB->createAll(System,FocusA->getKey("Guide0"),2,
		    FocusA->getKey("Guide0"),2);

  return;
}


}   // NAMESPACE essSystem

