/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   test/testAlgebra.cxx
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#include <set>
#include <map>
#include <stack>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <boost/tuple/tuple.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h" 
#include "BnId.h"
#include "Acomp.h"
#include "Algebra.h"

#include "testFunc.h"
#include "testAlgebra.h"

using namespace MonteCarlo;

testAlgebra::testAlgebra() 
  /*!
    Constructor
  */
{}

testAlgebra::~testAlgebra() 
  /*!
    Destructor
  */
{}

int 
testAlgebra::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: Test number to run
    \retval -1 : testAlgebra failed
    \retval 0 : All succeeded
  */
{
  ELog::RegMethod RegA("testAlgebra","applyTest");
  TestFunc::regSector("testAlgebra");

  typedef int (testAlgebra::*testPtr)();
  testPtr TPtr[]=
    {
      &testAlgebra::testAdditions,
      &testAlgebra::testCountLiterals,
      &testAlgebra::testCNF,
      &testAlgebra::testComplementary,
      &testAlgebra::testDNF,
      &testAlgebra::testMakeString,
      &testAlgebra::testMult,
      &testAlgebra::testSetFunction,
      &testAlgebra::testSetFunctionObjStr,
      &testAlgebra::testSubtract,
      &testAlgebra::testWeakDiv
    };
  const std::string TestName[]=
    {
      "Additions",
      "CountLiterals",
      "CNF",
      "Complementary",
      "DNF",
      "MakeString",
      "Mult",
      "SetFunction",
      "SetFunctionObjStr",
      "Subtract",
      "WeakDiv"
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
      if (extra<0 || 
	  static_cast<size_t>(extra)==i+1)
        {
	  TestFunc::regTest(TestName[i]);
	  const int retValue= (this->*TPtr[i])();
	  if (retValue || extra>0)
	    return retValue;
	}
    }
  return 0;
}


/*
int 
testAlgebra::testDeleteString()
{
  std::cerr<<"Test String "<<ELog::endErr;
  Algebra AA=new Algebra A;
  A.setFunction("a'bcd+a(cd+ff(x+y+z))");
  // A.setFunction("((x+y+z))");
  //  A.setFunction("a(cd+ff)");
  //  A.setFunction("a(cd+ff)");
  A.write(std::cout);
  return 0;
}
*/


int 
testAlgebra::testCountLiterals()
  /*!
    Test the number of literals in a string.
    Single test includes not.
    \retval -1 :: Failed to get teh correct number of literals
    \retval 0 :: success
   */
{
  ELog::RegMethod RegA("testAlgebra","testCountLiterals");

  Algebra A;
  A.setFunction("a'bcd+a(cd+ff(x+y+z))");
  int NLiteral=A.countLiterals();
  if (NLiteral!=9)
    {
      ELog::EM<<"Error with number of literals(9):"
	      <<NLiteral<<ELog::endDiag;
      ELog::EM<<"A== "<<A<<ELog::endDiag;
      return -1;
    }
  return 0;
}

int
testAlgebra::testDNF()
  /*!
    Test the DNF Structure 
    \retval 0 :: success (there is no fail!!!)
   */
{
  ELog::RegMethod RegA("testAlgebra","testDNF");

  Algebra A,B;
  std::vector<std::string> Func;
  //  Func.push_back("(a'b'c'd')+(a'b'c'd)+(a'b'cd')+(a'bc'd)+(a'bcd')+(a'bcd)+(ab'c'd')+(ab'c'd)+(ab'cd')+(abcd')");

  Func.push_back("a'b'c'+d'e'");
  Func.push_back("(a'b'c')+(a'b'c)+(a'bc')+(ab'c)+(abc')+(abc)");
  //  Func.push_back("ab((c'(d+e+f')g'h'i')+(gj'(k+l')(m+n)))");

  std::vector<std::string>::const_iterator sv;
  for(sv=Func.begin();sv!=Func.end();sv++)
    {
      A.setFunction(*sv);
      B.setFunction(*sv);
      A.makeDNF();
      if (!A.logicalEqual(B) || !A.getComp().isDNF())
        {
	  ELog::EM<<"Original "<<*sv<<" == "<<ELog::endErr;
	  ELog::EM<<"Function "<<B<<ELog::endErr;
	  ELog::EM<<"DNF == "<<A<<ELog::endErr;
	  return -1;
	}
    }
  return 0;
}

