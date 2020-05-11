/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   test/testModelSupport.cxx
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
#include <vector>
#include <list>
#include <deque>
#include <set>
#include <map>
#include <stack>
#include <string>
#include <sstream>
#include <algorithm>
#include <tuple>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Surface.h"
#include "Rules.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "HeadRule.h"
#include "Object.h"
#include "generateSurf.h"
#include "ModelSupport.h"

#include "testFunc.h"
#include "testModelSupport.h"


testModelSupport::testModelSupport() 
  /*!
    Constructor
  */
{}

testModelSupport::~testModelSupport() 
  /*!
    Destructor
  */
{
}

void
testModelSupport::addSurfaces(ModelSupport::surfRegister& SMap,
			      const int A,const int B)
  /*!
    Add a set of surfaces between A and B
    \param SMap :: Surface mape to add
						
   */
{
  ELog::RegMethod RegA("testModelSupport","addSurfaces");
  
  Geometry::Vec3D Origin;
  Geometry::Vec3D XAxis(0,1,0);
  
  for(int i=A;i<=B;i++)
    ModelSupport::buildPlane
      (SMap,i,Origin+XAxis*static_cast<double>(i),XAxis);
  return;
}

int 
testModelSupport::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: Test number to run
    \retval -1 : SetObject 
    \retval 0 : All succeeded
  */
{
  ELog::RegMethod RegA("testModelSupport","applyTest");
  TestFunc::regSector("testModelSupport");

  typedef int (testModelSupport::*testPtr)();
  testPtr TPtr[]=
    {
      &testModelSupport::testAltComposite,
      &testModelSupport::testRangeComposite,
      &testModelSupport::testRemoveOpenPair
    };
  const std::string TestName[]=
    {
      "AltComposite",
      "RangeComposite",
      "RemoveOpenPair"
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
testModelSupport::testRemoveOpenPair()
  /*!
    Test to set if various empty strings can be removed
    \return 0 on success
  */
{
  ELog::RegMethod RegA("testModelSupport","testRemoveOpenPair");

  typedef std::tuple<std::string,std::string> TTYPE;
  const std::vector<TTYPE> Tests=
    {
      TTYPE("1 -2 3 -4","1 -2 3 -4"),
      TTYPE("1 -2 () 3 -4","1 -2 3 -4"),
      TTYPE("1 -2 ( 3 : -4)","1 -2 ( 3 : -4 )"),
      TTYPE("1 -2 ( 3 : : -4 :)","1 -2 ( 3 : -4 )"),
      TTYPE("1 -2 ( 3 : ( -4 ( 6 : 5 )))","1 -2 ( 3 : ( -4 ( 6 : 5 ) ) )")
    };
  
  
  for(const TTYPE& tc : Tests)
    {
      const std::string Res=ModelSupport::removeOpenPair(std::get<0>(tc));
      if (StrFunc::singleLine(Res)!=std::get<1>(tc))
	{
	  ELog::EM<<"Result  == "<<StrFunc::singleLine(Res)<<ELog::endTrace;
	  ELog::EM<<"Expect  == "<<std::get<1>(tc)<<ELog::endTrace;
	  return -1;
	}
    }
  return 0;
}

int
testModelSupport::testAltComposite()
  /*!
    Test to set if various empty strings can be removed
    \return 0 on success
  */
{
  ELog::RegMethod RegA("testModelSupport","testRemoveOpenPair");

  ModelSupport::surfRegister SMap;
  addSurfaces(SMap,10,50);
  
  typedef std::tuple<std::string,int,int,int,std::string> TTYPE;
  const std::vector<TTYPE> Tests=
    {
      TTYPE("1 -2 3 -4",10,20,30,"11 -12 13 -14"),
      TTYPE("1 -2 3M -4",10,20,30,"11 -12 23 -14"),
      TTYPE("1 -2 3A -4B",10,20,30,"11 -12 13"),
      TTYPE("1 -2 -4B 3A",10,20,30,"11 -12 13"),
      TTYPE("1 -2 -43A -4B",10,20,30,"11 -12 -14"),
      TTYPE("1 -2 -43MA -4MB",10,20,30,"11 -12 -24")
    };
  
  
  for(const TTYPE& tc : Tests)
    {
      const int BA(std::get<1>(tc));
      const int BB(std::get<2>(tc));
      const int BC(std::get<3>(tc));
      const std::string Res=
	ModelSupport::getAltComposite(SMap,BA,BB,BC,std::get<0>(tc));
      if (StrFunc::singleLine(Res)!=std::get<4>(tc))
	{
	  ELog::EM<<"Result  == "<<StrFunc::singleLine(Res)
		  <<" == "<<ELog::endTrace;
	  ELog::EM<<"Expect  == "<<std::get<4>(tc)
		  <<" == "<<ELog::endTrace;
	  return -1;
	}
    }
  return 0;
}

int
testModelSupport::testRangeComposite()
  /*!
    Test to set if various empty strings can be removed
    \return 0 on success
  */
{
  ELog::RegMethod RegA("testModelSupport","testRemoveOpenPair");

  ModelSupport::surfRegister SMap;
  addSurfaces(SMap,100,50);
  addSurfaces(SMap,500,550);
  addSurfaces(SMap,1500,1550);
  
  typedef std::tuple<std::string,int,int,int,std::string> TTYPE;
  const std::vector<TTYPE> Tests=
    {
     TTYPE("501 -502 503 -504",501,504,0,"1501 -1502 1503 -1504"),
     TTYPE("501R -502R 503R -504R",501,504,0,"1501 -1502 1503 -1504"),
     TTYPE("501R -502R 503R -504R",501,504,1,"1502 -1503 1504 -1501"),
     TTYPE("501R -502R 503R ",501,504,1,"1502 -1503 1504"),
     TTYPE("501R -502R 503R -504R",501,504,-1,"1504 -1501 1502 -1503")
    };
  
  
  for(const TTYPE& tc : Tests)
    {
      const std::string item(std::get<0>(tc));
      const int iBase(std::get<1>(tc));
      const int iEnd(std::get<2>(tc));
      const int iOffset(std::get<3>(tc));
      const std::string Res=
	ModelSupport::getRangeComposite
	(SMap,iBase,iEnd,iOffset,1000,item);

      if (StrFunc::singleLine(Res)!=std::get<4>(tc))
	{
	  ELog::EM<<"Input   == "<<StrFunc::singleLine(item)
		  <<" == "
		  <<iBase<<" to "<<iEnd<<" Off= "<<iOffset<<ELog::endTrace;
	  ELog::EM<<"Result  == "<<StrFunc::singleLine(Res)
		  <<" == "<<ELog::endTrace;
	  ELog::EM<<"Expect  == "<<std::get<4>(tc)
		  <<" == "<<ELog::endTrace;
	  return -1;
	}
    }
  return 0;
}
