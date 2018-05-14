/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   test/testExtControl.cxx
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
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <cmath>
#include <complex>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <tuple>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "mathSupport.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "MapRange.h"
#include "EUnit.h"
#include "ExtControl.h"

#include "testFunc.h"
#include "testExtControl.h"

using namespace StrFunc;

testExtControl::testExtControl()
  /*!
    Constructor
   */
{}

testExtControl::~testExtControl()
  /*!
    Destructor
  */
{}

int 
testExtControl::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: Index of test
    \returns -ve on error 0 on success.
  */
{
  ELog::RegMethod RegA("testExtControl","applyTest");
  TestFunc::regSector("testExtControl");

  typedef int (testExtControl::*testPtr)();
  testPtr TPtr[]=
    {
      &testExtControl::testParse
    };

  const std::string TestName[]=
    {
      "Parse"
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
testExtControl::testParse()
  /*!
    Applies a test to convert
    \retval -1 :: failed to convert a good double
    \retval -2 :: converted a number with leading stuff
    \retval -3 :: converted a number with trailing stuff
    \retval 0 on success
  */
{
  ELog::RegMethod RegA("testExtControl","testParse");

  // type : Init string : final : results : (outputs)
  typedef std::tuple<std::string,std::string> TTYPE;
  std::vector<TTYPE> Tests;

  // Test of the split
  Tests.push_back(TTYPE("0.4V1","ext:n 0.4V1"));
  Tests.push_back(TTYPE("SV1","ext:n 0.4V1 SV1"));
  Tests.push_back(TTYPE("0.987654321","ext:n 0.4V1 SV1 0.988"));
  Tests.push_back(TTYPE("-SX","ext:n 0.4V1 SV1 0.988 -SX"));
  Tests.push_back(TTYPE("-0.4V2","ext:n 0.4V1 SV1 0.988 -SX -0.4V2"));
  
  int cnt(1);
  physicsSystem::ExtControl EX;
  std::vector<int> cellOutOrder;
  std::set<int> voidCells;

  for(const TTYPE& tc : Tests)
    {
      std::ostringstream cx;
      cellOutOrder.push_back(cnt);
      EX.addUnit(cnt,std::get<0>(tc));

      EX.write(cx,cellOutOrder,voidCells);
      const std::string Res=StrFunc::singleLine(cx.str());
      if (std::get<1>(tc)!=Res)
	{
	  ELog::EM<<"Failed on test "<<cnt<<ELog::endDiag;
	  ELog::EM<<"Expected :"<<std::get<1>(tc)<<":"<<ELog::endDiag;
	  ELog::EM<<"Obtained :"<<Res<<":"<<ELog::endDiag;
	  return -1;
	}
      cnt++;
    }
  return 0;
}
