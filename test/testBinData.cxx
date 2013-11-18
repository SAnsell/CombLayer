/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   test/testBinData.cxx
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#include <cmath>
#include <string>
#include <vector>
#include <complex>
#include <map>
#include <algorithm>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "support.h"
#include "doubleErr.h"
#include "mathSupport.h"
#include "BUnit.h"
#include "Boundary.h"
#include "BinData.h"

#include "testFunc.h"
#include "testBinData.h"


testBinData::testBinData() 
  /*!
    Constructor
  */
{}

testBinData::~testBinData() 
  /*!
    Destructor
  */
{}

int 
testBinData::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: Test number to run
    \returns -ve on error 0 on success.
  */
{
  ELog::RegMethod RegA("testBinData","applyTest");
  TestFunc::regSector("testBinData");

  typedef int (testBinData::*testPtr)();
  testPtr TPtr[]=
    {
      &testBinData::testAdd
    };
  const std::string TestName[]=
    {
      "Add"
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
testBinData::testAdd()
  /*!
    Test the addition of two data sets
    \return 0 on success
  */
{
  ELog::RegMethod RegA("testBinData","testAdd");
  BinData A;
  BinData B;
  
  for(int i=0;i<10;i++)
    {
      A.addData(i+0.0,i+1.0,1.0);
      B.addData(i+0.5,i+1.5,10.0); 
    }
  A+=B;
  const std::vector<BUnit>& AData=A.getData();
  
  for(size_t i=0;i<AData.size();i++)
    if ((i==0 && AData[i].Y!=6.0) ||
	(i!=0 && AData[i].Y!=11.0))
      {
	ELog::EM<<"["<<i<<"] == "<<AData[i]<<ELog::endDiag;
	return -1;
      }
  return 0;
}
