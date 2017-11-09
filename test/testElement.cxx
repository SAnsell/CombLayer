/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   test/testElement.cxx
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
#include <vector>
#include <list>
#include <map>
#include <stack>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "Zaid.h"
#include "Element.h"

#include "testFunc.h"
#include "testElement.h"

testElement::testElement() 
  /*!
    Constructor
  */
{}

testElement::~testElement() 
  /*!
    Destructor
  */
{}

int 
testElement::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: Index for test
    \retval -1 Range failed
    \retval 0 All succeeded
  */
{
  ELog::RegMethod RegA("testElement","applyTest");
  TestFunc::regSector("testElement");

  typedef int (testElement::*testPtr)();
  testPtr TPtr[]=
    {
      &testElement::testCinder,
      &testElement::testMass
    };
  const std::string TestName[]=
    {
      "Cinder",
      "Mass"
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
testElement::testCinder() 
  /*!
    Test the cinder build of the elements
    \retval -1 :: Unable to process line
    \retval 0 :: success
  */
{
  ELog::RegMethod RegA("testElement","testCinder");

  std::vector<double> cFrac;
  std::vector<size_t> cZ;
  const MonteCarlo::Element& TX=MonteCarlo::Element::Instance();
  MonteCarlo::Zaid c31;
  c31.setZaid("31000.24c ");
  c31.setDensity(1.0);
  TX.addZaid(c31,cZ,cFrac);
  
  if (cZ.size()!=2 || cZ[0]!=31069 || cZ[1]!=31071 ||
      fabs(cFrac[0]+cFrac[1]-1.0)>1e-5)
    {
      ELog::EM<<"Failed on get cinder element :"<<ELog::endWarn;
      for(size_t i=0;i<cZ.size();i++)
	ELog::EM<<"Z == "<<cZ[i]<<" "<<cFrac[i]<<ELog::endTrace;
      return -1;
    }
  return 0;
}


int
testElement::testMass() 
  /*!
    Test the mean atomic mass
    \retval -1 :: Unable to process line
    \retval 0 :: success
  */
{
  ELog::RegMethod RegA("testElement","testMass");
  // Masses from Mucal:
  static double meanAMASS[]=
    {
      0.100800E+01,0.400300E+01,0.694000E+01,0.901200E+01,0.108110E+02,
      0.120100E+02,0.140080E+02,0.160000E+02,0.190000E+02,0.201830E+02,
      0.229970E+02,0.243200E+02,0.269700E+02,0.280860E+02,0.309750E+02,
      0.320660E+02,0.354570E+02,0.399440E+02,0.391020E+02,0.400800E+02,
      0.449600E+02,0.479000E+02,0.509420E+02,0.519960E+02,0.549400E+02,
      0.558500E+02,0.589330E+02,0.586900E+02,0.635400E+02,0.653800E+02,
      0.697200E+02,0.725900E+02,0.749200E+02,0.789600E+02,0.799200E+02,
      0.838000E+02,0.854800E+02,0.876200E+02,0.889050E+02,0.912200E+02,
      0.929060E+02,0.959500E+02,0.000000E+00,0.101070E+03,0.102910E+03,
      0.106400E+03,0.107880E+03,0.112410E+03,0.114820E+03,0.118690E+03,
      0.121760E+03,0.127600E+03,0.126910E+03,0.131300E+03,0.132910E+03,
      0.137360E+03,0.138920E+03,0.140130E+03,0.140920E+03,0.144270E+03,
      0.000000E+00,0.150350E+03,0.152000E+03,0.157260E+03,0.158930E+03,
      0.162510E+03,0.164940E+03,0.167270E+03,0.168940E+03,0.173040E+03,
      0.174990E+03,0.178500E+03,0.180950E+03,0.183920E+03,0.186200E+03,
      0.190200E+03,0.192200E+03,0.195090E+03,0.196965E+03,0.200610E+03,
      0.204390E+03,0.207210E+03,0.209000E+03,0.000000E+00,0.000000E+00,
      0.000000E+00,0.000000E+00,0.000000E+00,0.000000E+00,0.232000E+03,
      0.000000E+00,0.238070E+03,0.000000E+00,0.000000E+00 
    };

  const MonteCarlo::Element& TX=MonteCarlo::Element::Instance();
  for(size_t i=1;i<95;i++)
    {
      if (std::abs(TX.mass(i)-meanAMASS[i-1])>1e-1)
	{
	  ELog::EM<<i<<" == "<<TX.mass(i)<<" "<<meanAMASS[i-1]<<ELog::endTrace;
	  return -1;
	}
    }


  return 0;
}

