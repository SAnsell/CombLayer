/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File: R3Common/R3FrontEndToyama.cxx
 *
 * Copyright (c) 2004-2025 by Konstantin Batkov / Stuart Ansell
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
#include <array>

#include "Exception.h"
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
#include "FixedGroup.h"
#include "FixedOffset.h"
#include "FixedRotate.h"
#include "FixedRotateGroup.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "CopiedComp.h"
#include "BlockZone.h"
#include "generateSurf.h"
#include "generalConstruct.h"

#include "GeneralPipe.h"
#include "VacuumPipe.h"
#include "CornerPipe.h"
#include "insertObject.h"
#include "insertCylinder.h"
#include "Bellows.h"
#include "LCollimator.h"
#include "GateValveCube.h"
#include "CylGateValve.h"
#include "OffsetFlangePipe.h"
#include "portItem.h"
#include "VirtualTube.h"
#include "PipeTube.h"
#include "CrossPipe.h"
#include "SquareFMask.h"
#include "BeamMount.h"
#include "HeatDump.h"
#include "EPSeparator.h"
#include "R3ChokeChamber.h"
#include "R3ChokeInsert.h"
#include "EntryPipe.h"
#include "MagnetM1.h"
#include "MagnetU1.h"
#include "CleaningMagnet.h"
#include "BremBlock.h"
#include "LObjectSupport.h"
#include "MovableSafetyMask.h"
#include "HeatAbsorberToyama.h"
#include "ProximityShielding.h"
#include "R3FrontEnd.h"
#include "R3FrontEndToyama.h"

namespace xraySystem
{

// Note currently uncopied:

R3FrontEndToyama::R3FrontEndToyama(const std::string& Key) :
  R3FrontEnd(Key),
  bellowPreMSM(std::make_shared<constructSystem::Bellows>(newName+"BellowPreMSM")),
  msm(std::make_shared<xraySystem::MovableSafetyMask>(newName+"MSM")),
  bellowPostMSM(std::make_shared<constructSystem::Bellows>(newName+"BellowPostMSM")),
  msmExitPipe(new constructSystem::VacuumPipe(newName+"MSMExitPipe")),
  bellowPreHA(std::make_shared<constructSystem::Bellows>(newName+"BellowPreHA")),
  ha(std::make_shared<xraySystem::HeatAbsorberToyama>(newName+"HeatAbsorber")),
  bellowPostHA(std::make_shared<constructSystem::Bellows>(newName+"BellowPostHA")),
  bellowDA(std::make_shared<constructSystem::Bellows>(newName+"BellowDA")),
  bremColl(new xraySystem::BremBlock(newName+"BremColl")),
  bremCollPipe(new constructSystem::VacuumPipe(newName+"BremCollPipe")),
  proxiShieldA(new xraySystem::ProximityShielding(newName+"ProxiShieldA")),
  proxiShieldAPipe(new constructSystem::VacuumPipe(newName+"ProxiShieldAPipe")),
  proxiShieldB(new xraySystem::ProximityShielding(newName+"ProxiShieldB")),
  proxiShieldBPipe(new constructSystem::VacuumPipe(newName+"ProxiShieldBPipe"))

