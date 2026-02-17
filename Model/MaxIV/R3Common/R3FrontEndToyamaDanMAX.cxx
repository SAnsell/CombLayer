/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File: R3Common/R3FrontEndToyamaDanMAX.cxx
 *
 * Copyright (c) 2004-2026 by Konstantin Batkov / Stuart Ansell
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
#include "HeatAbsorberR3Toyama.h"
#include "ProximityShielding.h"
#include "FlangePlate.h"
#include "BladeBPMToyama.h"

#include "R3FrontEnd.h"
#include "R3FrontEndToyamaDanMAX.h"

namespace xraySystem
{

// Note currently uncopied:

R3FrontEndToyamaDanMAX::R3FrontEndToyamaDanMAX(const std::string& Key) :
  R3FrontEnd(Key),
  pipeA(std::make_shared<constructSystem::VacuumPipe>(newName+"PumpingUnit1ReplacementPipe")), // TODO: pipeA currently replaces PumpingUnit1
  xbpm1(std::make_shared<xraySystem::BladeBPMToyama>(newName+"XBPM1")),
  flangePlateAA(std::make_shared<constructSystem::FlangePlate>(newName+"FlangePlateAA")),
  flangePlateA(std::make_shared<constructSystem::FlangePlate>(newName+"FlangePlateA")),
  flangePlateB(std::make_shared<constructSystem::FlangePlate>(newName+"FlangePlateB")),
  bellowPreMSM(std::make_shared<constructSystem::Bellows>(newName+"BellowPreMSM")),
  msm(std::make_shared<xraySystem::MovableSafetyMask>(newName+"MSM")),
  bellowPostMSM(std::make_shared<constructSystem::Bellows>(newName+"BellowPostMSM")),
  msmExitPipe(new constructSystem::VacuumPipe(newName+"MSMExitPipe")),
  flangePlateC(std::make_shared<constructSystem::FlangePlate>(newName+"FlangePlateC")),
  flangePlateD(std::make_shared<constructSystem::FlangePlate>(newName+"FlangePlateD")),
  flangePlateE(std::make_shared<constructSystem::FlangePlate>(newName+"FlangePlateE")),
  bellowPreHA(std::make_shared<constructSystem::Bellows>(newName+"BellowPreHA")),
  ha(std::make_shared<xraySystem::HeatAbsorberR3Toyama>(newName+"HeatAbsorber")),
  bellowPostHA(std::make_shared<constructSystem::Bellows>(newName+"BellowPostHA")),
  ionPump3(new constructSystem::CrossPipe(newName+"IonPump3")),
  valve2(std::make_shared<xraySystem::CylGateValve>(newName+"Valve2")),
  bellowDA(std::make_shared<constructSystem::Bellows>(newName+"BellowDA")),
  flangePlateF(std::make_shared<constructSystem::FlangePlate>(newName+"FlangePlateF")),
  flangePlateG(std::make_shared<constructSystem::FlangePlate>(newName+"FlangePlateG")),
  bremColl(new xraySystem::BremBlock(newName+"BremColl")),
  bremCollPipe(new constructSystem::VacuumPipe(newName+"BremCollPipe")),
  proxiShieldA(new xraySystem::ProximityShielding(newName+"ProxiShieldA")),
  proxiShieldAPipe(new constructSystem::VacuumPipe(newName+"ProxiShieldAPipe")),
  proxiShieldB(new xraySystem::ProximityShielding(newName+"ProxiShieldB")),
  proxiShieldBPipe(new constructSystem::VacuumPipe(newName+"ProxiShieldBPipe")),
  msmActive(false)

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
  // OR.addObject(fm1);
  // OR.addObject(bellowB);
  // OR.addObject(collABPipe);
  // OR.addObject(bellowC);
  // OR.addObject(fm2);
  // OR.addObject(fm3);
  // OR.addObject(eCutDisk);
  // OR.addObject(eCutMagDisk);
  // OR.addObject(collExitPipe);
  // OR.addObject(heatBox);
  // OR.addObject(heatDump);

