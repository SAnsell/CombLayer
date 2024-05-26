/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   test/testLineIntersect.cxx
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
#include "interPoint.h"
#include "Surface.h"
#include "surfIndex.h"
#include "Quadratic.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
#include "surfRegister.h"
#include "ModelSupport.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "SimMCNP.h"
#include "LineUnit.h"
#include "LineTrack.h"
#include "Line.h"
#include "LineIntersectVisit.h"

#include "testFunc.h"
#include "testLineIntersect.h"

using namespace ModelSupport;

testLineIntersect::testLineIntersect() 
  /*!
    Constructor
  */
{}

testLineIntersect::~testLineIntersect() 
  /*!
    Destructor
  */
{}

void
testLineIntersect::initSim()
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
testLineIntersect::createSurfaces()
  /*!
    Create the surface list
   */
{
  ELog::RegMethod RegA("testLineIntersect","createSurfaces");

  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();
  
  // First box :
  SurI.createSurface(11,"px -1");
  SurI.createSurface(12,"px 1");
  SurI.createSurface(13,"py -1");
  SurI.createSurface(14,"py 1");
  SurI.createSurface(15,"pz -1");
  SurI.createSurface(16,"pz 1");


  SurI.createSurface(7,"cz 0.02");
  SurI.createSurface(17,"cz 0.08");

  // Sphere :
  SurI.createSurface(100,"so 25");

  return;
}
  
void
testLineIntersect::createObjects()
  /*!
    Create Object for test
  */
{
  ELog::RegMethod RegA("testLineIntersect","createObjects");
  
  HeadRule HR;

  int cellIndex(2);
  const int surIndex(0);
 
  HR=ModelSupport::getHeadRule(surIndex,"15 -16 -17");
  ASim.addCell(cellIndex++,3,0.0,HR);

  return;
}
  
  

int 
testLineIntersect::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: Test number to run
    \retval -1 : SetObject 
    \retval 0 : All succeeded
  */
{
  ELog::RegMethod RegA("testLineIntersect","applyTest");
  TestFunc::regSector("testLineIntersect");

  typedef int (testLineIntersect::*testPtr)();
  testPtr TPtr[]=
    {
      &testLineIntersect::testInterPoint
    };
  const std::string TestName[]=
    {
      "InterPoint"
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
      initSim();
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
testLineIntersect::testInterPoint()
  /*!
    Tests a single track 
    \return 0 on success and -1 on error
  */
{
  ELog::RegMethod RegA("testLineIntersect","testInterPoint");

  // Point Org : Point Axis : Cell :: valid dist
  typedef std::tuple<Geometry::Vec3D,Geometry::Vec3D,int,double> TTYPE;

  const std::vector<TTYPE> Tests({
      //      TTYPE(Geometry::Vec3D(0,-0.07,0),Geometry::Vec3D(0,-1,0),2,0.08),
      TTYPE(Geometry::Vec3D(0.00505452,-0.0798402,-0.0104719),
	    Geometry::Vec3D(0,-1,0),2,0.08)
    });

  int cnt(1);

  initSim();


  for(const TTYPE& tc : Tests)
    {
      
      const int cellN=std::get<2>(tc);
      MonteCarlo::Object* OPtr=ASim.findObject(cellN);
      std::set<const Geometry::Surface*> SSet=
	OPtr->getHeadRule().getSurfaces();
      const Geometry::Vec3D Org=std::get<0>(tc);
      const Geometry::Vec3D VUnit=std::get<1>(tc);

      std::vector<Geometry::interPoint> IPTvec;
      MonteCarlo::LineIntersectVisit LI(Org,VUnit);
      for(const Geometry::Surface* SPtr : SSet)
	{
	  const std::vector<Geometry::interPoint>& SurfVec=
	    LI.getIntercept(SPtr);
	  for(const Geometry::interPoint& inter : SurfVec)
	    {
	      if (inter.D>Geometry::zeroTol)
		IPTvec.push_back(inter);
	      if (inter.SNum==17)
		ELog::EM<<"Inter == "<<inter<<ELog::endDiag;
	    }
	}
      Geometry::sortVector(IPTvec);
      if (!IPTvec.empty())
	ELog::EM<<"First == "<<IPTvec[0]<<ELog::endDiag;


      cnt++;
    }
  return 0;
}
