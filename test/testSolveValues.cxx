/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   test/testSolveValues.cxx
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
#include <map>
#include <string>
#include <algorithm>
#include <iterator>
#include <numeric>
#include <tuple>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "PolyFunction.h"
#include "PolyVar.h"
#include "solveValues.h"

#include "testFunc.h"
#include "testSolveValues.h"

using namespace mathLevel;

testSolveValues::testSolveValues() 
  /*!
    Constructor
  */
{}

testSolveValues::~testSolveValues()  
  /*!
    Destructor
  */
{}

int 
testSolveValues::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: test to execute [-1 all]
    \retval -1 Distance failed
    \retval 0 All succeeded
  */
{
  ELog::RegMethod RegA("testSolveValues","applyTest");
  TestFunc::regSector("testSolveValues");
  
  typedef int (testSolveValues::*testPtr)();
  testPtr TPtr[]=
    {
      &testSolveValues::testGetSolutions
    };

  const std::string TestName[]=
    {
      "GetSolutions"
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
testSolveValues::testGetSolutions()
  /*!
    Test a simple PolyVar<2> solution
    \retval 0 :: All passed
  */ 
{
  ELog::RegMethod RegA("testSolveValues","testGetSolutions");
  
  // Poly A/B/C number of solutions : Sum of solutions
  typedef std::tuple<std::string,std::string,
		       std::string,size_t,double> TTYPE;

  const std::vector<TTYPE> Tests=
    {

      TTYPE("-0.69465837z-0.7193398y-379.108596",
	    
	    "0.517449748z^2+(-0.999390827y+21.8953305)z+"
	    "0.482550252y^2-21.1440748y+x^2+52.575x+920.402005",
	    
	    "0.517780408z^2+(-0.998706012y-0.0363556101x+"
	    "4.60265241)z+0.482904825y^2+"
	    "(-0.0376473363x-4.50181022)y+"
	    "0.999314767x^2+1.5680273x-389.022646",			
	    2,1e-2),


      TTYPE("x-23","y-10","z-30",1,1e-2),
      
      TTYPE("x-23","y-10","z^2+y^2-1108.89",2,1e-2),
      
      TTYPE("z^2+y^2+10y+x^2","z^2+y^2-10y+x^2","z^2+y^2+x^2-10x",1,1e-2),
      
      TTYPE("-0.71934z+0.694658y+5.78095",
	    "-0.694658z-0.71934y-177.109",
	    "0.51745z^2+(-0.999391y+4.63118)z+0.48255y^2-4.47227y+x^2-95.7277",
	    2,1e-2)
    };


  PolyVar<3> FXYZ,GXYZ,HXYZ;
  int cnt(1);
  for(const TTYPE& tc : Tests)
    {
      FXYZ.read(std::get<0>(tc));
      GXYZ.read(std::get<1>(tc));
      HXYZ.read(std::get<2>(tc));
      solveValues SV;
      SV.setEquations(FXYZ,GXYZ,HXYZ);
      SV.getSolution();      

      // Add up solutions: 
      const std::vector<Geometry::Vec3D>& Res=SV.getAnswers();


      double sumResult(0.0);
      for(size_t j=0;j<Res.size();j++)
	{
	  const Geometry::Vec3D& Item=Res[j];
	  double sum(0.0);
	  sum+=fabs(FXYZ.substitute(Item.X(),Item.Y(),Item.Z()));
	  sum+=fabs(GXYZ.substitute(Item.X(),Item.Y(),Item.Z()));
	  sum+=fabs(HXYZ.substitute(Item.X(),Item.Y(),Item.Z()));
	  if (sum>sumResult)
	    sumResult=sum;
	}
  
      if (Res.size()!=std::get<3>(tc) || sumResult > std::get<4>(tc) ) 
	{
	  ELog::EM<<"Failed on test"<<cnt<<ELog::endCrit;
	  ELog::EM<<"FXYZ =="<<FXYZ<<ELog::endDiag;
	  ELog::EM<<"GXYZ =="<<GXYZ<<ELog::endDiag;
	  ELog::EM<<"HXYZ =="<<HXYZ<<ELog::endDiag;
	  ELog::EM<<"SumPT == "<<sumResult
		  <<" Exp=="<<std::get<4>(tc)<<ELog::endDiag;

	  ELog::EM<<"Results == ";
	  copy(Res.begin(),Res.end(),
	       std::ostream_iterator<Geometry::Vec3D>
	       (ELog::EM.Estream()," : "));
	  ELog::EM<<ELog::endDiag;

	  for(size_t j=0;j<Res.size();j++)
	    {
	      const Geometry::Vec3D& Item=Res[j];
	      ELog::EM<<"Solution "<<j+1<<ELog::endDiag;
	      ELog::EM<<"FXYZ == "<<FXYZ.substitute(Item.X(),Item.Y(),Item.Z())
		      <<ELog::endDiag;
	      ELog::EM<<"GXYZ == "<<GXYZ.substitute(Item.X(),Item.Y(),Item.Z())
		      <<ELog::endDiag;
	      ELog::EM<<"HXYZ == "<<HXYZ.substitute(Item.X(),Item.Y(),Item.Z())
		      <<ELog::endDiag;
	      ELog::EM<<"----------------------"<<ELog::endDiag;

	    }
	  
	  return -1;
	}
      cnt++;
    }
  
  return 0;
}


