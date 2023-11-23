/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   test/testMultiData.cxx
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
#include "testMultiData.h"


testMultiData::testMultiData() 
  /*!
    Constructor
  */
{}

testMultiData::~testMultiData() 
  /*!
    Destructor
  */
{}

int 
testMultiData::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: parameter to decide test
    \retval -1 :: Fail on angle
  */
{
  ELog::RegMethod RegA("testMultiData","applyTest");
  TestFunc::regSector("testMultiData");

  typedef int (testMultiData::*testPtr)();
  testPtr TPtr[]=
    {
      &testMultiData::testExchange,
      &testMultiData::testGetAxisRange,
      &testMultiData::testIntegrateMap,
      &testMultiData::testIntegrateMapRange,
      &testMultiData::testIntegrateValue,
      &testMultiData::testMultiRange,
      &testMultiData::testProjectMap,
      &testMultiData::testRange,
      &testMultiData::testSet
    };
  const std::vector<std::string> TestName=
    {
      "Exchange",
      "getAxisRange",
      "IntegrateMap",
      "IntegrateMapRange",
      "IntegrateValue",
      "MultiRange",
      "ProjectMap",
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
testMultiData::testExchange()
  /*!
    Test the exchange of two rows of a multidata
    \retval -1 on failure
    \retval 0 :: success 
  */
{
  ELog::RegMethod RegA("testMultiData","testExchange");
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

      multiData<int> X(index);

      std::vector<int>& data=X.getVector();
      std::iota(data.begin(),data.end(),1);


      multiData<int> Y=X.exchange(aExchange,bExchange);

      IndexCounter ICounter(index);
      bool eFlag(0);
      do
	{
	  const std::vector<size_t>& index=ICounter.getVector();
	  std::vector<size_t> indexNew=index;
	  std::swap(indexNew[aExchange],indexNew[bExchange]);
	  const int AO=X.value(index);
	  const int BO=Y.value(indexNew);
	  int CO=BO;
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
testMultiData::testSet()
  /*!
    Tests the Vec3D dot prod
    \retval -1 on failure
    \retval 0 :: success 
  */
{
  ELog::RegMethod RegA("testMultiData","testSet");

  multiData<int> Out(30,10,10);
  /*  Out.get()[0][0][0]=10;
  Out.get()[20][6][3]=20;
  Out.get()[10][9][2]=7;
  Out.get()[10][9][2]=8;
  Out.get()[10][9][2] = (Out.get()[20][6][3]);
  */
  multiData<int> Out1(10);
  Out1.get()[2]=10;
  Out1.get()[2]+=10;
  ELog::EM<<" ----------------- "<<ELog::endDiag;
  Out1.get()[3]=Out1.get()[2];
  for(size_t i=0;i<10;i++)
    if (Out1.get()[i]!=0)
      ELog::EM<<"OutONE["<<i<<"] == "<<
	Out1.get()[i]<<ELog::endDiag;
	
  
  for(size_t i=0;i<30;i++)
    for(size_t j=0;j<10;j++)
      for(size_t k=0;k<10;k++)
	{
	if (Out.get()[i][j][k]!=0)
	  ELog::EM<<"Out["<<i<<"]["<<j<<"]["<<k<<"] == "
		  <<Out.get()[i][j][k]<<ELog::endDiag;
      }
  return 0;
}

int
testMultiData::testIntegrateMap()
  /*!
    Thes the integration of a map in one direction
    \retval -1 on failure
    \retval 0 :: success 
  */
{

  // nA,nB,nC axis index :: sum
  typedef std::tuple<size_t,size_t,size_t,size_t> TTYPE;

  std::vector<TTYPE> Tests={
    { 5,5,5,  0},
    { 5,5,5,  1},
    { 5,8,9,  0},
    { 5,8,9,  2}
  };

  for(const TTYPE& tc : Tests)
    {
      const size_t iA(std::get<0>(tc));
      const size_t iB(std::get<1>(tc));
      const size_t iC(std::get<2>(tc));
      const size_t axisIndex(std::get<3>(tc));
      
      std::vector<int> C(iA*iB*iC);
      size_t index=0;
      for(size_t i=0;i<iA;i++)
	for(size_t j=0;j<iB;j++)
	  for(size_t k=0;k<iC;k++)
	    {
	      C[index]=static_cast<int>((1000000*i)+(1000*j)+k);
	      index++;
	    }

      multiData<int> MC(iA,iB,iC,C);

      
      multiData<int> Out=MC.integrateMap(axisIndex,{{},{},{}});

      multiData<int> OutX(Out.shape());
      for(size_t i=0;i<MC.shape()[0];i++)
	for(size_t j=0;j<MC.shape()[1];j++)
	  for(size_t k=0;k<MC.shape()[2];k++)
	    {
	      const size_t jB((axisIndex) ? i : j);
	      const size_t jC((axisIndex==2) ? j : k);
	      OutX.get()[jB][jC]+=MC.get()[i][j][k];
	    }
      for(size_t i=0;i<Out.shape()[0];i++)
	for(size_t j=0;j<Out.shape()[1];j++)
	  {
	    if (Out.get()[i][j] != OutX.get()[i][j])
	      {
		std::cout<<"Out["<<i<<"]["<<j<<"] == "
			 <<Out.get()[i][j]<<" :: "<<OutX.get()[i][j]<<std::endl;
		return -1;
	      }
	  }


    }
  return 0;
}

int
testMultiData::testIntegrateMapRange()
  /*!
    Thes the integration of a map in one direction
    \retval -1 on failure
    \retval 0 :: success 
  */
{
  // nA,nB,nC axis index :: sum
  typedef std::tuple<size_t,size_t,size_t,size_t,size_t,size_t> TTYPE;

  std::vector<TTYPE> Tests={
    { 5,5,5,  0,3,4},
    { 5,5,5,  1,3,4}
  };


  for(const TTYPE& tc : Tests)
    {
      const size_t iA(std::get<0>(tc));
      const size_t iB(std::get<1>(tc));
      const size_t iC(std::get<2>(tc));
      const size_t axisIndex(std::get<3>(tc));
      
      std::vector<int> C(iA*iB*iC);
      size_t index=0;
      for(size_t i=0;i<iA;i++)
	for(size_t j=0;j<iB;j++)
	  for(size_t k=0;k<iC;k++)
	    {
	      C[index]=static_cast<int>((1000000*i)+(1000*j)+k);
	      index++;
	    }
      
      multiData<int> MC(iA,iB,iC,C);

      size_t rA=std::get<4>(tc);
      size_t rB=std::get<5>(tc);
      size_t r[6]={0,100,0,100,0,100};

      for(size_t i=0;i<3;i++)
	{
	  if (i==axisIndex)
	    {
	      r[2*i]=rA;
	      r[2*i+1]=rB;
	    }
	  if (r[2*i+1]>MC.shape()[i])
	    r[2*i+1]=MC.shape()[i];
	}
      multiData<int> Out=MC.integrateMap
	(axisIndex,{{r[0],r[1]},{r[2],r[3]},{r[4],r[5]}});
      
      multiData<int> OutX(Out.shape());
      for(size_t i=0;i<MC.shape()[0];i++)
	for(size_t j=0;j<MC.shape()[1];j++)
	  for(size_t k=0;k<MC.shape()[2];k++)
	    {
	      const size_t aa[]={i,j,k};
	      const size_t jA=aa[axisIndex];
	      const size_t jB((axisIndex) ? i : j);
	      const size_t jC((axisIndex==2) ? j : k);
	      if (jA>=rA && jA<=rB)
		OutX.get()[jB][jC]+=MC.get()[i][j][k];
	    }
      int errFlag(0);
      for(size_t i=0;i<Out.shape()[0];i++)
	for(size_t j=0;j<Out.shape()[1];j++)
	  {
	    if (Out.get()[i][j] != OutX.get()[i][j])
	      {
		std::cout<<"Out["<<i<<"]["<<j<<"] == "
			 <<Out.get()[i][j]<<" :: "<<OutX.get()[i][j]<<std::endl;
		errFlag= -1;
	      }
	  }
      if (errFlag) return errFlag;
    }
  return 0;
}

int
testMultiData::testIntegrateValue()
  /*!
    Thes the integration of a map in N dimensions
    with ranges
    \retval -1 on failure
    \retval 0 :: success 
  */
{

  // nA,nB,nC [rangeA:rangeB/rangeA:rangeB/rangeA:rangeB]
  typedef std::tuple<size_t,size_t,size_t,
		     size_t,size_t,size_t,size_t,size_t,size_t,
		     int> TTYPE;

  std::vector<TTYPE> Tests={
    { 5,5,5,  1,1,0,0,0,0, 1000000 },
    { 5,5,5,  1,4,1,2,2,4, 1000000 }
  };

  for(const TTYPE& tc : Tests)
    {
      const size_t iA(std::get<0>(tc));
      const size_t iB(std::get<1>(tc));
      const size_t iC(std::get<2>(tc));

      // populate multiMap::
      std::vector<int> C(iA*iB*iC);
      size_t index=0;
      for(size_t i=0;i<iA;i++)
	for(size_t j=0;j<iB;j++)
	  for(size_t k=0;k<iC;k++)
	    {
	      C[index]=static_cast<int>((1000000*i)+(1000*j)+k);
	      index++;
	    }
      multiData<int> MC(iA,iB,iC,C);
      // get ranges:
      size_t r[6]=
	{std::get<3>(tc),std::get<4>(tc),
	 std::get<5>(tc),std::get<6>(tc),
	 std::get<7>(tc),std::get<8>(tc)};
      for(size_t i=0;i<3;i++)
	if (r[2*i]>=MC.shape()[i])
	  r[2*i]=MC.shape()[i]-1;

      
      // get range
      std::vector<int> outC=MC.getFlatRange({
	  {r[0],r[1]},{r[2],r[3]},{r[4],r[5]}});
      int sum(0);
      for(const int i : outC)
	sum+=i;
      
      const int V=MC.integrateValue({{r[0],r[1]},{r[2],r[3]},{r[4],r[5]}});      
      if (sum!=V) return -1;
    }
  return 0;
}



int
testMultiData::testProjectMap()
  /*!
    This projects in one direction, and integrats
    all other directions (and ranges if used)
    \retval -1 on failure
    \retval 0 :: success 
  */
{

  // nA,nB,nC axis index :: sum
  typedef std::tuple<size_t,size_t,size_t,size_t> TTYPE;

  std::vector<TTYPE> Tests={
    { 5,5,5,  0},
    { 5,5,5,  1},
    { 5,8,9,  0},
    { 5,8,9,  2}
  };

  for(const TTYPE& tc : Tests)
    {
      const size_t iA(std::get<0>(tc));
      const size_t iB(std::get<1>(tc));
      const size_t iC(std::get<2>(tc));
      const size_t axisIndex(std::get<3>(tc));
      
      std::vector<int> C(iA*iB*iC);
      size_t index=0;
      for(size_t i=0;i<iA;i++)
	for(size_t j=0;j<iB;j++)
	  for(size_t k=0;k<iC;k++)
	    {
	      C[index]=static_cast<int>((1000000*i)+(1000*j)+k);
	      index++;
	    }

      multiData<int> MC(iA,iB,iC,C);
      
      multiData<int> Out=MC.projectMap(axisIndex,{{},{},{}});

      multiData<int> OutX(Out.shape());
      for(size_t i=0;i<MC.shape()[0];i++)
	for(size_t j=0;j<MC.shape()[1];j++)
	  for(size_t k=0;k<MC.shape()[2];k++)
	    {
	      const size_t aa[]={i,j,k};
	      const size_t iA=aa[axisIndex];
	      OutX.get()[iA]+=MC.get()[i][j][k];
	    }
      
      for(size_t i=0;i<Out.shape()[0];i++)
	{
	  if (Out.get()[i] != OutX.get()[i])
	    {
	      std::cout<<"Out["<<i<<"] == "
		       <<Out.get()[i]<<" :: "<<OutX.get()[i]
		       <<std::endl;
	      return -1;
	    }
	}
      

    }
  return 0;
}

int
testMultiData::testGetAxisRange()
  /*!
    Tests removeing an axis of data
    \retval -1 on failure
    \retval 0 :: success 
  */
{
  ELog::RegMethod RegA("testMultiData","testGetAxisRange");

  // nA,nB,nC axis index :: sum
  typedef std::tuple<size_t,size_t,size_t,
		     size_t,size_t> TTYPE;

  std::vector<TTYPE> Tests={
    { 5,5,5,  0,2},
    { 5,5,5,  1,2}
  };

  for(const TTYPE& tc : Tests)
    {
      const size_t iA(std::get<0>(tc));
      const size_t iB(std::get<1>(tc));
      const size_t iC(std::get<2>(tc));
      const size_t axisIndex(std::get<3>(tc));
      const size_t iValue(std::get<4>(tc));
      
      std::vector<int> C(iA*iB*iC);
      size_t index=0;
      for(size_t i=0;i<iA;i++)
	for(size_t j=0;j<iB;j++)
	  for(size_t k=0;k<iC;k++)
	    {
	      C[index]=static_cast<int>((1000000*i)+(1000*j)+k);
	      index++;
	    }

      multiData<int> MC(iA,iB,iC,C);

      // get the vector of all the points NO in axisIndex
      std::vector<int> Out=MC.getAxisRange(axisIndex,iValue);
      std::vector<int>::iterator vc=Out.begin();
      index=0;
      const int multi[3]={1000000,1000,1};
      size_t iEnd[]={iA,iB,iC};
      iEnd[axisIndex]=1;
      for(size_t i=0;i<iEnd[0];i++)
	for(size_t j=0;j<iEnd[1];j++)
	  for(size_t k=0;k<iEnd[2];k++)
	    {
	      const int expect=
		static_cast<int>(i)*multi[0]+
		static_cast<int>(j)*multi[1]+
		static_cast<int>(k)+
		multi[axisIndex]*static_cast<int>(iValue);
	      if (expect != *vc)
		{
		  ELog::EM<<"Failed on test"<<i<<" "<<j<<" "<<k<<ELog::endDiag;
		  return -1;
		}
	      vc++;
	    }
    }
  
  return 0;
}

int
testMultiData::testRange()
  /*!
    Gets the vector of values based on a range:
    \retval -1 on failure
    \retval 0 :: success 
  */
{
  ELog::RegMethod RegA("testMultiData","testRange");

  // nA,nB,nC range(),range(),range()
  typedef std::tuple<size_t,size_t,size_t,
		     size_t,size_t,size_t,
		     size_t,size_t,size_t> TTYPE;
  std::vector<TTYPE> Tests={
    { 10,10,10,  1,1, 3,3, 0,0},
    { 10,10,10,  0,0, 3,3, 1,3}
  };

  for(const TTYPE& tc : Tests)
    {
      const size_t iA(std::get<0>(tc));
      const size_t iB(std::get<1>(tc));
      const size_t iC(std::get<2>(tc));
      std::vector<int> C(iA*iB*iC);
      size_t index=0;
      for(size_t i=0;i<iA;i++)
	for(size_t j=0;j<iB;j++)
	  for(size_t k=0;k<iC;k++)
	    {
	      C[index]=static_cast<int>((1000000*i)+(1000*j)+k);	    
	      index++;
	    }

      multiData<int> MC(iA,iB,iC,C);
      
      // gives a 2x5 array
      size_t r[6]=
	{std::get<3>(tc),std::get<4>(tc),
	 std::get<5>(tc),std::get<6>(tc),
	 std::get<7>(tc),std::get<8>(tc)};
      // for(size_t i=0;i<3;i++)
      // 	if (r[2*i]>=MC.shape()[i])
      // 	  r[2*i]=MC.shape()[i]-1;

      std::vector<int> outC=MC.getFlatRange
	({ {r[0],r[1]},{r[2],r[3]},{r[4],r[5]} });

      ELog::EM<<"OutC == "<<outC.size()<<ELog::endDiag;
    }
  
  return 0;
}

int
testMultiData::testMultiRange()
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
      std::vector<int> C(iA*iB*iC);
      size_t index=0;
      for(size_t i=0;i<iA;i++)
	for(size_t j=0;j<iB;j++)
	  for(size_t k=0;k<iC;k++)
	    {
	      C[index]=static_cast<int>((i*1000000)+(j*1000)+k);	    
	      index++;
	    }
      multiData<int> MC(iA,iB,iC,C);
      // gives a 2x5 array
      size_t r[6]=
	{std::get<3>(tc),std::get<4>(tc),
	 std::get<5>(tc),std::get<6>(tc),
	 std::get<7>(tc),std::get<8>(tc)};
      for(size_t i=0;i<3;i++)
	if (r[2*i]>=MC.shape()[i])
	  r[2*i]=MC.shape()[i]-1;
      
      multiData<int> outC=MC.getRange({
	  {r[0],r[1]},{r[2],r[3]},{r[4],r[5]}});
      
      index=0;

      for(size_t i=r[0];i<=r[1] && i<MC.shape()[0];i++)
	for(size_t j=r[2];j<=r[3] && j<MC.shape()[1];j++)
	  for(size_t k=r[4];k<=r[5] && k<MC.shape()[2];k++)
	    {
	      const int value=MC.get()[i][j][k];
	      const size_t tx=i-r[0];
	      const size_t ty=j-r[2];
	      const size_t tz=k-r[4];
	      const int testVal=outC.get()[tx][ty][tz];
	      if (value!=testVal)
		{
		  ELog::EM<<"Size == "<<outC.size()<<" "<<MC.size()
			  <<ELog::endDiag;
		  ELog::EM<<"Error with point ["
			  <<i<<"]["<<j<<"]["<<k<<"] == ["
			  <<tx<<"]["<<ty<<"]["<<tz<<"] :: "
			  <<value<<" : "<<testVal<<" D="
			  <<value-testVal
			  <<ELog::endDiag;
		  return -1;
		}
	    }
    }
  return 0;
}
