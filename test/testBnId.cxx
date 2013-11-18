/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   test/testBnId.cxx
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#include <set>
#include <map>
#include <stack>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include "Debug.h"
#include "BnId.h"

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "testFunc.h"
#include "testBnId.h"


testBnId::testBnId() 
  /*!
    Constructor
  */
{}

testBnId::~testBnId() 
  /*!
    Destructor
  */
{}

int 
testBnId::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: Test to use
    \retval 0 :: success
    \retval -ve :: error
  */
{
  ELog::RegMethod RegItem("testBnId","applyTest");
  TestFunc::regSector("testBnId");

  if (extra==0)
    {
      std::cout<<"Extra options "<<std::endl;
      std::cout<<"testMinTerm          (1)"<<std::endl;
    }
  int retVal;
  if (extra<0 || extra==1)
    {
      TestFunc::regTest("testMinTerm");
      retVal=testMinTerm();
      if (retVal)
	return retVal;
    }

  return 0;
}


int 
testBnId::testMinTerm()
  /*!
    Test the number of literals in a string.
    Single test includes not.
    \retval -1 :: Failed to get teh correct number of literals
    \retval 0 :: success
   */
{
  return 0;
}

