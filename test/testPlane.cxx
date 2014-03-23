/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   test/testPlane.cxx
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
#include <sstream>
#include <cmath>
#include <list>
#include <vector>
#include <map>
#include <string>
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
testPlane::testDistance()
  /*!
    Test the distance of a point from the plane
    \retval -1 :: failed build a plane
    \retval 0 :: All passed
  */
{
  ELog::RegMethod RegA("testPlane","testDistance");

  typedef boost::tuple<std::string,Geometry::Vec3D,double> TTYPE;
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
		  
  
  std::vector<TTYPE>::const_iterator tc;
  for(tc=Tests.begin();tc!=Tests.end();tc++)
    {
      Plane A;
      const int retVal=A.setSurface(tc->get<0>());
      if (retVal)
        {
	  ELog::EM<<"Failed to build "<<tc->get<0>()
		  <<" Ecode == "<<retVal<<ELog::endErr;
	  return -1;
	}
      const double R=A.distance(tc->get<1>());
      if (fabs(R-tc->get<2>())>1e-5)
	{
	  ELog::EM<<"Plane == "<<A<<ELog::endTrace;
	  ELog::EM<<"TestPoint == "<<tc->get<1>()<<ELog::endTrace;
	  ELog::EM<<"Distance [expect]== "<<R
		  <<" [ "<<tc->get<2>()<<" ]"<<ELog::endTrace;
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

  typedef boost::tuple<std::string,Geometry::Vec3D,
		       Geometry::Vec3D> TTYPE;
  std::vector<TTYPE> Tests;
  
  Tests.push_back(TTYPE("px 1",Geometry::Vec3D(0,0,0),
			Geometry::Vec3D(2,0,0)));

  Tests.push_back(TTYPE("px 1",Geometry::Vec3D(3,1,1),
			Geometry::Vec3D(-1,1,1)));

  

  std::vector<TTYPE>::const_iterator tc;
  for(tc=Tests.begin();tc!=Tests.end();tc++)
    {
      Plane A;
      A.setSurface(tc->get<0>());
      
      Geometry::Vec3D OutA=tc->get<1>();
      Geometry::Vec3D OutB=tc->get<2>();
      A.mirrorPt(OutA);
      A.mirrorPt(OutB);
      if (OutA!=tc->get<2>() || OutB!=tc->get<1>())
        {
	  ELog::EM<<"Plane ==> "<<A<<ELog::endTrace;
	  ELog::EM<<"Points == "<<tc->get<1>()<<" mirror to: "
		  <<OutA<<"( "<<tc->get<2>()<<" )"<<ELog::endTrace;
	  ELog::EM<<"Points == "<<tc->get<2>()<<" mirror to: "
		  <<OutB<<"( "<<tc->get<1>()<<" )"<<ELog::endTrace;
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
  typedef boost::tuple<int,Vec3D,Vec3D,Vec3D,std::string> TTYPE;
  std::vector<TTYPE> Tests;
  
  Tests.push_back(TTYPE(2,Vec3D(1,2,18),Vec3D(0,0,-1),
		       Vec3D(0,0,0),"54 p 0.0 0.0 -1 -18"));
  Tests.push_back(TTYPE(3,Vec3D(10,0,0),Vec3D(10,0,1),
			Vec3D(10,5,1),"54 px 10"));

  std::vector<TTYPE>::const_iterator tc;
  for(tc=Tests.begin();tc!=Tests.end();tc++)
    {
      Plane A(54,0);
      if (tc->get<0>()==2)
	A.setPlane(tc->get<1>(),tc->get<2>());
      else
	A.setPlane(tc->get<1>(),tc->get<2>(),tc->get<3>());
      std::ostringstream cx;
      cx<<A;
      if (StrFunc::fullBlock(cx.str())!=tc->get<4>())
	{
	  ELog::EM<<"Failed on test: "<<(tc-Tests.begin())+1<<ELog::endCrit;
	  ELog::EM<<"Plane  == "<<A;
	  ELog::EM<<"Expect == "<<tc->get<4>()<<ELog::endCrit;
	  return -1;
	}
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

  typedef boost::tuple<std::string,std::string,std::string> TTYPE;
  std::vector<TTYPE> Tests;
  
  Tests.push_back(TTYPE("px 1","tr2 1 0 1 "
			"1 0 0 0 1 0 0 0 1 ",
			"54 px 2"));
	
  std::vector<TTYPE>::const_iterator tc;
  for(tc=Tests.begin();tc!=Tests.end();tc++)
    {
      Plane A(54,2);
      A.setSurface(tc->get<0>());
      Transform X;
      X.setTransform(tc->get<1>());
      std::map<int,Geometry::Transform> TMap;
      TMap.insert(std::pair<int,Transform>(2,X));
      A.applyTransform(TMap);
      std::ostringstream cx;
      cx<<A;
      if (StrFunc::fullBlock(cx.str())!=tc->get<2>())
	{
	  ELog::EM<<"A == "<<A;
	  ELog::EM<<"Expected "<<tc->get<2>()<<ELog::endTrace;
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

  typedef boost::tuple<std::string,Geometry::Vec3D,
		       Quaternion,int> TTYPE;

  std::vector<TTYPE> Tests;

  Tests.push_back(TTYPE("px 1",Geometry::Vec3D(3.4,5.2,3.3),
			Quaternion::calcQRotDeg(90.0,Geometry::Vec3D(0,1,0)),
			-1));
  Tests.push_back(TTYPE("pz 1",Geometry::Vec3D(3.4,5.2,3.3),
			Quaternion::calcQRotDeg(90.0,Geometry::Vec3D(0,1,0)),
			1));
  
  std::vector<TTYPE>::const_iterator tc;
  for(tc=Tests.begin();tc!=Tests.end();tc++)
    {
      Plane A;
      A.setSurface(tc->get<0>());
      A.rotate(tc->get<2>());
      if (A.side(tc->get<1>())!=tc->get<3>() ||
	  A.Quadratic::side(tc->get<1>())!=tc->get<3>())
	{
	  ELog::EM<<"Org == "<<tc->get<0>()<<ELog::endTrace;
	  ELog::EM<<"A == "<<A<<ELog::endTrace;
	  
	  ELog::EM<<"Side == "<<A.side(tc->get<1>())
		  <<" != "<<tc->get<3>()<<ELog::endTrace;

	  ELog::EM<<"Quadratic Side == "<<A.Quadratic::side(tc->get<1>())
		  <<" != "<<tc->get<3>()<<ELog::endTrace;
	  return -1;
	}
    }
  return 0;
}
