/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   test/testWrapper.cxx
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
#include <cmath>
#include <complex> 
#include <vector>
#include <map> 
#include <list> 
#include <set>
#include <string>
#include <algorithm>
#include <functional>
#include <numeric>
#include <iterator>
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
#include "mathSupport.h"
#include "support.h"
#include "stringWrite.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Rules.h"
#include "Code.h"
#include "FItem.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "surfExpand.h"
#include "surfEqual.h"
#include "Wrapper.h"

#include "testFunc.h"
#include "testWrapper.h"

using namespace Geometry;

testWrapper::testWrapper() 
  /// Constructor
{}

testWrapper::~testWrapper() 
  /// Destructor
{}

int 
testWrapper::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: Test number to run
    \retval -1 : testWrapper failed
    \retval 0 : All succeeded
  */
{
  ELog::RegMethod RegA("testWrapper","applyTest");

  typedef int (testWrapper::*testPtr)();
  testPtr TPtr[]=
    {
      &testWrapper::testBox
    };
  const std::string TestName[]=
    {
      "Box"
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

void
testWrapper::createSurfaces()
  /*!
    Creates a list of surfaces for used in the objects
  */
{
  ELog::RegMethod RegA("testWrapper","createSurfaces");

  // PLANE SURFACES:
  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();
  SurI.reset();
  
  typedef std::pair<int,std::string> SCompT;
  const std::vector<SCompT> SurfLine=
    {
      SCompT(1,"px -1"),
      SCompT(2,"px 1"),
      SCompT(3,"py -2"),
      SCompT(4,"py 2"),
      SCompT(5,"pz -3"),
      SCompT(6,"pz 3"),
      
      SCompT(80001,"px 4.5"),
      SCompT(80002,"px 6.5"),
      
      SCompT(71,"so 0.8"),
      SCompT(72,"s -0.7 0 0 0.3"),
      SCompT(73,"s 0.6 0 0 0.4")
    };

  // Note that the testObject now manages the "new Plane"
  for(const SCompT& PItem : SurfLine)
    SurI.createSurface(PItem.first,PItem.second);
  
  return;
}

int
testWrapper::testBox()
  /*!
    Create a simple box and expand it
    \return 0 on success / -ve on error
  */
{
  ELog::RegMethod RegA("testWrapper","testBox");
  ModelSupport::surfIndex& SurI= ModelSupport::surfIndex::Instance();
  createSurfaces();

  // Object : OutObject : index  : surf
  typedef std::tuple<std::string,std::string,int,std::string> TTYPE;
  std::vector<TTYPE> Tests;
  
  Tests.push_back(TTYPE("1 -2 3 -4 5 -6","101 -102 103 -104 105 -106",
			1,"101 px -2"));

  HeadRule HOut;
  for(const TTYPE& tc : Tests)
    {
      std::string ObjA=std::get<0>(tc);
      
      ModelSupport::Wrapper WA;
      WA.setSurfOffset(100);
      const std::string NewObject=WA.createSurfaces(ObjA,1.0);
      HeadRule HResult;
      HResult.procString(std::get<1>(tc));
      if (!HOut.procString(NewObject) ||
	  HOut!=HResult)
	{
	  ELog::EM<<"Original == "<<ObjA<<ELog::endTrace;
	  ELog::EM<<"New      == "<<HOut<<ELog::endTrace;
	  ELog::EM<<"Expected == "<<std::get<1>(tc)<<ELog::endTrace;
	  return -1;
	}
      Geometry::Surface* SPtrN=SurI.getSurf(100+std::get<2>(tc));
      Geometry::Surface* SPtrO=SurI.getSurf(std::get<2>(tc));
      std::string Out =StrFunc::stringWrite(*SPtrN);
      if (StrFunc::fullBlock(Out)!=std::get<3>(tc))
	{
	  ELog::EM<<"Old   :"<<*SPtrO;
	  ELog::EM<<"New   :"<<StrFunc::fullBlock(Out)<<std::endl;
	  ELog::EM<<"Expect:"<<std::get<3>(tc)<<ELog::endErr;
	  return -2;
	}
    }
  return 0;
}
