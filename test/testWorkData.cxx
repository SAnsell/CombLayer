/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   test/testWorkData.cxx
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
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <cmath>
#include <list>
#include <vector>
#include <map>
#include <string>
#include <algorithm>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "doubleErr.h"
#include "WorkData.h"

#include "testFunc.h"
#include "testWorkData.h"


testWorkData::testWorkData() 
  /*!
    Constructor
  */
{}

testWorkData::~testWorkData() 
  /*!
    Destructor
  */
{}

int 
testWorkData::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: index of test
    \retval -1 Distance failed
    \retval 0 All succeeded
  */
{
  ELog::RegMethod RegA("testWorkData","applyTest");
  TestFunc::regSector("testWorkData");
  
  typedef int (testWorkData::*testPtr)();
  testPtr TPtr[]=
    {
      &testWorkData::testIntegral,
      &testWorkData::testRebin,
      &testWorkData::testSum
    };
  const std::string TestName[]=
    {
      "Integral",
      "Rebin",
      "Sum"
    };
  
  const int TSize(sizeof(TPtr)/sizeof(testPtr));
  if (!extra)
    {
      TestFunc::Instance().reportTest(std::cout);
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

void
testWorkData::populate(WorkData& A,const int NIndex,
		       const double xMin,const double xMax,
		       const double yA,const double yB)
  /*!
    Put some simple values into a workspace
    \param A :: Workspace to populate
    \param NIndex :: number of points
    \param xMin :: Xmin value
    \param xMax :: Xmax value
    \param yA :: Const for y poly
    \param yB :: Const for y*x poly
  */
{
  ELog::RegMethod RegA("testWorkDAta","populate");
  const double xStep((xMax-xMin)/NIndex);
  double xValue(xMin);
  A.initX(xMin);
  for(int i=0;i<NIndex;i++)
    {
      xValue+=xStep;
      A.pushData(xValue,DError::doubleErr(yA+yB*xValue,0.0));
    }
  return;
}


int
testWorkData::testIntegral()
  /*!
    Test the distance of a point from the cylinder
    \retval -1 :: failed build a cylinder
    \retval 0 :: All passed
  */
{
  ELog::RegMethod RegA("testWorkData","testIntegral");

  WorkData A;
  populate(A,10,0.0,10.0,1.0,1.0);

  DError::doubleErr FullRes=A.integrate(4.5,8.5);
  DError::doubleErr FullX=A.integrateX(4.5,8.5);

  DError::doubleErr RX;
  DError::doubleErr R;
  
  R+=A.integrate(4.5,7.2)+
    A.integrate(7.2,7.3)+
    A.integrate(7.3,7.6)+
    A.integrate(7.6,8.5);

  RX+=A.integrateX(4.5,7.2)+
    A.integrateX(7.2,7.3)+
    A.integrateX(7.3,7.6)+
    A.integrateX(7.6,8.5);

  if (R!=FullRes)
    {
      ELog::EM<<"Failure with R:"<<R<<" != "<<FullRes<<ELog::endCrit;
      ELog::EM<<"4.5 -> 7.2::"<<A.integrate(4.5,7.2)<<ELog::endWarn;
      ELog::EM<<"7.2 -> 7.3::"<<A.integrate(7.2,7.3)<<ELog::endWarn;
      ELog::EM<<"7.3 -> 7.6::"<<A.integrate(7.3,7.6)<<ELog::endWarn;
      ELog::EM<<"7.6 -> 8.5::"<<A.integrate(7.6,8.5)<<ELog::endWarn;
      return -1;
    }
  if (RX!=FullX)
    {
      ELog::EM<<"Failure with R:"<<RX<<" != "<<FullX<<ELog::endCrit;
      return -1;
    }

  return 0;
}
 
int
testWorkData::testRebin()
{
  ELog::RegMethod RegA("testWorkData","testRebin");

  WorkData A;

  // xmin,xmax,Y[0],Y[N],NPts
  typedef std::tuple<double,double,double,double,size_t> TTYPE;
  const std::vector<TTYPE> Tests=
    {
     TTYPE(-100.0,100.0, 1.0,10.0,10),
     TTYPE(0.4, 100.0, 0.6,10.0,10),
     TTYPE(1.4, 100.0, 1.2,10.0,9),
     TTYPE(-100, 9.4, 1.0,4.0,10)
    };
  size_t testIndex(1);
  for(const TTYPE& tc : Tests)
    {
      const double xMin(std::get<0>(tc));
      const double xMax(std::get<1>(tc));
			
      // 10pts: 0 to 10.0 : 0.0+x
      populate(A,10,0.0,10.0,0.0,1.0);
      A.rebin(xMin,xMax);

      const size_t N=A.getSize();
      const double YA=A.getYdata()[0];
      const double YB=A.getYdata()[N-1];

      const size_t ansN=std::get<4>(tc);
      const double ansYA=std::get<2>(tc);
      const double ansYB=std::get<3>(tc);
      
      if (N!=ansN ||
	  std::abs(YA-ansYA)>1e-3 ||
	  std::abs(YB-ansYB)>1e-3)
	{
	  ELog::EM<<"Failed on test "<<testIndex<<ELog::endDiag;
	  ELog::EM<<"N : "<<N<<" == "<<ansN<<ELog::endDiag;
	  ELog::EM<<"YA: "<<YA<<" == "<<ansYA<<ELog::endDiag;
	  ELog::EM<<"YB: "<<YB<<" == "<<ansYB<<ELog::endDiag;
	  for(size_t i=0;i<N;i++)
	    ELog::EM<<A.getXdata()[i]<<" : "
		    <<A.getYdata()[i]<<" : "
		    <<A.getXdata()[i+1]<<ELog::endDiag;
	  return -1;
	}
      testIndex++;
    }
  return 0;
}

int
testWorkData::testSum()
  /*!
    Sum two workspacs together
   */
{
  ELog::RegMethod RegA("testWorkData","testSum");

  WorkData A;
  WorkData B;

  populate(A,10,0.0,10.0,0.0,1.0);
  populate(B,10,0.0,10.0,0.0,-1.0);

  
  A.setWeight(10.0);
  B.setWeight(40.0);
  A+=B;

  // Check:
  for(size_t i=0;i<10;i++)
    {
      const double x(static_cast<double>(i+1));
      if (std::abs(A.getYdata()[i].getVal()+0.6*x)>1e-5)
	{
	  ELog::EM<<"Failed on point["<<i<<"]("<<-0.6*x<<") "
		  <<A.getYdata()[i]<<ELog::endDiag;
	  A.stream(ELog::EM.Estream());
	  ELog::EM<<ELog::endDiag;
	  return -1;
	}
    }
  return 0;

}
