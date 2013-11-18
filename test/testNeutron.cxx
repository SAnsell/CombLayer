/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   test/testNeutron.cxx
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
#include <list>
#include <vector>
#include <map>
#include <string>
#include <boost/tuple/tuple.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Triple.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "neutron.h"

#include "testFunc.h"
#include "testNeutron.h"

using namespace MonteCarlo;

testNeutron::testNeutron() 
  /*!
    Constructor
  */
{}

testNeutron::~testNeutron() 
  /*!
    Destructor
  */
{}

int 
testNeutron::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: index of test to access (-ve for all)
    \retval -ve : Failure number
    \retval 0 : All succeeded
  */
{
  ELog::RegMethod RegA("testNeutron","applyTest");
  TestFunc::regSector("testNeutron");

  typedef int (testNeutron::*testPtr)();
  testPtr TPtr[]=
    { 
      &testNeutron::testRefraction
    };

  std::string TestName[] = 
    {
      "Refraction"
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
testNeutron::testRefraction()
  /*!
    Test the refraction code. Added code to check that the
    h vector (eqn 16 of micro: B. Walter 2007 Eurographics) is 

    Also test the half angle condition
    \retval 0 :: success / -ve on failure
   */
{
  ELog::RegMethod RegA("testNeutron","testRefraction");

  typedef boost::tuple<double,Geometry::Vec3D,Geometry::Vec3D,Geometry::Vec3D> DT;
  typedef std::vector<DT> AnsTYPE;
  AnsTYPE NStore;

  // this is Ref index (N_o) : Incident Direction : Surface normal
  //  - Incident vector points towards the surface 
  //  - Surface normal point into N_i medium.
  //  - uVec of neutron

  /// Give B.uVec = ( 0,0.636396 -0.771362) : Wikipedia example [snells law]

  NStore.push_back(DT(1/0.9,
		      Geometry::Vec3D(0,0.707107,-0.707107),
		      Geometry::Vec3D(0,0,-1),Geometry::Vec3D(0,0.636396,-0.771362) ));

  // Reflection from surface:
  NStore.push_back(DT(1/1.51,
		      Geometry::Vec3D(0,cos(M_PI*40.0/180.0),sin(M_PI*40/180.0)),
		      Geometry::Vec3D(0,0,1),
		      Geometry::Vec3D(0,cos(M_PI*40.0/180.0),-sin(M_PI*40/180.0)) ));

  // Reflection from surface -ve normal: 
  NStore.push_back(DT(1/1.51,
		      Geometry::Vec3D(0,cos(M_PI*40.0/180.0),sin(M_PI*40/180.0)),
		      Geometry::Vec3D(0,0,-1),
		      Geometry::Vec3D(0,cos(M_PI*40.0/180.0),-sin(M_PI*40/180.0)) ));

  // Reflection from surface:
  NStore.push_back(DT(1/1.51,
		      Geometry::Vec3D(0,cos(M_PI*40.0/180.0),-sin(M_PI*40/180.0)),
		      Geometry::Vec3D(0,0,1),
		      Geometry::Vec3D(0,cos(M_PI*40.0/180.0),sin(M_PI*40/180.0)) ));

  // NStore.push_back(DT(1.33,
  // 		      Geometry::Vec3D(0,cos(M_PI*45.0/180.0),sin(M_PI*45.0/180.0)),
  // 		      Geometry::Vec3D(0,-1,0),Geometry::Vec3D(0,0,0) ));

  // NStore.push_back(DT(1.33,
  // 		      Geometry::Vec3D(0,cos(M_PI*30.0/180.0),sin(M_PI*30.0/180.0)),
  // 		      Geometry::Vec3D(0,-1,-1),Geometry::Vec3D(0,0,0) ));


  AnsTYPE::iterator vc;
  for(vc=NStore.begin();vc!=NStore.end();vc++)
    {
      vc->get<2>().makeUnit();
      neutron A(1.0,Geometry::Vec3D(0,0,0),vc->get<1>());
      neutron B(A);
      B.refract(1.0,vc->get<0>(),vc->get<2>());
      // Calc half vector :: This equals normal if and only if A,B obey Snells law
      if ( (B.uVec - vc->get<3>()).abs()>1e-5)
	{
	  ELog::EM<<"Failed on point "<<(vc-NStore.begin())+1<<ELog::endTrace;
	  ELog::EM<<"A == "<<A.uVec<<ELog::endTrace;
	  ELog::EM<<"B == "<<B.uVec<<ELog::endTrace;
	  ELog::EM<<"Expect == "<<vc->get<3>()<<ELog::endTrace;
	  ELog::EM<<"B(abs) == "<<B.uVec.abs()<<ELog::endTrace;
	  ELog::EM<<"cos Angle == "<<A.uVec.dotProd(B.uVec)<<ELog::endTrace;
	  ELog::EM<<"Angle == "<<(180.0/M_PI)*acos(A.uVec.dotProd(B.uVec))<<ELog::endTrace;
	  ELog::EM<<"Diff == "<<vc->get<3>()-B.uVec<<ELog::endTrace;
	  return -1;
	}
    }
  return 0;  
}
