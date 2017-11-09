/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   test/testBoxLine.cxx
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
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>
#include <complex>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <vector>
#include <algorithm>
#include <functional>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "Rules.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "boxValues.h"
#include "boxUnit.h"
#include "BoxLine.h"

#include "testFunc.h"
#include "testBoxLine.h"

using namespace ModelSupport;

testBoxLine::testBoxLine()
  /*!
    Constructor
  */
{
  initSim();
}

testBoxLine::~testBoxLine() 
  /*!
    Destructor
  */
{}

void
testBoxLine::initSim()
  /*!
    Set all the objects in the simulation:
  */
{
  ELog::RegMethod RegA("testBoxLine","initSim");

  ASim.resetAll();
  createSurfaces();
  createObjects();
  ASim.createObjSurfMap();
  return;
}

void 
testBoxLine::createSurfaces()
  /*!
    Create the surface list
   */
{
  ELog::RegMethod RegA("testBoxLine","createSurfaces");

  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();
  
  // First box :
  SurI.createSurface(1,"px -1");
  SurI.createSurface(2,"px 1");
  SurI.createSurface(3,"py -1");
  SurI.createSurface(4,"py 1");
  SurI.createSurface(5,"pz -1");
  SurI.createSurface(6,"pz 1");

  // Second box :
  SurI.createSurface(11,"px -3");
  SurI.createSurface(12,"px 3");
  SurI.createSurface(13,"py -3");
  SurI.createSurface(14,"py 3");
  SurI.createSurface(15,"pz -3");
  SurI.createSurface(16,"pz 3");

  // Sphere :
  SurI.createSurface(100,"so 25");
  
  return;
}


void
testBoxLine::createObjects()
  /*!
    Create Object for test
   */
{
  ELog::RegMethod RegA("testBoxLine","createObjects");

  std::string Out;
  int cellIndex(1);
  const int surIndex(0);

  Out=ModelSupport::getComposite(surIndex,"100");
  ASim.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));      // Outside void Void
  // Inner box
  Out=ModelSupport::getComposite(surIndex,"1 -2 3 -4 5 -6");
  ASim.addCell(MonteCarlo::Qhull(cellIndex++,3,0.0,Out));
  

  // Container box:
  Out=ModelSupport::getComposite(surIndex,"11 -12 13 -14 15 -16"
				 " (-1:2:-3:4:-5:6) ");
  ASim.addCell(MonteCarlo::Qhull(cellIndex++,5,0.0,Out));      // Al container

  Out=ModelSupport::getComposite(surIndex,"-100 (-11:12:-13:14:-15:16)");
  ASim.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));      // Outside void Void
  return;
}

int 
testBoxLine::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: Index of test
    \returns -ve on error 0 on success.
  */
{
  ELog::RegMethod RegA("testBoxLine","applyTest");
  TestFunc::regSector("testBoxLine");

  typedef int (testBoxLine::*testPtr)();
  testPtr TPtr[]=
    {
      &testBoxLine::testBasic,
      &testBoxLine::testJoin
    };

  const std::string TestName[]=
    {
      "Basic",
      "Join"
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
testBoxLine::testBasic()
  /*!
    Test to see if an isolated boxline can exist
    \returns 0 on succes and -ve on failure
  */
{
  ELog::RegMethod RegA("testBoxLine","testBasic");
  
  BoxLine AP("testBox");
  AP.addPoint(Geometry::Vec3D(0,0,0));
  AP.addPoint(Geometry::Vec3D(0,0,10));
  AP.addSection(1.0,3.0,0,0.0);
  AP.setInitZAxis(Geometry::Vec3D(1,0,0));
  AP.createAll(ASim);

  ASim.renumberAll();
  ASim.write("testBasic.x");
  return 0;
}

int
testBoxLine::testJoin()
  /*!
    Test to see if an joined pipe works
    \returns 0 on succes and -ve on failure
  */
{
  ELog::RegMethod RegA("testBoxLine","testJoin");
  
  BoxLine AP("testBox");

  AP.addPoint(Geometry::Vec3D(0,0,0));
  AP.addPoint(Geometry::Vec3D(0,0,10));
  AP.addPoint(Geometry::Vec3D(10,0,10));
  AP.addPoint(Geometry::Vec3D(15,0,15));


  AP.addSection(0.3,0.7,0,0.0);
  AP.addSection(0.3,0.7,5,0.0);
  AP.setInitZAxis(Geometry::Vec3D(1,0,0));
  AP.createAll(ASim);

  ASim.write("testJoinA.x");
  ASim.renumberAll();
  ASim.write("testJoin.x");
  return 0;
}

int
testBoxLine::testZigZag()
  /*!
    Test to see if a zig-zag pipe works
    \returns 0 on succes and -ve on failure
  */
{
  ELog::RegMethod RegA("testBoxLine","testZigZag");
  
  BoxLine AP("testBox");

  AP.addPoint(Geometry::Vec3D(25.5,-14.0502,-48.0918));
  AP.addPoint(Geometry::Vec3D(24.5,-35.2634,-26.8786));
  AP.addPoint(Geometry::Vec3D(20.5,-35.2634,-26.8786));
  AP.addPoint(Geometry::Vec3D(19.5,-14.0502,-48.0918));
  AP.addPoint(Geometry::Vec3D(15.5,-14.0502,-48.0918));
  AP.addPoint(Geometry::Vec3D(14.5,-35.2634,-26.8786));
  AP.addPoint(Geometry::Vec3D(10.5,-35.2634,-26.8786));
  AP.addPoint(Geometry::Vec3D(9.5,-14.0502,-48.0918));  

  AP.addSection(0.3,0.7,0,0.0);
  AP.setInitZAxis(Geometry::Vec3D(1,0,0));
  AP.createAll(ASim);

  ASim.write("testJoinA.x");
  ASim.renumberAll();
  ASim.write("testJoin.x");
  return 0;
}

