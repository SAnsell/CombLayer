/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   test/testVarBlock.cxx
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#include <cmath>
#include <complex>
#include <list>
#include <vector>
#include <map>
#include <string>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Triple.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "varBlock.h"

#include "testFunc.h"
#include "testVarBlock.h"


testVarBlock::testVarBlock() 
  /*!
    Constructor
  */
{}

testVarBlock::~testVarBlock() 
  /*!
    Destructor
  */
{}

int 
testVarBlock::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: index of test to access (-ve for all)
    \retval -ve : Failure number
    \retval 0 : All succeeded
  */
{
  ELog::RegMethod RegA("testVarBlock","applyTest");
  TestFunc::regSector("testVarBlock");

  typedef int (testVarBlock::*testPtr)();
  testPtr TPtr[]=
    { 
      &testVarBlock::testGetValue
    };

  std::string TestName[] = 
    {
      "GetValue"
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
testVarBlock::testGetValue()
  /*!

    Also test the half angle condition
    \retval 0 :: success / -ve on failure
   */
{
  ELog::RegMethod RegA("testVarBlock","testRefraction");


  
  return 0;  
}
