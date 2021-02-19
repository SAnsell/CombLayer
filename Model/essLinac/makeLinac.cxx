/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuild/makeLinac.cxx
 *
 * Copyright (c) 2004-2021 by Stuart Ansell / Konstantin Batkov
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
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedGroup.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "ExternalCut.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "PointMap.h"
#include "World.h"
#include "AttachSupport.h"
#include "LinkSupport.h"
#include "FrontBackCut.h"

#include "FrontEndBuilding.h"
#include "Linac.h"
#include "KlystronGallery.h"
#include "Stub.h"
#include "Berm.h"
#include "RFQ.h"
#include "makeLinac.h"
#include "StubWall.h"
#include "MEBT.h"
#include "CryoTransferLine.h"

namespace essSystem
{

makeLinac::makeLinac() :
  feb(new FrontEndBuilding("FEB")),
  LinacTunnel(new Linac("Linac")),
  KG(new KlystronGallery("KG")),
  berm(new Berm("Berm")),
  rfq(new RFQ("RFQ")),
  mebt(new MEBT("MEBT"))
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
  OR.addObject(rfq);
  OR.addObject(mebt);
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

  LinacTunnel->addInsertCell(voidCell);
  LinacTunnel->createAll(System,World::masterOrigin(),0);

  
  feb->addInsertCell(voidCell);

  feb->setCutSurf("floorLow",LinacTunnel->getFullRule(5));
  feb->setCutSurf("floorTop",LinacTunnel->getFullRule(15));
  feb->setCutSurf("roofLow",LinacTunnel->getFullRule(16));
  feb->setCutSurf("roofTop",LinacTunnel->getFullRule(6));
  feb->setCutSurf("leftSide",LinacTunnel->getFullRule(3));
  feb->setCutSurf("rightSide",LinacTunnel->getFullRule(4));
  feb->setCutSurf("FullUnit",LinacTunnel->getOuterSurf());

  feb->setPoint("floorLow",LinacTunnel->getLinkPt(5));
  feb->setPoint("floorTop",LinacTunnel->getLinkPt(15));
  feb->setPoint("roofLow",LinacTunnel->getLinkPt(16));
  feb->setPoint("roofTop",LinacTunnel->getLinkPt(6));

  feb->createAll(System,*LinacTunnel,1);

  ELog::EM<<"WARNING UNNECESSARLY COMPLEX INSERT ==> Use ExternalCut instead"
	  <<ELog::endWarn;

  //  attachSystem::addToInsertSurfCtrl(System,*berm,*LinacTunnel);
  //  attachSystem::addToInsertSurfCtrl(System,*berm,*feb);

  ELog::EM<<"EARLY RETURN"<<ELog::endCrit;
  return;


  KG->addInsertCell(voidCell);
  KG->createAll(System,*LinacTunnel,0);

  berm->addInsertCell(voidCell);
  berm->setCutSurf("ExternalSide",*KG,3);
  berm->setCutSurf("Floor",*KG,5);
  berm->createAll(System,*LinacTunnel,0);

  // Add RFQ
  //rfq->createAll(System,World::masterOrigin(),0);

  
  //attachSystem::addToInsertSurfCtrl(System,*feb,*rfq); //To add the RFQ

  //mebt->createAll(System,*rfq,1);
  //attachSystem::addToInsertControl(System,*feb,*mebt); //To add the MEBT

  const size_t nStubs(LinacTunnel->getNStubs());
  for (size_t i=0; i<nStubs; i++) //-27 to get no stubs
    {
      if(i>=20){ 
	const size_t stubNumber(100+i*10);
	const size_t active = System.getDataBase().EvalDefVar<size_t>
	  ("Stub"+std::to_string(stubNumber)+"Active", 1);
	if (active)
	  {
	    std::shared_ptr<Stub> stub(new Stub("Stub", stubNumber));
	    OR.addObject(stub);
	    stub->setFront(*KG,7);
	    stub->setBack(*LinacTunnel,-14);
	    stub->createAll(System,*LinacTunnel,0);
	    attachSystem::addToInsertSurfCtrl(System,*berm,stub->getCC("Full"));
	    attachSystem::addToInsertSurfCtrl(System,*LinacTunnel,
					      stub->getCC("Leg1"));
	    attachSystem::addToInsertSurfCtrl(System,*KG,stub->getCC("Leg3"));
	    
	    
	    // std::shared_ptr<StubWall> wall
	    //  (new StubWall("","StubWall",100+i*10));
	    //OR.addObject(wall);
	    //wall->setCutSurf("floor",*KG,8);
	    //wall->createAll(System,*stub,1);
	    //attachSystem::addToInsertSurfCtrl(System,*KG,*wall);
	    
	  }
      }
    }
  // Build the CryoTransferLine
  
  std::shared_ptr<CryoTransferLine> cryoTransferLine
    (new CryoTransferLine("CryoTransferLine"));
  OR.addObject(cryoTransferLine);
  cryoTransferLine->setFront(*KG,7);
  cryoTransferLine->setBack(*LinacTunnel,-14);
  cryoTransferLine->createAll(System,*LinacTunnel,0);
  
  attachSystem::addToInsertSurfCtrl(System,*berm,cryoTransferLine->getCC("Full"));
  attachSystem::addToInsertSurfCtrl(System,*LinacTunnel,
  				    cryoTransferLine->getCC("Leg1"));
    	    
  
  return;
}


}   // NAMESPACE essSystem
