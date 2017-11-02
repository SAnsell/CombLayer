/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   test/testFunction.cxx
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
#include <climits>
#include <cmath>
#include <list>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <iterator>
#include <tuple>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "support.h"
#include "funcList.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"

#include "testFunc.h"
#include "testFunction.h"

testFunction::testFunction() 
  /*!
    Constructor
  */
{}

testFunction::~testFunction() 
  /*!
    Destructor
  */
{}

int 
testFunction::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: index of test to apply
    \retval 0 :: Success
  */
{
  ELog::RegMethod RegA("testFunction","applyTest");
  TestFunc::regSector("testFunction");

  typedef int (testFunction::*testPtr)();
  testPtr TPtr[]=
    {
      &testFunction::testAnalyse,
      &testFunction::testBuiltIn,
      &testFunction::testCopyVarSet,
      &testFunction::testEval,
      &testFunction::testString, 
      &testFunction::testVariable,
      &testFunction::testVec3D,
      &testFunction::testVec3DFunctions
    };

  const std::string TestName[]=
    {
      "Analyse",
      "BuiltIn",
      "CopyVarSet",
      "Eval",
      "String",
      "Variable",
      "Vec3D",
      "Vec3DFunctions"
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
testFunction::testAnalyse()
  /*!
    Tests the function analysis.
    If the string is a mathematical expresssion then it should
    be analysed and a value set
    \return 0 on success/-ve on failure
  */
{
  ELog::RegMethod RegA("testFunction","testAnalyse");

  typedef std::tuple<std::string,double> TTYPE;
  std::vector<TTYPE> Tests;
  
  Tests.push_back(TTYPE("atan(tx*tx)",atan(100.0)));
  Tests.push_back(TTYPE("sin(tx+3)",sin(13.0)));
  Tests.push_back(TTYPE("sqrt(itemA)",sqrt(3.4)));
  Tests.push_back(TTYPE("sqrt(itemB)",sqrt(6.0)));
  Tests.push_back(TTYPE("sqrt(itemA+itemB)",sqrt(3.4+6.0)));

  FuncDataBase XX;   
  XX.addVariable("tx",10.0);
  XX.addVariable("itemA",3.4); 
  //  XX.Parse("sqrt(36.0)");
  XX.Parse("6.0");
  XX.addVariable("itemB");             // check the expression parse
  
  int cnt(1);
  for(const TTYPE& tc : Tests)
    {
      const int flag=XX.Parse(std::get<0>(tc));
      if (flag)
	{
	  ELog::EM<<"Compile error:"<<flag<<ELog::endWarn;
	  ELog::EM<<"Compile error:"<<std::get<0>(tc)<<ELog::endWarn;
	  return -cnt;
	}
      if (std::abs(XX.Eval<double>()-std::get<1>(tc))>1e-5)
	{
	  ELog::EM<<"Test     :"<<std::get<0>(tc)<<ELog::endWarn;
	  ELog::EM<<"Eval     :"<<XX.Eval<double>()<<ELog::endWarn;
	  ELog::EM<<"Expected :"<<std::get<1>(tc)<<ELog::endWarn;
	  return -cnt;
	}
      cnt++;
    }
  //  std::cout<<"Eval == "<<XX.Eval()<<" "<<sqrt(atan(100)+sin(13))<<std::endl;
  return 0;
}

int
testFunction::testBuiltIn()
  /*!
    Test builtin commands
    \retval 0 :: success
  */
{
  ELog::RegMethod RegA("testFunction","testBuiltIn");

  FuncDataBase XX;   

  std::vector<std::string> TestString;
  std::vector<double> Results;
  
  TestString.push_back("max(tx,ty)");
  Results.push_back(20);

  XX.addVariable("tx",10.0);
  XX.addVariable("ty",20.0);

  std::vector<std::string>::const_iterator sc;
  std::vector<double>::const_iterator rc=Results.begin();
  for(sc=TestString.begin();sc!=TestString.end();sc++,rc++)
    {
      if (XX.Parse(*sc) || fabs(XX.Eval<double>()-(*rc))>1e-6)
        {
	  ELog::EM<<"Failed to Parse"<<*sc<<ELog::endErr;
	  return static_cast<int>(sc-TestString.begin())-1;
	}
    }
  return 0;
}

int
testFunction::testCopyVarSet()
  /*!
    Test to copy Var set
    \return 0 on succes and -ve on failure
  */
{
  ELog::RegMethod RegA("testVarList","testCopyVarSet");

  FuncDataBase Control;
 
  Control.addVariable("fred2alpha",1);
  Control.addVariable("fred121alpha",2);
  Control.addVariable("A",3);
  Control.addVariable("B",4);
  Control.addVariable("fred123beta",5);
  Control.addVariable("fred7beta",6);
  Control.addVariable("freds6alpha",7);
  Control.addVariable("fred4alpha",8);
  
  Control.copyVarSet("fred","John");

  if (Control.EvalVar<int>("John4alpha")!=8)
    {
      ELog::EM<<"Failed on test alpha:"<<Control.EvalVar<int>("John4alpah")
              <<ELog::endDiag;
      return -1;
    }

  return 0;
}


int
testFunction::testEval()
  /*!
    Test eval output
    \retval 0 :: success
  */
{
  ELog::RegMethod RegA("testFunction","testEval");

  FuncDataBase XX;   

  typedef std::tuple<std::string,double,int,size_t,double> TTYPE;
  std::vector<TTYPE> Tests;
  
  Tests.push_back(TTYPE("V1",-1.2,-1,ULONG_MAX,-1.2));
  
  for(const TTYPE& tc : Tests)
    XX.addVariable(std::get<0>(tc),std::get<1>(tc));
  
  for(const TTYPE& tc : Tests)
    {
      const int A=XX.EvalVar<int>(std::get<0>(tc));
      const size_t B=XX.EvalVar<size_t>(std::get<0>(tc));
      const double C=XX.EvalVar<double>(std::get<0>(tc));
      int failFlag(0);
      if (A!=std::get<2>(tc)) failFlag |= 1;
      if (B!=std::get<3>(tc)) failFlag |= 2;
      if (fabs(C-std::get<4>(tc))>1e-6) failFlag |= 4;
      
      if (failFlag)
	{
	  ELog::EM<<"Variable "<<std::get<0>(tc)<<ELog::endTrace;
	  if (failFlag & 1)
	    ELog::EM<<"Failed on int:"<<A<<" ("
		    <<std::get<2>(tc)<<")"<<ELog::endTrace;
	  if (failFlag & 2)
	    ELog::EM<<"Failed on size_t:"<<B<<" ("
		    <<std::get<3>(tc)<<")"<<ELog::endTrace;
	  if (failFlag & 4)
	    ELog::EM<<"Failed on double:"<<C<<" ("
		    <<std::get<4>(tc)<<")"<<ELog::endTrace;
	  return -1;
	}
    }
  return 0;
}


int
testFunction::testString()
  /*!
    Test a function of a string
    \return -ve on error 
  */
{
  ELog::RegMethod RegA("testFucntion","testString");
  FuncDataBase XX;   
  XX.addVariable("test",std::string("AA"));
  XX.addVariable("testB","BB");
  const std::string A=XX.EvalVar<std::string>("test");
  const std::string B=XX.EvalVar<std::string>("testB");
  if (A!="AA" || B!="BB")
    {
      ELog::EM<<"A == "<<A<<ELog::endDebug;
      return -1;
    }

  return 0;
}
  

int
testFunction::testVec3D()
  /*!
    Test builtin commands
    \retval 0 :: success
  */
{
  ELog::RegMethod RegA("testFunction","testVec3D");

  FuncDataBase XX;   

  typedef std::tuple<std::string,Geometry::Vec3D> TTYPE;
  std::vector<TTYPE> Tests;
   
  Tests.push_back(TTYPE("V1",Geometry::Vec3D(3,4,5)));
  Tests.push_back(TTYPE("V2",Geometry::Vec3D(10,-14,12)));
 
  for(const TTYPE& tc : Tests)
    XX.addVariable(std::get<0>(tc),std::get<1>(tc));

  for(const TTYPE& tc : Tests)
    {
      if (XX.EvalVar<Geometry::Vec3D>(std::get<0>(tc))!=std::get<1>(tc))
	{
	  ELog::EM<<"XX == "<<XX.EvalVar<Geometry::Vec3D>("V1")<<ELog::endTrace;
	  ELog::EM<<"Parse == "<<XX.Parse("V1+V2")<<ELog::endTrace;
	  XX.printByteCode(ELog::EM.Estream());
	  ELog::EM<<ELog::endTrace;
	  ELog::EM<<"Eval == "<<XX.Eval<double>()<<ELog::endTrace;
	  return -1;
	}
    }

  //  XX.Parse("3 + 4.0");
  return 0;
}

int
testFunction::testVariable()
  /*!
    Test the setting/ getting of a variable
    \retval 0 :: success
  */
{
  ELog::RegMethod RegA("testFunction","testVariable");

  FuncDataBase XX;   

  typedef std::tuple<std::string,Geometry::Vec3D> VTYPE;
  std::vector<VTYPE> TestVar=
    {
      VTYPE("V1",Geometry::Vec3D(3,4,5)),
      VTYPE("V2",Geometry::Vec3D(10,-14,12))
    };
  for(const VTYPE& V : TestVar)
    XX.addVariable(std::get<0>(V),std::get<1>(V));      

  // String : output type  : double / Vec out
  typedef std::tuple<std::string,int,Geometry::Vec3D,double> TTYPE;
  std::vector<TTYPE> Tests;
   
  Tests.push_back(TTYPE("1+4.0",0,Geometry::Vec3D(3,2,3),5));
  Tests.push_back(TTYPE("vec3d(1,2,3)+vec3d(2,3,4)",1,Geometry::Vec3D(3,5,7),0));
  Tests.push_back(TTYPE("V1+vec3d(1,1,1)",1,Geometry::Vec3D(4,5,6),0));
  
  std::string VName("A");
  int cnt(1);
  for(const TTYPE& tc : Tests)
    {
      if (XX.Parse(std::get<0>(tc)))
	{
	  ELog::EM<<"PARSE Failure:  "<<cnt<<ELog::endDiag;
	  ELog::EM<<"Test :"<<std::get<0>(tc)<<ELog::endDiag;
	  return -1;
	}
      XX.addVariable(VName);
      if (std::get<1>(tc)==1)   // Vector
	{
	  const Geometry::Vec3D Res=
	    XX.EvalVar<Geometry::Vec3D>(VName);
	  if (std::get<2>(tc)!=Res)
	    {
	      ELog::EM<<"EVAL[Vec] Failure:  "
		      <<cnt<<ELog::endDiag;
	      ELog::EM<<"TC == "<<Res<<" != "<<std::get<2>(tc)<<ELog::endDiag;
	      return -1;
	    }
	}
      else   // Double 
	{
	  const double Res=XX.EvalVar<double>(VName);
	  if (fabs(std::get<3>(tc)-Res)>Geometry::zeroTol)
	    {
	      ELog::EM<<"EVAL[double] Failure:  "
		      <<cnt<<ELog::endDiag;
	      ELog::EM<<std::setprecision(9)<<"TC == "<<Res<<" != "
		      <<std::get<3>(tc)<<ELog::endDiag;
	      return -1;
	    }
	}
      cnt++;
      VName[0]++;
    }
  return 0;
}

int
testFunction::testVec3DFunctions()
  /*!
    Test builtin commands
    \retval 0 :: success
  */
{
  ELog::RegMethod RegA("testFunction","testVec3D");

  FuncDataBase XX;   
 
  typedef std::tuple<std::string,Geometry::Vec3D> VTYPE;
  std::vector<VTYPE> TestVar;
  TestVar.push_back(VTYPE("V1",Geometry::Vec3D(3,4,5)));
  TestVar.push_back(VTYPE("V2",Geometry::Vec3D(10,-14,12)));
 
  // String : output type  : double / Vec out
  typedef std::tuple<std::string,int,Geometry::Vec3D,double> TTYPE;
  std::vector<TTYPE> Tests={
    TTYPE("vec3d(1,2,3)",1,Geometry::Vec3D(1,2,3),0),
    TTYPE("V1",1,Geometry::Vec3D(3,4,5),0),
    TTYPE("V1+vec3d(1,2,3)",1,Geometry::Vec3D(4,6,8),0),
    TTYPE("V1+vec3d(1,2,3)",1,Geometry::Vec3D(4,6,8),0),
    TTYPE("V1+vec3d(1,2,3)",1,Geometry::Vec3D(4,6,8),0),
    TTYPE("abs(V1+vec3d(1,2,3))",0,Geometry::Vec3D(0,0,0),sqrt(16+36+64)),
    TTYPE("V1*vec3d(1,2,3)",1,Geometry::Vec3D(2,-4,2),0),
    TTYPE("dot(vec3d(1,2,3),V1)",0,Geometry::Vec3D(0,0,0),(3+8+15))
  };
  
  for(const VTYPE& vc : TestVar)
    XX.addVariable(std::get<0>(vc),std::get<1>(vc));

  int cnt(1);
  for(const TTYPE& tc : Tests)
    {
      if (XX.Parse(std::get<0>(tc)))
	{
	  ELog::EM<<"PARSE Failure:  "<<cnt<<ELog::endDiag;
	  ELog::EM<<"Test :"<<std::get<0>(tc)<<ELog::endDiag;
	  return -1;
	}
      if (std::get<1>(tc)==1)   // Vector
	{
	  const Geometry::Vec3D Res=
	    XX.Eval<Geometry::Vec3D>();
	  if (std::get<2>(tc)!=Res)
	    {
	      ELog::EM<<"EVAL[Vec] Failure:  "<<cnt<<ELog::endDiag;
	      ELog::EM<<"String:  "<<std::get<0>(tc)<<ELog::endDiag;
	      ELog::EM<<"TC == "<<Res<<" != "<<std::get<2>(tc)<<ELog::endDiag;
	      return -1;
	    }
	}
      else   // Double 
	{
	  const double Res=XX.Eval<double>();
	  if (fabs(std::get<3>(tc)-Res)>Geometry::zeroTol)
	    {
	      ELog::EM<<"EVAL[double] Failure:  "
		      <<cnt<<ELog::endDiag;
	      ELog::EM<<"String:  "<<std::get<0>(tc)<<ELog::endDiag;
	      ELog::EM<<std::setprecision(9)<<"TC == "<<Res<<" != "
		      <<std::get<3>(tc)<<ELog::endDiag;
	      return -1;
	    }
	}
      cnt++;
    }
  return 0;
}
