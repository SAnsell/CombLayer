/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   test/testAlgebra.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#include <tuple>

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
      &testAlgebra::testComponentExpand,      
      &testAlgebra::testDNF,
      &testAlgebra::testExpand,
      &testAlgebra::testExpandBracket,
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
      "ComponentExpand",
      "DNF",
      "Expand",
      "ExpandBracket",
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
testAlgebra::testComponentExpand()
  /*!
    Expand the bracket form 
    \retval 0 :: success (there is no fail!!!)
   */
{
  ELog::RegMethod RegA("testAlgebra","testComponentExpand");

  typedef std::tuple<std::string,int,std::string,std::string> TTYPE;
  const std::vector<TTYPE> Tests=
    {
      TTYPE("a+bx(f+g)",1,"d+ey","ad+aey+dbx(f+g)+bx(f+g)ey"),
      TTYPE("a+bx",1,"d+ey","ad+aey+dbx+bxey"),
      TTYPE("a+bx",1,"d+e","ad+ae+dbx+ebx"),
      TTYPE("a+b",1,"d+e","ad+ae+bd+be"),
      
      TTYPE("ab",1,"d+e","dab+eab"),
      TTYPE("a+b",0,"d+e","a+b+d+e"),   // union
      
      TTYPE("ab",1,"cd","abcd"),
      TTYPE("ab",1,"d","abd"),
      TTYPE("ab",1,"def","abdef")
      
      
    };

  for(const TTYPE& tc : Tests)
    {
      Algebra A;
      A.setFunction(std::get<0>(tc));
      Algebra B;
      B.setFunction(std::get<2>(tc));
      const Acomp& AC=A.getComp();
      const Acomp& BC=B.getComp();

      const Acomp CC=AC.componentExpand(std::get<1>(tc),BC);
      std::string Out=CC.display();
      if (Out!=std::get<3>(tc))
	{
	  ELog::EM<<"Failed on  :"<<std::get<0>(tc)<<ELog::endDiag;
	  ELog::EM<<"Failed on  :"<<std::get<2>(tc)<<ELog::endDiag;
	  ELog::EM<<"Expect     :"<<std::get<3>(tc)<<ELog::endDiag;
	  ELog::EM<<"Display == :"<<Out<<ELog::endDiag;
	  return -1;
	}
    }
  return 0;
}

int
testAlgebra::testExpand()
  /*!
    Expand the bracket form 
    \retval 0 :: success (there is no fail!!!)
   */
{
  ELog::RegMethod RegA("testAlgebra","testExpand");

  typedef std::tuple<std::string,std::string,std::string> TTYPE;
  const std::vector<TTYPE> Tests=
    {
      TTYPE("(ab+xy)","cd","cdab+cdxy"),
      TTYPE("ab","cd","cdab"),
      TTYPE("abxy","cd","cdabxy"),
      TTYPE("ab","(c+d)","abc+abd"),
      TTYPE("(c+d)","ab","abc+abd"),
      TTYPE("(a+b)","(c+de)","ac+dea+bc+deb"),
      TTYPE("(a+d)","(b+c)","ab+ac+db+dc"),
      TTYPE("(a+c+d)","(b+c)","ab+ac+cb+cc+db+dc"),
      TTYPE("(c+de)","(a+b)","ca+cb+dea+deb"),
      TTYPE("(c+de+gh)","(a+b)","ca+cb+dea+gha+deb+ghb"),
      TTYPE("(c+de+gh)","(a+bx)","ca+bxc+dea+gha+bxde+bxgh")
    };

  for(const TTYPE& tc : Tests)
    {
      Algebra A;
      A.setFunction(std::get<0>(tc));
      Algebra B;
      B.setFunction(std::get<1>(tc));
      const Acomp& AC=A.getComp();
      const Acomp& BC=B.getComp();

      const Acomp CC=AC.expand(1,BC);
      std::string Out=CC.display();
      if (Out!=std::get<2>(tc))
	{
	  ELog::EM<<"Failed on  :"<<std::get<0>(tc)<<ELog::endDiag;
	  ELog::EM<<"Failed on  :"<<std::get<1>(tc)<<ELog::endDiag;
	  ELog::EM<<"Expect     :"<<std::get<2>(tc)<<ELog::endDiag;
	  ELog::EM<<"Display == :"<<Out<<ELog::endDiag;
	  return -1;
	}
    }
  return 0;
}

