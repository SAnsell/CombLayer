/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   Main/testMain.cxx
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
#include <list>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <memory>
#include <array>
#include <boost/format.hpp>
#include <boost/multi_array.hpp>

#include "Exception.h"
#include "MersenneTwister.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Debug.h"
#include "RefControl.h"
#include "RotCounter.h"
#include "BnId.h"
#include "Triple.h"
#include "mathSupport.h"
#include "MapSupport.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "XMLload.h"
#include "XMLattribute.h"
#include "XMLobject.h"
#include "XMLgroup.h"
#include "XMLcollect.h"
#include "NRange.h"
#include "Vec3D.h"
#include "SVD.h"
#include "Transform.h"
#include "Surface.h"
#include "Rules.h"
#include "RuleBinary.h"
#include "Acomp.h"
#include "Algebra.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Quadratic.h"
#include "Cone.h"
#include "Cylinder.h"
#include "Plane.h"
#include "EllipticCyl.h"
#include "surfIndex.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Convex.h"
#include "Vert2D.h"
#include "Convex2D.h"
#include "NList.h"
#include "ModeCard.h"
#include "PhysCard.h"
#include "LSwitchCard.h"
#include "PhysImp.h"
#include "PhysicsCards.h"
#include "funcList.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "simpleObj.h"
#include "MainProcess.h"
#include "MainInputs.h"

#include "testAlgebra.h"
#include "testAttachSupport.h"
#include "testBinData.h"
#include "testBnId.h"
#include "testBoost.h"
#include "testBoundary.h"
#include "testBoxLine.h"
#include "testCone.h"
#include "testContained.h"
#include "testConvex.h"
#include "testConvex2D.h"
#include "testCylinder.h"
#include "testDBMaterial.h"
#include "testDoubleErr.h"
#include "testElement.h"
#include "testEllipticCyl.h"
#include "testExtControl.h"
#include "testFace.h"
#include "testFortranWrite.h"
#include "testFunc.h"
#include "testFunction.h"
#include "testGeomSupport.h"
#include "testHeadRule.h"
#include "testInputParam.h"
#include "testInsertComp.h"
#include "testLine.h"
#include "testLineTrack.h"
#include "testLog.h"
#include "testMapRange.h"
#include "testMapSupport.h"
#include "testMasterRotate.h"
#include "testMaterial.h"
#include "testMathSupport.h"
#include "testMatrix.h"
#include "testMD5.h"
#include "testMersenne.h"
#include "testMesh3D.h"
#include "testModelSupport.h"
#include "testNeutron.h"
#include "testNList.h"
#include "testNRange.h"
#include "testObject.h"
#include "testObjectRegister.h"
#include "testObjectTrackAct.h"
#include "testObjSurfMap.h"
#include "testObjTrackItem.h"
#include "testPairFactory.h"
#include "testPairItem.h"
// #include "testPhysics.h"
#include "testPipeLine.h"
#include "testPipeUnit.h"
#include "testPlane.h"
#include "testPoly.h"
#include "testQuaternion.h"
#include "testRecTriangle.h"
#include "testRefPlate.h"
#include "testRotCounter.h"
#include "testRules.h"
#include "testSimpleObj.h"
#include "testSimpson.h"
#include "testSingleObject.h"
#include "testSimulation.h"
#include "testSolveValues.h"
#include "testSource.h"
#include "testSupport.h"
#include "testSurfDIter.h"
#include "testSurfDivide.h"
#include "testSurfEqual.h"
#include "testSurfExpand.h"
#include "testSurIntersect.h"
#include "testSurfRegister.h"
#include "testSVD.h"
#include "testTally.h"
#include "testVarNameOrder.h"
#include "testVec3D.h"
#include "testVolumes.h"
#include "testWorkData.h"
#include "testWrapper.h"
#include "testXML.h"

//
MTRand RNG(12345UL);

namespace ELog 
{
  ELog::OutputLog<EReport> EM;
  ELog::OutputLog<FileReport> FM("Spectrum.log");
  ELog::OutputLog<FileReport> RN("Renumber.txt");   ///< Renumber
  ELog::OutputLog<StreamReport> CellM;
}

int startTest(const int,const int,const int);

