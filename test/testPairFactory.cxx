/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   test/testPairFactory.cxx
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
#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "mathSupport.h"
#include "support.h"
#include "version.h"
#include "MapSupport.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "Triple.h"
#include "NList.h"
#include "NRange.h"
#include "Tally.h"
#include "cellFluxTally.h"
#include "pointTally.h"
#include "heatTally.h"
#include "tallyFactory.h"
#include "Transform.h"
#include "Surface.h"
#include "surfIndex.h"
#include "Quadratic.h"
#include "Plane.h"
#include "surfaceFactory.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FItem.h"
#include "FuncDataBase.h"
#include "SurInter.h"
#include "BnId.h"
#include "Acomp.h"
#include "Algebra.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "RemoveCell.h"
#include "WForm.h"
#include "weightManager.h"
#include "ObjSurfMap.h"
#include "ObjTrackItem.h"
#include "SrcData.h"
#include "SrcItem.h"
#include "Source.h"
#include "ReadFunctions.h"
#include "surfRegister.h"
#include "ModelSupport.h"
#include "pairBase.h"
#include "pairItem.h"
#include "pairFactory.h"

#include "testFunc.h"
#include "testPairFactory.h"

using namespace ModelSupport;

testPairFactory::testPairFactory()
  /*!
    Constructor
  */
{
  initSim();
}

testPairFactory::~testPairFactory() 
  /*!
    Destructor
  */
{}

void
testPairFactory::initSim()
  /*!
    Set all the objects in the simulation:
  */
{
  ELog::RegMethod RegA("testPairFactory","initSim");

  createSurfaces();
  return;
}

void 
testPairFactory::createSurfaces()
  /*!
    Create the surface list
    - One of each so that 
   */
{
  ELog::RegMethod RegA("testPairFactory","createSurfaces");

  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();
  
  // First Surface
  SurI.createSurface(1,"px -1");
  SurI.createSurface(2,"so 1");
  SurI.createSurface(3,"cx 1");
  SurI.createSurface(4,"kx 1 1");
  // Second
  SurI.createSurface(11,"px 1");
  SurI.createSurface(12,"so 2");
  SurI.createSurface(13,"cx 2");
  SurI.createSurface(14,"kx 2 1");
  
  return;
}

int 
testPairFactory::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: Index of test
    \returns -ve on error 0 on success.
  */
{
  typedef int (testPairFactory::*testPtr)();
  testPtr TPtr[]=
    {
      &testPairFactory::testConstructPair
    };

  const std::string TestName[]=
    {
      "ConstructPair"
    };

  const int TSize(sizeof(TPtr)/sizeof(testPtr));
  if (!extra)
    {
      std::ios::fmtflags flagIO=std::cout.setf(std::ios::left);
      for(size_t i=0;i<TSize;i++)
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
testPairFactory::testConstructPair()
  /*!
    Test a simple pair divide between two surfaces.
    \return -ve on error 
  */
{
  ELog::RegMethod RegA("testPairFactory","testBasicPair");

  ModelSupport::surfIndex& SurI= ModelSupport::surfIndex::Instance();
  
  // SurfN:SurfN : Type [string]
  typedef boost::tuple<int,int,std::string> TTYPE;
  typedef pairItem<Geometry::Plane,Geometry::Plane> PTYPE;
  std::vector<TTYPE> Tests;
  Tests.push_back(TTYPE(1,11,"Plane Plane"));
  Tests.push_back(TTYPE(3,13,"Cylinder Cylinder"));
  Tests.push_back(TTYPE(2,12,"Sphere Sphere"));


  std::vector<TTYPE>::const_iterator tc;
  for(tc=Tests.begin();tc!=Tests.end();tc++)
    {
      const int surfA(tc->get<0>());
      const int surfB(tc->get<1>());
      
      const Geometry::Surface* SA=SurI.getSurf(surfA);
      const Geometry::Surface* SB=SurI.getSurf(surfB);

      boost::shared_ptr<pairBase> pBase=
	boost::shared_ptr<pairBase>(pairFactory::createPair(SA,SB));

      const std::string typeName=(pBase) ? pBase->typeINFO() : "No Pointer";
      if (typeName!=tc->get<2>())
	{
	  ELog::EM<<"Test : "<<1+(tc-Tests.begin())<<ELog::endDebug;
	  ELog::EM<<"Obtained : "<<typeName<<ELog::endDebug;
	  ELog::EM<<"Expected : "<<tc->get<2>()<<ELog::endDebug;
	  ELog::EM<<"SA == "<<SA->className()<<ELog::endDebug;
	  ELog::EM<<"SB == "<<SB->className()<<ELog::endDebug;
	  return -1;
	}
      
    }
  return 0;
}
