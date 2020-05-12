/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   singleItemBuild/makeSingleItem.cxx
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
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
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedGroup.h"
#include "FixedOffsetGroup.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "LayerComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "InnerZone.h"

#include "World.h"
#include "AttachSupport.h"
#include "insertObject.h"
#include "insertPlate.h"
#include "insertSphere.h"
#include "insertCylinder.h"
#include "insertShell.h"

#include "Cryostat.h"
#include "TwinBase.h"
#include "TwinChopper.h"
#include "TwinChopperFlat.h"
#include "SingleChopper.h"
#include "DiskChopper.h"
#include "VacuumPipe.h"

#include "CryoMagnetBase.h"
#include "Dipole.h"
#include "Quadrupole.h"
#include "Sexupole.h"
#include "Octupole.h"
#include "LQuad.h"
#include "CorrectorMag.h"
#include "EPSeparator.h"
#include "QuadUnit.h"
#include "DipoleChamber.h"
#include "R3ChokeChamber.h"
#include "EPCombine.h"
#include "PreDipole.h"
#include "MagnetM1.h"
#include "MagnetBlock.h"
#include "CylGateValve.h"
#include "DipoleDIBMag.h"
#include "EArrivalMon.h"
#include "YagScreen.h"

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
      "default","CylGateValve","CorrectorMag","LQuad",
      "MagnetBlock","MagnetM1","Octupole","EPSeparator",
      "R3ChokeChamber","QuadUnit","DipoleChamber",
      "EPSeparator","Quadrupole","TargetShield",
      "DipoleDIBMag","EArrivalMon","YagScreen",
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

  if (item == "YagScreen")
    {
      std::shared_ptr<tdcSystem::YagScreen>
	YAG(new tdcSystem::YagScreen("YAG"));
      OR.addObject(YAG);

      YAG->addAllInsertCell(voidCell);
      YAG->createAll(System,World::masterOrigin(),0);

      return;
    }

  if (item == "CorrectorMag" )
    {
      std::shared_ptr<constructSystem::VacuumPipe>
	VC(new constructSystem::VacuumPipe("VC"));
      std::shared_ptr<tdcSystem::CorrectorMag>
	CM(new tdcSystem::CorrectorMag("CM","CM"));

      OR.addObject(VC);
      OR.addObject(CM);

      VC->addInsertCell(voidCell);
      VC->createAll(System,World::masterOrigin(),0);

      CM->setCutSurf("Inner",*VC,"outerPipe");
      CM->addInsertCell(voidCell);
      CM->createAll(System,World::masterOrigin(),0);

      return;
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

  if (item=="LQuad")
    {
      std::shared_ptr<tdcSystem::LQuad>
	LQ(new tdcSystem::LQuad("LQ","LQ"));

      OR.addObject(LQ);

      LQ->addInsertCell(voidCell);
      LQ->createAll(System,World::masterOrigin(),0);

      return;
    }

  if (item=="MagnetBlock")
    {

      std::shared_ptr<xraySystem::MagnetBlock>
	MB(new xraySystem::MagnetBlock("M1"));

      OR.addObject(MB);
      MB->addInsertCell(voidCell);
      MB->createAll(System,World::masterOrigin(),0);

      return;
    }

  if (item=="Seupole")
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

  if (item=="R3ChokeChamber")
    {
      std::shared_ptr<xraySystem::R3ChokeChamber>
	CChamber(new xraySystem::R3ChokeChamber("R3Chamber"));
      OR.addObject(CChamber);
      CChamber->addAllInsertCell(voidCell);
      CChamber->createAll(System,World::masterOrigin(),0);

      return;
    }
  if (item=="")
    {
      std::shared_ptr<xraySystem::R3ChokeChamber>
	CChamber(new xraySystem::R3ChokeChamber("R3Chamber"));
      OR.addObject(CChamber);
      CChamber->addAllInsertCell(voidCell);
      CChamber->createAll(System,World::masterOrigin(),0);

      return;
    }

  if (item=="QuadUnit" || item=="DipoleChamber" || item=="EPSeparator")
    {
      std::shared_ptr<xraySystem::QuadUnit>
	PDipole(new xraySystem::QuadUnit("PreDipole"));
      OR.addObject(PDipole);
      PDipole->addInsertCell(voidCell);
      PDipole->createAll(System,World::masterOrigin(),0);


      std::shared_ptr<xraySystem::DipoleChamber>
	DCSep(new xraySystem::DipoleChamber("DipoleChamber"));
      OR.addObject(DCSep);
      DCSep->addAllInsertCell(voidCell);
      DCSep->createAll(System,*PDipole,2);

      std::shared_ptr<xraySystem::EPSeparator>
	EPSep(new xraySystem::EPSeparator("EPSep"));
      OR.addObject(EPSep);
      EPSep->addInsertCell(voidCell);
      EPSep->createAll(System,*PDipole,2);
      return;
    }

  if (item=="Quadrupole")
    {

      std::shared_ptr<xraySystem::Quadrupole>
	Quad(new xraySystem::Quadrupole("Quad","Quad"));
      OR.addObject(Quad);
      Quad->addInsertCell(voidCell);
      Quad->createAll(System,World::masterOrigin(),0);
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

      TubeA->addInsertCell(voidCell);
      TubeA->createAll(System,World::masterOrigin(),0);
      TubeB->addInsertCell(voidCell);
      TubeB->createAll(System,*TubeA,2);
      return;
    }


  if (item=="DipoleDIBMag")
    {
      std::shared_ptr<constructSystem::VacuumPipe>
	VC(new constructSystem::VacuumPipe("VC"));
      std::shared_ptr<tdcSystem::DipoleDIBMag>
	DIB(new tdcSystem::DipoleDIBMag("DIB"));

      OR.addObject(VC);
      OR.addObject(DIB);

      VC->addInsertCell(voidCell);
      VC->createAll(System,World::masterOrigin(),0);

      DIB->setCutSurf("Inner",*VC,"outerPipe");
      DIB->addInsertCell(voidCell);
      DIB->createAll(System,World::masterOrigin(),0);

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
