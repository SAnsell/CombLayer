/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   test/testSurfDivide.cxx
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
#include <sstream>
#include <cmath>
#include <complex>
#include <vector>
#include <map>
#include <set>
#include <list>
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
#include "MapSupport.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
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
#include "HeadRule.h"
#include "Object.h"
#include "surfRegister.h"
#include "ModelSupport.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "SimMCNP.h"
#include "surfDBase.h"
#include "mergeMulti.h"
#include "mergeTemplate.h"
#include "surfDivide.h"
#include "surfCompare.h"

#include "testFunc.h"
#include "testSurfDivide.h"

using namespace ModelSupport;

testSurfDivide::testSurfDivide()
  /*!
    Constructor
  */
{}

testSurfDivide::~testSurfDivide() 
  /*!
    Destructor
  */
{}

void
testSurfDivide::initSim()
  /*!
    Set all the objects in the simulation:
  */
{
  ELog::RegMethod RegA("testSurfDivide","initSim");

  ASim.resetAll();
  createSurfaces();
  createObjects();
  ASim.createObjSurfMap();
  return;
}

void 
testSurfDivide::createSurfaces()
  /*!
    Create the surface list
   */
{
  ELog::RegMethod RegA("testSurfDivide","createSurfaces");

  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();
  SurI.reset();
  // First box :
  SurI.createSurface(11,"px -1");
  SurI.createSurface(12,"px 1");
  SurI.createSurface(13,"py -1");
  SurI.createSurface(14,"py 1");
  SurI.createSurface(15,"pz -1");
  SurI.createSurface(16,"pz 1");
  Geometry::Plane Pn(17,0);
  Pn.setPlane(Geometry::Vec3D(-0.5,1,0),Geometry::Vec3D(-1,0.5,0),
	      Geometry::Vec3D(-0.5,1,1),Geometry::Vec3D(0,1,0));
  SurI.insertSurface(Pn.clone());

  // Second box  [corner intersect ]:
  SurI.createSurface(21,"px -3");
  SurI.createSurface(22,"px 3");
  SurI.createSurface(23,"py -3");
  SurI.createSurface(24,"py 3");
  SurI.createSurface(25,"pz -3");
  SurI.createSurface(26,"pz 3");
  Geometry::Plane PnX(27,0);
  PnX.setPlane(Geometry::Vec3D(-3,2,0),Geometry::Vec3D(-2,3,0),
	      Geometry::Vec3D(-3,2,1));
  SurI.insertSurface(PnX.clone());

  // Far box :
  SurI.createSurface(31,"px 10");
  SurI.createSurface(32,"px 15");

  // Sphere :
  SurI.createSurface(100,"so 25");
  
  return;
}

void
testSurfDivide::createObjects()
  /*!
    Create Object for test
  */
{
  ELog::RegMethod RegA("testSurfDivide","createObjects");

  std::string Out;
  int cellIndex(2);
  const int surIndex(0);
  Out=ModelSupport::getComposite(surIndex,"100");
  ASim.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));   // Outside void Void

  Out=ModelSupport::getComposite(surIndex,"11 -12 13 -14 15 -16");
  ASim.addCell(MonteCarlo::Object(cellIndex++,3,0.0,Out));      // steel object

  Out=ModelSupport::getComposite(surIndex,"21 -22 23 -24 25 -26 -27");
  ASim.addCell(MonteCarlo::Object(cellIndex++,3,0.0,Out));      // steel object

  Out=ModelSupport::getComposite(surIndex,"11 -12 13 -14 15 -16 -17");
  ASim.addCell(MonteCarlo::Object(cellIndex++,3,0.0,Out));      // steel object

  Out=ModelSupport::getComposite(surIndex,"21 -22 23 -24 25 -26 (-13:14:-15:16)");
  ASim.addCell(MonteCarlo::Object(cellIndex++,3,0.0,Out));      // # 5
  
  return;
}

