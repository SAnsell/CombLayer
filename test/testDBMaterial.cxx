 /********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   test/testDBMaterial.cxx
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
#include <set>
#include <string>
#include <algorithm>
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
#include "stringCombine.h"
#include "RefCon.h"
#include "Element.h"
#include "Zaid.h"
#include "MXcards.h"
#include "Material.h"
#include "DBMaterial.h"

#include "testFunc.h"
#include "testDBMaterial.h"


using namespace ModelSupport;

testDBMaterial::testDBMaterial() 
  /*!
    Constructor
  */
{}

testDBMaterial::~testDBMaterial() 
  /*!
    Destructor
  */
{}

int 
testDBMaterial::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: index of test
    \retval -1 Distance failed
    \retval 0 All succeeded
  */
{
  ELog::RegMethod RegA("testDBMaterial","applyTest");

  TestFunc::regSector("testDBMaterial");
  typedef int (testDBMaterial::*testPtr)();
  testPtr TPtr[]=
    {
      &testDBMaterial::testCombine
    };
  const std::string TestName[]=
    {
      "Combine"
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
testDBMaterial::testCombine()
  /*!
    Test the addition function of two materials
    \retval -1 :: failed build a cone
    \retval 0 :: All passed
  */
{
  ELog::RegMethod RegA("testDBMaterial","testCombine");


  // "MatLine" : MTLine : MibLine
  typedef std::tuple<std::string,int,std::string> TTYPE;
  std::vector<TTYPE> Tests={
    TTYPE("H2O%D2O%50",1,"1002.70c 0.0330067")
  };

  DBMaterial& DB=DBMaterial::Instance();

  int cnt(1);
  for(const TTYPE& tc : Tests)
    {
      const int res=DB.createMaterial(std::get<0>(tc));
      if (res && res==std::get<1>(tc))
	{
	  const MonteCarlo::Material& MT=
	    DB.getMaterial(std::get<0>(tc));

	  std::ostringstream cx;
	  cx<<MT;
	  const std::string SLine=StrFunc::singleLine(cx.str());
	  if (SLine.find(std::get<2>(tc))==std::string::npos)
	    {
	      ELog::EM<<"Test "<<cnt<<ELog::endDiag;
	      ELog::EM<<"String "<<std::get<2>(tc)<<ELog::endDiag;
	      ELog::EM<<"Material --: \n"<<MT<<ELog::endDiag;
	      return -1;
	    }
	}
      else if (res!=std::get<1>(tc))
	{
	  ELog::EM<<"Test "<<cnt<<ELog::endDiag;
	  ELog::EM<<"String "<<std::get<0>(tc)<<ELog::endDiag;
	  ELog::EM<<"Failed RES "<<res<<" "<<ELog::endDiag;
	  return -1;
	}
      cnt++;


    }
  return 0;
}

  
  

