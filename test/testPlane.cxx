/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   test/testPlane.cxx
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#include "Quaternion.h"
#include "Transform.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Plane.h"

#include "testFunc.h"
#include "testPlane.h"

using namespace Geometry;

testPlane::testPlane() 
  /*!
    Constructor
  */
{}

testPlane::~testPlane() 
  /*!
    Destructor
  */
{}

int 
testPlane::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: index of test
    \retval -1 Distance failed
    \retval 0 All succeeded
  */
{
  ELog::RegMethod RegA("testPlane","applyTest");
  TestFunc::regSector("testPlane");

  typedef int (testPlane::*testPtr)();
  testPtr TPtr[]=
    {
      &testPlane::testDistance,
      &testPlane::testQuaternion,
      &testPlane::testMirror,
      &testPlane::testMirrorAxis,
      &testPlane::testSet,
      &testPlane::testTransform
    };
  const std::string TestName[]=
    {
      "Distance",
      "Quaternion",
      "Mirror",
      "MirrorAxis",
      "Set",
      "Transform"
    };
  const int TSize(sizeof(TPtr)/sizeof(testPtr));
  if (!extra)
    {
      TestFunc::Instance().writeList(std::cout,TSize,TestName);
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
testPlane::testDistance()
  /*!
    Test the distance of a point from the plane
    \retval -1 :: failed build a plane
    \retval 0 :: All passed
  */
{
  ELog::RegMethod RegA("testPlane","testDistance");

  typedef std::tuple<std::string,Geometry::Vec3D,double> TTYPE;
  std::vector<TTYPE> Tests;
  
  Tests.push_back(TTYPE("px 1",Geometry::Vec3D(1.2,0.6,0.4),0.2));
  Tests.push_back(TTYPE("p 0.0 0.5 0.5 1.0",
			Geometry::Vec3D(7,1,1),0.0));
  Tests.push_back(TTYPE("p 0.0 0.5 0.5 1.0",
			Geometry::Vec3D(7,2,0),0.0));
  // Note the negative since coming from the reverse of the 
  Tests.push_back(TTYPE("p 0.0 0.5 0.5 1.0",              
			Geometry::Vec3D(1.2,0.8,0.4),
			-sqrt(2*0.16)));
		  
  
  for(const TTYPE& tc : Tests)
    {
      Plane A;
      const int retVal=A.setSurface(std::get<0>(tc));
      if (retVal)
        {
	  ELog::EM<<"Failed to build "<<std::get<0>(tc)
		  <<" Ecode == "<<retVal<<ELog::endErr;
	  return -1;
	}
      const double R=A.distance(std::get<1>(tc));
      if (fabs(R-std::get<2>(tc))>1e-5)
	{
	  ELog::EM<<"Plane == "<<A<<ELog::endTrace;
	  ELog::EM<<"TestPoint == "<<std::get<1>(tc)<<ELog::endTrace;
	  ELog::EM<<"Distance [expect]== "<<R
		  <<" [ "<<std::get<2>(tc)<<" ]"<<ELog::endTrace;
	  return -2;
	}
    }

  return 0;
}

int
testPlane::testMirror()
  /*!
    Test of the mirror plane
    \retval -1 :: failed build a plane
    \retval 0 :: All passed
  */
{
  ELog::RegMethod RegA("testPlane","testMirror");

  typedef std::tuple<std::string,Geometry::Vec3D,
		       Geometry::Vec3D> TTYPE;
  std::vector<TTYPE> Tests;
  
  Tests.push_back(TTYPE("px 1",Geometry::Vec3D(0,0,0),
			Geometry::Vec3D(2,0,0)));

  Tests.push_back(TTYPE("px 1",Geometry::Vec3D(3,1,1),
			Geometry::Vec3D(-1,1,1)));

  

  for(const TTYPE& tc : Tests)
    {
      Plane A;
      A.setSurface(std::get<0>(tc));
      
      Geometry::Vec3D OutA=std::get<1>(tc);
      Geometry::Vec3D OutB=std::get<2>(tc);
      A.mirrorPt(OutA);
      A.mirrorPt(OutB);
      if (OutA!=std::get<2>(tc) || OutB!=std::get<1>(tc))
        {
	  ELog::EM<<"Plane ==> "<<A<<ELog::endTrace;
	  ELog::EM<<"Points == "<<std::get<1>(tc)<<" mirror to: "
		  <<OutA<<"( "<<std::get<2>(tc)<<" )"<<ELog::endTrace;
	  ELog::EM<<"Points == "<<std::get<2>(tc)<<" mirror to: "
		  <<OutB<<"( "<<std::get<1>(tc)<<" )"<<ELog::endTrace;
	  return -1;
	}
    } 
  
  return 0;
}

int
testPlane::testMirrorAxis()
  /*!
    Test of the mirror plane for an axis
    \retval -1 :: failed build a plane
    \retval 0 :: All passed
  */
{
  ELog::RegMethod RegA("testPlane","testMirrorAxis");

  // Test Points
  std::vector<Geometry::Vec3D> Pt;
  std::vector<Geometry::Vec3D> PtX;
  
  
  Plane A(78,2);
  A.setSurface("px 1");

  Pt.push_back(Geometry::Vec3D(0,1,1));
  PtX.push_back(Geometry::Vec3D(0,1,1));

  Pt.push_back(Geometry::Vec3D(1,0,0));
  PtX.push_back(Geometry::Vec3D(-1,0,0));

  Pt.push_back(Geometry::Vec3D(1,1,1));
  PtX.push_back(Geometry::Vec3D(-1,1,1));

  for(size_t i=0;i<Pt.size();i++)
    {
      Geometry::Vec3D Out=Pt[i];
      A.mirrorAxis(Out);
      if (Out!=PtX[i])
        {
	  ELog::EM<<"Failed on point "<<i+1<<ELog::endErr;
	  ELog::EM<<"  Plane ==> "<<A;
	  ELog::EM<<"  Points == "<<Pt[i]<<" mirror to: "
		  <<Out<<"( "<<PtX[i]<<" )"<<ELog::endErr;
	  return -1;
	}
    }
  
  return 0;
}

int
testPlane::testSet()
  /*!
    Test the setting of the plane
    \retval -1 :: failed build a plane
    \retval 0 :: All passed
  */
{
  ELog::RegMethod RegA("testPlane","testSet");

  // number of vectors : PtA:PtB:PtC: Expected string
  typedef std::tuple<int,Vec3D,Vec3D,Vec3D,std::string> TTYPE;
  std::vector<TTYPE> Tests;
  
  Tests.push_back(TTYPE(2,Vec3D(1,2,18),Vec3D(0,0,-1),
		       Vec3D(0,0,0),"54 p 0.0 0.0 -1 -18"));
  Tests.push_back(TTYPE(3,Vec3D(10,0,0),Vec3D(10,0,1),
			Vec3D(10,5,1),"54 px 10"));

  int cnt(1);
  for(const TTYPE& tc : Tests)
    {
      Plane A(54,0);
      if (std::get<0>(tc)==2)
	A.setPlane(std::get<1>(tc),std::get<2>(tc));
      else
	A.setPlane(std::get<1>(tc),std::get<2>(tc),std::get<3>(tc));
      std::ostringstream cx;
      cx<<A;
      if (StrFunc::fullBlock(cx.str())!=std::get<4>(tc))
	{
	  ELog::EM<<"Failed on test: "<<cnt<<ELog::endCrit;
	  ELog::EM<<"Plane  == "<<A;
	  ELog::EM<<"Expect == "<<std::get<4>(tc)<<ELog::endCrit;
	  return -1;
	}
      cnt++;
    }
  return 0;
}

int
testPlane::testTransform()
  /*!
    Test the transform of the plane
    \retval -1 :: failed build a plane
    \retval 0 :: All passed
  */
{
  ELog::RegMethod RegA("testPlane","testTransform");

  typedef std::tuple<std::string,std::string,std::string> TTYPE;
  std::vector<TTYPE> Tests;
  
  Tests.push_back(TTYPE("px 1","tr2 1 0 1 "
			"1 0 0 0 1 0 0 0 1 ",
			"54 px 2"));
	
  for(const TTYPE& tc : Tests)
    {
      Plane A(54,2);
      A.setSurface(std::get<0>(tc));
      Transform X;
      X.setTransform(std::get<1>(tc));
      std::map<int,Geometry::Transform> TMap;
      TMap.insert(std::pair<int,Transform>(2,X));
      A.applyTransform(TMap);
      std::ostringstream cx;
      cx<<A;
      if (StrFunc::fullBlock(cx.str())!=std::get<2>(tc))
	{
	  ELog::EM<<"A == "<<A;
	  ELog::EM<<"Expected "<<std::get<2>(tc)<<ELog::endTrace;
	  return -1;
	}
    }
  return 0;
}
   
int
testPlane::testQuaternion()
  /*!
    Test the quaternion rotation
    \return 0 on success
   */
{
  ELog::RegMethod RegA("testPlane","testQuaternion");

  typedef std::tuple<std::string,Geometry::Vec3D,
		       Quaternion,int> TTYPE;

  std::vector<TTYPE> Tests;

  Tests.push_back(TTYPE("px 1",Geometry::Vec3D(3.4,5.2,3.3),
			Quaternion::calcQRotDeg(90.0,Geometry::Vec3D(0,1,0)),
			-1));
  Tests.push_back(TTYPE("pz 1",Geometry::Vec3D(3.4,5.2,3.3),
			Quaternion::calcQRotDeg(90.0,Geometry::Vec3D(0,1,0)),
			1));
  
  for(const TTYPE& tc : Tests)
    {
      Plane A;
      A.setSurface(std::get<0>(tc));
      A.rotate(std::get<2>(tc));
      if (A.side(std::get<1>(tc))!=std::get<3>(tc) ||
	  A.Quadratic::side(std::get<1>(tc))!=std::get<3>(tc))
	{
	  ELog::EM<<"Org == "<<std::get<0>(tc)<<ELog::endTrace;
	  ELog::EM<<"A == "<<A<<ELog::endTrace;
	  
	  ELog::EM<<"Side == "<<A.side(std::get<1>(tc))
		  <<" != "<<std::get<3>(tc)<<ELog::endTrace;

	  ELog::EM<<"Quadratic Side == "<<A.Quadratic::side(std::get<1>(tc))
		  <<" != "<<std::get<3>(tc)<<ELog::endTrace;
	  return -1;
	}
    }
  return 0;
}
