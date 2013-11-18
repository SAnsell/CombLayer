/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   test/testFunction.cxx
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
#include <sstream>
#include <climits>
#include <cmath>
#include <list>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <iterator>
#include <boost/tuple/tuple.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "support.h"
#include "funcList.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"

#include "testFunc.h"
#include "testFunction.h"

testFunction::testFunction() 
  /*!
    Constructor
  */
{}

testFunction::~testFunction() 
  /*!
    Destructor
  */
{}

int 
testFunction::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: index of test to app;y
    \retval 0 :: Success
  */
{
  ELog::RegMethod RegA("testFunction","applyTest");
  TestFunc::regSector("testFunction");

  typedef int (testFunction::*testPtr)();
  testPtr TPtr[]=
    {
      &testFunction::testAnalyse,
      &testFunction::testBuiltIn,
      &testFunction::testEval,
      &testFunction::testVec3D
    };

  const std::string TestName[]=
    {
      "Analyse",
      "BuiltIn",
      "Eval",
      "Vec3D"
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
testFunction::testAnalyse()
  /*!
    Tests the function analysis
    \return 0 on success/-ve on failure
  */
{
  ELog::RegMethod RegA("testFunction","testAnalyse");

  typedef boost::tuple<std::string,double> TTYPE;
  std::vector<TTYPE> Tests;
  
  Tests.push_back(TTYPE("atan(tx*tx)",atan(100.0)));
  Tests.push_back(TTYPE("sin(tx+3)",sin(13.0)));
  Tests.push_back(TTYPE("sqrt(itemA+itemB)",sqrt(3.4+6.0)));

  FuncDataBase XX;   
  XX.addVariable("tx",10.0);
  XX.addVariable("itemA",3.4); 
  XX.Parse("sqrt(36.0)");
  XX.addVariable("itemB");             // check the expression parse
  
  int cnt(1);
  std::vector<TTYPE>::const_iterator tc;
  for(tc=Tests.begin();tc!=Tests.end();tc++)
    {
      const int flag=XX.Parse(tc->get<0>());
      if (flag)
	{
	  ELog::EM<<"Compile error:"<<flag<<ELog::endWarn;
	  return -cnt;
	}
      if (fabs(XX.Eval()-tc->get<1>())>1e-5)
	{
	  ELog::EM<<"Eval error:"<<XX.Eval()<<ELog::endWarn;
	  return -cnt;
	}
      cnt++;
    }
  //  std::cout<<"Eval == "<<XX.Eval()<<" "<<sqrt(atan(100)+sin(13))<<std::endl;
  return 0;
}

int
testFunction::testBuiltIn()
  /*!
    Test builtin commands
    \retval 0 :: success
  */
{
  ELog::RegMethod RegA("testFunction","testBuiltIn");

  FuncDataBase XX;   

  std::vector<std::string> TestString;
  std::vector<double> Results;
  
  TestString.push_back("max(tx,ty)");
  Results.push_back(20);

  XX.addVariable("tx",10.0);
  XX.addVariable("ty",20.0);

  std::vector<std::string>::const_iterator sc;
  std::vector<double>::const_iterator rc=Results.begin();
  for(sc=TestString.begin();sc!=TestString.end();sc++,rc++)
    {
      if (XX.Parse(*sc) || fabs(XX.Eval()-(*rc))>1e-6)
        {
	  ELog::EM<<"Failed to Parse"<<*sc<<ELog::endErr;
	  return static_cast<int>(sc-TestString.begin())-1;
	}
    }
  return 0;
}

int
testFunction::testEval()
  /*!
    Test eval output
    \retval 0 :: success
  */
{
  ELog::RegMethod RegA("testFunction","testEval");

  FuncDataBase XX;   

  typedef boost::tuple<std::string,double,int,size_t,double> TTYPE;
  std::vector<TTYPE> Tests;
  
  Tests.push_back(TTYPE("V1",-1.2,-1,ULONG_MAX,-1.2));
  
  std::vector<TTYPE>::const_iterator tc;
  for(tc=Tests.begin();tc!=Tests.end();tc++)
    XX.addVariable(tc->get<0>(),tc->get<1>());
  
  for(tc=Tests.begin();tc!=Tests.end();tc++)
    {
      const int A=XX.EvalVar<int>(tc->get<0>());
      const size_t B=XX.EvalVar<size_t>(tc->get<0>());
      const double C=XX.EvalVar<double>(tc->get<0>());
      int failFlag(0);
      if (A!=tc->get<2>()) failFlag |= 1;
      if (B!=tc->get<3>()) failFlag |= 2;
      if (fabs(C-tc->get<4>())>1e-6) failFlag |= 4;
      
      if (failFlag)
	{
	  ELog::EM<<"Variable "<<tc->get<0>()<<ELog::endTrace;
	  if (failFlag & 1)
	    ELog::EM<<"Failed on int:"<<A<<" ("
		    <<tc->get<2>()<<")"<<ELog::endTrace;
	  if (failFlag & 2)
	    ELog::EM<<"Failed on size_t:"<<B<<" ("
		    <<tc->get<3>()<<")"<<ELog::endTrace;
	  if (failFlag & 4)
	    ELog::EM<<"Failed on double:"<<C<<" ("
		    <<tc->get<4>()<<")"<<ELog::endTrace;
	  return -1;
	}
    }
  return 0;
}

int
testFunction::testVec3D()
  /*!
    Test builtin commands
    \retval 0 :: success
  */
{
  ELog::RegMethod RegA("testFunction","testVec3D");

  FuncDataBase XX;   

  typedef boost::tuple<std::string,Geometry::Vec3D> TTYPE;
  std::vector<TTYPE> Tests;
  
  Tests.push_back(TTYPE("V1",Geometry::Vec3D(3,4,5)));
  Tests.push_back(TTYPE("V2",Geometry::Vec3D(10,-14,12)));
  
  std::vector<TTYPE>::const_iterator tc;
  for(tc=Tests.begin();tc!=Tests.end();tc++)
    XX.addVariable(tc->get<0>(),tc->get<1>());
  
  for(tc=Tests.begin();tc!=Tests.end();tc++)
    {
      if (XX.EvalVar<Geometry::Vec3D>(tc->get<0>())!=tc->get<1>())
	{
	  ELog::EM<<"XX == "<<XX.EvalVar<Geometry::Vec3D>("V1")<<ELog::endTrace;
	  ELog::EM<<"Parse == "<<XX.Parse("V1+V2")<<ELog::endTrace;
	  XX.printByteCode(ELog::EM.Estream());
	  ELog::EM<<ELog::endTrace;
	  ELog::EM<<"Eval == "<<XX.Eval()<<ELog::endTrace;
	  return -1;
	}
    }
  return 0;
}

  
  
