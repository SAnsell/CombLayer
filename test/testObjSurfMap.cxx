/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   test/testObjSurfMap.cxx
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
#include <sstream>
#include <cmath>
#include <complex>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <stack>
#include <string>
#include <algorithm>
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
#include "Quaternion.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Rules.h"
#include "Debug.h"
#include "BnId.h"
#include "Acomp.h"
#include "Algebra.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "surfIndex.h"
#include "ObjSurfMap.h"

#include "testFunc.h"
#include "testObjSurfMap.h"

using namespace ModelSupport;

testObjSurfMap::testObjSurfMap()
  /*!
    Constructor
  */
{
  createSurfaces();
}

testObjSurfMap::~testObjSurfMap() 
  /*!
    Destructor
  */
{}

int 
testObjSurfMap::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: Index of test
    \returns -ve on error 0 on success.
  */
{
  ELog::RegMethod RegA("testObjSurfMap","applyTest");
  TestFunc::regSector("testObjSurfMap");

  typedef int (testObjSurfMap::*testPtr)();
  testPtr TPtr[]=
    {
      &testObjSurfMap::testMap
    };

  const std::string TestName[]=
    {
      "Map"
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

void 
testObjSurfMap::createSurfaces()
  /*!
    Create the surface list
   */
{
  ELog::RegMethod RegA("testObjSurfMap","createSurfaces");

  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();
  SurI.reset();
	     
  // First box :
  SurI.createSurface(1,"px -1");
  SurI.createSurface(2,"px 1");
  SurI.createSurface(3,"py -1");
  SurI.createSurface(4,"py 1");
  SurI.createSurface(5,"pz -1");
  SurI.createSurface(6,"pz 1");


  return;
}

int
testObjSurfMap::testMap()
  /*!
    Test the maping of surface
    \returns 0 on succes and -ve on failure
  */
{
  ELog::RegMethod RegA("testObjSurfMap","testMap");
  
  // Create Objects:
  std::vector<std::shared_ptr<MonteCarlo::Object> > OVec;
  OVec.push_back(std::shared_ptr<MonteCarlo::Object>
		 (new MonteCarlo::Object(1,1,0.1,"1 -2 3 4")));
  OVec.push_back(std::shared_ptr<MonteCarlo::Object>
		 (new MonteCarlo::Object(1,1,0.1,"1 2 3 4")));
  OVec.push_back(std::shared_ptr<MonteCarlo::Object>
		 (new MonteCarlo::Object(2,1,0.1,"1 2 3 4")));
  OVec.push_back(std::shared_ptr<MonteCarlo::Object>
		 (new MonteCarlo::Object(3,1,0.1,"1 2 3 4")));

  for(std::shared_ptr<MonteCarlo::Object>& OPtr : OVec)
    OPtr->createSurfaceList();

  ObjSurfMap OM;

  OM.addSurfaces(OVec[0].get());
  OM.addSurfaces(OVec[1].get());
  OM.addSurfaces(OVec[2].get());
  
  MonteCarlo::Object* OPtr = OM.getObj(2,0);
  
  if (OPtr!=OVec[1].get())
    {
      ELog::EM<<"Failed on 2,0 : "<<ELog::endWarn;
      if (OPtr)
	ELog::EM<<"Object == "<<*OPtr<<ELog::endWarn;
      return -1;
    }

  OPtr = OM.getObj(2,1);
  if (OPtr!=OVec[2].get())
    {
      ELog::EM<<"Failed on 2,1 : "<<ELog::endWarn;
      return -2;
    }
  
  return 0;
}




