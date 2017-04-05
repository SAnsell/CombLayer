/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/HEIMDAL.cxx
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
#include "TwinChopper.h"
#include "DetectorTank.h"
#include "LineShield.h"
#include "Jaws.h"
#include "HoleShape.h"
#include "JawSet.h"
#include "CylSample.h"
#include "CrystalMount.h"
#include "TubeDetBox.h"
#include "Cryostat.h"

#include "HEIMDAL.h"

namespace essSystem
{

HEIMDAL::HEIMDAL(const std::string& keyName) :
  attachSystem::CopiedComp("heimdal",keyName),
  startPoint(0),stopPoint(0),
  heimdalAxis(new attachSystem::FixedOffset(newName+"Axis",4)),

  FocusTA(new beamlineSystem::GuideLine(newName+"FA")),
  FocusCA(new beamlineSystem::GuideLine(newName+"FA")),

  VPipeB(new constructSystem::VacuumPipe(newName+"PipeB")),
  FocusTB(new beamlineSystem::GuideLine(newName+"FTB")),
  FocusCB(new beamlineSystem::GuideLine(newName+"FCB")),

  VPipeC(new constructSystem::VacuumPipe(newName+"PipeC")),
  FocusTC(new beamlineSystem::GuideLine(newName+"FTC")),
  FocusCC(new beamlineSystem::GuideLine(newName+"FCC")),

  ChopA(new constructSystem::ChopperUnit(newName+"ChopE")),
  ADiskOne(new constructSystem::DiskChopper(newName+"ADiskOne")),
  ADiskTwo(new constructSystem::DiskChopper(newName+"ADiskTwo"))
  
 /*!
    Constructor
    \param keyName :: keyname for the beamline
 */
{
  ELog::RegMethod RegA("HEIMDAL","HEIMDAL");
 
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  // This is necessary as not directly constructed:
  OR.cell(newName+"Axis");
  OR.addObject(heimdalAxis);

  OR.addObject(FocusTA);
  OR.addObject(FocusCA);
  OR.addObject(VPipeB);
  OR.addObject(FocusCB);
  OR.addObject(FocusTB);
  OR.addObject(FocusCB);

  OR.addObject(VPipeC);
  OR.addObject(FocusTC);
  OR.addObject(FocusCC);

  OR.addObject(ChopA);
  OR.addObject(ADiskOne);
  OR.addObject(ADiskTwo);
}

  
HEIMDAL::~HEIMDAL()
  /*!
    Destructor
  */
{}

void
HEIMDAL::setBeamAxis(const FuncDataBase& Control,
		   const GuideItem& GItem,
                   const bool reverseZ)
  /*!
    Set the primary direction object
    \param Control :: Database for variables
    \param GItem :: Guide Item to 
    \param reverseZ :: Reverse axis
   */
{
  ELog::RegMethod RegA("HEIMDAL","setBeamAxis");

  heimdalAxis->populate(Control);
  heimdalAxis->createUnitVector(GItem);
  heimdalAxis->setLinkCopy(0,GItem.getKey("Main"),0);
  heimdalAxis->setLinkCopy(1,GItem.getKey("Main"),1);
  heimdalAxis->setLinkCopy(2,GItem.getKey("Beam"),0);
  heimdalAxis->setLinkCopy(3,GItem.getKey("Beam"),1);

  // BEAM needs to be shifted/rotated:
  heimdalAxis->linkShift(3);
  heimdalAxis->linkShift(4);
  heimdalAxis->linkAngleRotate(3);
  heimdalAxis->linkAngleRotate(4);

  if (reverseZ)
    heimdalAxis->reverseZ();
  return;
}

void
HEIMDAL::buildBunkerUnits(Simulation& System,
                        const attachSystem::FixedComp& FTA,
                        const long int thermalIndex,
                        const attachSystem::FixedComp& FCA,
                        const long int coldIndex,
                        const int bunkerVoid)
  /*!
    Build all the components in the bunker space
    \param System :: simulation
    \param FTA :: FixedComp for thermal guide
    \param thermalIndex :: FixedComp link point for thermal
    \param FCA :: Fixed component for cold guide
    \param coldIndex :: FixedComp link point for cold
    \param bunkerVoid :: cell to place objects in
   */
{
  ELog::RegMethod RegA("HEIMDAL","buildBunkerUnits");
  
  VPipeB->addInsertCell(bunkerVoid);
  VPipeB->createAll(System,FTA,thermalIndex);

  // Offset from VPipeB center
  FocusTB->addInsertCell(VPipeB->getCells("Void"));
  FocusTB->createAll(System,*VPipeB,0,*VPipeB,0);
  
  VPipeC->addInsertCell(bunkerVoid);
  VPipeC->createAll(System,FocusTB->getKey("Guide0"),2);

  FocusTC->addInsertCell(VPipeB->getCells("Void"));
  FocusTC->createAll(System,*VPipeC,0,*VPipeC,0);
                                                        
  //  FocusCB->addInsertCell(VPipeB->getCells("Void"));
  //  FocusCB->createAll(System,*VPipeB,0,*VPipeB,0);


  return;
}

void
HEIMDAL::buildOutGuide(Simulation& System,
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
  ELog::RegMethod RegA("HEIMDAL","buildOutGuide");

  return;
}

void
HEIMDAL::buildHut(Simulation& System,
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
  ELog::RegMethod RegA("HEIMDAL","buildHut");
  return;
}

void
HEIMDAL::buildDetectorArray(Simulation& System,
                          const attachSystem::FixedComp& sampleFC,
                          const long int sampleIndex,
                          const int voidCell)
  /*!
    Builds the detector array in the cave (relative to the sample)
    \param System :: Simulation to build with
    \param sampleFC :: Sample (centre) fixed object
    \param sampleIndex :: Index for the sample
    \param voidCell :: Cell everything is in
  */
{
  ELog::RegMethod RegA("HEIMDAL","buildDetectorArray");
  return;
}
  

  
void
HEIMDAL::buildIsolated(Simulation& System,const int voidCell)
  /*!
    Carry out the build in isolation
    \param System :: Simulation system
    \param voidCell :: void cell
   */
{
  ELog::RegMethod RegA("HEIMDAL","buildIsolated");


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
      //      buildBunkerUnits(System,*FStart,startIndex,voidCell);
      // Set the start point fo rb
      //      FStart= &(FocusF->getKey("Guide0"));
      startIndex= 2;
    }
  if (stopPoint==2 || stopPoint==1) return;

