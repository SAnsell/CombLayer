/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   test/testPipeLine.cxx
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
#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Triple.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "Rules.h"
#include "HeadRule.h"
#include "Object.h"
#include "BnId.h"
#include "Acomp.h"
#include "Algebra.h"
#include "Line.h"
#include "Qhull.h"
#include "NRange.h"
#include "NList.h"
#include "KGroup.h"
#include "Source.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "generateSurf.h"
#include "AttachSupport.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "pipeUnit.h"
#include "PipeLine.h"

#include "testFunc.h"
#include "testPipeLine.h"

using namespace ModelSupport;

testPipeLine::testPipeLine()
  /*!
    Constructor
  */
{
  initSim();
}

testPipeLine::~testPipeLine() 
  /*!
    Destructor
  */
{}

void
testPipeLine::initSim()
  /*!
    Set all the objects in the simulation:
  */
{
  ASim.resetAll();
  createSurfaces();
  createObjects();
  ASim.createObjSurfMap();
  return;
}

void 
testPipeLine::createSurfaces()
  /*!
    Create the surface list
   */
{
  ELog::RegMethod RegA("testPipeLine","createSurfaces");

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
testPipeLine::createObjects()
  /*!
    Create Object for test
   */
{
  ELog::RegMethod RegA("testPipeLine","createObjects");

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
testPipeLine::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: Index of test
    \returns -ve on error 0 on success.
  */
{
  ELog::RegMethod RegA("testPipeLine","applyTest");
  TestFunc::regSector("testPipeLine");

  typedef int (testPipeLine::*testPtr)();
  testPtr TPtr[]=
    {
      &testPipeLine::testBasic,
      &testPipeLine::testJoin
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
testPipeLine::testBasic()
  /*!
    Test to see if an isolated pipe can exist
    \returns 0 on succes and -ve on failure
  */
{
  ELog::RegMethod RegA("testPipeLine","testBasic");

  
  PipeLine AP("testPipe");
  AP.addPoint(Geometry::Vec3D(0,0,0));
  AP.addPoint(Geometry::Vec3D(0,0,10));
  AP.addRadius(0.4,7,0.0);
  AP.createAll(ASim);

  ASim.renumberAll();
  ASim.write("testBasic.x");
  return 0;
}

int
testPipeLine::testJoin()
  /*!
    Test to see if an joined pipe works
    \returns 0 on succes and -ve on failure
  */
{
  ELog::RegMethod RegA("testPipeLine","testJoin");
  
  PipeLine AP("testPipe");
  AP.addPoint(Geometry::Vec3D(0,0,0));
  AP.addPoint(Geometry::Vec3D(0,0,10));
  AP.addPoint(Geometry::Vec3D(10,0,10));
  AP.addPoint(Geometry::Vec3D(15,0,15));
  AP.addRadius(0.4,7,0.0);
  AP.createAll(ASim);

  ASim.renumberAll();
  ASim.write("testJoin.x");
  return 0;
}

