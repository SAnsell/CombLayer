/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   test/testLineTrack.cxx
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#include "LineTrack.h"
#include "Cone.h"

#include "testFunc.h"
#include "testLineTrack.h"

using namespace ModelSupport;

testLineTrack::testLineTrack() 
  /*!
    Constructor
  */
{}

testLineTrack::~testLineTrack() 
  /*!
    Destructor
  */
{}

void
testLineTrack::initSim()
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
testLineTrack::createSurfaces()
  /*!
    Create the surface list
   */
{
  ELog::RegMethod RegA("testLineTrack","createSurfaces");

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

  // Top box
  SurI.createSurface(36,"pz 8");

  SurI.createSurface(37,"cz 4");

  // Sphere :
  SurI.createSurface(100,"so 25");

  /// Special Cone surface
  Geometry::Cone* CX=SurI.createUniqSurf<Geometry::Cone>(1007);  
  CX->setCone(Geometry::Vec3D(0,1,0),Geometry::Vec3D(0,1,0),45.0);
  SurI.insertSurface(CX);

  return;
}
  
void
testLineTrack::createObjects()
  /*!
    Create Object for test
  */
{
  std::string Out;
  int cellIndex(2);
  const int surIndex(0);
  Out=ModelSupport::getComposite(surIndex," 100 ");
  ASim.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));      // Outside void Void

  Out=ModelSupport::getComposite(surIndex,"11 -12 13 -14 15 -16");
  ASim.addCell(MonteCarlo::Object(cellIndex++,3,0.0,Out));      // steel object

  Out=ModelSupport::getComposite(surIndex,"21 -22 23 -24 25 -26"
				 " (-11:12:-13:14:-15:16) ");
  ASim.addCell(MonteCarlo::Object(cellIndex++,5,0.0,Out));      // Al container

  Out=ModelSupport::getComposite(surIndex,"-37 26 -36");
  ASim.addCell(MonteCarlo::Object(cellIndex++,4,0.0,Out));      // CH4 container

  // Sphereical container
  Out=ModelSupport::getComposite(surIndex,"-100 (-21:22:-23:24:-25:26) "
                                        "(37 : -26 : 36)");
  ASim.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));  

  return;
}
  
  

int 
testLineTrack::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: Test number to run
    \retval -1 : SetObject 
    \retval 0 : All succeeded
  */
{
  ELog::RegMethod RegA("testLineTrack","applyTest");
  TestFunc::regSector("testLineTrack");

  typedef int (testLineTrack::*testPtr)();
  testPtr TPtr[]=
    {
      &testLineTrack::testLine
    };
  const std::string TestName[]=
    {
      "Line"
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
testLineTrack::testLine()
  /*!
    Tracks a neutron through the system
    \return 0 on success and -1 on error
  */
{
  ELog::RegMethod RegA("testLineTrack","testLine");

  // Point A : Point B : Sum of cellIDs 
  typedef std::tuple<Geometry::Vec3D,Geometry::Vec3D,int,double> TTYPE;

  std::vector<TTYPE> Tests;
  // First test:
  // 5 outer / 17 void / 5 cylinder / 2 box / 1 inner box
  Tests.push_back(TTYPE(Geometry::Vec3D(0,0,30),Geometry::Vec3D(0,0,0),
			15,118.0));
  // Can't track passed an unbound cell
  Tests.push_back(TTYPE(Geometry::Vec3D(0,0,0),Geometry::Vec3D(0,0,30),14,113.0));
  // 7 void / 2 box / 2 inner / 2 box / 5 cyl / 2 void
  Tests.push_back(TTYPE(Geometry::Vec3D(0,0,-10),Geometry::Vec3D(0,0,10),22,81.0));
  // Accross cyl (rad 4)   6 void / 8 cyl / 6 void
  Tests.push_back(TTYPE(Geometry::Vec3D(-10,0,6),Geometry::Vec3D(10,0,6),14,92.0));

  // Accross angle cylinder
  Tests.push_back(TTYPE(Geometry::Vec3D(-10,0,6),Geometry::Vec3D(10,0,6),14,92.0));


  int cnt(1);
  for(const TTYPE& tc : Tests)
    {
      LineTrack LT(std::get<0>(tc),std::get<1>(tc));
      LT.calculate(ASim);
      if (!checkResult(LT,std::get<2>(tc),std::get<3>(tc)))
	{
	  ELog::EM<<"Failed on test :"<<cnt<<ELog::endTrace;
	  ELog::EM<<LT<<ELog::endTrace;
	  return -1;
	}
      cnt++;
    }
  return 0;
}

int
testLineTrack::checkResult(const LineTrack& LT,
			   const long int CSum,
			   const double TSum) const
  /*!
    Check the result from the test
    \param LT :: LineTrack to test
    \param CSum :: Sum of cells
    \param TSum :: Sum of Track*CellIndex
    \return true if good
   */
{
  ELog::RegMethod RegA("testLineTrack","checkResults");

  const std::vector<long int>& cells=LT.getCells();
  const std::vector<double>& tLen=LT.getSegmentLen();
  const std::vector<MonteCarlo::Object*>& oVec=LT.getObjVec();
  long int cValue(0);
  double tValue(0.0);
  for(size_t i=0;i<cells.size();i++)
    {
      if (!oVec[i] || oVec[i]->getName()!=cells[i])
	return 0;
      cValue+=cells[i]-1;
      tValue+=tLen[i]*static_cast<double>(cells[i]-1);
    }  
  return (cValue!=CSum || std::abs(TSum-tValue)>1e-3) ? 0 : 1;
}
