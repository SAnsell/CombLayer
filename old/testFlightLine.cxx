/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   old/testFlightLine.cxx
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
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/construct.hpp>
#include <boost/format.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "Element.h"
#include "Material.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Tensor.h"
#include "Vec3D.h"
#include "Triple.h"
#include "NRange.h"
#include "NList.h"
#include "Tally.h"
#include "Transform.h"
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfDivide.h"
#include "surfRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "FlightLine.h"

#include "testFunc.h"
#include "testFlightLine.h"

using namespace LensSystem;

testFlightLine::testFlightLine()
  /*!
    Constructor
  */
{}

testFlightLine::~testFlightLine() 
  /*!
    Destructor
  */
{}

int 
testFlightLine::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: Index of test
    \returns -ve on error 0 on success.
  */
{
  typedef int (testFlightLine::*testPtr)();
  testPtr TPtr[]=
    {
      &testFlightLine::testCreateCell
    };

  const std::string TestName[]=
    {
      "createCell"
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
	  TestFunc::bracketTest(TestName[i],std::cout);
	  const int retValue= (this->*TPtr[i])();
	  if (retValue || extra>0)
	    return retValue;
	}
    }
  return 0;
}

int
testFlightLine::testCreateCell()
  /*!
    Test the cylinder expansion system
    \returns 0 on succes and -ve on failure
  */
{
  ELog::RegMethod RegA("testFlightLine","testCreateCell");

  FlightLine A(
   Geometry::Vec3D(cos(-15.0*M_PI/180.0),sin(-15.0*M_PI/180.0),0.0),
   Geometry::Vec3D(0,0,0),25.0,20.0,20.0);
  FlightLine B(
   Geometry::Vec3D(1,0,0.0),
   Geometry::Vec3D(0,0,0),25.0,20.0,30.0);
  FlightLine C(
   Geometry::Vec3D(cos(15.0*M_PI/180.0),sin(15.0*M_PI/180.0),0.0),
   Geometry::Vec3D(0,0,0),25.0,20.0,20.0);
  
  A.generateSurfaces(10000);
  B.generateSurfaces(20000);
  C.generateSurfaces(30000);
  
  std::cout<<"B == "<<B.createCell(&A,&C)<<std::endl;
  
  return 0;
}




