/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   singleItemBuild/makeSingleItem.cxx
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
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
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
#include <array>
#include <algorithm>
#include <iterator>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "Line.h"
#include "inputParam.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "PointMap.h"
#include "World.h"
#include "FixedGroup.h"
#include "FixedOffsetGroup.h"
#include "FixedRotateGroup.h"
#include "insertObject.h"
#include "insertSphere.h"
#include "insertCylinder.h"
#include "insertShell.h"

#include "GeneralPipe.h"
#include "VacuumPipe.h"
#include "WindowPipe.h"
#include "Quadrupole.h"
#include "Sexupole.h"
#include "Octupole.h"
#include "LQuadF.h"
#include "LQuadH.h"
#include "LSexupole.h"
#include "ChopperPit.h"
#include "SingleChopper.h"
#include "CorrectorMag.h"
#include "CollTube.h"
#include "CollUnit.h"
#include "EPSeparator.h"
#include "EPCombine.h"
#include "EPContinue.h"
#include "QuadUnit.h"
#include "DipoleChamber.h"
#include "DipoleExtract.h"
#include "DipoleSndBend.h"
#include "R3ChokeChamber.h"
#include "HalfElectronPipe.h"
#include "MagnetM1.h"
#include "MagnetU1.h"
#include "MagnetBlock.h"
#include "CylGateValve.h"
#include "GateValveCube.h"
#include "GateValveCylinder.h"
#include "StriplineBPM.h"
#include "BeamDivider.h"
#include "CeramicGap.h"
#include "DipoleDIBMag.h"
#include "Dipole.h"
#include "EArrivalMon.h"
#include "EBeamStop.h"
#include "SixPortTube.h"
#include "FourPortTube.h"
#include "CrossWayTube.h"
#include "CrossWayBlank.h"
#include "TubeDetBox.h"
#include "GaugeTube.h"
#include "BremBlock.h"
#include "Scrapper.h"
#include "FlatPipe.h"
#include "TriPipe.h"
#include "TriGroup.h"
#include "CurveMagnet.h"
#include "MultiPipe.h"
#include "CooledScreen.h"
#include "YagScreen.h"
#include "YagUnit.h"
#include "YagUnitBig.h"
#include "BeamScrapper.h"
#include "TWCavity.h"
#include "SplitFlangePipe.h"
#include "Bellows.h"
#include "VirtualTube.h"
#include "PipeTube.h"
#include "PortTube.h"
#include "BlankTube.h"
#include "ButtonBPM.h"
#include "CleaningMagnet.h"
#include "UndulatorVacuum.h"
#include "PrismaChamber.h"
#include "PortTube.h"
#include "JawFlange.h"
#include "portItem.h"
#include "portSet.h"
#include "SquareFMask.h"
#include "IonPumpTube.h"
#include "IonGauge.h"
#include "TriggerTube.h"
#include "NBeamStop.h"
#include "BeamBox.h"
#include "BremTube.h"
#include "HPJaws.h"
#include "BoxJaws.h"
#include "DiffPumpXIADP03.h"
#include "CRLTube.h"
#include "ViewScreenTube.h"
#include "forkHoles.h"
#include "ExperimentalHutch.h"
#include "ConnectorTube.h"
#include "CornerPipe.h"
#include "LocalShielding.h"
#include "WrapperCell.h"
#include "FlangeDome.h"
#include "MonoShutter.h"
#include "RoundMonoShutter.h"
#include "GuideUnit.h"
#include "PlateUnit.h"
#include "BenderUnit.h"

#include "makeSingleItem.h"