  if (startPoint<2)
    {
    }
  if (stopPoint==3) return;

  if (startPoint<3)
    {
    }

  if (stopPoint==4) return;      

  if (startPoint<4)
    {
    }
  
  return;
}

  
void 
HEIMDAL::build(Simulation& System,
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
  ELog::RegMethod RegA("HEIMDAL","build");

  ELog::EM<<"\nBuilding HEIMDAL on : "<<GItem.getKeyName()<<ELog::endDiag;

  const FuncDataBase& Control=System.getDataBase();
  CopiedComp::process(System.getDataBase());
  stopPoint=Control.EvalDefVar<int>(newName+"StopPoint",0);
  ELog::EM<<"GItem == "<<GItem.getKey("Beam").getSignedLinkPt(-1)
	  <<ELog::endDiag;

  setBeamAxis(Control,GItem,0);
    
  FocusTA->addInsertCell(GItem.getCells("Void"));
  FocusTA->setFront(GItem.getKey("Beam"),-1);
  FocusTA->setBack(GItem.getKey("Beam"),-2);
  FocusTA->createAll(System,*heimdalAxis,-3,*heimdalAxis,-3);

  FocusCA->addInsertCell(GItem.getCells("Void"));
  FocusCA->setFront(GItem.getKey("Beam"),-1);
  FocusCA->setBack(GItem.getKey("Beam"),-2);
  FocusCA->createAll(System,*heimdalAxis,-3,*heimdalAxis,-3);

  if (stopPoint==1) return;                      // STOP At monolith
                                                 // edge  
  buildBunkerUnits(System,FocusTA->getKey("Guide0"),2,
                   FocusTA->getKey("Guide0"),2,
                   bunkerObj.getCell("MainVoid"));

  if (stopPoint==2) return;                      // STOP At bunker edge

  // // IN WALL
  // // Make bunker insert
  // BInsert->createAll(System,FocusF->getKey("Guide0"),2,bunkerObj);
  // attachSystem::addToInsertSurfCtrl(System,bunkerObj,"frontWall",*BInsert);  

  // // using 7 : mid point
  // FocusWall->addInsertCell(BInsert->getCell("Void"));
  // FocusWall->createAll(System,*BInsert,7,*BInsert,7);
  
  if (stopPoint==3) return;                      // STOP At bunker exit
  //  buildOutGuide(System,FocusWall->getKey("Guide0"),2,voidCell);

  if (stopPoint==4) return;                      // STOP At hutch
  //  buildHut(System,ChopperOutB->getKey("Beam"),2,voidCell);
  
  return;
}


}   // NAMESPACE essSystem