int attachCompTest(const int,const int);
int funcbaseTest(const int,const int);
int geometryTest(const int,const int);
int globalTest(const int,const int);
int lensTest(const int,const int);
int logTest(const int,const int);
int moderatorTest(const int,const int);
int montecarloTest(const int,const int);
int physicsTest(const int,const int);
int polyTest(const int,const int);
int processTest(const int,const int);
int supportTest(const int,const int);
int workTest(const int,const int);
int xmlTest(const int,const int);
int fullTest();

int
main(int argc,char* argv[])
{
///  ELog::FMessages.getReport().setFile("Fred");  
  ELog::RegMethod RControl("","main");
  mainSystem::activateLogging(RControl);
  ELog::EM.setDebug(0);
  ELog::EM.setAction(ELog::error);

  int retVal(0);
  if(argc>1)
    {
      int section(0);
      int type(0);
      int extra(0);
      StrFunc::convert(argv[1],section);
      if (argc>2)
	StrFunc::convert(argv[2],type);
      if (argc>3)
	StrFunc::convert(argv[3],extra);

      if (section<0 && type<0 && extra<0)
	retVal=fullTest();
      else
	retVal=startTest(section,type,extra);

      if (section*type*extra)
	{
	  ELog::EM<<ELog::endDiag;
      
	  if (!retVal)
	    ELog::EM<<"Success:"
		    <<TestFunc::Instance().summary()<<ELog::endDiag;
	  else
	    {
	      TestFunc::Instance().reportTest(ELog::EM.Estream());
	      ELog::EM<<"Failed: return value == "<<retVal<<ELog::endDiag;
	    }
	  ELog::EM<<ELog::endDiag;
	}
    }
  else
    startTest(0,0,0);

  ModelSupport::objectRegister::Instance().reset();
  ModelSupport::surfIndex::Instance().reset();
  return 0;
}

int
startTest(const int section,const int type,const int extra)
  /*!
    Processes all the tests:
    \param section :: Section to test
    \param type :: type to test
    \param extra :: extra number
    \return 0 on success / -ve on failure
  */
{
  TestFunc::Instance().regStatus(section,type,extra);
  try
    {
      if (section==0)
        {
	  std::cout<<"Namespace :: AttachSystem (1)"<<std::endl;
	  std::cout<<"Namespace :: FuncBase     (2)"<<std::endl;
	  std::cout<<"Namespace :: Geometry     (3)"<<std::endl;
	  std::cout<<"Namespace :: Lens         (4)"<<std::endl;
	  std::cout<<"Namespace :: Log          (5)"<<std::endl;
	  std::cout<<"Namespace :: Moderator    (6)"<<std::endl;
	  std::cout<<"Namespace :: MonteCarlo   (7)"<<std::endl;
	  std::cout<<"Namespace :: Physics      (8)"<<std::endl;
	  std::cout<<"Namespace :: Poly         (9)"<<std::endl;
	  std::cout<<"Namespace :: Process     (10)"<<std::endl;
	  std::cout<<"Namespace :: Support     (11)"<<std::endl;
	  std::cout<<"Namespace :: Work        (12)"<<std::endl;
	  std::cout<<"Namespace :: XML         (13)"<<std::endl;
	  std::cout<<"Namespace :: Global      (14)"<<std::endl;

	  return 0;
	}

      // AttachComp
      if (section==1)
	{
	  TestFunc::regGroup("AttachComp");
	  return attachCompTest(type,extra);
	}
      // FUNCBASE
      if (section==2)
	{
	  TestFunc::regGroup("FuncBase");
	  return funcbaseTest(type,extra);
	}

      // GEOMETRY
      if (section==3)
	{
	  TestFunc::regGroup("Geometry");
	  return geometryTest(type,extra);
	}

      // LENS
      if (section==4)
	{
	  TestFunc::regGroup("Lens");
	  return lensTest(type,extra);
	}

      // LOG
      if (section==5)
	{
	  TestFunc::regGroup("Log");
	  return logTest(type,extra);
	}

      // MODERATOR
      if (section==6)      
	{
	  TestFunc::regGroup("Moderator");
	  return moderatorTest(type,extra);
	}

      // MONTECARLO
      if (section==7)      
	{
	  TestFunc::regGroup("MonteCarlo");
	  return montecarloTest(type,extra);
	}

      // PHYSICS
      if (section==8)
	{
	  TestFunc::regGroup("Physics");
	  return physicsTest(type,extra);
	}
      // POLY
      if (section==9)
	{
	  TestFunc::regGroup("Poly");
	  return polyTest(type,extra);
	}

      // PROCESS
      if (section==10)
	{
	  TestFunc::regGroup("Process");
	  return processTest(type,extra);
	}

      // SUPPORT:
      if (section==11)
	{
	  TestFunc::regGroup("Support");
	  return supportTest(type,extra);
	}

      // WORK:
      if (section==12)
	{	  
	  TestFunc::regGroup("Work");
	  return workTest(type,extra);
	}

      // XML:
      if (section==13)
	{
	  TestFunc::regGroup("XML");
	  return xmlTest(type,extra);
	}
      
      // GLOBAL:
      if (section==14)
	{
	  TestFunc::regGroup("Global");
	  return globalTest(type,extra);
	}
      
    }
  catch (ColErr::ExBase& EObj)
    {
      std::cout<<"ERROR "<<EObj.what()<<std::endl;
      exit(1);
    }
  return 0;
}

