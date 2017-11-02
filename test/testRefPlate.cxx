/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   test/testRefPlate.cxx
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
#include <cmath>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <stack>
#include <string>
#include <sstream>
#include <algorithm>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfIndex.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "varList.h"
#include "Code.h"
#include "FItem.h"
#include "FuncDataBase.h"
#include "ContainedComp.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "simpleObj.h"
#include "Simulation.h"
#include "World.h"
#include "refPlate.h"

#include "testFunc.h"
#include "testRefPlate.h"

using namespace testSystem;

testRefPlate::testRefPlate() 
  /*!
    Constructor
  */
{}

testRefPlate::~testRefPlate() 
  /*!
    Destructor
  */
{}

void
testRefPlate::initSim()
  /*!
    Set all the objects in the simulation:
  */
{
  ELog::RegMethod RegA("testRefPlate","initSim");

  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();
  ModelSupport::objectRegister& OR=ModelSupport::objectRegister::Instance();

  SObj=std::shared_ptr<testSystem::simpleObj>(new simpleObj("testSimple"));
  OR.reset();
  ASim.resetAll();


  // Work Sphere :
  SurI.createSurface(100,"so 500");
  ASim.addCell(MonteCarlo::Qhull(1,0,0.0,"100"));  // Outside void 
  ASim.addCell(MonteCarlo::Qhull(5001,0,0.0,"-100"));  // Inside void 
  SObj->setInsertCell(5001);
  SObj->createAll(ASim,World::masterOrigin());
  return;
}

int 
testRefPlate::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: Test number to run
    \retval -1 : SetObject 
    \retval 0 : All succeeded
  */
{
  ELog::RegMethod RegA("testRefPlate","applyTest");
  TestFunc::regSector("testRefPlate");

  typedef int (testRefPlate::*testPtr)();
  testPtr TPtr[]=
    {
      &testRefPlate::testAddBlock,
      &testRefPlate::testAddTwoBlocks,
      &testRefPlate::testArrayBlocks
    };
  const std::string TestName[]=
    {
      "AddBlock",
      "AddTwoBlocks",
      "AddArrayBlocks"
    };
  
  const int TSize(sizeof(TPtr)/sizeof(testPtr));
  if (!extra)
    {
      std::ios::fmtflags flagIO=std::cout.setf(std::ios::left);
      for(int i=0;i<TSize;i++)
        {
	  std::cout<<std::setw(30)<<TestName[i]<<"("<<i+1<<")"<<std::endl;
	}
      std::cout.flags(flagIO);
      return 0;
    }
  for(int i=0;i<TSize;i++)
    {
      if (extra<0 || extra==i+1)
        {
	  TestFunc::regTest(TestName[i]);
	  initSim();
	  const int retValue= (this->*TPtr[i])();
	  if (retValue || extra>0)
	    return retValue;
	}
    }
  return 0;
}

int
testRefPlate::testAddBlock()
  /*!
    Test adding a block to the system
    \return -ve on error
  */
{
  ELog::RegMethod RegA("testRefPlate","testAddBlock");

  ts1System::refPlate RP("testPlate1");
  
  RP.setOrigin(*SObj,2);
  RP.setPlane("X",*SObj,-4);
  RP.setPlane("-X",*SObj,-3);
  RP.setPlane("-Z",*SObj,-5);
  RP.setPlane("Z",*SObj,-6);
  RP.setPlane("Y",2.3);

  RP.setInsertCell(SObj->getCell());
  RP.createAll(ASim);

  ASim.renumberAll();
  ASim.write("testRP.x");
  return 0;
}