int
testAlgebra::testCNF()
  /*!
    Test the CNF Structure 
    \retval 0 :: success (there is no fail!!!)
   */
{
  ELog::RegMethod RegA("testAlgebra","testCNF");

  typedef boost::tuple<std::string,std::string,std::string> TTYPE;
  std::vector<TTYPE> Tests;
  Tests.push_back(TTYPE("(f+x)(x+y+z)","(f+x)(x+y+z)","x+fy+fz"));
  Tests.push_back(TTYPE("aq+acp+ace","a(c+q)(e+p+q)","ace+acp+aq"));

  std::vector<TTYPE>::const_iterator tc;
  for(tc=Tests.begin();tc!=Tests.end();tc++)
    {
      Algebra A;
      A.setFunction(tc->get<0>());
      A.makeCNF();
      std::string Out=A.display();
      if (Out!=tc->get<1>())
	{
	  ELog::EM<<"Failed on CNF  :"<<tc->get<0>()<<ELog::endDiag;
	  ELog::EM<<"Function in CNF:"<<A<<ELog::endDiag;
	  return -1;
	}
      A.makeDNF();
      Out=A.display();
      if (Out!=tc->get<2>())
	{
	  ELog::EM<<"Failed on DNF  :"<<tc->get<0>()<<ELog::endDiag;
	  ELog::EM<<"Function in DNF:"<<A<<ELog::endDiag;
	  return -2;
	}
    }
  return 0;
}
int 
testAlgebra::testAdditions()
  /*!
    Add two different algebra's by union
    \return 0 on success
  */
{
  ELog::RegMethod RegA("testAlgebra","testAdditions");

  typedef boost::tuple<std::string,std::string,std::string> TTYPE;
  std::vector<TTYPE> Tests;

  Tests.push_back(TTYPE("a'bcd+a(cd+ff(x+y+z))",
			"jxyzi(ad+sw)",
			"a'bcd+a(cd+f(x+y+z))+jxyzi(ad+sw)"));

  Tests.push_back(TTYPE("a'b",
			"cd+a'b",
			"a'b+cd"));

  Tests.push_back(TTYPE("a'",
			"a",
			"a'+a"));

  std::vector<TTYPE>::const_iterator tc;
  Algebra A,B,C;
  for(tc=Tests.begin();tc!=Tests.end();tc++)
    {
      A.setFunction(tc->get<0>());      
      B.setFunction(tc->get<1>());      
      C.setFunction(tc->get<2>());
      B+=A;
      if (B!=C)
	{
	  ELog::EM<<"B Org = "<<tc->get<1>()<<ELog::endDiag;
	  ELog::EM<<"A    == "<<A<<ELog::endDiag;	  
	  ELog::EM<<"B    == "<<B<<ELog::endDiag;	  
	  ELog::EM<<"Res  == "<<C<<ELog::endDiag;	  
	  return -1;
	}
    }
  return 0;
}

int
testAlgebra::testMakeString()
  /*!
    Process algebra from a string
    \retval -ve on error
    \retval 0 on success
  */
{
  ELog::RegMethod RegA("testAlgebra","testMakeString");

  typedef boost::tuple<std::string,std::string,std::string> TTYPE;
  std::vector<TTYPE> Tests;
  Tests.push_back(TTYPE("(a:b)",
   			"a+b",
   			"b'a'"));
  Tests.push_back(TTYPE("a+bc",
   			"a+bc",
   			"a'(c'+b')"));
  Tests.push_back(TTYPE("a+(b+c)(e+d)",
			"a+(b+c)(d+e)",
			"a'(e'd'+c'b')"));
  Tests.push_back(TTYPE("a'bcd+a(cd+ff(x+y+z))",
			"a'bcd+a(cd+f(x+y+z))",
			"(d'+c'+b'+a)(a'+(f'+z'y'x')(d'+c'))"));
  Tests.push_back(TTYPE("(cd+ff)+b","b+f+cd","f'b'(d'+c')"));

  Algebra A;
  std::vector<TTYPE>::const_iterator tc;
  for(tc=Tests.begin();tc!=Tests.end();tc++)
    {
      A.setFunction(tc->get<0>());
      if (A.display()!=tc->get<1>())
	{
	  ELog::EM<<"Original :"<<tc->get<0>()<<ELog::endDiag;
	  ELog::EM<<"A ==     :"<<A.display()<<":"<<ELog::endDiag;
	  ELog::EM<<"Expected :"<<tc->get<1>()<<":"<<ELog::endDiag;
	  return -1;
	}
      A.Complement();
      if (A.display()!=tc->get<2>())
	{
	  ELog::EM<<"Original :"<<tc->get<0>()<<ELog::endDiag;
	  ELog::EM<<"A ==     :"<<tc->get<1>()<<ELog::endDiag;
	  ELog::EM<<"A' ==    :"<<A<<ELog::endDiag;
	  ELog::EM<<"Expected :"<<tc->get<2>()<<ELog::endDiag;
	  return -2;
	}
    }
  return 0;
}


