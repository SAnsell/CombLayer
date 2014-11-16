/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   test/testFace.cxx
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
#include <fstream>
#include <sstream>
#include <cmath>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <iterator>
#include <tuple>
#include <boost/format.hpp>

#include "Exception.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Vertex.h"
#include "Edge.h"
#include "Face.h"

#include "testFunc.h"
#include "testFace.h"

using namespace Geometry;

faceObj::faceObj(const int Offset,const Vec3D& A,
		 const Vec3D& B,const Vec3D& C)
  /*!
    Constructor / order preserved Triangle
    \param Offset :: Index offset values
    \param A :: First point
    \param B :: second point
    \param C :: Third point
  */
{
  VPtr[0]=new Vertex(Offset+1,A);
  VPtr[1]=new Vertex(Offset+2,B);
  VPtr[2]=new Vertex(Offset+3,C);
  for(int i=0;i<3;i++)
    {
      EPtr[i]=new Edge;
      EPtr[i]->setEndPts(VPtr[i],VPtr[(i+1) % 3]);
    }
  FPtr=new Face(VPtr[0],VPtr[1],VPtr[2],EPtr[0],EPtr[1],EPtr[2]);
}

faceObj::faceObj(const faceObj& A)
  /*!
    Copy Constructor 
    \param A :: Object to copy
   */
{
  for(int i=0;i<3;i++)
    VPtr[0]=new Vertex(*A.VPtr[i]);

  for(int i=0;i<3;i++)
    {
      EPtr[i]=new Edge;
      EPtr[i]->setEndPts(VPtr[i],VPtr[(i+1) % 3]);
    }
  FPtr=new Face(VPtr[0],VPtr[1],VPtr[2],EPtr[0],EPtr[1],EPtr[2]);
}

faceObj&
faceObj::operator=(const faceObj& A)
  /*!
    Assignment operator
    \param A :: Object to copy
    \return *this
  */
{
  if (this!=&A)
    {
      // Note : only have to copy the vertex points
      for(int i=0;i<3;i++)
	*VPtr[i]=*A.VPtr[i];
    }
  return *this;
}

faceObj::~faceObj()
  /*!
    Destructor
  */
{
  for(int i=0;i<3;i++)
    {
      delete VPtr[i];
      delete EPtr[i];
    }
  delete FPtr;
}


testFace::testFace() 
  /*!
    Constructor
  */
{}

testFace::~testFace() 
  /*!
    Destructor
  */
{}

