/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   test/testConvex2D.cxx
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#include <sstream>
#include <cmath>
#include <complex>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <functional>
#include <tuple>

#include <boost/format.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "MersenneTwister.h"
#include "Vert2D.h"
#include "Convex2D.h"

#include "testFunc.h"
#include "testConvex2D.h"

using namespace Geometry;

extern MTRand RNG;

testConvex2D::testConvex2D()
  /*!
    Constructor
  */
{}

testConvex2D::~testConvex2D() 
  /*!
    Destructor
  */
{}

void
testConvex2D::initConvexPlane(Geometry::Convex2D& A,
			      const Geometry::Vec3D& Origin,
			      const Geometry::Vec3D& Normal,
			      const int N,
			      const double Width,
			      const double RFactor)
  /*!
    Initialize a plane
    \param A :: Convex2D to populate
    \param Origin :: Point on plane						
    \param Normal :: Plane normal
    \param N :: number required
    \param Width :: Spread required
    \param RFactor :: Random factor to add
  */
{
  ELog::RegMethod RegItem("testConvex2D","initConvexPlane");

  const Geometry::Vec3D X=Normal.crossNormal();
  const Geometry::Vec3D Y=(Normal*X).unit();
  std::vector<Geometry::Vec3D> Pts;
  for(int i=0;i<N;i++)
    {
      // Note the extra unnecessary call to Rand.randNorm
      Pts.push_back(Origin+
		    X*(RNG.randNorm()*Width)+
		    Y*(RNG.randNorm()*Width)+
		    Normal*(RNG.randNorm()*RFactor));
    }
  A.setPoints(Pts);
  return;
}

void
testConvex2D::initCircle(Convex2D& A,const Geometry::Vec3D& Cent,
			 const Geometry::Vec3D& Norm,const double Radius,
			 const int N,const double Random)
 
  /*!
    Set a circle into the convex. 
    \param A :: convex to populate    
    \param Cent  :: Centre of circle
    \param Norm  :: Normal of circle
    \param N :: number required
    \param Radius :: Circle radius
    \param Random :: factor to add
   */
{
  ELog::RegMethod RegA("testConvex2D","initCircle");

  // Construct to 
  Geometry::Vec3D U,V;
  if (fabs(Norm[0]) >= fabs(Norm[1]))
    {
      const double factor = 1/sqrt(Norm[0]*Norm[0]+Norm[2]*Norm[2]);
      U[0] = -Norm[2]*factor;
      U[2]= Norm[0]*factor;
    }
  else
    {
      const double factor = 1/sqrt(Norm[1]*Norm[1]+Norm[2]*Norm[2]);
      U[1] = Norm[2]*factor;
      U[2]= -Norm[1]*factor;
    }
  V=Norm*U;
  std::vector<Geometry::Vec3D> Pts;  
  for(int i=0;i<N;i++)
    {
      const double angle=i*M_PI*2.0/N;
      Pts.push_back(Cent+U*RNG.randNorm(Radius*cos(angle),Random)+
		    V*RNG.randNorm(Radius*sin(angle),Random));
    }
  random_shuffle(Pts.begin(),Pts.end());
  A.setPoints(Pts);
  return;
}


