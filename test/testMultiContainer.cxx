/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   test/testMultiContainer.cxx
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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
#include <cmath>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <stack>
#include <map>
#include <tuple>
#include <memory>
#include <numeric>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "dataSlice.h"
#include "multiData.h"
#include "IndexCounter.h"

#include "testFunc.h"
#include "testMultiContainer.h"


testMultiContainer::testMultiContainer() 
  /*!
    Constructor
  */
{}

testMultiContainer::~testMultiContainer() 
  /*!
    Destructor
  */
{}

int 
testMultiContainer::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: parameter to decide test
    \retval -1 :: Fail on angle
  */
{
  ELog::RegMethod RegA("testMultiContainer","applyTest");
  TestFunc::regSector("testMultiContainer");

  typedef int (testMultiContainer::*testPtr)();
  testPtr TPtr[]=
    {
      &testMultiContainer::testRange,
      &testMultiContainer::testSet
    };
  const std::vector<std::string> TestName=
    {
      "Range",
      "Set"
    };
  
  const size_t TSize(sizeof(TPtr)/sizeof(testPtr));
  if (!extra) 
    {
      TestFunc::writeTests(TestName);
      return 0;
    }
  for(size_t i=0;i<TSize;i++)
    {
      if (extra<0 || static_cast<size_t>(extra)==i+1)
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
testMultiContainer::testRange()
  /*!
    Tests the Vec3D dot prod
    \retval -1 on failure
    \retval 0 :: success 
  */
{
  ELog::RegMethod RegA("testMultiContainer","testRange");
  return 0;
}

int
testMultiContainer::testSet()
  /*!
    Tests the Vec3D dot prod
    \retval -1 on failure
    \retval 0 :: success 
  */
{
  ELog::RegMethod RegA("testMultiContainer","testSet");

  multiData<std::string> Out(30,4,6);

  multiData<std::string> Out1(10);
  Out1.get()[2]=std::string("10");
  std::string unit("03");
  Out1.get()[2]+=unit;

  Out1.get()[3]=Out1.get()[2];
  for(size_t i=0;i<10;i++)
    {
      //    if (Out1.get()[i] != std::string(""))
      std::string item=Out1.get()[i];
      ELog::EM<<"OutONE["<<i<<"] == "<<
	item<<ELog::endDiag;
    }
  /*
  std::vector<std::string> V;
  for(size_t i=0;i<30;i++)
    V.push_back(std::to_string(i));
  
  multiData<std::string> Out(30,V);
  
  for(size_t i=0;i<30;i++)
    {
      std::string item=Out.get()[i];
      ELog::EM<<"Out["<<i<<"] "<<item
	      <<ELog::endDiag;
    }
  */
  return 0;
}
