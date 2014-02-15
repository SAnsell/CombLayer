/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   test/testContained.cxx
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
#include <list>
#include <deque>
#include <set>
#include <map>
#include <stack>
#include <string>
#include <sstream>
#include <algorithm>
#include <boost/tuple/tuple.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Surface.h"
#include "Rules.h"
#include "BnId.h"
#include "Acomp.h"
#include "Algebra.h"
#include "surfIndex.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"

#include "Debug.h"

#include "testFunc.h"
#include "testContained.h"

using namespace attachSystem;

testContained::testContained() 
  /*!
    Constructor
  */
{
  createSurfaces();
}

testContained::~testContained() 
  /*!
    Destructor
  */
{
}

void 
testContained::createSurfaces()
  /*!
    Create the surface list
   */
{
  ELog::RegMethod RegA("testContained","createSurfaces");

  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();

  // First box :
  SurI.createSurface(1,"px -1");
  SurI.createSurface(2,"px 1");
  SurI.createSurface(3,"py -1");
  SurI.createSurface(4,"py 1");
  SurI.createSurface(5,"pz -1");
  SurI.createSurface(6,"pz 1");

  // Second box :
  SurI.createSurface(11,"px -3");
  SurI.createSurface(12,"px 3");
  SurI.createSurface(13,"py -3");
  SurI.createSurface(14,"py 3");
  SurI.createSurface(15,"pz -3");
  SurI.createSurface(16,"pz 3");

  // Far box :
  SurI.createSurface(21,"px 10");
  SurI.createSurface(22,"px 15");

  // Sphere :
  SurI.createSurface(100,"so 25");


  // Sphere :
  SurI.createSurface(107,"cy 10");

  
  return;
}

int 
testContained::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: Test number to run
    \retval -1 : SetObject 
    \retval 0 : All succeeded
  */
{
  ELog::RegMethod RegA("testContained","applyTest");
  TestFunc::regSector("testContained");

  typedef int (testContained::*testPtr)();
  testPtr TPtr[]=
    {
      &testContained::testAddSurfString,
      &testContained::testIsOuterLine
    };
  const std::string TestName[]=
    {
      "AddSurfString",
      "IsOuterLine"
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
testContained::testAddSurfString()
  /*!
    Test the CellSTr method including the 
    complementary of both \#(XXX) and \#Cell.
    \retval 0 :: success
  */
{
  ELog::RegMethod RegA("testContained","testAddSurfString");

  typedef boost::tuple<std::string,std::string> TTYPE;
  std::vector<TTYPE> Tests;
  Tests.push_back(TTYPE("1 -2 3 -4","( -1 : -3 : 4 : 2 )"));
  

  
  ContainedComp C;
  std::vector<TTYPE>::const_iterator tc;
  for(tc=Tests.begin();tc!=Tests.end();tc++)
    {
      C.addOuterUnionSurf(tc->get<0>());
      if (StrFunc::fullBlock(C.getExclude())!=tc->get<1>())
	{
	  ELog::EM<<"Exclude == "<<C.getExclude()<<ELog::endTrace;
	  ELog::EM<<"Expect  == "<<tc->get<1>()<<ELog::endTrace;
	  return -1;
	}
    }
  return 0;
}

int
testContained::testIsOuterLine()
  /*!
    Test the line tracking through a cell .
    Determine if a line tracks the out cell component
    \retval 0 :: success / -ve on failure
  */
{
  ELog::RegMethod RegA("testContained","testAddSurfString");

  typedef boost::tuple<std::string,Geometry::Vec3D,Geometry::Vec3D,bool> TTYPE;
  std::vector<TTYPE> Tests;
  Tests.push_back(TTYPE("1 -2 3 -4 5 -6",
			Geometry::Vec3D(0,0,0),Geometry::Vec3D(1,0,0),1));

  Tests.push_back(TTYPE("1 -2 3 -4 5 -6",
			Geometry::Vec3D(-2,0,0),Geometry::Vec3D(-1.2,0,0),0));

  Tests.push_back(TTYPE("3 -4 -107",
			Geometry::Vec3D(-2,0,0),Geometry::Vec3D(-1.2,0,0),1));

  Tests.push_back(TTYPE("3 -4 -107",
			Geometry::Vec3D(-0,-2,0),Geometry::Vec3D(0,-1.2,0),0));

  Tests.push_back(TTYPE("3 -4 -107",
			Geometry::Vec3D(-1,0,0),Geometry::Vec3D(1,4,0),1));

  Tests.push_back(TTYPE("3 -4 -107",
			Geometry::Vec3D(1,4,0),Geometry::Vec3D(-1,0,0),1));
  

  std::vector<TTYPE>::const_iterator tc;
  for(tc=Tests.begin();tc!=Tests.end();tc++)
    {
      ContainedComp C;
      C.addOuterSurf(tc->get<0>());
      bool Res=C.isOuterLine(tc->get<1>(),tc->get<2>());
      if (Res!=tc->get<3>())
	{
	  ELog::EM<<"Surface  == "<<tc->get<0>()<<ELog::endTrace;
	  ELog::EM<<"Line == "<<tc->get<1>()<<" :: "
		  <<tc->get<2>()<<ELog::endTrace;
	  ELog::EM<<"Expect  == "<<tc->get<3>()<<ELog::endTrace;
	  return -1;
	}
    }
  return 0;
}



