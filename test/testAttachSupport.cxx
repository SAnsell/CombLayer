/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   test/testAttachSupport.cxx
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Transform.h"
#include "Surface.h"
#include "Rules.h"
#include "surfRegister.h"
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

#include "Debug.h"

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

  ASim.resetAll();
  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();
  // Work Sphere :
  SurI.createSurface(100,"so 500");
  ASim.addCell(MonteCarlo::Qhull(1,0,0.0,"100"));  // Outside void 
  ASim.addCell(MonteCarlo::Qhull(5001,0,0.0,"-100"));  // Inside void 
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
      &testAttachSupport::testBoundaryValid,
      &testAttachSupport::testInsertComponent
    };
  const std::string TestName[]=
    {
      "BoundaryValid",
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
testAttachSupport::testBoundaryValid() 
  /*!
    Test the situation that a point is within/outof the boundary
    \return 0 on success
  */
{
  ELog::RegMethod RegA("testAttachSupport","testBoundaryValid");

  
  typedef std::tuple<size_t,Geometry::Vec3D,int> TTYPE;
  std::vector<TTYPE> Tests;
  Tests.push_back(TTYPE(0,Geometry::Vec3D(0,0,0),0));
  Tests.push_back(TTYPE(0,Geometry::Vec3D(1000,0,0),1));
  Tests.push_back(TTYPE(1,Geometry::Vec3D(0,0,0),0));
  Tests.push_back(TTYPE(1,Geometry::Vec3D(1000,0,0),1));
  
  typedef int (attachSystem::ContainedComp::* MPtr)
    (const Geometry::Vec3D&) const;
  MPtr TPtr[]=
    {
      &ContainedComp::isBoundaryValid,
      &ContainedComp::isOuterValid
    };

  std::shared_ptr<testSystem::simpleObj> 
    CC(new testSystem::simpleObj("A"));
  CC->createAll(ASim,World::masterOrigin());


  int cnt(1);
  for(const TTYPE& tc : Tests)
    {
      const int R=((*CC).*TPtr[std::get<0>(tc)])(std::get<1>(tc));
      if (R!=std::get<2>(tc))
	{
	  ELog::EM<<"Failed Test "<<cnt<<ELog::endTrace;
	  ELog::EM<<"Result["<< std::get<2>(tc)<<"] == "<<R<<ELog::endTrace;
	  ELog::EM<<"Point["<< std::get<1>(tc)<<"] == "<<R<<ELog::endTrace;
	  return -1;
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
  SObj.back()->createAll(ASim,World::masterOrigin());

  return 0;
}



