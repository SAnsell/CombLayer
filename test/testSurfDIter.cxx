/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   test/testSurfDIter.cxx
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
#include <complex>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <string>
#include <algorithm>
#include <numeric>

#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "MapSupport.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FItem.h"
#include "FuncDataBase.h"

#include "surfDIter.h" 

#include "testFunc.h"
#include "testSurfDIter.h"

using namespace ModelSupport;

testSurfDIter::testSurfDIter()
  /*!
    Constructor
  */
{}


testSurfDIter::~testSurfDIter() 
  /*!
    Destructor
  */
{}

int 
testSurfDIter::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: Index of test [-ve for all]
    \returns -ve on error 0 on success.
  */
{
  ELog::RegMethod RegA("testSurfDIter","applyTest");
  TestFunc::regSector("testSurfDIter");

  typedef int (testSurfDIter::*testPtr)();
  testPtr TPtr[]=
    {
      &testSurfDIter::testPopulateQuadRange
    };

  const std::string TestName[]=
    {
      "PopulateQuadRange"
    };

  const size_t TSize(sizeof(TPtr)/sizeof(testPtr));
  if (!extra)
    {
      std::ios::fmtflags flagIO=std::cout.setf(std::ios::left);
      for(size_t i=0;i<TSize;i++)
        {
	  std::cout<<std::setw(30)<<TestName[i]<<"("<<i+1<<")"<<std::endl;
	}
      std::cout.flags(flagIO);
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
testSurfDIter::testPopulateQuadRange()
  /*!
    Test the population of Quad range
    \returns 0 on succes and -ve on failure
  */
{
  ELog::RegMethod RegA("testSurfDIter","testStatic");
  
  // First test
  FuncDataBase Control;

  // Basic verison:
  const size_t N(24);
  const double APt(-100.0);
  const double NPt(0.0);
  const double BPt(340.0);
  std::vector<double> Out;
  populateQuadRange(Control,N,"ATest",APt,NPt,BPt,Out);
  const double sum=
    std::accumulate(Out.begin(),Out.end(),0.0,std::plus<double>());
  if (std::abs(sum-1202.5)>1e-6)
    {
      ELog::EM<<"Failed on ATest"<<ELog::endDiag;
      ELog::EM<<"Sum == "<<sum<<ELog::endDiag;
      for(size_t i=0;i<Out.size();i++)
	ELog::EM<<"Pt["<<i<<"] == "<<Out[i]<<ELog::endDiag;
      return -1;
    }

  Control.addVariable("BTest6",2.0);
  populateQuadRange(Control,N,"BTest",APt,NPt,BPt,Out);
  const double sumB=
    std::accumulate(Out.begin(),Out.end(),0.0,std::plus<double>());
  if (std::abs(sumB-1564.42)>1e-2 || std::abs(Out[6]-2.0)>1e-6)
    {
      ELog::EM<<"Failed on BTest"<<ELog::endDiag;
      ELog::EM<<"Sum == "<<sumB-1564.42<<ELog::endDiag;
      ELog::EM<<"Pt[6] == "<<Out[6]-2.0<<ELog::endDiag;
      for(size_t i=0;i<Out.size();i++)
	ELog::EM<<"Pt["<<i<<"] == "<<Out[i]<<ELog::endDiag;
      return -1;
    }

  
  return 0;
}



