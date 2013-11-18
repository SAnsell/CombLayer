/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   test/testConvex.cxx
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <iterator>
#include <boost/bind.hpp>
#include <boost/format.hpp>

#include "Exception.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "MersenneTwister.h" 
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Vertex.h"
#include "Face.h"
#include "Convex.h"

#include "testFunc.h"
#include "testConvex.h"

extern MTRand RNG;

using namespace Geometry;

testConvex::testConvex() 
  /*!
    Constructor
  */
{}

testConvex::~testConvex() 
  /*!
    Destructor
  */
{}

int 
testConvex::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: index of test
    \retval -1 Distance failed
    \retval 0 All succeeded
  */
{
  ELog::RegMethod RegItem("testConvex","applyTest");
  TestFunc::regSector("testConvex");

  typedef int (testConvex::*testPtr)();
  testPtr TPtr[]=
    { 
      &testConvex::testSimpleHull,
      &testConvex::testHull,
      &testConvex::testPlanes,
      &testConvex::testCube,
      &testConvex::testNormal
    };

  std::string TestName[] = 
    {
      "SimpleHull",
      "Hull",
      "Planes",
      "Cube",
      "Normal"
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
testConvex::testSimpleHull()
  /*!
    Test a simple hull
    \retval -1 :: failed build a hull
    \retval 0 :: All passed
  */
{
  ELog::RegMethod RegItem("testConvex","testSimpleHull");
  Convex ATest;

  std::vector<Geometry::Vec3D> Pts;
  Pts.push_back(Geometry::Vec3D(1,1,0));
  Pts.push_back(Geometry::Vec3D(0,5,0));
  Pts.push_back(Geometry::Vec3D(3,0,0));
  Pts.push_back(Geometry::Vec3D(2,2,2));
  Pts.push_back(Geometry::Vec3D(5,6,2));
  
  ATest.setPoints(Pts);
  ATest.calcDTriangle();
  
  if (ATest.getFaces().size()!=4)
    return -1;

  return 0;
}

int
testConvex::testHull()
  /*!
    Test a simple hull to completion
    \retval -1 :: failed build a hull
    \retval 0 :: All passed
  */
{
  ELog::RegMethod RegItem("testConvex","testHull");

  Convex A;
  std::vector<Geometry::Vec3D> Pts;

  Pts.push_back(Geometry::Vec3D(0,0,0));
  Pts.push_back(Geometry::Vec3D(1,0,0));
  Pts.push_back(Geometry::Vec3D(0,1,0));
  Pts.push_back(Geometry::Vec3D(0,0,1));
  Pts.push_back(Geometry::Vec3D(0.1,0.1,0.1));
  
  A.setPoints(Pts);
  A.calcDTriangle();
  if (!A.isConvex())
    {
      ELog::EM<<"Convex == "<<A.isConvex()<<ELog::endDiag;
      ELog::EM<<ELog::endDiag;
      return -2;
    }

  A.constructHull();
  const std::vector<Face*>& FL=A.getFaces();
  if (FL.size()!=4) 
    return -1;
  for(size_t i=0;i<4;i++)
    for(size_t j=0;j<3;j++)
      {
	if (FL[i]->getVertex(j)->getID()<0 ||
	    FL[i]->getVertex(j)->getID()>3)
	  {
	    ELog::EM<<"Failed on vertex test "<<ELog::endCrit;
	    A.write(ELog::EM.Estream());
	    ELog::EM<<ELog::endDiag;
	    return -2;
	  }
      }
  return 0;
}

int
testConvex::testPlanes()
  /*!
    Test a plane system
    \retval -1 :: failed build a hull
    \retval 0 :: All passed
  */
{
  ELog::RegMethod RegItem("testConvex","testPlanes");
  std::vector<Geometry::Vec3D> Pts;

  Convex A;

  for(int i=0;i<2;i++)
    for(int j=0;j<2;j++)
      for(int k=0;k<2;k++)
	Pts.push_back(Geometry::Vec3D(1.0+i,1.0+j,1.0+k));
  
  A.setPoints(Pts);
  A.calcDTriangle();
  A.constructHull();
  A.createSurfaces(1);
  if (!A.isConvex())
    {
      ELog::EM<<"testPlane :: Convex == "<<A.isConvex()<<ELog::endDiag;
      return -1;
    }
  if (A.getPlanes().size()!=6)
    {
      ELog::EM<<"Number of surfaces == "<<A.getPlanes().size()<<ELog::endDiag;
      return -2;
    }


  int result[]={1,0,0};
  std::vector<Geometry::Vec3D> TPts;
  TPts.push_back(Geometry::Vec3D(1.2,1.2,1.5));
  TPts.push_back(Geometry::Vec3D(0.1,0.1,0.1));
  TPts.push_back(Geometry::Vec3D(2.2,1.2,1.5));

  for(size_t i=0;i<TPts.size();i++)
    {
      if (A.inHull(TPts[i])!=result[i])
	{
	  ELog::EM<<"testPlane :: Failed :: IS VALID "
		  <<A.inHull(TPts[i])<<" "<<result[i]<<ELog::endDiag;
	  ELog::EM<<"testPlane :: Point == "
		  <<TPts[i]<<" at "<<i<<ELog::endDiag;
	  return -1;
	}
      if (A.inSurfHull(TPts[i])!=result[i])
	{
	  ELog::EM<<"testPlane :: IS VALID "
		  <<A.inSurfHull(TPts[i])<<" "<<result[i]<<ELog::endDiag;
	  ELog::EM<<"testPlane Point == "
		  <<TPts[i]<<" at "<<i<<ELog::endDiag;
	  return -2;
	}
    }
  return 0;
}
  
  
int
testConvex::testCube()
  /*!
    Test a simple cube
    \retval -1 :: failed build a hull
    \retval 0 :: All passed
  */
{
  ELog::RegMethod RegItem("testConvex","testCube");
  Convex A;

  std::vector<Geometry::Vec3D> Pts;
  Pts.push_back(Geometry::Vec3D(0,0,0));
  Pts.push_back(Geometry::Vec3D(1,0,0));
  Pts.push_back(Geometry::Vec3D(1,1,0));
  Pts.push_back(Geometry::Vec3D(0,1,0));
  Pts.push_back(Geometry::Vec3D(0,0,-1));
  Pts.push_back(Geometry::Vec3D(1,0,-1));
  Pts.push_back(Geometry::Vec3D(1,1,-1));
  Pts.push_back(Geometry::Vec3D(0,1,-1));
  
  A.setPoints(Pts);
  A.calcDTriangle();
  A.constructHull();

  const std::vector<Face*>& FL=A.getFaces();
  if (FL.size()!=12)
    {
      ELog::EM<<"Face count wrong"<<ELog::endWarn;
      A.write(ELog::EM.Estream());
      ELog::EM<<ELog::endDiag;
      return -1;
    }

  if (!A.isConvex())
    {
      // Test 50 random points
      for(int i=0;i<50;i++)
	{
	  const Geometry::Vec3D TPt(RNG.randNorm(0.5,1.0),
				    RNG.randNorm(0.5,1.0),
				    RNG.randNorm(-0.5,1.0));
	  int flag(0);
	  if (TPt[0]<0.0 && TPt[0]>1.0) flag++;
	  if (TPt[1]<0.0 && TPt[1]>1.0) flag++;
	  if (TPt[2]<-1.0 && TPt[2]>0.0) flag++;
	  if ((flag && A.inHull(TPt)) || 
	      (!flag && !A.inHull(TPt)))
	    {
	      ELog::EM<<"Failed on Position test at "
		      <<TPt<<ELog::endCrit;
	      A.write(ELog::EM.Estream());
	      ELog::EM<<ELog::endDiag;
	      return -2;
	    }
	}
    }
  return 0;
}

int
testConvex::testNormal()
  /*!
    Test a simple normal to an object
    \retval -1 :: failed build a hull
    \retval 0 :: All passed
  */
{
  ELog::RegMethod RegItem("testConvex","testNormal");

  Convex A;

  std::vector<Geometry::Vec3D> Pts;
  Pts.push_back(Geometry::Vec3D(0,0,0));
  Pts.push_back(Geometry::Vec3D(1,0,0));
  Pts.push_back(Geometry::Vec3D(1,1,0));
  Pts.push_back(Geometry::Vec3D(0,1,0));
  Pts.push_back(Geometry::Vec3D(0,0,-1));
  Pts.push_back(Geometry::Vec3D(1,0,-1));
  Pts.push_back(Geometry::Vec3D(1,1,-1));
  Pts.push_back(Geometry::Vec3D(0,1,-1));
  
  A.setPoints(Pts);
  A.calcDTriangle();
  A.constructHull();
  const std::vector<Face*> FC=A.getFaces();
  for(size_t i=0;i<FC.size();i++)
    {
      const Geometry::Vec3D& A=FC[i]->getVertex(0)->getV();
      const Geometry::Vec3D& B=FC[i]->getVertex(1)->getV();
      const Geometry::Vec3D& C=FC[i]->getVertex(2)->getV();
      if (FC[i]->volumeSign(FC[i]->getNormal()+(A+B+C)/3.0)!=1)
	{
	  ELog::EM<<"Point "<<i<<ELog::endTrace;
	  ELog::EM<<"Face normal == "<<(A+B+C)/3<<" = "
		  <<FC[i]->getNormal();
	  ELog::EM<<" View == "
		  <<FC[i]->volumeSign(FC[i]->getNormal()+(A+B+C)/3.0)
		  <<ELog::endTrace;
	  return -1;
	}
    }
  return 0;
}
