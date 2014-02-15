/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   test/testInsertComp.cxx
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
#include <set>
#include <map>
#include <stack>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "BnId.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Surface.h"
#include "Rules.h"
#include "InsertComp.h"

#include "testFunc.h"
#include "testInsertComp.h"


testInsertComp::testInsertComp() 
///< Constructor
{}

testInsertComp::~testInsertComp() 
///< Destructor
{}

int 
testInsertComp::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: Index of test to run
    \return 0 on success / -ve on error
  */
{
  ELog::RegMethod RegA("testInsertComp","applyTest");
  TestFunc::regSector("testInsertComp");

  typedef int (testInsertComp::*testPtr)();
  testPtr TPtr[]=
    {
      &testInsertComp::testAddition,

    };
  const std::string TestName[]=
    {
      "Addition"
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
testInsertComp::testAddition()
  /*!
    Test adding stuff to the tree.
    \return 0 on success
  */
{
  ELog::RegMethod RegA("testInsertComp","testAddition");
  attachSystem::InsertComp A;
  A.addInterSurf(-7);
  A.addInterSurf(23);
  A.addInterSurf(33);

  return 0;
}

