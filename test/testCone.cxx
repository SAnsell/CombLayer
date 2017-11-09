/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   test/testCone.cxx
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
#include <sstream>
#include <cmath>
#include <list>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
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
#include "Transform.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Cone.h"

#include "testFunc.h"
#include "testCone.h"

using namespace Geometry;

testCone::testCone() 
  /*!
    Constructor
  */
{}

testCone::~testCone() 
  /*!
    Destructor
  */
{}

int 
testCone::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: index of test
    \retval -1 Distance failed
    \retval 0 All succeeded
  */
{
  ELog::RegMethod RegA("testCone","applyTest");
  TestFunc::regSector("testCone");
  typedef int (testCone::*testPtr)();
  testPtr TPtr[]=
    {
      &testCone::testDistance,
      &testCone::testSide,
      &testCone::testSideDirection,
      &testCone::testSurfaceNormal,
    };
  const std::string TestName[]=
    {
      "Distance",
      "Side",
      "SideDirection",
      "SurfaceNormal"
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
testCone::testSideDirection()
  /*!
    Test the sidedirection function [via surface]
    \retval -1 :: failed build a cone
    \retval 0 :: All passed
  */
{
  ELog::RegMethod RegA("testCone","testSideDirection");


  // Cylinder : Start Point : path-direction : Side
  typedef std::tuple<size_t,Geometry::Vec3D,Geometry::Vec3D,int> TTYPE;
  std::vector<Geometry::Cone> ConeUnits;
  std::vector<TTYPE> Tests;
  
  ConeUnits.push_back(Geometry::Cone(1,0));
  ConeUnits.back().setCone
    (Geometry::Vec3D(0,0,0),Geometry::Vec3D(0,1,0),45.0);
  // OutWard
  Tests.push_back(TTYPE(0,Geometry::Vec3D(0,1,1),Geometry::Vec3D(0,0,1),1));
  // Inward
  Tests.push_back(TTYPE(0,Geometry::Vec3D(0,1,1),Geometry::Vec3D(0,0,-1),-1));
  // Inward
  Tests.push_back(TTYPE(0,Geometry::Vec3D(0,0,1),Geometry::Vec3D(0,0,-1),-1));
  // Outward
  Tests.push_back(TTYPE(0,Geometry::Vec3D(0,0,-1),Geometry::Vec3D(0,0,-1),1));

  int cnt(1);
  for(const TTYPE& tc : Tests)
    {
      const Geometry::Cone& CX=ConeUnits[std::get<0>(tc)];
      const Geometry::Vec3D& Pt=std::get<1>(tc);
      if (CX.sideDirection(Pt,std::get<2>(tc))!=std::get<3>(tc))
	{
	  const Geometry::Vec3D SN=CX.surfaceNormal(Pt);
	  ELog::EM<<"Test == "<<cnt<<ELog::endDebug;
	  ELog::EM<<"Distance == "<<CX.distance(Pt)<<ELog::endDebug;
	  ELog::EM<<"Side direction "<<SN<<":"<<std::get<2>(tc)<<ELog::endDebug;
	  ELog::EM<<"DotProd "<<SN.dotProd(std::get<2>(tc))<<" : "
		  <<CX.sideDirection(Pt,std::get<2>(tc))
		  <<ELog::endDebug;
	}
      cnt++;
    }
  return 0;
}

int
testCone::testSurfaceNormal()
  /*!
    Test the distance of a point from the cone
    \retval -1 :: failed build a cone
    \retval 0 :: All passed
  */
{
  ELog::RegMethod RegA("testCone","testSurfaceNormal");

  // Cone : Start Point : Normal : NResults : distance A : distance B 
  typedef std::tuple<std::string,Geometry::Vec3D,Geometry::Vec3D>
		       TTYPE;
  std::vector<TTYPE> Tests;
  
  Tests.push_back(TTYPE("ky 1 1 1",Geometry::Vec3D(-1,2,0),
			Geometry::Vec3D(-1,-1,0).unit()));
  Tests.push_back(TTYPE("ky 1 1 -1",Geometry::Vec3D(-1,0,0),
			Geometry::Vec3D(-1,1,0).unit()));
  Tests.push_back(TTYPE("ky 1 1 -1",Geometry::Vec3D(1,0,0),
			Geometry::Vec3D(1,1,0).unit()));
  Tests.push_back(TTYPE("ky 1 1 1",Geometry::Vec3D(1,2,0),
			Geometry::Vec3D(1,-1,0).unit()));
  
  int cnt(1);
  for(const TTYPE& tc : Tests)
    {
      Cone A;
      const int retVal=A.setSurface(std::get<0>(tc));
      if (retVal)
        {
	  ELog::EM<<"Failed to build "<<std::get<0>(tc)
		  <<" Ecode == "<<retVal<<ELog::endErr;
	  return -1;
	}
      const Geometry::Vec3D RNorm=A.surfaceNormal(std::get<1>(tc));
      if (RNorm!=std::get<2>(tc))
	{
	  ELog::EM<<"Failure for test "<<cnt<<ELog::endCrit;
	  ELog::EM<<"Normal "<<RNorm<<" :: "<<
	    std::get<2>(tc)<<ELog::endCrit;
	  return -1;
	}
      cnt++;
    }
  return 0;
}


int
testCone::testSide()
  /*!
    Test the side of the point relative to the cone:
    \return -ve on error
   */
{
  ELog::RegMethod RegA("testCone","testSide");

  // Cone : Start Point : side 
  typedef std::tuple<std::string,Geometry::Vec3D,int> TTYPE;
  std::vector<TTYPE> Tests;
  
  Tests.push_back(TTYPE("ky 1 0.8 1",Geometry::Vec3D(0,3,0),-1));
  Tests.push_back(TTYPE("ky 1 0.7 1",Geometry::Vec3D(0,-3,0),1));
  
  int cnt(1);
  for(const TTYPE& tc : Tests)
    {
      Cone A;
      const int retVal=A.setSurface(std::get<0>(tc));
      if (retVal)
        {
	  ELog::EM<<"Failed to build "<<std::get<0>(tc)
		  <<" Ecode == "<<retVal<<ELog::endCrit;
	  return -1;
	}
      const int RSide=A.side(std::get<1>(tc));
      if (RSide!=std::get<2>(tc))
	{
	  ELog::EM<<"Failure for test "<<cnt<<ELog::endCrit;
	  ELog::EM<<"Side ["<<std::get<2>(tc)<<"] ::"
		  <<RSide<<ELog::endCrit;
	  ELog::EM<<"Point "<<std::get<1>(tc)<<ELog::endCrit;
	  return -1;
	}
      cnt++;
    }
  return 0;
}


int
testCone::testDistance()
  /*!
    Test the distance of a point from the cone
    \retval -1 :: failed build a cone
    \retval 0 :: All passed
  */
{
  ELog::RegMethod RegA("testCone","testDistance");
  Geometry::Cone A(1,0);
  if (A.setSurface("kx 0 1"))
    {
      ELog::EM<<"Cone not built"<<ELog::endErr;
      return -1;
    }

  Geometry::Vec3D P(-1,-1.2,0);
  if (fabs(A.distance(P)-sqrt(2.0)/10.0)>1e-5)
    {
      ELog::EM<<"Cone == "<<A<<ELog::endDiag;
      ELog::EM<<"Distance == "<<A.distance(P)<<ELog::endDiag;
      return -1;
    }
  return 0;
}
  
  

