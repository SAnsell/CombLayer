/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   test/testSurfExpand.cxx
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
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>
#include <complex>
#include <vector>
#include <set>
#include <map>
#include <vector>
#include <algorithm>
#include <functional>
#include <memory>
#include <tuple>

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
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "surfRegister.h"
#include "surfExpand.h"

#include "testFunc.h"
#include "testSurfExpand.h"

using namespace ModelSupport;

testSurfExpand::testSurfExpand()
  /*!
    Constructor
  */
{}

testSurfExpand::~testSurfExpand() 
  /*!
    Destructor
  */
{}

int 
testSurfExpand::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: Index of test
    \returns -ve on error 0 on success.
  */
{
  ELog::RegMethod RegA("testSurfExpand","applyTest");
  TestFunc::regSector("testSurfExpand");

  typedef int (testSurfExpand::*testPtr)();
  testPtr TPtr[]=
    {
      &testSurfExpand::testCylinder,
      &testSurfExpand::testPlane
    };

  const std::string TestName[]=
    {
      "Cylinder",
      "Plane"
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
testSurfExpand::testCylinder()
  /*!
    Test the cylinder expansion system
    \returns 0 on succes and -ve on failure
  */
{
  ELog::RegMethod RegA("testSurfExpand","testCylinder");
  typedef std::tuple<std::string,std::string,double> TTYPE;

  std::vector<TTYPE> Tests;
  Tests.push_back(TTYPE("c/x -3.0 -4.0 8.0",
			"c/x -3.0 -4.0 6.5",-1.5));
  
  return procSurface<Geometry::Cylinder>(Tests);
}

int
testSurfExpand::testPlane()
  /*!
    Test the plane expansion system
    \returns 0 on succes and -ve on failure
  */
{
  ELog::RegMethod RegA("testSurfExpand","testPlane");

  // Surface : Surface : 
  typedef std::tuple<std::string,std::string,double> TTYPE;

  std::vector<TTYPE> Tests;
  Tests.push_back(TTYPE("px -8.0","px -10.5",-2.5));
  Tests.push_back(TTYPE("pz 12.0","pz 22.0",10.0));
  Tests.push_back(TTYPE("p -1 0 0 1","p -1 0 0 11",10.0));
  
  std::vector<TTYPE>::const_iterator tc;
  int flag(0);

  for(const TTYPE& tc : Tests)
    {
      Geometry::Plane* PX=new Geometry::Plane(1,0);
      Geometry::Plane* AimX=new Geometry::Plane(2,0);
      PX->setSurface(std::get<0>(tc));
      AimX->setSurface(std::get<1>(tc));
      Geometry::Plane* OutX=dynamic_cast<Geometry::Plane*>
	(surfaceCreateExpand(PX,std::get<2>(tc)));

      if (!OutX || (*OutX!=*AimX))
	{
	  ELog::EM<<"Failed : mod == "<<std::get<2>(tc)<<ELog::endCrit;
	  ELog::EM<<"Original surface "<<*PX;
	  if (OutX)
	    ELog::EM<<"New surface      "<<*OutX;
	  else
	    ELog::EM<<"New surface      NULL"<<ELog::endCrit;
	  ELog::EM<<"Aim surface      "<<*AimX;
	  ELog::EM<<ELog::endCrit;
	  flag=-1;
	}
      delete PX;
      delete AimX;
      delete OutX;
      if (flag)
	return -1;
    }
  return 0;
}

template<typename SurfTYPE>
int
testSurfExpand::procSurface(const std::vector<TTYPE>& Tests) const
  /*!
    Test the general surface expand
    \tparam SurfTYPE :: Surface type
    \param Tests :: Test string to process
    \returns 0 on succes and -ve on failure
  */
{
  ELog::RegMethod RegA("testSurfExpand","procSurface<ST>");

  int flag(0);
  for(const TTYPE& tc : Tests)  
    {
      SurfTYPE* InitPtr=new SurfTYPE(1,0);
      SurfTYPE* AimPtr=new SurfTYPE(2,0);
      InitPtr->setSurface(std::get<0>(tc));
      AimPtr->setSurface(std::get<1>(tc));
      Geometry::Surface* expSPtr=
	surfaceCreateExpand(InitPtr,std::get<2>(tc));
      SurfTYPE* OutPtr=dynamic_cast<SurfTYPE*>(expSPtr);

      if (!OutPtr || (*OutPtr != *AimPtr))
	{
	  ELog::EM<<"Failed : mod == "<<std::get<2>(tc)<<ELog::endCrit;
	  ELog::EM<<"Original surface "<<*InitPtr;
	  if (OutPtr)
	    ELog::EM<<"New surface      "<<*OutPtr;
	  else
	    ELog::EM<<"New surface      NULL"<<ELog::endCrit;
	  ELog::EM<<"Aim surface      "<<*AimPtr;
	  ELog::EM<<ELog::endCrit;
	  flag = -1;
	}
      delete InitPtr;
      delete AimPtr;
      delete expSPtr;
      if (flag)
	return -1;
    }
  return 0;
}

///\cond TEMPLATE

template int 
testSurfExpand::procSurface<Geometry::Plane>
(const std::vector<TTYPE>&) const;

template int 
testSurfExpand::procSurface<Geometry::Cylinder>
(const std::vector<TTYPE>&) const;

///\endcond TEMPLATE