  // OR.addObject(pipeB);
  // OR.addObject(bellowE);
  // OR.addObject(aperturePipeA);
  // OR.addObject(moveCollA);
  // OR.addObject(bellowF);
  // OR.addObject(pump4);
  // OR.addObject(bellowG);
  // OR.addObject(aperturePipeB);
  // OR.addObject(moveCollB);
  // OR.addObject(bellowH);
  // OR.addObject(pipeC);

  // OR.addObject(gateA);
  // OR.addObject(bellowI);
  // OR.addObject(florTubeA);
  // OR.addObject(bellowJ);
  // OR.addObject(valve3);
  // OR.addObject(offPipeA);
  // OR.addObject(shutterBox);
  // OR.addObject(shutters[0]);
  // OR.addObject(shutters[1]);
  // OR.addObject(offPipeB);
  OR.addObject(pipeA);
  OR.addObject(xbpm1);
  OR.addObject(flangePlateAA);
  OR.addObject(flangePlateA);
  OR.addObject(flangePlateB);
  OR.addObject(bellowPreMSM);
  OR.addObject(msm);
  OR.addObject(bellowPostMSM);
  OR.addObject(msmExitPipe);
  OR.addObject(flangePlateC);
  OR.addObject(flangePlateD);
  OR.addObject(flangePlateE);
  OR.addObject(bellowPreHA);
  OR.addObject(ha);
  OR.addObject(bellowPostHA);
  OR.addObject(ionPump3);
  OR.addObject(valve2);
  OR.addObject(bellowDA);
  OR.addObject(flangePlateF);
  OR.addObject(flangePlateG);
  OR.addObject(bremColl);
  OR.addObject(bremCollPipe);
  OR.addObject(proxiShieldA);
  OR.addObject(proxiShieldAPipe);
  OR.addObject(proxiShieldB);
  OR.addObject(proxiShieldBPipe);
}

R3FrontEndToyamaDanMAX::~R3FrontEndToyamaDanMAX()
  /*!
    Destructor
   */
{}

