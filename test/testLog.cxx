/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   test/testLog.cxx
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
#include <complex>
#include <string>
#include <algorithm>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"

#include "testFunc.h"
#include "testLog.h" 

testLog::testLog() 
  /// Constructor
{}

testLog::~testLog() 
  /// Destructor
{}

int 
testLog::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: Test number to run
    \retval -1 : testLog failed
    \retval 0 : All succeeded
  */
{
  ELog::RegMethod RegA("testLog","applyTest");
  TestFunc::regSector("testLog");

  typedef int (testLog::*testPtr)();
  testPtr TPtr[]=
    {
      &testLog::testENDL
    };
  const std::string TestName[]=
    {
      "ENDL"
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
testLog::testENDL()
  /*!
    Test of the output of log 
    Mainly to test colour
    \return 0
   */
{
  ELog::RegMethod RegA("testLog","testENDL");

  ELog::EM.setTypeFlag(1);
  try
    {
      ELog::EM<<"THIS output is in diagnostic mode"<<ELog::endDiag;
      ELog::EM<<"THIS output is in basic mode"<<ELog::endBasic;
      ELog::EM<<"THIS output is in debug mode"<<ELog::endDebug;
      ELog::EM<<"THIS output is in crit mode"<<ELog::endCrit;
      ELog::EM<<"THIS output is in warning mode"<<ELog::endWarn;
      ELog::EM<<"THIS output is in trace mode"<<ELog::endTrace;
      ELog::EM<<"THIS long output is in trace mode"<<ELog::endTrace;
      //      ELog::EM<<"THIS output is in Error mode"<<ELog::endErr;
    }	    
  catch (ColErr::ExitAbort& EA)
    {
      ELog::EM<<"Success catching EA:"<<ELog::endDiag;
    }
  ELog::EM<<"NEW LINE\nSPLIT "<<std::endl;
  ELog::EM.diagnostic();
  
  ELog::EM<<"THIS output is in debug mode"<<ELog::endDebug;
  ELog::EM<<ELog::endDebug;
  ELog::EM<<"END of EMPTY LINE:"<<ELog::endDebug;
  return 0;
}
