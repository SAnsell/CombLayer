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

  // Far box :
  SurI.createSurface(21,"px 10");
  SurI.createSurface(22,"px 15");

  // Sphere :
  SurI.createSurface(100,"so 25");
  
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

  Out=ModelSupport::getComposite(surIndex,"1 -2 3 -4 5 -6");
  ASim.addCell(MonteCarlo::Qhull(cellIndex++,3,0.0,Out));      // steel object

  Out=ModelSupport::getComposite(surIndex,"11 -12 13 -14 15 -16");
  ASim.addCell(MonteCarlo::Qhull(cellIndex++,5,0.0,Out));      // Al container

  Out=ModelSupport::getComposite(surIndex,"21 -22 3 -4 5 -6");
  ASim.addCell(MonteCarlo::Qhull(cellIndex++,8,0.0,Out));      // Gd box 

  Out=ModelSupport::getComposite(surIndex,"-100 (-11:12:-13:14:-15:16)"
				 " #4");
  ASim.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));      // Void
  
  ASim.removeComplements();

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
      &testContainedSpace::testBoundary
    };
  const std::string TestName[]=
    {
      "Boundary"
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


  ModelSupport::setGenericPhysics(ASim,"CEM03");
  ASim.prepareWrite();
  ASim.write("testCC1.x");

  typedef std::tuple<int,std::string>  TTYPE;
  std::vector<TTYPE> Tests=
    {
      TTYPE(3," 15 -12 11 -16 ")
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
  ModelSupport::setGenericPhysics(ASim,"CEM03");
  ASim.prepareWrite();
  ASim.write("testCD1.x");

  return 0;
}

