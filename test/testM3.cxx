/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   test/testM3.cxx
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
#include <iostream>
#include <iomanip>
#include <cmath>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <stack>
#include <map>
#include <tuple>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "dataSlice.h"
#include "multiData.h"
#include "M3.h"

#include "testFunc.h"
#include "testM3.h"


testM3::testM3() 
  /*!
    Constructor
  */
{}

testM3::~testM3() 
  /*!
    Destructor
  */
{}

int 
testM3::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: parameter to decide test
    \retval -1 :: Fail on angle
  */
{
  ELog::RegMethod RegA("testM3","applyTest");
  TestFunc::regSector("testM3");

  typedef int (testM3::*testPtr)();
  testPtr TPtr[]=
    {
      &testM3::testDiagonalize,
      &testM3::testEchelon
    };
  const std::vector<std::string> TestName=
    {
      "Diagonalize",
      "Echelon"
    };
  
  const size_t TSize(sizeof(TPtr)/sizeof(testPtr));
  if (!extra) 
    {
      TestFunc::writeTests(TestName);
      return 0;
    }
  for(size_t i=0;i<TSize;i++)
    {
      if (extra<0 || static_cast<size_t>(extra)==i+1)
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
testM3::testEchelon()
  /*!
    Tests the row echelon method
    \retval -1 on failure
    \retval 0 :: success 
  */
{
  ELog::RegMethod RegA("testM3","testRowEchelon");
  // test matrix:
  const std::vector<Geometry::M3<double>> testM3({
      {   4,-3,-3,
	  -2,-3,-3,
	  -1, 1, 2 }
    });
  for(Geometry::M3<double> M : testM3)
    {
      M.rowEchelon();
      ELog::EM<<"M == "<<M<<ELog::endDiag;
    }
  return 0;
}


int
testM3::testDiagonalize()
  /*!
    Tests the M3 diagonalization
    \retval -1 on failure
    \retval 0 :: success 
  */
{
  ELog::RegMethod RegA("testM3","testDiagonalize");

  // test matrix:
  //M.rowEchelon();  const std::vector<Geometry::M3<double>> testM3({
  //      {   4,-3,-3,
  //	  -2,-3,-3,
  //	  -1, 1, 2 }
  //    });
  /*  const std::vector<Geometry::M3<double>> testM3({
      {   2, 0, 0,
	  1, 2, -1,
	  1, 3, -2 }
    });
  */
  const std::vector<Geometry::M3<double>> testM3({
      {   0, 0, -2,
	  1, 2, 1,
	  1, 0, 3 }
    });
  
  
  for(const Geometry::M3<double>& M : testM3)
    {
      Geometry::M3<double> Pinvert;
      Geometry::M3<double> Diag;
      Geometry::M3<double> P;
      if (!M.diagonalize(Pinvert,Diag,P))
	{
	  ELog::EM<<"Failed to diagonalize"<<ELog::endDiag;
	  return -1;
	}
      Geometry::M3<double> Out=Pinvert*Diag*P;
      //      ELog::EM<<"P == "<<P*Diag*Pinvert<<ELog::endDiag;
      
      ELog::EM<<"M == "<<M<<ELog::endDiag;
      ELog::EM<<"PI == "<<Pinvert<<ELog::endDiag;
      ELog::EM<<"Diag == "<<Diag<<ELog::endDiag;
      ELog::EM<<"P == "<<P<<ELog::endDiag;
      ELog::EM<<"Out == "<<Out<<ELog::endDiag;

    }

  return 0;
}
