/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   test/testObjectTrackAct.cxx
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
#include <boost/functional.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/array.hpp>


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
#include "BnId.h"
#include "Rules.h"
#include "surfIndex.h"
#include "neutron.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "ObjSurfMap.h"
#include "Simulation.h"
#include "surfRegister.h"
#include "ModelSupport.h"
#include "LineTrack.h"
#include "ObjectTrackAct.h"

#include "testFunc.h"
#include "testObjectTrackAct.h"

using namespace ModelSupport;

testObjectTrackAct::testObjectTrackAct() 
  /*!
    Constructor
  */
{
  initSim();
}

testObjectTrackAct::~testObjectTrackAct() 
  /*!
    Destructor
  */
{}

void
testObjectTrackAct::initSim()
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
testObjectTrackAct::createSurfaces()
  /*!
    Create the surface list
   */
{
  ELog::RegMethod RegA("testObjectTrackAct","createSurfaces");

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
testObjectTrackAct::createObjects()
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

  Out=ModelSupport::getComposite(surIndex,"11 -12 13 -14 15 -16"
				 " (-1:2:-3:4:-5:6) ");
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
testObjectTrackAct::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: Test number to run
    \retval -1 : SetObject 
    \retval 0 : All succeeded
  */
{
  ELog::RegMethod RegA("testObjectTrackAct","applyTest");
  TestFunc::regSector("testObjectTrackAct");

  typedef int (testObjectTrackAct::*testPtr)();
  testPtr TPtr[]=
    {
      &testObjectTrackAct::testPointDet
    };
  const std::string TestName[]=
    {
      "PointDet"
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
testObjectTrackAct::testPointDet()
  /*!
    Tracks a neutron through the system
    \return 0 on success and -1 on error
  */
{
  ELog::RegMethod RegA("testObjectTrackAct","testPointDet");

  ObjectTrackAct OA(Geometry::Vec3D(20,0,0));

  typedef boost::tuple<int,double>  TTYPE;
  std::vector<TTYPE> Tests;
  // Test neutrons
  Tests.push_back(TTYPE(2,8.0));
  Tests.push_back(TTYPE(3,8.0));
  
  // Calculate all cells
  ASim.calcAllVertex();

  const Simulation::OTYPE& Cells=ASim.getCells();
  Simulation::OTYPE::const_iterator vc;
  for(vc=Cells.begin();vc!=Cells.end();vc++)
    {
      if (!vc->second->isPlaceHold())
	OA.addUnit(ASim,vc->first,vc->second->getCofM());
    }

  std::vector<TTYPE>::const_iterator tc;
  for(tc=Tests.begin();tc!=Tests.end();tc++)
    {
      const double D=OA.getMatSum(tc->get<0>());
      if (fabs(D-tc->get<1>())>1e-4)
	{
	  ELog::EM<<"OA == "<<OA<<ELog::endTrace;
	  ELog::EM<<"Cell["<<tc->get<0>()<<"] "
		  <<tc->get<1>()<<" == "<<D<<ELog::endCrit;
	  return -1;
	}
    }
  
  return 0;
}

