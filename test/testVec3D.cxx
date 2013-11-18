/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   test/testVec3D.cxx
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

#include "testFunc.h"
#include "testVec3D.h"

using namespace Geometry;

testVec3D::testVec3D() 
  /*!
    Constructor
  */
{}

testVec3D::~testVec3D() 
  /*!
    Destructor
  */
{}

int 
testVec3D::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: parameter to decide test
    \retval -1 :: Fail on angle
  */
{
  int retValue(0);
  if (!extra)
    {
      std::cout<<"TestDotProd            (1)"<<std::endl;
      return 0;
    }

  if (extra<0 || extra==1)
    {
      TestFunc::regTest("testDotProd");
      retValue+=testDotProd();
      if (retValue || extra>0)
	return retValue;
    }
 
  return retValue;
}

int
testVec3D::testDotProd()
  /*!
    Tests the Vec3D dot prod
    \retval -1 on failure
    \retval 0 :: success 
  */
{
  ELog::RegMethod RegA("testVec3D","testDotProd");

  typedef boost::tuple<size_t,size_t,double> TTYPE;
  std::vector<TTYPE> Tests;

  // Set to test between:
  std::vector<Vec3D> VSet;
  VSet.push_back(Vec3D(1,2,0));  
  VSet.push_back(Vec3D(0,0,-1)); 
  VSet.push_back(Vec3D(1,0,0)); 
  VSet.push_back(Vec3D(0,1,0)); 
  VSet.push_back(Vec3D(-1,0,0)); 
  VSet.push_back(Vec3D(0,-1,0)); 
  VSet.push_back(Vec3D(1,-3,-4)); 
  VSet.push_back(Vec3D(7,2,-1)); 
  
  Tests.push_back(TTYPE(0,1,0.0));
  Tests.push_back(TTYPE(0,2,1.0));
  Tests.push_back(TTYPE(0,3,2.0));  
  Tests.push_back(TTYPE(4,0,-1.0));
  Tests.push_back(TTYPE(4,5,0.0));
  Tests.push_back(TTYPE(0,0,5.0));   // test self and two comps
  Tests.push_back(TTYPE(6,7,5.0));   // test self and two comps

  std::vector<TTYPE>::const_iterator tc;
  for(tc=Tests.begin();tc!=Tests.end();tc++)
    {
      const size_t iA(tc->get<0>());
      const size_t iB(tc->get<1>());
      const double dp=VSet[iA].dotProd(VSet[iB]);
      if (fabs(dp-tc->get<2>())>1e-5)
	{
	  ELog::EM<<"Test Num   "<<(tc-Tests.begin())+1<<ELog::endTrace;
	  ELog::EM<<"Vectors == "<<VSet[iA]<<ELog::endTrace;
	  ELog::EM<<"        == "<<VSet[iB]<<ELog::endTrace;
	  ELog::EM<<"Dot prod = "<<dp<<" ("<<tc->get<2>()<<")"<<ELog::endTrace;
	  return -1;
	}
    }
  return 0;
}

  