  /*!
    Constructor
    \param Key :: Name of construction key
    \param Index :: Index number
  */
{
  ModelSupport::objectRegister& OR = ModelSupport::objectRegister::Instance();

  // OR.addObject(transPipe);
  // OR.addObject(magBlockM1);
  // OR.addObject(magBlockU1);
  // OR.addObject(epSeparator);
  // OR.addObject(chokeChamber);
  // OR.addObject(chokeInsert);

  // OR.addObject(dipolePipe);
  // OR.addObject(eTransPipe);
  // OR.addObject(bellowA);
  // OR.addObject(collA);
  // OR.addObject(bellowB);
  // OR.addObject(collABPipe);
  // OR.addObject(bellowC);
  // OR.addObject(collB);
  // OR.addObject(collC);
  // OR.addObject(eCutDisk);
  // OR.addObject(eCutMagDisk);
  // OR.addObject(collExitPipe);
  // OR.addObject(heatBox);
  // OR.addObject(heatDump);

  // OR.addObject(pipeB);
  // OR.addObject(bellowE);
  // OR.addObject(aperturePipe);
  // OR.addObject(moveCollA);
  // OR.addObject(bellowF);
  // OR.addObject(ionPC);
  // OR.addObject(bellowG);
  // OR.addObject(aperturePipeB);
  // OR.addObject(moveCollB);
  // OR.addObject(bellowH);
  // OR.addObject(pipeC);

  // OR.addObject(gateA);
  // OR.addObject(bellowI);
  // OR.addObject(florTubeA);
  // OR.addObject(bellowJ);
  // OR.addObject(gateTubeB);
  // OR.addObject(offPipeA);
  // OR.addObject(shutterBox);
  // OR.addObject(shutters[0]);
  // OR.addObject(shutters[1]);
  // OR.addObject(offPipeB);
  OR.addObject(bellowPreMSM);
  OR.addObject(msm);
  OR.addObject(bellowPostMSM);
  OR.addObject(msmExitPipe);
  OR.addObject(bellowPreHA);
  OR.addObject(ha);
  OR.addObject(bellowPostHA);
  OR.addObject(bellowDA);
  OR.addObject(proxiShieldA);
  OR.addObject(proxiShieldAPipe);
  OR.addObject(bremColl);
  OR.addObject(bremCollPipe);
  OR.addObject(proxiShieldB);
  OR.addObject(proxiShieldBPipe);
}

R3FrontEndToyama::~R3FrontEndToyama()
  /*!
    Destructor
   */
{}

void
R3FrontEndToyama::populate(const FuncDataBase& Control)
  /*!
    Populate the intial values [movement]
    \param Control :: DataBase
   */
{
  FixedRotate::populate(Control);
  outerRadius=Control.EvalVar<double>(keyName+"OuterRadius");
  frontOffset=Control.EvalDefVar<double>(keyName+"FrontOffset",0.0);
  return;
}


void
R3FrontEndToyama::createSurfaces()
  /*!
    Create surfaces
  */
{
  ELog::RegMethod RegA("R3FrontEndToyama","createSurfaces");

  if (outerRadius>Geometry::zeroTol)
    {
      ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,outerRadius);
      buildZone.setSurround(HeadRule(-SMap.realSurf(buildIndex+7)));
    }

  if (!frontActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin+Y*frontOffset,Y);
      buildZone.initFront(HeadRule(SMap.realSurf(buildIndex+1)));
      setFront(-SMap.realSurf(buildIndex+1));
    }
  else
    buildZone.setFront(getFrontRule());

  return;
}

// void
// R3FrontEndToyama::insertFlanges(Simulation& System,
// 			  const constructSystem::PipeTube& PT,
// 			  const size_t offset)
//   /*!
//     Boilerplate function to insert the flanges from pipetubes
//     that extend past the linkzone in to ther neighbouring regions.
//     \param System :: Simulation to use
//     \param PT :: PipeTube
//    */
// {
//   ELog::RegMethod RegA("R3FrontEndToyama","insertFlanges");

//   size_t voidN=buildZone.getNItems("Unit");
//   if (voidN<offset)
//     throw ColErr::InContainerError<size_t>
//       (offset, "Offset to large for buildZone cells:"+std::to_string(voidN));
//   voidN-=offset;

//   buildZone.insertComponent(System,"Unit",voidN,
// 			PT.getFullRule("FlangeA"));
//   buildZone.insertComponent(System,"Unit",voidN,
// 			PT.getFullRule("FlangeB"));
//   buildZone.insertComponent(System,"Unit",voidN+2,
// 			PT.getFullRule("FlangeA"));
//   buildZone.insertComponent(System,"Unit",voidN+2,
// 			PT.getFullRule("FlangeB"));
//   return;
// }