int
globalTest(const int type,const int extra)
  /*!
    Test global Function
    \param type :: type of test
    \param extra :: extra value
    \return failed test number [number of tests if type<0]
  */
{
  const std::vector<std::string> TestName=
    {
      "testBnId",
      "testBoost",
      "testHeadRule",
      "testInsertComp",
      "testMapRange",
      "testMapSupport",
      "testMersenne",
      "testNList",
      "testNRange",
      "testRotCounter",
      "testRules",
      "testSimulation",
      "testSource",
      "testTally"
    };
  const size_t TSize(TestName.size());

  if (type==0)
    {
      TestFunc::Instance().reportTest(std::cout);
      boost::format FMT("%1$s%|20t|(%2$d)");
      for(size_t i=0;i<TSize;i++)
	std::cout<<FMT % TestName[i] % (i+1)<<std::endl;
      return 0;
    }

  int index((type<0) ? 0 : type-1);
  int X(0);
  do
    {
      index++;
      int cnt(1);
      if (index==cnt)
	{
	  testBnId A;
	  X=A.applyTest(extra);
	}
      cnt++;
      if (index==cnt)
	{
	  testBoost A;
	  X=A.applyTest(extra);
	}
      cnt++;
      if (index==cnt)
	{
	  testHeadRule A;
	  X=A.applyTest(extra);
	}
      cnt++;
      if (index==cnt)
	{
	  testInsertComp A;
	  X=A.applyTest(extra);
	}
      cnt++;
      if(index==cnt)
	{
	  testMapRange A;
	  X=A.applyTest(extra);
	}
      cnt++;
      if(index==cnt)
	{
	  testMapSupport A;
	  X=A.applyTest(extra);
	}
      cnt++;
      if(index==cnt)
	{
	  testMersenne A;
	  X=A.applyTest(extra);
	}
      cnt++;
      if(index==cnt)
	{
	  testNList A;
	  X=A.applyTest(extra);
	}
      cnt++;
      if(index==cnt)
	{
	  testNRange A;
	  X=A.applyTest(extra);
	}
      cnt++;
      if(index==cnt)
	{
	  testRotCounter A;
	  X=A.applyTest(extra);
	}
      cnt++;
      if(index==cnt)
	{
	  testRules A;
	  X=A.applyTest(extra);
	}
      cnt++;
      if(index==cnt)
	{
	  testSimulation A;
	  X=A.applyTest(extra);
	}
      cnt++;
      if(index==cnt)
	{
	  testSource A;
	  X=A.applyTest(extra);
	}
      cnt++;
      if(index==cnt)
	{
	  testTally A;
	  X=A.applyTest(extra);
	}

    } while (!X && type!=index && index<static_cast<int>(TSize));
    
  if (X)
    return -index;

  return X;
}


