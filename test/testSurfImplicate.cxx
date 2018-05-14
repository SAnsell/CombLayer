/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   test/testSurfImplicate.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#include "Cylinder.h"
#include "surfImplicates.h"

#include "testFunc.h"
#include "testSurfImplicate.h"

using namespace Geometry;

testSurfImplicate::testSurfImplicate() 
  /*!
    Constructor
  */
{}

testSurfImplicate::~testSurfImplicate() 
  /*!
    Destructor
  */
{}

int 
testSurfImplicate::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: index of test
    \retval -1 Distance failed
    \retval 0 All succeeded
  */
{
  ELog::RegMethod RegA("testSurfImplicate","applyTest");
  TestFunc::regSector("testSurfImplicate");

  typedef int (testSurfImplicate::*testPtr)();
  testPtr TPtr[]=
    {
      &testSurfImplicate::testPlaneCylinder,
      &testSurfImplicate::testPlanePlane
    };
  const std::string TestName[]=
    {
      "PlaneCylinder",
      "PlanePlane"
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
testSurfImplicate::testPlanePlane()
  /*!
    Test the plane plane interaction
    \return -ve on test fail
  */
{
  ELog::RegMethod RegA("testPlane","testPlanePlane");

  const Geometry::surfImplicates& SImp=
      Geometry::surfImplicates::Instance();

  
  typedef std::tuple<std::string,std::string,int,int> TTYPE;
  std::vector<TTYPE> Tests=
    {
      TTYPE("px 1","px 2",-1,-1),
      TTYPE("pz 2","pz 1",1,1),
      TTYPE("p 0 0 -1 0.0 ","pz 1",1,-1),
      TTYPE("pz 1 ","p 0 0 -1 0.0 ",1,-1),
      TTYPE("pz 1 ","p 0 0 -1 10.0 ",1,-1),
      TTYPE("pz 1 ","p 0 0 -1 -10.0 ",-1,1)
      
    };

  for(const TTYPE& tc : Tests)
    {
      Plane A;
      Plane B;
      A.setSurface(std::get<0>(tc));
      B.setSurface(std::get<1>(tc));

      std::pair<int,int> Res=
	SImp.isImplicate(&A,&B);

      if (Res.first!=std::get<2>(tc) ||
	  Res.second!=std::get<3>(tc))
	{
	  ELog::EM<<"Plane A == "<<A<<ELog::endDiag;
	  ELog::EM<<"Plane B == "<<B<<ELog::endDiag;
	  ELog::EM<<"Found  == "<<Res.first<<" "<<Res.second<<ELog::endDiag;
	  ELog::EM<<"D == "<<B.distance(Geometry::Vec3D(0,0,0))<<ELog::endDiag;
	  ELog::EM<<"Expect == "<<std::get<2>(tc)<<" "
		  <<std::get<3>(tc)<<ELog::endDiag;
	  return -1;
	}
    }
  return 0;
}

int
testSurfImplicate::testPlaneCylinder()
  /*!
    Test the plane cylinder interaction
    \return -ve on test fail
  */
{
  ELog::RegMethod RegA("testPlane","testPlanePlane");

  const Geometry::surfImplicates& SImp=
      Geometry::surfImplicates::Instance();

  
  typedef std::tuple<std::string,std::string,int,int> TTYPE;
  std::vector<TTYPE> Tests=
    {
      TTYPE("pz 10","cx 2",1,1),
      TTYPE("pz -10","cx 2",-1,1),
      TTYPE("p 0 0 -1 10","cx 2",1,1),
      TTYPE("p 0 0 -1 -10","cx 2",-1,1),
      TTYPE("pz -10","cx 20",0,0)
      
    };

  for(const TTYPE& tc : Tests)
    {
      Plane A;
      Cylinder B;
      A.setSurface(std::get<0>(tc));
      B.setSurface(std::get<1>(tc));

      std::pair<int,int> Res=
	SImp.isImplicate(&A,&B);

      if (Res.first!=std::get<2>(tc) ||
	  Res.second!=std::get<3>(tc))
	{
	  ELog::EM<<"Plane A == "<<A<<ELog::endDiag;
	  ELog::EM<<"Plane B == "<<B<<ELog::endDiag;
	  ELog::EM<<"Found  == "<<Res.first<<" "<<Res.second<<ELog::endDiag;
	  ELog::EM<<"D == "<<B.distance(Geometry::Vec3D(0,0,0))<<ELog::endDiag;
	  ELog::EM<<"Expect == "<<std::get<2>(tc)<<" "
		  <<std::get<3>(tc)<<ELog::endDiag;
	  return -1;
	}
    }
  return 0;
}