void
R3FrontEndToyama::buildHeatTable(Simulation& System)
  /*!
    Build the heatDump table
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("R3FrontEndToyama","buildHeatTable");

  int outerCell;

  ha->createAll(System, *this, 0);

  bellowPreHA->createAll(System, *ha, "front");

  msmExitPipe->setBack(*bellowPreHA,"back");

  msmExitPipe->createAll(System,*bellowPostMSM,"back");
  outerCell=buildZone.createUnit(System,*msmExitPipe,"back");
  msmExitPipe->insertAllInCell(System,outerCell);

  outerCell=buildZone.createUnit(System,*ha,"#front");
  bellowPreHA->insertAllInCell(System,outerCell);

  outerCell=buildZone.createUnit(System,*ha,"back");
  ha->insertInCell(System,outerCell);

  constructSystem::constructUnit(System,buildZone,*ha,"back",*bellowPostHA);
  constructSystem::constructUnit(System,buildZone,*bellowPostHA,"back",*bellowD);

  constructSystem::constructUnit(System,buildZone,*bellowD,"back",*gateA);

  constructSystem::constructUnit(System,buildZone,*gateA,"back",*ionPB);

  constructSystem::constructUnit(System,buildZone,*ionPB,"back",*pipeB);

  constructSystem::constructUnit(System,buildZone,*pipeB,"back",*bellowDA);

  return;

}

void
R3FrontEndToyama::buildApertureTable(Simulation& System,
				   const attachSystem::FixedComp& preFC,
				   const long int preSideIndex)

  /*!
    Build the moveable aperature table
    \param System :: Simulation to use
    \param preFC :: Initial cell
    \param preSideIndex :: Initial side index
  */
{
  ELog::RegMethod RegA("R3FrontEndToyama","buildApertureTable");

  int outerCell;
  // NOTE order for master cell [Next 4 objects]
  aperturePipe->createAll(System,preFC,preSideIndex);
  moveCollA->addInsertCell(aperturePipe->getCell("Void"));
  moveCollA->createAll(System,*aperturePipe,"midPoint");

  // bellows AFTER movable aperture pipe
  bellowE->setFront(preFC,preSideIndex);
  bellowE->setBack(*aperturePipe,1);
  bellowE->createAll(System,preFC,preSideIndex);

  ionPC->createAll(System,preFC,preSideIndex);

  // bellows AFTER aperature ionpump and ion pump
  bellowF->setFront(*aperturePipe,2);
  bellowF->setBack(*ionPC,1);
  bellowF->createAll(System,preFC,preSideIndex);

  // now do insert:
  outerCell=buildZone.createUnit(System,*bellowE,2);
  bellowE->insertAllInCell(System,outerCell);

  outerCell=buildZone.createUnit(System,*aperturePipe,2);
  aperturePipe->insertAllInCell(System,outerCell);

  outerCell=buildZone.createUnit(System,*bellowF,2);
  bellowF->insertAllInCell(System,outerCell);

  outerCell=buildZone.createUnit(System,*ionPC,2);
  ionPC->insertInCell(System,outerCell);


  // Next 4 objects need to be build before insertion
  aperturePipeB->createAll(System,*ionPC,2);
  moveCollB->addInsertCell(aperturePipeB->getCell("Void"));
  moveCollB->createAll(System,*aperturePipeB,"midPoint");

  // bellows AFTER movable aperture pipe
  bellowG->setFront(*ionPC,2);
  bellowG->setBack(*aperturePipeB,1);
  bellowG->createAll(System,*ionPC,2);

  pipeC->createAll(System,*ionPC,2);

  // bellows AFTER movable aperture pipe
  bellowH->setFront(*aperturePipeB,2);
  bellowH->setBack(*pipeC,1);
  bellowH->createAll(System,*ionPC,2);


  // now do insert:
  outerCell=buildZone.createUnit(System,*bellowG,2);
  bellowG->insertAllInCell(System,outerCell);

  outerCell=buildZone.createUnit(System,*aperturePipeB,2);
  aperturePipeB->insertAllInCell(System,outerCell);

  outerCell=buildZone.createUnit(System,*bellowH,2);
  bellowH->insertAllInCell(System,outerCell);

  outerCell=buildZone.createUnit(System,*pipeC,2);
  pipeC->insertAllInCell(System,outerCell);


  return;
}

