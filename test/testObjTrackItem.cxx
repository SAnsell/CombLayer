/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   test/testObjTrackItem.cxx
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
#include "ObjTrackItem.h"

#include "testFunc.h"
#include "testObjTrackItem.h"

using namespace ModelSupport;

testObjTrackItem::testObjTrackItem() 
  /*!
    Constructor
  */
{
  initSim();
}

testObjTrackItem::~testObjTrackItem() 
  /*!
    Destructor
  */
{}

void
testObjTrackItem::initSim()
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
testObjTrackItem::createSurfaces()
  /*!
    Create the surface list
   */
{
  ELog::RegMethod RegA("testObjTrackItem","createSurfaces");

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
testObjTrackItem::createObjects()
  /*!
    Create Object for test
   */
{
  ELog::RegMethod RegA("testObjTrackItem","createObjects");

  std::string Out;
  int cellIndex(1);
  const int surIndex(0);
  Out=ModelSupport::getComposite(surIndex,"100");
  ASim.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));  // Outside void Void
  MonteCarlo::Object* OPtr=ASim.findQhull(cellIndex-1);
  OPtr->setImp(0);

  Out=ModelSupport::getComposite(surIndex,"1 -2 3 -4 5 -6");
  ASim.addCell(MonteCarlo::Qhull(cellIndex++,3,0.0,Out));    // steel object

  Out=ModelSupport::getComposite(surIndex,"11 -12 13 -14 15 -16"
                                          " (-1:2:-3:4:-5:6) ");
  
  ASim.addCell(MonteCarlo::Qhull(cellIndex++,5,0.0,Out));      // Al container

  Out=ModelSupport::getComposite(surIndex,"21 -22 3 -4 5 -6");
  ASim.addCell(MonteCarlo::Qhull(cellIndex++,8,0.0,Out));      // Gd box 

  Out=ModelSupport::getComposite(surIndex,"-100 (-11:12:-13:14:-15:16) #4");
  ASim.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));      // Void
  
  ASim.removeComplements();

  return;
}
  

int 
testObjTrackItem::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: Test number to run
    \retval -1 : SetObject 
    \retval 0 : All succeeded
  */
{
  ELog::RegMethod RegA("testObjTrackItem","applyTest");
  TestFunc::regSector("testObjTrackItem");

  typedef int (testObjTrackItem::*testPtr)();
  testPtr TPtr[]=
    {
      &testObjTrackItem::testTrackNeutron
    };
  const std::string TestName[]=
    {
      "TrackNeutron"
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
testObjTrackItem::testTrackNeutron()
  /*!
    Tracks a neutron through the system
    \return 0 on success and -1 on error
  */
{
  ELog::RegMethod RegA("testObjTrackItem","testTrackNeutron");

  typedef std::tuple<size_t,size_t,size_t> TTYPE;
  typedef std::tuple<size_t,int,double> RTYPE;

  // Test neutrons
  const std::vector<MonteCarlo::neutron> TNeut=
    {
      MonteCarlo::neutron(10,Geometry::Vec3D(0,0,0),Geometry::Vec3D(1,0,0))
    };
    
  // Results [number / sum Mat / length sum]
  const std::vector<RTYPE> Results=
    {
      RTYPE(3,8,3.0)
    };
  
  // Tests [ neutron : StartVec3D : EndVec3D ]
  const std::vector<TTYPE> TestIndex=
    {
      TTYPE(0,0,1)
    };
  
  for(size_t index=0;index<TestIndex.size();index++)
    {
      const TTYPE& tc(TestIndex[index]);

      MonteCarlo::neutron nOut(TNeut[std::get<0>(tc)]);
      ObjTrackItem OA(nOut.Pos,nOut.uVec);
			    
      LineTrack A(nOut.Pos,nOut.uVec,8.0);
      A.calculate(ASim);

      const std::vector<MonteCarlo::Object*>& OVec=A.getObjVec();
      const std::vector<double>& TVec=A.getTrack();
      for(size_t i=0;i<OVec.size();i++)
	{
	  const MonteCarlo::Object* OPtr=OVec[i];
	  if (!OPtr)
	    {
	      ELog::EM<<"No object for point "<<i<<ELog::endDiag;
	      return -1;
	    }
	  OA.addDistance(OPtr->getMat(),TVec[i]);
	}

      const int errFlag=
	checkResult(OA,Results[index]);

      if (errFlag) return errFlag;       
    }

  return 0;            
}


int
testObjTrackItem::checkResult(const ObjTrackItem& OA,
	      const std::tuple<size_t,int,double>& Result) const
  /*!
    Checks a result
    \param OA :: Object item to compare
    \param Result :: 
    - int:number of units:
    - int:Sum of materials 
    - double:length in non-zero material
    \return -ve on error / 0 on success
   */
{
  ELog::RegMethod RegA("trackObjTrackItem","checkResult");

  int resFlag(0);
  const std::map<int,double>& MT=OA.getTrack();
  const size_t size=MT.size();
  if (std::get<0>(Result)!=size)
    resFlag=1;

  int sumMat(0);
  double sumDist(0.0);
  std::map<int,double>::const_iterator mc;
  for(mc=MT.begin();mc!=MT.end();mc++)
    {
      sumMat+=mc->first;
      sumDist+=(mc->first) ? mc->second : 0;
    }
  if (sumMat!=std::get<1>(Result)) 
    resFlag+=2;

  if (std::abs(sumDist-std::get<2>(Result))>1e-4) 
    resFlag+=4;
  
  if (resFlag)
    {
      ELog::EM<<"Size     ["<<std::get<0>(Result)<<
	"] == "<<size<<ELog::endWarn;
      ELog::EM<<"Sum Mat  ["<<std::get<1>(Result)<<
	"] == "<<sumMat<<ELog::endWarn;
      ELog::EM<<"Sum Dist ["<<std::get<2>(Result)<<
	"] == "<<sumDist<<ELog::endWarn;
    }
  return -resFlag;
}
