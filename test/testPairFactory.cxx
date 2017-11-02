/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   test/testPairFactory.cxx
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
#include <complex>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <stack>
#include <string>
#include <algorithm>
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
#include "mathSupport.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "Quadratic.h"
#include "Plane.h"
#include "surfaceFactory.h"
#include "Rules.h"
#include "HeadRule.h"
#include "pairBase.h"
#include "pairItem.h"
#include "pairFactory.h"

#include "testFunc.h"
#include "testPairFactory.h"

using namespace ModelSupport;

testPairFactory::testPairFactory()
  /*!
    Constructor
  */
{
  initSim();
}

testPairFactory::~testPairFactory() 
  /*!
    Destructor
  */
{}

void
testPairFactory::initSim()
  /*!
    Set all the objects in the simulation:
  */
{
  ELog::RegMethod RegA("testPairFactory","initSim");

  createSurfaces();
  return;
}

void 
testPairFactory::createSurfaces()
  /*!
    Create the surface list
    - One of each so that 
   */
{
  ELog::RegMethod RegA("testPairFactory","createSurfaces");

  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();
  SurI.reset();
  
  // First Surface
  SurI.createSurface(1,"px -1");
  SurI.createSurface(2,"so 1");
  SurI.createSurface(3,"cx 1");
  SurI.createSurface(4,"kx 1 1");
  // Second
  SurI.createSurface(11,"px 1");
  SurI.createSurface(12,"so 2");
  SurI.createSurface(13,"cx 2");
  SurI.createSurface(14,"kx 2 1");
  
  return;
}

int 
testPairFactory::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: Index of test
    \returns -ve on error 0 on success.
  */
{
  ELog::RegMethod RegA("testPairFactory","applyTest");
  TestFunc::regSector("testPairFactory");

  typedef int (testPairFactory::*testPtr)();
  testPtr TPtr[]=
    {
      &testPairFactory::testConstructPair
    };

  const std::string TestName[]=
    {
      "ConstructPair"
    };

  const int TSize(sizeof(TPtr)/sizeof(testPtr));
  if (!extra)
    {
      std::ios::fmtflags flagIO=std::cout.setf(std::ios::left);
      for(size_t i=0;i<TSize;i++)
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
testPairFactory::testConstructPair()
  /*!
    Test a simple pair divide between two surfaces.
    \return -ve on error 
  */
{
  ELog::RegMethod RegA("testPairFactory","testBasicPair");

  ModelSupport::surfIndex& SurI= ModelSupport::surfIndex::Instance();
  
  // SurfN:SurfN : Type [string]
  typedef std::tuple<int,int,std::string> TTYPE;

  std::vector<TTYPE> Tests;
  Tests.push_back(TTYPE(1,11,"Plane Plane"));
  Tests.push_back(TTYPE(3,13,"Cylinder Cylinder"));
  Tests.push_back(TTYPE(2,12,"Sphere Sphere"));

  int cnt(1);
  for(const TTYPE& tc : Tests)
    {
      const int surfA(std::get<0>(tc));
      const int surfB(std::get<1>(tc));
      
      const Geometry::Surface* SA=SurI.getSurf(surfA);
      const Geometry::Surface* SB=SurI.getSurf(surfB);

      std::shared_ptr<pairBase> pBase=
	std::shared_ptr<pairBase>(pairFactory::createPair(SA,SB));

      const std::string typeName=(pBase) ? pBase->typeINFO() : "No Pointer";
      if (typeName!=std::get<2>(tc))
	{
	  ELog::EM<<"Test : "<<cnt<<ELog::endDebug;
	  ELog::EM<<"Obtained : "<<typeName<<ELog::endDebug;
	  ELog::EM<<"Expected : "<<std::get<2>(tc)<<ELog::endDebug;
	  ELog::EM<<"SA == "<<SA->className()<<ELog::endDebug;
	  ELog::EM<<"SB == "<<SB->className()<<ELog::endDebug;
	  return -1;
	}
      cnt++;
      
    }
  return 0;
}
