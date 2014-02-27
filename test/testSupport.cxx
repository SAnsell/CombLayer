/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   test/testSupport.cxx
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
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <cmath>
#include <complex>
#include <vector>
#include <string>
#include <algorithm>
#include <boost/regex.hpp>
#include <boost/tuple/tuple.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "mathSupport.h"
#include "support.h"
#include "regexSupport.h"

#include "testFunc.h"
#include "testSupport.h"

using namespace StrFunc;

testSupport::testSupport()
  /*!
    Constructor
   */
{}

testSupport::~testSupport()
  /*!
    Destructor
  */
{}

int 
testSupport::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: Index of test
    \returns -ve on error 0 on success.
  */
{
  ELog::RegMethod RegA("testSupport","applyTest");

  typedef int (testSupport::*testPtr)();
  testPtr TPtr[]=
    {
      &testSupport::testConvert,
      &testSupport::testConvPartNum,
      &testSupport::testExtractWord,
      &testSupport::testFullBlock,
      &testSupport::testItemize,
      &testSupport::testSection,
      &testSupport::testSectPartNum,
      &testSupport::testSingleLine,
      &testSupport::testStrComp,
      &testSupport::testStrFullCut,
      &testSupport::testStrParts,
      &testSupport::testStrRemove,
      &testSupport::testStrSplit
    };

  const std::string TestName[]=
    {
      "Convert",
      "ConvPartNum",
      "ExtractWord",
      "FullBlock",
      "Itemize",
      "Section",
      "SectPartNum",
      "SingleLine",
      "StrComp",
      "StrFullCut",
      "StrParts",
      "StrRemove",
      "StrSplit"
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
testSupport::testConvert()
  /*!
    Applies a test to convert
    \retval -1 :: failed to convert a good double
    \retval -2 :: converted a number with leading stuff
    \retval -3 :: converted a number with trailing stuff
    \retval 0 on success
  */
{
  ELog::RegMethod RegA("testSupport","testConvert");

  // type : Init string : final : results : (outputs)
  typedef boost::tuple<int,std::string,int,
		       int,double,std::string> TTYPE;
  std::vector<TTYPE> Tests;

  int resultFlag;       // Section return
  bool checkFlag;       // Output matches

  Tests.push_back(TTYPE(2,"V 1 tth ",1,0,0.0,"V"));
  Tests.push_back(TTYPE(0,"1 tth ",1,1,0.0,""));
  Tests.push_back(TTYPE(0,"tth ",0,1,0.0,""));
  Tests.push_back(TTYPE(1,"0.0000",1,0,0.0000,""));
  Tests.push_back(TTYPE(1,"-0.0001",1,0,-0.0001,""));
  Tests.push_back(TTYPE(2,"0.0001",1,0,0.0,"0.0001"));
  Tests.push_back(TTYPE(0,"-10004      ",1,-10004,0.0,""));
  Tests.push_back(TTYPE(1," e3.4 ",0,0,0.0,""));
  Tests.push_back(TTYPE(1," 3.4g ",0,0,0.0,""));
  Tests.push_back(TTYPE(1," 3e4 ",1,0,30000.0,""));
  Tests.push_back(TTYPE(1," 3",1,0,3,""));

  std::vector<TTYPE>::const_iterator tc;
  for(tc=Tests.begin();tc!=Tests.end();tc++)
    {
      std::string outS;
      double outD(0.0);
      int outI(0);

      const int typeFlag=tc->get<0>();
      const std::string TLine=tc->get<1>();
      if (typeFlag==0)        // Int
	{
	  resultFlag=convert(TLine,outI);
	  checkFlag= (tc->get<3>()!=outI);
	}
      else if (typeFlag==1)        // double
	{
	  resultFlag=convert(TLine,outD);
	  checkFlag= (fabs(tc->get<4>()-outD)>1e-15);
	}
      else                          // String
	{
	  resultFlag=convert(TLine,outS);
	  checkFlag= (tc->get<5>()!=outS);
	}

      if (resultFlag!=tc->get<2>() || (resultFlag && checkFlag))
	{
	  ELog::EM<<"TEST :: "<<(tc-Tests.begin())+1<<ELog::endDiag;
	  ELog::EM<<"Result == "<<resultFlag<<" ("
		  <<tc->get<2>()<<")"<<ELog::endDiag;
	  ELog::EM<<"Output[Int] == "<<outI<<":"<<tc->get<3>()
		  <<ELog::endDiag;
	  ELog::EM<<"Output[double] == "<<outD<<":"<<tc->get<4>()
		  <<ELog::endDiag;
	  ELog::EM<<"Output[String] == "<<outS<<":"<<tc->get<5>()
		  <<ELog::endDiag;
	  return -1;
	}
    }
  return 0;
}

int
testSupport::testConvPartNum()
  /*!
    Applies a test to convPartNum
    \return -ve on error
  */
{
  ELog::RegMethod RegA("testSupport","testConvPartNum");

  // type : Init string : final : results : (outputs)
  typedef boost::tuple<int,std::string,size_t,int,double> TTYPE;
  std::vector<TTYPE> Tests;
  
  size_t resultFlag(1);       // Section return
  bool checkFlag(1);       // Output matches
  
  Tests.push_back(TTYPE(0,"1 tth ",1,1,0.0));
  Tests.push_back(TTYPE(0,"tth ",0,1,0.0));
  Tests.push_back(TTYPE(1,"0.0000",6,0,0.0000));
  Tests.push_back(TTYPE(1,"-0.0001",7,0,-0.0001));
  Tests.push_back(TTYPE(0,"-10004      ",6,-10004,0.0));
  Tests.push_back(TTYPE(1," e3.4 ",0,0,0.0));
  Tests.push_back(TTYPE(1," 3.4g ",4,0,3.4));
  Tests.push_back(TTYPE(1," 3e4 ",4,0,30000.0));
  Tests.push_back(TTYPE(1," 3g",2,0,3.0));

  std::vector<TTYPE>::const_iterator tc;
  for(tc=Tests.begin();tc!=Tests.end();tc++)
    {
      double outD(0.0);
      int outI(0);

      const int typeFlag=tc->get<0>();
      const std::string TLine=tc->get<1>();
      if (typeFlag==0)        // Int
	{
	  resultFlag=convPartNum(TLine,outI);
	  checkFlag= (tc->get<3>()!=outI);
	}
      else if (typeFlag==1)        // double
	{
	  resultFlag=convPartNum(TLine,outD);
	  checkFlag= (fabs(tc->get<4>()-outD)>1e-15);
	}

      if (resultFlag!=tc->get<2>() || (resultFlag && checkFlag))
	{
	  ELog::EM<<"TEST :: "<<(tc-Tests.begin())+1<<ELog::endDiag;
	  ELog::EM<<"Result == "<<resultFlag<<" ("
		  <<tc->get<2>()<<")"<<ELog::endDiag;
	  ELog::EM<<"Output[Int] == "<<outI<<":"<<tc->get<3>()
		  <<ELog::endDiag;
	  ELog::EM<<"Output[double] == "<<outD<<":"<<tc->get<4>()
		  <<ELog::endDiag;
	  return -1;
	}
    }
  return 0;
}


int
testSupport::testExtractWord()
  /*!
    Applies a test to the extractWord
    The object is to find a suitable lenght
    of a string in a group of words
    \retval -1 :: failed find word in string
    when the pattern exists.
  */
{
  ELog::RegMethod RegA("testSupport","testExtractWork");

  std::string Ln="Name wav wavelength other stuff";
  int retVal=extractWord(Ln,"wavelengt",4);
  if (!retVal ||
      Ln!="Name wav  other stuff")
    {
      ELog::EM<<"Extract work == "<<retVal<<ELog::endErr;
      ELog::EM<<"X == "<<Ln<< "=="<<ELog::endErr;
      return -1;
    }
  return 0;  
}

int
testSupport::testFullBlock()
  /*!
    Applies a test to the fullBlock Command 
    \retval -1 :: failed find word in string
    when the pattern exists.
  */
{
  ELog::RegMethod RegA("testSupport","testFullBlock");

  typedef boost::tuple<std::string,std::string> TTYPE;
  std::vector<TTYPE> Tests;
  
  Tests.push_back(TTYPE("abcde","abcde"));
  Tests.push_back(TTYPE("abcde)","abcde)"));
  
  std::vector<TTYPE>::const_iterator tc;
  for(tc=Tests.begin();tc!=Tests.end();tc++)
    {
      std::string Out=StrFunc::fullBlock(tc->get<0>());
      if (Out!=tc->get<1>())
	{
	  ELog::EM<<"Input  == "<<tc->get<0>()<<" =="<<ELog::endTrace;
	  ELog::EM<<"Out    == "<<Out<<" =="<<ELog::endTrace;
	  ELog::EM<<"Expect == "<<tc->get<1>()<<" =="<<ELog::endTrace;
	  return -1;
	}
    }
  return 0;  
}


int
testSupport::testItemize()
  /*!
    Applies a test to the extractWord
    The object is to find a suitable lenght
    of a string in a group of words
    \retval -1 :: failed find word in string
    when the pattern exists.
  */
{
  ELog::RegMethod RegA("testSupport","testItemize");

  std::string Ln="Name   6Gap  -3.4 next";
  typedef boost::tuple<int,std::string,double> ResTYPE; 
  std::vector<ResTYPE> Results;
  Results.push_back(ResTYPE(-1,"Name",0.0));
  Results.push_back(ResTYPE(-1,"6Gap",0.0));
  Results.push_back(ResTYPE(1,"-3.4",-3.4));
  Results.push_back(ResTYPE(-1,"next",0.0));
  Results.push_back(ResTYPE(0,"",0.0));
  
  std::string Out;
  double item(1); 
  for(size_t i=0;i<Results.size();i++) 
    {
      const int flag=itemize(Ln,Out,item);
      if (flag!=Results[i].get<0>() ||
	  (flag && Results[i].get<1>()!=Out) ||
	  (flag==1 && fabs(Results[i].get<2>()-item)>1e-7) )
        {
	  ELog::EM<<"Failed on "<<i+1
		  <<":: flag == "<<flag<<" "<<Out<<" "<<item<<ELog::endErr;
	  ELog::EM<<"Expected :: flag == "
		  <<Results[i].get<0>()<<" "<<Results[i].get<1>()<<" "
		  <<Results[i].get<2>()<<ELog::endErr;
	  return -1;
	}
    }
  return 0;  
}

int
testSupport::testSection()
  /*!
    Applies a test to section
    \retval -1 :: failed to section a string
    \retval 0 on success
  */
{
  ELog::RegMethod RegA("testSupport","testSection");

  // type : Init string : final : results : (outputs)
  typedef boost::tuple<int,std::string,std::string,int,
		       int,double,std::string> TTYPE;
  std::vector<TTYPE> Tests;

  int resultFlag;       // Section return
  bool checkFlag;       // Output matches

  Tests.push_back(TTYPE(2,"V 1 tth ","1 tth ",1,0,0.0,"V"));
  Tests.push_back(TTYPE(0,"1 tth ","tth ",1,1,0.0,""));
  Tests.push_back(TTYPE(0,"tth ","tth ",0,1,0.0,""));
  Tests.push_back(TTYPE(1,"0.0001","",1,0,0.0001,""));
  Tests.push_back(TTYPE(2,"0.0001","",1,0,0.0,"0.0001"));
  Tests.push_back(TTYPE(1," e3.4 ","",0,0,0.0,""));
  Tests.push_back(TTYPE(1," 3.4g ","",0,0,0.0,""));
  Tests.push_back(TTYPE(1," 3e4 ","",1,0,30000.0,""));
  Tests.push_back(TTYPE(3,"3","",1,3,0.0,""));
  
  std::vector<TTYPE>::const_iterator tc;
  for(tc=Tests.begin();tc!=Tests.end();tc++)
    {
      std::string outS;
      double outD(0.0);
      int outI(0);
      size_t outUI(0);

      const int typeFlag=tc->get<0>();
      std::string TLine=tc->get<1>();
      if (typeFlag==0)        // Int
	{
	  resultFlag=section(TLine,outI);
	  checkFlag= (tc->get<4>()!=outI || 
		      tc->get<2>()!=TLine);
	}
      else if (typeFlag==1)        // double
	{
	  resultFlag=section(TLine,outD);
	  checkFlag= (fabs(tc->get<5>()-outD)>1e-15 || 
		      tc->get<2>()!=TLine);
	}
      else if (typeFlag==2)        // String
	{
	  resultFlag=section(TLine,outS);
	  checkFlag= (tc->get<6>()!=outS || tc->get<2>()!=TLine);
	}
      else         // Size_t
	{
	  resultFlag=section(TLine,outUI);
	  checkFlag= (static_cast<size_t>(tc->get<4>())!=outUI 
					 || tc->get<2>()!=TLine);
	}

      if (resultFlag!=tc->get<3>() || (resultFlag && checkFlag))
	{
	  ELog::EM<<"TEST :: "<<(tc-Tests.begin())+1<<ELog::endDiag;
	  ELog::EM<<"Result == "<<resultFlag<<" ("
		  <<tc->get<3>()<<")"<<ELog::endDiag;
	  ELog::EM<<"Output[Int] == "<<outI<<":"<<tc->get<4>()
		  <<ELog::endDiag;
	  ELog::EM<<"Output[double] == "<<outD<<":"<<tc->get<5>()
		  <<ELog::endDiag;
	  ELog::EM<<"Output[String] == "<<outS<<":"<<tc->get<6>()
		  <<ELog::endDiag;
	  ELog::EM<<"Final string :"<<tc->get<2>()<<ELog::endDiag;
	  ELog::EM<<"Found string :"<<TLine<<ELog::endDiag;
	  return -1;
	}
    }
  return 0;
}

int
testSupport::testSectPartNum()
  /*!
    Applies a test to sectPartNum
    \retval -1 :: failed to convert a good double
    \retval -2 :: failed to get a NumX
    \retval -3 :: converted a number with leading stuff
    \retval 0 on success
  */
{
  ELog::RegMethod RegA("testSupport","testSectPartNum");

  double X;
  std::string NTest="   3.4   ";
  if (!sectPartNum(NTest,X) || X!=3.4)
    {
      ELog::EM<<"testSectPartNum Fail 1"<<ELog::endErr;
      return -1;
    }
  X=9.0;
  NTest="   3.4g   ";
  if (!sectPartNum(NTest,X) || X!=3.4)
    {
      ELog::EM<<"testSectPartNum Fail 2 "<<X<<ELog::endErr;
      return -2;
    }
  X=9.0;
  NTest="   e3.4   ";
  if (sectPartNum(NTest,X))
    {
      ELog::EM<<"testSupport Fail 3 : "<<X<<" :"<<NTest<<ELog::endErr;
      return -3;
    }
  return 0;
}

int
testSupport::testSingleLine()
  /*!
    Test the single line function
    \return -1 on error 
  */
{
  ELog::RegMethod RegA("testSupport","testSingleLine");  

  // type : Init string : final : results : (outputs)
  typedef boost::tuple<std::string,std::string> TTYPE;
  std::vector<TTYPE> Tests;

  Tests.push_back(TTYPE("2   3","2 3"));
  
  std::vector<TTYPE>::const_iterator tc;
  for(tc=Tests.begin();tc!=Tests.end();tc++)
    {
      const std::string Res=StrFunc::singleLine(tc->get<0>());
      if (tc->get<1>()!=Res)
	{
	  ELog::EM<<"TEST :: "<<(tc-Tests.begin())+1<<ELog::endDiag;
	  ELog::EM<<"Result == "<<Res<<" == "<<ELog::endDiag;
	  ELog::EM<<"Expec  == "<<tc->get<1>()<<" == "<<ELog::endDiag;
	  return -1;
	}
    }
  return 0;

}

int
testSupport::testStrComp()
  /*!
    Applies a test to the StrComp.
    Gets a single component from a single serach
    \retval -1 :: Failed first test
  */
{
  ELog::RegMethod RegA("testSupport","testStrComp");

  std::string Target ="6log 8.9 90.0";
  std::string searchString="(^|\\s)(\\d+)log(\\s|$)";
  boost::regex Re(searchString);

  int Out(0);
  if (!StrFunc::StrComp(Target,Re,Out,1) ||
      Out!=6)
    {
      ELog::EM<<"Init :"<<Target<<ELog::endErr;
      ELog::EM<<"Reg :"<<searchString<<ELog::endErr;
      ELog::EM<<"StrComp == "<<StrFunc::StrComp(Target,Re,Out,3)<<ELog::endErr;
      ELog::EM<<"X == "<<Out<< "=="<<ELog::endErr;
      return -1;
    }
  return 0;  
}

int
testSupport::testStrFullCut()
  /*!
    Applies a test to the StrCut
    \retval -1 :: failed StrSplit into strings 
    when the pattern exists.
    \retval -2 :: succeeded on a failure pattern
    \retval 0 :: success
  */
{
  ELog::RegMethod RegA("testSupport","testStrFullCut");

  // Input : Search : results : Remainder
  typedef boost::tuple<std::string,std::string,double,
		       std::string,std::string> TTYPE;
  std::vector<TTYPE> Tests;
  

  // remove space/> at start then split on 
  Tests.push_back(TTYPE("xxx $var 4.5     6.7 8.9 90.0",
			"\\s*\\$\\S+\\s+(\\S+)\\s+\\S+",
			4.5,"",
			"xxx 8.9 90.0"));

  Tests.push_back(TTYPE("<test attrb=test><Axd> ",
			"<([^>]+)\\s+([^>]*)>",
			0,"test:attrb=test:",
			"<Axd> "));
		  
  Tests.push_back(TTYPE(" mx54:h my",
			"^\\s{0,5}([mM]\\D*)",
			0,"mx:",
			"54:h my"));
		  
  std::vector<TTYPE>::const_iterator tc;
  for(tc=Tests.begin();tc!=Tests.end();tc++)
    {
      double Out=0.0;
      std::vector<std::string> OutVec;
      std::ostringstream cx;
      std::string Target=tc->get<0>();
      boost::regex Re(tc->get<1>());
      int flag(0);
      // Basic test
      if (tc->get<3>().empty())
	{

	  flag=StrFullCut(Target,Re,Out,0);
	  if (Out!=tc->get<2>())
	    flag=0;
	}
      // Vector test:
      else
	{
	  flag=StrFullCut(Target,Re,OutVec);
	  copy(OutVec.begin(),OutVec.end(),
	       std::ostream_iterator<std::string>(cx,":"));
	  if (cx.str()!=tc->get<3>())
	    flag=0;
	}
      if (!flag || Target!=tc->get<4>())
	{
	  ELog::EM<<"Init == "<<tc->get<0>()<<ELog::endTrace;
	  ELog::EM<<"Search == "<<tc->get<1>()<<ELog::endTrace;
	  ELog::EM<<"Target == "<<Target<<" =="<<ELog::endTrace;
	  ELog::EM<<"Out == "<<Out<<ELog::endTrace;
	  ELog::EM<<"CX == "<<cx.str()<<" =="<<ELog::endTrace;
	  return -1;
	}
    }
  return 0;
}

int
testSupport::testStrRemove()
  /*!
    Applies a test to the StrRemove
    \retval -2 :: succeeded on a failure pattern
    \retval 0 :: success
  */
{
  
  ELog::RegMethod RegA("testSupport","testStrRemove");

  typedef boost::tuple<std::string,std::string,
		       std::string,std::string> TTYPE;
  std::vector<TTYPE> Tests;

  Tests.push_back(TTYPE(" $var x4.5    x6.7   8.9 x90.1 7.3 x44.5 theEnd",
			"x\\S+","x4.5",
			" $var     x6.7   8.9 x90.1 7.3 x44.5 theEnd"));

  std::vector<TTYPE>::const_iterator tc;
  for(tc=Tests.begin();tc!=Tests.end();tc++)
    {
      std::string Y=tc->get<0>();
      boost::regex sstr(tc->get<1>());
      std::string Out;
      if (!StrRemove(Y,Out,sstr) ||
	  Out!=tc->get<2>() || Y!=tc->get<3>())
	{
	  ELog::EM<<"Init :"<<tc->get<0>()<<ELog::endTrace;
	  ELog::EM<<"Regex :"<<tc->get<1>()<<ELog::endTrace;
	  ELog::EM<<"Remain :"<<Y<<ELog::endTrace;
	  ELog::EM<<"Out :"<<Out<<ELog::endTrace;
	  return -1;
	}
    }

  return 0;
}

int
testSupport::testStrParts()
  /*!
    Applies a test to the StrParts function
    This splits the stringinto parts based on 
    \retval 0 :: success
  */
{
  ELog::RegMethod RegA("testSupport","testStrParts");
  
  typedef boost::tuple<std::string,std::string,unsigned int,std::string> TTYPE;
  std::vector<TTYPE> Tests;
  
  // remove space/> at start then split on 
  Tests.push_back(TTYPE("$var s566>s4332 dxx","[\\s>]*([^\\s>]+)",4,"$var:s566:s4332:dxx:"));

  std::vector<TTYPE>::const_iterator tc;
  for(tc=Tests.begin();tc!=Tests.end();tc++)
    {
      std::string Y=tc->get<0>();
      boost::regex sstr(tc->get<1>());
      std::vector<std::string> outVec=StrFunc::StrParts(Y,sstr);      
      std::ostringstream cx;
      copy(outVec.begin(),outVec.end(),
	   std::ostream_iterator<std::string>(cx,":"));

      if (outVec.size()!=tc->get<2>() || cx.str()!=tc->get<3>())
	{
	  ELog::EM<<"Test of :"<<tc->get<0>()<<ELog::endTrace;
	  ELog::EM<<"Remain[] == "<<Y<< "=="<<ELog::endTrace;
	  ELog::EM<<"Out == "<<cx.str()<<ELog::endTrace;
	  return -1;
	}
    }
  return 0;
}

int 
testSupport::testStrSplit()
  /*!
    Test the StrFullSplit/StrSingleSplit test
    \retval -1 on StrFullSplit failure
  */
{
  ELog::RegMethod RegA("testSupport","testStrSplit");

  // Start with a <(name)<spc> !> >       
  std::string searchString="<(\\S+)\\s+([^>]*)";
  std::string Target="Pre : <Key Attrib=test> <KeyB Attrib=> KeyC <KeyD A=Y>";
  boost::regex Re(searchString);
  std::vector<std::string> Out;
  StrFunc::StrFullSplit(Target,Re,Out);
  if (Out.size()!=6 || 
      Out[0]!="Key" || Out[1]!="Attrib=test" ||
      Out[2]!="KeyB" || Out[3]!="Attrib=" ||
      Out[4]!="KeyD" || Out[5]!="A=Y")
    {
      ELog::EM<<" ---- StrFullSplit ------- "<<ELog::endErr;
      ELog::EM<<"SearhString == "<<searchString<<ELog::endErr;
      ELog::EM<<"Target == "<<Target<<ELog::endErr;
      ELog::EM<<"Out Size == "<<Out.size()<<ELog::endErr;
      copy(Out.begin(),Out.end(),
	   std::ostream_iterator<std::string>(ELog::EM.Estream()," : "));
      ELog::EM<<ELog::endErr;
      return -1;
    }

  Out.clear();
  if (!StrFunc::StrSingleSplit(Target,Re,Out) ||
      Out.size()!=2 || Out[0]!="Key" ||
      Out[1]!="Attrib=test")
    {
      ELog::EM<<" ---- StrSingleSplit ------- "<<ELog::endErr;
      ELog::EM<<"SearhString == "<<searchString<<ELog::endErr;
      ELog::EM<<"Target == "<<Target<<ELog::endErr;
      ELog::EM<<"Out Size == "<<Out.size()<<ELog::endErr;
      copy(Out.begin(),Out.end(),
	   std::ostream_iterator<std::string>(ELog::EM.Estream()," : "));
      ELog::EM<<ELog::endErr;
      return -2;
    }
  

  // Start with a <(name)<spc> !> >       
  Out.clear();
  searchString="(^|\\D+)(\\d*)";
  Target="19 : -56 -45 (4:- 5)";
  boost::regex ReX(searchString,boost::regex::perl);
  StrFunc::StrFullSplit(Target,ReX,Out);
  std::ostringstream cx;
  copy(Out.begin(),Out.end(),
       std::ostream_iterator<std::string>(cx,""));
  if (Out.size()!=12 || 
      cx.str()!=Target)
    {
      ELog::EM<<" ---- StrFullSplit ------- "<<ELog::endErr;
      ELog::EM<<"SearhString == "<<searchString<<ELog::endErr;
      ELog::EM<<"Target == "<<Target<<ELog::endErr;
      ELog::EM<<"Out Size == "<<Out.size()<<ELog::endErr;
      ELog::EM<<"Report == "<<cx.str()<<ELog::endErr;
      copy(Out.begin(),Out.end(),
	   std::ostream_iterator<std::string>(ELog::EM.Estream(),"xx"));
      ELog::EM<<ELog::endErr;
      return -3;
    }

  // Start with a <(name)<spc> !> >       
  Out.clear();
  searchString="(\\d+)\\.(\\d\\d)(\\S)";
  Target=" 54097.70c 4506.80c";
  boost::regex ReY(searchString,boost::regex::perl);
  StrFunc::StrSingleSplit(Target,ReY,Out);
  cx.str("");
  copy(Out.begin(),Out.end(),
       std::ostream_iterator<std::string>(cx,":"));
  if (Out.size()!=3 || 
      cx.str()!="54097:70:c:")
    {
      ELog::EM<<" ---- StrSingleSplit ------- "<<ELog::endErr;
      ELog::EM<<"SearhString == "<<searchString<<ELog::endErr;
      ELog::EM<<"Target == "<<Target<<ELog::endErr;
      ELog::EM<<"Out Size == "<<Out.size()<<ELog::endErr;
      ELog::EM<<"Report == "<<cx.str()<<ELog::endErr;
      copy(Out.begin(),Out.end(),
	   std::ostream_iterator<std::string>(ELog::EM.Estream(),"xx"));
      ELog::EM<<ELog::endErr;
      return -4;
    }

  return 0;
}
