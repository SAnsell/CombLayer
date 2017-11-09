/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   test/testInputParam.cxx
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
#include <complex>
#include <vector>
#include <set>
#include <map>
#include <stack>
#include <string>
#include <sstream>
#include <algorithm>
#include <tuple>

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
      &testInputParam::testCntVec3D,
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
      "CntVec3D",
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
testInputParam::testCntVec3D()
  /*!
    Test the default regisitration
    \return 0 on success
  */
{
  ELog::RegMethod RegA("testInputParam","testCntVec3D");
  
  inputParam A;
  A.regMulti("d","dbl");

  std::vector<std::string> Names=
    { "-k", "-d", "Vec3D(3,4,5)", "-d", "6","-3","5","8",
      "-d","5" "Vec3D(1,2,3)"};

  typedef std::tuple<size_t,size_t,Geometry::Vec3D,size_t> TTYPE;
  std::vector<TTYPE> Tests =
    {
      TTYPE(0,0,Geometry::Vec3D(3,4,5),1),
      TTYPE(1,1,Geometry::Vec3D(-3,5,8),4)


    } ;

  
  A.processMainInput(Names);

  for(const TTYPE& tc : Tests)
    {
      const size_t part=std::get<0>(tc);
      size_t index=std::get<1>(tc);
      Geometry::Vec3D OutA=A.getCntVec3D("d",part,index);
      if (std::get<2>(tc)!=OutA ||
	  index!=std::get<3>(tc))
	{
	  ELog::EM<<"Out =="<<OutA<<ELog::endDiag;
	  ELog::EM<<"Cnt =="<<index<<ELog::endDiag;
	  return -1;
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
  A.regItem("d","dbl");

  // Test partial [Success]
  A.regDefItem<int>("i","int",3,-8);
  // Test partial [Success]
  A.regDefItem<int>("j","jint",3,-8,-9);
  A.regDefItem<std::string>("X","xmlout",1,"Model.xml");

  std::vector<std::string> Names=
    { "-k", "-i", "10", "7", "-X", "test.xml" };
  
  A.processMainInput(Names);

  if (!A.flag("i") ||
      A.getValue<int>("i",0)!=10 ||
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
  A.regItem("d","dbl",1,10000);
  A.regItem("i","int",1,1000);
  
  try
    {
      A.setValue<int>("i",10);
      A.setValue<double>("d",20.0);
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
  A.regItem("x","xlong");
  A.regMulti("m","mlong",2,0,1000);
  A.setValue<double>("x",30.0);
  
  // key : varKey : index : Except flag : result
  typedef std::tuple<std::string,std::string,
		       size_t,bool,double> TTYPE;
  std::vector<TTYPE> Tests;

  Tests.push_back(TTYPE("x","testX",0,1,30.0));
  Tests.push_back(TTYPE("y","testX",0,1,20.30));
  Tests.push_back(TTYPE("x","nothing",0,1,30.0));
  Tests.push_back(TTYPE("y","nothing",0,0,0.0));
  
  for(const TTYPE& tc : Tests)
    {
      try
	{
	  const double D=
	    A.getFlagDef<double>(std::get<0>(tc),Control,
				 std::get<1>(tc),std::get<2>(tc));
	  if (!std::get<3>(tc) || fabs(D-std::get<4>(tc))>1e-5)
	    {
	      ELog::EM<<"Key "<<std::get<0>(tc)<<" "
		      <<std::get<1>(tc)<<ELog::endTrace;
	      ELog::EM<<"D "<<D<<ELog::endTrace;
	      return -11;
	    }  
        }
      catch (ColErr::InContainerError<std::string>& EX)
	{
	  if (std::get<3>(tc))
	    {
	      ELog::EM<<"Exception Key "<<std::get<0>(tc)<<" "
		      <<std::get<1>(tc)<<ELog::endTrace;
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
  typedef std::tuple<std::string,bool,int,int,int,int> ITYPE;
  typedef std::tuple<std::string,bool,int,double,double,double> DTYPE;
  typedef std::tuple<std::string,bool,int,Geometry::Vec3D> VTYPE;
  
  std::vector<ITYPE> ITests;
  std::vector<DTYPE> DTests;
  std::vector<VTYPE> VTests;

  ITests.push_back(ITYPE("f",1,0,0,0,0));
  ITests.push_back(ITYPE("i",1,1,10,0,0));

  VTests.push_back(VTYPE("SP",1,1,Geometry::Vec3D(20,30,40)));

  inputParam A;
  A.regFlag("f","flag");
  A.regItem("i","int");
  A.regItem("d","dbl");
  A.regDefItem<double>("y","yobj",3,4.5);
  A.regMulti("E","exclude",1);

  A.regItem("SP","sdefPos");
  std::vector<std::string> Names={
    "--flag","-i","10",
    "-j","A","y",
    "-SP","20.0","30.0","40.0"
    };

  A.processMainInput(Names);
  

  int cnt(1);
  for(const ITYPE& tc : ITests)
    { 
      const std::string& key=std::get<0>(tc);
      const int NP=std::get<2>(tc);
      if (A.flag(key)!=std::get<1>(tc) ||
	  (NP>0 && A.getValue<int>(key,0)!=std::get<3>(tc)) ||
	  (NP>1 && A.getValue<int>(key,1)!=std::get<4>(tc)) ||
	  (NP>2 && A.getValue<int>(key,2)!=std::get<5>(tc)) )
	{
	  ELog::EM<<"Process TEST "<<cnt<<ELog::endTrace;
	  ELog::EM<<"Names.size() == "<<Names.size()<<ELog::endTrace;
	  ELog::EM<<"Item["<<key<<"] failed:"<<ELog::endTrace;
	  ELog::EM<<"NP == "<<NP<<ELog::endTrace;
	  ELog::EM<<"out == "<<std::get<1>(tc)<<ELog::endTrace;
	  A.write(ELog::EM.Estream());
	  ELog::EM<<ELog::endTrace;
	  return -1;
	}
      cnt++;
    }

  for(const VTYPE& vc : VTests)
    {
      const std::string& key=std::get<0>(vc);
      const int NP=std::get<2>(vc);
      if (A.flag(key)!=std::get<1>(vc) ||
	  (NP>0 && A.getValue<Geometry::Vec3D>(key,0)!=std::get<3>(vc)))
	{
	  ELog::EM<<"Names.size() == "<<Names.size()<<ELog::endTrace;
	  ELog::EM<<"Flag["<<std::get<1>(vc)<<"] =="
		  <<A.flag(key)<<ELog::endTrace;
	  if (NP>0)
	    ELog::EM<<"Vec3D["<<std::get<3>(vc)<<"] =="
		    <<A.getValue<Geometry::Vec3D>(key,0)<<ELog::endTrace;
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

  typedef std::tuple<std::string,size_t,size_t,size_t,std::string> TTYPE;
  std::vector<TTYPE> Tests;
  Tests.push_back(TTYPE("TC",0,0,1,"help"));
  Tests.push_back(TTYPE("TC",1,0,2,"1"));
  Tests.push_back(TTYPE("TC",2,1,3,"444"));
  Tests.push_back(TTYPE("TC",2,2,3,"23"));

  // TEST code:
  inputParam A;
  A.regMulti("TC","tallyC",3,1);

  std::vector<std::string> Names={
    "-TC","help",
    "-TC","1","2",
    "-TC","3.4","444","23" };

  A.processMainInput(Names);

  int cnt(1);
  std::string Out;
  for(const TTYPE& tc : Tests)
    {
      const std::string& key=std::get<0>(tc);
      if (!A.flag(key)) 
	{
	  ELog::EM<<"Failed on key "<<key<<ELog::endTrace;
	  return -cnt;
	}

      if ( A.setCnt(key)<=std::get<1>(tc) ||            // How many TC
	   A.itemCnt(key,std::get<1>(tc))<=std::get<2>(tc) )
	{
	  ELog::EM<<"Grp Cnt =="<<A.setCnt(key)<<ELog::endTrace;
	  ELog::EM<<"Item Cnt=="<<A.itemCnt(key,std::get<1>(tc))<<
	    ":"<<std::get<3>(tc)<<ELog::endTrace;
	  return -cnt;
	}

      Out=A.getValue<std::string>(key,std::get<1>(tc),std::get<2>(tc));
      
      
      if (A.itemCnt(key,std::get<1>(tc))!=std::get<3>(tc) ||
	  Out!=std::get<4>(tc))
	{
	  ELog::EM<<"Item["<<key<<"] failed:"<<Out<<":"<<ELog::endTrace;
	  ELog::EM<<"Grp = "<<std::get<1>(tc)<<" "<<std::get<2>(tc)<<ELog::endTrace;
	  ELog::EM<<"Expected ="<<std::get<4>(tc)<<":"<<ELog::endTrace;
	  ELog::EM<<"return flag ="<<A.itemCnt(key,std::get<1>(tc))
		  <<ELog::endTrace;

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

  typedef std::tuple<std::string,bool> TTYPE;
  std::vector<TTYPE> Tests;
  Tests.push_back(TTYPE("r",0));
  Tests.push_back(TTYPE("R",0));
  Tests.push_back(TTYPE("i",1));
  // TEST code:
  inputParam A;
  A.regMulti("R","R",3,0);
  A.regMulti("r","r",3,0);
  std::vector<std::string> RItems(10,"");
  A.regDefItemList<std::string>("i","i",10,RItems);
  std::vector<std::string> Names=
    { "-i","","" };
  A.processMainInput(Names);

  int cnt(1);
  std::string Out;
  for(const TTYPE& tc : Tests)
    {
      const std::string& key=std::get<0>(tc);
      if (A.flag(key)!=std::get<1>(tc)) 
	{
	  ELog::EM<<"Flag["<<key<<"] == "<<A.flag(key)
		  <<" ("<<std::get<1>(tc)<<")"<<ELog::endTrace;
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
  typedef std::tuple<std::string,size_t,size_t,std::string> TTYPE;
  
  std::vector<TTYPE> Tests;

  Tests.push_back(TTYPE("E",0,0,"Zoom"));
  Tests.push_back(TTYPE("E",1,0,"Extra"));
  Tests.push_back(TTYPE("TC",0,1,"4"));
  Tests.push_back(TTYPE("TC",1,2,"8"));
  Tests.push_back(TTYPE("TD",0,1,"2"));
  Tests.push_back(TTYPE("TD",1,1,"5"));

  inputParam A; 
  A.regMulti("E","exclude",100,1);
  A.regMulti("TC","tallyC",100,3);
  A.regMulti("TD","def",100,1,3);
  A.regMulti("TF","singleFlag",100,2,3);

  std::vector<std::string> Names={"-E","Zoom",
				  "-E","Extra",
				  "-TC","3","4","5",
				  "-TC","6","7","8",
				  "-TD","1","2",
				  "-TD","4","5","6"};

  A.processMainInput(Names);
  int cnt(1);
  int exceptionFlag=0;
  for(const TTYPE& tc : Tests)
    {
      const std::string& key=std::get<0>(tc);
      if (A.flag(key))
	{
	  const size_t setIndex=std::get<1>(tc);
	  const size_t itemIndex=std::get<2>(tc);
	  std::string Out;
	  try
	    {
	      Out=A.getValue<std::string>(key,setIndex,itemIndex);
	    }
	  catch(ColErr::ExBase& A)
	    {
	      exceptionFlag=1;
	      ELog::EM<<"Exception == "<<A.what()<<ELog::endDiag;
	    }
	  if (exceptionFlag || (Out!=std::get<3>(tc)))
	    {
	      ELog::EM<<"Test == "<<cnt<<ELog::endDiag;
	      ELog::EM<<"Item["<<key<<"] failed:"<<ELog::endTrace;
	      ELog::EM<<"Expected ="<<std::get<3>(tc)<<ELog::endTrace;
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
  A.regItem("d","dbl");
  A.regItem("i","int");
  A.regFlag("f","flag");
  A.regFlag("g","Gflag");
  
  try
    {
      A.setValue<int>("i",10);
      A.setValue<double>("d",10.0);
      A.setFlag("g");
      if (A.flag("f") || !A.flag("g"))
	{
	  ELog::EM<<"Failed to find non-flag :f/g"<<ELog::endTrace;
	  return -1;
	}
      A.setValue<double>("xa",10.0);
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
  A.regItem("d","dbl");
  A.regItem("i","int");
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
    StrFunc::stripMultSpc(" -d       dbl        not-set :: \n"
			  " -f       flag       not-set :: \n"
			  " -i       int        not-set :: \n"
			  " -x       xobj       not-set ::  4.5 4.5 4.5\n"
			  " -y       yobj       set ::  10.0 20.0 4.5\n");
  
  inputParam A;
  A.regItem("d","dbl");               // single double item
  A.regItem("i","int");                  // single int item
  A.regFlag("f","flag");                      // Flag item 
  A.regDefItem<double>("x","xobj",3,4.5);
  A.regDefItem<double>("y","yobj",3,4.5);

  std::vector<std::string> Names=
    { "-y","10.0","20.0" };

  A.processMainInput(Names);
  std::ostringstream cx;
  A.write(cx);
  std::string Out=StrFunc::stripMultSpc(cx.str());
  if (Out!=OutString)
    {
      for(size_t i=0;i<125;i++)
	if (Out[i]!=OutString[i])
	  {
	    ELog::EM<<"Cell "<<i<<" :"
		    <<static_cast<unsigned int>(Out[i])<<"=="
		    <<static_cast<unsigned int>(OutString[i])<<" ||| "
		  <<(Out[i])<<"=="
		    <<(OutString[i])<<ELog::endTrace;
	  }
      
      ELog::EM<<"Failed on string :"<<
	Out.size()<<" "<<OutString.size()<<std::endl;
      ELog::EM<<Out<<ELog::endTrace;
      ELog::EM<<OutString<<ELog::endTrace;
      return -1;
    }
  
  return 0;
}