int
funcbaseTest(const int type,const int extra)
  /*!
    Test math functions
    \param type :: type of test
    \param extra :: extra value
    \return 0 on success / -ve on failure
  */
{
  if (type==0)
    {
      TestFunc::Instance().reportTest(std::cout);
      std::cout<<"testFunction             (1)"<<std::endl;
      std::cout<<"testMD5                  (2)"<<std::endl;
      std::cout<<"testVarNameOrder         (3)"<<std::endl;
    }

  if(type==1 || type<0)
    {
      testFunction A;
      const int X=A.applyTest(extra);
      if (X) return -1;
    }
  if(type==2 || type<0)
    {
      testMD5 A;
      const int X=A.applyTest(extra);
      if (X) return -2;
    }
  if(type==3 || type<0)
    {
      testVarNameOrder A;
      const int X=A.applyTest(extra);
      if (X) return -3;
    }
  return 0;
}

int
geometryTest(const int type,const int extra)
  /*!
    Test geometry objects
    \param type :: type of test
    \param extra :: extra value
    \return 0 on success / -ve on failure
  */
{
  const std::vector<std::string> TestName=
    {
      "testCone",
      "testConvex",
      "testConvex2D",
      "testCylinder",
      "testEllipticCyl",
      "testFace",
      "testGeomSupport",
      "testLine",
      "testMasterRotate",
      "testMesh3D",
      "testQuaternion",
      "testPlane",
      "testRecTriangle",
      "testSurIntersect",
      "testSVD",
      "testVec3D"
    };

  if (type==0)
    {
      TestFunc::Instance().reportTest(std::cout);
      boost::format FMT("%1$s%|20t|(%2$d)");
      for(size_t i=0;i<TestName.size();i++)
	std::cout<<FMT % TestName[i] % (i+1)<<std::endl;
      return 0;
    }

  int index((type<0) ? 0 : type-1);
  int X(0);
  int testNum;
  do
    {
      index++;
      testNum=1;
      if (index==testNum++)
	{
	  testCone A;
	  X=A.applyTest(extra);
	}

      if (index==testNum++)
	{
	  testConvex A;
	  X=A.applyTest(extra);
	}
      
      if(index==testNum++)
	{
	  testConvex2D A;
	  X=A.applyTest(extra);
	}
      if(index==testNum++)
	{
	  testCylinder A;
	  X=A.applyTest(extra);
	}
      if(index==testNum++)
	{
	  testEllipticCyl A;
	  X=A.applyTest(extra);
	}
      if(index==testNum++)
	{
	  testFace A;
	  X=A.applyTest(extra);
	}      
      if(index==testNum++)
	{
	  testGeomSupport A;
	  X=A.applyTest(extra);
	}      
      if(index==testNum++)
	{
	  testLine A;
	  X=A.applyTest(extra);
	}
      
      if(index==testNum++)
	{
	  testMasterRotate A;
	  X=A.applyTest(extra);
	}
      
      if(index==testNum++)
	{
	  testMesh3D A;
	  X=A.applyTest(extra);
	}

      if(index==testNum++)
	{
	  testQuaternion A;
	  X=A.applyTest(extra);
	}
      
      if(index==testNum++)
	{
	  testPlane A;
	  X=A.applyTest(extra);
	}
      
      if(index==testNum++)
	{
	  testRecTriangle A;
	  X=A.applyTest(extra);
	}

      if(index==testNum++)
	{
	  testSurIntersect A;
	  X=A.applyTest(extra);
	}
      
      if(index==testNum++)
	{
	  testSVD A;
	  X=A.applyTest(extra);
	}
      
      if(index==testNum++)
	{
	  testVec3D A;
	  X=A.applyTest(extra);
	}
      
    } while (!X && type!=index &&
	     index<testNum-1);
    
  return X;
}

int
lensTest(const int type,const int)
  /*!
    Test lens commands
    \param type :: type of test
    \param extra :: extra value
    \return 0 on success / -ve on failure
  */
{
  if (type==0)
    {
      //      std::cout<<"testFlightLine          (1)"<<std::endl;
    }
  if (type==1 || type<0)
    {
      //      testFlightLine A;
      //      const int X=A.applyTest(extra);
      //      std::cout<<"testFlightLine(1) == "<<X<<std::endl;
      //      if (X) return -1;
    }
  return 0;
}

