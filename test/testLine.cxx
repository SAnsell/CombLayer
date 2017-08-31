/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   test/testLine.cxx
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
#include <complex>
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
#include "Surface.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Cone.h"
#include "EllipticCyl.h"
#include "Sphere.h"
#include "General.h"
#include "Line.h"
#include "LineIntersectVisit.h"
#include "SurInter.h"

#include "testFunc.h"
#include "testLine.h"

using namespace Geometry;


testLine::testLine() 
  /// Constructor
{}

testLine::~testLine() 
  /// Destructor
{}

int 
testLine::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: index of test
    \retval -1 Failed
    \retval 0 All succeeded
  */
{
  ELog::RegMethod RegA("testLine","applyTest");
  TestFunc::regSector("testLine");

  typedef int (testLine::*testPtr)();
  testPtr TPtr[]=
    {
      &testLine::testConeIntersect,
      &testLine::testCylinderIntersect,
      &testLine::testEllipticCylIntersect,
      &testLine::testInterDistance
    };
  const std::string TestName[]=
    {
      "ConeIntersect",
      "CylinderIntersect",
      "EllipticCylIntersect",
      "InterDistance"
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
testLine::testConeIntersect()
  /*!
    Test the intersection of cones
    \return -ve on error
  */
{
  ELog::RegMethod RegA("testLine","testConeIntersect");

  // Cone : Start Point : Normal : NResults : distance A : distance B 
  typedef std::tuple<std::string,Geometry::Vec3D,Geometry::Vec3D,
		     size_t,Geometry::Vec3D,Geometry::Vec3D> TTYPE;
  const std::vector<TTYPE> Tests=
    {
      TTYPE("ky 1 1",Geometry::Vec3D(-3,0,0),Geometry::Vec3D(0,1,0),
	    2,Geometry::Vec3D(-3,-2,0),Geometry::Vec3D(-3,4,0)),
      
      TTYPE("k/y 2 1 4 1",Geometry::Vec3D(-3,0,0),Geometry::Vec3D(1,0,0),
	    0,Geometry::Vec3D(-3,-2,0),Geometry::Vec3D(-3,4,0)),

      TTYPE("k/y 2 1 4 1",Geometry::Vec3D(-3,0,2),Geometry::Vec3D(0,1,0),
	    2,Geometry::Vec3D(-3,-4.38516,2),Geometry::Vec3D(-3,6.38516,2))

      
      // TTYPE("k/y 12.3 1 -1",Geometry::Vec3D(-7.0,8.90,0),Geometry::Vec3D(1,0,0),
      // 	    2,10.4,3.6)
    };
  
  int cnt(1);
  for(const TTYPE& tc : Tests)
    {
      Cone A;
      Line LX;
      LX.setLine(std::get<1>(tc),std::get<2>(tc));
      
      const int retVal=A.setSurface(std::get<0>(tc));
      if (retVal)
        {
	  ELog::EM<<"Failed to build "<<std::get<0>(tc)
		  <<" Ecode == "<<retVal<<ELog::endErr;
	  return -1;
	}
      std::vector<Geometry::Vec3D> OutPt;
      const size_t NR=LX.intersect(OutPt,A);
      
      std::vector<Geometry::Vec3D> OutPtExtra;
      const size_t NRExtra=LX.intersect(OutPtExtra,static_cast<Quadratic>(A));

      int retValue(0);
      if (NR!=std::get<3>(tc) || NRExtra!=std::get<3>(tc))
	{
	  ELog::EM<<"Failure for test "<<cnt<<ELog::endCrit;
	  ELog::EM<<"Solution Count"<<NR<<" ["<<
	    std::get<3>(tc)<<"] "<<ELog::endCrit;
	  retValue=-1;
	}
      
      if (NR>0 && OutPt[0].Distance(std::get<4>(tc)) > 1e-5)
	retValue=-1;
      if (NR>1 && OutPt[1].Distance(std::get<5>(tc)) > 1e-5)
	retValue=-1;

      if (retValue)
	{
	  ELog::EM<<"Failure for test "<<cnt<<ELog::endCrit;
	  ELog::EM<<"Line "<<std::get<1>(tc)<<" -- "
		  <<std::get<1>(tc)<<" -- "<<ELog::endDiag;
	  if (NR)
	    {
	      ELog::EM<<"Point A "<<OutPt[0]<<" :: "<<std::get<4>(tc)
		    <<ELog::endDiag;
	      ELog::EM<<"Point AX "<<OutPtExtra[0]<<" :: "<<std::get<4>(tc)
		      <<ELog::endDiag;
	    }
	  if (NR>1)
	    {
	      ELog::EM<<"Point B "<<OutPt[1]<<" :: "<<std::get<5>(tc)
		      <<ELog::endDiag;
	      ELog::EM<<"Point B "<<OutPtExtra[1]<<" :: "<<std::get<5>(tc)
		      <<ELog::endDiag;
	    }
	  return retValue;
	}
      cnt++;
    }
  return 0;
}

int
testLine::testCylinderIntersect()
  /*!
    Test the intersection of cylinders
    \return -ve on error
  */
{
  ELog::RegMethod RegA("testLine","testCylinderIntersect");

  // Cylinder : Start Point : Normal : NResults : distance A : distance B 
  typedef std::tuple<std::string,Geometry::Vec3D,Geometry::Vec3D,
		       size_t,double,double> TTYPE;
  std::vector<TTYPE> Tests = {
    TTYPE("c/z 0.0 0.0 100.0",
	  Geometry::Vec3D(0,0,0),Geometry::Vec3D(1,0,0),
	  2,100.0,100.0) ,
    TTYPE("c/z 0.0 0.0 100.0",
	  Geometry::Vec3D(0,0,17),Geometry::Vec3D(1,0,0),
	  2,100.0,100.0) ,

  };
  
  int cnt(1);
  for(const TTYPE& tc : Tests)
    {
      Cylinder A;
      Line LX;
      LX.setLine(std::get<1>(tc),std::get<2>(tc));
      
      const int retVal=A.setSurface(std::get<0>(tc));
      if (retVal)
        {
	  ELog::EM<<"Failed to build "<<std::get<0>(tc)
		  <<" Ecode == "<<retVal<<ELog::endErr;
	  return -1;
	}
      std::vector<Geometry::Vec3D> OutPt;
      const size_t NR=LX.intersect(OutPt,A);
      const double DA=(NR>0) ? OutPt[0].Distance(std::get<1>(tc)) : 0.0;
      const double DB=(NR>1) ? OutPt[1].Distance(std::get<1>(tc)) : 0.0;
      
      if (NR!=std::get<3>(tc) || 
	  (NR>0 && fabs(DA-std::get<4>(tc))> 1e-5) ||
	  (NR>1 && fabs(DB-std::get<5>(tc))> 1e-5) )
	{
	  ELog::EM<<"Test num "<<cnt<<ELog::endDiag;
	  ELog::EM<<"Solution Count:"<<NR<<" ["<<std::get<3>(tc)<<"] "
		  <<ELog::endDiag;
	  if (NR>0)
	    {
	      ELog::EM<<"Point A "<<OutPt[0]<<" :: "<<
		std::get<1>(tc)+std::get<2>(tc)*std::get<4>(tc)<<ELog::endDiag;
	      ELog::EM<<"DA "<<DA<<ELog::endDiag;
	    }
	  if (NR>1)
	    {
	      ELog::EM<<"Point B "<<OutPt[1]<<" :: "<<
		std::get<1>(tc)+std::get<2>(tc)*std::get<5>(tc)<<ELog::endDiag;
	      ELog::EM<<"DB "<<DB<<ELog::endDiag;
	    }
	  return -1;
	}
      cnt++;
    }

  return 0;
}

int
testLine::testEllipticCylIntersect()
  /*!
    Test the intersection of EllipticCylinder
    \return -ve on error
  */
{
  ELog::RegMethod RegA("testLine","testEllipticCyl");

  // EllipCyl : Start Point : Normal : NResults : distance A : distance B 
  typedef std::tuple<std::string,Geometry::Vec3D,Geometry::Vec3D,
		       size_t,double,double> TTYPE;
  const std::vector<TTYPE> Tests=
    {
      TTYPE("ey 1.0 1.0",
	    Geometry::Vec3D(-3,0,0),Geometry::Vec3D(1,0,0),
	    2,2.0,4.0),
  
      TTYPE("ey 1.0 1.0",
	    Geometry::Vec3D(-3,-3.0,0),Geometry::Vec3D(1,0,0),
	    2,2.0,4.0),
      
      TTYPE("e/y 1.0 0.0 1.0 1.0",
	    Geometry::Vec3D(-3,-3.0,0),Geometry::Vec3D(1,0,0),
	    2,3.0,5.0),
      
      TTYPE("e/y 1.0 0.0 3.0 1.0",
	    Geometry::Vec3D(-6,-3.0,0),Geometry::Vec3D(1,0,0),
	    2,4.0,10.0)
    };

  int cnt(1);
  for(const TTYPE& tc : Tests)
    {
      EllipticCyl A;
      Line LX;
      LX.setLine(std::get<1>(tc),std::get<2>(tc));
      
      const int retVal=A.setSurface(std::get<0>(tc));
      if (retVal)
        {
	  ELog::EM<<"Failed to build "<<std::get<0>(tc)
		  <<" Ecode == "<<retVal<<ELog::endCrit;
	  return -1;
	}
      std::vector<Geometry::Vec3D> OutPt;
      const size_t NR=LX.intersect(OutPt,A);

      if (NR!=std::get<3>(tc))
	{
	  ELog::EM<<"Failure for test "<<cnt<<ELog::endCrit;
	  ELog::EM<<"Solution Count:"<<NR<<" ["<<
	    std::get<3>(tc)<<"] "<<ELog::endCrit;
	  return -1;
	}
      const double DA=(NR>0) ? OutPt[0].Distance(std::get<1>(tc)) : 0.0;
      const double DB=(NR>1) ? OutPt[1].Distance(std::get<1>(tc)) : 0.0;
      if (NR>0 && fabs(DA-std::get<4>(tc))> 1e-5) 
	{
	  ELog::EM<<"Failure for test "<<cnt<<ELog::endCrit;
	  ELog::EM<<"Point A "<<OutPt[0]<<" :: expect =="<<
	    std::get<1>(tc)+std::get<2>(tc)*std::get<4>(tc)<<ELog::endDebug;
	  ELog::EM<<"DA "<<DA<<ELog::endCrit;
	  ELog::EM<<"Surface "<<A<<ELog::endCrit;
	  return -1;
	}
      if (NR>1 && fabs(DB-std::get<5>(tc))> 1e-5) 
	{
	  ELog::EM<<"Failure for test "<<cnt<<ELog::endCrit;
	  ELog::EM<<"Point B "<<OutPt[1]<<" :: "<<
	    std::get<1>(tc)+std::get<2>(tc)*std::get<5>(tc)<<ELog::endCrit;
	  ELog::EM<<"DB "<<DB<<ELog::endCrit;
	  return -1;
	}
      cnt++;
    }
  return 0;
}

int
testLine::testInterDistance()
  /*!
    Test the interPoint/interDistance template function
    \return 0 sucess / -ve on failure
  */
{
  ELog::RegMethod RegItem("testLine","testInterDistance");


  std::vector<std::shared_ptr<Geometry::Surface> > SurList;
  SurList.push_back(std::shared_ptr<Geometry::Surface>
		    (new Geometry::Plane(1,0)));
  SurList.back()->setSurface("px 80");
  SurList.push_back(std::shared_ptr<Geometry::Surface>
		    (new Geometry::Cylinder(2,0)));
  SurList.back()->setSurface("c/z 3 5 50");

  // surfN : Origin : Axis : results 
  typedef std::tuple<const Geometry::Surface*,Geometry::Vec3D,
		       Geometry::Vec3D,double> TTYPE;
  const std::vector<TTYPE> Tests=
    {
      
      TTYPE(SurList[0].get(),Geometry::Vec3D(0,0,0),
	    Geometry::Vec3D(1,0,0),80.0),
      TTYPE(SurList[1].get(),Geometry::Vec3D(0,0,0),
	    Geometry::Vec3D(1,0,0),sqrt(50*50-25)+3.0)
    };

  // DO Tests:
  for(const TTYPE& tc : Tests)
    {
      MonteCarlo::LineIntersectVisit LI(std::get<1>(tc),std::get<2>(tc));
      const Geometry::Surface* SPtr=std::get<0>(tc);
      
      LI.clearTrack();
      const double out=LI.getForwardDist(SPtr);
      
      if (std::abs(out-std::get<3>(tc))>1e-5)
	{
	  ELog::EM<<"Line :"<<std::get<1>(tc)<<" :: "
		  <<std::get<2>(tc)<<ELog::endTrace;	      
	  ELog::EM<<"Out  "<<out<<" != "<<std::get<3>(tc)<<ELog::endTrace;
	  ELog::EM<<"Track "<<LI.getTrack()<<ELog::endTrace;
	  ELog::EM<<"Surface :"<<*SPtr<<ELog::endTrace;	      
	  return -1;
	}
    }

  return 0;
}

  
