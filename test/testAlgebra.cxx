/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   test/testAlgebra.cxx
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
#include "AcompTools.h"
#include "Acomp.h"
#include "Algebra.h"

#include "Binary.h"
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
      &testAlgebra::testExpandBracket,
      &testAlgebra::testExpandCNFBracket,
      &testAlgebra::testInsert,
      &testAlgebra::testIsTrue,
      &testAlgebra::testLogicalCover,      
      &testAlgebra::testLogicalEqual,
      &testAlgebra::testMakeString,
      &testAlgebra::testMerge,
      &testAlgebra::testMult,
      &testAlgebra::testResolveTrue,
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
      "ExpandBracket",
      "ExpandCNFBracket",
      "Insert",
      "IsTrue",
      "LogicalCover",
      "LogicalEqual",
      "MakeString",
      "Merge",
      "Mult",
      "ResolveTrue",
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
      // stuff for CNF
      //      TTYPE("a",0,"bcd","(a+b)(a+c)(a+d)"),
      TTYPE("a",0,"b+c","(a+b+c)"),
      TTYPE("a",0,"bc+de","(a+bc)(a+de)"),
      TTYPE("a",0,"bc+de","(a+bc)(a+de)"),
      
      TTYPE("a",0,"cd+(fx+fy+fz)","(a+cd)(a+fx)(a+fy)(a+fz)"),
      TTYPE("af",0,"cd","(a+c)(a+d)(c+f)(d+f)"),
      // stuff for DNF
      TTYPE("a",1,"cd+(fx+fy+fz)","acd+afx+afy+afz"),  
      TTYPE("af",1,"cd+(fx+fy+fz)","afcd+affx+affy+affz"),	    
      TTYPE("cd+f(x+y+z)",1,"a","acd+af(x+y+z)"),
      TTYPE("a",1,"cd+(fx+fy+fz)","acd+afx+afy+afz"),
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
testAlgebra::testMerge()
  /*!
    Merge the bracket form 
    \retval 0 :: success (there is no fail!!!)
   */
{
  ELog::RegMethod RegA("testAlgebra","testMerge");

  typedef std::tuple<std::string,std::string> TTYPE;
  const std::vector<TTYPE> Tests=
    {
      TTYPE("a(bc)","abc"),
      TTYPE("a(b+c)","a(b+c)"),
      TTYPE("acd+(afx+afy+afz)","acd+afx+afy+afz")
    };


  for(const TTYPE& tc : Tests)
    {
      Algebra A;
      A.setFunction(std::get<0>(tc));
      const Acomp& AC = A.getComp();
      const std::string preOut=A.display();
      A.merge();
      
      std::string Out=AC.display();
      if (Out!=std::get<1>(tc))
	{
	  ELog::EM<<"Failed on  :"<<std::get<0>(tc)<<ELog::endDiag;
	  ELog::EM<<"PreOut on  :"<<preOut<<ELog::endDiag;
	  ELog::EM<<"Expected== :"<<std::get<1>(tc)<<ELog::endDiag;
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
      TTYPE("a(b+c)","ab+ac"),
      TTYPE("a(cd+f(x+y+z))","acd+afx+afy+afz"),
      TTYPE("x+a'bcd+a(cd+f(x+y+z))","x+a'bcd+acd+afx+afy+afz"),
      TTYPE("(a+d)(hb+c)","abh+ac+bdh+cd"),
      TTYPE("(a+b)(c+d)(e+f)","ace+acf+ade+adf+bce+bcf+bde+bdf"),
      TTYPE("(a+d)(b+c+e)","ab+ac+ae+bd+cd+de"),
      TTYPE("(a+d+f)(b+c+e)","ab+ac+ae+bd+bf+cd+cf+de+ef"),
      TTYPE("x(a+d+f)(b+c)","abx+acx+bdx+bfx+cdx+cfx")
    };

  size_t cnt(0);
  for(const TTYPE& tc : Tests)
    {
      cnt++;
      Algebra A;
      A.setFunction(std::get<0>(tc));
      const std::string preOut=A.display();
      A.expandBracket();
      
      std::string Out=A.display();
      if (Out!=std::get<1>(tc))
	{
	  ELog::EM<<"TEST == "<<cnt<<ELog::endDiag;
	  ELog::EM<<"Failed on  :"<<std::get<0>(tc)<<ELog::endDiag;
	  ELog::EM<<"PreOut on  :"<<preOut<<ELog::endDiag;
	  ELog::EM<<"Expected== :"<<std::get<1>(tc)<<ELog::endDiag;
	  ELog::EM<<"Display == :"<<Out<<ELog::endDiag;
	  ELog::EM<<" MERGE == "<<A.display()<<ELog::endDiag;
	  const Acomp& AC=A.getComp();

	  ELog::EM<<" MERGE == "<<AC.displayDepth(0)<<ELog::endDiag;
	  A.merge();
	  ELog::EM<<" MERGE == "<<A.display()<<ELog::endDiag;

	  return -1;
	}
    }
  return 0;
}

int
testAlgebra::testExpandCNFBracket()
  /*!
    Expand the bracket int CNF form
    \retval 0 :: success (there is no fail!!!)
   */
{
  ELog::RegMethod RegA("testAlgebra","testExpandCNFBracket");

  typedef std::tuple<std::string,std::string> TTYPE;
  const std::vector<TTYPE> Tests=
    {
      TTYPE("a(b+c+de)","a(b+c+d)(b+c+e)"),
      TTYPE("g'(b+c+d'ef')","g'(b+c+f')(b+c+d')(b+c+e)"),
      TTYPE("a+b+cd(e+f)","(a+b+c)(a+b+d)(a+b+e+f)"),
      //      TTYPE("a'b'c(b+c')","!!"),   // ALWAY FALSE
      TTYPE("f+(a'b'c(b+c'))","f"),   

      TTYPE("n'o'p(a+b+h)(o+p')","!!"),
      TTYPE("a(b+c)","a(b+c)"),      
      TTYPE("q(q'+r)(o+r)","qr"),
      TTYPE("ab+ac","a(b+c)"),
      TTYPE("ab+cd","(a+c)(a+d)(b+c)(b+d)"),

      TTYPE("a+bc","(a+b)(a+c)"),
      TTYPE("a+(bc+de)","(a+b+d)(a+b+e)(a+c+d)(a+c+e)"),
      TTYPE("a+b+c","a+b+c"),
      TTYPE("a(cd+f(x+y+z))","a(c+f)(c+x+y+z)(d+f)(d+x+y+z)"),
      TTYPE("c+(de+f)","(c+d+f)(c+e+f)"),
      TTYPE("(a+b)(c+(de+f))","(a+b)(c+d+f)(c+e+f)"),
      TTYPE("(a+bx)(c+d+f)","(a+b)(a+x)(c+d+f)"),
      TTYPE("(a+bx)(c+d+f)(c+e+f)","(a+b)(a+x)(c+d+f)(c+e+f)"),
      TTYPE("(a+bx)(c+(de+f))","(a+b)(a+x)(c+d+f)(c+e+f)"),
      TTYPE("ac+ad+bc+bd+bd","(a+b)(c+d)"),
      TTYPE("f'(e'+f)","e'f'"),
      TTYPE("b+c(d+e)","(b+c)(b+d+e)"),
      TTYPE("x+(b+c)(b+d+e)","(b+c+x)(b+d+e+x)"),
      //      TTYPE("l+m'+r(n+o'+p+q')","(l+m'+r)(l+n+o'+p+q')(m'+n+o'+p+q')"),
      //      TTYPE("(a+b)(l+m'+r(n+o'+p+q'))","(l+m'+r)(l+n+o'+p+q')(m'+n+o'+p+q')")
      
      //      TTYPE("ace+acf+ade+adf+bce+bcf+bde+bdf","(a+b)(c+d)(e+f)")
      //      TTYPE("(a+d)(b+c+e)","ab+ac+ae+bd+cd+de"),
      //      TTYPE("(a+d+f)(b+c+e)","ab+ac+ae+bd+bf+cd+cf+de+ef"),
      //      TTYPE("x(a+d+f)(b+c)","abx+acx+bdx+bfx+cdx+cfx")


    };

  size_t cnt(0);
  for(const TTYPE& tc : Tests)
    {
      cnt++;
      Algebra A;
      Algebra B;
      Algebra C;
      A.setFunction(std::get<0>(tc));
      B.setFunction(std::get<0>(tc));
      C.setFunction(std::get<1>(tc));
      const std::string preOut=A.display();
      A.expandCNFBracket();
      
      std::string Out=A.display();

      const bool ABflag=A.logicalEqual(B);   // expand : original
      const bool ACflag=A.logicalEqual(C);   // expand : result
      const bool BCflag=B.logicalEqual(C);   // original : result
      
      if (Out!=std::get<1>(tc) || !ABflag)
	{
	  ELog::EM<<"TEST == "<<cnt<<ELog::endDiag;
	  ELog::EM<<"Original   :"<<std::get<0>(tc)<<ELog::endDiag;
	  ELog::EM<<"Expected== :"<<std::get<1>(tc)<<ELog::endDiag;
	  ELog::EM<<"CNFform == :"<<Out<<ELog::endDiag;
	  ELog::EM<<"logic CNF:orig    "<<ABflag<<ELog::endDiag;
	  ELog::EM<<"logic CNF:expect  "<<ACflag<<ELog::endDiag;
	  ELog::EM<<"logic orig:expect "<<BCflag<<ELog::endDiag;
	  return -1;
	}
    }
  return 0;
}


int
testAlgebra::testInsert()
  /*!
    Just check the insertion of a fucntion
    \retval 0 :: success  -1 on failrue
   */
{
  ELog::RegMethod RegA("testAlgebra","testInsert");

  typedef std::tuple<std::string,std::string,std::string> TTYPE;
  const std::vector<TTYPE> Tests=
    {
      TTYPE("a","(b+c)","a(b+c)"),
      TTYPE("bd","a(b+c)","abd(b+c)"),
      TTYPE("(x+y')","abcde(b+c)","abcde(b+c)(x+y')"),
      TTYPE("x+y'","abcde(b+c)","abcde(b+c)(x+y')"),
      TTYPE("x+a","abcde(b+c)(fe+g)","abcde(a+x)(b+c)(g+ef)")
    };

  size_t cnt(0);
  for(const TTYPE& tc : Tests)
    {
      cnt++;
      Algebra A;
      Algebra B;
      A.setFunction(std::get<0>(tc));
      B.setFunction(std::get<1>(tc));
      
      Acomp AC=A.getComp();
      const Acomp& BC=B.getComp();
      AC*=BC;
      std::string Out=AC.display();
      if (Out!=std::get<2>(tc))
	{
	  ELog::EM<<"TEST == "<<cnt<<ELog::endDiag;
	  ELog::EM<<"Failed on  :"<<std::get<0>(tc)<<ELog::endDiag;
	  ELog::EM<<"Failed x   :"<<std::get<1>(tc)<<ELog::endDiag;
	  ELog::EM<<"Algebra    :"<<Out<<ELog::endDiag;
	  return -1;
	}
    }
  return 0;
}

int
testAlgebra::testIsTrue()
  /*!
    Just check the isTrue fuction
    \retval 0 :: success  -1 on failrue
   */
{
  ELog::RegMethod RegA("testAlgebra","testIsTrue");

  typedef std::tuple<std::string,Binary,int> TTYPE;
  const std::vector<TTYPE> Tests=
    {
      TTYPE("a'b'c(d+e+f)(b+c')",Binary("001000"),0),
      TTYPE("a'b'c(d+e+f)(b+c')",Binary("001100"),0),
      TTYPE("a'b'c(d+e+f)(b+c')",Binary("001001"),0)
	    
      // TTYPE("(a+b)",Binary("00"),0),
      // TTYPE("(a+b)",Binary("01"),1),
      // TTYPE("(a+b)",Binary("11"),1),
      // TTYPE("c(a+b)",Binary("010"),0),
      // TTYPE("c(a+b)",Binary("101"),1),
      // TTYPE("(d+c)(abc)",Binary("1110"),1),
      // TTYPE("(a+b)(b+c+d)",Binary("1110"),1),
      // TTYPE("abc+abd+ae",Binary("11111"),1)
    };

  size_t cnt(0);
  for(const TTYPE& tc : Tests)
    {
      cnt++;
      Algebra A;
      A.setFunction(std::get<0>(tc));
      
      const Acomp& AC=A.getComp();
      std::map<int,int> BMap;
      const size_t BNlen=std::get<1>(tc).getPad();
      size_t BN=std::get<1>(tc).getNumber();	
      for(size_t i=0;i<BNlen;i++)
	{
	  BMap.emplace(static_cast<int>(BNlen-i),BN % 2);
	  BN /= 2;
	}
      const int flag=AC.isTrue(BMap);
      if (flag!=std::get<2>(tc))
	{
	  ELog::EM<<"TEST == "<<cnt<<ELog::endDiag;
	  ELog::EM<<"Failed on  :"<<std::get<0>(tc)<<ELog::endDiag;
	  ELog::EM<<"Algebra    :"<<A<<ELog::endDiag;
	  ELog::EM<<"Binary     :"<<std::get<1>(tc)<<ELog::endDiag;
	  ELog::EM<<"Flag       :"<<flag<<ELog::endDiag;
	  return -1;
	}
    }
  return 0;
}

int
testAlgebra::testLogicalEqual()
  /*!
    Expand the bracket int CNF form
    \retval 0 :: success (there is no fail!!!)
   */
{
  ELog::RegMethod RegA("testAlgebra","testLogicalEqual");

  typedef std::tuple<std::string,std::string,int> TTYPE;
  const std::vector<TTYPE> Tests=
    {
      TTYPE("a'b'c(d+e+f)(a+b')","a'b'c(d+e+f)",1)
    };

  size_t cnt(0);
  for(const TTYPE& tc : Tests)
    {
      cnt++;
      Algebra A;
      Algebra B;
      A.setFunction(std::get<0>(tc));
      B.setFunction(std::get<1>(tc));

      const Acomp& AC=A.getComp();
      const Acomp& BC=B.getComp();
      const int flag=AC.logicalEqual(BC);
      if (flag!=std::get<2>(tc))
	{
	  ELog::EM<<"TEST == "<<cnt<<ELog::endDiag;
	  ELog::EM<<"Failed on  :"<<std::get<0>(tc)<<ELog::endDiag;
	  ELog::EM<<"Failed on  :"<<std::get<1>(tc)<<ELog::endDiag;

	  ELog::EM<<"logic AC "<<AC<<ELog::endDiag;
	  ELog::EM<<"logic BC "<<BC<<ELog::endDiag;
	  ELog::EM<<"logicEqual== "<<flag<<ELog::endDiag;
	  return -1;
	}
    }
  return 0;
}

int
testAlgebra::testLogicalCover()
  /*!
    Expand the bracket int CNF form
    \retval 0 :: success (there is no fail!!!)
   */
{
  ELog::RegMethod RegA("testAlgebra","testLogicalCover");

  typedef std::tuple<std::string,std::string,int> TTYPE;
  const std::vector<TTYPE> Tests=
    {
      TTYPE("(a+b)","(a+b+c)",1),
      TTYPE("(a+b+c+d)","(a+b+c+d)",3)
    };

  size_t cnt(0);
  for(const TTYPE& tc : Tests)
    {
      cnt++;
      Algebra A;
      Algebra B;
      A.setFunction(std::get<0>(tc));
      B.setFunction(std::get<1>(tc));

      const Acomp& AC=A.getComp();
      const Acomp& BC=B.getComp();
      const int flag=AC.logicalIntersectCover(BC);
      if (flag!=std::get<2>(tc))
	{
	  ELog::EM<<"TEST == "<<cnt<<ELog::endDiag;
	  ELog::EM<<"Failed on  :"<<std::get<0>(tc)<<ELog::endDiag;
	  ELog::EM<<"Failed on  :"<<std::get<1>(tc)<<ELog::endDiag;

	  ELog::EM<<"logic AC "<<AC<<ELog::endDiag;
	  ELog::EM<<"logic BC "<<BC<<ELog::endDiag;
	  ELog::EM<<"logicCover== "<<flag<<ELog::endDiag;
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
    \retval -1 :: fail on CNF expansion
    \retval -2 :: fail on DNF expansion
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
	  ELog::EM<<"Return value   :"<<Out<<ELog::endDiag;
	  ELog::EM<<"Exprected      :"<<std::get<1>(tc)<<ELog::endDiag;
	  return -1;
	}
      
      A.makeDNF();
      Out=A.display();
      if (Out!=std::get<2>(tc))
	{
	  ELog::EM<<"Failed on DNF  :"<<std::get<0>(tc)<<ELog::endDiag;
	  ELog::EM<<"Expected       :"<<std::get<2>(tc)<<ELog::endDiag;
	  ELog::EM<<"Result         :"<<Out<<ELog::endDiag;
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
  const std::vector<TTYPE> Tests=
    {
      TTYPE("(a:b)","a+b","a'b'"),
      TTYPE("a+bc","a+bc","a'(b'+c')"),

      TTYPE("a+(b+c)(e+d)",
	    "a+(b+c)(d+e)",
	    "a'(d'e'+b'c')"),
      
      TTYPE("a'bcd+a(cd+ff(x+y+z))",
	    "a'bcd+a(cd+f(x+y+z))",
	    "(a'+(f'+x'y'z')(c'+d'))(a+b'+c'+d')"),
      
      TTYPE("(cd+ff)+b","b+f+cd","b'f'(c'+d')")
    };

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
  const std::vector<TTYPE> Tests=
    {
      TTYPE("a+b'+(c)","a+b","(a+b'+c)(a+b)")
    };

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

  typedef std::tuple<std::string,std::string> TTYPE;
  const std::vector<TTYPE> Tests=
    {
      TTYPE("ac+ad+bc+bd+ae'","a+b"),
      TTYPE("afbc(x+y+z)","abc"),
      TTYPE("afbc(x+y+z)","x+y") 
    };

  
  for(const TTYPE tc : Tests)
    {
      Algebra A;
      A.setFunction(std::get<0>(tc));
      Algebra B;
      B.setFunction(std::get<1>(tc));

      std::pair<Algebra,Algebra> X=A.algDiv(B);
      // Check multiplication:
      Algebra XY=X.first*B;
      XY+=X.second;     

      if (!XY.logicalEqual(A))
	{
	  ELog::EM<<"A == "<<A<<ELog::endDiag;
	  ELog::EM<<"B == "<<B<<ELog::endDiag;
	  ELog::EM<<ELog::endDiag;
	  ELog::EM<<"A div == "<<X.first<<ELog::endDiag;
	  ELog::EM<<"A rem == "<<X.second<<ELog::endDiag;
	  ELog::EM<<ELog::endDiag;
	  ELog::EM<<"Multiplied "<<XY<<ELog::endDiag;
	  ELog::EM<<"Logic:"<<XY.logicalEqual(A)<<ELog::endDiag;
	  ELog::EM<<ELog::endDiag;
	  XY.makeDNF();
	  ELog::EM<<"XY(DNF):"<<XY<<ELog::endDiag;      
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
  Tests.push_back(TTYPE("(a+b)","a'b'"));
  Tests.push_back(TTYPE("ab((c'(d+e+f')g'h'i')+(gj'(k+l')(m+n)))",
			"a'+b'+(g'+j+m'n'+k'l)(c+g+h+i+d'e'f)"));

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
testAlgebra::testResolveTrue()
  /*!
    Expand the bracket form 
    \retval 0 :: success (there is no fail!!!)
   */
{
  ELog::RegMethod RegA("testAlgebra","testResolveTrue");

  typedef std::tuple<std::string,std::string,std::string> TTYPE;
  const std::vector<TTYPE> Tests=
    {
      // stuff for CNF
      //      TTYPE("a",0,"bcd","(a+b)(a+c)(a+d)"),
      TTYPE("a+b","b","~~"),
      TTYPE("ab(c+d)","c","ab"),
      TTYPE("ab(c+de)","c","ab"),
      TTYPE("ab(c+de)","a'","!!"),
      TTYPE("ab(c+de)","c'","abde")
    };

  for(const TTYPE& tc : Tests)
    {
      Algebra A;
      A.setFunction(std::get<0>(tc));
      A.resolveTrue(std::get<1>(tc));
      std::string Out=A.display();
      if (Out!=std::get<2>(tc))
	{
	  ELog::EM<<"Failed on  :"<<std::get<0>(tc)<<ELog::endDiag;
	  ELog::EM<<"True var   :"<<std::get<1>(tc)<<ELog::endDiag;
	  ELog::EM<<"Expect     :"<<std::get<2>(tc)<<ELog::endDiag;
	  
	  ELog::EM<<"Display == :"<<Out<<ELog::endDiag;
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
  Tests.push_back(TTYPE("(1 : -2 : 3 : -4)","a+b'+c+d'"));

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
  ELog::RegMethod RegA("testAlgebra","testSetFunction");


  typedef std::tuple<std::string,std::string> TTYPE; 
  std::vector<TTYPE> Tests;
  Tests.push_back(TTYPE("#(f+i)(xy)","f'i'xy"));

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
