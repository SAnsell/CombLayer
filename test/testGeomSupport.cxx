/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   test/testGeomSupport.cxx
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
#include <vector>
#include <deque>
#include <map>
#include <string>
#include <algorithm>
#include <iterator>
#include <memory>
#include <tuple>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "mathSupport.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "geomSupport.h"

#include "testFunc.h"
#include "testGeomSupport.h"

testGeomSupport::testGeomSupport() 
  /*!
    Constructor
  */
{}

testGeomSupport::~testGeomSupport() 
  /*!
    Destructor
  */
{}

int 
testGeomSupport::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: Value to determine which test to apply
    \return 0 :: success -ve otherwize
  */
{
  ELog::RegMethod RegA("testGeomSupport","applyTest");
  TestFunc::regSector("testGeomSupport");

  typedef int (testGeomSupport::*testPtr)();
  testPtr TPtr[]=
    {
      &testGeomSupport::testCornerCircle,
      &testGeomSupport::testCornerCircleTouch
    };

  const std::string TestName[]=
    {
      "CornerCircle",
      "CornerCircleTouch"
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
testGeomSupport::testCornerCircleTouch()
  /*!
    Test the intersection of general quadratic surfaces
    \retval 0 :: success
  */
{
  ELog::RegMethod RegA("testSurInterSect","testCornerCircleTouch");
    
  // Centre , A , B , Radius , Result
  typedef std::tuple<Geometry::Vec3D,Geometry::Vec3D,
		     Geometry::Vec3D,double,
		     Geometry::Vec3D> TTYPE;
  std::vector<TTYPE> Tests;

  Tests={TTYPE(Geometry::Vec3D(0,0,0),Geometry::Vec3D(0,1,0),
	       Geometry::Vec3D(1,0,0),	1.0,Geometry::Vec3D(1,1,0)),
	 
	 TTYPE(Geometry::Vec3D(7,3,1),Geometry::Vec3D(7,4,1),
	       Geometry::Vec3D(8,3,1),3.0,Geometry::Vec3D(10,6,1))
	 
  };

 
  int cnt(1);
  for(const TTYPE& tc : Tests)
    {
      const Geometry::Vec3D Cent =std::get<0>(tc);
      const Geometry::Vec3D A =std::get<1>(tc);
      const Geometry::Vec3D B =std::get<2>(tc);
      const double Rad =std::get<3>(tc);
      const Geometry::Vec3D testResult =std::get<4>(tc);


      Geometry::Vec3D Result=
	Geometry::cornerCircleTouch(Cent,A,B,Rad);
      if (Result.Distance(testResult)>Geometry::zeroTol)
	{
	  ELog::EM<<"Failed on test "<<cnt<<ELog::endDiag;
	  ELog::EM<<"C =  "<<Cent<<ELog::endDiag;
	  ELog::EM<<"A =  "<<A<<ELog::endDiag;
	  ELog::EM<<"B =  "<<B<<ELog::endDiag;
	  ELog::EM<<"R =  "<<Rad<<ELog::endDiag;
	  ELog::EM<<"Result "<<Result<<ELog::endDiag;
	  ELog::EM<<"Expect "<<testResult<<ELog::endDiag;
	  return -1;
	}
      cnt++;
    }

  return 0;
}


int
testGeomSupport::testCornerCircle()
  /*!
    Test the intersection of general quadratic surfaces
    \retval 0 :: success
  */
{
  ELog::RegMethod RegA("testSurInterSect","testCornerCircle");
    
  // Centre , A , B , Radius 
  typedef std::tuple<Geometry::Vec3D,Geometry::Vec3D,
		     Geometry::Vec3D,double> TTYPE;

  // results
  typedef std::tuple<Geometry::Vec3D,Geometry::Vec3D> RTYPE;
  std::vector<TTYPE> Tests;
  std::vector<RTYPE> Results;

  Tests={TTYPE(Geometry::Vec3D(0,0,0),Geometry::Vec3D(0,1,0),
	       Geometry::Vec3D(1,0,0),	1.0),
	 
	 TTYPE(Geometry::Vec3D(7,3,1),Geometry::Vec3D(7,4,1),
	       Geometry::Vec3D(8,3,1),	3.0)
  };
  
  Results={
    RTYPE(Geometry::Vec3D(1,0,0),Geometry::Vec3D(0,1,0)),
    RTYPE(Geometry::Vec3D(10,3,1),Geometry::Vec3D(7,6,1))
  };
	  
  size_t resIndex(0);
  for(const TTYPE& tc : Tests)
    {
      const Geometry::Vec3D Cent =std::get<0>(tc);
      const Geometry::Vec3D A =std::get<1>(tc);
      const Geometry::Vec3D B =std::get<2>(tc);
      const double Rad =std::get<3>(tc);
      const Geometry::Vec3D ResA=std::get<0>(Results[resIndex]);
      const Geometry::Vec3D ResB=std::get<1>(Results[resIndex]);
      
      const std::pair<Geometry::Vec3D,Geometry::Vec3D>
	output=Geometry::cornerCircle(Cent,A,B,Rad);

      if ((ResA.Distance(output.first)>Geometry::zeroTol ||
	   ResB.Distance(output.second)>Geometry::zeroTol) &&
	  (ResA.Distance(output.second)>Geometry::zeroTol ||
	   ResB.Distance(output.first)>Geometry::zeroTol) )
	{
	  ELog::EM<<"Failed on test "<<resIndex+1<<ELog::endDiag;
	  ELog::EM<<"C =  "<<Cent<<ELog::endDiag;
	  ELog::EM<<"A =  "<<A<<ELog::endDiag;
	  ELog::EM<<"B =  "<<B<<ELog::endDiag;
	  ELog::EM<<"R =  "<<Rad<<ELog::endDiag;
	  ELog::EM<<"ResA "<<output.first<<ELog::endDiag;
	  ELog::EM<<"Expect "<<ResA<<ELog::endDiag;

	  ELog::EM<<"ResB "<<output.second<<ELog::endDiag;
	  ELog::EM<<"Expect "<<ResB<<ELog::endDiag;
	  return -1;
	}
      resIndex++;
    }

  return 0;
}