int
testAlgebra::testExpandBracket()
  /*!
    Expand the bracket form 
    \retval 0 :: success (there is no fail!!!)
   */
{
  ELog::RegMethod RegA("testAlgebra","testExpandBracket");

  typedef std::tuple<std::string,std::string> TTYPE;
  const std::vector<TTYPE> Tests=
    {
      //      TTYPE("a(b+c)","ab+ac"),
      TTYPE("(a+d)(hb+c)","ab+ac+db+dc"),
      TTYPE("(a+b)(c+d)(e+f)","ab+ac+db+dc"),
      TTYPE("(a+d)(b+c+e)","ab+ac+ae+db+dc+de"),
      TTYPE("(a+d+f)(b+c+e)","ab+ac+ae+db+dc+de+fb+fc+fe"),
      TTYPE("x(a+d+f)(b+c)","(xa+xd+xf)(b+c)"),
      TTYPE("x+a'bcd+a(cd+ff(x+y+z))","x"),
      TTYPE("a(b+c)","ab+ac")

    };

  for(const TTYPE& tc : Tests)
    {
      Algebra A;
      A.setFunction(std::get<0>(tc));
      const std::string preOut=A.display();
      A.expandBracket();
      
      std::string Out=A.display();
      if (Out!=std::get<1>(tc))
	{
	  ELog::EM<<"Failed on  :"<<std::get<0>(tc)<<ELog::endDiag;
	  ELog::EM<<"PreOut on  :"<<preOut<<ELog::endDiag;
	  ELog::EM<<"Display == :"<<Out<<ELog::endDiag;
	  return -1;
	}
    }
  return 0;
}