int
logTest(const int type,const int extra)
  /*!
    Test log command
    \param type :: type of test
    \param extra :: extra value
    \return 0 on success / -ve on failure
  */
{
  if (type==0)
    {
      TestFunc::Instance().reportTest(std::cout);
      std::cout<<"testOutputLog             (1)"<<std::endl;
    }
  if (type==1 || type<0)
    {
      testLog A;
      const int X=A.applyTest(extra);
      std::cout<<"testOutputLog(1) == "<<X<<std::endl;
      if (X) return X;
    }
  return 0;
}

int
moderatorTest(const int type,const int extra)
  /*!
    Test MonteCarlo Function
    \param type :: type of test
    \param extra :: extra value
    \return 0 on success / -ve on failure
  */
{
  ELog::RegMethod RegA("testMain[F]","moderatorTest");

  if (type==0)
    {
      TestFunc::Instance().reportTest(std::cout);
      std::cout<<"testRefPlate         (1)"<<std::endl;
      std::cout<<"testSingleObject     (2)"<<std::endl;
    }

  if(type==1 || type<0)
    {
      testRefPlate A;
      const int X=A.applyTest(extra);
      if (X) return X;
    }
  if(type==2 || type<0)
    {
      testSingleObject A;
      const int X=A.applyTest(extra);
      if (X) return X;
    }

  return 0;
}

int
montecarloTest(const int type,const int extra)
  /*!
    Test MonteCarlo Function
    \param type :: type of test
    \param extra :: extra value
    \return 0 on success / -ve on failure
  */
{
  ELog::RegMethod RegA("testMain","montecarloTest");

  if (type==0)
    {
      TestFunc::Instance().reportTest(std::cout);
      std::cout<<"testAlgebra          (1)"<<std::endl;
      std::cout<<"testDBMaterial       (2)"<<std::endl;
      std::cout<<"testElement          (3)"<<std::endl;
      std::cout<<"testMaterial         (4)"<<std::endl;
      std::cout<<"testNeutron          (5)"<<std::endl;
      std::cout<<"testObject           (6)"<<std::endl;
    }

  if(type==1 || type<0)
    {
      testAlgebra A;
      const int X=A.applyTest(extra);
      if (X) return X;
    }
  if(type==2 || type<0)
    {
      testDBMaterial A;
      const int X=A.applyTest(extra);
      if (X) return X;
    }

  if(type==3 || type<0)
    {
      testElement A;
      const int X=A.applyTest(extra);
      if (X) return X;
    }


  if(type==4 || type<0)
    {
      testMaterial A;
      const int X=A.applyTest(extra);
      if (X) return X;
    }

  if(type==5 || type<0)
    {
      testNeutron A;
      const int X=A.applyTest(extra);
      if (X) return X;
    }

  if(type==6 || type<0)
    {
      testObject A;
      const int X=A.applyTest(extra);
      if (X) return X;
    }

  return 0;
}

int
attachCompTest(const int type,const int extra)
  /*!
    Test process command
    \param type :: type of test
    \param :: extra value
    \return 0 on success / -ve on failure
  */
{
  if (type==0)
    {
      TestFunc::Instance().reportTest(std::cout);
      std::cout<<"testAttachSupport          (1)"<<std::endl;
      std::cout<<"testContained              (2)"<<std::endl;
    }
  if(type==1 || type<0)
    {
      testAttachSupport A;
      const int X=A.applyTest(extra);
      if (X) return X;
    }
  if(type==2 || type<0)
    {
      testContained A;
      const int X=A.applyTest(extra);
      if (X) return X;
    }
  return 0;

}
int
physicsTest(const int type,const int extra)
  /*!
    Test process command
    \param type :: type of test
    \param :: extra value
    \return 0 on success / -ve on failure
  */
{
  if (type==0)
    {
      TestFunc::Instance().reportTest(std::cout);
      std::cout<<"testExtControl    (1)"<<std::endl;
    }
  if(type==1 || type<0)
    {
      testExtControl A;
      const int X=A.applyTest(extra);
      if (X) return X;
    }
  return 0;

}

