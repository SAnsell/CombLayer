/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   test/testAttachSupport.cxx
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. 
 *
 ****************************************************************************/
#include <fstream>
#include <iomanip>
#include <iostream>
#include <cmath>
#include <vector>
#include <list>
#include <deque>
#include <set>
#include <map>
#include <stack>
#include <string>
#include <sstream>
#include <algorithm>
#include <memory>
#include <tuple>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "surfIndex.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "ContainedComp.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "simpleObj.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "SimMCNP.h"
#include "World.h"

#include "testFunc.h"
#include "testAttachSupport.h"

using namespace attachSystem;

testAttachSupport::testAttachSupport() 
  /*!
    Constructor
  */
{}

testAttachSupport::~testAttachSupport() 
  /*!
    Destructor
  */
{
  ASim.resetAll();
}

void
testAttachSupport::initSim()
  /*!
    Set all the objects in the simulation:
  */
{
  ELog::RegMethod RegA("testAttachSupport","initSim");

  // initialize world
  // this code is also in MainProcess::buildWorld
  ASim.resetAll();
  
  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();
  // Work Sphere :
  SurI.createSurface(100,"so 500");
  ASim.addCell(MonteCarlo::Object(5001,0,0.0,"-100"));  // Inside void

  return;
}


int 
testAttachSupport::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: Test number to run
    \retval -1 : SetObject 
    \retval 0 : All succeeded
  */
{
  ELog::RegMethod RegA("testAttachSupport","applyTest");
  TestFunc::regSector("testAttachSupport");

  typedef int (testAttachSupport::*testPtr)();
  testPtr TPtr[]=
    {
      &testAttachSupport::testInsertComponent
    };
  const std::string TestName[]=
    {
      "InsertComponent"
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
	  const int retValue= (this->*TPtr[i])();
	  if (retValue || extra>0)
	    return retValue;
	}
    }
  return 0;
}


int
testAttachSupport::testInsertComponent()
  /*!
    Test the CellStr method including the 
    complementary of both \#(XXX) and \#Cell.
    \retval 0 :: success
  */
{
  ELog::RegMethod RegA("testAttachSupport","testInsertComponent");

  initSim();
  SObj.push_back(SOTYPE(new testSystem::simpleObj("A")));
  SObj.back()->addInsertCell(5001);
  SObj.back()->createAll(ASim,World::masterOrigin(),0);

  return 0;
}



