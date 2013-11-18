/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   test/testQuadratic.cxx
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
#include <list>
#include <vector>
#include <map>
#include <string>
#include <algorithm>

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
#include "Transform.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Cylinder.h"
#include "Sphere.h"

#include "testFunc.h"
#include "testQuadratic.h"

using namespace Geometry;

testQuadratic::testQuadratic() 
  /*!
    Constructor
  */
{}

testQuadratic::~testQuadratic() 
  /*!
    Destructor
  */
{}

int 
testQuadratic::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: index of test
    \retval -1 Distance failed
    \retval 0 All succeeded
  */
{
  ELog::RegMethod RegA("testQuadratic","applyTest");
  TestFunc::regSector("testQuadratic");

  int retValue(0);
  if (!extra)
    {
      std::cout<<"TestDistance            (1)"<<std::endl;
      std::cout<<"TestSet                 (2)"<<std::endl;
      return 0;
    }

  if (extra<0 || extra==1)
    {
      TestFunc::regTest("testDistance");
      retValue=testDistance();
      if (retValue || extra>0)
	return retValue;
    }

  if (extra<0 || extra==2)
    {
      TestFunc::regTest("testSet");
      retValue=testSet();
      if (retValue || extra>0)
	return retValue;
    }
 
  return retValue;
}

int
testQuadratic::testDistance()
  /*!
    Test the distance of a point from the cylinder
    \retval -1 :: failed build a cylinder
    \retval 0 :: All passed
  */
{
  ELog::RegMethod RegA("testQuadratic","testDistance");
  std::vector<std::string> CylStr;
  CylStr.push_back("1 cx 1");                // Cylinder origin
  CylStr.push_back("2 c/x 0.5 0.5 1.0");     // Cylinder along x axis 
  Geometry::Vec3D P(1.2,0.6,0.4);
  
  ELog::EM.lock();
  double results[]={ 1-sqrt(0.6*0.6+0.4*0.4),  1.0-sqrt(2*0.1*0.1) };

  std::vector<std::string>::const_iterator vc;
  Cylinder A;
  int cnt(0);
  for(vc=CylStr.begin();vc!=CylStr.end();vc++,cnt++)
    {
      const int retVal=A.setSurface(*vc);
      if (retVal)
        {
	  ELog::EM<<"Failed to build "<<*vc<<" Ecode == "<<retVal<<ELog::endErr;
	  ELog::EM.dispatch(1);
	  return -1;
	}
      ELog::EM<<"Cylinder == "<<A<<ELog::endDiag;

      ELog::EM<<"TestPoint == "<<P<<ELog::endDiag;
      double R=A.distance(P);
      ELog::EM<<"Distance [expect]== "<<R<<" [ "<<results[cnt]<<" ]"<<ELog::endDiag;
      ELog::EM<<"--------------"<<ELog::endDiag;
      if (fabs(results[cnt]-R)>1e-5)
        {
	  ELog::EM.error();
	  ELog::EM.dispatch(1);
	  return -(cnt+2);
	}
      ELog::EM.dispatch(0);
    }

  ELog::EM.unlock();
  return 0;
}

int
testQuadratic::testSet()
  /*!
    Test the setting of the cone
    \retval -1 :: failed build a cylinder
    \retval 0 :: All passed
  */
{
  ELog::RegMethod RegA("testQuadratic","testSet");
  Geometry::Vec3D Axis(0,0,-1);
  Geometry::Vec3D Point(1,2,18);
  
  Cylinder A(54,0);
  A.setCylinder(Point,Axis,4.0);
  ELog::EM<<"Cylinder == "<<A<<ELog::endDiag;

  return 0;
}
  
 