int 
testFace::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: index of test
    \retval -1 Distance failed
    \retval 0 All succeeded
  */
{
  ELog::RegMethod RegItem("testFace","applyTest");
  TestFunc::regSector("testFace");

  typedef int (testFace::*testPtr)();
  testPtr TPtr[]=
    { 
      &testFace::testInTriangle,
      &testFace::testNonPlanar,
      &testFace::testPlanar
    };

  std::string TestName[] = 
    {
      "InTriangle",
      "NonPlanar",
      "Planar",
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
testFace::testInTriangle()
  /*!
    Test to determine if the inTriangle test works
    \return 0 on success
  */
{
  faceObj FA(10,Vec3D(0,0,0),Vec3D(0,1,0),Vec3D(1,0,0));
  faceObj FB(50,Vec3D(8,0,0),Vec3D(3,0,-1),Vec3D(3,0,1));
  faceObj FC(670,Vec3D(8,0,1),Vec3D(1,1,7),Vec3D(1,0,0));

  typedef std::tuple<const faceObj*,Geometry::Vec3D,int> RTYPE;

  std::vector<RTYPE> Results;

  Results.push_back(RTYPE(&FA,Vec3D(0,0,0),0));  // corner case
  Results.push_back(RTYPE(&FA,Vec3D(0,0.5,0),0));  // edge case
  Results.push_back(RTYPE(&FA,Vec3D(0.25,0.25,0),1));  // middle
  Results.push_back(RTYPE(&FA,Vec3D(-0.25,0.25,0),-1));  // outside
  Results.push_back(RTYPE(&FB,Vec3D(1,0,0),-1));  // outside
  Results.push_back(RTYPE(&FC,Vec3D(0.02,0.02,0),-1));  // outside

  int cnt(0);
  for(const RTYPE& tc : Results)
    {
      const faceObj& GA=*std::get<0>(tc);
      const Geometry::Vec3D& Pt=std::get<1>(tc);
      if (GA->flatValid(Pt)!=std::get<2>(tc))
	{
	  ELog::EM<<"Triangle FA["<<cnt<<"] "<<Pt<<" == "
		  <<GA->flatValid(Pt)<<ELog::endCrit;
	  return -1;
	}
      cnt++;
    }
  return 0;
}

int
testFace::testNonPlanar()
  /*!
    Test simple interlocking / non interlocking triangles
    \retval -1 :: failed build a hull
    \retval 0 :: All passed
  */
{
  ELog::RegMethod RegItem("testFace","testSimpleHull");

  faceObj FA(10,Vec3D(0,0,0),Vec3D(0,1,0),Vec3D(1,0,0));
  faceObj FB(20,Vec3D(0,0,1),Vec3D(0,1,1),Vec3D(1,0,1));
  faceObj FC(30,Vec3D(0,0,0),Vec3D(1,1,0),Vec3D(1,-1,0));
  faceObj FD(40,Vec3D(0.5,0,0.0),Vec3D(3,0,1),Vec3D(3,0,-1));

  faceObj FDprime(50,Vec3D(8,0,0),Vec3D(3,0,-1),Vec3D(3,0,1));
  faceObj FDx(50,Vec3D(8,0,0),Vec3D(3,0,-1),Vec3D(3,0,1));
  // Touching case: (fc) [Side]
  faceObj FE(60,Vec3D(8,0,1),Vec3D(1,1,0),Vec3D(1,-1,0));
  // Touching case: (fc) [Part Side]
  faceObj FF(60,Vec3D(8,0,1),Vec3D(1,1,0),Vec3D(1,0,0));
  // Touching case: (fc) [Point:on side]
  faceObj FG(670,Vec3D(8,0,1),Vec3D(1,1,7),Vec3D(1,0,0));

  typedef std::tuple<const faceObj*,const faceObj*,
		       int,std::string> TTYPE;
  std::vector<TTYPE> Tests;
  Tests.push_back(TTYPE(&FA,&FB,-1,std::string("A out of B")));
  Tests.push_back(TTYPE(&FC,&FD,1,std::string("Inside plane crossing")));
  Tests.push_back(TTYPE(&FC,&FDprime,-1,std::string("Long cross plane")));
  Tests.push_back(TTYPE(&FC,&FE,-1,std::string("Touching sides")));
  Tests.push_back(TTYPE(&FC,&FF,-1,std::string("Part touching sides")));
  Tests.push_back(TTYPE(&FC,&FG,-1,std::string("Point touching sides")));

  int cnt(1);
  for(const TTYPE& tc : Tests)
    {
      const faceObj& GA= *(std::get<0>(tc));
      const faceObj& GB= *(std::get<1>(tc));
      const int res= std::get<2>(tc);
      const std::string& Info= std::get<3>(tc);
      
      if (GA->Intersect(*GB)!=res || 
       	  GB->Intersect(*GA)!=res )
       	{
       	  ELog::EM<<Info<<ELog::endCrit;
       	  ELog::EM<<"Test="<<cnt<<ELog::endCrit;
       	  ELog::EM<<"A/B == "<<GA->Intersect(*GB)<<" ["
       		  <<res<<"]"<<ELog::endCrit;
       	  ELog::EM<<"B/A == "<<GB->Intersect(*GA)<<" ["
       		  <<res<<"]"<<ELog::endCrit;
       	  return -cnt;
       	}
      cnt++;
    }
  return 0;
}

int
testFace::testPlanar()
  /*!
    Test simple interlocking / non interlocking triangles
    \retval -1 :: failed build a hull
    \retval 0 :: All passed
  */
{
  ELog::RegMethod RegItem("testFace","testSimpleHull");

  faceObj FA(10,Vec3D(0,0,0),Vec3D(1,1,0),Vec3D(1,-1,0));

  faceObj FB(20,Vec3D(10,0,0),Vec3D(10,1,0),Vec3D(10,-1,0));
  // this intersect FA
  faceObj FC(30,Vec3D(0.5,0,0),Vec3D(0.8,1,0),Vec3D(0.8,-1,0));

  // Outside edge touch
  faceObj FD(40,Vec3D(-1,-1,0),Vec3D(1,-1,0),Vec3D(0,1,0));
  faceObj FE(50,Vec3D(-1,1,0),Vec3D(1,1,0),Vec3D(0,8,0));
  // This is a star around F/G
  faceObj FF(60,Vec3D(-1,-1,0),Vec3D(1,-1,0),Vec3D(0,1,0));
  faceObj FG(70,Vec3D(-1,1,0),Vec3D(1,1,0),Vec3D(0,-2,0));
  // This is an edge touch around H/J
  faceObj FH(80,Vec3D(-1,-1,0),Vec3D(1,-1,0),Vec3D(0,1,0));
  faceObj FJ(90,Vec3D(-1,-1,0),Vec3D(0,-1,0),Vec3D(0,-8,0));


  typedef std::tuple<const faceObj*,const faceObj*,
		       int,std::string> TTYPE;
  std::vector<TTYPE> Tests;
  Tests.push_back(TTYPE(&FA,&FB,-1,std::string("A not touching B")));
  Tests.push_back(TTYPE(&FA,&FC,1,std::string("A inside B")));
  Tests.push_back(TTYPE(&FD,&FE,-1,std::string("A touch point B")));
  Tests.push_back(TTYPE(&FF,&FG,1,std::string("A star B")));
  Tests.push_back(TTYPE(&FH,&FJ,-1,std::string("A touch line B")));

  int cnt(1);
  for(const TTYPE& tc : Tests)
    {
      const faceObj& GA= *(std::get<0>(tc));
      const faceObj& GB= *(std::get<1>(tc));
      const int res= std::get<2>(tc);
      const std::string& Info= std::get<3>(tc);
      
      if (GA->Intersect(*GB)!=res || 
       	  GB->Intersect(*GA)!=res )
       	{
       	  ELog::EM<<Info<<ELog::endCrit;
       	  ELog::EM<<"Test="<<cnt<<ELog::endCrit;
       	  ELog::EM<<"A/B == "<<GA->Intersect(*GB)<<" ["
       		  <<res<<"]"<<ELog::endCrit;
       	  ELog::EM<<"B/A == "<<GB->Intersect(*GA)<<" ["
       		  <<res<<"]"<<ELog::endCrit;
       	  return -cnt;
       	}
      cnt++;
    }
   
  return 0;
}

