/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   test/testMaterial.cxx
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#include <math.h>
#include <list>
#include <vector>
#include <map>
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

#include "testFunc.h"
#include "testMaterial.h"

using namespace MonteCarlo;

testMaterial::testMaterial() 
  /*!
    Constructor
  */
{}

testMaterial::~testMaterial() 
  /*!
    Destructor
  */
{}

int 
testMaterial::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: index of test
    \retval -1 Distance failed
    \retval 0 All succeeded
  */
{
  ELog::RegMethod RegA("testMaterial","applyTest");
  TestFunc::regSector("testMaterial");
  typedef int (testMaterial::*testPtr)();
  testPtr TPtr[]=
    {
      &testMaterial::testPlus,
    };
  const std::string TestName[]=
    {
      "Plus",
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
testMaterial::testPlus()
  /*!
    Test the addition function of two materials
    \retval -1 :: failed build a cone
    \retval 0 :: All passed
  */
{
  ELog::RegMethod RegA("testMaterial","testSideDirection");


  // "MatLine" : MTLine : MibLine
  typedef std::tuple<std::string,std::string,std::string> ZTYPE;
  std::vector<ZTYPE> MatStr;
  typedef std::tuple<size_t,size_t,size_t> TTYPE;
  std::vector<TTYPE> Tests;

  MatStr.push_back(ZTYPE("82204.70c 0.1 ","",""));

  std::vector<Material> MatVec;

  int N(0);
  for(const ZTYPE& zc : MatStr)
    {
      const std::string Name="Mat"+StrFunc::makeString(N);
      Material MObj;
      MObj.setMaterial(N,Name,std::get<0>(zc),
		       std::get<1>(zc),std::get<2>(zc));
      MatVec.push_back(MObj);
      N++;
    }

  for(const TTYPE& tc : Tests)
    {
      Material A(MatVec[std::get<0>(tc)]);
      const Material B(MatVec[std::get<1>(tc)]);
      const Material C(MatVec[std::get<2>(tc)]);
      A+=B;
      std::ostringstream AX;
      std::ostringstream CX;
      A.write(AX);
      C.write(CX);
      
      if (CX.str()!=AX.str())
	{
	  ELog::EM<<"A == "<<AX.str()<<ELog::endDebug;
	  ELog::EM<<"C == "<<CX.str()<<ELog::endDebug;
	}

    }
  return 0;
}

  
  

