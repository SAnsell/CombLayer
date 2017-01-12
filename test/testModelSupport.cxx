/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   test/testModelSupport.cxx
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#include <deque>
#include <set>
#include <map>
#include <stack>
#include <string>
#include <sstream>
#include <algorithm>
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
#include "Surface.h"
#include "Rules.h"
#include "BnId.h"
#include "Acomp.h"
#include "Algebra.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "ModelSupport.h"

#include "testFunc.h"
#include "testModelSupport.h"


testModelSupport::testModelSupport() 
  /*!
    Constructor
  */
{}

testModelSupport::~testModelSupport() 
  /*!
    Destructor
  */
{
}

int 
testModelSupport::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: Test number to run
    \retval -1 : SetObject 
    \retval 0 : All succeeded
  */
{
  ELog::RegMethod RegA("testModelSupport","applyTest");
  TestFunc::regSector("testModelSupport");

  typedef int (testModelSupport::*testPtr)();
  testPtr TPtr[]=
    {
      &testModelSupport::testRemoveOpenPair
    };
  const std::string TestName[]=
    {
      "RemoveOpenPair",
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
testModelSupport::testRemoveOpenPair()
  /*!
    Test to set if various empty strings can be removed
    \return 0 on success
  */
{
  ELog::RegMethod RegA("testModelSupport","testRemoveOpenPair");

  typedef std::tuple<std::string,std::string> TTYPE;
  const std::vector<TTYPE> Tests=
    {
      TTYPE("1 -2 3 -4","1 -2 3 -4"),
      TTYPE("1 -2 () 3 -4","1 -2 3 -4"),
      TTYPE("1 -2 ( 3 : -4)","1 -2 ( 3 : -4 )"),
      TTYPE("1 -2 ( 3 : : -4 :)","1 -2 ( 3 : -4 )")
    };
  
  
  for(const TTYPE& tc : Tests)
    {
      const std::string Res=ModelSupport::removeOpenPair(std::get<0>(tc));
      if (StrFunc::singleLine(Res)!=std::get<1>(tc))
	{
	  ELog::EM<<"Result  == "<<StrFunc::singleLine(Res)<<ELog::endTrace;
	  ELog::EM<<"Expect  == "<<std::get<1>(tc)<<ELog::endTrace;
	  return -1;
	}
    }
  return 0;
}
