/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   test/testVec3D.cxx
 *
 * Copyright (c) 2004-2024 by Stuart Ansell
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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "M3.h"

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
      &testVec3D::testOuterProduct,
      &testVec3D::testRead
    };
  const std::vector<std::string> TestName=
    {
      "DotProd",
      "OuterProduct",
      "Read"
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
testVec3D::testDotProd()
  /*!
    Tests the Vec3D dot prod
    \retval -1 on failure
    \retval 0 :: success 
  */
{
  ELog::RegMethod RegA("testVec3D","testDotProd");

  typedef std::tuple<size_t,size_t,double> TTYPE;
  const std::vector<TTYPE> Tests({
      TTYPE(0,1,0.0),
      TTYPE(0,2,1.0),
      TTYPE(0,3,2.0),  
      TTYPE(4,0,-1.0),
      TTYPE(4,5,0.0),
      TTYPE(0,0,5.0),   // test self and two comps
      TTYPE(6,7,5.0)   // test self and two comps
    });
      
  // Set to test between:
  const std::vector<Vec3D> VSet({
      Vec3D(1,2,0),  
      Vec3D(0,0,-1), 
      Vec3D(1,0,0), 
      Vec3D(0,1,0), 
      Vec3D(-1,0,0), 
      Vec3D(0,-1,0), 
      Vec3D(1,-3,-4), 
      Vec3D(7,2,-1)
    }); 
  

  int cnt(1);
  for(const TTYPE& tc : Tests)
    {
      const size_t iA(std::get<0>(tc));
      const size_t iB(std::get<1>(tc));
      const double dp=VSet[iA].dotProd(VSet[iB]);
      if (std::abs(dp-std::get<2>(tc))>1e-5)
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
testVec3D::testOuterProduct()
  /*!
    Tests the Vec3D outer  product
    \retval -1 on failure
    \retval 0 :: success 
  */
{
  ELog::RegMethod RegA("testVec3D","testDotProd");

  Geometry::Vec3D U(0,1,2.3);
  Geometry::Vec3D D(3.1,5,6.0);
  D.makeUnit();

  // U' M U ==> (UD)^2 
  M3<double> M=D.outerProd(D);

  Geometry::Vec3D R=M*U;
  const double dp=R.dotProd(U);
  const double rExpect=U.dotProd(D);

  ELog::EM<<"R == "<<dp<<" : "<<rExpect*rExpect<<ELog::endDiag;
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
  const std::vector<TTYPE> 
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

  