int
testAlgebra::testMult()
  /*!
    Test algebraic multiplication
    \retval 0 on success
  */
{
  ELog::RegMethod RegA("testAlgebra","testMult");

  // A * B == C
  typedef boost::tuple<std::string,std::string,std::string> TTYPE;
  std::vector<TTYPE> Tests;

  Tests.push_back(TTYPE("a+b'+(c)","a+b","(b'+a+c)(a+b)"));

  Algebra A,B,C,D;
  std::vector<TTYPE>::const_iterator tc;
  for(tc=Tests.begin();tc!=Tests.end();tc++)
    {
      A.setFunction(tc->get<0>());
      B.setFunction(tc->get<1>());
      C.setFunction(tc->get<2>());
      A*=B;
      if (A.display()!=tc->get<2>())
	{
	  D.setFunction(tc->get<0>());
	  ELog::EM<<"A      == "<<D<<ELog::endTrace;
	  ELog::EM<<"B      == "<<B<<ELog::endTrace;
	  ELog::EM<<"C      == "<<A<<ELog::endTrace;
	  ELog::EM<<"expect == "<<tc->get<2>()<<ELog::endTrace;
	  ELog::EM<<"C == "<<(C==A)<<ELog::endTrace;
	  return -1;
	}
    }
  return 0;
}  

int 
testAlgebra::testWeakDiv()
  /*!
    Test weak division algorithm
    \retval -ve on error
    \retval 0 on success
  */
{
  ELog::RegMethod RegA("testAlgebra","testWeakDiv");

  std::vector<std::string> mainFunc;
  std::vector<std::string> dFunc;
  mainFunc.push_back("ac+ad+bc+bd+ae'");
  dFunc.push_back("a+b");
  mainFunc.push_back("afbc(x+y+z)");
  dFunc.push_back("abc");
  mainFunc.push_back("afbc(x+y+z)");
  dFunc.push_back("x+y");
  

  for(size_t i=0;i<mainFunc.size();i++)
    {
      Algebra A;
      A.setFunction(mainFunc[i]);
      Algebra B;
      B.setFunction(dFunc[i]);

      std::pair<Algebra,Algebra> X=A.algDiv(B);
      // Check multiplication:
      Algebra XY=X.first*B;
      XY+=X.second;     

      if (!XY.logicalEqual(A) || i==3)
	{
	  ELog::EM<<"A == "<<A<<ELog::endErr;
	  ELog::EM<<"B == "<<B<<ELog::endErr;
	  ELog::EM<<ELog::endErr;
	  ELog::EM<<"A div == "<<X.first<<ELog::endErr;
	  ELog::EM<<"A rem == "<<X.second<<ELog::endErr;
	  ELog::EM<<ELog::endErr;
	  ELog::EM<<"Multiplied "<<XY<<ELog::endErr;
	  ELog::EM<<"Logic:"<<XY.logicalEqual(A)<<ELog::endErr;
	  ELog::EM<<ELog::endErr;
	  XY.makeDNF();
	  ELog::EM<<"XY(DNF):"<<XY<<ELog::endErr;      
	  return -1;
	}
    }
  return 0;
}

