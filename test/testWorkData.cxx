/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   test/testWorkData.cxx
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
      &testWorkData::testSum
    };
  const std::string TestName[]=
    {
      "Integral",
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
testWorkData::testSum()
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
