/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   test/testHeadRule.cxx
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
#include <cmath>
#include <vector>
#include <set>
#include <map>
#include <stack>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <memory>
#include <boost/tuple/tuple.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "NameStack.h"
#include "MemStack.h"
#include "RegMethod.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "BnId.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Surface.h"
#include "Rules.h"
#include "RuleBinary.h"
#include "HeadRule.h"
#include "Object.h"
#include "surfIndex.h"
#include "mapIterator.h"

#include "testFunc.h"
#include "testHeadRule.h"


testHeadRule::testHeadRule() 
/*!
  Constructor
*/
{}

testHeadRule::~testHeadRule() 
 /*! 
   Destructor
 */
{}

void
testHeadRule::createSurfaces()
  /*!
    Create the surface list
   */
{
  ELog::RegMethod RegA("testObject","createSurfaces");
  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();
  SurI.reset();
  // First box :
  SurI.createSurface(1,"px -1");
  SurI.createSurface(2,"px 1");
  SurI.createSurface(3,"py -1");
  SurI.createSurface(4,"py 1");
  SurI.createSurface(5,"pz -1");
  SurI.createSurface(6,"pz 1");

  SurI.createSurface(11,"px -1");
  SurI.createSurface(12,"px 1");
  SurI.createSurface(13,"py -1");
  SurI.createSurface(14,"py 1");
  SurI.createSurface(15,"pz -1");
  SurI.createSurface(16,"pz 1");
  return;
}

