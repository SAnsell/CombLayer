/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   test/testSimulation.cxx
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
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "mathSupport.h"
#include "support.h"
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
#include "ModeCard.h"
#include "PhysCard.h"
#include "PhysImp.h"
#include "LSwitchCard.h"
#include "KGroup.h"
#include "SrcData.h"
#include "SrcItem.h"
#include "Source.h"
#include "KCode.h"
#include "PhysicsCards.h"
#include "ReadFunctions.h"
#include "surfRegister.h"
#include "ModelSupport.h"
#include "neutron.h"
#include "Simulation.h"

#include "testFunc.h"
#include "testSimulation.h"

testSimulation::testSimulation() 
  /*!
    Constructor
  */
{
  initSim();
}

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
testSimulation::createObjects()
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

  typedef int (testSimulation::*testPtr)();
  testPtr TPtr[]=
    {
      &testSimulation::testCreateObjSurfMap,
      &testSimulation::testInCell,
      &testSimulation::testTrackNeutron
    };
  const std::string TestName[]=
    {
      "CreateObjSurfMap",
      "InCell",
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
testSimulation::testTrackNeutron()
  /*!
    Tracks a neutron through the system
    \return 0 on success and -1 on error
  */
{
  ELog::RegMethod RegA("testSimulation","testTrackNeutron");

  std::vector<MonteCarlo::neutron> TNeut;
  std::vector<int> CellInit;
  
  TNeut.push_back(MonteCarlo::neutron(10,Geometry::Vec3D(0,0,0),
				      Geometry::Vec3D(1,0,0)));

  
  // InitObj : Neut : dist, surfN
  typedef boost::tuple<int,size_t,double,int> tTYPE;
  std::vector<tTYPE> testIndex;
  testIndex.push_back(tTYPE(2,0,1.0,-2)); 
  
  for(size_t i=0;i<testIndex.size();i++)
    {
      const tTYPE& TI=testIndex[i];
      MonteCarlo::neutron nOut(TNeut[TI.get<1>()]);      

      // Find Initial cell
      MonteCarlo::Object* OPtr=
	ASim.findCell(nOut.Pos,0);
      if (!OPtr || OPtr->getName()!=TI.get<0>())
	{
	  ELog::EM<<"Init cell incorrect"<<ELog::endWarn;
	  return -1;
	}
      // Track to outgoing surface
      const Geometry::Surface* SPtr;          // Output surface
      double aDist;                           // Output distribution
      const int SN=OPtr->trackOutCell(nOut,aDist,SPtr);
      // Error check result:
  // InitObj : Neut : dist, surfN
      if (SN!=TI.get<3>() || fabs(TI.get<2>()-aDist)>1e-4)
	{
	  ELog::EM<<"Results == "<<i<<ELog::endWarn;
	  ELog::EM<<"Exit surf("<<TI.get<3>()
		  <<") == "<<SN<<ELog::endWarn;
	  ELog::EM<<"Init neut == "<<TNeut[TI.get<1>()]<<ELog::endWarn;
	  ELog::EM<<"Distance("<<TI.get<2>()<<") == "
		  <<aDist<<ELog::endWarn;
	  ELog::EM<<"------------------"<<ELog::endDebug;
	  return -1;
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
  
  ASim.createObjSurfMap();
  const ModelSupport::ObjSurfMap* OPtr=ASim.getOSM();
  if (!OPtr) return -1;
  const ModelSupport::ObjSurfMap::STYPE& MVec=OPtr->getObjects(2);

  if (MVec.size()!=1 || MVec[0]->getName()!=3)
    {
      ModelSupport::ObjSurfMap::STYPE::const_iterator mc;
      for(mc=MVec.begin();mc!=MVec.end();mc++)
	ELog::EM<<"Obj == "<<(*mc)->getName()<<ELog::endDiag;
      return -2;
    }

  const ModelSupport::ObjSurfMap::STYPE& MVecB=OPtr->getObjects(5);
  if (MVecB.size()!=2  || MVecB[0]->getName()!=2 ||
      MVecB[1]->getName()!=4)
    {
      ModelSupport::ObjSurfMap::STYPE::const_iterator mc;
      for(mc=MVecB.begin();mc!=MVecB.end();mc++)
	ELog::EM<<"Obj == "<<(*mc)->getName()<<ELog::endDiag;
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

  std::vector<Geometry::Vec3D> Pts;
  std::vector<int> CellN;
  
  Pts.push_back(Geometry::Vec3D(0,0,0));
  Pts.push_back(Geometry::Vec3D(0,26,0));
  Pts.push_back(Geometry::Vec3D(0,2,0));
  Pts.push_back(Geometry::Vec3D(12.5,0.3,0));
  Pts.push_back(Geometry::Vec3D(0,5,0));


  CellN.push_back(2);
  CellN.push_back(1);
  CellN.push_back(3);
  CellN.push_back(4);  
  CellN.push_back(5);

  for(size_t i=0;i<Pts.size();i++)
    {
      MonteCarlo::Object* OPtr=ASim.findCell(Pts[i],0);
      ELog::EM<<"Cell == "<<((OPtr) ? OPtr->getName() : 0)<<ELog::endWarn;
      if (OPtr && CellN[i]!=OPtr->getName())
	{
	  ELog::EM<<"Failed on point:"<<Pts[i]<<ELog::endWarn;
	  ELog::EM<<"  Cell == "<<CellN[i]<<" != "
		  <<*OPtr<<ELog::endDebug;
	  return -1;
	}
      else if (!OPtr && CellN[i])
	{
	  ELog::EM<<"Failed on point:"<<Pts[i]<<ELog::endWarn;
	  ELog::EM<<"  Cell == "<<CellN[i]<<" != Null"<<ELog::endWarn;
	  return -1;
	}
    }
      
  return 0;
}
