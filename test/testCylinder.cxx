/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   test/testCylinder.cxx
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
#include <tuple>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
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
#include "Plane.h"
#include "General.h"
#include "Cylinder.h"

#include "testFunc.h"
#include "testCylinder.h"

using namespace Geometry;

testCylinder::testCylinder() 
  /*!
    Constructor
  */
{}

testCylinder::~testCylinder() 
  /*!
    Destructor
  */
{}

int 
testCylinder::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: index of test
    \retval -1 Distance failed
    \retval 0 All succeeded
  */
{
  ELog::RegMethod RegA("testCylinder","applyTest");
  TestFunc::regSector("testCylinder");

  typedef int (testCylinder::*testPtr)();
  testPtr TPtr[]=
    {
      &testCylinder::testDistance,
      &testCylinder::testGeneral,
      &testCylinder::testMirror,
      &testCylinder::testSet,
      &testCylinder::testSideDirection,
      &testCylinder::testTransform
    };

  const std::string TestName[]=
    {
      "Distance",
      "General",
      "Mirror",
      "Set",
      "SideDirection",
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
testCylinder::testDistance()
  /*!
    Test the distance of a point from the cylinder
    \retval -1 :: failed build a cylinder
    \retval 0 :: All passed
  */
{
  ELog::RegMethod RegA("testCylinder","testDistance");

  
  typedef std::tuple<std::string,Geometry::Vec3D,double> TTYPE;
  std::vector<TTYPE> Tests;
  
  Tests.push_back(TTYPE("1 cx 1",Geometry::Vec3D(1.2,0.6,0.4),
			1-sqrt(0.6*0.6+0.4*0.4)));
  Tests.push_back(TTYPE("2 c/x 0.5 0.5 1.0",Geometry::Vec3D(1.2,0.6,0.4),
			1.0-sqrt(2*0.1*0.1)));

  int cnt(1);
  for(const TTYPE& tc : Tests)
    {
      Cylinder A;
      const int retVal=A.setSurface(std::get<0>(tc));
      if (retVal)
        {
	  ELog::EM<<"Failed to build "<<std::get<0>(tc)
		  <<" Ecode == "<<retVal<<ELog::endCrit;
	  return -1;
	}
      const double R=A.distance(std::get<1>(tc));
      if (fabs(R-std::get<2>(tc))>1e-5)
	{
	  ELog::EM<<"Cylinder == "<<A<<ELog::endDiag;
	  ELog::EM<<"TestPoint == "<<std::get<1>(tc)<<ELog::endDiag;

	  ELog::EM<<"Distance [expect]== "<<R<<" [ "
		  <<std::get<2>(tc)<<" ]"<<ELog::endDiag;;
	  ELog::EM<<"--------------"<<ELog::endDiag;
	  return -cnt;
	}
      cnt++;
    }
  return 0;
}

int
testCylinder::testGeneral()
  /*!
    Test the conversion of the cylinder to a general
    object
    \retval -1 :: failed build a cylinder
    \retval 0 :: All passed
  */
{
  ELog::RegMethod RegA("testCylinder","testGeneral");

  const double Radius(10.3);
  //  const Geometry::Vec3D Cent(0,1,0);
  const Geometry::Vec3D Cent(0,-125.165,-125.346);
  const Geometry::Vec3D Axis(0,-0.71934,-0.694658);
  const Geometry::Vec3D RA(Axis.crossNormal());
  const Geometry::Vec3D RB(RA*Axis);
 
  // Example taken from non-working surfIntersect
  Cylinder CX(1,0);
  General GA(2,0);
  CX.setCylinder(Cent,Axis,10.3);
  GA.setSurface("gq 1 0.4825498467  0.5174501533  0  -0.9993907987  0  0"
		" -4.472935931  4.631864504  -95.72466884  ");
  // Test a number of random points on the cylinder
  Geometry::Vec3D TPt;
  TPt=(RB*Radius)+Cent;
  
  if (!GA.onSurface(TPt) || !CX.onSurface(TPt) )
    {
      ELog::EM<<"Failed to have point on surface:"<<TPt<<ELog::endCrit;
      ELog::EM<<"CX == "<<CX.onSurface(TPt)<<" "
	      <<CX.distance(TPt)<<" "<<
	CX.Quadratic::onSurface(TPt)<<ELog::endWarn;
      ELog::EM<<"GA == "<<GA.onSurface(TPt)<<" "
	      <<GA.distance(TPt)<<ELog::endWarn;

      ELog::EM<<"CX == "<<CX<<ELog::endWarn;
      ELog::EM<<"GQ == "<<GA<<ELog::endWarn;
      return -1;
    }
  return 0;
}

int
testCylinder::testMirror()
  /*!
    Test the mirroring of the cone
    \retval -1 :: failed build a cylinder
    \retval 0 :: All passed
  */
{
  ELog::RegMethod RegA("testCylinder","testMirror");
  

  Geometry::Plane P(111,0);
  P.setSurface("px 1");

  
  Geometry::Vec3D Axis(0,0,-1);
  Geometry::Vec3D Point(6,2,18);  
  Cylinder A(201,0);  
  A.setCylinder(Point,Axis,4.0);
  Cylinder Atest(A);

  typedef std::tuple<Cylinder*,Plane*,Vec3D,Vec3D> TTYPE;
  std::vector<TTYPE> Tests;
  Tests.push_back(TTYPE(&A,&P,Vec3D(-4,2,18),Vec3D(0,0,-1)));

  int cnt(1);
  for(const TTYPE& tc : Tests)
    {
      Cylinder AT=*(std::get<0>(tc));  // get a copy
      AT.mirror(*std::get<1>(tc));
      if (AT.getCentre()!=std::get<2>(tc) ||
	  AT.getNormal()!=std::get<3>(tc))
	{
	  ELog::EM<<"Test "<<cnt<<ELog::endTrace;
	  ELog::EM<<"Cylinder = "<<*std::get<0>(tc);
	  ELog::EM<<"Plane = "<<*std::get<1>(tc);
	  ELog::EM<<"Result "<<AT;
	  ELog::EM<<ELog::endTrace;
	}
      cnt++;
    }
  return 0;
}

int
testCylinder::testSideDirection()
  /*!
    Test the side direction function 
    \return 0 on success
   */
{
  ELog::RegMethod RegA("testCylinder","testSideDirection");

  // Cylinder : Start Point : path-direction : Side
  typedef std::tuple<size_t,Geometry::Vec3D,Geometry::Vec3D,int> TTYPE;
  std::vector<Geometry::Cylinder> CylUnits;
  std::vector<TTYPE> Tests;
  
  CylUnits.push_back(Geometry::Cylinder(1,0));
  CylUnits.back().
    setCylinder(Geometry::Vec3D(0,0,0),
		Geometry::Vec3D(0,0,1),5.0);

  // inward:
  Tests.push_back(TTYPE(0,Geometry::Vec3D(5,0,6.5),Geometry::Vec3D(-1,0,0),-1));
  // Outward 
  Tests.push_back(TTYPE(0,Geometry::Vec3D(5,0,6.5),Geometry::Vec3D(1,0,0),1));

  int cnt(1);
  for(const TTYPE& tc : Tests)
    {
      const Geometry::Cylinder& CX=CylUnits[std::get<0>(tc)];
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
testCylinder::testSet()
  /*!
    Test the setting of the cone
    \retval -1 :: failed build a cylinder
    \retval 0 :: All passed
  */
{
  ELog::RegMethod RegA("testCylinder","testSet");

  // axis:Point:dist: String
  typedef std::tuple<Geometry::Vec3D,Geometry::Vec3D,
		       double,std::string> TTYPE;
  std::vector<TTYPE> Tests;
  
  Tests.push_back(TTYPE(Vec3D(0,0,-1),Vec3D(1,2,18),4.0,"54 c/z 1 2 4"));

  for(const TTYPE& tc : Tests)
    {
      Cylinder A(54,0);
      A.setCylinder(std::get<1>(tc),std::get<0>(tc),std::get<2>(tc));
      std::ostringstream cx;
      cx<<A;
      if (StrFunc::removeOuterSpace(cx.str())!=std::get<3>(tc))
	{
	  ELog::EM<<"Cylinder == "<<A<<ELog::endDiag;
	  ELog::EM<<"Expect :"<<std::get<3>(tc)<<":"<<ELog::endDiag;
	  ELog::EM<<"Found  :"<<StrFunc::removeOuterSpace(cx.str())
		  <<":"<<ELog::endDiag;
	  return -1;
	}
    }

  return 0;
}


int
testCylinder::testTransform()
  /*!
    Test the transform of the cone
    \retval -1 :: failed build a cylinder
    \retval 0 :: All passed
  */
{
  ELog::RegMethod RegA("testCylinder","testTransform");

  // Tr / surf / Point / dist / dist+Trans
  typedef std::tuple<std::string,std::string,
		       Vec3D,double,double> TTYPE;

  std::vector<TTYPE> Tests;  
  Tests.push_back(TTYPE("tr2 1 1 2 "
			"1 0 0 0 1 0 0 0 1 ",	
			"c/y -3 4.15 0.3",
			Vec3D(0,0,0),
			sqrt(4.15*4.15+9)-0.3,
			sqrt(4+6.15*6.15)-0.3));

  int cnt(1);
  for(const TTYPE& tc : Tests)
    {
      Cylinder A(54,2);
      A.setSurface(std::get<1>(tc));
      double D=A.distance(std::get<2>(tc));

      if (fabs(D-std::get<3>(tc))>1e-5)
	{
	  ELog::EM<<"Failed on intial distance : "<<
	    "Test:"<<cnt<<ELog::endTrace;
	  ELog::EM<<"Surface = "<<A;
	  ELog::EM<<"Distance == "<<D<<" Expected == "
		  <<std::get<3>(tc)<<ELog::endTrace;
	  return -1;
	}
      Geometry::Transform X;
      const int result=X.setTransform(std::get<0>(tc));
      std::map<int,Geometry::Transform> TMap;
      TMap.insert(std::pair<int,Geometry::Transform>(2,X));
      A.applyTransform(TMap);
      D=A.distance(std::get<2>(tc));

      if (result || fabs(D-std::get<4>(tc))>1e-5)
	{
	  ELog::EM<<"Failed on transform distance : "<<
	    "Test:"<<cnt<<ELog::endTrace;
	  ELog::EM<<"Result = "<<result<<ELog::endTrace;
	  ELog::EM<<"Surface = "<<A;
	  ELog::EM<<"Transform = "<<X;
	  ELog::EM<<"Distance == "<<D<<" Expected == "
		  <<std::get<4>(tc)<<ELog::endTrace;
	  return -2;
	}
      cnt++;
    }
  return 0;
}
   
 
