/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   test/testSource.cxx
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
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "ManagedPtr.h"
#include "mathSupport.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "SrcData.h"
#include "SrcItem.h"
#include "DSTerm.h"
#include "Source.h"

#include "testFunc.h"
#include "testSource.h"

using namespace SDef;

testSource::testSource() 
  /// Constructor
{}

testSource::~testSource() 
  /// Destructor
{}

int
testSource::checkSDef(const std::string& Res,const std::string& Expect)
  /*!
    check the Res vs Expected
    \param Res :: Result
    \param Expect :: Expected result
    \return -ve on failure / 0 on success
  */
{
  
  if (StrFunc::fullBlock(Res)!=Expect)
    {
      ELog::EM<<"Test Failed"<<ELog::endDiag;
      ELog::EM<<"Expect:"<<Expect<<":"<<ELog::endDiag;
      ELog::EM<<"Out   :"<<StrFunc::fullBlock(Res)<<":"<<ELog::endDiag;
      const std::string Out=StrFunc::fullBlock(Res);
      for(size_t i=0;i<Out.size();i++)
	if (isspace(Out[i]))
	  ELog::EM<<"<spc>";
      	else if (isalnum(Out[i]) || Out[i]=='.')
	  ELog::EM<<Out[i];
      	else
	  ELog::EM<<"<"<<(int)Out[i]<<">";
	  
      ELog::EM<<ELog::endDiag;

      for(size_t i=0;i<Expect.size();i++)
	if (isspace(Expect[i]))
	  ELog::EM<<"<spc>";
	else if (isalnum(Expect[i]) || Expect[i]=='.')
	  ELog::EM<<Expect[i];
	else
	  ELog::EM<<"<"<<(int)Expect[i]<<">";

      ELog::EM<<ELog::endDiag;
      return -1;
    }
  return 0;
}

int 
testSource::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: index of test
    \retval -1 Failed
    \retval 0 All succeeded
  */
{
  ELog::RegMethod RegA("testSource","applyTest");
  TestFunc::regSector("testSource");

  typedef int (testSource::*testPtr)();
  testPtr TPtr[]=
    {
      &testSource::testBasic,
      &testSource::testItem,
      &testSource::testProbTable
    };

  std::string TestName[]=
    {
      "Basic",
      "Item",
      "ProbTable"
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
	  if (retValue)
	    return i+1;
	}
    }
  return 0;
}

int
testSource::testBasic()
  /*!
    Simple test of source term [basic components]
    \return 0 sucess / -ve on failure
  */
{
  ELog::RegMethod RegItem("testSource","testBasic");

	
  const std::string Expect("sdef cel=1 x=3.4");
  Source A;
  A.setComp("cel",1);
  A.setComp("x",3.4);

  std::ostringstream cx;
  A.write(cx);
  return checkSDef(cx.str(),Expect);
}

int
testSource::testProbTable()
  /*!
    Tests the Creation of Line and stuff
    \return 0 sucess / -ve on failure
  */
{
  ELog::RegMethod RegItem("testSource","testProbTable");

  const std::string Expect
    ("sdef cel=1 erg=d1 x=3.4\n"
     "si1 A 0.3 0.5 0.9\n"
     "sp1 0.2 0.3"  );
  
  Source A;
  A.setComp("cel",1);
  A.setComp("x",3.4);

  SrcData D1(1);

  SrcInfo SI1('A');
  SI1.addData(0.3);
  SI1.addData(0.5);
  SI1.addData(0.9);

  SrcProb SP1(0);
  SP1.addData(0.2);
  SP1.addData(0.3);
  D1.addUnit(SI1);
  D1.addUnit(SP1);
  A.setData("erg",D1);

  std::ostringstream cx;
  A.write(cx);
  return checkSDef(cx.str(),Expect);
}

int
testSource::testItem()
  /*!
    Test the interPoint/interDistance template function
    \return 0 sucess / -ve on failure
  */
{
  ELog::RegMethod RegItem("testSource","testItem");
  const std::string Expect
    ("sdef ccc=76 dir=1 erg=fdir=d3 par=9 tr=1 vec=0.0 0.0 -5 x=d1 y=d2\n"
     "sp1 -41 1.3344 0\n"
     "sp2 -41 1.3344 0\n"
     "ds3 s 18 17 15 13");

  Source A;
  
  A.setComp("dir",1.0);
  A.setComp("vec",Geometry::Vec3D(0,0,-5));
  A.setComp("tr",1);
  A.setComp("par",9);
  A.setComp("ccc",76);

  SrcData D1(1);
  SrcProb SP1(1);
  SP1.setFminus(-41,1.3344,0);
  D1.addUnit(SP1);

  SrcData D2(2);
  D2.addUnit(SP1);
  A.setData("x",D1);
  A.setData("y",D2);
  
  SrcData D3(3);
  DSTerm<int>* DS=D3.getDS<int>();
  DS->setType("dir",'s');
  DS->addData(18);
  DS->addData(17);
  DS->addData(15);
  DS->addData(13);

  A.setData("erg",D3);


  std::ostringstream cx;
  A.write(cx);
  return checkSDef(cx.str(),Expect);
}
  
  
