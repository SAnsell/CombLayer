/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   test/testMasterRotate.cxx
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
#include <memory>
#include <tuple>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Plane.h"
#include "transComp.h"
#include "localRotate.h"
#include "masterRotate.h"

#include "testFunc.h"
#include "testMasterRotate.h"

testMasterRotate::testMasterRotate() 
/*!
  Constructor
*/
{}

testMasterRotate::~testMasterRotate() 
  /*!
    Destructor
  */
{}

void
testMasterRotate::buildRotations(const int angleFlag)
  /*!
    Build a suitable rotation base
    \param angleFlag :: add an angle system
  */
{
  ELog::RegMethod RegA("testMasterRotate","buildRotations");

  masterRotate& MR = masterRotate::Instance();
  // Move X to Z:
  MR.addRotation(Geometry::Vec3D(0,1,0),
		 Geometry::Vec3D(0,0,0),
		 90.0);
  //Move XY to -X-Y 
  MR.addRotation(Geometry::Vec3D(0,0,1),
		 Geometry::Vec3D(0,0,0),
		 -90.0);
  
  MR.addMirror(Geometry::Plane
	       (1,0,Geometry::Vec3D(0,0,0),
		Geometry::Vec3D(1,0,0)));
  if (angleFlag)
    MR.addRotation(Geometry::Vec3D(0,0,1),
		   Geometry::Vec3D(0,0,0),
		   42.85);
  
  return;
}

int 
testMasterRotate::applyTest(const int extra)
/*!
  Applies all the tests and returns 
  the error number
  \param extra :: Test to execute [-1 all]
  \retval -1 :: Fail on angle
*/
{
  ELog::RegMethod RegA("testMasterRotate","applyTest");
  TestFunc::regSector("testMasterRotate");

  typedef int (testMasterRotate::*testPtr)();
  testPtr TPtr[]=
    { 
      &testMasterRotate::testReverse,
      &testMasterRotate::testReverseAxis
    };

  std::string TestName[] = 
    {
      "Reverse",
      "ReverseAxis"
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
testMasterRotate::testReverse()
/*!
  Tests the reverse of the rotation
  \retval -1 on failure
  \retval 0 :: success 
*/
{
  ELog::RegMethod RegA("testMasterRotate","testReverse");
  buildRotations(0);

  typedef std::tuple<size_t,size_t> TTYPE;

  std::vector<Geometry::Vec3D> VSet;
  VSet.push_back(Geometry::Vec3D(1,2,0));  

  std::vector<Geometry::Vec3D> RSet;
  RSet.push_back(Geometry::Vec3D(1,2,0));  
  // VSet.push_back(Vec3D(0,0,-1)); 
  // VSet.push_back(Vec3D(1,0,0)); 
  // VSet.push_back(Vec3D(0,1,0)); 
  // VSet.push_back(Vec3D(-1,0,0)); 
  // VSet.push_back(Vec3D(0,-1,0)); 
  // VSet.push_back(Vec3D(1,-3,-4)); 
  // VSet.push_back(Vec3D(7,2,-1)); 
  
  std::vector<TTYPE> Tests;
  Tests.push_back(TTYPE(0,0));

  const masterRotate& MR = masterRotate::Instance();
  int cnt(1);
  for(const TTYPE& tc : Tests)
    {
      const size_t iA(std::get<0>(tc));
      const size_t iB(std::get<1>(tc));
      const Geometry::Vec3D PtA=MR.reverseRotate(VSet[iA]);
      const Geometry::Vec3D PtB=MR.calcRotate(PtA);
      
      if (PtB!=RSet[iB])
	{
	  ELog::EM<<"Test Num   "<<cnt<<ELog::endTrace;
	  ELog::EM<<"Vectors == "<<VSet[iA]<<ELog::endTrace;
	  ELog::EM<<"        == "<<RSet[iB]<<ELog::endTrace;

	  ELog::EM<<"Rot Vectors == "<<PtA<<ELog::endTrace;
	  ELog::EM<<"            == "<<PtB<<ELog::endTrace;
	  return -1;
	}
      cnt++;
    }

  
  return 0;
}

int
testMasterRotate::testReverseAxis()
/*!
  Tests the reverse of the rotation (for an axis)
  \retval -1 on failure
  \retval 0 :: success 
*/
{
  ELog::RegMethod RegA("testMasterRotate","testReverseAxis");
  buildRotations(0);

  typedef std::tuple<size_t,size_t> TTYPE;
  std::vector<Geometry::Vec3D> VSet;
  VSet.push_back(Geometry::Vec3D(1,0,0).unit());  
  VSet.push_back(Geometry::Vec3D(1,2,0).unit());  

  std::vector<Geometry::Vec3D> RSet;
  RSet.push_back(Geometry::Vec3D(1,0,0).unit());  
  RSet.push_back(Geometry::Vec3D(1,2,0).unit());  
  // VSet.push_back(Vec3D(0,0,-1)); 
  // VSet.push_back(Vec3D(1,0,0)); 
  // VSet.push_back(Vec3D(0,1,0)); 
  // VSet.push_back(Vec3D(-1,0,0)); 
  // VSet.push_back(Vec3D(0,-1,0)); 
  // VSet.push_back(Vec3D(1,-3,-4)); 
  // VSet.push_back(Vec3D(7,2,-1)); 
  
  std::vector<TTYPE> Tests;
  Tests.push_back(TTYPE(0,0));

  const masterRotate& MR = masterRotate::Instance();
  int cnt(1);
  for(const TTYPE& tc : Tests)
    {
      const size_t iA(std::get<0>(tc));
      const size_t iB(std::get<1>(tc));
      const Geometry::Vec3D PtA=MR.reverseAxisRotate(VSet[iA]);
      const Geometry::Vec3D PtB=MR.calcAxisRotate(PtA);
      
      if (PtB!=RSet[iB])
	{
	  ELog::EM<<"Test Num   "<<cnt<<ELog::endTrace;
	  ELog::EM<<"Vectors == "<<VSet[iA]<<ELog::endTrace;
	  ELog::EM<<"        == "<<RSet[iB]<<ELog::endTrace;

	  ELog::EM<<"Rot Vectors == "<<PtA<<ELog::endTrace;
	  ELog::EM<<"            == "<<PtB<<ELog::endTrace;
	  return -1;
	}
      cnt++;
    }

  
  return 0;
}

