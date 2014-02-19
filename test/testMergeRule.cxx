/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   test/testMergeRule.cxx
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#include <iterator>
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
#include "ReadFunctions.h"
#include "surfRegister.h"
#include "ModelSupport.h"
#include "neutron.h"
#include "Simulation.h"
#include "Token.h"
#include "surfDBase.h"
#include "pairBase.h"
#include "pairItem.h"
#include "mergeRule.h"
#include "surfDivide.h"
#include "surfCompare.h"

#include "testFunc.h"
#include "testMergeRule.h"

using namespace ModelSupport;

testMergeRule::testMergeRule()
  /*!
    Constructor
  */
{
  initSim();
}

testMergeRule::~testMergeRule() 
  /*!
    Destructor
  */
{}

void
testMergeRule::initSim()
  /*!
    Set all the objects in the simulation:
  */
{
  ELog::RegMethod RegA("testMergeRule","initSim");

  ASim.resetAll();
  createSurfaces();
  createObjects();
  ASim.createObjSurfMap();
  return;
}

void 
testMergeRule::createSurfaces()
  /*!
    Create the surface list
   */
{
  ELog::RegMethod RegA("testMergeRule","createSurfaces");

  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();
  
  // First box :
  SurI.createSurface(1,"px -1");
  SurI.createSurface(2,"px 1");
  SurI.createSurface(3,"py -1");
  SurI.createSurface(4,"py 1");
  SurI.createSurface(5,"pz -1");
  SurI.createSurface(6,"pz 1");

  // Second box  [corner intersect ]:
  SurI.createSurface(11,"px -3");
  SurI.createSurface(12,"px 3");
  SurI.createSurface(13,"py -3");
  SurI.createSurface(14,"py 3");
  SurI.createSurface(15,"pz -3");
  SurI.createSurface(16,"pz 3");
  Geometry::Plane Pn(17,0);
  Pn.setPlane(Geometry::Vec3D(-3,2,0),Geometry::Vec3D(-2,3,0),
	      Geometry::Vec3D(-3,2,1));
  SurI.insertSurface(Pn.clone());

  // Far box :
  SurI.createSurface(21,"px 10");
  SurI.createSurface(22,"px 15");

  // Sphere :
  SurI.createSurface(100,"so 25");
  
  return;
}

void
testMergeRule::createObjects()
  /*!
    Create Object for test
  */
{
  ELog::RegMethod RegA("testMergeRule","createObjects");

  std::string Out;
  int cellIndex(1);
  const int surIndex(0);
  Out=ModelSupport::getComposite(surIndex,"100");
  ASim.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));  // Outside void Void

  Out=ModelSupport::getComposite(surIndex,"1 -2 3 -4 5 -6");
  ASim.addCell(MonteCarlo::Qhull(cellIndex++,3,0.0,Out));      // steel object

  Out=ModelSupport::getComposite(surIndex,"11 -12 13 -14 15 -16 -17");
  ASim.addCell(MonteCarlo::Qhull(cellIndex++,3,0.0,Out));      // steel object

  return;
}

int 
testMergeRule::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: Index of test
    \returns -ve on error 0 on success.
  */
{
  typedef int (testMergeRule::*testPtr)();
  testPtr TPtr[]=
    {
      &testMergeRule::testBasicPair
    };

  const std::string TestName[]=
    {
      "BasicPair"
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
testMergeRule::testBasicPair()
  /*!
    Test a simple pair divide between two surfaces.
    \return -ve on error 
  */
{
  ELog::RegMethod RegA("testMergeRule","testBasicPair");
  typedef pairItem<Geometry::Plane,Geometry::Plane> PITYPE;

  mergeRule MA;
  
  MA.setInner("1");
  MA.setOuter("2");

  MA.setInnerReplace("$1"); 
  MA.setOuterReplace("-$2");
  
  PITYPE PA(1,2);
  MA.addRule(&PA);

  MonteCarlo::Qhull BaseObj(1,3,0.0,"1 -2 3 -4 5 -6");      // Base object
  BaseObj.populate(); // needed ??

  std::vector<Token> CellToken=BaseObj.cellVec();
  return 0;
}

int 
testMergeRule::checkSurfaceEqual
(const int SN,const std::string& surfStr) const
  /*!
    Test to see if the surfaces match expected
    \param SN :: Surface number
    \param surfStr :: Surface String [expected]
    \return -ve on error / 0 on success
  */
{
  ELog::RegMethod RegA("testMergeRule","checkSurfaceEqual");

  const ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();
  const Geometry::Surface* SPtr=
    Geometry::surfaceFactory::Instance().processLine(surfStr);
  if (!SPtr)
    {
      ELog::EM<<"Failed to create surface "<<surfStr<<ELog::endCrit;
      return -1;
    }
  const Geometry::Surface* TPtr=SurI.getSurf(SN);
  
  if (!TPtr)
    {
      ELog::EM<<"Failed to find surface number:"<<SN<<ELog::endCrit;
      return -1;
    }
  if (!ModelSupport::equalSurface(SPtr,TPtr))
    {
      ELog::EM<<"Surfaces not equal:"<<ELog::endCrit;
      ELog::EM<<"SPtr :"<<*SPtr<<ELog::endCrit;
      ELog::EM<<"TPtr :"<<*TPtr<<ELog::endCrit;
      return -1;
    }
    
  return 0;
}
