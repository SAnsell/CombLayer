/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   test/testPipeUnit.cxx
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>
#include <complex>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <vector>
#include <algorithm>
#include <functional>
#include <memory>
 
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "SimMCNP.h"
#include "ModelSupport.h"
#include "ContainedComp.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedUnit.h"
#include "BaseMap.h"
#include "CellMap.h"

#include "pipeUnit.h"

#include "testFunc.h"
#include "testPipeUnit.h"

using namespace ModelSupport;

testPipeUnit::testPipeUnit()
  /*!
    Constructor
  */
{}

testPipeUnit::~testPipeUnit() 
  /*!
    Destructor
  */
{}

void
testPipeUnit::initSim()
  /*!
    Set all the objects in the simulation:
  */
{
  ASim.resetAll();
  createSurfaces();
  createObjects();
  ASim.createObjSurfMap();
    
  return;
}

void 
testPipeUnit::createSurfaces()
  /*!
    Create the surface list
   */
{
  ELog::RegMethod RegA("testPipeUnit","createSurfaces");

  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();
  
  // Outer sphere
  SurI.createSurface(100,"so 50");
  return;
}

void
testPipeUnit::createObjects()
  /*!
    Create Object for test
  */
{
  ELog::RegMethod RegA("testPipeUnit","createObjects");

  std::string Out;
  int cellIndex(2);
  const int surIndex(0);

  Out=ModelSupport::getComposite(surIndex,"-100");
  ASim.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));

  MonteCarlo::Object* outerObj=ASim.findObject(74123);
  if (!outerObj)
    ELog::EM<<"NO OUTER CELL "<<ELog::endErr;
  outerObj->addIntersection(HeadRule(" 100 "));

  return;
}

int 
testPipeUnit::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: Index of test
    \returns -ve on error 0 on success.
  */
{
  ELog::RegMethod RegA("testPipeUnit","applyTest");
  TestFunc::regSector("testPipeUnit");

  typedef int (testPipeUnit::*testPtr)();
  testPtr TPtr[]=
    {
      &testPipeUnit::testBasic
    };

  const std::string TestName[]=
    {
      "Basic"
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
	  initSim();
	  TestFunc::regTest(TestName[i]);
	  const int retValue= (this->*TPtr[i])();
	  if (retValue || extra>0)
	    return retValue;
	}
    }
  return 0;
}

int
testPipeUnit::testBasic()
  /*!
    Test to see if an isolated pipe can exist
    \returns 0 on succes and -ve on failure
  */
{
  ELog::RegMethod RegA("testPipeUnit","testBasic");

  std::vector<cylValues> CV;
  CV.push_back(cylValues(5.0,3,0));

  pipeUnit AP("tst",1);
  AP.setPoints(Geometry::Vec3D(0,0,0),Geometry::Vec3D(10.0,0,0));

  ASim.populateCells();
  AP.buildUnit(ASim,0,CV);
  return 0;
}

