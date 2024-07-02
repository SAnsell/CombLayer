/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   test/testM2.cxx
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
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "Vec2D.h"
#include "dataSlice.h"
#include "multiData.h"
#include "M2.h"

#include "testFunc.h"
#include "testM2.h"


testM2::testM2() 
  /*!
    Constructor
  */
{}

testM2::~testM2() 
  /*!
    Destructor
  */
{}

int 
testM2::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: parameter to decide test
    \retval -1 :: Fail on angle
  */
{
  ELog::RegMethod RegA("testM2","applyTest");
  TestFunc::regSector("testM2");

  typedef int (testM2::*testPtr)();
  testPtr TPtr[]=
    {
      &testM2::testEigenValues,
      &testM2::testInit
    };
  const std::vector<std::string> TestName=
    {
      "EigenValues",
      "Init"
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
testM2::testInit()
  /*!
    Tests the M2 setting (with determinate)
    \retval -1 on failure
    \retval 0 :: success 
  */
{
  ELog::RegMethod RegA("testM2","testInit");

  // nA,nB,nC axis index :: sum
  typedef std::tuple<double,double,double,double> TTYPE;

  std::vector<TTYPE> Tests={
    { 1,0,0,1 },
    { 1,-0.5,-0.5,1 },
    { 4,0,3,-5 },
    { 4,0,4,4 },
    { 2,3,4,5 },
    { 1,4,4,1 }

  };
  for(const TTYPE& tc : Tests)
    {
      const double a(std::get<0>(tc));
      const double b(std::get<1>(tc));
      const double c(std::get<2>(tc));
      const double d(std::get<3>(tc));
      Geometry::M2<double> M(a,b,c,d);
      if (M.check())
	ELog::EM<<"FAIL"<<ELog::endErr;
    }

  return 0;
}

int
testM2::testEigenValues()
  /*!
    Tests the M2 eigenvalues
    \retval -1 on failure
    \retval 0 :: success 
  */
{
  ELog::RegMethod RegA("testM2","testEigenValues");

  // nA,nB,nC axis index :: sum
  typedef std::tuple<double,double,double,double> TTYPE;

  const std::vector<TTYPE> Tests={
    {-5,2.0,-9,6.0}
    //    {2.0,3.0,14.0,8.0},
    //    {-0.75,0,0,-0.75}
  };
  
  for(const TTYPE& tc : Tests)
    {
      const double a(std::get<0>(tc));
      const double b(std::get<1>(tc));
      const double c(std::get<2>(tc));
      const double d(std::get<3>(tc));
      Geometry::M2<double> M(a,b,c,d);
      M.constructEigen();

      const auto [aS,bS]=M.getEigPair();

      ELog::EM<<"EValue = "<<aS<<" "<<bS<<ELog::endDiag;
      const Geometry::Vec2D eA=M.getEigVec(0);
      const Geometry::Vec2D eB=M.getEigVec(1);
      ELog::EM<<"EVec = "<<eA<<" Size= "<<eA.abs()<<ELog::endDiag;
      ELog::EM<<"EVec = "<<eB<<" Size= "<<eB.abs()<<ELog::endDiag;

      Geometry::Vec2D checkA=M*eA;
      Geometry::Vec2D checkB=M*eB;
      ELog::EM<<"M*EVec = "<<checkA<<":"<<eA*aS<<ELog::endDiag;
      ELog::EM<<"M*EVec = "<<checkB<<":"<<eB*bS<<ELog::endDiag; 
    }

  return 0;
}
