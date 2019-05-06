/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   singleItemBuild/makeSingleItem.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#include "Quadrupole.h"
#include "EPSeparator.h"
#include "PreDipole.h"
#include "DipoleChamber.h"
#include "R3ChokeChamber.h"
#include "EPCombine.h"
#include "PreBendPipe.h"
#include "MagnetM1.h"

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

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  const int voidCell(74123);

  std::shared_ptr<xraySystem::MagnetM1>
    MagBlock(new xraySystem::MagnetM1("M1Block"));
  OR.addObject(MagBlock);

  MagBlock->addInsertCell(voidCell);
  MagBlock->createAll(System,World::masterOrigin(),0);
  
  return;
  
  std::shared_ptr<xraySystem::EPSeparator>
    EPsep(new xraySystem::EPSeparator("EPSeparator"));
  OR.addObject(EPsep);
  
  EPsep->addInsertCell(voidCell);
  EPsep->createAll(System,World::masterOrigin(),0);

  return;
  
  
  std::shared_ptr<xraySystem::R3ChokeChamber>
    CChamber(new xraySystem::R3ChokeChamber("R3Chamber"));
  OR.addObject(CChamber);
  CChamber->addAllInsertCell(voidCell);
  CChamber->createAll(System,World::masterOrigin(),0);

  return;
  

  
  std::shared_ptr<xraySystem::PreDipole>
    PDipole(new xraySystem::PreDipole("PreDipole"));
  OR.addObject(PDipole);
  PDipole->addInsertCell(voidCell);
  PDipole->createAll(System,World::masterOrigin(),0);


  std::shared_ptr<xraySystem::DipoleChamber>
    DCSep(new xraySystem::DipoleChamber("DipoleChamber"));
  OR.addObject(DCSep);
  DCSep->addAllInsertCell(voidCell);
  DCSep->createAll(System,*PDipole,2);
  return;

  std::shared_ptr<xraySystem::EPSeparator>
    EPSep(new xraySystem::EPSeparator("EPSep"));
  OR.addObject(EPSep);
  EPSep->addInsertCell(voidCell);
  EPSep->createAll(System,*PDipole,2);
  return;

  std::shared_ptr<xraySystem::Quadrupole>
    Quad(new xraySystem::Quadrupole("Quad","Quad"));
  OR.addObject(Quad);
  Quad->addInsertCell(voidCell);
  Quad->createAll(System,World::masterOrigin(),0);
  return;


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
  
	    
	  
  Target->addInsertCell(voidCell);
  Target->createAll(System,World::masterOrigin(),0);

  Surround->addInsertCell(voidCell);
  Surround->createAll(System,World::masterOrigin(),0);

  TubeA->addInsertCell(voidCell);
  TubeA->addInsertCell(Surround->getCell("Main"));
  TubeA->createAll(System,World::masterOrigin(),0);


  //  constructSystem::SingleChopper AS("singleChopper");
  //  AS.addInsertCell(voidCell);
  //  AS.createAll(System,World::masterOrigin(),0);

//  constructSystem::CryoMagnetBase A("CryoB");
//  A.addInsertCell(voidCell);
//  A.createAll(System,World::masterOrigin(),0);
}


}   // NAMESPACE singleItemSystem

