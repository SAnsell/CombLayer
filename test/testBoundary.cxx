/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   test/testBoundary.cxx
 *
 * Copyright (c) 2004-204 by Stuart Ansell
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
#include <fstream>
#include <sstream>
#include <cmath>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <functional>
#include <iterator>
#include <tuple>

#include "Exception.h"
#include "GTKreport.h"
#include "FileReport.h"
#include "NameStack.h"
#include "OutputLog.h"
#include "RegMethod.h"
#include "doubleErr.h"
#include "BUnit.h"
#include "Boundary.h"

#include "testFunc.h"
#include "testBoundary.h"

testBoundary::testBoundary()  
  /*!
    Constructor 
  */
{}

testBoundary::~testBoundary() 
  /*!
    Destructor
   */
{}

int 
testBoundary::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: control variable
    \returns -ve on error 0 on success.
  */
{

  ELog::RegMethod RegA("testBoundary","applyTest");
  TestFunc::regSector("testBoundary");

  typedef int (testBoundary::*testPtr)();
  testPtr TPtr[]=
    {
      &testBoundary::testBinBoundary,
      &testBoundary::testCreateBoundary

    };
  const std::string TestName[]=
    {
      "BinBoundary",
      "CreateBoundary"
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
testBoundary::testCreateBoundary()
  /*!
    Test of the sndValue functor.
    \returns -ve on error 0 on success.
  */
{
  ELog::RegMethod RegA("testBoundary","testCreateBoundary");

  // item A / frac A : Item B / frac b
  typedef std::tuple<size_t,double,size_t,double> RTYPE;

  std::vector<double> X;
  std::vector<double> Xnew;
  

  X.push_back(1);
  X.push_back(9.0);
  X.push_back(9.6);
  X.push_back(10);
  X.push_back(15);
  X.push_back(22);

  for(int i=-10;i<40;i++)
    Xnew.push_back(i);
  
  // Results are offset by getIndex
  std::vector<RTYPE> Results;
  Results.push_back(RTYPE(0,0.125,0,-1));
  Results.push_back(RTYPE(0,0.125,0,-1));
  Results.push_back(RTYPE(0,0.125,0,-1));
  Results.push_back(RTYPE(0,0.125,0,-1));
  Results.push_back(RTYPE(0,0.125,0,-1));
  Results.push_back(RTYPE(0,0.125,0,-1));
  Results.push_back(RTYPE(0,0.125,0,-1));
  Results.push_back(RTYPE(0,0.125,0,-1));
  Results.push_back(RTYPE(1,1.0,2,1.0));
  Results.push_back(RTYPE(3,0.2,0,-1));
  Results.push_back(RTYPE(3,0.2,0,-1));
  Results.push_back(RTYPE(3,0.2,0,-1));
  Results.push_back(RTYPE(3,0.2,0,-1));
  Results.push_back(RTYPE(3,0.2,0,-1));
  Results.push_back(RTYPE(4,1/7.0,0,-1));
  Results.push_back(RTYPE(4,1/7.0,0,-1));
  Results.push_back(RTYPE(4,1/7.0,0,-1));
  Results.push_back(RTYPE(4,1/7.0,0,-1));
  Results.push_back(RTYPE(4,1/7.0,0,-1));
  Results.push_back(RTYPE(4,1/7.0,0,-1));
  Results.push_back(RTYPE(4,1/7.0,0,-1));


  // Expected result:
  // Contributions are 
  
  // [0] 0:1    None
  // [1] 1:2    [0]1/8
  // [1] 1:2    [0]1/8

  Boundary BB;
  // Fractions of X that should be used if Xnew is the new bining
  // Add dead points are zero
  BB.setBoundary(X,Xnew);
  Boundary::BTYPE::const_iterator vc;
  std::vector<RTYPE>::const_iterator rc=Results.begin();

  // Start point in New for boundarys:
  size_t cnt(BB.getIndex());
  if (cnt!=11)
    {
      ELog::EM<<"Failed on getIndex() "<<ELog::endTrace;
      return -1;
    }
    
  for(vc=BB.begin();vc!=BB.end() && rc!=Results.end();vc++,rc++)
    {
      const size_t iA=std::get<0>(*rc);
      const double fA=std::get<1>(*rc);
      const size_t iB=std::get<2>(*rc);
      const double fB=std::get<3>(*rc);
      std::pair<size_t,double> ItemA=vc->getItem(0);
      if (ItemA.first!=iA || fabs(ItemA.second-fA)>1e-5)
	{
	  ELog::EM<<"Failed with item "<<vc-BB.begin()<<ELog::endCrit;
	  ELog::EM<<"fa == "<<fA<<" "<<ItemA.second<<ELog::endTrace;
	  break;
	}
      if (fB>=0)
	{
	  ItemA=vc->getItem(1);
	  if (ItemA.first!=iB || fabs(ItemA.second-fB)>1e-5)
	    {
	      ELog::EM<<"Failed with second item "
		      <<vc-BB.begin()<<ELog::endCrit;
	      ELog::EM<<"fb == "<<fB<<" "<<ItemA.second<<ELog::endTrace;
	      break;
	    }
	}
    }
  if (rc!=Results.end() || vc!=BB.end())
    {
      ELog::EM<<"FAILURE "<<ELog::endCrit;

      for(vc=BB.begin();vc!=BB.end() && rc!=Results.end();vc++,cnt++)
	{
	  ELog::EM<<cnt<<" "<<Xnew[cnt]<<" = "<<Xnew[cnt+1]<<" :: "
		  <<*vc<<ELog::endDiag;
	}  
      return -1;
    }
  return 0;
}

int
testBoundary::testBinBoundary()
  /*!
    Test of the 
    \returns -ve on error 0 on success.
   */
{
  ELog::RegMethod RegA("testBoundary","testBinBoundary");

  std::vector<BUnit> OrigN;
  std::vector<BUnit> NewN;

  OrigN.push_back(BUnit(0,1,DError::doubleErr(1,0.0)));
  OrigN.push_back(BUnit(3,4,DError::doubleErr(3,0.0)));
  OrigN.push_back(BUnit(7,8,DError::doubleErr(7,0.0)));

  NewN.push_back(BUnit(0,1,DError::doubleErr(1,0.0)));
  
  Boundary BB;

  // TEST ONE:
  BB.setBoundary(OrigN,NewN);
  if (BB.getIndex()!=0) 
    {
      ELog::EM<<"Incorrect index :"<<BB.getIndex()<<ELog::endWarn;      
      return -1;
    }
  std::pair<size_t,double> Res=BB.getItem(0,0);
  if (Res.first!=0 || fabs(Res.second-1.0)>1e-5)
    {
      ELog::EM<<"Incorrect components: "<<ELog::endWarn;
      size_t cnt(0);
      Boundary::BTYPE::const_iterator vc;
      for(vc=BB.begin();vc!=BB.end();vc++,cnt++)
	{
	  ELog::EM<<cnt<<"== "<<NewN[cnt]<<" :: "<<*vc<<ELog::endDiag;
	}  
      return -1;
    }

  // TEST TWO:
  NewN.clear();
  NewN.push_back(BUnit(0.5,1.5,DError::doubleErr(1,0.0)));
  BB.setBoundary(OrigN,NewN);
  if (BB.getIndex()!=0) 
    {
      ELog::EM<<"[Test 2] Incorrect index :"
	      <<BB.getIndex()<<ELog::endWarn;      
      return -2;
    }
  Res=BB.getItem(0,0);
  if (Res.first!=0 || fabs(Res.second-0.5)>1e-5)
    {
      ELog::EM<<"[Test 2] Incorrect components: "<<ELog::endWarn;
      size_t cnt(0);
      Boundary::BTYPE::const_iterator vc;
      for(vc=BB.begin();vc!=BB.end();vc++,cnt++)
	{
	  ELog::EM<<cnt<<"== "<<NewN[cnt]<<" :: "<<*vc<<ELog::endDiag;
	}  
      return -2;
    }

  // TEST THREE:
  NewN.clear();
  NewN.push_back(BUnit(0.5,5.0,DError::doubleErr(1,0.0)));
  BB.setBoundary(OrigN,NewN);
  if (BB.getIndex()!=0) 
    {
      ELog::EM<<"[Test 3] Incorrect index :"
	      <<BB.getIndex()<<ELog::endWarn;      
      return -3;
    }
  Res=BB.getItem(0,0);
  std::pair<int,double> ResB=BB.getItem(0,1);
  if (Res.first!=0 || fabs(Res.second-0.5)>1e-5
      || fabs(BB.getItem(0,1).second-1.0)>1e-5)
    {
      ELog::EM<<"[Test 3] Incorrect components: "<<ELog::endWarn;
      ELog::EM<<"A == "<<ResB.first<<" "<<ResB.second<<std::endl;
      size_t cnt(0);
      Boundary::BTYPE::const_iterator vc;
      for(vc=BB.begin();vc!=BB.end();vc++,cnt++)
	ELog::EM<<cnt<<"== "<<NewN[cnt]<<" :: "<<*vc<<ELog::endDiag;
      return -3;
    }

  // TEST FOUR:
  NewN.clear();
  NewN.push_back(BUnit(0.4,2.0,DError::doubleErr(1,0.0)));
  NewN.push_back(BUnit(0.9,3.5,DError::doubleErr(1,0.0)));
  BB.setBoundary(OrigN,NewN);
  if (BB.getIndex()!=0) 
    {
      ELog::EM<<"[Test 4] Incorrect index :"
	      <<BB.getIndex()<<ELog::endWarn;      
      return -4;
    }
  
  Res=BB.getItem(0,0);
  std::pair<int,double> ResA=BB.getItem(0,1);
  ResB=BB.getItem(1,0);
  std::pair<int,double> ResC=BB.getItem(1,1);

  if (Res.first!=0 || fabs(Res.second-0.6)>1e-5 ||
      ResA.first!=-1 || fabs(ResA.second)>1e-5 ||
      ResB.first!=0 || fabs(ResB.second-0.1)>1e-5 ||
      ResC.first!=1 || fabs(ResC.second-0.5)>1e-5)
    {
      ELog::EM<<"[Test 4] Incorrect components: "<<ELog::endWarn;
      ELog::EM<<"0 == "<<Res.first<<" "<<Res.second<<std::endl;
      ELog::EM<<"A == "<<ResA.first<<" "<<ResA.second<<std::endl;
      ELog::EM<<"B == "<<ResB.first<<" "<<ResB.second<<std::endl;
      ELog::EM<<"C == "<<ResC.first<<" "<<ResC.second<<std::endl;
      size_t cnt(0);
      Boundary::BTYPE::const_iterator vc;
      for(vc=BB.begin();vc!=BB.end();vc++,cnt++)
	ELog::EM<<cnt<<" == "<<NewN[cnt]<<" :: "<<*vc<<ELog::endDiag;

      return -4;
    }
      
  return 0;
}
