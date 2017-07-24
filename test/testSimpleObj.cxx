/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   test/testSimpleObj.cxx
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
#include <cmath>
#include <complex> 
#include <vector>
#include <list> 
#include <map> 
#include <set>
#include <string>
#include <algorithm>
#include <functional>
#include <numeric>
#include <iterator>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Surface.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FItem.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "ObjSurfMap.h"
#include "surfRegister.h"
#include "ModelSupport.h"
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "World.h"

#include "testFunc.h"
#include "simpleObj.h"
#include "testSimpleObj.h"


testSimpleObj::testSimpleObj() 
  /*!
    Constructor
  */
{
  initSim();
}

testSimpleObj::~testSimpleObj() 
  /*!
    Destructor
  */
{}

void
testSimpleObj::initSim()
  /*!
    Set all the objects in the simulation:
  */
{
  ASim.resetAll();
  return;
}
  

int 
testSimpleObj::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: Test number to run
    \retval -1 : SetObject 
    \retval 0 : All succeeded
  */
{
  ELog::RegMethod RegA("testSimpleObj","applyTest");
  TestFunc::regSector("testsimpleObj");
  
  typedef int (testSimpleObj::*testPtr)();
  testPtr TPtr[]=
    {
      &testSimpleObj::testCreateObj,
      &testSimpleObj::testRotateAngle
    };
  const std::string TestName[]=
    {
      "CreateObj",
      "RotateAngle"
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
testSimpleObj::testCreateObj()
  /*!
    test the createation of the surface map
    \return -1 on failure
  */
{
  ELog::RegMethod RegA("testSimpleObj","testCreateObj");

  initSim();
  World::createOuterObjects(ASim);
  
  testSystem::simpleObj A("simple");
  A.addInsertCell(74123);
  A.createAll(ASim,World::masterOrigin());

  testSystem::simpleObj B("simpleB");
  B.addInsertCell(74123);
  B.setOffset(Geometry::Vec3D(-10,0,0));
  B.setMat(5);
  B.setRefFlag(1);
  B.createAll(ASim,World::masterOrigin());

  std::vector<int> rOffset;
  std::vector<int> rRange;  
  ASim.renumberSurfaces(rOffset,rRange);
  ASim.renumberCells(rOffset,rRange);
  ASim.prepareWrite();
  ASim.write("simple.x");
  return 0;  
}

int
testSimpleObj::testRotateAngle()
  /*!
    Test the rotation of the object
    \return -1 on failure
  */
{
  ELog::RegMethod RegA("testSimpleObj","testAngleRotate");

  initSim();
  World::createOuterObjects(ASim);
  
  testSystem::simpleObj A("simpleBase");
  A.addInsertCell(74123);
  A.setMat(11);
  A.createAll(ASim,World::masterOrigin(),0,0,0);


  testSystem::simpleObj B("simpleRotate");
  B.addInsertCell(74123);
  B.setMat(3);
  B.setOffset(Geometry::Vec3D(0,50.0,0));
  B.createAll(ASim,World::masterOrigin(),0,45.0,0);

  testSystem::simpleObj C("simpleAngle");
  C.addInsertCell(74123);
  C.setMat(5);
  C.setOffset(Geometry::Vec3D(50.0,0.0,0));
  C.createAll(ASim,World::masterOrigin(),45.0,0,90.0);


  std::vector<int> rOffset;
  std::vector<int> rRange;  
  ASim.renumberSurfaces(rOffset,rRange);
  ASim.renumberCells(rOffset,rRange);
  ASim.prepareWrite();
  ASim.write("simple.x");
  return 0;  
}