int 
testSurfDivide::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: Index of test [-ve for all]
    \returns -ve on error 0 on success.
  */
{
  ELog::RegMethod RegA("testSurfDivide","applyTest");
  TestFunc::regSector("testSurfDivide");

  typedef int (testSurfDivide::*testPtr)();
  testPtr TPtr[]=
    {
      &testSurfDivide::testBasicPair,
      &testSurfDivide::testMultiOuter,
      &testSurfDivide::testStatic,
      &testSurfDivide::testTemplate,
      &testSurfDivide::testTemplateInnerPair,
      &testSurfDivide::testTemplatePair
    };

  const std::string TestName[]=
    {
      "BasicPair",
      "MultiOuter",
      "Static",
      "Template",
      "TemplateInnerPair",
      "TemplatePair"
    };

  const size_t TSize(sizeof(TPtr)/sizeof(testPtr));
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
  for(size_t i=0;i<TSize;i++)
    {
      if (extra<0 || static_cast<size_t>(extra)==i+1)
        {
	  initSim();
	  TestFunc::regTest(TestName[i]);
	  const int retValue= (this->*TPtr[i])();
	  if (retValue || extra>0)
	    return retValue;
	}
    }
  return 0;
}

int
testSurfDivide::testStatic()
  /*!
    Test the cylinder expansion system
    \returns 0 on succes and -ve on failure
  */
{
  ELog::RegMethod RegA("testSurfDivide","testStatic");
  return 0;
}

int
testSurfDivide::testMultiOuter()
  /*!
    Test a single to multiple divide 
    \return -ve on error 
  */
{
  ELog::RegMethod RegA("testSurfDivide","testMultiOuter");
  int resTest(0);

  ModelSupport::surfDivide DA;
  DA.addFrac(0.6);
  DA.addFrac(0.8);
  DA.addMaterial(4);
  DA.addMaterial(5);
  DA.addMaterial(6);

  // Test Cell 4:

  DA.init(); 
  DA.setCellN(4);   // Cube cell
  DA.setOutNum(11,8001);
  DA.makeMulti<Geometry::Plane>(23,24,27);
  DA.activeDivide(ASim);

  // Initial cell : 11 -12 13 -14 15 -16 -17
  resTest=checkResults(11,"21 -22 23 -8002 -8001 25 -26");
  resTest+=checkResults(12,"21 -22 ( 8001 : 8002 ) -8004 -8003 25 -26");
  resTest+=checkResults(13,"21 -22 ( 8003 : 8004 ) -24 25 -26 -27");

  resTest+=checkSurfaceEqual(8001,"8001 py 0.6");
  
  //  resTest+=checkSurface(17,"8002 py 0.6");
  //  resTest+=checkSurface(8002,"8002 py 0.6");

  // sqrt(2)*0.8 sqrt
  //  resTest+=checkSurface(8004,"8004 p -0.56568 0.5 0 1"); 

  // surfaces : py -3 : py 3 
  // 17: p -3,2  -2 3  (xy)

  Geometry::Plane Pn(8002,0);
  Pn.setPlane(Geometry::Vec3D(-3,0,0),Geometry::Vec3D(-0.42426,0.82426,0));

  // sqrt(2)*0.8 sqrt
  resTest+=checkSurfaceEqual
    (8002,"8104 p -0.4576523893115 0.889131199 0.0 0.92132034");  

  if (resTest) return -2;
  
  return 0;
}

