/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   test/testPoly.cxx
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
#include <stack>
#include <algorithm>
#include <tuple>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "PolyFunction.h"
#include "PolyVar.h"

#include "testFunc.h"
#include "testPoly.h"

using namespace mathLevel;

testPoly::testPoly() 
  /*!
    Constructor
  */
{}

testPoly::~testPoly()  
  /*!
    Destructor
  */
{}

int 
testPoly::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: test to execute [-1 all]
    \retval -1 Distance failed
    \retval 0 All succeeded
  */
{
  ELog::RegMethod RegA("testPoly","applyTest");
  TestFunc::regSector("testPoly");

  typedef int (testPoly::*testPtr)();
  testPtr TPtr[]=
    {
      &testPoly::testAddition,             
      &testPoly::testBase,
      &testPoly::testBezout,
      &testPoly::testBezoutFail,
      &testPoly::testCopy,
      &testPoly::testDurandKernerRoots,
      &testPoly::testEqualTemplate,
      &testPoly::testExpand,
      &testPoly::testGetMaxSize,
      &testPoly::testGetVarFlag,
      &testPoly::testMinimalReduction,
      &testPoly::testMultiplication,
      &testPoly::testRead,
      &testPoly::testSetComp,
      &testPoly::testSingleVar,
      &testPoly::testSubVariable,
      &testPoly::testTriplet,
      &testPoly::testVariable
    };

  const std::string TestName[]=
    {
      "Addition",
      "Base",
      "Bezout",
      "BezoutFail",
      "Copy",
      "DurandKernerRoots",
      "EqualTemplate",
      "Expand",
      "GetMaxSize",
      "GetVarFlag",
      "MinimalReduction", 
      "Multiplication",
      "Read",
      "SetComp",
      "SingleVar",
      "SubVariable",
      "Triplet",
      "Variables"            
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
testPoly::testAddition()
  /*!
    Test the SetComponent to make an interesting function
    \retval 0 :: All passed
  */ 
{
  ELog::RegMethod RegA("testPoly","testAddition");

  // size/
  typedef std::tuple<int,std::string,int,
		     std::string,std::string> TTYPE;
  std::vector<TTYPE> Tests;
  
  
  Tests.push_back(TTYPE(3,"3z^2+4z",2,"2.2y^2+5.3y",
			"3z^2+4z+2.2y^2+5.3y"));
  
  std::ostringstream cx;
  for(const TTYPE& tc : Tests)
    {
      cx.str("");
      const int iA=std::get<0>(tc);
      const int iB=std::get<2>(tc);

      if (iA==3 && iB==2)
	{
	  PolyVar<3> FXYZ;
	  PolyVar<2> GXY;
	  PolyVar<3> HXYZ;
	  FXYZ.read(std::get<1>(tc));
	  GXY.read(std::get<3>(tc));
	  HXYZ=FXYZ.operator+(GXY);
	  cx<<HXYZ;

	}
      if (cx.str()!=std::get<4>(tc))
	{
	  ELog::EM<<"HXYZ   == "<<cx.str()<<":"<<ELog::endErr;
	  ELog::EM<<"Expect == "<<std::get<4>(tc)<<":"<<ELog::endErr;
	  return -1;
	}
    }
  return 0;
}

int
testPoly::testBase()
  /*!
    Test the distance of a point from the cone
    \retval -1 :: failed build a cone
    \retval 0 :: All passed
  */
{

  return 0;
}

int
testPoly::testBezout()
  /*!
    Test the Bezout reduction (only 2x2)
    \retval 0 :: All passed
  */ 
{
  ELog::RegMethod RegA("testPoly","testBezout");
  // TEST 1:
  PolyVar<2> FXY; 
  PolyVar<2> GXY; 

  // Setting by variable:
 
  FXY.read("y+x^2+x+1");
  GXY.read("xy+2x+5");
  PolyVar<1> Out=FXY.reduce(GXY);
  Out.unitPrimary();
  std::ostringstream cx;
  cx<<Out;
  if (cx.str()!="x^3+x^2-x-5")
    {
      ELog::EM<<"GXY "<<GXY<<ELog::endDiag;
      ELog::EM<<"Out ="<<Out<<ELog::endErr;
      ELog::EM<<"Expected "<<"x^3+x^2-x-5"<<ELog::endErr;
      return -1;
    }

  // TEST 2:
  FXY.read("y^2-9x-9");
  GXY.read("y^3-27");
  Out=GXY.reduce(FXY);
  cx.str("");
  Out.unitPrimary();
  cx<<Out;
  if (cx.str()!="x^3+3x^2+3x")
    {
      ELog::EM<<"FXY ="<<FXY<<ELog::endWarn;
      ELog::EM<<"GXY ="<<GXY<<ELog::endWarn;
      ELog::EM<<"Out ="<<Out<<ELog::endWarn;
      ELog::EM<<"Expected "<<"x^3+3x^2+3x"<<ELog::endWarn;
      return -2;
    }

  // TEST 3 : 

  PolyVar<3> FXYZ; 
  PolyVar<3> GXYZ; 
  PolyVar<3> HXYZ; 
  
  int errFlag(0);

  FXYZ.read("3z+y+x-4");
  GXYZ.read("2x-3z+y-4");
  HXYZ.read("2x-3z+y-4");
  PolyVar<2> OutX=FXYZ.reduce(GXYZ);
  PolyVar<2> OutY=FXYZ.reduce(HXYZ);
  PolyVar<1> OutFinal=OutX.reduce(OutY);
  std::vector<double> rR=OutFinal.realRoots();
  for(size_t i=0;i<rR.size();i++)
    {      
      PolyVar<1> SA=OutX.substitute(rR[i]);
      std::vector<double> sS=SA.realRoots();
      for(unsigned int j=0;j<sS.size();j++)
        {
	  PolyVar<1> ZA=FXYZ.substitute<1>(rR[i],sS[j]);
	  std::vector<double> tT=ZA.realRoots();
	  const double Results=GXYZ.substitute(rR[i],sS[j],tT[0]);
	  if (fabs(Results)>1e-5)
	    errFlag=1;
	}
    }
  
  if (errFlag)
    {
      ELog::EM<<"FXYZ == "<<FXYZ<<ELog::endDiag;
      ELog::EM<<"GXYZ == "<<GXYZ<<ELog::endDiag;
      ELog::EM<<"HXYZ == "<<HXYZ<<ELog::endDiag;
      ELog::EM<<"OX ="<<OutX<<ELog::endDiag;
      ELog::EM<<"OY ="<<OutY<<ELog::endDiag;
      ELog::EM<<"Final == "<<OutFinal<<ELog::endDiag;
      
      for(size_t i=0;i<rR.size();i++)
	{
	  PolyVar<1> SA=OutX.substitute(rR[i]);
	  ELog::EM<<"Sub == "<<SA<<ELog::endDiag;
	  std::vector<double> sS=SA.realRoots();
	  for(unsigned int j=0;j<sS.size();j++)
	    {
	      ELog::EM<<"Y == "<<j<<" "<<sS[j]<<ELog::endDiag;
	      PolyVar<1> ZA=FXYZ.substitute<1>(rR[i],sS[j]);
	      ELog::EM<<"ZA == "<<ZA<<ELog::endDiag;
	      std::vector<double> tT=ZA.realRoots();
	      ELog::EM<<"TA == "<<tT[0]<<ELog::endDiag;
	      const double Results=GXYZ.substitute(rR[i],sS[j],tT[0]);
	      ELog::EM<<"RESULT for ("<<rR[i]<<","
		      <<sS[j]<<","<<tT[0]<<") == "<<Results<<ELog::endDiag;
	    }
	}
      return -3;
    }

  return 0;
}

int
testPoly::testBezoutFail()
  /*!
    Test of Bezout suppost to fail : 
    -- It should exit elegently
    \return 0 on succes / -1 on failure
  */
{
  ELog::RegMethod("testPoly","testBezoutFail");

  PolyVar<3> FXYZ;
  PolyVar<3> GXYZ;
  PolyVar<3> HXYZ;

  FXYZ.read("-0.69465837z-0.7193398y-379.108596");
  GXYZ.read("0.517449748z^2+(-0.999390827y+21.8953305)z+"
	    "0.482550252y^2-21.1440748y+x^2+52.575x+920.402005");
  HXYZ.read("0.517780408z^2+(-0.998706012y-0.0363556101x+4.60265241)z"
	    "+0.482904825y^2+(-0.0376473363x-4.50181022)y+"
	    "0.999314767x^2+1.5680273x-389.022646");

  

  // mathLevel::solveValues SV;
  // SV.setEquations(FXYZ,GXYZ,HXYZ);
  // SV.getSolution();
  // ELog::EM<<"Solutions == "<<SV.getAnswers()<<ELog::endDebug;
  // return 0;


  // REAL TEST:

  // Test of two non-intesecting cylinders
  FXYZ.read("-y");
  GXYZ.read("0.0918274z^2+0.0918274x^2-1");
  HXYZ.read(" z^2+x^2-9.61");

  mathLevel::PolyVar<3> masterXYZ(FXYZ);
  if (masterXYZ.expand(GXYZ) && masterXYZ.expand(HXYZ))
    return 0;


  mathLevel::PolyVar<2> OutX=FXYZ.reduce(GXYZ);
  mathLevel::PolyVar<2> OutY=FXYZ.reduce(HXYZ);
  
  mathLevel::PolyVar<2> masterXY(OutX);
  if (masterXY.expand(OutY))
    return 0;

  return -1;
}

int
testPoly::testCopy()
  /*!
    Test the building of a PolyValue
    \return error number / 0 on success
   */
{
  ELog::RegMethod RegA("testPoly","testCopy");

  // TEST 1:
  const double valB[]={0,0,0,0,0,0,0,0,-1,0};
  std::vector<double> Pts(10);
  copy(valB,valB+10,Pts.begin());
  PolyVar<3> FXYZ;
  FXYZ.makeTriplet(Pts);
  PolyVar<3> GXYZ(FXYZ);
  if (GXYZ.isComplete() && GXYZ==FXYZ)
    {
      ELog::EM<<"GXYZ isComplete "<<GXYZ.isComplete()<<ELog::endDebug;
      ELog::EM<<"GXYZ  "<<GXYZ<<ELog::endDebug;
      return -1;
    }
  return 0;
}


int
testPoly::testDurandKernerRoots()
  /*!
    Test the determination of roots
    \return error number / 0 on success
   */
{
  ELog::RegMethod RegA("testPoly","testCopy");

  // Function / Nroots / smallest / largest roots 
  typedef std::tuple<std::string,size_t,std::complex<double>,
		     std::complex<double> > TTYPE;
  typedef std::complex<double> CX;
  std::vector<TTYPE> Tests =
    {
      TTYPE("x^2+x-6",2,CX(-3.0,0.0),CX(2.0,0.0)),
      TTYPE("x^3-9x^2+26x-24",3,CX(2.0,0.0),CX(4.0,0.0)),
      TTYPE("x^4-14x^3+71x^2-154x+120",4,CX(2.0,0.0),CX(5.0,0.0)),
      TTYPE("x^4-11x^3+44x^2-76x+48",4,CX(2.0,0.0),CX(4.0,0.0))  // repeat roots
    };

  PolyVar<1> FX;
  std::vector<CX> Res;
  for(const TTYPE& tc : Tests)
    {
      FX.read(std::get<0>(tc));
      Res=FX.calcDurandKernerRoots();
      std::sort(Res.begin(),Res.end(),
	   [](const CX& A,const CX& B)
	   {
	     return A.real()<B.real();
	   });

      const size_t nSolution=Res.size();
      const double dA=(nSolution) ?
	sqrt(std::norm(Res.front()-std::get<2>(tc))) : 0.0;
      const double dB=(nSolution>1) ?
	sqrt(std::norm(Res.back()-std::get<3>(tc))) : 0.0;
      
      if (nSolution!=std::get<1>(tc) ||
	  fabs(dA)>1e-6 || fabs(dB)>1e-6)
	{
	  ELog::EM<<"FX == "<<FX<<ELog::endDiag;
	  for(const CX& ans : Res)
	    ELog::EM<<"Res == "<<ans<<ELog::endDiag;
	  return -1;
	}
    }
  
  return 0;
}


int
testPoly::testEqualTemplate()
  /*!
    Test operator=
    \retval -1 :: Vailes to process PolyVar
    \retval 0 :: All passed
  */ 
{
  ELog::RegMethod RegA("testPoly","testEqualTemplate");

  
  PolyVar<3> GXYZ(2); 
  PolyVar<1> FX(2);

  // Setting by variable:
  std::vector<double> C;
  C.push_back(1.1);
  C.push_back(2.2);
  C.push_back(3.3);
  FX=C;
  //  (std::vector<double>&) FX=C;

  GXYZ=FX;
  std::ostringstream ax;
  std::ostringstream bx;
  ax<<GXYZ;
  bx<<FX;
  if (ax.str()!=bx.str())
    {
      ELog::EM<<"GXYZ == "<<GXYZ<<ELog::endDiag;
      ELog::EM<<"FX == "<<FX<<ELog::endDiag;
      return -1;
    }
  return 0;
}


int
testPoly::testExpand()
  /*!
    Test the Expansion combinations
    \retval 0 :: All passed
  */ 
{
  ELog::RegMethod RegA("testPoly","testExpand");  

  PolyVar<3> FXYZ; 
  PolyVar<3> GXYZ; 

  typedef std::tuple<std::string,std::string,std::string> TTYPE;
  std::vector<TTYPE> Tests;
    
  Tests.push_back(TTYPE("x-32","y+10","y+x-22"));
  Tests.push_back(TTYPE("x-23","y-10","y+x-33"));
  Tests.push_back(TTYPE("-z","z^2+x^2-1","z^2-z+x^2-1"));
  Tests.push_back(TTYPE(
      "-z",
      "z^2+(-y-x+5)z+0.5y^2+(-9x-10)y+0.4x^2+9x+45",
      "z^2+(-y-x+5)z+0.5y^2+(-9x-10)y+0.4x^2+9x+45"));

  for(const TTYPE& tc : Tests)
    {
      if (FXYZ.read(std::get<0>(tc)))
        {
	  ELog::EM<<"Failed to process object :"
		  <<std::get<0>(tc)<<ELog::endErr;
	  return -1;
	}
      if (GXYZ.read(std::get<1>(tc)))
        {
	  ELog::EM<<"Failed to process object :"
		  <<std::get<1>(tc)<<ELog::endErr;
	  return -1;
	}
      FXYZ.expand(GXYZ);
      std::ostringstream cx;
      cx<<FXYZ;
      if (cx.str()!=std::get<2>(tc))
        {
	  ELog::EM<<"Items: "<<std::get<0>(tc)<<" :: "
		  <<std::get<1>(tc)<<ELog::endErr;
	  ELog::EM<<"Data: "<<FXYZ<<" :: "
		  <<std::get<2>(tc)<<" :: "<<ELog::endErr;
	  return -1;
	}
    }
  return 0;
}

int
testPoly::testGetMaxSize()
  /*!
    Test of the getMaxSize routine
    \retval 0 :: success 
    \retval -1 :: failure
   */
{
  ELog::RegMethod RegA("testPoly","testGetMaxSize");


  std::string Line="y+(x+3)y^3+(x+x^5)z^5+3.0y^2";
  const size_t Index=PolyFunction::getMaxSize(Line,'y');
  
  if (Index!=3)
    {
      ELog::EM<<"Index "<<Index<<ELog::endErr;
      return -1;
    }

  return 0;
}

int
testPoly::testGetVarFlag()
  /*!
    Test the flag of the explicit variables present
    \retval 0 :: All passed
  */ 
{
  ELog::RegMethod RegA("testPoly","testGetFlag");  

  PolyVar<3> FXYZ; 
  typedef std::pair<std::string,size_t> TTYPE;
  std::vector<TTYPE> testItem;
    
  testItem.push_back(TTYPE("z^2+3xz+x-16",5));
  testItem.push_back(TTYPE("z^2+3xy+x-16",7));
  testItem.push_back(TTYPE("x^2+3xy+x-16",3));
  testItem.push_back(TTYPE("z^2+3zy+z-16",6));
  testItem.push_back(TTYPE("z^2+3zy+z",6));  
  testItem.push_back(TTYPE("z^2",4));
  testItem.push_back(TTYPE("y^2",2));
  testItem.push_back(TTYPE("y-23",2));
  testItem.push_back(TTYPE("x-16",1));
  testItem.push_back(TTYPE("-6",0));

  std::vector<TTYPE>::const_iterator tc;
  for(tc=testItem.begin();tc!=testItem.end();tc++)
    {
      if (FXYZ.read(tc->first))
        {
	  ELog::EM<<"Failed to process object :"
		  <<tc->first<<ELog::endErr;
	  return -1;
	}
      const size_t flag=FXYZ.getVarFlag();
      if (flag!=tc->second)
        {
	  ELog::EM<<"Item: "<<tc->first
		  <<" == "<<flag<<" (expect=="
		  <<tc->second<<")"<<ELog::endErr;
	  return -1;
	}
    }
    return 0;
}

int
testPoly::testMinimalReduction()
  /*!
    Test a reduction of a set of polynomials when a minimalization 
    can use subVariable to reduce the number of variables
    \return -ve on Failure
  */
{
  ELog::RegMethod RegA("testPoly","testMinimalReduction");
  PolyVar<3> FXYZ;
  PolyVar<3> GXYZ;
  PolyVar<3> HXYZ;
  FXYZ.read("x-23");
  GXYZ.read("y-10");
  HXYZ.read("z^2+y^2-1108.89");

  //PolyVar<3> Array[]={FXYZ,GXYZ,HXYZ};
  //  unsigned int flag;

  return 0;
}


int
testPoly::testMultiplication()
  /*!
    Test the SetComponent to make an interesting function
    \retval 0 :: All passed
  */ 
{
  ELog::RegMethod RegA("testPoly","testMultiplication");
  std::stringstream cx;
  PolyVar<1> FX;
  PolyVar<1> GX;
  PolyVar<1> HX;
  PolyVar<2> FXY;
  PolyVar<2> GXY;
  PolyVar<2> HXY;
  PolyVar<2> JXY;

  // SINGLE
  FX.read("x^2+5x+3");
  GX.read("x-2");
  HX=FX*GX;
  cx<<HX;
  if (cx.str()!="x^3+3x^2-7x-6")
    {
      ELog::EM<<"SINGLE MULT"<<ELog::endDiag;
      ELog::EM<<FX<<ELog::endDiag;
      ELog::EM<<GX<<ELog::endDiag;
      ELog::EM<<HX<<ELog::endDiag;
      return -1;
    }

  // DOUBLE
  cx.str("");
  FXY.read("x^2+5x+3");
  GXY.read("y-2");
  HXY=FXY*GXY;
  cx<<HXY;
  if (cx.str()!="(x^2+5x+3)y-2x^2-10x-6")
    {
      ELog::EM<<"Double MULT"<<ELog::endDiag;
      ELog::EM<<FXY<<ELog::endDiag;
      ELog::EM<<GXY<<ELog::endDiag;
      ELog::EM<<HXY<<ELog::endDiag;
      return -2;
    }

  // MORE DOUBLE [x+y and a zero sum]
  cx.str("");
  FXY.read("xy+x^2");
  GXY.read("y^2+y-1");
  HXY=FXY*GXY;
  cx<<HXY;
  if (cx.str()!="xy^3+(x^2+x)y^2+(x^2-x)y-x^2")
    {
      ELog::EM<<"Double MULT"<<ELog::endErr;
      ELog::EM<<FXY<<ELog::endCrit;
      ELog::EM<<GXY<<ELog::endCrit;
      ELog::EM<<HXY<<ELog::endCrit;
      return -3;
    }
  // CHECK LAPLACE ERROR:
  cx.str("");
  FXY.read("2x+5");
  GXY.read("x");
  HXY.read("x^2+x+1");
  JXY.read("1");
  FXY*=JXY;
  GXY*=HXY;
  GXY-=FXY;
  cx<<GXY;
  if (cx.str()!="x^3+x^2-x-5")
    {
      ELog::EM<<"LAPLACE MULT"<<ELog::endDiag;
      ELog::EM<<FXY<<ELog::endErr;
      ELog::EM<<GXY<<ELog::endErr;
      ELog::EM<<HXY<<ELog::endErr;
      return -4;
    }

  return 0;
}

int
testPoly::testRead()
  /*!
    Test the read function
    \retval 0 :: scucess
   */
{
  ELog::RegMethod RegA("testPoly","testRead");

  std::stringstream cx;
  std::vector<std::string> TLine;
  std::vector<std::string> OLine;
  std::vector<double> Value;

  // String : Size : x : y : z : result
  typedef std::tuple<int,std::string,
		       double,double,double,std::string,double> TTYPE;
  std::vector<TTYPE> Tests;
  
  Tests.push_back(TTYPE(1,"-1.0x^3-x+3.4",2.0,3.0,4.0,
			"-x^3-x+3.4",-6.6));
  // Double
  Tests.push_back(TTYPE(2,"3y^2-6",2.0,3.0,4.0,
			"3y^2-6",21.0));
  Tests.push_back(TTYPE(2,"(x^3+3.4)y^2-(x^4+3)y+x^2+6",2.0,3.0,4.0,
			"(x^3+3.4)y^2+(-x^4-3)y+x^2+6",55.6)); 
  Tests.push_back(TTYPE(2,"-(x^3-3.4)y^2-y-x^2+6",2.0,3.0,4.0,
			"(-x^3+3.4)y^2-y-x^2+6",-42.4));       
  Tests.push_back(TTYPE(2,"-1.0y-x^2+6",2.0,3.0,4.0,
			"-y-x^2+6",-1.0));   
  // Triple
  Tests.push_back(TTYPE(3,"z^2+xyz-1.0y-x^2+6",2.0,3.0,4.0,
			"z^2+xyz-y-x^2+6",39.0));   
  Tests.push_back(TTYPE(3,"z^2+y^2z-y-x^2+6",2.0,3.0,4.0,
			"z^2+y^2z-y-x^2+6",51.0));   
  Tests.push_back(TTYPE(3,"z+y-x+6",2.0,3.0,4.0,"z+y-x+6",11.0));   
  Tests.push_back(TTYPE(3,"-x+y+z+6",2.0,3.0,4.0,"z+y-x+6",11.0));   

  PolyVar<1> FX;
  PolyVar<2> GXY;
  PolyVar<3> HXYZ;

  const char* outName[]={"SINGLE","DOUBLE","TRIPLE"};

  int errFlag(0);
  double Values[3];

  for(const TTYPE& tc : Tests)
    {
      const int VarNum(std::get<0>(tc));
      Values[0]=std::get<2>(tc);
      Values[1]=std::get<3>(tc);
      Values[2]=std::get<4>(tc);
      cx.str("");
      if (VarNum==1)
	{
	  cx.str("");
	  FX.read(std::get<1>(tc));
	  cx<<FX;
	  if (cx.str()!=std::get<5>(tc) || 
	      fabs(FX(Values)-std::get<6>(tc))>1e-5)
	    errFlag=1;
	  }
      if (VarNum==2)
	{
	  cx.str("");
	  GXY.read(std::get<1>(tc));
	  cx<<GXY;
	  if (cx.str()!=std::get<5>(tc) || 
	      fabs(GXY(Values)-std::get<6>(tc))>1e-5)
	    errFlag=2;
	}
      if (VarNum==3)
	{
	  cx.str("");
	  HXYZ.read(std::get<5>(tc));
	  cx<<HXYZ;
	  if (cx.str()!=std::get<5>(tc) || 
	      fabs(HXYZ(Values)-std::get<6>(tc))>1e-5)
	    errFlag=3;
	}
      if (errFlag)
	{
	  TestFunc::bracketTest(outName[std::get<0>(tc)-1],ELog::EM.Estream());
	  ELog::EM<<"VarCount : "<<std::get<0>(tc)<<ELog::endErr;	      
	  ELog::EM<<"Input : "<<std::get<1>(tc)<<" ::"<<ELog::endErr;
	  ELog::EM<<"Expect: "<<std::get<5>(tc)<<" ::"<<ELog::endErr;
	  ELog::EM<<"String: "<<cx.str()<<" ::"<<ELog::endErr;
	  if (std::get<0>(tc)==1)
	    {
	      ELog::EM<<"FX    : "<<FX<<ELog::endErr;
	      ELog::EM<<"Eval == "<<FX(std::get<2>(tc))
		      <<" ("<<std::get<6>(tc)<<")"<<ELog::endErr;
	    }
	  else if (std::get<0>(tc)==2)
	    {
	      ELog::EM<<"GXY    : "<<GXY<<ELog::endErr;
	      ELog::EM<<"Eval == "<<GXY(std::get<2>(tc),std::get<3>(tc))
		      <<" ("<<std::get<6>(tc)<<")"<<ELog::endErr;
	    }
	  else if (std::get<0>(tc)==3)
	    {
	      ELog::EM<<"HXYZ    : "<<HXYZ<<ELog::endErr;
	      ELog::EM<<"Eval == "
		      <<HXYZ(std::get<2>(tc),std::get<3>(tc),std::get<4>(tc))
		      <<" ("<<std::get<6>(tc)<<")"<<ELog::endErr;
	      ELog::EM<<"Value == "
		      <<HXYZ(Values)
		      <<" ("<<std::get<6>(tc)<<")"<<ELog::endErr;
	    }
	  return -1;
	}
    }

  return 0;
}

int
testPoly::testSetComp()
  /*!
    Test the SetComponent to make an interesting function
    \retval 0 :: All passed
  */ 
{
  ELog::RegMethod RegA("testPoly","testSetComp");
  
  // size/
  // Comp : value : current value : clear
  typedef std::tuple<size_t,double,std::string,bool> TTYPE;
  std::vector<TTYPE> Tests;
    
  Tests.push_back(TTYPE(1,4.0,"4z",0));
  Tests.push_back(TTYPE(2,3.0,"3z^2+4z",0));

  PolyVar<3> GXYZ(3);
  for(const TTYPE& tc : Tests)
    {
      std::ostringstream cx;
      GXYZ.setComp(std::get<0>(tc),std::get<1>(tc));
      cx<<GXYZ;
      GXYZ.setComp(2,3.0);
      if (cx.str()!=std::get<2>(tc))
	{
	  ELog::EM<<"Expect   :"<<std::get<2>(tc)<<ELog::endDiag;
	  ELog::EM<<"Current  :"<<GXYZ<<ELog::endDiag;
	  return -1;
	}
    }
  return 0;
}

int 
testPoly::testSingleVar()
  /*!
    Test a single value
    \return 0 on success
   */
{
  ELog::RegMethod RegA("testPoly","testSingleVar");

  PolyVar<3> FXYZ;
  typedef std::vector<std::pair<std::string,std::string> > TTYPE;
  TTYPE testItem;

  testItem.push_back(TTYPE::value_type("3x^2+x-16","3x^2+x-16"));
  testItem.push_back(TTYPE::value_type("3y^2+y-16","3x^2+x-16"));
  testItem.push_back(TTYPE::value_type("y-16","x-16"));
  testItem.push_back(TTYPE::value_type("z^2+z-16","x^2+x-16"));

  TTYPE::const_iterator tc;
  for(tc=testItem.begin();tc!=testItem.end();tc++)
    {
      if (FXYZ.read(tc->first))
        {
	  ELog::EM<<"Failed to process object :"
		  <<tc->first<<ELog::endErr;
	  return -1;
	}
      try
	{
	  PolyVar<1> SV=FXYZ.singleVar();
	  std::ostringstream cx;
	  cx<<SV;
	  if (cx.str()!=tc->second)
	    {
	      ELog::EM<<"Item: "<<tc->first
		      <<" == "<<cx.str()<<" (expect=="
		      <<tc->second<<")"<<ELog::endErr;
	      return -3;
	    }
	}
      catch (ColErr::ExBase& A)
	{
	  ELog::EM<<"Failed with exception "<<A.what()<<ELog::endErr;
	  return -2;
	}
    }
  return 0;
}

int
testPoly::testSubVariable()
  /*!
    Test the reduction of the polynominal by a variable value.
    It places a value in the variable.
    \retval -1 :: failed
    \retval 0 :: All passed
  */
{
  ELog::RegMethod RegA("testPoly","testSubVariable");

  typedef std::tuple<std::string,size_t,double,std::string> TTYPE;
  std::vector<TTYPE> Tests;

  Tests.push_back(TTYPE("(6xy^2+4x)z^3+(y+x^2+3)z+x-4",2,
			3.0,"162xy^2+3y+3x^2+109x+5"));

  Tests.push_back(TTYPE("(6xy^2+4x)z^3+(y+x^2+3)z+x-4",0,-2.0,
			"(-12x^2-8)y^3+(x+7)y-6"));


  for(const TTYPE& tc : Tests)
    {
      PolyVar<3> FXYZ;
      FXYZ.read(std::get<0>(tc));
      PolyVar<2> outXY=FXYZ.subVariable(std::get<1>(tc),std::get<2>(tc));
      std::ostringstream cx;
      cx<<outXY;
      if (cx.str()!=std::get<3>(tc))
	{
	  ELog::EM<<"FXYZ == "<<FXYZ<<ELog::endTrace;
	  ELog::EM<<"Substitution on "<<std::get<1>(tc)
		  <<" == "<<std::get<2>(tc)<<ELog::endTrace;
	  ELog::EM<<"Out == "<<outXY<<ELog::endTrace;
	  return -1;
	}
    }
   
  return 0;
}

int
testPoly::testTriplet()
  /*!
    Test the building of a PolyValue for a 
    XYZ value from a 10 pointsystem
    \return error number / 0 on success
   */
{
  ELog::RegMethod RegA("testPoly","testTriplet");

  PolyVar<3> FXYZ;
  typedef std::tuple<const double*,double,double,double,double> TTYPE;

  std::vector<TTYPE> Tests;

  const double valA[]={1,2,3,4,5,6,7,8,9,10};
  const double valB[]={0,0,0,0,0,0,0,0,-1,0};
  Tests.push_back(TTYPE(valA,2,3,4,290));
  Tests.push_back(TTYPE(valB,2,3,4,-4));

  std::vector<double> Pts(10);

  for(const TTYPE& tc : Tests)
    {
      copy(std::get<0>(tc),std::get<0>(tc)+10,Pts.begin());

      PolyVar<3> FXYZ;
      FXYZ.makeTriplet(Pts);
      
      const double result=
	FXYZ.substitute(std::get<1>(tc),std::get<2>(tc),std::get<3>(tc));
      if (fabs(result-std::get<4>(tc))>1e-5)
	{
	  ELog::EM<<"FXYZ == "<<FXYZ<<ELog::endTrace;
	  ELog::EM<<"Result["<<std::get<1>(tc)<<","
		  <<std::get<2>(tc)<<","<<std::get<3>(tc)
		  <<"] == "<<result<<ELog::endTrace;
	  ELog::EM<<"Expected == "<<std::get<4>(tc)<<ELog::endTrace;
	  return -1;
	}
    }
  return 0;
}
  
int
testPoly::testVariable()
  /*!
    Test the setting of a variable
    \retval 0 :: All passed
  */ 
{
  ELog::RegMethod RegA("testPoly","testVariable");
  typedef std::tuple<std::string,size_t,std::string,bool> TTYPE;
  std::vector<TTYPE> Tests;
  Tests.push_back(TTYPE("3.3x^2+2.2x+1.1",1,"(3.3x^2+2.2x+1.1)y",0));
  Tests.push_back(TTYPE("4.4",0,"(3.3x^2+2.2x+1.1)y+4.4",0));
  
  PolyVar<2> GXY(2); 
  int cnt(1);
  for(const TTYPE& tc : Tests)
    {
      PolyVar<1> FX;
      FX.read(std::get<0>(tc));
      GXY.setComp(std::get<1>(tc),FX);
      std::ostringstream cx;
      cx<<GXY;
      if (cx.str()!=std::get<2>(tc))
	{
	  ELog::EM<<"Test "<<cnt<<ELog::endTrace;
	  ELog::EM<<"Expected : "<<std::get<2>(tc)<<ELog::endTrace;
	  ELog::EM<<"Obtained : "<<cx.str()<<ELog::endTrace;
	  return -1;
	}
      if (std::get<3>(tc))
	GXY.zeroPoly();

      cnt++;
    }
  
  return 0;
}
  
