/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   test/testSimulation.cxx
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#include <tuple>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "surfIndex.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
#include "ObjSurfMap.h"
#include "surfRegister.h"
#include "ModelSupport.h"
#include "Process.h"
#include "mcnpDefPhysics.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "SimMCNP.h"

#include "testFunc.h"
#include "testSimulation.h"

testSimulation::testSimulation() 
  /*!
    Constructor
  */
{}

testSimulation::~testSimulation() 
  /*!
    Destructor
  */
{}

void
testSimulation::initSim()
  /*!
    Set all the objects in the simulation:
  */
{
  ASim.resetAll();
  createSurfaces();
  createObjects();
  return;
}

void 
testSimulation::createSurfaces()
  /*!
    Create the surface list
   */
{
  ELog::RegMethod RegA("testSimulation","createSurfaces");

  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();
  
  // First box :
  SurI.createSurface(11,"px -1");
  SurI.createSurface(12,"px 1");
  SurI.createSurface(13,"py -1");
  SurI.createSurface(14,"py 1");
  SurI.createSurface(15,"pz -1");
  SurI.createSurface(16,"pz 1");

  // Second box :
  SurI.createSurface(21,"px -3");
  SurI.createSurface(22,"px 3");
  SurI.createSurface(23,"py -3");
  SurI.createSurface(24,"py 3");
  SurI.createSurface(25,"pz -3");
  SurI.createSurface(26,"pz 3");

  SurI.createSurface(27,"c/y 2.0 0.0 0.5");
  SurI.createSurface(44,"py 10.0");
  // Far box :
  SurI.createSurface(31,"px 10");
  SurI.createSurface(32,"px 15");

  // Sphere :
  SurI.createSurface(100,"so 25");
  
  return;
}
  
void
testSimulation::createObjects()
  /*!
    Create Object for test
   */
{
  std::string Out;
  int cellIndex(2);
  const int surIndex(0);
     
  Out=ModelSupport::getComposite(surIndex,"-1 100 ");
  ASim.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));
  // Outside void Void

  Out=ModelSupport::getComposite(surIndex,"11 -12 13 -14 15 -16");
  ASim.addCell(MonteCarlo::Object(cellIndex++,3,0.0,Out));      // steel object

  Out=ModelSupport::getComposite(surIndex,"21 -22 (-27:-24) 23 -44 25 -26 "
				 " (-11:12:-13:14:-15:16) ");
  ASim.addCell(MonteCarlo::Object(cellIndex++,5,0.0,Out));      // Al container

  Out=ModelSupport::getComposite(surIndex,"31 -32 13 -14 15 -16");
  ASim.addCell(MonteCarlo::Object(cellIndex++,8,0.0,Out));      // Gd box 

  Out=ModelSupport::getComposite
    (surIndex,"-100 (-21:22:-23:44:-25:26:(27 24)) #5");

  ASim.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));      // Void  
  ASim.removeComplements();

  return;
}
  

int 
testSimulation::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: Test number to run
    \retval -1 : SetObject 
    \retval 0 : All succeeded
  */
{
  ELog::RegMethod RegA("testSimulation","applyTest");
  TestFunc::regSector("testSimulation");
  
  typedef int (testSimulation::*testPtr)();
  testPtr TPtr[]=
    {
      &testSimulation::testCreateObjSurfMap,
      &testSimulation::testInCell,
      &testSimulation::testSplitCell
    };
  const std::string TestName[]=
    {
      "CreateObjSurfMap",
      "InCell",
      "SplitCell"
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
testSimulation::testCreateObjSurfMap()
  /*!
    test the createation of the surface map
    \return -1 on failure
  */
{
  ELog::RegMethod RegA("testSimulation","testCreateObjSurfMap");

  initSim();
  ASim.createObjSurfMap();
  const ModelSupport::ObjSurfMap* OPtr=ASim.getOSM();
  if (!OPtr) return -1;
  
  const ModelSupport::ObjSurfMap::STYPE& MVec=OPtr->getObjects(12);
  if (MVec.size()!=1 || MVec[0]->getName()!=4)
    {
      ELog::EM<<"Size == "<<MVec.size()<<" expected : 1 "<<ELog::endDiag;
      for(const MonteCarlo::Object* mc : MVec)
	ELog::EM<<"Obj[12] == "<<mc->getName()<<ELog::endDiag;
	    
      return -2;
    }

  const ModelSupport::ObjSurfMap::STYPE& MVecB=
    OPtr->getObjects(-16);
  if (MVecB.size()!=2  || MVecB[0]->getName()!=3 ||
      MVecB[1]->getName()!=5)
    {
      ELog::EM<<"Size(16) == "<<MVecB.size()<<" expected : 2 "<<ELog::endDiag;

      
      for(const MonteCarlo::Object* mc : MVecB)
	ELog::EM<<"Obj[-16]== "<<mc->getName()<<ELog::endDiag;
      return -3;
    }

  return 0;  
}

int
testSimulation::testInCell()
  /*!
    Test the point is a which cell
    \retval 0 :: success
  */
{
  ELog::RegMethod RegA("testSimulation","testInCell");

  // Point :: cell to expect
  typedef std::tuple<Geometry::Vec3D,int> TTYPE;
  const std::vector<TTYPE> Tests=
    {
      TTYPE(Geometry::Vec3D(0,0,0),3),
      TTYPE(Geometry::Vec3D(0,26,0),2),
      TTYPE(Geometry::Vec3D(0,2,0),4),
      TTYPE(Geometry::Vec3D(12.5,0.3,0),5),
      TTYPE(Geometry::Vec3D(0,5,0),6)
    };

  for(const TTYPE& tc : Tests)
    {
      const Geometry::Vec3D& Pt=std::get<0>(tc);
      const int CN(std::get<1>(tc));
      MonteCarlo::Object* OPtr=ASim.findCell(Pt,0);

      if (OPtr && CN!=OPtr->getName())
	{
	  ELog::EM<<"Failed on point: "<<Pt<<ELog::endWarn;
	  ELog::EM<<"  Cell == "<<CN<<" != "
		  <<*OPtr<<ELog::endDebug;
	  return -1;
	}
      else if (!OPtr && CN)
	{
	  ELog::EM<<"Failed on point:"<<Pt<<ELog::endWarn;
	  ELog::EM<<"  Cell == "<<CN<<" != Null"<<ELog::endWarn;
	  return -1;
	}
    }
      
  return 0;
}

int
testSimulation::testSplitCell()
  /*!
    Framework test to test splitting of a cell
    \return 0 on success / -1 on failure
  */
{
  ELog::RegMethod RegA("testSimulation","testSplitCell");

  // divide plane : cell : OutName "
  typedef std::tuple<std::string,int,std::string> TTYPE;
  const std::vector<TTYPE> Tests=
    {
      //      TTYPE("px 0",2,"testA.x"),
      //      TTYPE("px 0",3,"testB.x"),
      TTYPE("py 0",3,"testC.x")
    };

  ModelSupport::surfIndex& SurI=
    ModelSupport::surfIndex::Instance();
  
  for(const TTYPE& tc : Tests)
    {
      initSim();
      SurI.createSurface(1001,std::get<0>(tc));

      ASim.splitObject(std::get<1>(tc),std::get<1>(tc)+100,1001);
      mcnpSystem::setGenericPhysics(ASim,"CEM03");
      ASim.prepareWrite();
      ASim.write(std::get<2>(tc));
    }
  
  return 0;
}
