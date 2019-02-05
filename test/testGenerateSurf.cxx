/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   test/testSurfExpand.cxx
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
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>
#include <complex>
#include <vector>
#include <set>
#include <map>
#include <vector>
#include <algorithm>
#include <functional>
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
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "surfRegister.h"
#include "surfExpand.h"
#include "generateSurf.h"

#include "testFunc.h"
#include "testGenerateSurf.h"

using namespace ModelSupport;

testGenerateSurf::testGenerateSurf()
  /*!
    Constructor
  */
{}

testGenerateSurf::~testGenerateSurf() 
  /*!
    Destructor
  */
{}

int 
testGenerateSurf::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: Index of test
    \returns -ve on error 0 on success.
  */
{
  ELog::RegMethod RegA("testGenerateSurf","applyTest");
  TestFunc::regSector("testGenerateSurf");

  typedef int (testGenerateSurf::*testPtr)();
  testPtr TPtr[]=
    {
      &testGenerateSurf::testExpandedSurf,
      &testGenerateSurf::testPlane
    };

  const std::string TestName[]=
    {
      "ExpandedSurf",
      "Plane"
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
testGenerateSurf::testExpandedSurf()
  /*!
    Psuedo test of the external Cut getExpandedSurf
  */
{
  ELog::RegMethod RegA("testGenerateSurf","testExapndedSurf");

  // Surface : Surface : 
  typedef std::tuple<std::string,std::string,Geometry::Vec3D,double> TTYPE;

  const std::vector<TTYPE> Tests=
    {
      TTYPE("px 8.0","px 10.5",Geometry::Vec3D(0,0,0),2.5),
      TTYPE("px 8.0","px 5.5",Geometry::Vec3D(20,0,0),2.5),
      TTYPE("px -8.0","px -10.5",Geometry::Vec3D(0,0,0),2.5),
      TTYPE("pz 12.0","pz 22.0",Geometry::Vec3D(0,0,0),10.0),
      TTYPE("p -1 0 0 1","p -1 0 0 11",Geometry::Vec3D(0,0,0),10.0)
    };
  
  std::vector<TTYPE>::const_iterator tc;

  surfRegister SMap;
  int surfN=2;
  for(const TTYPE& tc : Tests)
    {
      Geometry::Plane* PX=new Geometry::Plane(1,0);
      Geometry::Plane* AimX=new Geometry::Plane(surfN,0);
      
      PX->setSurface(std::get<0>(tc));
      AimX->setSurface(std::get<1>(tc));
      
      const Geometry::Vec3D& expandCentre=std::get<2>(tc);
      const double D=std::get<3>(tc);


      int sideFlag=PX->side(expandCentre);
      if (sideFlag==0) sideFlag=1;
      const Geometry::Plane* OutX=
	ModelSupport::buildShiftedPlane(SMap,surfN,PX,-sideFlag*D);

      
      if (!OutX || (*OutX!=*AimX))
	{
	  ELog::EM<<"Failed : "<<surfN-1<<ELog::endCrit;
	  ELog::EM<<"Original surface "<<*PX;
	  if (OutX)
	    ELog::EM<<"New surface      "<<*OutX;
	  else
	    ELog::EM<<"New surface      NULL"<<ELog::endDiag;
	  
	  ELog::EM<<"Aim surface      "<<*AimX;
	  ELog::EM<<"Center      "<<expandCentre<<ELog::endDiag;
	  ELog::EM<<"Side      "<<PX->side(expandCentre)<<ELog::endDiag;
	  ELog::EM<<ELog::endCrit;
	  return -1;
	}

      delete PX;
      delete AimX;
      surfN++;
    }
  return 0;
}


int
testGenerateSurf::testPlane()
  /*!
    Test the plane expansion system
    \returns 0 on succes and -ve on failure
  */
{
  ELog::RegMethod RegA("testGenerateSurf","testPlane");

  // Surface : Surface : 
  typedef std::tuple<std::string,std::string,double> TTYPE;

  const std::vector<TTYPE> Tests=
    {
      TTYPE("px 8.0","px 10.5",2.5),
      TTYPE("px 8.0","px 5.5",-2.5),
      TTYPE("px -8.0","px -10.5",-2.5),
      TTYPE("pz 12.0","pz 22.0",10.0),
      TTYPE("p -1 0 0 1","p -1 0 0 11",10.0)
    };
  
  std::vector<TTYPE>::const_iterator tc;

  surfRegister SMap;
  int surfN=2;
  for(const TTYPE& tc : Tests)
    {
      Geometry::Plane* PX=new Geometry::Plane(1,0);
      Geometry::Plane* AimX=new Geometry::Plane(surfN,0);
      
      PX->setSurface(std::get<0>(tc));
      AimX->setSurface(std::get<1>(tc));
      
      const double D=std::get<2>(tc);
     
      
      const Geometry::Plane* OutX=
	ModelSupport::buildShiftedPlane(SMap,surfN,PX,D);

      if (!OutX || (*OutX!=*AimX))
	{
	  ELog::EM<<"Failed : "<<surfN-1<<ELog::endCrit;
	  ELog::EM<<"Original surface "<<*PX;
	  if (OutX)
	    ELog::EM<<"New surface      "<<*OutX;
	  else
	    ELog::EM<<"New surface      NULL"<<ELog::endDiag;
	  
	  ELog::EM<<"Aim surface      "<<*AimX;
	  ELog::EM<<ELog::endCrit;
	  return -1;
	}
	    
      delete PX;
      delete AimX;
      surfN++;
    }
  return 0;
}