void
R3FrontEndToyamaDanMAX::populate(const FuncDataBase& Control)
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
R3FrontEndToyamaDanMAX::createSurfaces()
  /*!
    Create surfaces
  */
{
  ELog::RegMethod RegA("R3FrontEndToyamaDanMAX","createSurfaces");

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
// R3FrontEndToyamaDanMAX::insertFlanges(Simulation& System,
// 			  const constructSystem::PipeTube& PT,
// 			  const size_t offset)
//   /*!
//     Boilerplate function to insert the flanges from pipetubes
//     that extend past the linkzone in to ther neighbouring regions.
//     \param System :: Simulation to use
//     \param PT :: PipeTube
//    */
// {
//   ELog::RegMethod RegA("R3FrontEndToyamaDanMAX","insertFlanges");

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
R3FrontEndToyamaDanMAX::buildSupport5(Simulation& System,
				 const attachSystem::FixedComp& preFC,
				 const std::string& preSide)
  /*!
    Build Support5 - the Heat Absorber table
    \param System :: Simulation to use
    \param preFC :: initial Fixedcomp
    \param preSide :: link point on initial FC
  */
{
  ELog::RegMethod RegA("R3FrontEndToyamaDanMAX","buildSupport5");

  int outerCell;

  outerCell=buildZone.createUnit(System,*ha,"#front");
  bellowPreHA->insertAllInCell(System,outerCell);

  outerCell=buildZone.createUnit(System,*ha,"back");
  ha->insertInCell(System,outerCell);

  constructSystem::constructUnit(System,buildZone,*ha,"back",*bellowPostHA);
  constructSystem::constructUnit(System,buildZone,*bellowPostHA,"back",*flangePlateE);
  constructSystem::constructUnit(System,buildZone,*flangePlateE,"back",*bellowD);

  constructSystem::constructUnit(System,buildZone,*bellowD,"back",*valve1);

  constructSystem::constructUnit(System,buildZone,*valve1,"back",*ionPump3);

  constructSystem::constructUnit(System,buildZone,*ionPump3,"back",*pipeB);

  constructSystem::constructUnit(System,buildZone,*pipeB,"back",*valve2);

  return;

}

void
R3FrontEndToyamaDanMAX::buildSupport6(Simulation& System,
				   const attachSystem::FixedComp& preFC,
				   const long int preSideIndex)

  /*!
    Build Support6 - the table hosting both movable masks
    \param System :: Simulation to use
    \param preFC :: Initial Fixed Component
    \param preSideIndex :: Initial FC side index
  */
{
  ELog::RegMethod RegA("R3FrontEndToyamaDanMAX","buildSupport6");

  constructSystem::constructUnit(System,buildZone,preFC,"back",*flangePlateF);
  constructSystem::constructUnit(System,buildZone,*flangePlateF,"back",*bellowE);
  constructSystem::constructUnit(System,buildZone,*bellowE,"back",*aperturePipeA);
  moveCollA->addInsertCell(aperturePipeA->getCell("Void"));
  moveCollA->createAll(System,*aperturePipeA,"midPoint");
  constructSystem::constructUnit(System,buildZone,*aperturePipeA,"back",*bellowF);
  constructSystem::constructUnit(System,buildZone,*bellowF,"back",*pump4);
  constructSystem::constructUnit(System,buildZone,*pump4,"back",*bellowG);
  constructSystem::constructUnit(System,buildZone,*bellowG,"back",*aperturePipeB);
  moveCollB->addInsertCell(aperturePipeB->getCell("Void"));
  moveCollB->createAll(System,*aperturePipeB,"midPoint");
  constructSystem::constructUnit(System,buildZone,*aperturePipeB,"back",*bellowH);

  // // bellows AFTER movable aperture pipe
  // bellowE->setFront(preFC,preSideIndex);
  // bellowE->setBack(*aperturePipeA,1);
  // bellowE->createAll(System,preFC,preSideIndex);

  // pump4->createAll(System,preFC,preSideIndex);

  // bellows AFTER aperature ionpump and ion pump
  // bellowF->setFront(*aperturePipeA,2);
  // bellowF->setBack(*pump4,1);
  // bellowF->createAll(System,preFC,preSideIndex);

  // now do insert:
  // outerCell=buildZone.createUnit(System,*bellowE,2);
  // bellowE->insertAllInCell(System,outerCell);

  // outerCell=buildZone.createUnit(System,*aperturePipeA,2);
  // aperturePipeA->insertAllInCell(System,outerCell);

  // outerCell=buildZone.createUnit(System,*bellowF,2);
  // bellowF->insertAllInCell(System,outerCell);

  // outerCell=buildZone.createUnit(System,*pump4,2);
  // pump4->insertInCell(System,outerCell);


  // Next 4 objects need to be build before insertion
  // aperturePipeB->createAll(System,*pump4,2);

  // bellows AFTER movable aperture pipe
  // bellowG->setFront(*pump4,2);
  // bellowG->setBack(*aperturePipeB,1);
  // bellowG->createAll(System,*pump4,2);

  // pipeC->createAll(System,*pump4,2);

  // // bellows AFTER movable aperture pipe
  // bellowH->setFront(*aperturePipeB,2);
  // bellowH->setBack(*pipeC,1);
  // bellowH->createAll(System,*pump4,2);


  // now do insert:
  // outerCell=buildZone.createUnit(System,*bellowG,2);
  // bellowG->insertAllInCell(System,outerCell);

  // outerCell=buildZone.createUnit(System,*aperturePipeB,2);
  // aperturePipeB->insertAllInCell(System,outerCell);

  // outerCell=buildZone.createUnit(System,*bellowH,2);
  // bellowH->insertAllInCell(System,outerCell);

  // outerCell=buildZone.createUnit(System,*pipeC,2);
  // pipeC->insertAllInCell(System,outerCell);



  return;
}

void
R3FrontEndToyamaDanMAX::buildSupport7(Simulation& System)
  /*!
    Build Support7 - the shutter block table.
    BellowI y-offset is an external variable
    (shutter table is fixed and can't move)
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("R3FrontEndToyamaDanMAX","buildSupport7");
  int outerCell;


  bellowI->createAll(System,*this, 0);

  pipeC->setFront(*flangePlateG, 2);
  pipeC->setBack(*bellowI, 1);
  pipeC->createAll(System,*flangePlateG, 2);
  outerCell=buildZone.createUnit(System,*bellowI,"#front");
  pipeC->insertAllInCell(System,outerCell);

  outerCell=buildZone.createUnit(System,*bellowI,"back");
  bellowI->insertAllInCell(System,outerCell);

  florTubeA->setPortRotation(3,Geometry::Vec3D(1,0,0));
  florTubeA->createAll(System,*bellowI,2);
  const constructSystem::portItem& FPI=florTubeA->getPort(1);
  outerCell=buildZone.createUnit(System,
				 FPI,FPI.getSideIndex("OuterPlate"));

  florTubeA->insertAllInCell(System,outerCell);
  florTubeA->intersectPorts(System,0,3);
  florTubeA->intersectPorts(System,1,3);
  florTubeA->intersectPorts(System,0,2);
  florTubeA->intersectPorts(System,1,2);


  // bellows
  bellowJ->createAll(System,FPI,FPI.getSideIndex("OuterPlate"));
  outerCell=buildZone.createUnit(System,*bellowJ,2);
  bellowJ->insertAllInCell(System,outerCell);

  insertFlanges(System,*florTubeA,3);

  constructSystem::constructUnit(System,buildZone,*bellowJ,"back",*valve3);

  // Broximity shielding A
  proxiShieldAPipe->createAll(System,*valve3,"back");
  constructSystem::pipeMagUnit(System,buildZone,proxiShieldAPipe,"#front","outerPipe",proxiShieldA);
  constructSystem::pipeTerminate(System,buildZone,proxiShieldAPipe);

  constructSystem::constructUnit(System,buildZone,*proxiShieldAPipe,"back",*offPipeA);

  //  insertFlanges(System,*valve3);

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
R3FrontEndToyamaDanMAX::buildMSM(Simulation& System,
			   attachSystem::FixedComp& preFC,
			   const std::string& preSide)
/*!
  Build the Movable Safety Mask
  \param System :: Simulation to use
  \param preFC :: initial Fixedcomp
  \param preSide :: link point on initial FC
*/
{
  ELog::RegMethod RegA("R3FrontEndToyamaDanMAX","buildMSM");
  int outerCell;

  msm->createAll(System, *this, 0);
  bellowPreMSM->createAll(System, *msm, "front");

  auto p = dynamic_cast<attachSystem::FrontBackCut*>(std::addressof(preFC));
  //  p->setFront(*bellowCA,2);  // build it later (setBack depending on the msmActive flag)
  p->setBack(*bellowPreMSM,"back");

  preFC.createAll(System,*flangePlateD,"back");
  outerCell=buildZone.createUnit(System,preFC,"back");
  dynamic_cast<attachSystem::ContainedGroup*>(std::addressof(preFC))->insertAllInCell(System,outerCell);

  outerCell=buildZone.createUnit(System,*msm,"#front");
  bellowPreMSM->insertAllInCell(System,outerCell);

  outerCell=buildZone.createUnit(System,*msm,"back");
  msm->insertInCell(System,outerCell);

  constructSystem::constructUnit(System,buildZone,*msm,"back",*bellowPostMSM);

}

void
R3FrontEndToyamaDanMAX::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("R3FrontEndToyamaDanMAX","buildObjects");

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

  // if (stopPoint == "TransPipe") { // TODO: remove the downstream M1 from geometry (errors)
  //   lastComp=transPipe;
  //   setCell("MasterVoid", outerCell);
  //   return;
  // }

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

  if (stopPoint=="ChokeChamber") {
    lastComp=chokeChamber;
    setCell("MasterVoid",outerCell);
    return;
  }

  eCutDisk->setNoInsert();
  eCutDisk->addInsertCell(chokeChamber->getCell("PhotonVoid"));
  eCutDisk->createAll(System,*chokeChamber,
		      chokeChamber->getSideIndex("-photon"));

  if (stopPoint != "U1Block") {
  // FM1 Built relateive to MASTER coordinate
    fm1->createAll(System,*this,0);
    flangePlateA->createAll(System,*fm1,"front");
    bellowA->createAll(System,*flangePlateA,"back");
  }

  magBlockU1->createAll(System,*epSeparator,"Electron");

  dipolePipe->setFront(*chokeChamber,"photon");
  if (stopPoint != "U1Block")
    dipolePipe->setBack(*bellowA,"back");
  else {
    dipolePipe->setBack(*magBlockU1,-4);
    dipolePipe->insertInCell("FlangeB",System,magBlockU1->getCell("BackVoid"));
  }

  dipolePipe->createAll(System,*chokeChamber,"photon");

  //  magBlockU1->insertAllInCell(System,buildZone.getCell("dipoleUnit"));
  magBlockU1->insertDipolePipe(System,*dipolePipe);

  outerCell=buildZone.createUnit(System,*magBlockU1,-3);
  buildZone.addCell("preM1",outerCell);
  dipolePipe->insertInCell("FlangeA",System,outerCell);
  dipolePipe->insertInCell("Main",System,outerCell);

  outerCell=buildZone.createUnit(System,*magBlockU1,4);
  //  magBlockU1->insertAllInCell(System,outerCell); // TODO: simplify
  magBlockU1->insertInCell("Main",System,outerCell);

  if (stopPoint=="U1Block") {
    // optional, to simplify external void even further
    // if (ExternalCut::isActive("REWall"))
    //   {
    // 	buildZone.setMaxExtent(getRule("REWall"));
    // 	outerCell=buildZone.createUnit(System);
    //   }
    buildZone.rebuildInsertCells(System); // simplifies external void
    buildExtras(System);
    setCell("MasterVoid",outerCell);
    lastComp=magBlockU1;
    return;
  }

  const xraySystem::EntryPipe& entryPipe=
    magBlockU1->getEntryPipe();
  eCutMagDisk->setNoInsert();
  eCutMagDisk->addInsertCell(entryPipe.getCell("Void"));
  eCutMagDisk->addInsertCell(entryPipe.getCell("Wall"));
  eCutMagDisk->createAll(System,entryPipe,"-back");

  eTransPipe->setFront(*chokeChamber,"electron");
  eTransPipe->setBack(*magBlockU1,"voidFront");
  eTransPipe->createAll(System,*chokeChamber,"electron");
  // can be commented out if ETransPipeOuterVoid is set
  // eTransPipe->insertInCell("FlangeA",System,chokeChamber->getCell("PhotonOuterVoid"));
  // eTransPipe->insertInCell("FlangeA",System,chokeChamber->getCell("BlockOuter"));
  // eTransPipe->insertInCell("FlangeB",System,outerCell);
  eTransPipe->insertInCell("Main",System,chokeChamber->getCell("BlockOuter"));
  eTransPipe->insertInCell("Main",System,buildZone.getCell("preM1"));

  outerCell=buildZone.createUnit(System,*bellowA,"#back");
  dipolePipe->insertInCell("Main",System,outerCell);
  dipolePipe->insertInCell("FlangeB",System,outerCell);

  outerCell=buildZone.createUnit(System,*bellowA,"front");
  bellowA->insertAllInCell(System,outerCell);

  outerCell=buildZone.createUnit(System,*flangePlateA,"front");
  flangePlateA->insertInCell(System,outerCell);

  outerCell=buildZone.createUnit(System,*fm1,"back");
  fm1->insertInCell(System,outerCell);


  if (stopPoint=="Dipole")
    {
      buildZone.rebuildInsertCells(System);
      setCell("MasterVoid",outerCell);
      lastComp=dipolePipe;
      return;
    }

  constructSystem::constructUnit(System,buildZone,*fm1,"back",*flangePlateB);
  constructSystem::constructUnit(System,buildZone,*flangePlateB,"back",*bellowB);

  xbpm1->createAll(System,*this,0);
  flangePlateAA->createAll(System,*xbpm1,"front");
  pipeA->setBack(*flangePlateAA,"back");
  pipeA->createAll(System,*bellowB,"back");

  outerCell=buildZone.createUnit(System,*pipeA,"back");
  pipeA->insertAllInCell(System,outerCell);

  outerCell=buildZone.createUnit(System,*flangePlateAA,"front");
  flangePlateAA->insertInCell(System,outerCell);

  outerCell=buildZone.createUnit(System,*xbpm1,"back");
  xbpm1->insertAllInCell(System,outerCell);

  // FM2 Built relateive to MASTER coordinate

  fm2->createAll(System,*this,0);
  flangePlateC->createAll(System,*fm2,"front");
  bellowC->createAll(System,*flangePlateC,"back");

  // pipe before bellowC (between FM1 and FM2)
  collABPipe->setFront(*xbpm1,"back");
  collABPipe->setBack(*bellowC,"back");
  collABPipe->createAll(System,*xbpm1,"back");

  // permanent magnet (e/p separator) in the middle of this pipe
  constructSystem::pipeMagUnit(System,buildZone,collABPipe,"#front","outerPipe",pMag);
  constructSystem::pipeTerminate(System,buildZone,collABPipe);

  outerCell=buildZone.createUnit(System,*bellowC,"front");
  bellowC->insertAllInCell(System,outerCell);

  outerCell=buildZone.createUnit(System,*flangePlateC,"front");
  flangePlateC->insertInCell(System,outerCell);


  outerCell=buildZone.createUnit(System,*fm2,"back");
  fm2->insertInCell(System,outerCell);

  std::shared_ptr<attachSystem::FixedComp> linkFC(fm2);
  // We never have FM3 in Toyama front-ends
  // if (collFM3Active)
  //   {
  //     constructSystem::constructUnit
  // 	(System,buildZone,*fm2,"back",*fm3);
  //     linkFC=fm3;
  //   }

  constructSystem::constructUnit(System,buildZone,*linkFC,"back",*bellowCA);
  constructSystem::constructUnit(System,buildZone,*bellowCA,"back",*flangePlateD);

  if (msmActive) {
    buildMSM(System,*collExitPipe,"back");

    ha->createAll(System, *this, 0);
    bellowPreHA->createAll(System, *ha, "front");

    msmExitPipe->setBack(*bellowPreHA,"back");
    msmExitPipe->createAll(System,*bellowPostMSM,"back");
    outerCell=buildZone.createUnit(System,*msmExitPipe,"back");
    msmExitPipe->insertAllInCell(System,outerCell);

    buildSupport5(System, *bellowPostMSM, "back");
  } else {
    ha->createAll(System, *this, 0);
    //    bellowPreHA->setFront(*ha, "front");
    bellowPreHA->setBack(*flangePlateD, "back");
    bellowPreHA->createAll(System, *ha, "front");

    buildSupport5(System, *flangePlateD, "back");
  }

  if (stopPoint=="Support5")
    {
      buildZone.rebuildInsertCells(System);
      setCell("MasterVoid",outerCell);
      lastComp=valve2;
      return;
    }

  constructSystem::constructUnit(System,buildZone,*valve2,"back",*bellowDA);

  buildSupport6(System,*bellowDA,2);

  constructSystem::constructUnit(System,buildZone,*bellowH,"back",*flangePlateG);

  buildSupport7(System);

  // Bremsstrahlung collimator
  outerCell = constructSystem::constructUnit(System,buildZone,*offPipeB,"back",*bremCollPipe);
  bremColl->addInsertCell(bremCollPipe->getCell("Void"));
  bremColl->addInsertCell(bremCollPipe->getCell("FlangeAInnerVoid"));
  bremColl->addInsertCell(bremCollPipe->getCell("FlangeBInnerVoid"));
  bremColl->addInsertCell(offPipeB->getCell("Void"));
  bremColl->createAll(System,*bremCollPipe,0);


  // Proximity shielding B
  proxiShieldBPipe->createAll(System,*bremCollPipe,"back");
  proxiShieldB->setCutSurf("Inner", *proxiShieldBPipe, "outerPipe");
  proxiShieldB->createAll(System,*proxiShieldBPipe,"#front");

  if (ExternalCut::isActive("REWall"))
    {
      buildZone.setMaxExtent(getRule("REWall"));
      outerCell=buildZone.createUnit(System);
    }

  buildZone.rebuildInsertCells(System); // simplifies buildZone outer void

  proxiShieldB->insertInCell(System,outerCell);
  proxiShieldBPipe->insertInCell("FlangeA", System, outerCell);
  proxiShieldBPipe->insertInCell("Main", System, outerCell);

  buildExtras(System);
  setCell("MasterVoid",outerCell);
  lastComp=proxiShieldBPipe;

  return;
}


void
R3FrontEndToyamaDanMAX::createAll(Simulation& System,
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
  ELog::RegMethod RControl("R3FrontEndToyamaDanMAX","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  buildObjects(System);
  createLinks();

  return;
}


}   // NAMESPACE xraySystem
