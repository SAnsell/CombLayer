/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   test/testEllipticCyl.cxx
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
#include "Plane.h"
#include "General.h"
#include "Cylinder.h"
#include "EllipticCyl.h"

#include "testFunc.h"
#include "testEllipticCyl.h"

using namespace Geometry;

testEllipticCyl::testEllipticCyl() 
  /*!
    Constructor
  */
{}

testEllipticCyl::~testEllipticCyl() 
  /*!
    Destructor
  */
{}

int 
testEllipticCyl::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: index of test
    \retval -1 Distance failed
    \retval 0 All succeeded
  */
{
  ELog::RegMethod RegA("testEllipticCyl","applyTest");
  TestFunc::regSector("testEllipticCyl");

  typedef int (testEllipticCyl::*testPtr)();
  testPtr TPtr[]=
    {
      &testEllipticCyl::testDistance,
      &testEllipticCyl::testGeneral,
      &testEllipticCyl::testMirror,
      &testEllipticCyl::testSet,
      &testEllipticCyl::testTransform
    };

  const std::string TestName[]=
    {
      "Distance",
      "General",
      "Mirror",
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
testEllipticCyl::testDistance()
  /*!
    Test the distance of a point from the cylinder
    \retval -1 :: failed build a cylinder
    \retval 0 :: All passed
  */
{
  ELog::RegMethod RegA("testEllipticCyl","testDistance");


  typedef std::tuple<std::string,Geometry::Vec3D,double> TTYPE;
  const std::vector<TTYPE> Tests=
    {
      TTYPE("ez 5.0 5.0",Geometry::Vec3D(0,0,0),5.0),
      TTYPE("ez 3.0 5.0",Geometry::Vec3D(0,0,0),3.0),
      TTYPE("ez 3.0 5.0",Geometry::Vec3D(0,0,3.4),3.0),
      TTYPE("ez 5.0 5.0",Geometry::Vec3D(-4,4,0),sqrt(32)-5.0),
      TTYPE("ez 3.0 5.0",Geometry::Vec3D(3,0,0),0.0),
      TTYPE("ez 5.0 3.0",Geometry::Vec3D(1,0,0),2.90474)
    };
  
  for(const TTYPE& tc : Tests)
    {
      EllipticCyl A;
      const int retVal=A.setSurface(std::get<0>(tc));
      if (retVal)
        {
	  ELog::EM<<"Failed to build "<<std::get<0>(tc)
		  <<" Ecode == "<<retVal<<ELog::endCrit;
	  return -1;
	}
      double R=A.distance(std::get<1>(tc));
      if (std::abs(R-std::get<2>(tc))>1e-5)
	{
	  ELog::EM<<"elliCylinder == "<<A;
	  ELog::EM<<"String == "<<std::get<0>(tc)<<ELog::endDiag;
	  ELog::EM<<"TestPoint == "<<std::get<1>(tc)<<ELog::endDiag;

	  ELog::EM<<"Distance [expect]== "<<R<<" [ "
		  <<std::get<2>(tc)<<" ]"<<ELog::endDiag;;
	  ELog::EM<<"--------------"<<ELog::endDiag;
	  return -1;
	}
    }
  return 0;
}

int
testEllipticCyl::testGeneral()
  /*!
    Test the conversion of the cylinder to a general
    object
    \retval -1 :: failed build a cylinder
    \retval 0 :: All passed
  */
{
  ELog::RegMethod RegA("testEllipticCyl","testGeneral");

  // Cent:axis::Laxis:dist: String
  typedef std::tuple<Geometry::Vec3D,Geometry::Vec3D,Geometry::Vec3D,
		       double,double,std::string> TTYPE;
  const std::vector<TTYPE> Tests=
    {
      TTYPE(Vec3D(0,0,0),Vec3D(0,0,1),Vec3D(1,0,0),5.0,4.0,"54 ez 5.0 4.0"),
      TTYPE(Vec3D(0,0,0),Vec3D(1,0,0),Vec3D(0,1,0),4.0,5.0,"54 ex 4.0 5.0"),
      TTYPE(Vec3D(0,1,1.0),Vec3D(1,0,0),Vec3D(0,1,0),4.0,5.0,
	    "54 e/x 1.0 1.0 4.0 5.0")
    };

  int cnt(1);
  for(const TTYPE& tc : Tests)
    {
      EllipticCyl A(54,0);
      EllipticCyl B(54,0);

      // TESTS:
      A.setEllipticCyl(std::get<0>(tc),std::get<1>(tc),std::get<2>(tc),
		       std::get<3>(tc),std::get<4>(tc));
      A.normalizeGEQ(9);
      B.setSurface(std::get<5>(tc));
      B.normalizeGEQ(9);

      if ((A!=B) || (A.Quadratic::operator!=(B)))
	{
	  ELog::EM<<"TEST: "<<cnt<<ELog::endDiag;

	  ELog::EM<<"Elliptic[A]        == "<<A;
	  ELog::EM<<"Elliptic[B]        == "<<B;
	  ELog::EM<<ELog::endDiag;
	  return -1;
	}
      cnt++;
    }

  return 0;
}

int
testEllipticCyl::testMirror()
  /*!
    Test the mirroring of the cone
    \retval -1 :: failed build a cylinder
    \retval 0 :: All passed
  */
{
  ELog::RegMethod RegA("testEllipticCyl","testMirror");
  

  Geometry::Plane P(111,0);
  P.setSurface("px 1");

  
  Geometry::Vec3D Axis(0,0,-1);
  Geometry::Vec3D Point(6,2,18);  
  Cylinder A(201,0);  
  A.setCylinder(Point,Axis,4.0);
  Cylinder Atest(A);

  typedef std::tuple<Cylinder*,Plane*,Vec3D,Vec3D> TTYPE;
  const std::vector<TTYPE> Tests=
    {
      TTYPE(&A,&P,Vec3D(-4,2,18),Vec3D(0,0,-1))
    };
  
  int cnt(1);
  for(const TTYPE& tc : Tests)
    {
      Cylinder AT=*(std::get<0>(tc));  // get a copy
      AT.mirror(*std::get<1>(tc));
      if (AT.getCentre()!=std::get<2>(tc) ||
	  AT.getNormal()!=std::get<3>(tc))
	{
	  ELog::EM<<"Test "<<cnt<<ELog::endDiag;
	  ELog::EM<<"Cylinder = "<<*std::get<0>(tc);
	  ELog::EM<<"Plane = "<<*std::get<1>(tc);
	  ELog::EM<<"Result "<<AT;
	  ELog::EM<<ELog::endDiag;
	}
      cnt++;
    }
  return 0;
}


int
testEllipticCyl::testSet()
  /*!
    Test the setting of the cone
    \retval -1 :: failed build a cylinder
    \retval 0 :: All passed
  */
{
  ELog::RegMethod RegA("testEllipticCyl","testSet");
  // Cent:axis::Laxis:dist: String
  typedef std::tuple<Geometry::Vec3D,Geometry::Vec3D,Geometry::Vec3D,
		       double,double,std::string> TTYPE;
  const std::vector<TTYPE> Tests={
      
    TTYPE(Vec3D(0,0,0),Vec3D(0,0,1),Vec3D(1,0,0),4.0,4.0,"54 cz 4.0"),
    TTYPE(Vec3D(0,0,0),Vec3D(1,0,0),Vec3D(0,0,1),4.0,4.0,"54 cx 4.0"),
    TTYPE(Vec3D(0,1,1.0),Vec3D(1,0,0),Vec3D(0,0,1),4.0,4.0,"54 c/x 1.0 1.0 4.0")
  };

  for(const TTYPE& tc : Tests)
    {
      EllipticCyl A(54,0);

      Cylinder C(54,0);
      General G(54,0);
      C.setSurface(std::get<5>(tc));
      C.normalizeGEQ(9);
      G.setSurface(std::get<5>(tc));
      G.normalizeGEQ(9);

      // TESTS:
      A.setEllipticCyl(std::get<0>(tc),std::get<1>(tc),std::get<2>(tc),
		       std::get<3>(tc),std::get<4>(tc));
      A.normalizeGEQ(9);
      
      if ((G!=A) && (C.Quadratic::operator!=(A))) 
	{
	  ELog::EM<<"Cylinder        == "<<C;
	  ELog::EM<<"General         == "<<G;
	  ELog::EM<<"Elliptic        == "<<A;
	  ELog::EM<<"General[Quad]   == ";
	  G.Quadratic::write(ELog::EM.Estream());
	  ELog::EM<<"Cylinder[Quad]  == ";
	  C.Quadratic::write(ELog::EM.Estream());
	  ELog::EM<<"General[Quad]   == ";
	  G.Quadratic::write(ELog::EM.Estream());

	  ELog::EM<<ELog::endDiag;
	  return -1;
	}
    }

  return 0;
}


int
testEllipticCyl::testTransform()
  /*!
    Test the transform of the cone
    \retval -1 :: failed build a cylinder
    \retval 0 :: All passed
  */
{
  ELog::RegMethod RegA("testEllipticCyl","testTransform");

  // Tr / surf / Point / dist / dist+Trans
  typedef std::tuple<std::string,std::string,
		       Vec3D,double,double> TTYPE;

  const std::vector<TTYPE> Tests = {
    TTYPE("tr2 1 1 2 "
	  "1 0 0 0 1 0 0 0 1 ",	
	  "c/y -3 4.15 0.3",
	  Vec3D(0,0,0),
	  sqrt(4.15*4.15+9)-0.3,
	  sqrt(4+6.15*6.15)-0.3)
  };

  int cnt(1);
  for(const TTYPE& tc : Tests)
    {
      Cylinder A(54,2);
      A.setSurface(std::get<1>(tc));
      double D=A.distance(std::get<2>(tc));

      if (std::abs(D-std::get<3>(tc))>1e-5)
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

      if (result || std::abs(D-std::get<4>(tc))>1e-5)
	{
	  ELog::EM<<"Failed on transform distance : "<<
	    "Test:"<<cnt<<ELog::endDiag;
	  ELog::EM<<"Result = "<<result<<ELog::endDiag;
	  ELog::EM<<"Surface = "<<A;
	  ELog::EM<<"Transform = "<<X;
	  ELog::EM<<"Distance == "<<D<<" Expected == "
		  <<std::get<4>(tc)<<ELog::endDiag;
	  return -2;
	}
      cnt++;
    }
  return 0;
}
   
 
