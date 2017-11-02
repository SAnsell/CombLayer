/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   test/testVec3D.cxx
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

#include "testFunc.h"
#include "testVec3D.h"

using namespace Geometry;

testVec3D::testVec3D() 
  /*!
    Constructor
  */
{}

testVec3D::~testVec3D() 
  /*!
    Destructor
  */
{}

int 
testVec3D::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: parameter to decide test
    \retval -1 :: Fail on angle
  */
{
  ELog::RegMethod RegA("testVec3D","applyTest");
  TestFunc::regSector("testVec3D");

  typedef int (testVec3D::*testPtr)();
  testPtr TPtr[]=
    {
      &testVec3D::testDotProd,
      &testVec3D::testRead
    };
  const std::vector<std::string> TestName=
    {
      "DotProd",
      "Read"
    };
  
  const int TSize(sizeof(TPtr)/sizeof(testPtr));
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
testVec3D::testDotProd()
  /*!
    Tests the Vec3D dot prod
    \retval -1 on failure
    \retval 0 :: success 
  */
{
  ELog::RegMethod RegA("testVec3D","testDotProd");

  typedef std::tuple<size_t,size_t,double> TTYPE;
  std::vector<TTYPE> Tests;

  // Set to test between:
  std::vector<Vec3D> VSet;
  VSet.push_back(Vec3D(1,2,0));  
  VSet.push_back(Vec3D(0,0,-1)); 
  VSet.push_back(Vec3D(1,0,0)); 
  VSet.push_back(Vec3D(0,1,0)); 
  VSet.push_back(Vec3D(-1,0,0)); 
  VSet.push_back(Vec3D(0,-1,0)); 
  VSet.push_back(Vec3D(1,-3,-4)); 
  VSet.push_back(Vec3D(7,2,-1)); 
  
  Tests.push_back(TTYPE(0,1,0.0));
  Tests.push_back(TTYPE(0,2,1.0));
  Tests.push_back(TTYPE(0,3,2.0));  
  Tests.push_back(TTYPE(4,0,-1.0));
  Tests.push_back(TTYPE(4,5,0.0));
  Tests.push_back(TTYPE(0,0,5.0));   // test self and two comps
  Tests.push_back(TTYPE(6,7,5.0));   // test self and two comps

  int cnt(1);
  for(const TTYPE& tc : Tests)
    {
      const size_t iA(std::get<0>(tc));
      const size_t iB(std::get<1>(tc));
      const double dp=VSet[iA].dotProd(VSet[iB]);
      if (fabs(dp-std::get<2>(tc))>1e-5)
	{
	  ELog::EM<<"Test Num   "<<cnt<<ELog::endTrace;
	  ELog::EM<<"Vectors == "<<VSet[iA]<<ELog::endTrace;
	  ELog::EM<<"        == "<<VSet[iB]<<ELog::endTrace;
	  ELog::EM<<"Dot prod = "<<dp<<" ("<<std::get<2>(tc)<<")"
		  <<ELog::endTrace;
	  return -1;
	}
      cnt++;
    }
  return 0;
}

int
testVec3D::testRead()
  /*!
    Tests the Vec3D read of a Vec3D 
    \retval -1 on failure
    \retval 0 :: success 
  */
{
  ELog::RegMethod RegA("testVec3D","testReadVec3D");

  typedef std::tuple<std::string,Vec3D> TTYPE;
  std::vector<TTYPE> Tests;
  Tests={
    TTYPE("3,4 8",Vec3D(3,4,8)),
    TTYPE("Vec3D(3,4 8)",Vec3D(3,4,8))
  };

  int cnt(1);
  for(const TTYPE& tc : Tests)
    {
      Geometry::Vec3D A(-1,-1,-1);
      std::istringstream cx(std::get<0>(tc));
      cx>>A;
      
      if (std::get<1>(tc)!=A)
	{
	  ELog::EM<<"Test Num   "<<cnt<<ELog::endDiag;
	  ELog::EM<<"String == "<<std::get<0>(tc)<<ELog::endDiag;
	  ELog::EM<<"Vec3D  == "<<A<<ELog::endDiag;
	  ELog::EM<<"Expect == "<<std::get<1>(tc)<<ELog::endDiag;
	  return -1;
	}
      cnt++;
    }
  return 0;
}

  