namespace singleItemSystem
{

makeSingleItem::makeSingleItem()
 /*!
    Constructor
 */
{}


makeSingleItem::~makeSingleItem()
  /*!
    Destructor
  */
{}

void
makeSingleItem::build(Simulation& System,
		      const mainSystem::inputParam& IParam)
  /*!
    Carry out the full build
    \param System :: Simulation system
    \param IParam :: Input parameters
   */
{
  // For output stream
  ELog::RegMethod RegA("makeSingleItem","build");

  std::set<std::string> validItems
    ({
      "default",
	"CornerPipe","ChopperPit","CylGateValve","SingleChopper",
	"GateValveCube","GateValveCylinder","CleaningMagnet",
	"CorrectorMag","Jaws","LQuadF","LQuadH","LSexupole",
	"MagnetBlock","Sexupole","MagnetM1","MagnetU1",
	"Octupole","CeramicGap",
	"EBeamStop","FMask","R3ChokeChamber",
	"DipoleExtract","DipoleSndBend","Dipole",
	"EPSeparator","EPCombine","EPContinue",
	"Quadrupole","TargetShield","FourPort","DoublePort",
	"FlatPipe","TriPipe","TriGroup","SixPort","CrossWay","CrossBlank",
	"GaugeTube","BremBlock","DipoleDIBMag","EArrivalMon","YagScreen",
	"YAG","YagUnit","YagUnitBig","CooledScreen","CooledUnit",
	"StriplineBPM","BeamDivider","BeamScrapper",
	"Scrapper","TWCavity","Bellow", "VacuumPipe","WindowPipe",
	"HalfElectronPipe",
	"MultiPipe","PipeTube","PortTube","BlankTube","ButtonBPM",
	"PrismaChamber","uVac", "UndVac","UndulatorVacuum",
	"IonPTube","IonGauge","CollTube",
	"NBeamStop","MagTube","TriggerTube",
	"BremTube","HPJaws","BoxJaws","HPCombine","ViewTube",
	"DiffPumpXIADP03","CRLTube","ExperimentalHutch",
	"ConnectorTube","LocalShield","FlangeDome",
	"MonoShutter","RoundMonoShutter","TubeDetBox",
	"GuideUnit","PlateUnit","BenderUnit",
	"Help","help"
    });

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  const int voidCell(74123);

  const std::string item=
    IParam.getDefValue<std::string>("default","singleItem");

  if (validItems.find(item)==validItems.end())
    throw ColErr::InContainerError<std::string>
      (item,"Item no a single component");

  ELog::EM<<"Conponent == "<<item<<ELog::endDiag;
  if (item=="default" || item == "CylGateValve" )
    {
      std::shared_ptr<xraySystem::CylGateValve>
	GV(new xraySystem::CylGateValve("GV"));

      OR.addObject(GV);

      GV->addInsertCell(voidCell);
      GV->createAll(System,World::masterOrigin(),0);

      return;
    }
  if (item == "ChopperPit")
    {
      std::shared_ptr<constructSystem::ChopperPit>
	PitA(new constructSystem::ChopperPit("ChopperPit"));

      OR.addObject(PitA);

      PitA->addInsertCell(voidCell);
      PitA->createAll(System,World::masterOrigin(),0);
      return;
    }
  if (item == "SingleChopper")
    {
      std::shared_ptr<essConstruct::SingleChopper>
	SC(new essConstruct::SingleChopper("singleChopper"));

      OR.addObject(SC);

      SC->addInsertCell(voidCell);
      SC->createAll(System,World::masterOrigin(),0);
      return;
    }
  if (item == "MagTube")
    {
      std::shared_ptr<insertSystem::insertCylinder>
	TubeA(new insertSystem::insertCylinder("MagTube"));
      std::shared_ptr<insertSystem::insertCylinder>
	TubeB(new insertSystem::insertCylinder("MagTubeB"));

      OR.addObject(TubeA);
      OR.addObject(TubeB);

      TubeA->addInsertCell(voidCell);
      TubeA->createAll(System,World::masterOrigin(),0);

      TubeB->addInsertCell(voidCell);
      TubeB->createAll(System,World::masterOrigin(),0);
      return;
    }
  if (item == "GateValveCube" )
    {
      std::shared_ptr<constructSystem::GateValveCube>
	GV(new constructSystem::GateValveCube("GVCube"));

      OR.addObject(GV);

      GV->addInsertCell(voidCell);
      GV->createAll(System,World::masterOrigin(),0);

      return;
    }
  if (item == "GateValveCylinder" )
    {
      std::shared_ptr<constructSystem::GateValveCylinder>
	GV(new constructSystem::GateValveCylinder("GVCylinder"));

      OR.addObject(GV);

      GV->addInsertCell(voidCell);
      GV->createAll(System,World::masterOrigin(),0);

      return;
    }

  // This is similar to yagscreen/cooledscreen and
  // can attach to flange.
  if (item == "BeamScrapper")
    {
      std::shared_ptr<xraySystem::BeamScrapper>
	BS(new xraySystem::BeamScrapper("BeamScrapper"));
      OR.addObject(BS);

      BS->addAllInsertCell(voidCell);
      BS->setBeamAxis(Geometry::Vec3D(0,10,0),
		       Geometry::Vec3D(1,0,0));
      BS->createAll(System,World::masterOrigin(),0);

      return;
    }

  if (item == "YAG" || item=="YagScreen")
    {
      std::shared_ptr<tdcSystem::YagScreen>
	YAG(new tdcSystem::YagScreen("YAG"));
      OR.addObject(YAG);

      YAG->addAllInsertCell(voidCell);
      YAG->setBeamAxis(Geometry::Vec3D(0,-10,0),
		       Geometry::Vec3D(1,0,0));
      YAG->createAll(System,World::masterOrigin(),0);

      return;
    }

  if (item == "CooledScreen")
    {
      std::shared_ptr<xraySystem::CooledScreen>
	Cool(new xraySystem::CooledScreen("Cool"));
      OR.addObject(Cool);

      Cool->addAllInsertCell(voidCell);
      Cool->setBeamAxis(Geometry::Vec3D(0,-10,0),
		       Geometry::Vec3D(1,0,0));
      Cool->createAll(System,World::masterOrigin(),0);

      return;
    }

  if (item == "YagUnit")
    {
      std::shared_ptr<tdcSystem::YagUnit>
	yagUnit(new tdcSystem::YagUnit("YU"));

      std::shared_ptr<tdcSystem::YagScreen>
	yagScreen(new tdcSystem::YagScreen("YAG"));

      OR.addObject(yagUnit);
      OR.addObject(yagScreen);

      yagUnit->addInsertCell(voidCell);
      yagUnit->createAll(System,World::masterOrigin(),0);

      yagScreen->setBeamAxis(*yagUnit,1);
      yagScreen->createAll(System,*yagUnit,4);
      yagScreen->insertInCell("Outer",System,voidCell);
      yagScreen->insertInCell("Connect",System,yagUnit->getCell("PlateB"));
      yagScreen->insertInCell("Connect",System,yagUnit->getCell("Void"));
      yagScreen->insertInCell("Payload",System,yagUnit->getCell("Void"));

      return;
    }

  if (item == "CooledUnit")
    {
      std::shared_ptr<tdcSystem::YagUnit>
	yagUnit(new tdcSystem::YagUnit("YU"));

      std::shared_ptr<xraySystem::CooledScreen>
	coolScreen(new xraySystem::CooledScreen("Cool"));

      OR.addObject(yagUnit);
      OR.addObject(coolScreen);

      yagUnit->addInsertCell(voidCell);
      yagUnit->createAll(System,World::masterOrigin(),0);

      coolScreen->setBeamAxis(*yagUnit,1);
      coolScreen->createAll(System,*yagUnit,4);
      coolScreen->insertInCell("Outer",System,voidCell);
      coolScreen->insertInCell("Connect",System,yagUnit->getCell("PlateB"));
      coolScreen->insertInCell("Connect",System,yagUnit->getCell("Void"));
      coolScreen->insertInCell("Payload",System,yagUnit->getCell("Void"));

      return;
    }
  if (item == "YagUnitBig")
    {
      std::shared_ptr<tdcSystem::YagUnitBig>
	yagUnit(new tdcSystem::YagUnitBig("YUBig"));

      std::shared_ptr<tdcSystem::YagScreen>
	yagScreen(new tdcSystem::YagScreen("YAG"));

      OR.addObject(yagUnit);
      OR.addObject(yagScreen);

      yagUnit->addInsertCell(voidCell);
      yagUnit->createAll(System,World::masterOrigin(),0);

      yagScreen->setBeamAxis(*yagUnit,1);
      yagScreen->createAll(System,*yagUnit,4);
      yagScreen->insertInCell("Outer",System,voidCell);
      yagScreen->insertInCell("Connect",System,yagUnit->getCell("Plate"));
      yagScreen->insertInCell("Connect",System,yagUnit->getCell("Void"));
      yagScreen->insertInCell("Payload",System,yagUnit->getCell("Void"));


      return;
    }
  if (item == "FlangeDome")
    {
      std::shared_ptr<constructSystem::PipeTube>
	ft(new constructSystem::PipeTube("FlangeTube"));
      std::shared_ptr<constructSystem::FlangeDome>
	fd(new constructSystem::FlangeDome("FlangeDome"));
      OR.addObject(ft);
      OR.addObject(fd);

      ft->addAllInsertCell(voidCell);
      ft->setOuterVoid();
      ft->createAll(System,World::masterOrigin(),0);

      fd->addInsertCell(voidCell);
      fd->setCutSurf("plate",*ft,"front");
      fd->createAll(System,*ft,1);

      return;
    }
  if (item == "StriplineBPM")
    {
      std::shared_ptr<tdcSystem::StriplineBPM>
	bpm(new tdcSystem::StriplineBPM("BPM"));
      OR.addObject(bpm);

      bpm->addInsertCell(voidCell);
      bpm->createAll(System,World::masterOrigin(),0);

      return;
    }
  if (item == "EBeamStop")
    {
      std::shared_ptr<tdcSystem::EBeamStop>
	eBeam(new tdcSystem::EBeamStop("EBeam"));
      OR.addObject(eBeam);

      eBeam->addAllInsertCell(voidCell);
      eBeam->createAll(System,World::masterOrigin(),0);

      return;
    }
  if (item == "CeramicGap")
    {
      std::shared_ptr<tdcSystem::CeramicGap>
	cSep(new tdcSystem::CeramicGap("CerSep"));
      OR.addObject(cSep);

      cSep->addInsertCell(voidCell);
      cSep->createAll(System,World::masterOrigin(),0);

      return;
    }
  if (item == "BeamDivider")
    {
      std::shared_ptr<tdcSystem::BeamDivider>
	bd(new tdcSystem::BeamDivider("BeamDiv"));
      OR.addObject(bd);

      bd->addAllInsertCell(voidCell);
      bd->createAll(System,World::masterOrigin(),0);

      return;
    }
  if (item == "CleaningMagnet")
    {
      std::shared_ptr<tdcSystem::CleaningMagnet>
	cm(new tdcSystem::CleaningMagnet("CleaningMagnet"));
      OR.addObject(cm);

      cm->addInsertCell(voidCell);
      cm->createAll(System,World::masterOrigin(),0);

      return;
    }

  if (item == "CorrectorMag" )
    {
      std::shared_ptr<constructSystem::VacuumPipe>
	VC(new constructSystem::VacuumPipe("CorrectorMagPipe"));
      std::shared_ptr<xraySystem::CorrectorMag>
	CM(new xraySystem::CorrectorMag("CM"));

      OR.addObject(VC);
      OR.addObject(CM);

      VC->addAllInsertCell(voidCell);
      VC->createAll(System,World::masterOrigin(),0);

      CM->setCutSurf("Inner",*VC,"outerPipe");
      CM->addInsertCell(voidCell);
      CM->createAll(System,*VC,0);

      return;
    }
  if (item == "CleaningMagnet")
    {
      std::shared_ptr<tdcSystem::CleaningMagnet>
	cm(new tdcSystem::CleaningMagnet("CleaningMagnet"));
      OR.addObject(cm);

      cm->addInsertCell(voidCell);
      cm->createAll(System,World::masterOrigin(),0);

      return;
    }
  if (item == "FMask")
    {
      std::shared_ptr<xraySystem::SquareFMask>
	fm(new xraySystem::SquareFMask("FMask"));
      OR.addObject(fm);

      fm->addInsertCell(voidCell);
      fm->createAll(System,World::masterOrigin(),0);

      // with pipes:
      std::shared_ptr<xraySystem::SquareFMask>
	fmb(new xraySystem::SquareFMask("FMaskB"));
      OR.addObject(fmb);

      fmb->addInsertCell(voidCell);
      fmb->createAll(System,World::masterOrigin(),0);

      return;
    }

  if (item == "Jaws")
    {
      // diagnostic box
      std::shared_ptr<constructSystem::PortTube>
	diagBox(new constructSystem::PortTube("DiagnosticBox"));
      // two pairs of jaws
      std::array<std::shared_ptr<constructSystem::JawFlange>,2>
	jawComp
	({
	  std::make_shared<constructSystem::JawFlange>("DiagnosticBoxJawUnit0"),
	  std::make_shared<constructSystem::JawFlange>("DiagnosticBoxJawUnit1")
	});
      OR.addObject(diagBox);
      OR.addObject(jawComp[0]);
      OR.addObject(jawComp[1]);

      diagBox->addAllInsertCell(voidCell);
      diagBox->createAll(System,World::masterOrigin(),0);

      for(size_t index=0;index<2;index++)
	{
	  const constructSystem::portItem& DPI=diagBox->getPort(index);
	  jawComp[index]->setFillRadius(DPI,"InnerRadius",DPI.getCell("Void"));

	  jawComp[index]->addInsertCell(diagBox->getCell("Void"));
	  if (index)
	    jawComp[index]->addInsertCell(jawComp[index-1]->getCell("Void"));
	  jawComp[index]->secondaryUnitVector(*diagBox,0);
	  jawComp[index]->createAll(System,DPI,"InnerPlate");
	}

      // simplify the DiagnosticBox inner cell
      diagBox->splitVoidPorts(System,"SplitOuter",2001,
			      diagBox->getCell("Void"),{0,2});
    }

  if (item == "DoublePort")
    {
      std::shared_ptr<constructSystem::PipeTube>
	dPipe(new constructSystem::PipeTube("DoublePipe"));
      dPipe->addAllInsertCell(voidCell);
      dPipe->createAll(System,World::masterOrigin(),0);
    }
  
  if (item == "EArrivalMon" )
    {
      std::shared_ptr<tdcSystem::EArrivalMon>
	EA(new tdcSystem::EArrivalMon("BeamMon"));

      OR.addObject(EA);

      EA->addInsertCell(voidCell);
      EA->createAll(System,World::masterOrigin(),0);

      return;
    }
  if (item == "LocalShield")
    {
      std::shared_ptr<tdcSystem::LocalShielding>
	sb(new tdcSystem::LocalShielding("ShieldB"));
      std::shared_ptr<tdcSystem::LocalShielding>
	sc(new tdcSystem::LocalShielding("ShieldC"));
      std::shared_ptr<tdcSystem::LocalShielding>
	sd(new tdcSystem::LocalShielding("ShieldD"));
      std::shared_ptr<tdcSystem::LocalShielding>
	se(new tdcSystem::LocalShielding("ShieldE"));
      std::shared_ptr<attachSystem::WrapperCell>
	ls(new attachSystem::WrapperCell("","Cell"));

      OR.addObject(sb);
      OR.addObject(sc);
      OR.addObject(sd);
      OR.addObject(se);
      OR.addObject(ls);

      ls->addUnit(sb);
      ls->addUnit(sc);
      ls->addUnit(sd);
      ls->addUnit(se);

      ls->setSurfaces({
    	    {"front",{"ShieldE","#back"}},      // -1050002
	    {"back",{"ShieldB","#back"}},       // -1020002
	    {"left",{"ShieldC","left"}},        // 1030003
	    {"right",{"ShieldB","#right"}},     // -102004
	    {"base",{"ShieldE","base"}},        // 1050005
	    {"top",{"ShieldB","#top"}}          // -1020006
	});

      ls->setConnections
	({
	  {"ShieldB",{"THIS",""}},
	  {"ShieldC",{"ShieldB","bottom"}},
	  {"ShieldD",{"ShieldB","front"}},
	  {"ShieldE",{"ShieldB","front"}}
	});

      ls->addInsertCell(voidCell);
      ls->createAll(System,World::masterOrigin(),0);

      return;
    }

  if (item=="LQuadF")
    {
      std::shared_ptr<constructSystem::VacuumPipe>
	VC(new constructSystem::VacuumPipe("QHVC"));
      std::shared_ptr<tdcSystem::LQuadF>
	QF(new tdcSystem::LQuadF("QF","QF"));

      OR.addObject(VC);
      OR.addObject(QF);

      VC->addAllInsertCell(voidCell);
      VC->createAll(System,World::masterOrigin(),0);

      QF->setCutSurf("Inner",*VC,"outerPipe");
      QF->addInsertCell(voidCell);
      QF->createAll(System,World::masterOrigin(),0);

      return;
    }

  if (item=="LQuadH")
    {
      std::shared_ptr<constructSystem::VacuumPipe>
	VC(new constructSystem::VacuumPipe("QHVC"));
      std::shared_ptr<tdcSystem::LQuadH>
	QH(new tdcSystem::LQuadH("QH","QH"));

      OR.addObject(VC);
      OR.addObject(QH);

      VC->addAllInsertCell(voidCell);
      VC->createAll(System,World::masterOrigin(),0);

      QH->setCutSurf("Inner",*VC,"outerPipe");
      QH->addInsertCell(voidCell);
      QH->createAll(System,World::masterOrigin(),0);

      return;
    }

  if (item=="LSexupole")
    {
      std::shared_ptr<constructSystem::VacuumPipe>
	VC(new constructSystem::VacuumPipe("QHVC"));
      std::shared_ptr<tdcSystem::LSexupole>
	LS(new tdcSystem::LSexupole("LS","LS"));

      OR.addObject(VC);
      OR.addObject(LS);

      VC->addAllInsertCell(voidCell);
      VC->createAll(System,World::masterOrigin(),0);

      LS->setCutSurf("Inner",*VC,"outerPipe");
      LS->addInsertCell(voidCell);
      LS->createAll(System,World::masterOrigin(),0);

      return;
    }

  if (item=="MagnetBlock")
    {

      std::shared_ptr<xraySystem::MagnetBlock>
	MB(new xraySystem::MagnetBlock("MB1"));

      OR.addObject(MB);
      MB->addAllInsertCell(voidCell);
      MB->createAll(System,World::masterOrigin(),0);

      return;
    }

  if (item=="Sexupole")
    {
      std::shared_ptr<xraySystem::Sexupole>
	SXX(new xraySystem::Sexupole("SXX","SXX"));

      OR.addObject(SXX);

      SXX->addInsertCell(voidCell);
      SXX->createAll(System,World::masterOrigin(),0);

      return;
    }

  if (item=="MagnetM1")
    {
      std::shared_ptr<xraySystem::MagnetM1>
	MagBlock(new xraySystem::MagnetM1("M1Block"));

      OR.addObject(MagBlock);

      MagBlock->addAllInsertCell(voidCell);
      MagBlock->createAll(System,World::masterOrigin(),0);

      return;
    }

  if (item=="MagnetU1")
    {
      std::shared_ptr<xraySystem::MagnetU1>
	MagBlock(new xraySystem::MagnetU1("U1Block"));

      OR.addObject(MagBlock);

      MagBlock->addAllInsertCell(voidCell);
      MagBlock->createAll(System,World::masterOrigin(),0);

      return;
    }

  if (item=="Octupole")
    {
      std::shared_ptr<xraySystem::Octupole>
	OXX(new xraySystem::Octupole("M1BlockOXX","M1BlockOXX"));
      OR.addObject(OXX);
      OXX->addInsertCell(voidCell);
      OXX->createAll(System,World::masterOrigin(),0);

      return;
    }

  if (item=="EPSeparator")
    {
      std::shared_ptr<xraySystem::EPSeparator>
	EPsep(new xraySystem::EPSeparator("EPSeparator"));
      OR.addObject(EPsep);

      EPsep->addInsertCell(voidCell);
      EPsep->createAll(System,World::masterOrigin(),0);

      return;
    }

  if (item=="EPCombine" || item=="EPContinue")
    {
      const double angle(1.5*M_PI/180.0);
      const Geometry::Vec3D photOrg(0,0,0);
      const Geometry::Vec3D elecOrg(1.84502,0,0);
      const Geometry::Vec3D elecAxis(sin(angle),cos(angle),0);

      std::shared_ptr<xraySystem::EPCombine>
	EPcom(new xraySystem::EPCombine("EPCombine"));
      OR.addObject(EPcom);
      EPcom->setEPOriginPair(photOrg,elecOrg,elecAxis);
      EPcom->addInsertCell(voidCell);
      EPcom->createAll(System,World::masterOrigin(),0);

      if (item=="EPContinue")
	{

	  std::shared_ptr<xraySystem::EPContinue>
	    EPcont(new xraySystem::EPContinue("EPContinue"));
	  OR.addObject(EPcont);
	  EPcont->setEPOriginPair(*EPcom);
	  EPcont->addInsertCell(voidCell);
	  EPcont->createAll(System,*EPcom,2);
	}
      return;
    }

  if (item=="R3ChokeChamber")
    {
      const double angle(1.5*M_PI/180.0);
      const Geometry::Vec3D photOrg(0,0,0);
      const Geometry::Vec3D elecOrg(1.84502,0,0);
      const Geometry::Vec3D elecAxis(sin(angle),cos(angle),0);
      
      std::shared_ptr<xraySystem::R3ChokeChamber>
	CChamber(new xraySystem::R3ChokeChamber("R3Chamber"));
      OR.addObject(CChamber);
      CChamber->setEPOriginPair(photOrg,elecOrg,elecAxis);
	      
      CChamber->addAllInsertCell(voidCell);
      CChamber->createAll(System,World::masterOrigin(),0);

      return;
    }
  if (item=="R3Chamber")
    {
      std::shared_ptr<xraySystem::R3ChokeChamber>
	CChamber(new xraySystem::R3ChokeChamber("R3Chamber"));
      OR.addObject(CChamber);
      CChamber->addAllInsertCell(voidCell);
      CChamber->createAll(System,World::masterOrigin(),0);

      return;
    }

  if (item=="DipoleExtract")
    {
      std::shared_ptr<xraySystem::DipoleExtract>
	DE(new xraySystem::DipoleExtract("DipoleExtract"));
      OR.addObject(DE);
      DE->addInsertCell(voidCell);
      DE->createAll(System,World::masterOrigin(),0);

      return;
    }
  if (item=="DipoleSndBend")
    {
      std::shared_ptr<xraySystem::DipoleSndBend>
	DB(new xraySystem::DipoleSndBend("DipoleSndBend"));
      OR.addObject(DB);
      DB->addAllInsertCell(voidCell);
      DB->createAll(System,World::masterOrigin(),0);

      return;
    }
  if (item=="HalfElectronPipe")
    {
      std::shared_ptr<xraySystem::HalfElectronPipe>
	HE(new xraySystem::HalfElectronPipe("M1BlockHalfElectron"));
      OR.addObject(HE);
      HE->addAllInsertCell(voidCell);
      HE->createAll(System,World::masterOrigin(),0);

      return;
    }
  if (item == "PrismaChamber")
    {
      std::shared_ptr<tdcSystem::PrismaChamber>
	sc(new tdcSystem::PrismaChamber("PrismaChamber"));
      OR.addObject(sc);

      sc->addInsertCell(voidCell);
      sc->createAll(System,World::masterOrigin(),0);

      return;
    }

  if (item=="Quadrupole")
    {
      std::shared_ptr<xraySystem::Quadrupole>
	Quad(new xraySystem::Quadrupole("QFend","QFend"));
      OR.addObject(Quad);
      Quad->addInsertCell(voidCell);
      Quad->createAll(System,World::masterOrigin(),0);
      return;
    }
  if (item == "Scrapper")
    {
      std::shared_ptr<tdcSystem::Scrapper>
	sc(new tdcSystem::Scrapper("Scrapper"));
      OR.addObject(sc);

      sc->addInsertCell(voidCell);
      sc->createAll(System,World::masterOrigin(),0);

      return;
    }

  if (item=="TargetShield")
    {
      std::shared_ptr<insertSystem::insertSphere>
	Target(new insertSystem::insertSphere("Target"));
      std::shared_ptr<insertSystem::insertShell>
	Surround(new insertSystem::insertShell("Shield"));
      std::shared_ptr<insertSystem::insertCylinder>
	TubeA(new insertSystem::insertCylinder("TubeA"));
      std::shared_ptr<insertSystem::insertCylinder>
	TubeB(new insertSystem::insertCylinder("TubeB"));

      OR.addObject(Target);
      OR.addObject(TubeA);
      OR.addObject(TubeB);
      OR.addObject(Surround);

      ELog::EM<<"ASDFASFDSAF "<<ELog::endDiag;
      TubeA->addInsertCell(voidCell);
      TubeA->createAll(System,World::masterOrigin(),0);
      ELog::EM<<"ASDFASFDSAF "<<ELog::endDiag;
      TubeB->addInsertCell(voidCell);
      TubeB->createAll(System,*TubeA,2);
      return;
    }

  if (item=="SixPort")
    {
      std::shared_ptr<tdcSystem::SixPortTube>
	SP(new tdcSystem::SixPortTube("SixPort"));

      OR.addObject(SP);

      SP->addInsertCell(voidCell);
      SP->createAll(System,World::masterOrigin(),0);

      return;
    }
  if (item=="FourPort")
    {
      std::shared_ptr<xraySystem::FourPortTube>
	FP(new xraySystem::FourPortTube("FourPort"));

      OR.addObject(FP);
      FP->setSideVoid();
      FP->addInsertCell(voidCell);
      FP->createAll(System,World::masterOrigin(),0);

      return;
    }
  if (item=="CrossWay")
    {
      std::shared_ptr<xraySystem::CrossWayTube>
	SP(new xraySystem::CrossWayTube("CrossWay"));

      OR.addObject(SP);

      SP->addInsertCell(voidCell);
      SP->createAll(System,World::masterOrigin(),0);

      return;
    }
  if (item=="CrossBlank")
    {
      std::shared_ptr<tdcSystem::CrossWayBlank>
	SP(new tdcSystem::CrossWayBlank("CrossBlank"));

      OR.addObject(SP);

      SP->addInsertCell(voidCell);
      SP->createAll(System,World::masterOrigin(),0);

      return;
    }
  if (item=="GaugeTube")
    {
      std::shared_ptr<xraySystem::GaugeTube>
	SP(new xraySystem::GaugeTube("GaugeTube"));

      OR.addObject(SP);

      SP->addInsertCell(voidCell);
      SP->createAll(System,World::masterOrigin(),0);

      return;
    }
  if (item=="BremBlock")
    {
      std::shared_ptr<xraySystem::BremBlock>
	BB(new xraySystem::BremBlock("BremBlock"));

      OR.addObject(BB);

      BB->addInsertCell(voidCell);
      BB->createAll(System,World::masterOrigin(),0);

      return;
    }
  if (item=="IonPTube")
    {
      std::shared_ptr<xraySystem::IonPumpTube>
	SP(new xraySystem::IonPumpTube("IonPTube"));

      OR.addObject(SP);

      SP->addInsertCell(voidCell);
      SP->createAll(System,World::masterOrigin(),0);

      return;
    }
  if (item=="IonGauge")
    {
      std::shared_ptr<xraySystem::IonGauge>
	IG(new xraySystem::IonGauge("IonGauge"));

      OR.addObject(IG);

      IG->addInsertCell(voidCell);
      IG->createAll(System,World::masterOrigin(),0);

      return;
    }
  if (item=="CollTube")
    {
      std::shared_ptr<xraySystem::CollTube>
	CT(new xraySystem::CollTube("CollTube"));
      std::shared_ptr<xraySystem::CollUnit>
	CU(new xraySystem::CollUnit("CollUnit"));

      OR.addObject(CT);
      OR.addObject(CU);


      CT->addInsertCell(voidCell);
      CT->createAll(System,World::masterOrigin(),0);
      CU->addInsertCell(CT->getCell("Void"));
      CU->addInsertCell(CT->getCell("PipeVoid"));
      CU->setCutSurf("Flange",*CT,"TopFlange");
      CU->createAll(System,*CT,"Origin");

      return;
    }
  if (item=="TriggerTube")
    {
      std::shared_ptr<xraySystem::TriggerTube>
	SP(new xraySystem::TriggerTube("TriggerTube"));

      OR.addObject(SP);

      SP->addInsertCell(voidCell);
      SP->createAll(System,World::masterOrigin(),0);

      return;
    }

  if (item=="NBeamStop")
    {
      std::shared_ptr<tdcSystem::NBeamStop>
	BS(new tdcSystem::NBeamStop("BeamStop"));
      std::shared_ptr<tdcSystem::BeamBox>
	BX(new tdcSystem::BeamBox("BeamBox"));

      OR.addObject(BS);
      OR.addObject(BX);

      BX->addInsertCell(voidCell);
      BX->createAll(System,World::masterOrigin(),0);

      BS->addInsertCell(BX->getCells("Void"));
      BS->createAll(System,World::masterOrigin(),0);

      return;
    }

  if (item == "ConnectorTube")
    {
      std::shared_ptr<xraySystem::ConnectorTube>
	cp(new xraySystem::ConnectorTube("ConnectorTube"));
      OR.addObject(cp);

      cp->addInsertCell(voidCell);
      cp->createAll(System,World::masterOrigin(),0);

      return;
    }

  if (item == "TriPipe")
    {
      std::shared_ptr<tdcSystem::TriPipe>
	tp(new tdcSystem::TriPipe("TriPipe"));
      OR.addObject(tp);

      tp->addAllInsertCell(voidCell);
      tp->createAll(System,World::masterOrigin(),0);

      return;
    }

  if (item == "TriGroup")
    {
      std::shared_ptr<tdcSystem::TriGroup>
	tp(new tdcSystem::TriGroup("TriGroup"));
      OR.addObject(tp);

      tp->addAllInsertCell(voidCell);
      tp->createAll(System,World::masterOrigin(),0);

      return;
    }

  if (item == "CurveMag")
    {
      std::shared_ptr<tdcSystem::CurveMagnet>
	cm(new tdcSystem::CurveMagnet("CMag"));
      OR.addObject(cm);

      cm->addAllInsertCell(voidCell);
      cm->createAll(System,World::masterOrigin(),0);

      return;
    }

  if (item == "UndVac" || item=="UndulatorVacuum" || item=="uVac")
    {
      std::shared_ptr<tdcSystem::UndulatorVacuum>
	uVac(new tdcSystem::UndulatorVacuum("UVac"));
      OR.addObject(uVac);

      uVac->addInsertCell(voidCell);
      uVac->createAll(System,World::masterOrigin(),0);

      return;
    }

  if (item == "MultiPipe")
    {
      std::shared_ptr<tdcSystem::MultiPipe>
	tp(new tdcSystem::MultiPipe("MultiPipe"));
      OR.addObject(tp);

      tp->addAllInsertCell(voidCell);
      tp->createAll(System,World::masterOrigin(),0);

      return;
    }

  if (item == "FlatPipe")
    {
      std::shared_ptr<tdcSystem::FlatPipe>
	tp(new tdcSystem::FlatPipe("FlatPipe"));
      OR.addObject(tp);

      tp->addAllInsertCell(voidCell);
      tp->createAll(System,World::masterOrigin(),0);

      return;
    }
  if (item == "CornerPipe")
    {
      std::shared_ptr<constructSystem::CornerPipe>
	cp(new constructSystem::CornerPipe("CornerPipe"));
      OR.addObject(cp);

      cp->addAllInsertCell(voidCell);
      cp->createAll(System,World::masterOrigin(),0);

      return;
    }

  if (item=="DipoleDIBMag")
    {
      std::shared_ptr<constructSystem::VacuumPipe>
	VC(new constructSystem::VacuumPipe("DipolePipe"));
      std::shared_ptr<tdcSystem::DipoleDIBMag>
	DIB(new tdcSystem::DipoleDIBMag("DIB"));

      OR.addObject(VC);
      OR.addObject(DIB);

      VC->addAllInsertCell(voidCell);
      VC->createAll(System,World::masterOrigin(),0);

      DIB->setCutSurf("Inner",*VC,"outerPipe");
      DIB->addInsertCell(voidCell);
      DIB->createAll(System,World::masterOrigin(),0);

      return;
    }

  if (item=="Dipole")
    {
      std::shared_ptr<constructSystem::VacuumPipe>
	VC(new constructSystem::VacuumPipe("DipolePipe"));
      std::shared_ptr<xraySystem::Dipole>
	DIB(new xraySystem::Dipole("DIM"));

      OR.addObject(VC);
      OR.addObject(DIB);
      
      //VC->addAllInsertCell(voidCell);
      //VC->createAll(System,World::masterOrigin(),0);

      //      DIB->setCutSurf("Inner",*VC,"outerPipe");
      DIB->addInsertCell(voidCell);
      DIB->createAll(System,World::masterOrigin(),0);

      return;
    }

  if (item=="TWCavity") // traveling wave cavity
    {
      std::shared_ptr<constructSystem::VacuumPipe>
	pipeA(new constructSystem::VacuumPipe("PipeA"));
      std::shared_ptr<tdcSystem::TWCavity> cavity(new tdcSystem::TWCavity("TWCavity"));
      std::shared_ptr<constructSystem::VacuumPipe>
	pipeB(new constructSystem::VacuumPipe("PipeB"));

      OR.addObject(pipeA);
      OR.addObject(cavity);
      OR.addObject(pipeB);

      pipeA->addAllInsertCell(voidCell);
      pipeA->createAll(System,World::masterOrigin(),0);

      cavity->addInsertCell(voidCell);
      cavity->createAll(System,*pipeA,"back");

      pipeB->addAllInsertCell(voidCell);
      pipeB->createAll(System,*cavity,"back");

      return;
    }

  if (item=="Bellow")
    {
      std::shared_ptr<constructSystem::Bellows>
	bellow(new constructSystem::Bellows("Bellow"));
      OR.addObject(bellow);

      bellow->addInsertCell(voidCell);
      bellow->createAll(System,World::masterOrigin(),0);

      return;
    }

  if (item == "VacuumPipe" )
    {
      std::shared_ptr<constructSystem::VacuumPipe>
	VC(new constructSystem::VacuumPipe("VC"));

      OR.addObject(VC);

      VC->addAllInsertCell(voidCell);
      VC->createAll(System,World::masterOrigin(),0);

      return;
    }

  if (item == "WindowPipe" )
    {
      std::shared_ptr<constructSystem::WindowPipe>
	VC(new constructSystem::WindowPipe("VCWin"));

      OR.addObject(VC);

      VC->addAllInsertCell(voidCell);
      VC->createAll(System,World::masterOrigin(),0);

      return;
    }

    if (item == "PipeTube" )
      {
	std::shared_ptr<constructSystem::Bellows>
	  bellowTube(new constructSystem::Bellows("BellowTube"));

	std::shared_ptr<constructSystem::PipeTube>
	  pipeTube(new constructSystem::PipeTube("PipeTube"));

	OR.addObject(bellowTube);
	OR.addObject(pipeTube);

	bellowTube->addInsertCell(voidCell);
	bellowTube->createAll(System,World::masterOrigin(),0);

	pipeTube->setPortRotation(3,Geometry::Vec3D(0,0,1));
	pipeTube->addAllInsertCell(voidCell);
	pipeTube->createAll(System,*bellowTube,"back");

	ELog::EM<<"pipeTube == "<<pipeTube->getCentre()<<ELog::endDiag;
	ELog::EM<<"pipeTube[Y] == "<<pipeTube->getY()<<ELog::endDiag;
	ELog::EM<<"pipeTube[Z] == "<<pipeTube->getZ()<<ELog::endDiag;

	return;
      }
    if (item == "PortTube" )
      {
	std::shared_ptr<constructSystem::PortTube>
	  pipeTube(new constructSystem::PortTube("PortTube"));

	OR.addObject(pipeTube);

	pipeTube->addAllInsertCell(voidCell);
	pipeTube->createAll(System,World::masterOrigin(),0);

	return;
      }

    if (item == "BlankTube" )
      {
	std::shared_ptr<constructSystem::BlankTube>
	  blankTube(new constructSystem::BlankTube("BlankTube"));

	OR.addObject(blankTube);

	blankTube->addAllInsertCell(voidCell);
	blankTube->createAll(System,World::masterOrigin(),0);

	return;
      }

    if (item == "ButtonBPM" )
      {
	std::shared_ptr<tdcSystem::ButtonBPM>
	  buttonBPM(new tdcSystem::ButtonBPM("ButtonBPM"));

	OR.addObject(buttonBPM);

	buttonBPM->addInsertCell(voidCell);
	buttonBPM->createAll(System,World::masterOrigin(),0);

	return;
      }
    if (item == "BremTube" )
      {
	std::shared_ptr<xraySystem::BremTube>
	  bremTube(new xraySystem::BremTube("BremTube"));

	OR.addObject(bremTube);

	bremTube->addInsertCell(voidCell);
	bremTube->createAll(System,World::masterOrigin(),0);

	return;
      }
    if (item == "HPJaws" )
      {
	std::shared_ptr<xraySystem::HPJaws>
	  hp(new xraySystem::HPJaws("HPJaws"));

	OR.addObject(hp);

	hp->addInsertCell(voidCell);
	hp->createAll(System,World::masterOrigin(),0);

	return;
      }
    if (item == "BoxJaws")
      {
	std::shared_ptr<xraySystem::BoxJaws>
	  bj(new xraySystem::BoxJaws("BoxJaws"));

	OR.addObject(bj);

	bj->addInsertCell(voidCell);
	bj->createAll(System,World::masterOrigin(),0);

	return;
      }
    if (item == "DiffPumpXIADP03")
      {
	std::shared_ptr<xraySystem::DiffPumpXIADP03>
	  dp(new xraySystem::DiffPumpXIADP03("DiffXIA"));

	OR.addObject(dp);

	dp->addInsertCell(voidCell);
	dp->createAll(System,World::masterOrigin(),0);

	return;
      }

    if (item == "CRLTube")
      {
	std::shared_ptr<xraySystem::CRLTube>
	  clr(new xraySystem::CRLTube("CRLTube"));
	OR.addObject(clr);

	clr->addAllInsertCell(voidCell);
	clr->createAll(System,World::masterOrigin(),0);

	return;
      }
    if (item == "HPCombine")
      {
	std::shared_ptr<xraySystem::BremTube>
	  bremTube(new xraySystem::BremTube("BremTube"));
	std::shared_ptr<xraySystem::HPJaws>
	  hp(new xraySystem::HPJaws("HPJaws"));

	OR.addObject(bremTube);
	OR.addObject(hp);

	bremTube->addInsertCell(voidCell);
	bremTube->createAll(System,World::masterOrigin(),0);

	hp->addInsertCell(voidCell);
	hp->setFront(*bremTube,2);
	hp->setFlangeJoin();
	hp->createAll(System,*bremTube,"back");

	return;
      }
    if (item == "ViewTube" )
      {
	std::shared_ptr<xraySystem::ViewScreenTube>
	  vt(new xraySystem::ViewScreenTube("ViewTube"));
	std::shared_ptr<tdcSystem::YagScreen>
	  yagScreen(new tdcSystem::YagScreen("YAG"));

	OR.addObject(vt);

	vt->addInsertCell(voidCell);
	vt->createAll(System,World::masterOrigin(),0);

	yagScreen->setBeamAxis(*vt,1);
	yagScreen->createAll(System,*vt,4);
	yagScreen->insertInCell("Outer",System,voidCell);
	yagScreen->insertInCell("Connect",System,vt->getCell("Plate"));
	yagScreen->insertInCell("Connect",System,vt->getCell("Void"));
	yagScreen->insertInCell("Payload",System,vt->getCell("Void"));

	return;
      }

    if (item == "MonoShutter" )
      {
	std::shared_ptr<xraySystem::MonoShutter>
	  ms(new xraySystem::MonoShutter("MS"));

	OR.addObject(ms);

	ms->createAll(System,World::masterOrigin(),0);
	ms->insertAllInCell(System,voidCell);
	return;
      }
    if (item == "RoundMonoShutter" )
      {
	std::shared_ptr<xraySystem::RoundMonoShutter>
	  rs(new xraySystem::RoundMonoShutter("RMS"));

	OR.addObject(rs);

	rs->createAll(System,World::masterOrigin(),0);
	rs->insertAllInCell(System,voidCell);

	return;
      }

    if (item == "ExperimentalHutch")
      {
	std::shared_ptr<xraySystem::ExperimentalHutch>
	  eh(new xraySystem::ExperimentalHutch("ExptHutch"));

	OR.addObject(eh);

	eh->addInsertCell(voidCell);
	eh->createAll(System,World::masterOrigin(),0);

	return;
      }

    if (item == "TubeDetBox")
      {
	std::shared_ptr<constructSystem::TubeDetBox>
	  Box(new constructSystem::TubeDetBox("TDetBox",0));
	OR.addObject(Box);
	
	Box->addInsertCell(voidCell);
	Box->createAll(System,World::masterOrigin(),0);
	return;
      }
    if (item == "GuideUnit" || item=="PlateUnit")
      {
	std::shared_ptr<beamlineSystem::PlateUnit>
	  FA(new beamlineSystem::PlateUnit("FA"));
	OR.addObject(FA);
	
	FA->addInsertCell(voidCell);
	FA->createAll(System,World::masterOrigin(),0);
	return;
      }
    if (item == "BenderUnit")
      {
	std::shared_ptr<beamlineSystem::BenderUnit>
	  BA(new beamlineSystem::BenderUnit("BA"));
	OR.addObject(BA);
	BA->addInsertCell(voidCell);
	BA->createAll(System,World::masterOrigin(),0);
	return;
      }

    if (item=="Help" || item=="help")
      {

	ELog::EM<<"Valid items for single selection:\n"<<ELog::endDiag;

	for(const std::string& Name : validItems)
	  ELog::EM<<"Item : "<<Name<<"\n";

	ELog::EM<<"-----------"<<ELog::endDiag;
      }

  return;
}

}   // NAMESPACE singleItemSystem
