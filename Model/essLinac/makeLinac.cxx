/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuild/makeLinac.cxx
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
#include "support.h"
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
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "World.h"
#include "AttachSupport.h"
#include "LinkSupport.h"
#include "FrontBackCut.h"

#include "Linac.h"
#include "KlystronGallery.h"
#include "Stub.h"
#include "Berm.h"
#include "makeLinac.h"

namespace essSystem
{

makeLinac::makeLinac() :
  LinacTunnel(new Linac("Linac")),
  KG(new KlystronGallery("KG")),
  berm(new Berm("Berm")),
  stub(new Stub("Stub",1))
 /*!
    Constructor
 */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(LinacTunnel);
  OR.addObject(KG);
  OR.addObject(berm);
  OR.addObject(stub);
}


makeLinac::~makeLinac()
  /*!
    Destructor
  */
{}

void
makeLinac::build(Simulation& System,
	       const mainSystem::inputParam& IParam)
  /*!
    Carry out the full build
    \param System :: Simulation system
    \param IParam :: Input parameters
   */
{
  // For output stream
  ELog::RegMethod RegA("makeLinac","build");

  int voidCell(74123);

  LinacTunnel->createAll(System,World::masterOrigin(),0);

  KG->addInsertCell(voidCell);
  KG->createAll(System,*LinacTunnel,0);

  berm->addInsertCell(voidCell);
  berm->createAll(System,*LinacTunnel,0,*KG,3,5);

  attachSystem::addToInsertSurfCtrl(System,*berm,*LinacTunnel);

  stub->setFront(*KG,-7);
  stub->setBack(*LinacTunnel,-14);
  stub->createAll(System,*LinacTunnel,0);

  attachSystem::addToInsertSurfCtrl(System,*berm,stub->getCC("Full"));
  attachSystem::addToInsertSurfCtrl(System,*LinacTunnel,stub->getCC("Leg1"));
  attachSystem::addToInsertSurfCtrl(System,*KG,stub->getCC("Leg3"));

  return;
}


}   // NAMESPACE essSystem
