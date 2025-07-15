/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   test/testSurIntersect.cxx
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
#include <sstream>
#include <cmath>
#include <complex>
#include <vector>
#include <deque>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <iterator>
#include <memory>
#include <tuple>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Sphere.h"
#include "Cylinder.h"
#include "Cone.h"
#include "General.h"
#include "Intersect.h"
#include "SglLine.h"
#include "DblLine.h"
#include "Circle.h"
#include "Ellipse.h"
#include "SurInter.h"

#include "testFunc.h"
#include "testSurIntersect.h"

testSurIntersect::testSurIntersect() 
  /*!
    Constructor
  */
{}

testSurIntersect::~testSurIntersect() 
  /*!
    Destructor
  */
{}

int 
testSurIntersect::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: Value to determine which test to apply
    \return 0 :: success -ve otherwize
  */
{
  ELog::RegMethod RegA("testSurIntersect","applyTest");
  TestFunc::regSector("testSurIntersect");

  typedef int (testSurIntersect::*testPtr)();
  testPtr TPtr[]=
    {
      &testSurIntersect::testConePlaneIntersect,
      &testSurIntersect::testCylPlaneIntersect,
      &testSurIntersect::testMakePoint_Quad,
      &testSurIntersect::testNearPoint,
      &testSurIntersect::testProcessPoint
    };

  const std::string TestName[]=
    {
      "ConePlaneIntersect",
      "CylPlaneIntersect",
      "MakePoint(quadratic)",
      "nearPoint",
      "ProcessPoint"
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
testSurIntersect::testMakePoint_Quad()
  /*!
    Test the intersection of general quadratic surfaces
    \retval 0 :: success
  */
{
  ELog::RegMethod RegA("testSurInterSect","testMakePoint_Quad");

  Geometry::Plane TA(1),TB(3),TC(4);
  Geometry::Plane TD(26),TE(501),TF(502);
  Geometry::Cylinder CA(5);
  Geometry::Cylinder CB(35);
  Geometry::General GA(25);
  Geometry::Sphere SA(11),SB(12),SC(13);

  if (TA.setSurface("px 23") ||
      TB.setSurface("py 10") ||
      TC.setSurface("pz 30") ||
      CA.setSurface("c/x 0 0 33.3") ||
      SA.setSurface("s 0 -5 0 5.0") ||
      SB.setSurface("s 0 5 0 5.0") ||
      SC.setSurface("s 5 0 0 5.0") ||

      GA.setSurface("gq  1  0.4825502516  0.5174497484  0  -0.999390827  0  0"
		    " -4.472273951  4.631175248  -95.72774538 ") ||
      TD.setSurface("p -1 0 0 -0.64") ||
      TE.setSurface("p 0 -0.7193398003 -0.6946583705 177.1085962") ||
      TF.setSurface("p 0 0.6946583705 -0.7193398003 12.21904561")
      )
    {
      ELog::EM<<"Failed to set surfaces"<<ELog::endErr;
      return -1;
    }

  const Geometry::Vec3D Cent(0,-125.165,-125.346);
  const Geometry::Vec3D Axis(0,-0.71934,-0.694658);
  CB.setCylinder(Cent,Axis,10.3);
  std::vector<Geometry::Vec3D> Out=
    SurInter::makePoint(&TA,&TB,&TC);

  if (Out.size()!=1 || Out[0]!=Geometry::Vec3D(23,10,30))
    {
      ELog::EM<<"Failed on basic plane test"<<ELog::endErr;
      ELog::EM<<"Size == "<<Out.size()<<ELog::endErr;
      copy(Out.begin(),Out.end(),
	   std::ostream_iterator<Geometry::Vec3D>(ELog::EM.Estream(),"\n"));
      ELog::EM.error();
      return -2;
    }

  Geometry::Quadratic* QAptr=static_cast<Geometry::Quadratic*>(&TA);
  Geometry::Quadratic* QBptr=static_cast<Geometry::Quadratic*>(&TB);
  Geometry::Quadratic* QCptr=static_cast<Geometry::Quadratic*>(&TC);
  Geometry::Quadratic* QDptr=static_cast<Geometry::Quadratic*>(&CA);
  Geometry::Quadratic* QFptr=static_cast<Geometry::Quadratic*>(&TF);

  // DEBUG
  // Quadratic test for spheres:
  Out.clear();
  //  Out=SurInter::makePoint(QEptr,QFptr,QGptr);  
  Out=SurInter::makePoint(&SA,&SB,&SC);
  if (Out.size()!=1 ||
      Out[0]!=Geometry::Vec3D(0,0,0))
    {
      ELog::EM<<"Failed on quadratic sphere test"<<ELog::endWarn;
      ELog::EM<<"Size == "<<Out.size()<<ELog::endWarn;
      copy(Out.begin(),Out.end(),
	   std::ostream_iterator<Geometry::Vec3D>(ELog::EM.Estream(),"\n"));
      ELog::EM.error();
      return -3;
    }


  Out=SurInter::makePoint(QAptr,QBptr,QCptr);
  if (Out.size()!=1 || Out[0]!=Geometry::Vec3D(23,10,30))
    {
      ELog::EM<<"Failed on quadTest(Plane) test"<<ELog::endErr;
      ELog::EM<<"Size == "<<Out.size()<<ELog::endErr;
      copy(Out.begin(),Out.end(),
	   std::ostream_iterator<Geometry::Vec3D>(ELog::EM.Estream(),"\n"));
      ELog::EM.error();
      return -3;
    }
  // Basic test for cyliner 
  Out=SurInter::makePoint(&TA,&TB,&CA);
  if (Out.size()!=2 || 
      Out[0]!=Geometry::Vec3D(23,10,-sqrt(33.3*33.3-100.0)) ||
      Out[1]!=Geometry::Vec3D(23,10,sqrt(33.3*33.3-100.0)) )
    {
      ELog::EM<<"Failed on basic cylinder test"<<ELog::endCrit;
      ELog::EM<<"Size == "<<Out.size()<<ELog::endCrit;
      copy(Out.begin(),Out.end(),
	   std::ostream_iterator<Geometry::Vec3D>(ELog::EM.Estream(),"\n"));
      ELog::EM<<ELog::endCrit;
      return -4;
    }

  // Quadratic test for cyliner 
  Out.clear();
  Out=SurInter::makePoint(QAptr,QBptr,QDptr);
  if (Out.size()!=2 || 
      Out[0]!=Geometry::Vec3D(23,10,-sqrt(33.3*33.3-100.0)) ||
      Out[1]!=Geometry::Vec3D(23,10,sqrt(33.3*33.3-100.0)) )
    {
      ELog::EM<<"Failed on quadratic cylinder test"<<ELog::endErr;
      ELog::EM<<"Size == "<<Out.size()<<ELog::endErr;
      copy(Out.begin(),Out.end(),
	   std::ostream_iterator<Geometry::Vec3D>(ELog::EM.Estream(),"\n"));
      ELog::EM.error();
      return -4;
    }

  // Quadratic/Cylinder
  Out.clear();
  Out=SurInter::makePoint(&TD,&TE,&CB);
  std::vector<Geometry::Vec3D> OutQ=SurInter::makePoint(&TD,&TE,&GA);
  if (Out.size()!=2 && OutQ.size()!=2 &&
      (Out[0]-OutQ[0]).abs()>1e-5 && (Out[1]-OutQ[1]).abs()>1e-5)
    {
      ELog::EM<<"Failed on cylinder test"<<ELog::endErr;
      ELog::EM<<"Size == "<<Out.size()<<ELog::endErr;
      copy(Out.begin(),Out.end(),
	   std::ostream_iterator<Geometry::Vec3D>(ELog::EM.Estream(),"\n"));
      ELog::EM.error();
      return -5;
    }

  // Quadratic/Cylinder
  Out.clear();
  Out=SurInter::makePoint(&TD,&TF,&CB);
  OutQ=SurInter::makePoint(QFptr,&GA,&TD);
  if (Out.size()!=2 && OutQ.size()!=2 &&
      (Out[0]-OutQ[0]).abs()>1e-5 && (Out[1]-OutQ[1]).abs()>1e-5)
    {
      ELog::EM<<"Failed on cylinder test[2]"<<ELog::endErr;
      ELog::EM<<"Size == "<<Out.size()<<ELog::endErr;
      copy(Out.begin(),Out.end(),
	   std::ostream_iterator<Geometry::Vec3D>(ELog::EM.Estream(),"\n"));
      ELog::EM.error();
      return -6;
    }

  return 0;
}


int
testSurIntersect::testNearPoint()
  /*!
    Tests the Creation of Line and stuff
    \retval 0 :: success
  */
{
  ELog::RegMethod RegA("testSurIntersect","testNearPoint");

  std::vector<Geometry::Vec3D> Pts;
  Pts.push_back(Geometry::Vec3D(10,10,-10));
  Pts.push_back(Geometry::Vec3D(-8,8,-8));
  Pts.push_back(Geometry::Vec3D(0,0,0));
  Pts.push_back(Geometry::Vec3D(-2,0,2));
  
  
  // Target / result
  typedef std::tuple<Geometry::Vec3D,size_t> TTYPE;
  std::vector<TTYPE> Tests;

  Tests.push_back(TTYPE(Geometry::Vec3D(0,0,0),2));
  Tests.push_back(TTYPE(Geometry::Vec3D(8,8,-8),0));

 
  int cnt(1);
  for(const TTYPE& tc : Tests)
    {
      Geometry::Vec3D Result=
	SurInter::nearPoint(Pts,std::get<0>(tc));
      if (Result!=Pts[std::get<1>(tc)])
	{
	  ELog::EM<<"Failed on test "<<cnt<<ELog::endTrace;
	  ELog::EM<<"Target "<<std::get<0>(tc)<<ELog::endTrace;
	  ELog::EM<<"Result "<<Result<<ELog::endTrace;
	  ELog::EM<<"Expect "<<Pts[std::get<1>(tc)]<<ELog::endTrace;
	  return -1;
	}
      cnt++;
    }

  return 0;
}


int
testSurIntersect::testProcessPoint()
  /*!
    Tests the Creation of Line and stuff
    \retval 0 :: success
  */
{
  ELog::RegMethod RegA("testSurInterSect","testProcessPoint");

  std::vector<Geometry::Surface*> SList;

  // 5 PLANES:
  Geometry::Plane TA(1),TB(3),TC(4),TD(33003),TE(145);
  TA.setSurface("px 23");
  TB.setSurface("py 10");
  TC.setSurface("pz 30");
  TD.setSurface("px 8.3");
  TE.setSurface("p -0.7009093959 -0.7132503199 6.022082159");
  SList.push_back(&TA);
  SList.push_back(&TB);
  SList.push_back(&TC);
  SList.push_back(&TD);
  SList.push_back(&TE);
  // 3 CYLINDER:  [8]
  Geometry::Cylinder C(2),D(33005),CX(8);
  C.setSurface("cz 30");
  CX.setSurface("c/y 1 1 30");
  D.setSurface("c/x -3.133492157 -3.78186718 7.25");
  SList.push_back(&C);
  SList.push_back(&CX);
  SList.push_back(&D);

  // surf : surf : surf : OutSize / out
  typedef std::tuple<size_t,size_t,size_t,unsigned int,Geometry::Vec3D> TTYPE;
  const std::vector<TTYPE> Tests=
    {
      // Simple plane
      TTYPE(0,1,2,1,Geometry::Vec3D(23,10,30)),
      // Failed to hit cylinder
      TTYPE(0,1,5,0,Geometry::Vec3D(0,0,0)),
      // Hits to cylinder
      TTYPE(0,1,6,2,Geometry::Vec3D(23,10,-sqrt(900-22*22)+1.0)),
      // Hits to cylinder at angle
      TTYPE(3,4,6,0,Geometry::Vec3D(0,0,0))
    };

  int cnt(1);
  for(const TTYPE& tc : Tests)
    {
      const Geometry::Surface* aPtr=SList[std::get<0>(tc)];
      const Geometry::Surface* bPtr=SList[std::get<1>(tc)];
      const Geometry::Surface* cPtr=SList[std::get<2>(tc)];
      std::vector<Geometry::Vec3D> Out=
	SurInter::processPoint(aPtr,bPtr,cPtr);
      if (Out.size()!=std::get<3>(tc) ||
	  (!Out.empty() && Out[0]!=std::get<4>(tc)))
	{
	  ELog::EM<<"Out.size == "<<Out.size()<<ELog::endDiag;
	  for(size_t i=0;i<Out.size();i++)
	    ELog::EM<<"Out ["<<i<<"] == "<<Out[i]<<ELog::endDiag;
	  ELog::EM<<"Expected point "<<std::get<4>(tc)<<ELog::endDiag;
	  ELog::EM<<"Return == "<<cnt<<ELog::endDiag;
	  return -cnt;
	}
      cnt++;
    }
  return 0;
}

int
testSurIntersect::testConePlaneIntersect()
  /*!
    Tests the Cone-plane intersect
    \retval 0 :: success
  */
{
  ELog::RegMethod RegA("testSurInterSect","testConePlaneIntersect");

  Geometry::Plane PA(1,Geometry::Vec3D(0,0,20),
		     Geometry::Vec3D(-1.0,0,1.0));
  //  PA.setSurface("pz 20");

  // Cylinder
  Geometry::Cone CA(11,Geometry::Vec3D(0,0,0),
		    Geometry::Vec3D(0,0,1),30.0);
  
  // Void test:
  std::unique_ptr<Geometry::Ellipse> IPtr
    (dynamic_cast<Geometry::Ellipse*>(SurInter::calcIntersect(CA,PA)));
  
  if (!IPtr)
    {
      ELog::EM<<"Error : Intersect zero (Void) "<<ELog::endCrit;
      return -1;
    }
  ELog::EM<<"Centre == "<<IPtr->centre()<<ELog::endDiag;
  ELog::EM<<"Minor axis == "<<IPtr->getMinorAxis()<<ELog::endDiag;
  ELog::EM<<"Major axis == "<<IPtr->getMajorAxis()<<ELog::endDiag;

  Geometry::Vec3D Org=IPtr->centre();
  Geometry::Vec3D aAxis=IPtr->getMinorAxis();
  Geometry::Vec3D bAxis=IPtr->getMajorAxis();

  const double theta(25.0*M_PI/180.0);
  Geometry::Vec3D Point = Org+aAxis*std::sin(theta)+bAxis*std::cos(theta);
  ELog::EM<<"Side["<<Point<<"] == "<<CA.side(Point)
	  <<"   "<<CA.distance(Point)<<ELog::endDiag;
  ELog::EM<<"Plane["<<Point<<"] == "<<PA.side(Point)<<ELog::endDiag;
    
  return 0;
}

int
testSurIntersect::testCylPlaneIntersect()
  /*!
    Tests the Ellipse/Cirlce/Line plane intersect
    \retval 0 :: success
  */
{
  ELog::RegMethod RegA("testSurInterSect","testCylPlaneIntersect");

  Geometry::Plane TA(1),TB(2),TC(3),TD(4),TE(5);
  TA.setSurface("px 23");
  TB.setSurface("py 10");
  TC.setSurface("py 3");
  TD.setSurface("pz 8.3");
  // 30 degree plane
  TE.setPlane(Geometry::Vec3D(0,0,0),
	      Geometry::Vec3D(0,cos(M_PI/6.0),sin(M_PI/6.0)));          
  // Cylinder
  Geometry::Cylinder CA(11),CB(12);
  CA.setSurface("cz 10");
  CB.setSurface("c/z 1 2 10");
  //  CX.setSurface("c/y 1 1 30");
  //  D.setSurface("c/x -3.133492157 -3.78186718 7.25");
  //  ELog::EM<<CX<<ELog::endDiag;

  // Void test:
  std::shared_ptr<Geometry::Intersect> IPtr(SurInter::calcIntersect(CA,TA));
  if (IPtr)
    {
      ELog::EM<<"Error : Intersect non zero (Void) "<<*IPtr<<ELog::endCrit;
      return -1;
    }
  // Sng Line
  IPtr.reset(SurInter::calcIntersect(CA,TB));
  if (!IPtr)
    {
      ELog::EM<<"Intersect zero [SngLine]"<<ELog::endCrit;
      return -2;
    }
  std::shared_ptr<Geometry::SglLine> SPtr=
    std::dynamic_pointer_cast<Geometry::SglLine,Geometry::Intersect>(IPtr);
  // Care in the test as Z can be anything
  Geometry::Vec3D tPt=SPtr->getPt();
  tPt[2]=0.0;
  if (!SPtr || 
      SPtr->getNorm()!=Geometry::Vec3D(0,0,1) || 
      tPt!=Geometry::Vec3D(0,10,0))
    {
      ELog::EM<<"SPtr fail [SngLine]"<<*IPtr<<ELog::endCrit;
      ELog::EM<<"Tpt"<<tPt<<" :: "<<SPtr->getNorm()<<ELog::endCrit;
      return -2;      
    }
  
  // DblLine:
  IPtr.reset(SurInter::calcIntersect(CB,TC));
  if (!IPtr)
    {
      ELog::EM<<"Intersect zero [DblLine]"<<*IPtr<<ELog::endCrit;
      return -3;
    }
  std::shared_ptr<Geometry::DblLine> DPtr=
    std::dynamic_pointer_cast<Geometry::DblLine,Geometry::Intersect>(IPtr);

  // Test to see if the y value is 3, and the distance from 
  // the centre of the cylinder is R.
  tPt=DPtr->getPtA()-CB.getCentre();
  Geometry::Vec3D tPtB=DPtr->getPtB()-CB.getCentre();
  const double r=CB.getRadius();
  tPt[2]=0.0;
  tPtB[2]=0.0;
  if (!DPtr || 
      DPtr->getNorm()!=Geometry::Vec3D(0,0,1) || 
      fabs(DPtr->getPtA()[1]-3.0)>Geometry::zeroTol ||
      fabs(DPtr->getPtB()[1]-3.0)>Geometry::zeroTol ||
      fabs(tPt.abs()-r)>Geometry::zeroTol ||
      fabs(tPtB.abs()-r)>Geometry::zeroTol)
    {
      ELog::EM<<"DblPtr fail "<<*IPtr<<ELog::endCrit;
      ELog::EM<<"tPt == "<<tPt<<ELog::endCrit;
      return -3;      
    }
  // CIRCLE:
  
  IPtr.reset(SurInter::calcIntersect(CB,TD));
  if (!IPtr)
    {
      ELog::EM<<"Intersect zero [Circle]"<<*IPtr<<ELog::endCrit;
      return -4;
    }
  std::shared_ptr<Geometry::Circle> CPtr=
    std::dynamic_pointer_cast<Geometry::Circle,Geometry::Intersect>(IPtr);
  if (!CPtr || 
      CPtr->getNorm()!=Geometry::Vec3D(0,0,1) || 
      fabs(CPtr->getRadius()-r)>Geometry::zeroTol ||
      CPtr->getCentre()!=Geometry::Vec3D(1,2,8.3) )
    {
      ELog::EM<<"CirPtr fail "<<*IPtr<<ELog::endCrit;
      return -4;      
    }
  // ELLIPSE:
  IPtr.reset(SurInter::calcIntersect(CA,TE));
  if (!IPtr)
    {
      ELog::EM<<"Intersect zero [Ellipse]"<<*IPtr<<ELog::endCrit;
      return -5;
    }
  std::shared_ptr<Geometry::Ellipse> EPtr=
    std::dynamic_pointer_cast<Geometry::Ellipse,Geometry::Intersect>(IPtr);
  if (!EPtr ||
      !CA.onSurface(EPtr->ParamPt(0.822)) ||
      !TE.onSurface(EPtr->ParamPt(0.763)) )
    {
      ELog::EM<<"CirPtr fail "<<*IPtr<<ELog::endCrit;
      return -4;      
    }
  
  return 0;
}

  
  