int
processTest(const int type,const int extra)
  /*!
    Test process command
    \param type :: type of test
    \param :: extra value
    \return 0 on success / -ve on failure
  */
{
  if (type==0)
    {
      TestFunc::Instance().reportTest(std::cout);
      std::cout<<"testBoxLine          (1)"<<std::endl;
      std::cout<<"testInputParam       (2)"<<std::endl;
      std::cout<<"testLineTrack        (3)"<<std::endl;
      std::cout<<"testObjectRegister   (4)"<<std::endl;
      std::cout<<"testObjectTrackAct   (5)"<<std::endl;
      std::cout<<"testObjSurfMap       (6)"<<std::endl;
      std::cout<<"testObjTrackItem     (7)"<<std::endl;
      std::cout<<"testPairFactory      (8)"<<std::endl;
      std::cout<<"testPairItem         (9)"<<std::endl;
      std::cout<<"testPipeLine        (10)"<<std::endl;
      std::cout<<"testPipeUnit        (11)"<<std::endl;
      std::cout<<"testSimpleObj       (12)"<<std::endl;
      std::cout<<"testSurfDIter       (13)"<<std::endl;
      std::cout<<"testSurfDivide      (14)"<<std::endl;
      std::cout<<"testSurfEqual       (15)"<<std::endl;
      std::cout<<"testSurfExpand      (16)"<<std::endl;
      std::cout<<"testSurfRegister    (17)"<<std::endl;
      std::cout<<"testVolumes         (18)"<<std::endl;
      std::cout<<"testWrapper         (19)"<<std::endl;
    }
  int index(1);
  if(type==index || type<0)
    {
      testBoxLine A;
      const int X=A.applyTest(extra);
      if (X) return X;
    }
  index++;
  
  if(type==index || type<0)
    {
      testInputParam A;
      const int X=A.applyTest(extra);
      if (X) return X;
    }
  index++;
  
  if(type==index || type<0)
    {
      testLineTrack A;
      const int X=A.applyTest(extra);
      if (X) return X;
    }
  index++;
  
  if(type==index || type<0)
    {
      testObjectRegister A;
      const int X=A.applyTest(extra);
      if (X) return X;
    }
  index++;
  
  if(type==index || type<0)
    {
      testObjectTrackAct A;
      const int X=A.applyTest(extra);
      if (X) return X;
    }
  index++;
  
  if(type==index || type<0)
    {
      testObjSurfMap A;
      const int X=A.applyTest(extra);
      if (X) return X;
    }
  index++;
    
  if(type==index || type<0)
    {
      testObjTrackItem A;
      const int X=A.applyTest(extra);
      if (X) return X;
    }
  index++;
  
  if(type==index || type<0)
    {
      testPairFactory A;
      const int X=A.applyTest(extra);
      if (X) return X;
    }
  index++;
  
  if(type==index || type<0)
    {
      testPairItem A;
      const int X=A.applyTest(extra);
      if (X) return X;
    }
  index++;
  
  if(type==index || type<0)
    {
      testPipeLine A;
      const int X=A.applyTest(extra);
      if (X) return X;
    }
  index++;
  
  if(type==index || type<0)
    {
      testPipeUnit A;
      const int X=A.applyTest(extra);
      if (X) return X;
    }
  index++;
  
  if(type==index || type<0)
    {
      testSimpleObj A;
      const int X=A.applyTest(extra);
      if (X) return X;
    }
  index++;
  
  if(type==index || type<0)
    {
      testSurfDIter A;
      const int X=A.applyTest(extra);
      if (X) return X;
    }
  index++;

  if(type==index || type<0)
    {
      testSurfDivide A;
      const int X=A.applyTest(extra);
      if (X) return X;
    }
  index++;
  
  if(type==index || type<0)
    {
      testSurfEqual A;
      const int X=A.applyTest(extra);
      if (X) return X;
    }
  index++;
  
  if(type==index || type<0)
    {
      testSurfExpand A;
      const int X=A.applyTest(extra);
      if (X) return X;
    }
  index++;
  
  if(type==index || type<0)
    {
      testSurfRegister A;
      const int X=A.applyTest(extra);
      if (X) return X;
    }
  index++;
  
  if(type==index || type<0)
    {
      testVolumes A;
      const int X=A.applyTest(extra);
      if (X) return X;
    }
  index++;
  
  if(type==index || type<0)
    {
      testWrapper A;
      const int X=A.applyTest(extra);
      if (X) return X;
    }

  return 0;
}