int
testSurfDivide::testBasicPair()
  /*!
    Test a simple pair divide between two surfaces.
    \return -ve on error 
  */
{
  ELog::RegMethod RegA("testSurfDivide","testBasicPair");
  
  ModelSupport::surfDivide DA;
  DA.addFrac(0.6);
  DA.addFrac(0.8);
  DA.addMaterial(4);
  DA.addMaterial(5);
  DA.addMaterial(6);


  // Test Cell 3:
  DA.init(); 
  DA.setCellN(3);   // Cube cell
  DA.setOutNum(11,8001);
  DA.makePair<Geometry::Plane>(13,14);
  DA.activeDivide(ASim);
  
  int resTest=checkResults(11,"11 -12 13 -8001 15 -16");
  resTest+=checkResults(12,"11 -12 8001 -8002 15 -16");
  resTest+=checkResults(13,"11 -12 8002 -14 15 -16");
  resTest+=checkSurfaceEqual(8001,"8001 py 0.2");
  resTest+=checkSurfaceEqual(8002,"8002 py 0.6");
  if (resTest) return -1;

  
  // Test Cell 2 (in reverse):

  initSim();
  DA.init();
  DA.setCellN(3);   // Cube cell
  DA.setOutNum(11,8001);
  DA.makePair<Geometry::Plane>(-14,13);
  DA.activeDivide(ASim);

  resTest=checkResults(11,"11 -12 8001 -14 15 -16");
  resTest+=checkResults(12,"11 -12 8002 -8001 15 -16");
  resTest+=checkResults(13,"11 -12 13 -8002 15 -16");
  resTest+=checkSurfaceEqual(8001,"8001 py -0.2");
  resTest+=checkSurfaceEqual(8002,"8002 py -0.6");

  if (resTest) return -2;
  
  return 0;
}

int 
testSurfDivide::checkSurfaceEqual
(const int SN,const std::string& surfStr) const
  /*!
    Test to see if the surfaces match expected
    \param SN :: Surface number
    \param surfStr :: Surface String [expected]
    \return -ve on error / 0 on success
  */
{
  ELog::RegMethod RegA("testSurfDivide","checkSurfaceEqual");

  const ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();
  const Geometry::Surface* SPtr=
    Geometry::surfaceFactory::Instance().processLine(surfStr);
  if (!SPtr)
    {
      ELog::EM<<"Failed to create surface "<<surfStr<<ELog::endCrit;
      return -1;
    }
  const Geometry::Surface* TPtr=SurI.getSurf(SN);
  
  int retval(-1);
  if (!TPtr)
    ELog::EM<<"Failed to find surface number:"<<SN<<ELog::endCrit;
  else if (!ModelSupport::equalSurface(SPtr,TPtr))
    {
      ELog::EM<<"Surfaces not equal:"<<ELog::endCrit;
      ELog::EM<<"SPtr :"<<*SPtr<<ELog::endCrit;
      ELog::EM<<"TPtr :"<<*TPtr<<ELog::endCrit;
    }
  else
    retval=0;
  
  delete SPtr;
  return retval;
}

int
testSurfDivide::checkResults(const int CN,
			     const std::string& strTest) const
  /*!
    Check a result and determine if it is ok
    \param CN :: Cell number
    \param strTest :: String to test
    \return 0 on success/ -ve on error
  */
{
  ELog::RegMethod RegA("testSurfDivide","checkResults");

  const MonteCarlo::Object* OP=ASim.findObject(CN);
  if (!OP)
    {
      ELog::EM<<"No cell for: "<<CN<<ELog::endCrit;
      return -1;
    }
  HeadRule HR;
  HR.procString(strTest);
  if (OP->getHeadRule()!=HR)
    {
      ELog::EM<<"Cell  : "<<OP->cellCompStr()<<" from "<<OP->getName()
	      <<ELog::endCrit;
      ELog::EM<<"Expect: "<<strTest<<ELog::endCrit;
      ELog::EM<<"Cell: "<<*OP<<ELog::endCrit;
      return -1;
    }
  return 0;
}

