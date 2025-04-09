/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   test/testEllipse.cxx
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
#include <complex>
#include <cmath>
#include <list>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <tuple>
#include <random>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "Line.h"
#include "Intersect.h"
#include "Ellipse.h"

#include "testFunc.h"
#include "testEllipse.h"

using namespace Geometry;

testEllipse::testEllipse() 
  /*!
    Constructor
  */
{}

testEllipse::~testEllipse() 
  /*!
    Destructor
  */
{}

int 
testEllipse::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: index of test
    \retval -1 Distance failed
    \retval 0 All succeeded
  */
{
  ELog::RegMethod RegA("testEllipse","applyTest");

  TestFunc::regSector("testEllipse");
  typedef int (testEllipse::*testPtr)();
  testPtr TPtr[]=
    {
      &testEllipse::testLineIntercept,
      &testEllipse::testScaleFactor
    };
  const std::string TestName[]=
    {
      "lineIntercept",
      "ScaleFactor"
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
testEllipse::testLineIntercept() 
  /*!
    Test the scaleFactor
    \retval -1 :: failed build a cone
    \retval 0 :: All passed
  */
{
  ELog::RegMethod RegA("testEllipse","testLineIntercept");

  // ellIndex lineIndex :Result number : Point A / Point B
  typedef std::tuple
    <size_t,size_t,size_t,Geometry::Vec3D,Geometry::Vec3D> resTYPE;
  
  const std::vector<Geometry::Ellipse> ellTEST({
      {Geometry::Vec3D(0,0,0),Geometry::Vec3D(0,1.0,0),
       Geometry::Vec3D(0,0.0,1.0)}, // circle
      {Geometry::Vec3D(0,0,0),Geometry::Vec3D(0,1.0,0),
       Geometry::Vec3D(0,0.0,3.0)}, // orthogonal ellipse
      {Geometry::Vec3D(0,0,0),Geometry::Vec3D(0,1.0,2.0),
       Geometry::Vec3D(0,-0.5,3.0)} // non-orthogonal ellipse
    });
  
  const std::vector<Geometry::Line> lineTEST({
      {Geometry::Vec3D(0,0,0),Geometry::Vec3D(0,1.0,0)},
      {Geometry::Vec3D(2.0,0,0),Geometry::Vec3D(3.0,1.0,0.0)},
      {Geometry::Vec3D(0.0,0.5,0.5),Geometry::Vec3D(0.0,1.0,0.0)}
    });

  const std::vector<resTYPE> resTEST({
      {0,0,2,Geometry::Vec3D(0,-1.0,0),Geometry::Vec3D(0,1.0,0)},
      {0,1,2,Geometry::Vec3D(0,-1.0,0),Geometry::Vec3D(0,1.0,0)},
      {0,2,2,Geometry::Vec3D(0,-0.866025,0.5),Geometry::Vec3D(0,0.866025,0.5)},
      {1,0,2,Geometry::Vec3D(0,-1.0,0),Geometry::Vec3D(0,1.0,0)},
      {1,1,2,Geometry::Vec3D(0,-1.0,0),Geometry::Vec3D(0,1.0,0)},
      {1,2,2,Geometry::Vec3D(0,-0.866025,0.5),Geometry::Vec3D(0,0.866025,0.5)},
      {2,0,2,Geometry::Vec3D(0,-1.0,0),Geometry::Vec3D(0,1.0,0)},
      {2,1,2,Geometry::Vec3D(0,-1.0,0),Geometry::Vec3D(0,1.0,0)},
      {2,2,2,Geometry::Vec3D(0,-0.866025,0.5),Geometry::Vec3D(0,0.866025,0.5)}
    });


  int cnt(1);
  for(const resTYPE& tc : resTEST)
    {
      const size_t ellIndex(std::get<0>(tc));
      const size_t lineIndex(std::get<1>(tc));
      const size_t res(std::get<2>(tc));
      const Geometry::Vec3D& rPtA(std::get<3>(tc));
      const Geometry::Vec3D& rPtB(std::get<4>(tc));
      
      const Geometry::Ellipse& E(ellTEST[ellIndex]);
      const Geometry::Line& L(lineTEST[lineIndex]);

      Geometry::Vec3D APt,BPt;
      const size_t outIndex=E.lineIntercept(L,APt,BPt);
      // side return 0 if point is on side
      if (outIndex!=res ||
	  (res==2 && (E.side(APt) || E.side(BPt))))
	{
	  ELog::EM<<"Ellipse == "<<E<<ELog::endDiag;
	  ELog::EM<<"Line == "<<L<<ELog::endDiag;
	  ELog::EM<<"outIndex["<<res<<"] == "<<outIndex<<ELog::endDiag;
	  if (outIndex>0)
	    {
	      ELog::EM<<"APt["<<rPtA<<"] == "<<APt<<ELog::endDiag;
	      ELog::EM<<"BPt["<<rPtB<<"] == "<<BPt<<ELog::endDiag;
	      ELog::EM<<"Side = "<<E.side(APt)<<" "<<E.side(BPt)
		      <<ELog::endDiag;
	    }
	  return -1;
	}
      cnt++;
    }
  return 0;
}


int
testEllipse::testScaleFactor() 
  /*!
    Test the scaleFactor
    \retval -1 :: failed build a cone
    \retval 0 :: All passed
  */
{
  ELog::RegMethod RegA("testEllipse","testScaleFactor");

  // Centre : max axis : min axis :: testPt scalefactor
  typedef std::tuple<Geometry::Vec3D,Geometry::Vec3D,Geometry::Vec3D,
		     Geometry::Vec3D,double> TTYPE;
  const std::vector<TTYPE> Tests
    ({
     { Geometry::Vec3D(0,0,0),
       Geometry::Vec3D(0,1.0,0),Geometry::Vec3D(0,0.0,1.0),
       Geometry::Vec3D(0.0,1.0,1.0),std::sqrt(2.0)
     },
     { Geometry::Vec3D(0,0,0),
       Geometry::Vec3D(0,1.0,0),Geometry::Vec3D(0,0.0,1.0),
       Geometry::Vec3D(0.0,4.0,0.0),4.0
     },
     { Geometry::Vec3D(0,1.0,0),
       Geometry::Vec3D(0,1.0,0),Geometry::Vec3D(0,0.0,1.0),
       Geometry::Vec3D(0.0,4.0,0.0),3.0
     },
     { Geometry::Vec3D(0,0.0,0),
       Geometry::Vec3D(0,2.0,0),Geometry::Vec3D(0,0.0,1.0),
       Geometry::Vec3D(0.0,4.0,0.0),2.0
     },
     { Geometry::Vec3D(0,0.0,0),
       Geometry::Vec3D(0,2.0,0),Geometry::Vec3D(0,0.0,1.0),
       Geometry::Vec3D(0.0,4.0,3.0),3.60555
     }
     });
  

  int cnt(1);
  for(const TTYPE& tc : Tests)
    {
      const Geometry::Vec3D cent(std::get<0>(tc));
      const Geometry::Vec3D majAxis(std::get<1>(tc));
      const Geometry::Vec3D minAxis(std::get<2>(tc));
      const Geometry::Vec3D testPt(std::get<3>(tc));
      const double res(std::get<4>(tc));
      
      const Geometry::Ellipse CX(cent,majAxis,minAxis);
      const double r=CX.scaleFactor(testPt);
      Geometry::Ellipse CY(CX);
      CY.scale(r);
      
      if (std::abs(r-res)>1e-5 || CY.side(testPt)!=0)
	{
	  ELog::EM<<"Test == "<<cnt<<ELog::endDiag;
	  ELog::EM<<"CX == "<<CX<<ELog::endDiag;
	  ELog::EM<<"CY == "<<CY<<ELog::endDiag;
	  ELog::EM<<"CYside == "<<CY.side(testPt)<<ELog::endDiag;
	  ELog::EM<<"result["<<res<<"] == "<<r<<ELog::endDiag;
	}
      cnt++;
    }
  return 0;
}
