/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   test/testMultiString.cxx
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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
#include <map>
#include <tuple>
#include <memory>
#include <numeric>

#include <type_traits>
#include <typeinfo>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "dataSlice.h"
#include "multiData.h"
#include "IndexCounter.h"

#include "testFunc.h"
#include "testMultiString.h"


testMultiString::testMultiString() 
  /*!
    Constructor
  */
{}

testMultiString::~testMultiString() 
  /*!
    Destructor
  */
{}

int 
testMultiString::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: parameter to decide test
    \retval -1 :: Fail on angle
  */
{
  ELog::RegMethod RegA("testMultiString","applyTest");
  TestFunc::regSector("testMultiString");

  typedef int (testMultiString::*testPtr)();
  testPtr TPtr[]=
    {
      &testMultiString::testExchange,
      &testMultiString::testMultiRange,
      &testMultiString::testRange,
      &testMultiString::testSet
    };
  const std::vector<std::string> TestName=
    {
      "Exchange",
      "MultiRange",
      "Range",
      "Set"
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
testMultiString::testExchange()
  /*!
    Test the exchange of two rows of a multidata
    \retval -1 on failure
    \retval 0 :: success 
  */
{
  ELog::RegMethod RegA("testMultiString","testExchange");
  // nA,nB,nC,nD,aIndex,bIndex
  typedef std::tuple<size_t,size_t,size_t,size_t,size_t,size_t> TTYPE;

  const std::vector<TTYPE> Tests={
    { 4,4,0,0,  0,1},
    { 3,4,0,0,  0,1},
    { 3,3,3,0,  0,1},
    { 3,3,3,0,  1,2},
    { 3,6,3,0,  0,2},
    { 3,6,7,9,  0,2}
  };

  for(const TTYPE& tc : Tests)
    {
      const size_t iA(std::get<0>(tc));
      const size_t iB(std::get<1>(tc));
      const size_t iC(std::get<2>(tc));
      const size_t iD(std::get<3>(tc));

      const size_t aExchange(std::get<4>(tc));
      const size_t bExchange(std::get<5>(tc));

      std::vector<size_t> index;
      index.push_back(iA);  
      index.push_back(iB);  // min size 2
      if (iC) index.push_back(iC);
      if (iD) index.push_back(iD);

      multiData<std::string> X(index);

      std::vector<std::string>& data=X.getVector();
      for(size_t i=0;i<data.size();i++)
	data[i]=std::to_string(i+1);

      multiData<std::string> Y=X.exchange(aExchange,bExchange);

      IndexCounter ICounter(index);
      bool eFlag(0);
      do
	{
	  const std::vector<size_t>& index=ICounter.getVector();
	  std::vector<size_t> indexNew=index;
	  std::swap(indexNew[aExchange],indexNew[bExchange]);
	  const std::string AO=X.value(index);
	  const std::string BO=Y.value(indexNew);
	  std::string CO=BO;
	  if (index.size()==2)
	     CO=Y.get()[indexNew[0]][indexNew[1]];
	  if (index.size()==3)
	     CO=Y.get()[indexNew[0]][indexNew[1]][indexNew[2]];
	  if (index.size()==4)
	     CO=Y.get()[indexNew[0]][indexNew[1]][indexNew[2]][indexNew[3]];
	  if (AO!=BO || AO!=CO)
	    {
	      ELog::EM<<"Error "<<AO<<" "<<BO<<" "<<ICounter<<ELog::endDiag;
	      eFlag=1;
	    }
	} while(!(ICounter++));

      if (eFlag)
	{
	  ELog::EM<<"X == "<<X<<ELog::endDiag;
	  ELog::EM<<"Y == "<<Y<<ELog::endDiag;
	}

    }
  return 0;
}


int
testMultiString::testMultiRange()
  /*!
    Tests the abilty to integrate over a shapme baesd on multi indexes
    \retval -1 on failure
    \retval 0 :: success 
  */
{
  ELog::RegMethod RegA("testMultiData","testMultiRange");

  // nA,nB,nC range(),range(),range()
  typedef std::tuple<size_t,size_t,size_t,
		     size_t,size_t,size_t,
		     size_t,size_t,size_t> TTYPE;
  std::vector<TTYPE> Tests={
    { 10,10,10, 0,0, 0,9, 1,1},
    { 10,10,10, 0,10, 0,9, 1,1},
    { 1000,121,1064, 0,1000,0,12310, 128,128}  // note shape is oversize

  };

  for(const TTYPE& tc : Tests)
    {
      const size_t iA(std::get<0>(tc));
      const size_t iB(std::get<1>(tc));
      const size_t iC(std::get<2>(tc));
      std::vector<std::string> C(iA*iB*iC);
      size_t index=0;
      for(size_t i=0;i<iA;i++)
	for(size_t j=0;j<iB;j++)
	  for(size_t k=0;k<iC;k++)
	    {
	      C[index]=std::to_string((i*1000000)+(j*1000)+k);	    
	      index++;
	    }

      multiData<std::string> MC(iA,iB,iC,C);
      // gives a 2x5 array
      size_t r[6]=
	{std::get<3>(tc),std::get<4>(tc),
	 std::get<5>(tc),std::get<6>(tc),
	 std::get<7>(tc),std::get<8>(tc)};
      for(size_t i=0;i<3;i++)
	if (r[2*i]>=MC.shape()[i])
	  r[2*i]=MC.shape()[i]-1;
      
      multiData<std::string> outC=MC.getRange({
	  {r[0],r[1]},{r[2],r[3]},{r[4],r[5]}});
      
      index=0;

      for(size_t i=r[0];i<=r[1] && i<MC.shape()[0];i++)
	for(size_t j=r[2];j<=r[3] && j<MC.shape()[1];j++)
	  for(size_t k=r[4];k<=r[5] && k<MC.shape()[2];k++)
	    {
	      const std::string value=MC.get()[i][j][k];
	      const size_t tx=i-r[0];
	      const size_t ty=j-r[2];
	      const size_t tz=k-r[4];
	      const std::string testVal=outC.get()[tx][ty][tz];
	      if (value!=testVal)
		{
		  ELog::EM<<"Size == "<<outC.size()<<" "<<MC.size()
			  <<ELog::endDiag;
		  ELog::EM<<"Error with point ["
			  <<i<<"]["<<j<<"]["<<k<<"] == ["
			  <<tx<<"]["<<ty<<"]["<<tz<<"] :: "
			  <<value<<" : "<<testVal
			  <<ELog::endDiag;
		  return -1;
		}
	    }
    }
  return 0;
}

int
testMultiString::testRange()
  /*!
    Tests the Vec3D dot prod
    \retval -1 on failure
    \retval 0 :: success 
  */
{
  ELog::RegMethod RegA("testMultiString","testRange");

  // nA,nB,nC range(),range(),range()
  typedef std::tuple<size_t,size_t,size_t,
		     size_t,size_t,size_t,
		     size_t,size_t,size_t,
		     size_t> TTYPE;
  std::vector<TTYPE> Tests={
    { 10,10,10,  1,1, 3,3, 0,0, 1},
    { 10,10,10,  0,0, 3,3, 1,3, 3}
  };
  
  for(const TTYPE& tc : Tests)
    {
      const size_t iA(std::get<0>(tc));
      const size_t iB(std::get<1>(tc));
      const size_t iC(std::get<2>(tc));
      std::vector<std::string> C(iA*iB*iC);
      size_t index=0;
      for(size_t i=0;i<iA;i++)
	for(size_t j=0;j<iB;j++)
	  for(size_t k=0;k<iC;k++)
	    {
	      C[index]=std::to_string((1000000*i)+(1000*j)+k);	    
	      index++;
	    }
      multiData<std::string> MC(iA,iB,iC,C);
      
      // gives a 2x5 array
      size_t r[6]=
	{std::get<3>(tc),std::get<4>(tc),
	 std::get<5>(tc),std::get<6>(tc),
	 std::get<7>(tc),std::get<8>(tc)};

      std::vector<std::string> outC=MC.getFlatRange
	({ {r[0],r[1]},{r[2],r[3]},{r[4],r[5]} });

      if (outC.size()!=std::get<9>(tc))
	{
	  ELog::EM<<"OutC == "<<outC.size()<<ELog::endDiag;
	  return -1;
	}
    }


  return 0;
}

int
testMultiString::testSet()
  /*!
    Tests the Vec3D dot prod
    \retval -1 on failure
    \retval 0 :: success 
  */
{
  ELog::RegMethod RegA("testMultiString","testSet");

  //  template <typename T> std::string type_name();
  //  multiData<std::string> Out(30,4,6);


  multiData<std::string> Out1(10);



  //  SU += unit;
  std::string unit("A");
  Out1.get()[2] = "Unit";
  Out1.get()[4] = unit+":X";
  Out1.get()[3] = Out1.get()[2];
  Out1.get()[4] += std::string("ASDFAF");
  for(size_t i=0;i<5;i++)
    {
      if (Out1.get()[i] != std::string(""))
	{
	  std::string item=Out1.get()[i];
	  ELog::EM<<"OutONE["<<i<<"] == "<<
	    item<<ELog::endDiag;
	}
    }
  // 3D set
  multiData<std::string> Out(30,10,10);
  Out.get()[0][0][0]=std::to_string(10);
  Out.get()[20][6][3]=std::to_string(20*6*3);
  Out.get()[10][9][2]=std::string("Teststring");
  Out.get()[10][9][2]=std::to_string(10*9*2);
  Out.get()[10][0][0] = Out.get()[20][6][3];

  for(size_t i=0;i<30;i++)
    for(size_t j=0;j<10;j++)
      for(size_t k=0;k<10;k++)
	{
	if (Out.get()[i][j][k]!="")
	  ELog::EM<<"Out["<<i<<"]["<<j<<"]["<<k<<"] == "
		  <<std::string(Out.get()[i][j][k])<<ELog::endDiag;
      }

  
  /*
  std::vector<std::string> V;
  for(size_t i=0;i<30;i++)
    V.push_back(std::to_string(i));
  
  multiData<std::string> Out(30,V);
  
  for(size_t i=0;i<30;i++)
    {
      std::string item=Out.get()[i];
      ELog::EM<<"Out["<<i<<"] "<<item
	      <<ELog::endDiag;
    }
  */
  return 0;
}
