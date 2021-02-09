/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   test/testGroupRange.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#include <iostream>
#include <iomanip>
#include <cmath>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <stack>
#include <set>
#include <map>
#include <tuple>
#include <algorithm>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "support.h"
#include "groupRange.h"

#include "testFunc.h"
#include "testGroupRange.h"


testGroupRange::testGroupRange() 
  /*!
    Constructor
  */
{}

testGroupRange::~testGroupRange() 
  /*!
    Destructor
  */
{}

groupRange
testGroupRange::makeGrp(std::string Line) 
  /*!
    Bulid a group
    \param Line :: line to build from 
    \return Group range
   */
{
  ELog::RegMethod RegA("testGroupRange","makeGrp");

  groupRange AGrp;
  int A,B;
  while(!StrFunc::isEmpty(Line))
    {
      std::string Units=StrFunc::getDelimUnit("[","]",Line);
      if (StrFunc::section(Units,A) &&
	  StrFunc::section(Units,B))
	{
	  AGrp.addItem(A,B);
	}
      else if (StrFunc::section(Line,A))
	{
	  AGrp.addItem(A);
	}
      else
	{
	  ELog::EM<<"Failed to read line "<<ELog::endDiag;
	  return AGrp;
	}
    }
  return AGrp;
}

int 
testGroupRange::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: parameter to decide test
    \retval -1 :: Fail on angle
  */
{
  ELog::RegMethod RegA("testGroupRange","applyTest");
  TestFunc::regSector("testGroupRange");

  typedef int (testGroupRange::*testPtr)();
  testPtr TPtr[]=
    {
      &testGroupRange::testAddItem,
      &testGroupRange::testGetNext,
      &testGroupRange::testInsert,
      &testGroupRange::testMerge,
      &testGroupRange::testRemove,
      &testGroupRange::testValid
    };
  const std::vector<std::string> TestName=
    {
      "AddItem",
      "GetNext",
      "Insert",
      "Merge",
      "Remove",
      "Valid"
    };
  
  const size_t TSize(sizeof(TPtr)/sizeof(testPtr));
  if (!extra) 
    {
      TestFunc::writeTests(TestName);
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
testGroupRange::testAddItem()
  /*!
    Tests the adding of items to a range
    \retval -1 on failure
    \retval 0 :: success 
  */
{
  ELog::RegMethod RegA("testGroupRange","testAddItem");

  const std::vector<std::vector<int>> TVec
    ({
      {-3,-4,-5,3,4,5,7,21},
      {-3,-4,-5,2,3,4,5,7,21},
      {-3,-4,-5,2,3,4,5,7,21}

    });
  typedef std::tuple<int,int> TTYPE;

  std::vector<TTYPE> Tests
    ({{1,4}});

  for(size_t i=0;i<Tests.size();i++)
    {
      const TTYPE& tc=Tests[i];

      groupRange A(TVec[i]);
      groupRange B(TVec[i+1]);
      A.addItem(std::get<0>(tc),std::get<1>(tc));
      if (A!=B)
	{
	  ELog::EM<<"A == "<<A<<ELog::endDiag;
	  ELog::EM<<"B == "<<B<<ELog::endDiag;
	  ELog::EM<<"Add == "<<std::get<0>(tc)<<":"
		  <<std::get<1>(tc)<<ELog::endDiag;
	}
    }
  return 0;
}

int
testGroupRange::testGetNext()
  /*!
    Tests the get Next functoin
    \retval -1 on failure
    \retval 0 :: success 
  */
{
  ELog::RegMethod RegA("testGroupRange","testGetNext");

  const std::vector<int> testVec({3,4,5,7,21,1,2,-3,-4,-5,7});
  
  groupRange A(testVec);
  const std::vector<int> outVec=A.getAllCells();

  int res(-300);
  for(const int Ans :outVec)
    {
      res=A.getNext(res);      
      if (res!=Ans)
	{
	  ELog::EM<<"Failed :"<<ELog::endDiag;
	  ELog::EM<<"A      = "<<A<<ELog::endDiag;;
	  ELog::EM<<"Ans    = "<<Ans<<ELog::endDiag;;
	  ELog::EM<<"Res    = "<<res<<ELog::endDiag;;
	  return -1;
	}
    }
  /// Now test intermidiates
  typedef std::tuple<int,int> TTYPE;
  const std::vector<TTYPE> Tests({
      TTYPE(6,7),
      TTYPE(18,21)
	});
  
  for(const TTYPE& tc : Tests)
    {
      res=A.getNext(std::get<0>(tc));
      if (res!=std::get<1>(tc))
	{
	  ELog::EM<<"Res    == "<<res<<ELog::endDiag;
	  ELog::EM<<"Start  == "<<std::get<0>(tc)<<ELog::endDiag;
	  ELog::EM<<"Expect == "<<std::get<1>(tc)<<ELog::endDiag;
	  return -1;
	}
    }
  return 0;
}

int
testGroupRange::testInsert()
  /*!
    Tests the merge units
    \retval -1 on failure
    \retval 0 :: success 
  */
{
  ELog::RegMethod RegA("testGroupRange","testInsert");

  groupRange A;

  const std::vector<int> testVec({3,4,5,7,21,1,2,-3,-4,-5,7});
  std::vector<int> orderVec(testVec);

  std::sort(orderVec.begin(),orderVec.end());
  orderVec.erase
    (std::unique(orderVec.begin(),orderVec.end()),
     orderVec.end());

  for(const int I : testVec)
    A.addItem(I);

  const std::vector<int> outVec=A.getAllCells();

  if (outVec!=orderVec)
    {
      ELog::EM<<"Failed :"<<ELog::endDiag;
      ELog::EM<<"TestVec = ";
      for(const int I : testVec)
	ELog::EM<<I<<" ";
      ELog::EM<<ELog::endDiag;

      ELog::EM<<"OrderVec = ";
      for(const int I : orderVec)
	ELog::EM<<I<<" ";
      ELog::EM<<ELog::endDiag;

      ELog::EM<<"OutVec = ";
      for(const int I : outVec)
	ELog::EM<<I<<" ";
      ELog::EM<<ELog::endDiag;

      ELog::EM<<"A == "<<A<<ELog::endDiag;
      return -1;
    }
  ELog::EM<<"A == "<<A<<ELog::endDiag;
  return 0;
}



int
testGroupRange::testMerge()
  /*!
    Tests the merge units
    \retval -1 on failure
    \retval 0 :: success 
  */
{
  ELog::RegMethod RegA("testGroupRange","testMerge");

  const std::vector<std::vector<int>> TVec
    ({
      {std::vector<int>({3,4,5,7,9,21,1}) },
      {std::vector<int>({1,4,5,8,21,1}) },
      {std::vector<int>({1,3,4,5,7,9,10,11,40}) },
	{std::vector<int>({4,5,6,7,8,9,10,11,12,13,14})},
      {std::vector<int>({4,5,6,7,8,9,10})}
    });     

  const std::vector<std::vector<int>> AVec
    ({
      {std::vector<int>({1,3,4,5,7,8,9,21}) },
      {std::vector<int>({1,3,4,5,6,7,8,9,10,11,12,13,14,40})},
      {std::vector<int>({1,3,4,5,6,7,8,9,10,11,12,13,14})},
      {std::vector<int>({1,3,4,5,6,7,8,9,10,11,40})}
    });     

  typedef std::tuple<size_t,size_t,size_t> TTYPE;
  const std::vector<TTYPE> Tests
    ({
        TTYPE(0,1,0),
	  TTYPE(2,3,1),
	  TTYPE(3,2,1),
	  TTYPE(2,4,3),
	  TTYPE(4,2,3)
     });
      
	
  for(const TTYPE& tc : Tests)
    {
      groupRange A(TVec[std::get<0>(tc)]);
      groupRange APrime(A);
      groupRange B(TVec[std::get<1>(tc)]);
      groupRange Res(AVec[std::get<2>(tc)]);

      A.combine(B);
      const std::vector<int> outVec=A.getAllCells();
      const std::vector<int>& ansVec=AVec[std::get<2>(tc)];

      
      if (ansVec!=outVec)
	{
	  ELog::EM<<"Failed :"<<ELog::endDiag;
	  ELog::EM<<"A    = "<<APrime<<ELog::endDiag;;
	  ELog::EM<<"B    = "<<B<<ELog::endDiag;;
	  ELog::EM<<"Comb = "<<A<<ELog::endDiag;;
	  ELog::EM<<"Res  = "<<Res<<ELog::endDiag;;
	  return -1;
	}
    }
  
  return 0;
}

int
testGroupRange::testRemove()
  /*!
    Tests the removal of units
    \retval -1 on failure
    \retval 0 :: success 
  */
{
  ELog::RegMethod RegA("testGroupRange","testRemoval");

  typedef std::tuple<std::string,int,std::string> TTYPE;
  const std::vector<TTYPE> Tests
    ({
        TTYPE("[100 200] [300 400]",101,"100 [102 200] [300 400]"),
      	TTYPE("[100 200] [300 400]",201,"[100 200] [300 400]"),
      	TTYPE("[100 200] [300 400]",200,"[100 199] [300 400]"),
	TTYPE("[100 200] [300 400]",100,"[101 200] [300 400]"),
	TTYPE("[100 200] [300 400]",300,"[100 200] [301 400]"),
	TTYPE("[100 200] [300 400]",301,"[100 200] 300 [302 400]")
    });

  int tCnt(0);
  for(const TTYPE& tc : Tests)
    {
      tCnt++;
      const int cellN=std::get<1>(tc);
      groupRange AGrp=makeGrp(std::get<0>(tc));
      AGrp.removeItem(cellN);
      groupRange BGrp=makeGrp(std::get<2>(tc));
      
      if (AGrp!=BGrp)
	{
	  ELog::EM<<"TEST = "<<tCnt<<ELog::endDiag;
	  ELog::EM<<"AGrp = "<<AGrp<<ELog::endDiag;
	  ELog::EM<<"BGrp = "<<BGrp<<ELog::endDiag;
	  return -1;
	}
    }
      
	      
  return 0;
}

int
testGroupRange::testValid()
  /*!
    Tests the valid units
    \retval -1 on failure
    \retval 0 :: success 
  */
{
  ELog::RegMethod RegA("testGroupRange","testMerge");

  typedef std::tuple<std::string,int,bool> TTYPE;
  const std::vector<TTYPE> Tests
    ({
      	TTYPE("[100 200] [300 400]",101,1),
        TTYPE("[100 200]",101,1),
	TTYPE("[100 200]",99,0),
	TTYPE("[100 200]",201,0),
	TTYPE("[100 200] [300 400]",201,0),
	TTYPE("[100 200] [300 400]",401,0),
	TTYPE("[100 200] [300 400]",301,1),
	  TTYPE("[100 200] [300 400]",99,0),
	  TTYPE("[100 200] [300 400]",100,1),
	  TTYPE("[100 200] [300 400]",300,1),
	  TTYPE("[100 200] [300 400]",400,1)
    });

  for(const TTYPE& tc : Tests)
    {
      groupRange AGrp;
      int A,B;
      std::string Line=std::get<0>(tc);
      while(!StrFunc::isEmpty(Line))
	{
	  std::string Units=StrFunc::getDelimUnit("[","]",Line);
	  if (StrFunc::section(Units,A) &&
	      StrFunc::section(Units,B))
	    {
	      AGrp.addItem(A,B);
	    }
	  else if (StrFunc::section(Line,A))
	    {
	      AGrp.addItem(A);
	    }
	  else
	    {
	      ELog::EM<<"Failed to read line "<<ELog::endDiag;
	      return -1;
	    }
	}
      if (AGrp.valid(std::get<1>(tc))!=std::get<2>(tc))
	{
	  ELog::EM<<"AGrp = "<<AGrp<<ELog::endDiag;
	  ELog::EM<<"cell = "<<std::get<1>(tc)<<ELog::endDiag;
	  ELog::EM<<"RET  = "<<AGrp.valid(std::get<1>(tc))<<ELog::endDiag;
	  return -1;
	}
    }
      
	      
  return 0;
}

  