void
R3FrontEndToyama::buildShutterTable(Simulation& System,
			      const attachSystem::FixedComp& preFC,
			      const std::string& preSide)
  /*!
    Build the shutter block table
    \param System :: Simulation to use
    \param preFC :: initial Fixedcomp
    \param preSide :: link point on initial FC
  */
{
  ELog::RegMethod RegA("R3FrontEndToyama","buildShutterTable");
  int outerCell;

  // bellows
  constructSystem::constructUnit(System,buildZone,preFC,preSide,*bellowI);

  florTubeA->setPortRotation(3,Geometry::Vec3D(1,0,0));
  florTubeA->createAll(System,*bellowI,2);
  const constructSystem::portItem& FPI=florTubeA->getPort(1);
  outerCell=buildZone.createUnit(System,
				 FPI,FPI.getSideIndex("OuterPlate"));

  florTubeA->insertAllInCell(System,outerCell);

  // bellows
  bellowJ->createAll(System,FPI,FPI.getSideIndex("OuterPlate"));
  outerCell=buildZone.createUnit(System,*bellowJ,2);
  bellowJ->insertAllInCell(System,outerCell);

  insertFlanges(System,*florTubeA,3);

  constructSystem::constructUnit(System,buildZone,*bellowJ,"back",*gateTubeB);

  // Broximity shielding A
  proxiShieldAPipe->createAll(System,*gateTubeB,"back");
  constructSystem::pipeMagUnit(System,buildZone,proxiShieldAPipe,"#front","outerPipe",proxiShieldA);
  constructSystem::pipeTerminate(System,buildZone,proxiShieldAPipe);

  constructSystem::constructUnit(System,buildZone,*proxiShieldAPipe,"back",*offPipeA);

  //  insertFlanges(System,*gateTubeB);

  shutterBox->createAll(System,*offPipeA,"FlangeBCentre");
  outerCell=buildZone.createUnit(System,*shutterBox,"back");

  shutterBox->splitVoidPorts
    (System,"SplitVoid",1001,shutterBox->getCell("Void"),{0,1});


  shutterBox->splitVoidPorts(System,"SplitOuter",2001,
			       outerCell,{0,1});
  shutterBox->insertMainInCell(System,shutterBox->getCells("SplitOuter"));
  shutterBox->insertPortInCell(System,0,shutterBox->getCell("SplitOuter",0));
  shutterBox->insertPortInCell(System,1,shutterBox->getCell("SplitOuter",1));



  for(size_t i=0;i<shutters.size();i++)
    {
      const constructSystem::portItem& PI=shutterBox->getPort(i);
      shutters[i]->addInsertCell("Support",PI.getCell("Void"));
      shutters[i]->addInsertCell("Support",shutterBox->getCell("SplitVoid",i));
      shutters[i]->addInsertCell("Block",shutterBox->getCell("SplitVoid",i));

      shutters[i]->createAll(System,*offPipeA,
			     offPipeA->getSideIndex("FlangeACentre"),
			     PI,PI.getSideIndex("InnerPlate"));
    }

  constructSystem::constructUnit
    (System,buildZone,*shutterBox,"back",*offPipeB);

  return;
}

void
R3FrontEndToyama::buildMSM(Simulation& System,
			   const attachSystem::FixedComp& preFC,
			   const std::string& preSide)