int
testAlgebra::testComplementary()
  /*!
    Test the complementary rule:
    \retval 0 :: success
   */
{
  ELog::RegMethod RegA("testAlgebra","testComplementary");

  typedef boost::tuple<std::string,std::string> TTYPE;
  std::vector<TTYPE> Tests;
  Tests.push_back(TTYPE("(a+b)","b'a'"));
  Tests.push_back(TTYPE("ab((c'(d+e+f')g'h'i')+(gj'(k+l')(m+n)))",
			"b'+a'+(g'+j+n'm'+k'l)(c+g+h+i+e'd'f)"));
  std::vector<TTYPE>::const_iterator tc;
  for(tc=Tests.begin();tc!=Tests.end();tc++)
    {
      Algebra A;
      A.setFunction(tc->get<0>());
      A.Complement();
      if (A.display()!=tc->get<1>())
	{
	  ELog::EM<<"A      == "<<A.display()<<ELog::endTrace;
	  ELog::EM<<"Expect == "<<tc->get<1>()<<ELog::endTrace;
	  return -1;
	}
    }
  return 0;
}


int
testAlgebra::testSetFunctionObjStr()
  /*!
    Test translation to MCNPX
    \return 0 on success
   */
{
  ELog::RegMethod RegA("testAlgebra","testSetFunctionObjStr");


  typedef boost::tuple<std::string,std::string> TTYPE;
  std::vector<TTYPE> Tests;
  Tests.push_back(TTYPE("(1 : -2 : 3 : -4)","d'+b'+a+c"));

  std::vector<TTYPE>::const_iterator tc;
  for(tc=Tests.begin();tc!=Tests.end();tc++)
    {
      Algebra A;
      A.setFunctionObjStr(tc->get<0>());
      if (A.display()!=tc->get<1>())
	{
	  ELog::EM<<"Input    == "<<tc->get<0>()<<ELog::endTrace;
	  ELog::EM<<"A        == "<<A<<ELog::endTrace;
	  ELog::EM<<"Expect   == "<<tc->get<1>()<<ELog::endTrace;
	  return -1;
	}
    }
  return 0;
}

int
testAlgebra::testSetFunction()
  /*!
    Set the standard algebraic function setting
    \return 0 on success
   */
{
  ELog::RegMethod RegA("testAlgebra","testSetFunctionObjStr");


  typedef boost::tuple<std::string,std::string> TTYPE; 
  std::vector<TTYPE> Tests;
  Tests.push_back(TTYPE("#(f+i)(xy)","i'f'xy"));

  std::vector<TTYPE>::const_iterator tc;
  for(tc=Tests.begin();tc!=Tests.end();tc++)
    {
      Algebra A;
      A.setFunction(tc->get<0>());
      if (A.display()!=tc->get<1>())
	{
	  ELog::EM<<"Input    == "<<tc->get<0>()<<ELog::endTrace;
	  ELog::EM<<"A        == "<<A<<ELog::endTrace;
	  ELog::EM<<"Expect   == "<<tc->get<1>()<<ELog::endTrace;
	  return -1;
	}
    }
  return 0;
}

int
testAlgebra::testSubtract()
  /*!
    Test the subtraction algorithm
    \return 0 on success
   */
{
  ELog::RegMethod RegA("testAlgebra","testSubtract");

  Algebra A,B,C;
  
  typedef boost::tuple<std::string,std::string,std::string> TTYPE;
  std::vector<TTYPE> Tests;
  Tests.push_back(TTYPE("a+b+cd","cd","(d'+c')(a+b+cd)"));
  
  std::vector<TTYPE>::const_iterator tc;
  for(tc=Tests.begin();tc!=Tests.end();tc++)
    {
      A.setFunction(tc->get<0>());
      B.setFunction(tc->get<1>());
      C.setFunction(tc->get<2>());
      A-=B;
      if (A!=C)
	{
	  ELog::EM<<"Init A :"<<tc->get<0>()<<ELog::endTrace;
	  ELog::EM<<"B      :"<<B<<ELog::endTrace;
	  ELog::EM<<"A      :"<<A<<ELog::endTrace;
	  ELog::EM<<"Expect :"<<C<<ELog::endTrace;
	  return -1;
	}
    }
  return 0;
}
