/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   test/testNList.cxx
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
#include <cmath>
#include <vector>
#include <list>
#include <map>
#include <stack>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "Triple.h"
#include "NList.h"
#include "support.h"

#include "testFunc.h"
#include "testNList.h"


using namespace tallySystem;

testNList::testNList() 
  /*!
    Constructor
  */
{}

testNList::~testNList() 
  /*!
    Destructor
  */
{}

int 
testNList::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: Index for test
    \retval -1 Range failed
    \retval 0 All succeeded
  */
{
  ELog::RegMethod RegA("testNList","applyTest");
  TestFunc::regSector("testNList");

  int retValue(0);
  if (!extra)
    {
      std::cout<<"TestRange            (1)"<<std::endl;
      return 0;
    }

  if (extra<0 || extra==1)
    {
      TestFunc::regTest("testRange");
      retValue+=testRange();
      if (retValue || extra>0)
	return retValue;
    }
  return retValue;
}

int
testNList::testRange() 
  /*!
    Test the processsing with intervals 
    (does not actually check the intervals
    other than to write it to the screen)
    \retval -1 :: Unable to process line
    \retval 0 :: success
  */
{
  ELog::RegMethod RegA("testNList","testRange");
  const std::string testString("1 2 -3 inp=45 out= 56 T");
  const std::string expectString("1 2 -3 inp= 45 out= 56 T");
  NList<int> NE;
  if (NE.processString(testString))
    {
      ELog::EM<<"Error with NList "<<ELog::endWarn;
      return -1;
    }

  std::ostringstream cx;
  NE.write(cx);

  if (StrFunc::fullBlock(cx.str())!=expectString)
    {
      ELog::EM<<"Failed on process string"<<ELog::endDiag;
      ELog::EM<<"Retvalue == "<<cx.str()<<" =="<<ELog::endDiag;
      ELog::EM<<"Expected == "<<expectString<<" =="<<ELog::endDiag;
      return -2;
    }
      
  cx.str("");
  NList<int> NX(NE);
  NX.write(cx);
  if (StrFunc::fullBlock(cx.str())!=expectString)
    {
      ELog::EM<<"Failed on constructor"<<ELog::endDiag;
      ELog::EM<<"Retvalue == "<<cx.str()<<" =="<<ELog::endDiag;
      ELog::EM<<"Expected == "<<expectString<<" =="<<ELog::endDiag;
      return -2;
    }
  
  cx.str("");
  NList<int> NY;
  NY=NX;
  NY.write(cx);
  if (StrFunc::fullBlock(cx.str())!=expectString)
    {
      ELog::EM<<"Failed on assignment"<<ELog::endDiag;
      ELog::EM<<"Retvalue == "<<cx.str()<<" =="<<ELog::endDiag;
      ELog::EM<<"Expected == "<<expectString<<" =="<<ELog::endDiag;
      return -3;
    }

  return 0;
}