int
testAlgebra::testCNF()
  /*!
    Test the CNF/DNF Structure 
    \retval 0 :: success (there is no fail!!!)
   */
{
  ELog::RegMethod RegA("testAlgebra","testCNF");

  typedef std::tuple<std::string,std::string,std::string> TTYPE;
  const std::vector<TTYPE> Tests=
    {
      TTYPE("(f+x)(x+y+z)","(f+x)(x+y+z)","x+fy+fz"),
      TTYPE("aq+acp+ace","a(c+q)(e+p+q)","ace+acp+aq")
    };

  for(const TTYPE& tc : Tests)
    {
      Algebra A;
      A.setFunction(std::get<0>(tc));
      A.makeCNF();

      std::string Out=A.display();
      if (Out!=std::get<1>(tc))
	{
	  ELog::EM<<"Failed on CNF  :"<<std::get<0>(tc)<<ELog::endDiag;
	  ELog::EM<<"Function in CNF:"<<A<<ELog::endDiag;
	  return -1;
	}
      A.makeDNF();
      Out=A.display();
      if (Out!=std::get<2>(tc))
	{
	  ELog::EM<<"Failed on DNF  :"<<std::get<0>(tc)<<ELog::endDiag;
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

  typedef std::tuple<std::string,std::string,std::string> TTYPE;
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

  Algebra A,B,C;
  for(const TTYPE& tc : Tests)
    {
      A.setFunction(std::get<0>(tc));      
      B.setFunction(std::get<1>(tc));      
      C.setFunction(std::get<2>(tc));
      B+=A;
      if (B!=C)
	{
	  ELog::EM<<"B Org = "<<std::get<1>(tc)<<ELog::endDiag;
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

  typedef std::tuple<std::string,std::string,std::string> TTYPE;
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
  for(const TTYPE& tc : Tests)
    {
      A.setFunction(std::get<0>(tc));
      if (A.display()!=std::get<1>(tc))
	{
	  ELog::EM<<"Original :"<<std::get<0>(tc)<<ELog::endDiag;
	  ELog::EM<<"A ==     :"<<A.display()<<":"<<ELog::endDiag;
	  ELog::EM<<"Expected :"<<std::get<1>(tc)<<":"<<ELog::endDiag;
	  return -1;
	}
      A.Complement();
      if (A.display()!=std::get<2>(tc))
	{
	  ELog::EM<<"Original :"<<std::get<0>(tc)<<ELog::endDiag;
	  ELog::EM<<"A ==     :"<<std::get<1>(tc)<<ELog::endDiag;
	  ELog::EM<<"A' ==    :"<<A<<ELog::endDiag;
	  ELog::EM<<"Expected :"<<std::get<2>(tc)<<ELog::endDiag;
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
  typedef std::tuple<std::string,std::string,std::string> TTYPE;
  std::vector<TTYPE> Tests;

  Tests.push_back(TTYPE("a+b'+(c)","a+b","(b'+a+c)(a+b)"));

  Algebra A,B,C,D;
  for(const TTYPE& tc : Tests)
    {
      A.setFunction(std::get<0>(tc));
      B.setFunction(std::get<1>(tc));
      C.setFunction(std::get<2>(tc));
      A*=B;
      if (A.display()!=std::get<2>(tc))
	{
	  D.setFunction(std::get<0>(tc));
	  ELog::EM<<"A      == "<<D<<ELog::endTrace;
	  ELog::EM<<"B      == "<<B<<ELog::endTrace;
	  ELog::EM<<"C      == "<<A<<ELog::endTrace;
	  ELog::EM<<"expect == "<<std::get<2>(tc)<<ELog::endTrace;
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

  typedef std::tuple<std::string,std::string> TTYPE;
  std::vector<TTYPE> Tests;
  Tests.push_back(TTYPE("(a+b)","b'a'"));
  Tests.push_back(TTYPE("ab((c'(d+e+f')g'h'i')+(gj'(k+l')(m+n)))",
			"b'+a'+(g'+j+n'm'+k'l)(c+g+h+i+e'd'f)"));

  for(const TTYPE& tc : Tests)
    {
      Algebra A;
      A.setFunction(std::get<0>(tc));
      A.Complement();
      if (A.display()!=std::get<1>(tc))
	{
	  ELog::EM<<"A      == "<<A.display()<<ELog::endTrace;
	  ELog::EM<<"Expect == "<<std::get<1>(tc)<<ELog::endTrace;
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


  typedef std::tuple<std::string,std::string> TTYPE;
  std::vector<TTYPE> Tests;
  Tests.push_back(TTYPE("(1 : -2 : 3 : -4)","d'+b'+a+c"));

  for(const TTYPE& tc : Tests)
    {
      Algebra A;
      A.setFunctionObjStr(std::get<0>(tc));
      if (A.display()!=std::get<1>(tc))
	{
	  ELog::EM<<"Input    == "<<std::get<0>(tc)<<ELog::endTrace;
	  ELog::EM<<"A        == "<<A<<ELog::endTrace;
	  ELog::EM<<"Expect   == "<<std::get<1>(tc)<<ELog::endTrace;
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


  typedef std::tuple<std::string,std::string> TTYPE; 
  std::vector<TTYPE> Tests;
  Tests.push_back(TTYPE("#(f+i)(xy)","i'f'xy"));

  for(const TTYPE& tc : Tests)
    {
      Algebra A;
      A.setFunction(std::get<0>(tc));
      if (A.display()!=std::get<1>(tc))
	{
	  ELog::EM<<"Input    == "<<std::get<0>(tc)<<ELog::endTrace;
	  ELog::EM<<"A        == "<<A<<ELog::endTrace;
	  ELog::EM<<"Expect   == "<<std::get<1>(tc)<<ELog::endTrace;
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
  
  typedef std::tuple<std::string,std::string,std::string> TTYPE;
  std::vector<TTYPE> Tests;
  Tests.push_back(TTYPE("a+b+cd","cd","(d'+c')(a+b+cd)"));
  
  for(const TTYPE& tc : Tests)
    {
      A.setFunction(std::get<0>(tc));
      B.setFunction(std::get<1>(tc));
      C.setFunction(std::get<2>(tc));
      A-=B;
      if (A!=C)
	{
	  ELog::EM<<"Init A :"<<std::get<0>(tc)<<ELog::endTrace;
	  ELog::EM<<"B      :"<<B<<ELog::endTrace;
	  ELog::EM<<"A      :"<<A<<ELog::endTrace;
	  ELog::EM<<"Expect :"<<C<<ELog::endTrace;
	  return -1;
	}
    }
  return 0;
}
