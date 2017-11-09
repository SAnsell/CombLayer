/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   test/testNRange.cxx
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
#include <cmath>
#include <vector>
#include <list>
#include <map>
#include <stack>
#include <string>
#include <sstream>
#include <numeric>
#include <algorithm>
#include <functional>
#include <iterator>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "support.h"
#include "Triple.h"
#include "NRange.h"

#include "testFunc.h"
#include "testNRange.h"

testNRange::testNRange() 
  /*!
    Constructor
  */
{}

testNRange::~testNRange() 
  /*!
    Destructor
  */
{}

int 
testNRange::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: Index parameter to test
    \retval -1 Range failed
    \retval 0 All succeeded
  */
{
  ELog::RegMethod RegA("testNRange","applyTest");
  TestFunc::regSector("testNRange");

  typedef int (testNRange::*testPtr)();
  testPtr TPtr[]=
    {
      &testNRange::testCondense,
      &testNRange::testOperator,
      &testNRange::testOutput,
      &testNRange::testRange
    };
  const std::string TestName[]=
    {
      "Condense",
      "Operator",
      "Output",
      "Range"
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
testNRange::testCondense()
  /*!
    Test the processsing with intervals 
    \retval -1 :: Unable to process line
    \retval 0 :: success
  */
{
  ELog::RegMethod RegA("testNRange","testCondense");

  const std::vector<std::string> TString=
    {
      "1 8log 1e+09 3r",
      "1 3r 4 5i 8",
      "1 8log 1e+09 3i 1.2e+09",
      "1e-3 251log 1e+09"
    };

  for(size_t i=0;i<TString.size();i++)
    {
      NRange NE;
      if (NE.processString(TString[i]))
	{
	  ELog::EM<<"Error with processing:"<<TString[i]<<ELog::endCrit;
	  return -1;
	}

      NE.condense(1e-6);
      std::ostringstream cx;
      cx<<NE;
      if (StrFunc::fullBlock(cx.str())!=TString[i])
	{
	  // Strip into components and test:
	  double VT,VC;
	  std::string ST,SC;
	  int flagT,flagC;
	  int errFlag(0);
	  std::string TItem=TString[i];
	  std::string CItem=cx.str();
	  do 
	    {
	      // Test if both numeric
	      flagT=StrFunc::section(TItem,VT);
	      flagC=StrFunc::section(CItem,VC);
	      if (flagT!=flagC ||                    // not same  : not equal
		  (flagT && fabs(VT-VC)>1e-6))       
		errFlag=1;
	      else if (!flagT)
		{
		  flagT=StrFunc::section(TItem,ST);
		  flagC=StrFunc::section(CItem,SC);
		  if ( flagT!=flagC || ST!=SC)
		    errFlag=1;
		}
	    } while(!errFlag && (flagT+flagC));
	    
	  if (errFlag)
	    {
	      ELog::EM<<"Test string == "<<TString[i]<<ELog::endWarn;
	      ELog::EM<<"NE == "<<NE<<ELog::endWarn;
	      return -2;
	    }
	}
    }
  return 0;
}

int
testNRange::testOperator() 
  /*!
    Test the processsing with intervals 
    \retval -1 :: Unable to process line
    \retval 0 :: success
  */
{
  ELog::RegMethod RegA("testNRange","testOperator");
  double results[]={0.001,0.01,0.1,1.0,10,100,
                  100,100,100,
		  1000,1001,1002,1003,1004,1005,
		  1006,1007,1008,1009,1010,1011  };

  const std::string testString("0.001 4log 100 3r 1000 49i 1050");
  NRange NE;
  if (NE.processString(testString))
    {
      ELog::EM<<"Error with processing:"<<testString<<ELog::endCrit;
      return -1;
    }
  // Basic string
  for(int i=0;i<20;i++)
    {
      if (fabs(results[i]-NE[i])/results[i]>1e-5)
	{
	  ELog::EM<<NE<<ELog::endBasic;
	  ELog::EM<<"NE["<<i<<"]=="<<NE[i]<<ELog::endTrace;
	  return -2;
	}
    }
  return 0;
}

int
testNRange::testOutput() 
  /*!
    Test the processsing with intervals 
    (does not actually check the intervals
    other than to write it to the screen)
    \retval -1 :: Unable to process line
    \retval 0 :: success
  */
{
  ELog::RegMethod RegA("testNRange","testOutput");

  std::string testString("1.0 8log 1e9 3r");
  std::string testStringB("1000.0 49i 1050.0");
  NRange NE;
  if (NE.processString(testString))
    {
      ELog::EM<<"Error with processing:"<<testString<<ELog::endCrit;
      return -1;
    }
  // Basic string
  std::vector<double> values;
  NE.writeVector(values);
  const double sum=accumulate(values.begin(),values.end(),0.0,
				   std::plus<double>());
  if (std::abs(sum-4.11111111e9)>1e3)
    {
      ELog::EM<<"Sum == "<<sum<<" from:"<<testString<<ELog::endWarn;
      ELog::EM<<"Data == ";
      copy(values.begin(),values.end(),
	   std::ostream_iterator<double>(ELog::EM.Estream(),","));
      ELog::EM<<ELog::endCrit;
      return -2;
    }

  NE.clear();
  if (NE.processString(testStringB))
    {
      ELog::EM<<"Error with processing (B):"<<testStringB<<ELog::endCrit;
      return -1;
    }
  // Basic string
  NE.writeVector(values);
  const double sumB=accumulate(values.begin(),values.end(),0.0,
				   std::plus<double>());
  if (std::abs(sumB-52275.0)>1e-3)
    {
      ELog::EM<<"Sum == "<<sumB<<" ("<<std::abs(sumB-52275.0)<<ELog::endWarn;
      ELog::EM<<" from:"<<testStringB<<ELog::endWarn;
      ELog::EM<<"Data == ";
      copy(values.begin(),values.end(),
	   std::ostream_iterator<double>(ELog::EM.Estream(),","));
      ELog::EM<<ELog::endCrit;
      return -3;
    }
  return 0;
}

int
testNRange::testRange() 
  /*!
    Test the processsing with intervals 
    (does not actually check the intervals
    other than to write it to the screen)
    \retval -1 :: Unable to process line
    \retval 0 :: success
  */
{
  ELog::RegMethod RegA("testNRange","testRange");

  const std::string testString("1.3e2 100i 1.6e6 5r 12e8 10log 3.7e9");
  const std::string expectString("130 100i 1.6e+06 5r 1.2e+09 10log 3.7e+09");

  NRange NE;
  if (NE.processString(testString))
    {
      ELog::EM<<"Error with NRange "<<ELog::endCrit;
      return -1;
    }
  std::ostringstream cx;
  NE.write(cx);
  if (StrFunc::fullBlock(cx.str())!=expectString)
    {
      ELog::EM<<"Failed on string match"<<ELog::endDiag;
      ELog::EM<<"Result == "<<cx.str()<<ELog::endDiag;
      ELog::EM<<"Expect == "<<expectString<<ELog::endDiag;
      return -1;
    }
  
  return 0;
}