int
testRefPlate::testAddTwoBlocks()
  /*!
    Test adding a block to the system
    \return -ve on error
  */
{
  ELog::RegMethod RegA("testRefPlate","testAddBlock");

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  std::shared_ptr<ts1System::refPlate> 
    RA(new ts1System::refPlate("testPlate1"));  
  OR.addObject(RA);

  RA->setOrigin(*SObj,2);
  RA->setPlane("X",*SObj,-4);
  RA->setPlane("-X",*SObj,-3);
  RA->setPlane("-Z",*SObj,-5);
  RA->setPlane("Z",*SObj,-6);
  RA->setPlane("Y",2.3);

  RA->setInsertCell(SObj->getCell());
  RA->createAll(ASim);
  
  // SECOND BLOCK:
  
  std::shared_ptr<ts1System::refPlate> 
    RB(new ts1System::refPlate("testPlate2"));  
  OR.addObject(RB);
  
  RB->setOrigin(*SObj,4);
  RB->setPlane("X",*RA,4);
  RB->setPlane("-X",*SObj,-1);
  RB->setPlane("-Z",*SObj,-5);
  RB->setPlane("Z",*SObj,-6);
  RB->setPlane("Y",2.3);

  RB->setInsertCell(SObj->getCell());
  RB->createAll(ASim);

  // THIRD BLOCK:
  
  std::shared_ptr<ts1System::refPlate> 
    RC(new ts1System::refPlate("testPlate3"));  
  OR.addObject(RC);
  
  RC->setOrigin(*SObj,1);
  RC->setPlane("X",*RB,4);
  RC->setPlane("-X",*SObj,-4);
  RC->setPlane("-Z",*SObj,-5);
  RC->setPlane("Z",*SObj,-6);
  RC->setPlane("Y",2.3);

  RC->setInsertCell(SObj->getCell());
  RC->createAll(ASim);
  
  ASim.renumberAll();
  ASim.write("testRP.x");
  return 0;
}

int
testRefPlate::testArrayBlocks()
  /*!
    Test adding a block to the system
    \return -ve on error
  */
{
  ELog::RegMethod RegA("testRefPlate","testAddBlock");

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  typedef std::shared_ptr<ts1System::refPlate> PTYPE;
  std::vector<PTYPE> RVec;
  
  const std::string rpName("testPlate");
  for(int i=0;i<7;i++)
    {
      RVec.push_back(PTYPE(new ts1System::refPlate
			   (StrFunc::makeString(rpName,i+1))));  
      OR.addObject(RVec.back());
    }
 
  RVec[0]->setOrigin(*SObj,2);
  RVec[0]->setPlane("-X",*SObj,-3);
  RVec[0]->setPlane("X",*SObj,-4);
  RVec[0]->setPlane("Y",2.3);

  RVec[1]->setOrigin(*SObj,4);
  RVec[1]->setPlane("-X",*SObj,-1);
  RVec[1]->setPlane("X",*RVec[0],4);
  RVec[1]->setPlane("Y",2.3);

  RVec[2]->setOrigin(*SObj,1);
  RVec[2]->setPlane("-X",*SObj,-3);
  RVec[2]->setPlane("X",*RVec[1],4);
  RVec[2]->setPlane("Y",2.3);

  RVec[3]->setOrigin(*SObj,3);
  RVec[3]->setPlane("-X",*RVec[0],3);
  RVec[3]->setPlane("X",*RVec[2],4);
  RVec[3]->setPlane("Y",2.3);

  for(size_t i=0;i<4;i++)
    {
      RVec[i]->setPlane("-Z",*SObj,-5);
      RVec[i]->setPlane("Z",*SObj,-6);
    }
  
  for(size_t i=4;i<6;i++)
    {
      ELog::EM<<"RVE == "<<i<<ELog::endDiag;
      RVec[i]->setOrigin(*SObj,static_cast<long int>(i));
      RVec[i]->setPlane("-X",*RVec[0],4);
      RVec[i]->setPlane("X",*RVec[2],4);
      RVec[i]->setPlane("-Z",*RVec[1],4);
      RVec[i]->setPlane("Z",*RVec[3],4);
      RVec[i]->setPlane("Y",2.3);
    }

  for(size_t i=0;i<6;i++)
    {
      ELog::EM<<"ITEM:"<<i<<ELog::endDiag;
      RVec[i]->setInsertCell(SObj->getCell());
      RVec[i]->createAll(ASim);
    }
  
  ASim.renumberAll();
  ASim.write("testRP.x");
  return 0;
}