int 
testConvex2D::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: index of test
    \retval -1 Distance failed
    \retval 0 All succeeded
  */
{
  ELog::RegMethod RegItem("testConvex2D","applyTest");
  TestFunc::regSector("testConvex2D");

  typedef int (testConvex2D::*testPtr)();
  testPtr TPtr[]=
    { 
      &testConvex2D::testArea,
      &testConvex2D::testHull,
      &testConvex2D::testInHull,
      &testConvex2D::testMaxElement,
      &testConvex2D::testRandomPlane,
      &testConvex2D::testSimplePlane
    };

  std::string TestName[] = 
    {
      "Area",
      "Hull",
      "InHull",
      "MaxElement",
      "RandomPlane",
      "SimplePlane"

    };
  const int TSize(sizeof(TPtr)/sizeof(testPtr));

  int retValue(0);
  boost::format FmtStr("test%1$s%|30t|(%2$d)\n");
  if (!extra)
    {
      for(int i=0;i<TSize;i++)
	std::cout<<FmtStr % TestName[i] % (i+1);
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
 
  return retValue;
}

int
testConvex2D::testSimplePlane()
  /*!
    Test a simple Plane
    \retval -1 :: failed build a hull
    \retval 0 :: All passed
  */
{
  ELog::RegMethod RegItem("testConvex2D","testSimplePlane");

  Geometry::Vec3D Normal(1,0,0);
  Geometry::Vec3D Orig(10,0,5);
  Convex2D A;
  initConvexPlane(A,Orig,Normal,50,10.0,0.0);
  // Consider a plane 
  //   3x+4y+5z-4=0
  A.calcNormal();


  Normal.makeUnit();
  Geometry::Vec3D Fminus=A.getNormal()-Normal;
  Geometry::Vec3D Fplus=A.getNormal()+Normal;
  if (Fminus.abs()>1e-10 && Fplus.abs()>1e-10)
    {
      ELog::EM<<"N == "<<Normal<<" "<<A.getNormal()<<ELog::endWarn;
      ELog::EM<<"Point == "<<Fminus<<" ++ "<<Fplus<<ELog::endWarn;
      return -2;
    }
  return 0;
}

int
testConvex2D::testRandomPlane()
  /*!
    Test a simple Plane
    \retval -1 :: failed build a hull
    \retval 0 :: All passed
  */
{
  ELog::RegMethod RegItem("testConvex2D","testRandomPlane");

  // Consider a plane 
  //   -3x-4y+5z-4=0
  Convex2D A;
  Geometry::Vec3D Normal(-3,-4,5);
  Geometry::Vec3D Orig(5,5,6);
  initConvexPlane(A,Orig,Normal,50,10.0,1.0);

  const double Wt=A.calcNormal();  
  Normal.makeUnit();
  Geometry::Vec3D Fminus=A.getNormal()-Normal;
  Geometry::Vec3D Fplus=A.getNormal()+Normal;
  if (Fminus.abs()>Wt/100.0 && Fplus.abs()>Wt/100.0)
    {
      ELog::EM<<"Normal== "<<Normal<<ELog::endWarn;
      ELog::EM<<"Result== "<<A.getNormal()<<ELog::endWarn;
      ELog::EM<<"Point == "<<Fminus<<" "<<Fplus<<ELog::endWarn;
      return -2;
    }
  return 0;
}

int
testConvex2D::testMaxElement()
  /*!
    Calculate the max element in a list
    \return 0 on succes -ve on failure
  */
{
  ELog::RegMethod RegA("testConvex2D","testMaxElement");
  Convex2D A;
  
  const Geometry::Vec3D Normal(3,4,5);
  const Geometry::Vec3D Orig(0,0,0);
  initConvexPlane(A,Orig,Normal,50,10.0,0.0);
  A.calcNormal();
  
  A.constructHull();

  const Geometry::Vec3D& CP=A.getCentroid();
  const std::vector<Geometry::Vec3D>& Pts=A.getPoints();
  double maxVal(0.0);
  size_t index(0);
  for(size_t i=0;i<Pts.size();i++)
    {
      const double DX=CP.Distance(Pts[i]);
      if (DX>maxVal)
	 {
	   maxVal=DX;
	   index=i;
	 }
    }
  if (index!=A.getDistPoint())
    {
      ELog::EM<<"Failed on Point (local):"<<index<<" "<<maxVal<<ELog::endWarn;
      ELog::EM<<"Local Pt:"<<Pts[A.getDistPoint()]<<ELog::endWarn;
      ELog::EM<<"Found Point:"<<Pts[index]<<ELog::endWarn;
      ELog::EM<<"Centroid"<<CP<<ELog::endWarn;
      return -1;
    }
  return 0;
}

int
testConvex2D::testHull()
  /*!
    Test the construction of the hull
 
    \return 0 on success / -ve on failure
   */
{
  ELog::RegMethod RegA("testConvex2D","testHull");

  Convex2D B;
  std::vector<Geometry::Vec3D> Pt;
  Pt.push_back(Geometry::Vec3D(-1,0,0));
  Pt.push_back(Geometry::Vec3D(1,0,0));
  Pt.push_back(Geometry::Vec3D(0.1,1,0));
  Pt.push_back(Geometry::Vec3D(0,-2,0));
  Pt.push_back(Geometry::Vec3D(0,0,0));
  B.setPoints(Pt);
  B.constructHull();
  std::vector<Geometry::Vec3D> Out=B.getSequence();

  // Find if the 0,0,0 point has been dropped:
  std::vector<Geometry::Vec3D>::iterator vc=
    find_if(Out.begin(),Out.end(),
	    std::bind(std::equal_to<Geometry::Vec3D>(),Pt[4],
		      std::placeholders::_1));
  
  if (Out.size()!=4  || vc!=Out.end()) 
    {
      for(size_t i=0;i<Out.size();i++)
	ELog::EM<<Out[i]<<" == "<<Out[i].abs()<<ELog::endWarn;
      return -1;
    }
  
  Convex2D A;
  const Geometry::Vec3D PVec(1.0,0,0);
  const Geometry::Vec3D Zero(0.0,0.0,0.0);
  initCircle(A,Zero,PVec,10.0,10,1.0);
  A.constructHull();
  Out=A.getSequence();
  Geometry::Vec3D X(0,1,0);
  

  int errFlag(0);
  for(size_t i=0;i<Out.size() && !errFlag;i++)
    {
      if (fabs(Out[i].dotProd(Vec3D(1,0,0)))  >Geometry::zeroTol) 
	errFlag=1;
    }
  if (errFlag)
    {
      for(size_t i=0;i<Out.size();i++)
	ELog::EM<<Out[i]<<" == "<<Out[i].abs()<<ELog::endWarn;
      return -1;
    }
  return 0;
}

int
testConvex2D::testArea()
  /*!
    Test the area of a convex2d on a plane
 
    \return 0 on success / -ve on failure
   */
{
  ELog::RegMethod RegA("testConvex2D","testArea");


  Convex2D A;
  //  Geometry::Vec3D Centre(3,4,5);       
  const Geometry::Vec3D Centre(0,0,0);     // Centre
  const Geometry::Vec3D X(1,0,0);          // X coordinate
  const Geometry::Vec3D Y(0,1,0);          // Y coordinate
  
  std::vector<Geometry::Vec3D> Pts;
  Pts.push_back(Centre+X*4.0+Y*0.001);
  Pts.push_back(Centre-X*4.0+Y*0.003);
  Pts.push_back(Centre-Y*4.0+X*0.008);
  Pts.push_back(Centre+Y*4.0+X*0.001);

  A.setPoints(Pts);
  A.constructHull();
  Pts=A.getSequence();
  if (fabs(A.calcArea()-32.0)>1e-5)
    {
      for(size_t i=0;i<4;i++)
	ELog::EM<<"Point["<<i<<"]=="<<Pts[i]<<ELog::endTrace;
      ELog::EM<<"Area = "<<fabs(A.calcArea()-32.0)<<ELog::endTrace;
      return -1;
    }

  return 0;
}

int
testConvex2D::testInHull()
  /*!
    Test to determine if point in hull
    \return -ve on error
  */
{
  ELog::RegMethod RegA("testConvex2D","testInHull");

  Convex2D A;
  //  Geometry::Vec3D Centre(3,4,5);       
  const Geometry::Vec3D Centre(0,0,0);     // Centre
  const Geometry::Vec3D X(1,0,0);          // X coordinate
  const Geometry::Vec3D Y(0,1,0);          // Y coordinate
  
  std::vector<Geometry::Vec3D> Pts;

  Pts.push_back(Centre-X*4.0-Y*4.0);
  Pts.push_back(Centre-X*4.0+Y*4.0);
  Pts.push_back(Centre+X*4.0+Y*4.0);
  Pts.push_back(Centre+X*4.0-Y*4.0);

  A.setPoints(Pts);
  A.constructHull();

  // TESTS: Point / Results
  typedef std::tuple<Geometry::Vec3D,int> TTYPE;
  std::vector<TTYPE> Tests;
  Tests.push_back(TTYPE(Geometry::Vec3D(0,0,0),1));        
  Tests.push_back(TTYPE(Geometry::Vec3D(0,6,0),-1));
  Tests.push_back(TTYPE(Geometry::Vec3D(3,3,0),1));
  Tests.push_back(TTYPE(Geometry::Vec3D(3,3,-6),1));
  Tests.push_back(TTYPE(Geometry::Vec3D(2,4,-6),0));

  int cnt(1);
  for(const TTYPE& tc : Tests)
    {
      const int res=A.inHull(std::get<0>(tc));
      if (res!=std::get<1>(tc))
        {
	  ELog::EM<<"Test["<<cnt<<ELog::endDiag;
	  ELog::EM<<"Point = "<<std::get<0>(tc)<<ELog::endDiag;
	  ELog::EM<<"Res[ "<<std::get<1>(tc)<<" ] == "<<res<<ELog::endDiag;
	  return -1;
	}
      cnt++;
    } 

  return 0;
}