/*!
  Build the Movable Safety Mask
  \param System :: Simulation to use
  \param preFC :: initial Fixedcomp
  \param preSide :: link point on initial FC
*/
{
  ELog::RegMethod RegA("R3FrontEndToyama","buildMSM");
  int outerCell;

  ELog::EM << "MSM is always built for Toyama front-ends" << ELog::endWarn;

  msm->createAll(System, *this, 0);
  bellowPreMSM->createAll(System, *msm, "front");

  collExitPipe->setBack(*bellowPreMSM,"back");

  collExitPipe->createAll(System,*collB,"back");
  outerCell=buildZone.createUnit(System,*collExitPipe,"back");
  collExitPipe->insertAllInCell(System,outerCell);

  outerCell=buildZone.createUnit(System,*msm,"#front");
  bellowPreMSM->insertAllInCell(System,outerCell);

  outerCell=buildZone.createUnit(System,*msm,"back");
  msm->insertInCell(System,outerCell);

  constructSystem::constructUnit(System,buildZone,*msm,"back",*bellowPostMSM);

}

void
R3FrontEndToyama::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("R3FrontEndToyama","buildObjects");

  int outerCell;

  buildZone.addInsertCells(this->getInsertCells());

  const attachSystem::FixedComp& undulatorFC=
    buildUndulator(System,*this,0);

  outerCell=buildZone.createUnit(System,undulatorFC,"back");

  magBlockM1->createAll(System,*this,0);


  transPipe->setCutSurf("front",undulatorFC,2);
  transPipe->setCutSurf("back",*magBlockM1,1);
  transPipe->createAll(System,undulatorFC,2);

  outerCell=buildZone.createUnit(System,*transPipe,2);
  transPipe->insertAllInCell(System,outerCell);

  outerCell=buildZone.createUnit(System,*magBlockM1,2);
  magBlockM1->insertAllInCell(System,outerCell);

  epSeparator->setCutSurf("front",*magBlockM1,2);
  epSeparator->setEPOriginPair(*magBlockM1,"Photon","Electron");
  epSeparator->createAll(System,*magBlockM1,2);
  outerCell=buildZone.createUnit(System,*epSeparator,2);
  epSeparator->insertInCell(System,outerCell);

  chokeChamber->setCutSurf("front",*epSeparator,2);
  chokeChamber->setEPOriginPair(*epSeparator,"Photon","Electron");
  chokeChamber->createAll(System,*epSeparator,2);
  outerCell=buildZone.createUnit(System,*chokeChamber,2);
  chokeChamber->insertAllInCell(System,outerCell);
  chokeChamber->setCell("BlockOuter",outerCell);

  chokeInsert->setCutSurf("front",*chokeChamber,"innerSide");
  chokeInsert->addInsertCell(chokeChamber->getCell("MainVoid"));
  chokeInsert->addInsertCell(chokeChamber->getCell("SideVoid"));
  chokeInsert->createAll(System,*chokeChamber,"innerSide");

  eCutDisk->setNoInsert();
  eCutDisk->addInsertCell(chokeChamber->getCell("PhotonVoid"));
  eCutDisk->createAll(System,*chokeChamber,
		      chokeChamber->getSideIndex("-photon"));

  // FM1 Built relateive to MASTER coordinate
  collA->createAll(System,*this,0);
  bellowA->createAll(System,*collA,1);

  dipolePipe->setFront(*chokeChamber,"photon");
  dipolePipe->setBack(*bellowA,"back");
  dipolePipe->createAll(System,*chokeChamber,"photon");
  outerCell=buildZone.createUnit(System,*dipolePipe,2);
  dipolePipe->insertAllInCell(System,outerCell);
  buildZone.addCell("dipoleUnit",outerCell);

  magBlockU1->createAll(System,*epSeparator,"Electron");
  magBlockU1->insertAllInCell(System,buildZone.getCell("dipoleUnit"));
  magBlockU1->insertDipolePipe(System,*dipolePipe);

  const xraySystem::EntryPipe& entryPipe=
    magBlockU1->getEntryPipe();
  eCutMagDisk->setNoInsert();
  eCutMagDisk->addInsertCell(entryPipe.getCell("Void"));
  eCutMagDisk->addInsertCell(entryPipe.getCell("Wall"));
  eCutMagDisk->createAll(System,entryPipe,"-back");

  eTransPipe->setFront(*chokeChamber,"electron");
  eTransPipe->setBack(*magBlockU1,"voidFront");
  eTransPipe->createAll(System,*chokeChamber,"electron");
  eTransPipe->insertInCell("FlangeA",System,
			   chokeChamber->getCell("PhotonOuterVoid"));
  eTransPipe->insertInCell("FlangeA",System,
			   chokeChamber->getCell("BlockOuter"));
  eTransPipe->insertInCell("Main",System,
			   chokeChamber->getCell("BlockOuter"));
  eTransPipe->insertInCell("Main",System,outerCell);
  eTransPipe->insertInCell("FlangeB",System,outerCell);

  outerCell=buildZone.createUnit(System,*bellowA,1);
  bellowA->insertAllInCell(System,outerCell);

  outerCell=buildZone.createUnit(System,*collA,2);
  collA->insertInCell(System,outerCell);


  if (stopPoint=="Dipole")
    {
      lastComp=dipolePipe;
      setCell("MasterVoid",outerCell);
      return;
    }

  constructSystem::constructUnit
    (System,buildZone,*collA,"back",*bellowB);

  // FM2 Built relateive to MASTER coordinate

  collB->createAll(System,*this,0);
  bellowC->createAll(System,*collB,1);

  collABPipe->setFront(*bellowB,2);
  collABPipe->setBack(*bellowC,2);
  collABPipe->createAll(System,*bellowB,"back");

  constructSystem::pipeMagUnit(System,buildZone,collABPipe,"#front","outerPipe",pMag);
  constructSystem::pipeTerminate(System,buildZone,collABPipe);

  outerCell=buildZone.createUnit(System,*bellowC,1);
  bellowC->insertAllInCell(System,outerCell);

  outerCell=buildZone.createUnit(System,*collB,2);
  collB->insertInCell(System,outerCell);

  std::shared_ptr<attachSystem::FixedComp> linkFC(collB);
  // We never have FM3 in Toyama front-ends
  // if (collFM3Active)
  //   {
  //     constructSystem::constructUnit
  // 	(System,buildZone,*collB,"back",*collC);
  //     linkFC=collC;
  //   }

  collExitPipe->setFront(*linkFC,2);

  buildMSM(System,*collExitPipe,"back");
  buildHeatTable(System);
  buildApertureTable(System,*bellowDA,2);
  buildShutterTable(System,*pipeC,"back");

  // Bremsstrahlung collimator
  constructSystem::constructUnit(System,buildZone,*offPipeB,"back",*bremCollPipe);
  bremColl->addInsertCell(bremCollPipe->getCell("Void"));
  bremColl->addInsertCell(bremCollPipe->getCell("FlangeAInnerVoid"));
  bremColl->addInsertCell(bremCollPipe->getCell("FlangeBInnerVoid"));
  bremColl->createAll(System,*bremCollPipe,0);

  // Proximity shielding B
  proxiShieldBPipe->createAll(System,*bremCollPipe,"back");
  constructSystem::pipeMagUnit(System,buildZone,proxiShieldBPipe,"#front","outerPipe",proxiShieldB);
  constructSystem::pipeTerminate(System,buildZone,proxiShieldBPipe);

  if (ExternalCut::isActive("REWall"))
    {
      buildZone.setMaxExtent(getRule("REWall"));
      outerCell=buildZone.createUnit(System);
    }

  buildZone.rebuildInsertCells(System);

  buildExtras(System);
  setCell("MasterVoid",outerCell);
  lastComp=proxiShieldBPipe;

  return;
}


void
R3FrontEndToyama::createAll(Simulation& System,
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
  ELog::RegMethod RControl("R3FrontEndToyama","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  buildObjects(System);
  createLinks();

  return;
}


}   // NAMESPACE xraySystem
