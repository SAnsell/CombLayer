/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   test/testQuaternion.cxx
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
#include <iterator>
#include <tuple>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"

#include "testFunc.h"
#include "testQuaternion.h"

using namespace Geometry;

testQuaternion::testQuaternion() 
/*!
  Constructor
*/
{}

testQuaternion::~testQuaternion() 
  /*!
    Destructor
  */
{}

int 
testQuaternion::applyTest(const int extra)
/*!
  Applies all the tests and returns 
  the error number
  \param extra :: Test to execute [-1 all]
  \retval -1 :: Fail on angle
*/
{
  ELog::RegMethod RegA("testQuaternion","applyTest");
  TestFunc::regSector("testQuaternion");

  typedef int (testQuaternion::*testPtr)();
  testPtr TPtr[]=
    { 
      &testQuaternion::testInverse,
      &testQuaternion::testMultiplication,
      &testQuaternion::testRotation,
      &testQuaternion::testVecRot
    };

  std::string TestName[] = 
    {
      "Inverse",
      "Multiplication",
      "Rotation",
      "VecRot"
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
testQuaternion::testInverse()
  /*!
    Tests the Inverse of a Quaternion
    \retval -1 on failure
    \retval 0 :: success 
  */
{
  ELog::RegMethod RegA("testQuaternion","testInverse");

  Quaternion XA(Quaternion::calcQRotDeg(45,0,1,0));
  Quaternion QA(XA);
  QA.inverse();
  QA.inverse();
  if (QA!=XA)
    {
      QA=XA;
      ELog::EM<<"A' == "<<QA.inverse()<<ELog::endDebug;
      ELog::EM<<"A == "<<QA.inverse()<<ELog::endDebug;
      return -1;
    }
  return 0;
}

int
testQuaternion::testMultiplication()
/*!
  Tests the Rotation of a vector by the quaternion
  \retval -1 on failure
  \retval 0 :: success 
*/
{
  ELog::RegMethod RegA("testQuaternion","testMultiplication");
  
  typedef std::tuple<Quaternion,Quaternion,Quaternion> TTYPE;  
  std::vector<TTYPE> Tests;

  Tests.push_back(TTYPE(Quaternion(1,2,3,4),Quaternion(-3,4,5,6),
			Quaternion(-50,-4,0,-8)));

  for(const TTYPE& tc : Tests)
    {
      Quaternion Ans=std::get<0>(tc)*std::get<1>(tc);
      if (Ans!=std::get<2>(tc))
	{
	  ELog::EM<<"A       == "<<std::get<0>(tc)<<ELog::endTrace;
	  ELog::EM<<"B       == "<<std::get<1>(tc)<<ELog::endTrace;
	  ELog::EM<<"Answer  == "<<Ans<<ELog::endTrace;
	  ELog::EM<<"Expect  == "<<std::get<2>(tc)<<ELog::endTrace;
	  return -1;
	}
    }
  return 0;
}

int
testQuaternion::testRotation()
/*!
  Tests the Rotation of a vector by the quaternion
  \retval -1 on failure
  \retval 0 :: success 
*/
{
  ELog::RegMethod RegA("testQuaternion","testRotation");

  typedef std::tuple<double,Vec3D,Vec3D> TTYPE;

  std::vector<TTYPE> Test;
  std::vector<TTYPE> TestR;

  Test.push_back(TTYPE(45.0,Vec3D(1,0,0),Vec3D(1/sqrt(2),1/sqrt(2),0.0)));
  Test.push_back(TTYPE(90.0,Vec3D(1,0,0),Vec3D(0,1,0)));

  TestR.push_back(TTYPE(M_PI/4.0,Vec3D(1,0,0),Vec3D(1/sqrt(2),1/sqrt(2),0.0)));
  TestR.push_back(TTYPE(M_PI/2.0,Vec3D(1,0,0),Vec3D(0,1,0)));

  Vec3D Z(0,0,1);
  for(const TTYPE& tc : Test)
    {
      Geometry::Vec3D A(std::get<1>(tc));
      Quaternion QA(Quaternion::calcQRotDeg(std::get<0>(tc),Z));
      QA.rotate(A);
      if (A!=std::get<2>(tc))
	{
	  ELog::EM<<"A(expect) == "<<std::get<2>(tc)<<ELog::endCrit;
	  ELog::EM<<"A == "<<A<<ELog::endCrit;
	  ELog::EM<<"Rot angle == "<<std::get<0>(tc)<<ELog::endCrit;
	  return -1;
	}
    }
  // Test Radian
  for(const TTYPE& tc : TestR)
    {
      Geometry::Vec3D A(std::get<1>(tc));
      Quaternion QA(Quaternion::calcQRot(std::get<0>(tc),Z));
      QA.rotate(A);
      if (A!=std::get<2>(tc))
	{
	  ELog::EM<<"A(expect) == "<<std::get<2>(tc)<<ELog::endCrit;
	  ELog::EM<<"A == "<<A<<ELog::endCrit;
	  ELog::EM<<"Rot angle == "<<std::get<0>(tc)<<ELog::endCrit;
	  return -1;
	}
    }
  return 0;
}

int
testQuaternion::testVecRot()
  /*!
    There are two ways to calcuate the rotated vector
    This test them to see if they are the same.
    \return -ve on failure / 0 on success
   */
{
  ELog::RegMethod RegA("testQuaternion","testVecRot");

  Quaternion QA(0.0120516298, 0.594058931, -0.00890144333, 0.804294169);
  QA.makeUnit();
  
  Vec3D VT(0,0,-1);
  const double w=QA[0];

  Vec3D qvec=QA.getVec();
  Vec3D v(0,0,-1);
  Vec3D v2=qvec*(v);
  Vec3D v3=qvec*(v2);
  v2 *= (2.0*w);
  v3 *= 2.0;
  
  if (QA.rotate(VT)!=(v+v2+v3))
    {
      ELog::EM<<"Alg == "<<v+v2+v3<<ELog::endTrace;
      ELog::EM<<"QRot == "<<QA.rotate(VT)<<ELog::endTrace;
      return -1;
    }
  return 0;
}
