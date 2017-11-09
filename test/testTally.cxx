/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   test/testTally.cxx
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
#include <algorithm>
#include <iterator>
#include <tuple>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Triple.h"
#include "NRange.h"
#include "NList.h"
#include "MatrixBase.h"
#include "Vec3D.h"
#include "support.h"
#include "Quaternion.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Tally.h"
#include "pointTally.h"
#include "tallyFactory.h"

#include "testFunc.h"
#include "testTally.h"

using namespace tallySystem;

testTally::testTally() 
  /*!
    Constructor
  */
{}

testTally::~testTally() 
  /*!
    Destructor
  */
{}


int 
testTally::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: index of test
    \retval 0 All succeeded
  */
{
  ELog::RegMethod RegA("testTally","applyTest");
  TestFunc::regSector("testTally");

  typedef int (testTally::*testPtr)();
  testPtr TPtr[]=
    {
      &testTally::testAddParticles,
      &testTally::testAddLine,
      &testTally::testFirstLine,
      &testTally::testFuCard,
      &testTally::testGetElm,
      &testTally::testGetID

    };
  const std::string TestName[]=
    {
      "AddParticles",
      "AddLine",
      "FirstLine",
      "FuCard",
      "GetElm",
      "GetID"
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
testTally::testGetID() 
  /*!
    Test a simple GetID call
    \retval -1 :: Unable to process line
    \retval 0 :: success
  */
{
  ELog::RegMethod RegA("testTally","testGetID");

  std::string Line="f56:n,x 4.5";
  int ID=Tally::getID(Line);
  if (ID!=56)
    {
      ELog::EM<<"Error with getID "<<Line<<ELog::endTrace;
      ELog::EM<<"ID == "<<ID<<ELog::endTrace;
      return -1;
    }
 
  Line=" 1.0e1 2.0e2";
  ID=Tally::getID(Line);
  if (ID>=0)
    {
      ELog::EM<<"Error with getID "<<Line<<ELog::endTrace;
      ELog::EM<<"ID == "<<ID<<ELog::endTrace;
      return -1;
    }
  return 0;
}

int
testTally::testGetElm() 
  /*!
    Test a simple GetElm call
    \retval -1 :: Unable to process line
    \retval 0 :: success
  */
{
  std::string Line="f56:n,x 4.5";
  std::pair<std::string,int> PX=Tally::getElm(Line);
  if (PX.second!=56 || PX.first!="f")
    {
      ELog::EM<<"Error with getID "<<Line<<ELog::endTrace;
      ELog::EM<<"ID == "<<PX.first<<" "<<PX.second<<ELog::endTrace;
      return -1;
    }
  return 0;
}

int
testTally::testFirstLine() 
  /*!
    Test a simple firstLine call
    \retval -1 :: Normal line caused failure
    \retval -2 :: Comment line caused failure
    \retval -3 :: Continuation line caused failure
    \retval 0 :: success
  */
{
  std::string Line="f56:n,x 4.5";
  int out=Tally::firstLine(Line);
  if (out!=1)
    {
      ELog::EM<<"Error with firstLine "<<Line<<ELog::endTrace;
      ELog::EM<<"Ret == "<<out<<ELog::endTrace;
      return -1;
    }

  Line="c f56:n,x 4.5";
  out=Tally::firstLine(Line);
  if (out!=-1)
    {
      ELog::EM<<"Error with firstLine "<<Line<<ELog::endTrace;
      ELog::EM<<"Ret == "<<out<<ELog::endTrace;
      return -2;
    }

  Line="  1.0E1 1.4578e 2.E1";
  out=Tally::firstLine(Line);
  if (out!=0)
    {
      ELog::EM<<"Error with firstLine "<<Line<<ELog::endTrace;
      ELog::EM<<"Ret == "<<out<<ELog::endTrace;
      return -3;
    }

  return 0;
}

int
testTally::testAddLine() 
  /*!
    Test a simple AddLine
    \retval -1 :: Unable to process line
    \retval 0 :: success
  */
{
  ELog::RegMethod RegA("testTally","testAddLine");


  Tally A(56);
  std::string Line="f56:n, -x ,ye,s 4.5";
  int out=A.addLine(Line);
  if (out)
    {
      ELog::EM<<"Error with addLine "<<Line<<ELog::endTrace;
      ELog::EM<<"Out == "<<out<<ELog::endTrace;
      return -1;
    }
  /*
  const std::vector<std::string>& PP=A.getParticles();
  ELog::EM<<"P == :";
  copy(PP.begin(),PP.end(),
       std::ostream_iterator<std::string>(ELog::EM.Estream(),","));
  ELog::EM<<ELog::endTrace;
  */
  return 0;
}

int
testTally::testAddParticles() 
  /*!
    Test a simple addPParticle
    \retval -1 :: Unable to process line
    \retval 0 :: success
  */
{
  ELog::RegMethod RegA("testTally","testAddParticles");

  typedef std::tuple<std::string,std::string> TTYPE;
  std::vector<TTYPE> Tests;
  
  Tests.push_back(TTYPE("n,p","n,p"));
  Tests.push_back(TTYPE("n, e, p","n,e,p"));

  Tally A(56);
  std::string Line="f56";
  int out=A.addLine(Line);
  if (out)
    {
      ELog::EM<<"Error with addLine in addParticles "<<Line<<ELog::endTrace;
      ELog::EM<<"Out == "<<out<<ELog::endTrace;
      return -1;
    }
  
  for(const TTYPE& tc : Tests)
    {
      A.setParticles(std::get<0>(tc));
      std::string PP=A.getParticles();
      if (PP!=std::get<1>(tc))
	{
	  ELog::EM<<"Tally ="<<A<<ELog::endTrace;
	  ELog::EM<<"Particle == "<<PP<<ELog::endTrace;
	  return -2;
	}
    }

  return 0;
}

int
testTally::testFuCard()
  /*!
    Test the Fu card
    \return -ve on error
  */
{
  ELog::RegMethod RegA("testTally","testFuCard");

  typedef std::tuple<int,int,std::string> TTYPE;

  std::vector<TTYPE> Tests;
  
  Tests.push_back(TTYPE(3,3,"fu55 3 3 1 7i 9"));
  Tests.push_back(TTYPE(3,10,"fu55 3 10 1 28i 30"));
  Tests.push_back(TTYPE(-3,10,"fu55 -3 10"));

  for(const TTYPE& tc : Tests)
    {
      pointTally A(55);
      A.divideWindow(std::get<0>(tc),std::get<1>(tc));
      std::stringstream cx;
      cx<<A;
      std::string Line=StrFunc::getLine(cx);
      int testFlag(0);
      while(!Line.empty())
	{
	  if (Line.find("fu")!=std::string::npos &&
	      (Line==std::get<2>(tc)))
	    {
	      testFlag=1;
	    }
	  Line=StrFunc::getLine(cx);
	}
      if (!testFlag)
	{
	  ELog::EM<<"Failed on tally "
		  <<std::get<0>(tc)<<" "<<std::get<1>(tc)
		  <<ELog::endTrace;
	  ELog::EM<<"T=="<<cx.str()<<ELog::endDiag;
	  return -1;
	}
    }

  return 0;
}
