/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   test/testInputParam.cxx
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
#include <complex>
#include <vector>
#include <set>
#include <map>
#include <stack>
#include <string>
#include <sstream>
#include <algorithm>
#include <boost/tuple/tuple.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "mathSupport.h"
#include "MapSupport.h"
#include "InputControl.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "IItemBase.h"
#include "inputParam.h"

#include "testFunc.h"
#include "testInputParam.h"

using namespace mainSystem;

testInputParam::testInputParam() 
///< Constructor
{}

testInputParam::~testInputParam() 
///< Destructor
{}

void
testInputParam::initInput(const std::string Input[],
			  std::vector<std::string>& Names)
  /*!
    Create an input string from a terminated array
    \param Input :: String array [Terminated with empty string]
    \param Names :: Vector to add output to
  */
{
  for(int i=0;!Input[i].empty();i++)
    Names.push_back(Input[i]);
  return;
}

int 
testInputParam::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: Index of test to run
    \return 0 on success / -ve on error
  */
{
  ELog::RegMethod RegA("testInputParam","applyTest");
  TestFunc::regSector("testInputParam");

  typedef int (testInputParam::*testPtr)();
  testPtr TPtr[]=
    {
      &testInputParam::testDefValue,
      &testInputParam::testFlagDef,
      &testInputParam::testGetValue,
      &testInputParam::testInput,
      &testInputParam::testMulti,
      &testInputParam::testMultiExtract,
      &testInputParam::testMultiTail,
      &testInputParam::testSetValue,
      &testInputParam::testWrite,
      &testInputParam::testWriteDesc,
    };
  const std::string TestName[]=
    {
      "DefValue",
      "FlagDef",
      "GetValue",
      "Input",
      "Multi",
      "MultiExtract",
      "MultiTail",
      "SetValue",
      "Write",
      "WriteDesc"
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
testInputParam::testDefValue()
  /*!
    Test the default regisitration
    \return 0 on success
  */
{
  ELog::RegMethod RegA("testInputParam","testDefValue");
  
  inputParam A;
  A.regItem<double>("d","dbl");
  // Test partial [Success]
  A.regDefItem<int>("i","int",3,-8);
  // Test partial [Success]
  A.regDefItem<int>("j","jint",3,-8,-9);
  A.regDefItem<std::string>("X","xmlout",1,
			    "Model.xml");

  std::vector<std::string> Names;
  Names.push_back("-k");
  Names.push_back("-i");
  Names.push_back("10");
  Names.push_back("7");
  Names.push_back("-X");
  Names.push_back("test.xml");
  
  A.processMainInput(Names);
  if (!A.flag("i") || A.getValue<int>("i",0)!=10 ||
      A.getValue<int>("i",1)!=7 || 
      A.getValue<int>("i",2)!=-8)
    {
      ELog::EM<<"ITEM -i set=="<<A.flag("i")<<ELog::endTrace;
      ELog::EM<<"Item[1,2,3] == "<<A.getValue<int>("i",0)
	      <<" "<<A.getValue<int>("i",1)
	      <<" "<<A.getValue<int>("i",2)
	      <<ELog::endTrace;
      return -1;
    }

  if (!A.flag("X") || A.getValue<std::string>("X",0)!="test.xml")
    {
      ELog::EM<<"ITEM -X set=="<<A.flag("X")<<ELog::endTrace;
      ELog::EM<<"Item[1,2,3] == "<<A.getValue<std::string>("X",0)
	      <<ELog::endTrace;
      return -2;
    }

  return 0;
}

int
testInputParam::testGetValue()
  /*!
    Test adding stuff to the tree.
    \return 0 on success
  */
{
  ELog::RegMethod RegA("testInputParam","testGetValue");
  
  inputParam A;
  A.regItem<double>("d","dbl");
  A.regItem<int>("i","int");
  
  try
    {
      A.setValue("i",10);
      A.setValue("d",20.0);
      const double x=A.getValue<double>("d");
      if (fabs(x-20.0)>1e-8)
	{
	  ELog::EM<<"Failed on double:"<<x<<" "<<20.0<<ELog::endTrace;
	  return -1;
	}
    }
  catch (ColErr::InContainerError<std::string>& EA)
    {
      const std::string& SearchObj=EA.getItem();
      ELog::EM<<"Failed to find "<<SearchObj<<ELog::endTrace;
      return -2;
    }
  return 0;
}

int
testInputParam::testFlagDef()
  /*!
    Test the default system
    \return 0 on success
  */
{
  ELog::RegMethod RegA("testInputParam","testFlagDef");
  FuncDataBase Control;
  
  Control.addVariable("testX",20.3);

  inputParam A;
  A.regItem<double>("x","xlong");
  A.regMulti<double>("m","mlong",1);
  A.setValue("x",30.0);

  // key : varKey : index : Except flag : result
  typedef boost::tuple<std::string,std::string,
		       size_t,bool,double> TTYPE;
  std::vector<TTYPE> Tests;

  Tests.push_back(TTYPE("x","testX",0,1,30.0));
  Tests.push_back(TTYPE("y","testX",0,1,20.30));
  Tests.push_back(TTYPE("x","nothing",0,1,30.0));
  Tests.push_back(TTYPE("y","nothing",0,0,0.0));
  
  std::vector<TTYPE>::const_iterator tc;  
  for(tc=Tests.begin();tc!=Tests.end();tc++)
    {
      try
	{
	  const double D=
	    A.getFlagDef<double>(tc->get<0>(),Control,
				 tc->get<1>(),tc->get<2>());
	  if (!tc->get<3>() || fabs(D-tc->get<4>())>1e-5)
	    {
	      ELog::EM<<"Key "<<tc->get<0>()<<" "
		      <<tc->get<1>()<<ELog::endTrace;
	      ELog::EM<<"D "<<D<<ELog::endTrace;
	      return -11;
	    }  
        }
      catch (ColErr::InContainerError<std::string>& EX)
	{
	  if (tc->get<3>())
	    {
	      ELog::EM<<"Exception Key "<<tc->get<0>()<<" "
		      <<tc->get<1>()<<ELog::endTrace;
	      ELog::EM<<"EXc =="<<EX.what()<<ELog::endTrace;
	      return -2;
	    }  
	}
    }
  
  return 0;
}

int
testInputParam::testInput()
  /*!
    Test Inputing a stream on names to the system
    \return 0 on success
  */
{
  ELog::RegMethod RegA("testInputParam","testInput");

  // Test : flag : Npts : d[1] : d[2] : d[3]
  typedef boost::tuple<std::string,bool,int,int,int,int> ITYPE;
  typedef boost::tuple<std::string,bool,int,double,double,double> DTYPE;
  typedef boost::tuple<std::string,bool,int,Geometry::Vec3D> VTYPE;
  
  std::vector<ITYPE> ITests;
  std::vector<DTYPE> DTests;
  std::vector<VTYPE> VTests;

  ITests.push_back(ITYPE("f",1,0,0,0,0));
  ITests.push_back(ITYPE("i",1,1,10,0,0));

  VTests.push_back(VTYPE("SP",1,1,Geometry::Vec3D(20,30,40)));

  inputParam A;
  A.regFlag("f","flag");
  A.regItem<int>("i","int");
  A.regItem<double>("d","dbl");
  A.regDefItem<double>("y","yobj",3,4.5);
  A.regMulti<std::string>("E","exclude",1);

  A.regItem<Geometry::Vec3D>("SP","sdefPos");
  const std::string Input[]={
    "--flag","-i","10",
    "-j","A","y",
    "-SP","20.0","30.0","40.0",
    ""};

  std::vector<std::string> Names;
  initInput(Input,Names);
  A.processMainInput(Names);
  
  std::vector<ITYPE>::const_iterator tc;
  for(tc=ITests.begin();tc!=ITests.end();tc++)
    { 
      const std::string& key=tc->get<0>();
      const int NP=tc->get<2>();
      if (A.flag(key)!=tc->get<1>() ||
	  (NP>0 && A.getValue<int>(key,0)!=tc->get<3>()) ||
	  (NP>1 && A.getValue<int>(key,1)!=tc->get<4>()) ||
	  (NP>2 && A.getValue<int>(key,2)!=tc->get<5>()) )
	{
	  ELog::EM<<"Process TEST "<<tc-ITests.begin()<<ELog::endTrace;
	  ELog::EM<<"Names.size() == "<<Names.size()<<ELog::endTrace;
	  ELog::EM<<"Item["<<key<<"] failed:"<<ELog::endTrace;
	  ELog::EM<<"NP == "<<NP<<ELog::endTrace;
	  ELog::EM<<"out == "<<tc->get<1>()<<ELog::endTrace;
	  A.write(ELog::EM.Estream());
	  ELog::EM<<ELog::endTrace;
	  return -1;
	}
    }

  std::vector<VTYPE>::const_iterator vc;
  for(vc=VTests.begin();vc!=VTests.end();vc++)
    {
      const std::string& key=vc->get<0>();
      const int NP=vc->get<2>();
      if (A.flag(key)!=vc->get<1>() ||
	  (NP>0 && A.getValue<Geometry::Vec3D>(key,0)!=vc->get<3>()))
	{
	  ELog::EM<<"Names.size() == "<<Names.size()<<ELog::endTrace;
	  ELog::EM<<"Item["<<key<<"] failed:"<<ELog::endTrace;
	  A.write(ELog::EM.Estream());
	  ELog::EM<<ELog::endTrace;
	  return -2;
	}
    }
  
  return 0;
}

int
testInputParam::testMultiExtract()
  /*!
    Test Inputing a stream on names to the system
    \return 0 on success
  */
{
  ELog::RegMethod RegA("testInputParam","testMultiExtract");

  typedef boost::tuple<std::string,size_t,size_t,size_t,std::string> TTYPE;
  std::vector<TTYPE> Tests;
  Tests.push_back(TTYPE("TC",0,0,1,"help"));
  Tests.push_back(TTYPE("TC",1,0,2,"1"));
  Tests.push_back(TTYPE("TC",2,1,3,"444"));
  Tests.push_back(TTYPE("TC",2,2,3,"23"));

  // TEST code:
  inputParam A;
  A.regMulti<std::string>("TC","tallyC",3,1);
  const std::string Input[]=
    { "-TC","help",
      "-TC","1","2",
      "-TC","3.4","444","23",
      ""
    };

  std::vector<std::string> Names;
  for(int i=0;!Input[i].empty();i++)
    Names.push_back(Input[i]);

  A.processMainInput(Names);

  int cnt(1);
  std::string Out;
  std::vector<TTYPE>::const_iterator tc;
  for(tc=Tests.begin();tc!=Tests.end();tc++)
    {
      const std::string& key=tc->get<0>();
      if (!A.flag(key)) 
	{
	  ELog::EM<<"Failed on key "<<key<<ELog::endTrace;
	  return -cnt;
	}

      if ( A.grpCnt(key)<=tc->get<1>() ||            // How many TC
	   A.itemCnt(key,tc->get<1>())<=tc->get<2>() )
	{
	  ELog::EM<<"Grp Cnt =="<<A.grpCnt(key)<<ELog::endTrace;
	  ELog::EM<<"Item Cnt=="<<A.itemCnt(key,tc->get<1>())<<
	    ":"<<tc->get<3>()<<ELog::endTrace;
	  return -cnt;
	}

      Out=A.getCompValue<std::string>(key,tc->get<1>(),tc->get<2>());
	
      
      if (A.itemCnt(key,tc->get<1>())!=tc->get<3>() ||
	  Out!=tc->get<4>())
	{
	  ELog::EM<<"Item["<<key<<"] failed:"<<Out<<":"<<ELog::endTrace;
	  ELog::EM<<"Grp = "<<tc->get<1>()<<" "<<tc->get<2>()<<ELog::endTrace;
	  ELog::EM<<"Expected ="<<tc->get<4>()<<":"<<ELog::endTrace;
	  ELog::EM<<"return flag ="<<A.itemCnt(key,tc->get<1>())<<ELog::endTrace;

	  A.write(ELog::EM.Estream());
	  ELog::EM<<ELog::endTrace;
	  return -cnt;
	}
      cnt++;
    }

  return 0;
}

int
testInputParam::testMultiTail()
  /*!
    Test Inputing a stream on names to the system
    \return 0 on success
  */
{
  ELog::RegMethod RegA("testInputParam","testMultiExtract");

  typedef boost::tuple<std::string,bool> TTYPE;
  std::vector<TTYPE> Tests;
  Tests.push_back(TTYPE("r",0));
  Tests.push_back(TTYPE("R",0));
  Tests.push_back(TTYPE("i",1));
  // TEST code:
  inputParam A;
  A.regMulti<std::string>("R","R",3,0);
  A.regMulti<std::string>("r","r",3,0);
  std::vector<std::string> RItems(10,"");
  A.regDefItemList<std::string>("i","i",10,RItems);
  const std::string Input[]=
    { "-i","",
      "" };

  std::vector<std::string> Names;
  for(int i=0;!Input[i].empty();i++)
    Names.push_back(Input[i]);

  A.processMainInput(Names);

  int cnt(1);
  std::string Out;
  std::vector<TTYPE>::const_iterator tc;
  for(tc=Tests.begin();tc!=Tests.end();tc++)
    {
      const std::string& key=tc->get<0>();
      if (A.flag(key)!=tc->get<1>()) 
	{
	  ELog::EM<<"Flag["<<key<<"] == "<<A.flag(key)
		  <<" ("<<tc->get<1>()<<")"<<ELog::endTrace;
	  return -cnt;
	}
      cnt++;
    }

  return 0;
}

int
testInputParam::testMulti()
  /*!
    Test Inputing a stream on names to the system
    \return 0 on success
  */
{
  ELog::RegMethod RegA("testInputParam","testMulti");

  // Test : flag : Index : Value
  typedef boost::tuple<std::string,size_t,std::string> TTYPE;
  
  std::vector<TTYPE> Tests;

  Tests.push_back(TTYPE("E",0,"Zoom"));
  Tests.push_back(TTYPE("E",1,"Extra"));
  Tests.push_back(TTYPE("TC",1,"4"));
  Tests.push_back(TTYPE("TC",5,"8"));
  Tests.push_back(TTYPE("TD",1,"2"));
  Tests.push_back(TTYPE("TD",4,"5"));

  inputParam A;
  A.regMulti<std::string>("E","exclude",1);
  A.regMulti<std::string>("TC","tallyC",3);
  A.regMulti<std::string>("TD","def",3,1);
  A.regMulti<std::string>("TF","singleFlag",3,2);

  const std::string Input[]={"-E","Zoom",
			     "-E","Extra",
			     "-TC","3","4","5",
			     "-TC","6","7","8",
			     "-TD","1","2",
			     "-TD","4","5","6",
			     ""};                    // Empyt string
  std::vector<std::string> Names;
  for(int i=0;!Input[i].empty();i++)
    Names.push_back(Input[i]);
  
  A.processMainInput(Names);
  int cnt(1);
  std::vector<TTYPE>::const_iterator tc;
  for(tc=Tests.begin();tc!=Tests.end();tc++)
    {
      const std::string& key=tc->get<0>();
      if (A.flag(key))
	{
	  std::string Out=A.getValue<std::string>(key,tc->get<1>());

	  if (Out!=tc->get<2>())
	    {
	      ELog::EM<<"Item["<<key<<"] failed:"<<ELog::endTrace;
	      ELog::EM<<"Expected ="<<tc->get<2>()<<ELog::endTrace;
	      A.write(ELog::EM.Estream());
	      ELog::EM<<ELog::endTrace;
	      return -cnt;
	    }
	}
      cnt++;
    }
  
  return 0;
}

int
testInputParam::testSetValue()
  /*!
    Test adding stuff to the tree.
    \return 0 on success
  */
{
  ELog::RegMethod RegA("testInputParam","testSetValue");
  
  inputParam A;
  A.regItem<double>("d","dbl");
  A.regItem<int>("i","int");
  A.regFlag("f","flag");
  A.regFlag("g","Gflag");
  
  try
    {
      A.setValue("i",10);
      A.setValue("d",10.0);
      A.setFlag("g");
      if (A.flag("f") || !A.flag("g"))
	{
	  ELog::EM<<"Failed to find non-flag :f/g"<<ELog::endTrace;
	  return -1;
	}
      A.setValue("xa",10.0);
    }
  catch (ColErr::InContainerError<std::string>& EA)
    {
      const std::string& SearchObj=EA.getItem();
      if (SearchObj!="xa")
	{
	  ELog::EM<<"Failed to find "<<SearchObj<<ELog::endTrace;
	  return -2;
	}
    }
  return 0;
}

int
testInputParam::testWriteDesc()
  /*!
    Test the writing of the description
    \return 0 on success
   */
{
  ELog::RegMethod RegA("testInputParam","testWriteDesc");

  const std::string OutString=
    StrFunc::stripMultSpc(" -d       dbl \n"
			  " -f       flag   This is the desc\n"
			  " -i       int \n");

  inputParam A;
  A.regItem<double>("d","dbl");
  A.regItem<int>("i","int");
  A.regFlag("f","flag");
  A.setDesc("f","This is the desc");  

  std::ostringstream cx;
  A.writeDescription(cx);
  std::string Out=StrFunc::stripMultSpc(cx.str());
  if (Out!=OutString)
    {
      for(size_t i=0;i<40;i++)
	if (Out[i]!=OutString[i])
	  ELog::EM<<"Cell "<<i<<" :"
		  <<static_cast<unsigned int>(Out[i])<<"=="
		  <<static_cast<unsigned int>(OutString[i])<<ELog::endTrace;
      ELog::EM<<"Failed on string :"<<
	Out.size()<<" "<<OutString.size()<<std::endl;
      ELog::EM<<Out<<ELog::endTrace;
      ELog::EM<<OutString<<ELog::endTrace;
      return -1;
    }
  
  return 0;
}

int
testInputParam::testWrite()
  /*!
    Test the writing of the description
    \return 0 on success
   */
{
  ELog::RegMethod RegA("testInputParam","testWrite");

  const std::string OutString=
    StrFunc::stripMultSpc(" -d       dbl        not-set ::  -- \n"
			  " -f       flag       not-set :: \n"
			  " -i       int        not-set ::  -- \n"
			  " -x       xobj       not-set ::  --  -- (4.5) \n"
			  " -y       yobj       set ::  10  20 (4.5) \n");
  
  inputParam A;
  A.regItem<double>("d","dbl");               // single double item
  A.regItem<int>("i","int");                  // single int item
  A.regFlag("f","flag");                      // Flag item 
  A.regDefItem<double>("x","xobj",3,4.5);
  A.regDefItem<double>("y","yobj",3,4.5);

  std::vector<std::string> Names;
  Names.push_back("-y");
  Names.push_back("10.0");
  Names.push_back("20.0");

  A.processMainInput(Names);
  std::ostringstream cx;
  A.write(cx);
  std::string Out=StrFunc::stripMultSpc(cx.str());
  if (Out!=OutString)
    {
      for(size_t i=0;i<40;i++)
	if (Out[i]!=OutString[i])
	  ELog::EM<<"Cell "<<i<<" :"
		  <<static_cast<unsigned int>(Out[i])<<"=="
		  <<static_cast<unsigned int>(OutString[i])<<ELog::endTrace;
      ELog::EM<<"Failed on string :"<<
	Out.size()<<" "<<OutString.size()<<std::endl;
      ELog::EM<<Out<<ELog::endTrace;
      ELog::EM<<OutString<<ELog::endTrace;
      return -1;
    }
  
  return 0;
}