int 
testHeadRule::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: Index of test to run
    \return 0 on success / -ve on error
  */
{
  ELog::RegMethod RegA("testHeadRule","applyTest");
  TestFunc::regSector("testHeadRule");

  ELog::MemStack::Instance().setVFlag(1);
  typedef int (testHeadRule::*testPtr)();
  testPtr TPtr[]=
    {
      &testHeadRule::testAddInterUnion,
      &testHeadRule::testCountLevel,
      &testHeadRule::testGetComponent,
      &testHeadRule::testLevel,
      &testHeadRule::testRemoveSurf
    };
  const std::string TestName[]=
    {
      "AddUnterUnion",
      "CountLevel",
      "GetComponent",
      "Level",
      "RemoveSurf"
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
testHeadRule::testAddInterUnion()
  /*!
    Check the validity of a head rule over intersection + union
    \return 0 :: success / -ve on error
   */
{
  ELog::RegMethod RegA("testHeadRule","testAddInterUnion");

  createSurfaces();

  typedef boost::tuple<std::string,std::string> TTYPE;
  std::vector<TTYPE> Tests;
  Tests.push_back(TTYPE("1 -2 ","1 -2"));
  Tests.push_back(TTYPE("3 -4 ","1 -2 3 4"));
  Tests.push_back(TTYPE("5 -6 ","1 -2 3 4"));
  Tests.push_back(TTYPE("5 -6 ","1 -2 3 4"));
  
  std::vector<TTYPE>::const_iterator tc;
  HeadRule A;
  HeadRule B;
		  
  for(tc=Tests.begin();tc!=Tests.end();tc++)
    {
      HeadRule tmp;
      if (!tmp.procString(tc->get<0>()))
	{
	  ELog::EM<<"Failed to set tmp :"<<tc->get<0>()<<ELog::endDebug;
	  return -1;
	}
      A.addIntersection(tmp.getTopRule());
      B.addUnion(tmp.getTopRule());
      ELog::EM<<"A == "<<A.display()<<ELog::endDebug;
    }
  return 0;
}

int
testHeadRule::testCountLevel()
  /*!
    Check the validity of a head rule level 
    \return 0 :: success / -ve on error
   */
{
  ELog::RegMethod RegA("testHeadRule","testCountLevel");

  createSurfaces();

  typedef boost::tuple<std::string,size_t,size_t> TTYPE;
  std::vector<TTYPE> Tests;
  Tests.push_back(TTYPE("1 -2 3 -4 (5:-6) ",1,2));
  Tests.push_back(TTYPE("1 -2 3 -4 (5:-6) ",0,5));
  Tests.push_back(TTYPE("1 -2 3 -4 (5:-6:(7 -8)) ",2,2));
  Tests.push_back(TTYPE("1 -2 3 -4 (5:-6:(7 -8)) ",2,2));
  //  Tests.push_back(TTYPE("1 -2 3 -4 (5:-6) ",4,0));
  //  Tests.push_back(TTYPE("1 -2 3 -4 (5:-6) ",7,-1));


  
  std::vector<TTYPE>::const_iterator tc;
  for(TTYPE& tc : Tests)
    {
      HeadRule tmp;
      if (!tmp.procString(tc.get<0>()))
	{
	  ELog::EM<<"Failed to set tmp :"<<tc.get<0>()
		  <<ELog::endDebug;
	  return -1;
	}
      const size_t Res=tmp.countNLevel(tc.get<1>());
      if (Res!=tc.get<2>())
	{
	  ELog::EM<<"A == "<<tmp.display()<<ELog::endDebug;
	  ELog::EM<<"Res["<<tc.get<2>()<<"] == "<<Res<<ELog::endDebug;
	  return -1;
	}
    }
  return 0;
}

int
testHeadRule::testGetComponent()
  /*!
    Check the validity of a head rule level 
    \return 0 :: success / -ve on error
   */
{
  ELog::RegMethod RegA("testHeadRule","testLevel");

  createSurfaces();

  typedef boost::tuple<std::string,size_t,size_t,std::string> TTYPE;
  std::vector<TTYPE> Tests;
  Tests.push_back(TTYPE("1 -2 3 -4 (5:-6) ",0,5,"(5 : -6)"));
  Tests.push_back(TTYPE("1 -2 3 -4 (5:-6) ",0,4,"-4"));
  Tests.push_back(TTYPE("(-6 : 7 : 35 ) -208 207 "
			"-206 -205 204 -203 202 201 ( -16 : -97 : 55 ) ",
			0,1,"(-6 : 7 : 35)"));
  Tests.push_back(TTYPE("(-6 : 7 : 35 ) -208 207 "
			"-206 -205 204 -203 202 201 ( -16 : -97 : 55 ) ",
			0,2,"-208"));
  std::vector<TTYPE>::const_iterator tc;
  for(TTYPE& tc : Tests)
    {
      HeadRule tmp;
      if (!tmp.procString(tc.get<0>()))
	{
	  ELog::EM<<"Failed to set tmp :"<<tc.get<0>()
		  <<ELog::endDebug;
	  return -1;
	}
      HeadRule ARes=tmp.getComponent(tc.get<1>(),tc.get<2>());
      const std::string AStr=ARes.display();
      if (AStr!=tc.get<3>())
	{
	  ELog::EM<<"A == "<<ARes.display()<<ELog::endDebug;
	  return -1;
	}
    }
  return 0;
}

int
testHeadRule::testLevel()
  /*!
    Check the validity of a head rule level 
    \return 0 :: success / -ve on error
   */
{
  ELog::RegMethod RegA("testHeadRule","testLevel");

  createSurfaces();

  typedef boost::tuple<std::string,int,int> TTYPE;
  std::vector<TTYPE> Tests;
  Tests.push_back(TTYPE("1 -2 3 -4 (5:-6) ",5,1));
  Tests.push_back(TTYPE("1 -2 3 -4 (5:-6) ",4,0));
  Tests.push_back(TTYPE("1 -2 3 -4 (5:-6) ",7,-1));
  Tests.push_back(TTYPE("1 -2 3 -4 (5:-6:(7 -8)) ",8,2));
  
  std::vector<TTYPE>::const_iterator tc;
  HeadRule A;
  for(TTYPE& tc : Tests)
    {
      HeadRule tmp;
      if (!tmp.procString(tc.get<0>()))
	{
	  ELog::EM<<"Failed to set tmp :"<<tc.get<0>()
		  <<ELog::endDebug;
	  return -1;
	}
      const int Res=tmp.level(tc.get<1>());
      if (Res!=tc.get<2>())
	{
	  ELog::EM<<"A == "<<A.display()<<ELog::endDebug;
	  ELog::EM<<"Res["<<tc.get<2>()<<"] == "<<Res<<ELog::endDebug;
	  return -1;
	}
    }
  return 0;
}

int
testHeadRule::testRemoveSurf()
  /*!
    Check the validity of a removal of a surface(s)
    \return 0 :: success / -ve on error
   */
{
  ELog::RegMethod RegA("testHeadRule","testAddInterUnion");

  createSurfaces();

  typedef boost::tuple<std::string,std::string> TTYPE;
  std::vector<TTYPE> Tests;
  Tests.push_back(TTYPE("1 -2 ","1 -2"));
  Tests.push_back(TTYPE("3 -4 ","1 -2 3 4"));
  Tests.push_back(TTYPE("5 -6 ","1 -2 3 4"));
  Tests.push_back(TTYPE("5 -6 ","1 -2 3 4"));
  
  std::vector<TTYPE>::const_iterator tc;
  HeadRule A;
  HeadRule B;
		  
  for(tc=Tests.begin();tc!=Tests.end();tc++)
    {
      HeadRule tmp;
      if (!tmp.procString(tc->get<0>()))
	{
	  ELog::EM<<"Failed to set tmp :"<<tc->get<0>()<<ELog::endDebug;
	  return -1;
	}
      A.addIntersection(tmp.getTopRule());
      B.addUnion(tmp.getTopRule());
      ELog::EM<<"A == "<<A.display()<<ELog::endDebug;
    }
  return 0;
}



