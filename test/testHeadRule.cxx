/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   test/testHeadRule.cxx
 *
 * Copyright (c) 2004-2024 by Stuart Ansell
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
#include <complex>
#include <cmath>
#include <vector>
#include <set>
#include <map>
#include <stack>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <memory>
#include <tuple>

#include "FileReport.h"
#include "OutputLog.h"
#include "NameStack.h"
#include "MemStack.h"
#include "RegMethod.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "interPoint.h"
#include "support.h"
#include "Rules.h"
#include "HeadRule.h"
#include "surfIndex.h"
#include "SurInter.h"
#include "Surface.h"

//#include "Line.h"
//#include "LineIntersectVisit.h"

#include "testFunc.h"
#include "testHeadRule.h"


testHeadRule::testHeadRule() 
/*!
  Constructor
*/
{}

testHeadRule::~testHeadRule() 
 /*! 
   Destructor
 */
{}

void
testHeadRule::createSurfaces()
  /*!
    Create the surface list
   */
{
  ELog::RegMethod RegA("testObject","createSurfaces");
  ModelSupport::surfIndex& SurI=
    ModelSupport::surfIndex::Instance();
  SurI.reset();
  // First box :
  SurI.createSurface(1,"px -1");
  SurI.createSurface(2,"px 1");
  SurI.createSurface(3,"py -1");
  SurI.createSurface(4,"py 1");
  SurI.createSurface(5,"pz -1");
  SurI.createSurface(6,"pz 1");

  SurI.createSurface(11,"px -1");
  SurI.createSurface(12,"px 1");
  SurI.createSurface(13,"py -1");
  SurI.createSurface(14,"py 1");
  SurI.createSurface(15,"pz -1");
  SurI.createSurface(16,"pz 1");

  SurI.createSurface(21,"px -11");
  SurI.createSurface(22,"px 11");
  SurI.createSurface(23,"py -11");
  SurI.createSurface(24,"py 11");
  SurI.createSurface(25,"pz -11");
  SurI.createSurface(26,"pz 11");


  // Sphere :
  SurI.createSurface(100,"so 25");
  // Cylinder :
  SurI.createSurface(107,"cy 10");

  return;
}

