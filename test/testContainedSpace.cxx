 /********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   test/testObjectTrackAct.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
# include <algorithm>
#include <functional>
#include <numeric>
#include <iterator>
#include <memory>
#include <tuple>


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
#include "varList.h"
#include "Code.h"
#include "FItem.h"
#include "FuncDataBase.h"
#include "Rules.h"
#include "surfIndex.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "ObjSurfMap.h"
#include "Simulation.h"
#include "SimMCNP.h"
#include "surfRegister.h"
#include "ModelSupport.h"
#include "DefPhysics.h"

#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "ContainedSpace.h"

#include "testFunc.h"
#include "testContainedSpace.h"

using namespace attachSystem;

testContainedSpace::testContainedSpace() 
  /*!
    Constructor
  */
{
  initSim();
}

testContainedSpace::~testContainedSpace() 
  /*!
    Destructor
  */
{}

void
testContainedSpace::initSim()
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
testContainedSpace::createSurfaces()
  /*!
    Create the surface list
   */
{
  ELog::RegMethod RegA("testContainedSpace","createSurfaces");

  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();
  

  // Second box :
  SurI.createSurface(1,"py -2");
  SurI.createSurface(2,"py 2");
  SurI.createSurface(3,"px -1");
  SurI.createSurface(4,"px 1");
  SurI.createSurface(5,"pz -1");
  SurI.createSurface(6,"pz 1");

  // Second box :
  SurI.createSurface(11,"py -30");
  SurI.createSurface(12,"py 30");
  SurI.createSurface(13,"px -30");
  SurI.createSurface(14,"px 30");
  SurI.createSurface(15,"pz -30");
  SurI.createSurface(16,"pz 30");


  // Sphere :
  SurI.createSurface(100,"so 200");
  
  return;
}
  
void
testContainedSpace::createObjects()
  /*!
    Create Object for test
  */
{
  std::string Out;
  int cellIndex(1);
  const int surIndex(0);
  Out=ModelSupport::getComposite(surIndex,"100");
  ASim.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));      // Outside void Void

  Out=ModelSupport::getComposite(surIndex,"11 -12 13 -14 15 -16");
  ASim.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));      // Al container

  Out=ModelSupport::getComposite(surIndex,
				 " -100 (-11 : 12 : -13 : 14 : -15 : 16)");
  ASim.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));      // Void
  

  return;
}
  

int 
testContainedSpace::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: Test number to run
    \retval -1 : SetObject 
    \retval 0 : All succeeded
  */
{
  ELog::RegMethod RegA("testContainedSpace","applyTest");
  TestFunc::regSector("testContainedSpace");

  typedef int (testContainedSpace::*testPtr)();
  testPtr TPtr[]=
    {
      &testContainedSpace::testBoundary,
      &testContainedSpace::testInsert
    };
  const std::string TestName[]=
    {
      "Boundary",
      "Insert"
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
testContainedSpace::testBoundary()
  /*!
    Tracks a neutron through the system
    \return 0 on success and -1 on error
  */
{
  ELog::RegMethod RegA("testContainedSpace","testBoundary");


  // ModelSupport::setGenericPhysics(ASim,"CEM03");
  // ASim.prepareWrite();
  // ASim.write("testCC1.x");

  typedef std::tuple<int,std::string>  TTYPE;
  std::vector<TTYPE> Tests=
    {
      TTYPE(2," -16 13 -14 15 ")
    };
  
  // Calculate all cells
  for(const TTYPE& tc : Tests)
    {
      ContainedSpace CSx;
      CSx.setConnect(0,Geometry::Vec3D(0,0,0),Geometry::Vec3D(0,-1,0));
      CSx.setLinkSurf(0,1);
      CSx.setConnect(1,Geometry::Vec3D(0,2,0),Geometry::Vec3D(0,1,0));
      CSx.setLinkSurf(1,2);
      CSx.calcBoundary(ASim,std::get<0>(tc),4);
      const std::string Out=CSx.getBBox().display();
      if( Out!=std::get<1>(tc))
	{
	  ELog::EM<<"Error on test cell:"<<std::get<0>(tc)<<ELog::endDiag;
	  ELog::EM<<"BB     -- "<<Out<<ELog::endDiag;
	  ELog::EM<<"Expect -- "<<std::get<1>(tc)<<ELog::endDiag;
	  return -1;
	}
    }

  return 0;
}


int
testContainedSpace::testInsert()
  /*!
    Tests placing a ContainedSpace in an object
    \return 0 on success and -1 on error
  */
{
  ELog::RegMethod RegA("testContainedSpace","testInsert");


  // ModelSupport::setGenericPhysics(ASim,"CEM03");
  // ASim.prepareWrite();
  // ASim.write("testCC1.x");

  //  (a) insert a "mini object" in model
  //  (b) add outer boundary to ContainedSpace and then allow division
  const int surIndex(0);
  std::string Out=ModelSupport::getComposite(surIndex,"1 -2 3 -4 5 -6");
  ASim.addCell(10,5,0.0,Out);      

  ContainedSpace CSx;
  CSx.addOuterSurf(Out);
  CSx.setConnect(0,Geometry::Vec3D(0,-2,0),Geometry::Vec3D(0,-1,0));
  CSx.setLinkSurf(0,-1);
  CSx.setConnect(1,Geometry::Vec3D(0,2,0),Geometry::Vec3D(0,1,0));
  CSx.setLinkSurf(1,2);
  CSx.setPrimaryCell(2);  // cell 2
  CSx.setBuildCell(20);  // cell 2

  CSx.addInsertCell(2);
  CSx.insertObjects(ASim);

  ModelSupport::setGenericPhysics(ASim,"CEM03");
  ASim.prepareWrite();
  ASim.write("testCC1.x");
  

  return 0;
}

