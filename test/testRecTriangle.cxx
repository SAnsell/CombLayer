/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   test/testRecTriangle.cxx
*
 * Copyright (c) 2004-2023 by Stuart Ansell
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
#include <queue>
#include <string>
#include <algorithm>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "dataSlice.h"
#include "multiData.h"
#include "Triangle.h"
#include "Quadrilateral.h"
#include "RecTriangle.h"
#include "MeshGrid.h"

#include "testFunc.h"
#include "testRecTriangle.h"

using namespace Geometry;

testRecTriangle::testRecTriangle() 
  /*!
    Constructor
  */
{}

testRecTriangle::~testRecTriangle() 
  /*!
    Destructor
  */
{}

int 
testRecTriangle::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: index of test
    \retval -1 Distance failed
    \retval 0 All succeeded
  */
{
  ELog::RegMethod RegA("testRecTriangle","applyTest");
  TestFunc::regSector("testRecTriangle");

  typedef int (testRecTriangle::*testPtr)();
  testPtr TPtr[]=
    {
      &testRecTriangle::testMeshGridPoints,
      &testRecTriangle::testQuadPoints,
      &testRecTriangle::testTriPoints
    };
  const std::string TestName[]=
    {
      "MeshGridPoints",
      "QuadPoints",
      "TriPoints"
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
testRecTriangle::testTriPoints()
  /*!
    Test the distance of a point from the plane
    \retval -1 :: failed build a plane
    \retval 0 :: All passed
  */
{
  ELog::RegMethod RegA("testRecTriangle","testPoints");

  Geometry::Vec3D O(0,0,0);
  Geometry::Vec3D Y(0,1,0);
  Geometry::Vec3D Z(0,0,1);
  // Centre point +/- Axis
  RecTriangle<Triangle> A(O+Y+Z,Y,Z);

  multiData<double> Out(200,200);
  for(int i=0;i<300;i++)
    {
      const Geometry::Vec3D Pt=A.getNext();
      //negative will collapse to >199
      const size_t yP=static_cast<size_t>(100.0*Pt.Y());
      const size_t zP=static_cast<size_t>(100.0*Pt.Z());
      if (yP>199 || zP>199)
	{
	  ELog::EM<<"Failed on point "<<i<<" "<<Pt<<ELog::endWarn;
	  ELog::EM<<"Pt = "<<yP<<" "<<zP<<ELog::endWarn;	  
	  return -1;
	}
      Out.get()[yP][zP]++;
    }
  
  std::ofstream OX("testPoint.img");
  for(size_t i=0;i<200;i++)
    for(size_t j=0;j<200;j++)
      {
	OX<<i<<" "<<j<<" "<<Out.get()[i][j]<<std::endl;
	if (Out.get()[i][j]>1)
	  {
	    ELog::EM<<"Failed on point "<<i<<" "<<j<<ELog::endTrace;
	    return -2;
	  }
      }

  OX.close();
  return 0;
}

int
testRecTriangle::testQuadPoints()
  /*!
    Test the dispersion of the points at each level
    (2^L) is the number of points
    \retval -1 :: failed build a plane
    \retval 0 :: All passed
  */
{
  ELog::RegMethod RegA("testRecTriangle","testPoints");

  const size_t gridSize(500);

  Geometry::Vec3D O(0,0.5,0.5);
  Geometry::Vec3D Y(0,0.5,0);
  Geometry::Vec3D Z(0,0,0.5);
  RecTriangle<Quadrilateral> A(O,Y,Z);

  multiData<double> Out(gridSize,gridSize);
  for(int i=0;i<20000;i++)
    {
      const int L=A.getLevel();
      const Geometry::Vec3D Pt=A.getNext();
      const size_t yP=static_cast<size_t>(gridSize*Pt.Y());
      const size_t zP=static_cast<size_t>(gridSize*Pt.Z());
      if (yP>=gridSize || zP>=gridSize)
	{
	  ELog::EM<<"Failed on point "<<Pt<<ELog::endWarn;
	  return -1;
	}
      if (L<=5)
	Out.get()[yP][zP]++;
    }
  
  std::ofstream OX("testPoint.img");
  for(size_t i=0;i<gridSize;i++)
    for(size_t j=0;j<gridSize;j++)
      OX<<i<<" "<<j<<" "<<Out.get()[i][j]<<std::endl;

  OX.close();
  return 0;
}



int
testRecTriangle::testMeshGridPoints()
  /*!
    Test the distance of a point from the plane
    \retval -1 :: failed build a plane
    \retval 0 :: All passed
  */
{
  ELog::RegMethod RegA("testRecTriangle","testPoints");

  const int gridSize(200);

  Geometry::Vec3D O(0,0.5,0.5);
  Geometry::Vec3D Y(0,1.0,0);
  Geometry::Vec3D Z(0,0,1.0);
  MeshGrid A(O,Y,Z);

  multiData<double> Out(gridSize,gridSize);
  for(size_t i=0;i<1089;i++)
    {
      const int L=A.getLevel();
      const Geometry::Vec3D Pt=A.getNext();
      const size_t yP=static_cast<size_t>(gridSize*Pt.Y()*0.999);
      const size_t zP=static_cast<size_t>(gridSize*Pt.Z()*0.999);
      if (yP>=gridSize || zP>=gridSize)
	{
	  ELog::EM<<"Failed on point "<<Pt<<" at level "<<L<<ELog::endWarn;
	  return -1;
	}
      Out.get()[yP][zP]++;
    }
  
  std::ofstream OX("testPoint.img");
  for(size_t i=0;i<gridSize;i++)
    for(size_t j=0;j<gridSize;j++)
      OX<<i<<" "<<j<<" "<<Out.get()[i][j]<<std::endl;

  OX.close();
  return 0;
}

