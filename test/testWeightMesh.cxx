/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   test/testWeightMesh.cxx
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
#include "doubleErr.h"
#include "WeightMesh.h"

#include "testFunc.h"
#include "testWeightMesh.h"


testWeightMesh::testWeightMesh() 
  /*!
    Constructor
  */
{}

testWeightMesh::~testWeightMesh() 
  /*!
    Destructor
  */
{}

int 
testWeightMesh::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: index of test
    \retval -1 Distance failed
    \retval 0 All succeeded
  */
{
  ELog::RegMethod RegA("testWeightMesh","applyTest");
  TestFunc::regSector("testWeightMesh");
  
  typedef int (testWeightMesh::*testPtr)();
  testPtr TPtr[]=
    {
      &testWeightMesh::testPoint
    };
  const std::string TestName[]=
    {
      "Point"
    };
  const int TSize(sizeof(TPtr)/sizeof(testPtr));
  if (!extra)
    {
      TestFunc::Instance().reportTest(std::cout);
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
testWeightMesh::createXYZ(WeightSystem::WeightMesh& WMesh,
                          const std::string& XVal,
                          const std::string& YVal,
                          const std::string& ZVal) const
  /*!
    Create an XYZ mesh
    \param WMesh :: Weight mesh to add XYZ grid to 
    \param XVal :: String of x coordinates
    \param YVal :: String of y coordinates
    \param ZVal :: String of z coordinates
  */
{
  ELog::RegMethod RegA("testWeightMesh","createXYZ");

  
  std::string XYZ[3]={XVal,YVal,ZVal};

  std::vector<std::vector<double>> boundaryVal(3);
  std::vector<std::vector<size_t>> bCnt(3);
  
  for(size_t index=0;index<3;index++)
    {
      size_t i(0);
      double V;
      while(StrFunc::section(XYZ[index],V))
        {
	  if (i % 2)   // Odd : Integer
	    bCnt[index].push_back(static_cast<size_t>(V));
	  else
	    boundaryVal[index].push_back(V);
          i++;
	}
    }
  WMesh.setMesh(boundaryVal[0],bCnt[0],
                boundaryVal[1],bCnt[1],
                boundaryVal[2],bCnt[2]);
  return;
}

int
testWeightMesh::testPoint()
  /*!
    Test and individual point
    \return -ve on error / 0 on success
   */
{
  ELog::RegMethod RegA("testWeightMesh","testSum");

  typedef std::tuple<std::string,std::string,std::string,
                     size_t,size_t,size_t,Geometry::Vec3D> TTYPE;
  std::vector<TTYPE> Tests=
    {
      TTYPE{"-120 3 120.0","0 3 240.0","-120 3 120",
            1,1,2,Geometry::Vec3D(-40,80,40)}
    };

  WeightSystem::WeightMesh A;

  int cnt(1);
  for(const TTYPE& tc : Tests)
    {
      createXYZ(A,std::get<0>(tc),std::get<1>(tc),std::get<2>(tc));
      const size_t i(std::get<3>(tc)),j(std::get<4>(tc)),
        k(std::get<5>(tc));
      const Geometry::Vec3D Pt=A.point(i,j,k);
      const Geometry::Vec3D Res=std::get<6>(tc);
      
      if (Pt.Distance(Res)>1e-5)
        {
          ELog::EM<<"Failed on test "<<cnt<<ELog::endDiag;
          ELog::EM<<"Index "<<i<<":"<<j<<":"<<k<<ELog::endDiag;
          ELog::EM<<"Point  "<<Pt<<ELog::endDiag;
          ELog::EM<<"Expect "<<Res<<ELog::endDiag;
          return -1;
        }
    }
  
  return 0;

}