int
polyTest(const int type,const int extra)
  /*!
    Test log command
    \param type :: type of test
    \param extra :: extra value
    \return 0 on success / -ve on failure
  */
{
  if (type==0)
    {
      TestFunc::Instance().reportTest(std::cout);
      std::cout<<"Poly             (1)"<<std::endl;
      std::cout<<"SolveValues      (2)"<<std::endl;
    }

  if(type==1 || type<0)
    {
      testPoly A;
      const int X=A.applyTest(extra);
      if (X) return X;
    }

  if(type==2 || type<0)
    {
      testSolveValues A;
      const int X=A.applyTest(extra);
      if (X) return X;
    }
  
  return 0;
}

int
supportTest(const int type,const int extra)
  /*!
    Test support Function
    \param type :: type of test
    \param extra :: extra value
    \return 0 on success / -ve on failure
  */
{
  if (type==0)
    {
      TestFunc::Instance().reportTest(std::cout);
      std::cout<<"testDoubleErr           (1)"<<std::endl;
      std::cout<<"testFortranWrite        (2)"<<std::endl;
      std::cout<<"testMathSupport         (3)"<<std::endl;
      std::cout<<"testMatrix              (4)"<<std::endl;
      std::cout<<"testModelSupport        (5)"<<std::endl;
      std::cout<<"testSimpson             (6)"<<std::endl;
      std::cout<<"testSupport             (7)"<<std::endl;
      return 0;
    }

  if (type==1 || type<0)
    {
      testDoubleErr A;
      int X=A.applyTest(extra);
      if (X) return X;
    }

  if(type==2 || type<0)
    {
      testFortranWrite A;
      int X=A.applyTest(extra);
      if (X) return X;
    }

  if(type==3 || type<0)
    {
      testMathSupport A;
      int X=A.applyTest(extra);
      if (X) return X;
    }

  if(type==4 || type<0)
    {
      testMatrix A;
      int X=A.applyTest(extra);
      if (X) return X;
    }
  if(type==5 || type<0)
    {
      testModelSupport A;
      int X=A.applyTest(extra);
      if (X) return X;
    }
  if(type==6 || type<0)
    {
      testSimpson A;
      int X=A.applyTest(extra);
      if (X) return X;
    }
  if(type==7 || type<0)
    {
      testSupport A;
      int X=A.applyTest(extra);
      if (X) return X;
    }
  return 0;
}

int
workTest(const int type,const int extra)
  /*!
    Test work function
    \param type :: type of test
    \param extra :: extra value
    \return 0 on success / -ve on failure
  */
{
  if (type==0)
    {
      TestFunc::Instance().reportTest(std::cout);
      std::cout<<"testBinData          (1)"<<std::endl;
      std::cout<<"testBoundary         (2)"<<std::endl;
      std::cout<<"testWorkData         (3)"<<std::endl;
    }

  if(type==1 || type<0)
    {
      testBinData A;
      int X=A.applyTest(extra);
      if (X) return X;
    }

  if(type==2 || type<0)
    {
      testBoundary A;
      int X=A.applyTest(extra);
      if (X) return X;
    }

  if(type==3 || type<0)
    {
      testWorkData A;
      int X=A.applyTest(extra);
      if (X) return X;
    }

  return 0;
}

int
xmlTest(const int type,const int extra)
  /*!
    Test xml function
    \param type :: type of test
    \param extra :: extra value
    \return 0 on success / -ve on failure
  */
{
  if (type==0)
    {
      TestFunc::Instance().reportTest(std::cout);
      std::cout<<"testXML         (1)"<<std::endl;
    }

  if(type==1 || type<0)
    {
      testXML A;
      int X=A.applyTest(extra);
      if (X) return X;
    }

  return 0;
}

int
fullTest()
  /*!
    Those tests that can be run for all things
    \return error code
  */
{
  ELog::RegMethod RegA("testMain","fullTest");

  for(int i=0;i<14;i++)
    {
      std::ostringstream cx;
      cx<<"TEST SECTION : "<<i+1;
      TestFunc::bracketTest(cx.str(),ELog::EM.Estream());
      ELog::EM<<ELog::endCrit;
      const int outFlag=startTest(i+1,-1,-1);
      if (outFlag)
	return outFlag;
    }
  return 0;
}