int
testSurfDivide::testTemplate()
  /*!
    Placeholder for testing mergeTemplate object.
    \return 0 
  */
{
  ELog::RegMethod RegA("testSurfDivide","mergeTemplate");

  ModelSupport::surfDivide DA;
  DA.addFrac(0.2);
  DA.addFrac(0.6);
  DA.addMaterial(4);
  DA.addMaterial(6);
  DA.addMaterial(5);

  // Test Cell 1:
  DA.init(); 
  DA.setCellN(5);   // Cube cell
  DA.setOutNum(11,8001);
  
  mergeTemplate<Geometry::Plane,Geometry::Plane> tempRule;
  tempRule.setSurfPair(13,14);
  //  tempRule.setSurfPair(13,17);
  tempRule.setInnerRule(" 13 ");
  tempRule.setOuterRule(" -14 ");
  DA.addRule(&tempRule);
  DA.activeDivideTemplate(ASim);
  
  int resTest=checkResults(11,"11 -12 13 -8001 15 -16 -17");
  resTest+=checkResults(12,"11 -12 8001 -8002 15 -16 -17");
  resTest+=checkResults(13,"11 -12 8002 -14 15 -16 -17");

  resTest+=checkSurfaceEqual(8001,"8001 py -0.6");
  resTest+=checkSurfaceEqual(8002,"8002 py 0.2");

  if (resTest) return -1;

  return 0;
}

int
testSurfDivide::testTemplatePair()
  /*!
    Placeholder for testing mergeTemplate object.
    \return 0 
  */
{
  ELog::RegMethod RegA("testSurfDivide","mergeTemplatePair");

  ModelSupport::surfDivide DA;
  DA.addFrac(0.2);
  DA.addFrac(0.6);
  DA.addMaterial(4);
  DA.addMaterial(6);
  DA.addMaterial(5);

  // Test Cell 1:
  DA.init(); 
  DA.setCellN(5);   // Cube cell
  DA.setOutNum(11,8001);
  
  mergeTemplate<Geometry::Plane,Geometry::Plane> tempRule;
  tempRule.setSurfPair(13,14);
  tempRule.setSurfPair(13,17);
  //  tempRule.setSurfPair(13,17);
  tempRule.setInnerRule(" 13 ");
  tempRule.setOuterRule(" -17 -14 ");
  DA.addRule(&tempRule);
  DA.activeDivideTemplate(ASim);
  
  int resTest=checkResults(11,"11 -12 13 -8001 -8002 15 -16 ");
  resTest+=checkResults(12,"11 -12 (8001:8002) -8003 -8004 15 -16 ");
  resTest+=checkResults(13,"11 -12 (8003:8004) -14 15 -16 -17");

  resTest+=checkSurfaceEqual(8001,"8001 py -0.6");
  resTest+=checkSurfaceEqual(8003,"8003 py 0.2");

  if (resTest) return -1;

  return 0;
}

int
testSurfDivide::testTemplateInnerPair()
  /*!
    Placeholder for testing mergeTemplate object.
    \return 0 
  */
{
  ELog::RegMethod RegA("testSurfDivide","testTemplateInnerPair");

  ModelSupport::surfDivide DA;
  DA.addFrac(0.2);
  DA.addFrac(0.6);
  DA.addMaterial(4);
  DA.addMaterial(6);
  DA.addMaterial(5);

  // Test Cell 1:
  DA.init(); 
  DA.setCellN(6);   // Cube cell
  DA.setOutNum(11,8001);
  
  mergeTemplate<Geometry::Plane,Geometry::Plane> tempRule;
  tempRule.setSurfPair(15,25);
  tempRule.setSurfPair(16,26);
  //  tempRule.setSurfPair(13,17);
  tempRule.setInnerRule(" (-15:16) ");
  tempRule.setOuterRule(" 25 -26");
  DA.addRule(&tempRule);
  DA.activeDivideTemplate(ASim);
  
  int resTest=checkResults(11,"21 -22 -8002 8001 23 (-13:14:-15:16) -24");
  resTest+=checkResults
    (12," 21 23 -22 -24 8003 -8004 ( -8001 : 8002 : -13 : 14 )");
  resTest+=checkResults
    (13," 21 25 23 -26 -22 ( -8003 : 8004 : -13 : 14 ) -24");

  if (resTest) return -1;

  return 0;
}