int 
testHeadRule::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: Index of test to run
    \return 0 on success / -ve on error
  */
{
  ELog::RegMethod RegA("testHeadRule","applyTest");
  TestFunc::regSector("testHeadRule");

  ELog::MemStack::Instance().setVFlag(1);
  typedef int (testHeadRule::*testPtr)();
  testPtr TPtr[]=
    {
      &testHeadRule::testAddInterUnion,
      &testHeadRule::testCalcSurfIntersection,
      &testHeadRule::testCountLevel,
      &testHeadRule::testEqual,
      &testHeadRule::testFindNodes,      
      &testHeadRule::testFindTopNodes,
      &testHeadRule::testGetComponent,
      &testHeadRule::testGetLevel,
      &testHeadRule::testGetOppositeSurfaces,
      &testHeadRule::testInterceptRule,
      &testHeadRule::testIntersectHeadRule,
      &testHeadRule::testIsLineValid,
      &testHeadRule::testLevel,
      &testHeadRule::testPartEqual,
      &testHeadRule::testRemoveSurf,
      &testHeadRule::testReplacePart,
      &testHeadRule::testSurfSet,
      &testHeadRule::testSurfValid
    };
  const std::string TestName[]=
    {
      "AddInterUnion",
      "CalcSurfIntersection",
      "CountLevel",
      "Equal",
      "FindNodes",
      "FindTopNodes",
      "GetComponent",
      "GetLevel",
      "GetOppositeSurfaces",
      "InterceptRule",
      "IntersectHead",
      "IsLineValid",
      "Level",
      "PartEqual",
      "RemoveSurf",      
      "ReplacePart",      
      "SurfSet",
      "SurfValid"
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
testHeadRule::testAddInterUnion()
  /*!
    Check the validity of a head rule over intersection + union
    \return 0 :: success / -ve on error
   */
{
  ELog::RegMethod RegA("testHeadRule","testAddInterUnion");

  createSurfaces();

  typedef std::tuple<std::string,std::string,std::string> TTYPE;
  const std::vector<TTYPE> Tests=
    {
      TTYPE("1 -2 ","-2 1","-2 1"),
      TTYPE("3 -4 ","-2 -4 3 1","(( -2 1 ) : ( -4 3 ))"),
      TTYPE("5 -6 ","-2 -4 3 1 -6 5","(( -2 1 ) : ( -6 5 ) : ( -4 3 ))"), 
    };
  
  HeadRule A;
  HeadRule B;

  for(const TTYPE& tc : Tests)
    {
      HeadRule tmp;
      if (!tmp.procString(std::get<0>(tc)))
	{
	  ELog::EM<<"Failed to set tmp :"<<std::get<0>(tc)<<ELog::endDebug;
	  return -1;
	}
      A.addIntersection(tmp);
      B.addUnion(tmp);

      const std::string& IStr=std::get<1>(tc);
      const std::string& UStr=std::get<2>(tc);
      if (StrFunc::removeOuterSpace(B.display())!=UStr ||
	  StrFunc::removeOuterSpace(A.display())!=IStr)
	{
	  ELog::EM<<"Failed on test:"<<ELog::endDiag;
	  ELog::EM<<"A   == "<<A.display()<<ELog::endDiag;
	  ELog::EM<<"AEX == "<<IStr<<"\n"<<ELog::endDiag;
	  
	  ELog::EM<<"B   == "<<B.display()<<ELog::endDiag;
	  ELog::EM<<"BEX == "<<UStr<<ELog::endDiag;
	  return -1;
	}
	    
    }
  return 0;
}

int
testHeadRule::testCountLevel()
  /*!
    Check the validity of a head rule level 
    \return 0 :: success / -ve on error
   */
{
  ELog::RegMethod RegA("testHeadRule","testCountLevel");

  createSurfaces();

  typedef std::tuple<std::string,size_t,size_t> TTYPE;
  std::vector<TTYPE> Tests;
  Tests.push_back(TTYPE("1 -2 3 -4 (5:-6) ",1,2));
  Tests.push_back(TTYPE("1 -2 3 -4 (5:-6) ",0,5));
  Tests.push_back(TTYPE("1 -2 3 -4 (5:-6:(7 -8)) ",2,2));
  Tests.push_back(TTYPE("1 -2 3 -4 (5:-6:(7 -8)) ",2,2));
  //  Tests.push_back(TTYPE("1 -2 3 -4 (5:-6) ",4,0));
  //  Tests.push_back(TTYPE("1 -2 3 -4 (5:-6) ",7,-1));


  
  for(const TTYPE& tc : Tests)
    {
      HeadRule tmp;
      if (!tmp.procString(std::get<0>(tc)))
	{
	  ELog::EM<<"Failed to set tmp :"<<std::get<0>(tc)
		  <<ELog::endDebug;
	  return -1;
	}
      const size_t Res=tmp.countNLevel(std::get<1>(tc));
      if (Res!=std::get<2>(tc))
	{
	  ELog::EM<<"A == "<<tmp.display()<<ELog::endDebug;
	  ELog::EM<<"Res["<<std::get<2>(tc)<<"] == "<<Res<<ELog::endDebug;
	  return -1;
	}
    }
  return 0;
}

int
testHeadRule::testEqual()
  /*!
    Check the validity of a head rule level 
    \return 0 :: success / -ve on error
   */
{
  ELog::RegMethod RegA("testHeadRule","testEqual");

  createSurfaces();

  // level : NItems : testItem
  typedef std::tuple<std::string,std::string,bool> TTYPE;
  const std::vector<TTYPE> Tests=
    {
      TTYPE("1 -2 (8001:8002) -8003 -8004 5 -6",
	    "-8004 -8003 -2 -6 ( 8001 : 8002 ) 5 1 ",1),
      TTYPE("1 -2 3 -4 (5:-6) ","5 : -6",0),
      TTYPE("1","4",0),
      TTYPE("4","4",1),
      TTYPE("3 4","4",0),
      TTYPE("3 4","(3:4)",0)
    };

  int cnt(1);
  for(const TTYPE& tc : Tests)
    {
      HeadRule tmpA;
      HeadRule tmpB;
      if (!tmpA.procString(std::get<0>(tc)) ||
	  !tmpB.procString(std::get<1>(tc)) )
	{
	  ELog::EM<<"Failed to set tmpA/B :"<<std::get<0>(tc)
		  <<ELog::endDebug;
	  ELog::EM<<"Failed to set tmpA/B :"<<std::get<1>(tc)
		  <<ELog::endDebug;
	  return -1;
	}
      const bool Res=(tmpA==tmpB);
      if (Res!=std::get<2>(tc))
	{
	  ELog::EM<<"Test Failed: "<<cnt<<ELog::endDiag;
	  ELog::EM<<"A == "<<tmpA.display()<<ELog::endDiag;
	  ELog::EM<<"B == "<<tmpB.display()<<ELog::endDiag;

	  ELog::EM<<"Res["<<std::get<2>(tc)<<"] == "
		  <<Res<<ELog::endDiag;
	  return -1;
	}
      cnt++;
    }
  return 0;
}



int
testHeadRule::testFindNodes()
  /*!
    Check the validity of a head rule level 
    to find nodes withing the system
    \return 0 :: success / -ve on error
  */
{
  ELog::RegMethod RegA("testHeadRule","testFindNodes");

  createSurfaces();

  // level : NItems : testItem
  typedef std::tuple<std::string,size_t,size_t,size_t,std::string> TTYPE;
  const std::vector<TTYPE> Tests=
    {
      TTYPE("1 -2 3 -4 (5:-6) ",0,5,4,"5 : -6"),
      TTYPE("1 -2 3 -4 (5:-6) ",1,2,1,"5"),
      TTYPE("1 -2 3 -4 (5:-6) ",0,5,3,"-4"),
      TTYPE("-4",0,1,0,"-4")
    };

  int cnt(1);
  for(const TTYPE& tc : Tests)
    {
      HeadRule tmp;
      if (!tmp.procString(std::get<0>(tc)))
	{
	  ELog::EM<<"Failed to set tmp :"<<std::get<0>(tc)
		  <<ELog::endDebug;
	  return -1;
	}
      std::vector<const Rule*> Res=tmp.findNodes(std::get<1>(tc));
      
      if (Res.size()!=std::get<2>(tc) ||
	  Res[std::get<3>(tc)]->display()!=std::get<4>(tc))
	{
	  ELog::EM<<"Test Failed: "<<cnt<<ELog::endDiag;
	  ELog::EM<<"Proc String == "<<tmp.display()<<ELog::endDiag;
	  ELog::EM<<"NRes["<<std::get<2>(tc)<<"]  == "<<Res.size()<<ELog::endDiag;
	  ELog::EM<<ELog::endDiag;

	  ELog::EM<<"Res == "<<Res[std::get<3>(tc)]->display()<<ELog::endDiag;
	  for(const Rule* SPtr : Res)
	    ELog::EM<<SPtr->display()<<" ";
	  ELog::EM<<ELog::endDiag;

	  ELog::EM<<"RES["<<std::get<3>(tc)<<"] == "<<
	    Res[std::get<3>(tc)]->display()<<" == "<<
	    std::get<4>(tc)<<ELog::endDiag;
	
	  
	  return -1;
	}
      cnt++;
    }
  return 0;
}

int
testHeadRule::testFindTopNodes()
  /*!
    Check the validity of a head rule level 
    \return 0 :: success / -ve on error
   */
{
  ELog::RegMethod RegA("testHeadRule","testFindTopNodes");

  createSurfaces();

  typedef std::tuple<std::string,size_t,size_t,std::string> TTYPE;
  std::vector<TTYPE> Tests;
  Tests.push_back(TTYPE("1 -2 3 -4 (5:-6) ",5,4,"5 : -6"));
  

  for(const TTYPE& tc : Tests)
    {
      HeadRule tmp;
      if (!tmp.procString(std::get<0>(tc)))
	{
	  ELog::EM<<"Failed to set tmp :"<<std::get<0>(tc)
		  <<ELog::endDebug;
	  return -1;
	}
      std::vector<const Rule*> Res=
	tmp.findTopNodes();
      if (Res.size()!=std::get<1>(tc) ||
	  Res[std::get<2>(tc)]->display()!=std::get<3>(tc))
	{
	  ELog::EM<<"Test Failed "<<ELog::endDiag;
	  ELog::EM<<"A == "<<tmp.display()<<ELog::endDebug;
	  ELog::EM<<"N == "<<Res.size()<<ELog::endDebug;
	  ELog::EM<<"Expected["<<Res[std::get<2>(tc)]->display()
		  <<"] == "<<std::get<3>(tc)
		  <<ELog::endDebug;
	  for(const Rule* SPtr : Res)
	    ELog::EM<<SPtr->display()<<ELog::endDebug;
	  return -1;
	}
    }
  return 0;
}

int
testHeadRule::testGetComponent()
  /*!
    Check the extraction of an indexed compponnent at a given level within
    a HeadRule.
    \return 0 :: success / -ve on error
   */
{
  ELog::RegMethod RegA("testHeadRule","testLevel");

  createSurfaces();

  typedef std::tuple<std::string,size_t,size_t,std::string> TTYPE;
  const std::vector<TTYPE> Tests=
    {
      TTYPE("1 -2 3 -4 (5:-6) ",0,5,"(5 : -6)"),
      TTYPE("1 -2 3 -4 (5:-6) ",0,4,"-4"),
      TTYPE("(-6 : 7 : 35 ) -208 207 -206 -205 204 -203 202 201 ( -16 : -97 : 55 ) ", 0,1,"(-6 : 7 : 35)"),
      TTYPE("(-6 : 7 : 35 ) -208 207 -206 -205 204 -203 202 201 ( -16 : -97 : 55 ) ",0,2,"-208")
    };

  int cnt(1);
  for(const TTYPE& tc : Tests)
    {
      HeadRule tmp;
      if (!tmp.procString(std::get<0>(tc)))
	{
	  ELog::EM<<"Failed to set tmp :"<<std::get<0>(tc)
		  <<ELog::endDebug;
	  return -1;
	}
      HeadRule ARes=tmp.getComponent(std::get<1>(tc),std::get<2>(tc));
      const std::string AStr=StrFunc::removeOuterSpace(ARes.display());
      if (AStr!=std::get<3>(tc))
	{
	  ELog::EM<<"Test Failed: "<<cnt<<ELog::endDiag;
	
	  ELog::EM<<"AResult == "<<AStr<<ELog::endDiag;
	  ELog::EM<<"Expect  == "<<std::get<3>(tc)<<ELog::endDiag;
	  
	  return -2;
	}
      cnt++;
    }
  return 0;
}


int
testHeadRule::testGetOppositeSurfaces()
  /*!
    Tests if the function to determine the opposite signed surfaces
    works
    \retval 0 :: success
  */
{
  ELog::RegMethod RegA("testHeadRule","testGetOppositeSurfaces");

  createSurfaces();
    // HeadRule : surfaces (if any)
  typedef std::tuple<std::string,std::set<int>> TTYPE;

  // Target / result
  std::vector<TTYPE> Tests;
  Tests={
    TTYPE("1 -2 3 -4 (5:-6) ",{}),
    TTYPE("1 -2 3 -4 5 -6 (-5:-6) ",{5}),

    // 59 : 2/ 64 3/ 73 4/29 5/ 71 6/
    TTYPE("( 2 : 3 : -4 : 5 : -6 ) ( 2 : -5 ) "
	  "( -2 : 1 : 3 : -11 : -12 : 13 )"
	  "( -2 : -14 ) ( -15 : 16 : 21 : ( 22 23 ) )"
	  "24 -25 -1 ",{1,2,5})
  };

  int cnt(1);
  for(const TTYPE& tc : Tests)
    {
      HeadRule HM(std::get<0>(tc));
      HM.populateSurf();
      const std::set<int>& outSet=std::get<1>(tc);
      std::set<const Geometry::Surface*> realSet=
	HM.getOppositeSurfaces();

      int fail(0);
      for(const Geometry::Surface* SPtr : realSet)
	{
	  const int SN=SPtr->getName();
	  if (outSet.find(SN)==outSet.end())
	    fail++;
	}
      if (fail || realSet.size()!=outSet.size())
	{
	  ELog::EM<<"Failed on test "<<cnt<<ELog::endTrace;
	  ELog::EM<<"HR "<<HM<<ELog::endDiag;
	  ELog::EM<<"Found surfaces : \n";
	  for(const Geometry::Surface* SPtr : realSet)
	    {
	      const int SN=SPtr->getName();
	      ELog::EM<<"Surf= "<<SN<<ELog::endDiag;
	    }
	  return -1;
	}
      cnt++;
    }
  return 0;
}

int
testHeadRule::testInterceptRule()
  /*!
    Tests the point going into a HeadRule.
    The surface that the track crosses is signed. It needs 
    to have the sign that would make the surface true for 
    points that are at a greater distance from the origin along 
    the track. 
    [uses SurInter]
    \retval 0 :: success
  */
{
  ELog::RegMethod RegA("testHeadRule","testInterceptRule");

  createSurfaces();
  
  // HeadRule : Origin : Axis ::: Point : Surf
  typedef std::tuple<std::string,Geometry::Vec3D,Geometry::Vec3D,
		     Geometry::Vec3D,int> TTYPE;
    
  // Target / result
  std::vector<TTYPE> Tests;
  Tests={
    TTYPE("1",Geometry::Vec3D(0,0,0),Geometry::Vec3D(0,1,0),
	  Geometry::Vec3D(0,0,0),0),
    TTYPE("3",Geometry::Vec3D(0,0,0),Geometry::Vec3D(0,-1,0),
	  Geometry::Vec3D(0,-1,0),-3),
    TTYPE("3",Geometry::Vec3D(0,0,0),Geometry::Vec3D(0,1,0),
	  Geometry::Vec3D(0,-1,0),-3),
    TTYPE("-4",Geometry::Vec3D(0.5,0.5,0),Geometry::Vec3D(0,1,0),
	  Geometry::Vec3D(0.5,1.0,0),4),
    TTYPE("-4",Geometry::Vec3D(0.5,0.5,0),Geometry::Vec3D(0,1,0),
	  Geometry::Vec3D(0.5,1.0,0),4),
    TTYPE("-4",Geometry::Vec3D(0.5,1.5,0),Geometry::Vec3D(0,1,0),
	  Geometry::Vec3D(0.5,1.0,0),-4)
  };
 
  int cnt(1);
  for(const TTYPE& tc : Tests)
    {
      HeadRule HM(std::get<0>(tc));
      const Geometry::Vec3D& O(std::get<1>(tc));
      const Geometry::Vec3D& A(std::get<2>(tc));
      std::pair<Geometry::Vec3D,int> Result=
	SurInter::interceptRule(HM,O,A);
      const Geometry::Vec3D& RPt(std::get<3>(tc));
      int SNum(std::get<4>(tc));
      
      if (Result.second!=SNum ||
	  (SNum && Result.first.Distance(RPt)>1e-5))
	{
	  ELog::EM<<"Failed on test "<<cnt<<ELog::endTrace;
	  ELog::EM<<"HR "<<HM<<ELog::endDiag;
	  ELog::EM<<"Result "<<Result.first<<" at SN="<<Result.second
		  <<ELog::endDiag;
	  ELog::EM<<"Expect "<<RPt<<" at SN="<<SNum<<ELog::endDiag;

	  return -1;
	}
      cnt++;
    }

  return 0;
}

int
testHeadRule::testIntersectHeadRule()
  /*!
    Test the interPoint/interDistance template function
    \return 0 sucess / -ve on failure
  */
{
  ELog::RegMethod RegItem("testHeadRule","testIntersectHeadRule");

  createSurfaces();
  
  // HeadRule : Origin : Axis ::: Index : Point
  typedef std::tuple<std::string,Geometry::Vec3D,Geometry::Vec3D,
		     size_t,Geometry::Vec3D> TTYPE;
    
  // Target / result
  const std::vector<TTYPE> Tests=
    {
      TTYPE("1",Geometry::Vec3D(0,-1,0),Geometry::Vec3D(0,1,0),
	    0,Geometry::Vec3D(0,0,0)),
      TTYPE("1",Geometry::Vec3D(-1,-1,0),Geometry::Vec3D(1,0,0),
	    1,Geometry::Vec3D(0,-1,0))
    };
 
  for(const TTYPE& tc : Tests)
    {
      HeadRule HM(std::get<0>(tc));
      const Geometry::Vec3D& O(std::get<1>(tc));
      const Geometry::Vec3D& A(std::get<2>(tc));
      HM.populateSurf();

      std::vector<Geometry::interPoint> IPts;
      HM.calcSurfIntersection(O,A,IPts);

      const size_t index(std::get<3>(tc));
      const Geometry::Vec3D expectPoint(std::get<4>(tc));
      if (IPts.size()>index &&
	  expectPoint.Distance(IPts[index-1].Pt)>1e-5)
	{
	  ELog::EM<<"Line :"<<std::get<1>(tc)<<" :: "
		  <<std::get<2>(tc)<<ELog::endDiag;	      
	  ELog::EM<<"HR :"<<HM<<ELog::endDiag;
	  ELog::EM<<"Index   :"<<index<<ELog::endDiag;
	  ELog::EM<<"Expected :"<<expectPoint<<ELog::endDiag;
	  ELog::EM<<"Pts size == "<<IPts.size()<<ELog::endDiag;
	  if (IPts.size()>index)
	    ELog::EM<<"Actual   :"<<IPts[index]<<ELog::endDiag;
	  
	  return -1;
	}
    }

  return 0;
}


int
testHeadRule::testCalcSurfIntersection()
  /*!
    Tests the intersection between line and headrule
    \return 0 sucess / -ve on failure
  */
{
  ELog::RegMethod RegItem("testHeadRule","testCalcIntersection");

  createSurfaces();
  
  // HeadRule : Origin : Axis ::: Index : Point
  typedef std::tuple<std::string,Geometry::Vec3D,Geometry::Vec3D,
		     size_t,Geometry::Vec3D> TTYPE;
    
  // Target / result
  const std::vector<TTYPE> Tests=
    {
      TTYPE("21 -22 23 -24 25 -26 (-11:12:-13:14:-15:16)",
	    Geometry::Vec3D(0,0,-20),Geometry::Vec3D(0,0,1),
	    0,Geometry::Vec3D(0,0,0)),
      TTYPE("1 -2 3 -4 5 -6",Geometry::Vec3D(0,0,-10),Geometry::Vec3D(0,0,1),
	    0,Geometry::Vec3D(0,0,0))
    };
 
  for(const TTYPE& tc : Tests)
    {
      HeadRule HM(std::get<0>(tc));
      const Geometry::Vec3D& O(std::get<1>(tc));
      const Geometry::Vec3D& A(std::get<2>(tc));
      HM.populateSurf();
      std::vector<Geometry::interPoint> Pts;
      const size_t nPts=
	HM.calcSurfIntersection(O,A,Pts);

      
      ELog::EM<<"-----------------------------------"<<ELog::endDiag;
      ELog::EM<<"Origin = "<<O<<" == "<<A<<ELog::endDiag;
      const size_t index(std::get<3>(tc));
      const Geometry::Vec3D expectPoint(std::get<4>(tc));

      for(const Geometry::interPoint& PItem : Pts)
	{
	  ELog::EM<<"Pt == "<<PItem.Pt<<" : "<<PItem.SNum
		  <<" D="<<PItem.D<<" F="<<PItem.outFlag<<ELog::endDiag;

	}
    }

  return 0;
}

int
testHeadRule::testIsLineValid()
  /*!
    Test the line segment between start/end point is
    within a cell.
    Determine if a line tracks out of a cell component
    \retval 0 :: success / -ve on failure
  */
{
  ELog::RegMethod RegA("testHeadRule","testIsOuterLine");

  createSurfaces();
  
  // cell : start : end : exit(true/false)
  typedef std::tuple<std::string,Geometry::Vec3D,Geometry::Vec3D,bool> TTYPE;
  std::vector<TTYPE> Tests;
  Tests.push_back(TTYPE("1 -2 3 -4 5 -6",
			Geometry::Vec3D(0,0,0),Geometry::Vec3D(1,0,0),1));

  Tests.push_back(TTYPE("1 -2 3 -4 5 -6",
			Geometry::Vec3D(-2,0,0),Geometry::Vec3D(-1.2,0,0),0));

  Tests.push_back(TTYPE("3 -4 -107",
			Geometry::Vec3D(-2,0,0),Geometry::Vec3D(-1.2,0,0),1));

  Tests.push_back(TTYPE("3 -4 -107",
			Geometry::Vec3D(-0,-2,0),Geometry::Vec3D(0,-1.2,0),0));

  Tests.push_back(TTYPE("3 -4 -107",
			Geometry::Vec3D(-1,0,0),Geometry::Vec3D(1,4,0),1));

  Tests.push_back(TTYPE("3 -4 -107",
			Geometry::Vec3D(1,4,0),Geometry::Vec3D(-1,0,0),1));
  

  for(const TTYPE& tc : Tests)
    {
      HeadRule HR(std::get<0>(tc));
      HR.populateSurf();
      bool Res=HR.isLineValid(std::get<1>(tc),std::get<2>(tc));

      if (Res!=std::get<3>(tc))
	{
	  ELog::EM<<"Surface  == "<<std::get<0>(tc)<<ELog::endTrace;
	  ELog::EM<<"Line == "<<std::get<1>(tc)<<" :: "
		  <<std::get<2>(tc)<<ELog::endTrace;
	  ELog::EM<<"Expect  == "<<std::get<3>(tc)<<ELog::endTrace;
	  return -1;
	}
    }
  return 0;
}

int
testHeadRule::testLevel()
  /*!
    Check the validity of a head rule level 
    \return 0 :: success / -ve on error
   */
{
  ELog::RegMethod RegA("testHeadRule","testLevel");

  createSurfaces();
  // object : surf : level found
  typedef std::tuple<std::string,int,int> TTYPE;
  std::vector<TTYPE> Tests;
  Tests.push_back(TTYPE("1 -2 3 -4 (5:-6) ",5,1));
  Tests.push_back(TTYPE("1 -2 3 -4 (5:-6) ",4,0));
  Tests.push_back(TTYPE("1 -2 3 -4 (5:-6) ",7,-1));
  Tests.push_back(TTYPE("1 -2 3 -4 (5:-6:(7 -8)) ",8,2));
  
  HeadRule A;
  for(const TTYPE& tc : Tests)
    {
      HeadRule tmp;
      if (!tmp.procString(std::get<0>(tc)))
	{
	  ELog::EM<<"Failed to set tmp :"<<std::get<0>(tc)
		  <<ELog::endDebug;
	  return -1;
	}
      const int Res=tmp.level(std::get<1>(tc));
      if (Res!=std::get<2>(tc))
	{
	  ELog::EM<<"A == "<<A.display()<<ELog::endDebug;
	  ELog::EM<<"Res["<<std::get<2>(tc)<<"] == "<<Res<<ELog::endDebug;
	  return -1;
	}
    }
  return 0;
}

int
testHeadRule::testGetLevel()
  /*!
    Check the validity of a head rule level 
    \return 0 :: success / -ve on error
   */
{
  ELog::RegMethod RegA("testHeadRule","testGetLevel");

  createSurfaces();
  // object : surf : level found
  typedef std::tuple<std::string,size_t,std::string> TTYPE;
  std::vector<TTYPE> Tests;
  Tests.push_back(TTYPE("1 -2 3 -4 (5:-6) ",0," 1 -2 3 -4 "));
  Tests.push_back(TTYPE("1 -2 3 -4 (5:-6) ",1," (5:-6) "));
  Tests.push_back(TTYPE("1 -2 3 -4 (5:-6:(7 -8)) ",2," 7 -8 "));
  Tests.push_back(TTYPE("1 ",3," "));
  Tests.push_back(TTYPE("1 ",0,"1 "));
  Tests.push_back(TTYPE("1 2 ",0,"1  2"));
  Tests.push_back(TTYPE("1 -2 3 -4 (5:-6:(7 -8)) (12 13)",1," 5:-6 "));
  Tests.push_back(TTYPE("1 -2 3 -4 (5:-6:(7 -8)) (7 (12:13))",1,
                        " 5:-6:12:13 "));
  Tests.push_back(TTYPE("1 -2 3 -4 (5:-6:(7 -8)) (7 (12:13))",2,
                        " 7 -8 "));
  Tests.push_back(TTYPE("1 2 ",0,"1  2"));
  
  for(const TTYPE& tc : Tests)
    {
      HeadRule tmp,res;
      const std::string item(std::get<2>(tc));
      
      if (!tmp.procString(std::get<0>(tc)) ||
          (!StrFunc::isEmpty(item) && !res.procString(item) ))
	{
	  ELog::EM<<"Failed to set tmp :"<<std::get<0>(tc)
                  <<" ++ "<<std::get<2>(tc)
		  <<ELog::endDiag;
	  return -1;
	}

      HeadRule outLevel=tmp.getLevel(std::get<1>(tc));
      const bool resFlag=(outLevel.partMatched(res));

      if (!resFlag)
	{
	  ELog::EM<<"ALevel == "<<std::get<1>(tc)<<ELog::endDiag;
	  ELog::EM<<"Res["<<res.display()<<"] == "<<outLevel.display()<<ELog::endDiag;
	  return -1;
	}
    }
  return 0;
}

int
testHeadRule::testPartEqual()
  /*!
    Check the validity of a head rule level 
    \return 0 :: success / -ve on error
   */
{
  ELog::RegMethod RegA("testHeadRule","testPartEqual");

  createSurfaces();

  // level : NItems : testItem
  typedef std::tuple<std::string,std::string,bool> TTYPE;
  std::vector<TTYPE> Tests;

  Tests.push_back(TTYPE("3 4","4",1));
  Tests.push_back(TTYPE("3 4","-4",0));
  Tests.push_back(TTYPE("3 4 (9:7:-8)","(7:-8)",1));
  Tests.push_back(TTYPE("3 4 (9:7:-8)","(7:8)",0));
  Tests.push_back(TTYPE("3 4 (9:7:-8 )","(7:-8:(1 2))",0));
  Tests.push_back(TTYPE("3 4 (9:7:-8:(1 2))","(7:-8:(1 2))",1));
  Tests.push_back(TTYPE("3 4 (9:7:-8:(1 2))","(7:-8:(1 -2))",0));
  
  for(const TTYPE& tc : Tests)
    {
      HeadRule tmpA;
      HeadRule tmpB;
      if (!tmpA.procString(std::get<0>(tc)) ||
	  !tmpB.procString(std::get<1>(tc)) )
	{
	  ELog::EM<<"Failed to set tmpA/B :"<<std::get<0>(tc)
		  <<ELog::endDebug;
	  ELog::EM<<"Failed to set tmpA/B :"<<std::get<1>(tc)
		  <<ELog::endDebug;
	  return -1;
	}
      const bool Res=(tmpA.partMatched(tmpB));
      if (Res!=std::get<2>(tc))
	{
	  ELog::EM<<"Test Failed "<<ELog::endDiag;
	  ELog::EM<<"A == "<<tmpA.display()<<ELog::endDiag;
	  ELog::EM<<"B == "<<tmpB.display()<<ELog::endDiag;

	  ELog::EM<<"Res["<<std::get<2>(tc)<<"] == "
		  <<Res<<ELog::endDiag;
	  return -1;
	}
    }
  return 0;
}

int
testHeadRule::testReplacePart()
  /*!
    Check the validity of a head rule level 
    \return 0 :: success / -ve on error
   */
{
  ELog::RegMethod RegA("testHeadRule","testReplacePart");

  createSurfaces();

  // level : NItems : testItem
  typedef std::tuple<std::string,std::string,std::string,
		       bool,std::string> TTYPE;
  std::vector<TTYPE> Tests;

  Tests.push_back(TTYPE("3 4","4","8001",1,"3 8001"));
  Tests.push_back(TTYPE("3 4","-4","8001",0,"3 4"));
  Tests.push_back(TTYPE("3 4 (9:7:-8)","(7:-8)",
			"(8001:8002)",1,"3 4 (9:8001:8002)"));

  for(const TTYPE& tc : Tests)
    {
      HeadRule tmpA;
      HeadRule tmpB;
      HeadRule tmpC;
      HeadRule resRule;
      if (!tmpA.procString(std::get<0>(tc)) ||
	  !tmpB.procString(std::get<1>(tc)) ||
	  !tmpC.procString(std::get<2>(tc)) ||
	  !resRule.procString(std::get<4>(tc)) ) 
	{
	  ELog::EM<<"Failed to set tmpA/B :"<<std::get<0>(tc)
		  <<ELog::endDebug;
	  ELog::EM<<"Failed to set tmpA/B :"<<std::get<1>(tc)
		  <<ELog::endDebug;
	  ELog::EM<<"Failed to set tmpA/B :"<<std::get<2>(tc)
		  <<ELog::endDebug;
	  return -1;
	}
      const bool Res=(tmpA.subMatched(tmpB,tmpC));
      if (Res!=std::get<3>(tc) || tmpA!=resRule)
	{
	  ELog::EM<<"Test Failed "<<ELog::endDiag;
	  ELog::EM<<"A == "<<tmpA<<ELog::endDiag;
	  ELog::EM<<"B == "<<tmpB<<ELog::endDiag;
	  ELog::EM<<"C == "<<tmpC<<ELog::endDiag;
	  ELog::EM<<"Res["<<std::get<3>(tc)<<"] == "
		  <<Res<<ELog::endDiag;
	  ELog::EM<<"ResRULE["<<resRule<<"] != "
		  <<tmpA<<ELog::endDiag;
	  return -1;
	}
    }
  return 0;
}

int
testHeadRule::testRemoveSurf()
  /*!
    Check the validity of a removal of a surface(s)
    \return 0 :: success / -ve on error
   */
{
  ELog::RegMethod RegA("testHeadRule","testRemoveSurf");

  createSurfaces();

  typedef std::tuple<std::string,int,std::string> TTYPE;
  const std::vector<TTYPE> Tests=
    {
      TTYPE("1 -2 -3 4 -5 6",-5,"1 -2 -3 4 6"),
      TTYPE("1 -2 -3 4 -5 6",5,"1 -2 -3 4 -5 6")
    };
  
  HeadRule A;
  HeadRule B;

  int cnt(1);
  for(const TTYPE& tc : Tests)
    {
      if (!A.procString(std::get<0>(tc)))
	{
	  ELog::EM<<"Failed to set tmp :"<<std::get<0>(tc)<<ELog::endDiag;
	  return -1;
	}

      if (!B.procString(std::get<2>(tc)))
	{
	  ELog::EM<<"Failed to set B :"<<std::get<2>(tc)<<ELog::endDiag;
	  return -2;
	}
      A.removeTopItem(std::get<1>(tc));
      if (B!=A)
	{
	  ELog::EM<<"Test Failed:"<<cnt<<ELog::endDiag;
	  ELog::EM<<"A:"<<A.display()<<ELog::endDiag;
	  ELog::EM<<"B:"<<B.display()<<ELog::endDiag;
	  return -3;
	}
      cnt++;
    }
  return 0;
}

int
testHeadRule::testSurfValid()
  /*!
    Check the determination of all surfaces that have
    a point on a surface 
    \return 0 :: success / -ve on error
   */
{
  ELog::RegMethod RegA("testHeadRule","testSurfValid");
  typedef std::tuple<std::string,const Geometry::Vec3D,std::set<int>> TTYPE;

  const std::vector<TTYPE> Tests(
      {
	TTYPE("1 -2 3 -4 5 -6",Geometry::Vec3D(1,1,0),{2,4}),
	TTYPE("1 -22 23 -24 25 -26 (-1:2:-3:4:-5:6)",
	      Geometry::Vec3D(-1,-1,-1),{1,3,5})
      });
  createSurfaces();
  
  int cnt(1);
  for(const TTYPE& tc : Tests)
    {
      const Geometry::Vec3D& Pt(std::get<1>(tc));
      HeadRule A(std::get<0>(tc));
      A.populateSurf();
      const std::set<int>& TSurf=std::get<2>(tc);
      const std::set<int>  ASurf=A.surfValid(Pt);
      
      if (TSurf!=ASurf)
	{
	  ELog::EM<<"Test "<<cnt<<ELog::endDiag;
	  ELog::EM<<"Rule "<<A<<ELog::endDiag;
	  ELog::EM<<"Pt "<<Pt<<ELog::endDiag;
	  ELog::EM<<"ASurf ==";
	  for(const int SN : ASurf)
	    ELog::EM<<" "<<SN;
	  ELog::EM<<"\nTSurf ==";
	  for(const int SN : TSurf)
	    ELog::EM<<" "<<SN;
	  ELog::EM<<ELog::endDiag;
	  return -1;
	}	  
    }      
  
  
  return 0;
}

int
testHeadRule::testSurfSet()
  /*!
    Check the validity of a removal of a by a set
    \return 0 :: success / -ve on error
   */
{
  ELog::RegMethod RegA("testHeadRule","testSurfSet");

  createSurfaces();

  typedef std::tuple<std::string,std::string,std::string> TTYPE;
  std::vector<TTYPE> Tests;
  Tests.push_back(TTYPE("1 -2 ","1 -2",""));  
  

  HeadRule A;
  HeadRule B;
  HeadRule C;

  int cnt(1);
  for(const TTYPE& tc : Tests)
    {
      HeadRule A(std::get<0>(tc));
      HeadRule B(std::get<1>(tc));
      HeadRule C(std::get<2>(tc));
      std::set<int> SN=B.getSignedSurfaceNumbers();
      // remove all items in SN that are in A
      A.isolateSurfNum(SN);
      
      if (A!=C)
	{
	  ELog::EM<<"Failed on test "<<cnt<<ELog::endDiag;
	  ELog::EM<<"SN == ";
	  for(int SI : SN)
	    ELog::EM<<SI<<" ";
	  ELog::EM<<ELog::endDiag;
	  ELog::EM<<"A == "<<A<<ELog::endDebug;
	  ELog::EM<<"B == "<<B<<ELog::endDebug;
	  ELog::EM<<"C == "<<C<<ELog::endDebug;
	  return -1;
	}
      cnt++;
    }
  return 0;
}



