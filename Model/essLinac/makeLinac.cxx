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

#include "FrontEndBuilding.h"
#include "Linac.h"
#include "KlystronGallery.h"
#include "Stub.h"
#include "StubWall.h"
#include "Berm.h"
#include "makeLinac.h"

namespace essSystem
{

makeLinac::makeLinac() :
  feb(new FrontEndBuilding("FEB")),
  LinacTunnel(new Linac("Linac")),
  KG(new KlystronGallery("KG")),
  berm(new Berm("Berm"))
 /*!
    Constructor
 */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(feb);
  OR.addObject(LinacTunnel);
  OR.addObject(KG);
  OR.addObject(berm);
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
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  LinacTunnel->createAll(System,World::masterOrigin(),0);

  feb->addInsertCell(voidCell);
  feb->createAll(System,*LinacTunnel,1,5,15,16,6);
  
  KG->addInsertCell(voidCell);
  KG->createAll(System,*LinacTunnel,0);

  berm->addInsertCell(voidCell);
  berm->createAll(System,*LinacTunnel,0,*KG,4,5);

  attachSystem::addToInsertSurfCtrl(System,*berm,*LinacTunnel);
  attachSystem::addToInsertSurfCtrl(System,*berm,*feb);
  attachSystem::addToInsertSurfCtrl(System,*feb,*LinacTunnel);

  const size_t Nstubs(2);
  for (size_t i=0; i<Nstubs; i++)
    {
      std::shared_ptr<Stub> stub(new Stub("Stub", 100+i*10));
      OR.addObject(stub);
      stub->setFront(*KG,7);
      stub->setBack(*LinacTunnel,-13);
      stub->createAll(System,*LinacTunnel,0);
      attachSystem::addToInsertSurfCtrl(System,*berm,stub->getCC("Full"));
      attachSystem::addToInsertSurfCtrl(System,*LinacTunnel,stub->getCC("Leg1"));
      attachSystem::addToInsertSurfCtrl(System,*KG,stub->getCC("Leg3"));

      std::shared_ptr<StubWall> wall(new StubWall("","StubWall", 100+i*10));
      OR.addObject(wall);
      wall->createAll(System,*stub, 1, *KG, 8);
      attachSystem::addToInsertSurfCtrl(System,*KG,*wall);
    }
  return;
}


}   // NAMESPACE essSystem
