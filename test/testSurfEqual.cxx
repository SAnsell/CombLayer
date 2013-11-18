/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   test/testSurfEqual.cxx
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
#include <cmath>
#include <complex>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <stack>
#include <string>
#include <algorithm>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/array.hpp>
#include <boost/format.hpp>
#include <boost/tuple/tuple.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Triple.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "Quadratic.h"
#include "NullSurface.h"
#include "Sphere.h"
#include "Plane.h"
#include "surfaceFactory.h"
#include "surfEqual.h"

#include "testFunc.h"
#include "testSurfEqual.h"

using namespace ModelSupport;

testSurfEqual::testSurfEqual()
  /*!
    Constructor
  */
{
  createSurfaces();
}

testSurfEqual::~testSurfEqual() 
  /*!
    Destructor
  */
{}


void 
testSurfEqual::createSurfaces()
  /*!
    Create the surface list
   */
{
  ELog::RegMethod RegA("testSurfEqual","createSurfaces");

  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();
  
  // First box :
  SurI.createSurface(1,"px -1");
  SurI.createSurface(2,"px 1");
  SurI.createSurface(3,"py -1");
  SurI.createSurface(4,"py 1");
  SurI.createSurface(5,"pz -1");
  SurI.createSurface(6,"pz 1");
  SurI.createSurface(11,"p -1 0 0 1");
  
  return;
}


int 
testSurfEqual::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: Index of test
    \returns -ve on error 0 on success.
  */
{
  typedef int (testSurfEqual::*testPtr)();
  testPtr TPtr[]=
    {
      &testSurfEqual::testBasicPair,
      &testSurfEqual::testEqualSurfNum
    };

  const std::string TestName[]=
    {
      "BasicPair",
      "EqualSurfNum"
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
testSurfEqual::testBasicPair()
  /*!
    Test a simple pair divide between two surfaces.
    \return -ve on error 
  */
{
  ELog::RegMethod RegA("testSurfEqual","testBasicPair");
  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();
  
  typedef boost::tuple<int,int,int> TTYPE;
  std::vector<TTYPE> Tests;
  Tests.push_back(TTYPE(1,11,1));

  std::vector<TTYPE>::const_iterator tc;
  int flag(0);
  for(tc=Tests.begin();tc!=Tests.end() && !flag;tc++)
    {
      const Geometry::Surface* SA=SurI.getSurf(tc->get<0>());
      const Geometry::Surface* SB=SurI.getSurf(tc->get<1>());
      if (ModelSupport::oppositeSurfaces(SA,SB)!=tc->get<2>())
	{
	  ELog::EM<<"Failed :  "<<tc->get<0>()<<" "
		  <<tc->get<1>()<<ELog::endCrit;
	  ELog::EM<<"Surface :  "<<*SA<<ELog::endCrit;
	  ELog::EM<<"Surface :  "<<*SB<<ELog::endCrit;
	  ELog::EM<<"Opppite :  "<<ModelSupport::oppositeSurfaces(SA,SB)
		  <<ELog::endCrit;
	  flag=-1;
	}
    }
  return flag;
}

int
testSurfEqual::testEqualSurfNum()
  /*!
    Test a simple pair divide between two surfaces.
    \return -ve on error 
  */
{
  ELog::RegMethod RegA("testSurfEqual","testEqualSurfNum");

  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();
  
  typedef boost::tuple<int,int> TTYPE;
  std::vector<TTYPE> Tests;
  Tests.push_back(TTYPE(1,1));
  Tests.push_back(TTYPE(11,11));

  std::vector<TTYPE>::const_iterator tc;
  int flag(0);
  for(tc=Tests.begin();tc!=Tests.end() && !flag;tc++)
    {
      const Geometry::Surface* SA=SurI.getSurf(tc->get<0>());
      if (ModelSupport::equalSurfNum(SA)!=tc->get<1>())
	{
	  ELog::EM<<"Failed :  "<<tc->get<0>()<<" "
		  <<tc->get<1>()<<ELog::endCrit;
	  ELog::EM<<"Surface :  "<<*SA<<ELog::endCrit;
	  flag=-1;
	}
    }
  return flag;
}

