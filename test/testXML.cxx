/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   test/testXML.cxx
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
#include <iostream>
#include <iomanip>
#include <cmath>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <stack>
#include <set>
#include <map>
#include <iterator>
#include <tuple>
#include <boost/multi_array.hpp>


#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "XMLload.h"
#include "XMLattribute.h"
#include "XMLobject.h"
#include "XMLvector.h"
#include "XMLgrid.h"
#include "XMLgroup.h"
#include "XMLcomp.h"
#include "XMLread.h"
#include "XMLdatablock.h"
#include "XMLcollect.h"
#include "XMLnamespace.h" 
#include "XMLiterator.h"
#include "XMLgridSupport.h"

#include "testFunc.h"
#include "testUnitSupport.h"
#include "testXML.h"



using namespace XML;

testXML::testXML() 
  /*!
    Constructor
  */
{}

testXML::~testXML() 
  /*!
    Destructor
  */
{}

void
testXML::populateString(std::string& CX)
  /*!
    Fills a string with a valid 
    XML schema for testing
    \param CX :: String to write to
  */
{
  CX+="<metadata_entry>\n";
  CX+="<test>\n";
  CX+="<Beta1/>\n";
  CX+="<Beta2/>\n";
  CX+="<Beta3 units=\"uSec\">\n";
  CX+="<Alpha1>50</Alpha1>\n";
  CX+="<Alpha2>11</Alpha2>\n";
  CX+="<Alpha3>21</Alpha3>\n";
  CX+="<Gamma units=\"mSec^2\">16</Gamma>\n";
  CX+="<Gamma>21</Gamma>\n";
  CX+="<Gamma>31</Gamma>\n";
  CX+="<Item units=\"mSec\">15</Item>\n";
  CX+="</Beta3>\n";
  CX+="</test>\n";
  CX+="</metadata_entry>\n";
  return;
}

void
testXML::populateString2(std::string& CX)
  /*!
    Fills a string with a valid 
    XML schema for testing. A more complex example 
    \param CX :: Output string
  */
{
  CX+="<metadata_entry>\n";
  CX+="<test>\n";
  CX+="<Beta1/>\n";
  CX+="<Beta2/>\n";
  CX+="<Beta3 units=\"uSec\">\n";
  CX+="<InerGrp item=\"3.4\">\n";
  CX+="<Alpha1>50</Alpha1>\n";
  CX+="<Alpha2>11</Alpha2>\n";
  CX+="<Alpha3>21</Alpha3>\n";
  CX+="</InerGrp>\n";
  CX+="<Gamma units=\"mSec^2\">16</Gamma>\n";
  CX+="<Gamma>21</Gamma>\n";
  CX+="<Gamma>31</Gamma>\n";
  CX+="<Item units=\"mSec\">15</Item>\n";
  CX+="</Beta3>\n";
  CX+="</test>\n";
  CX+="</metadata_entry>\n";
  return;
}

void
testXML::populateString3(std::string& CX)
  /*!
    Fills a string with a valid 
    XML schema for testing. A more complex example 
    \param CX :: Output string
  */
{
  CX+="<metadata_entry>\n";
  CX+="<Top>";
  CX+="<A>54</A>";
  CX+="<Mid>";
  CX+="<B>51</B>";
  CX+="<Lower>";
  CX+="<C>49</C>";
  CX+="</Lower>";
  CX+="</Mid>";
  CX+="</Top>";
  CX+="</metadata_entry>\n";
  return;
}

const std::string&
testXML::buildXMLstring() const
  /*!
    Provid the basic build string from buildXML
    \return full string
  */
{
  static std::string XMLout=
    "<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?>\n"
    "<metadata_entry>\n"
    "  <test>\n"
    "    <Beta1/>\n"
    "    <Beta2/>\n"
    "    <Beta3 units=\"uSec\">\n"
    "      <Alpha1>10</Alpha1>\n"
    "      <Alpha2>20</Alpha2>\n"
    "      <Alpha3>20</Alpha3>\n"
    "      <Gamma units=\"uSec^2\">10</Gamma>\n"
    "      <Gamma>20</Gamma>\n"
    "      <Gamma>30</Gamma>\n"
    "      <Item units=\"uSec\">10</Item>\n"
    "    </Beta3>\n"
    "  </test>\n"
    "</metadata_entry>\n";
  return XMLout;
}

void
testXML::buildXML()
  /*!
    Simple function to populate
    the master system
  */
{
  ELog::RegMethod RegA("testXML","buildXML");
  
  XOut.clear();
  XOut.addGrp("test");
  XOut.addNumGrp("Beta");
  XOut.closeGrp();
  XOut.addNumGrp("Beta");
  XOut.closeGrp();
  XOut.addNumGrp("Beta");
  XOut.addNumComp("Alpha",10);
  XOut.addNumComp("Alpha",20);
  XOut.addNumComp("Alpha",20);
  XOut.addComp("Gamma",10);
  XOut.addComp("Gamma",20);
  XOut.addComp("Gamma",30);
  XOut.addAttribute("Gamma","units",std::string("uSec^2"));
  XOut.addAttribute("units",std::string("uSec"));
  XOut.addComp("Item",10);
  XOut.addAttribute("Item","units",std::string("uSec"));
  XOut.closeGrp();
  return;
}

int 
testXML::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: Index number of test to try
      - [-1 :: all]
      - [0 :: display list]
    \returns -ve on error 0 on success.
  */
{
  ELog::RegMethod RegA("testXML","applyTest");
  TestFunc::regSector("testXML");

  // Stuff to be done first:
  typedef int (testXML::*testPtr)();
  testPtr TPtr[]=
    { 
      &testXML::testNumber,	    
      &testXML::testGetObj,	    
      &testXML::testBinSearch,	    
      &testXML::testSplitGroup,	    
      &testXML::testReadObject,	    
      &testXML::testParent,	    
      &testXML::testFileName,	    
      &testXML::testCutString,	    
      &testXML::testNextGrp,	    
      &testXML::testLoadSystem,	    
      &testXML::testPartLoad,	    
      &testXML::testLoadInclude,    
      &testXML::testCombineGrid,    
      &testXML::testCombineDeepGrid,
      &testXML::testDeleteObj,	    
      &testXML::testDataBlock,	    
      &testXML::testGroupContent,   
      &testXML::testXMLiterator,     
      &testXML::testProcString    
    };

  std::string TestName[] = 
    {
      "testNumber",
      "testGetObj",
      "testBinSearch",
      "testSplitGroup",
      "testReadObject",
      "testParent",
      "testFileName",
      "testCutString",
      "testNextGrp",    
      "testLoadSystem",
      "testPartLoad",
      "testLoadInclude",
      "testCombineGrid",
      "testCombineDeepGrid",
      "testDeleteObj",
      "testDataBlock",
      "testGroupContent",
      "testXMLiterator",
      "testProcString"
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
testXML::testNumber()
  /*!
    Tests numbered groups etc
    -- This test should show numbered objects e.g. alpha3  
    -- and no missing gaps starting from 1"<<std::endl;
    \retval 0 :: Success
  */
{
  ELog::RegMethod RegA("testXML","testNumber");
  buildXML();
  std::ostringstream cx;
  XOut.writeXML(cx);
  if (StrFunc::stripMultSpc(cx.str())
      !=StrFunc::stripMultSpc(buildXMLstring()))
    {
      ELog::EM<<cx.str()<<ELog::endDiag;
      ELog::EM<<"--------"<<ELog::endDiag;
      ELog::EM<<buildXMLstring()<<ELog::endDiag;
      return -1;
    }
  return 0;
}

int 
testXML::testGetObj()
  /*!
    This test the XML at depth.
    The object is to get components
    deliminated by : at depth.
    \retval -1 :: findObj failed
    \retval -2 :: getObj failed
    \retval -3 :: findObj Iteration count failed
   */
{
  buildXML();
  XMLobject* AR=XOut.findObj("Beta3");
  XMLgroup* GPtr=dynamic_cast<XMLgroup*>(AR);
  if (!GPtr)  return -1;  // both AR and GPtr must have worked


  AR=XOut.findObj("test/Beta3/Alpha3");
  if (!AR)
    return -3;

  XMLobject* AD=XOut.getObj("test/Beta3/Alpha3");
  if (!AD && AD!=AR)
    return -4;

  // Test of the numbering system
  XMLobject* GA=XOut.findObj("Gamma",0);
  XMLobject* GB=XOut.findObj("Gamma",2);
  if (!GA || !GB || GA==GB)
    return -5;

  return 0;
}

int
testXML::testNextGrp()
  /*!
    Test the XMLnamespace getNextGroup
    \retval -1 :: Failed
  */
{
  ELog::RegMethod RegA("testXML","testNextGrp");

  // key : Number of components : Composite
  typedef std::tuple<int,std::string,unsigned int,std::string> TTYPE;
  std::vector<TTYPE> Tests;
  Tests.push_back(TTYPE(1,"Out",0,""));
  Tests.push_back(TTYPE(1,"test",1,"f=\"54\"::"));
  Tests.push_back(TTYPE(2,"test",0,""));
  Tests.push_back(TTYPE(1,"testA",1,"f=\"44\"::"));
  Tests.push_back(TTYPE(2,"testA",0,""));
  Tests.push_back(TTYPE(-1,"testB",0,""));
  Tests.push_back(TTYPE(-1,"testC",2,"a=\"10\"::b=\"39\"::"));
  Tests.push_back(TTYPE(2,"Out",0,""));


  std::ofstream OX;
  OX.open("testXML.xml");

  OX<<"<Out>\n";
  OX<<"<test f=\"54\"> Some text </test>";
  OX<<"<testA f=\"44\"> Some more text </testA>\n";
  OX<<"<testB/>\n";
  OX<<"<testC a=\"10\" b=\"39\"/>\n";
  OX<<"</Out>"<<std::endl;
  OX.close();
  
  XML::XMLload IFile("testXML.xml");
  std::string Key;
  std::vector<std::string> Att;

  int cnt(1);
  for(const TTYPE& tc : Tests)
    {
      const int flag=XML::getNextGroup(IFile,Key,Att);
      std::ostringstream cx;
      copy(Att.begin(),Att.end(),
	   std::ostream_iterator<std::string>(cx,"::"));
      if (flag!=std::get<0>(tc) || Key!=std::get<1>(tc) || 
	  Att.size()!=std::get<2>(tc) || 
	  (!Att.empty() && cx.str()!=std::get<3>(tc)) )
	{
	  ELog::EM<<"Test ="<<cnt<<ELog::endWarn;
	  ELog::EM<<"Flag["<<std::get<0>(tc)<<"] = "<<flag<<ELog::endDiag;
	  ELog::EM<<"Failed on["<<std::get<1>(tc)<<"]= "
		  <<Key<<" ="<<ELog::endDiag;
	  ELog::EM<<"Attn Size["<<std::get<2>(tc)<<"] ="
		  <<Att.size()<<ELog::endDiag;
	  ELog::EM<<"Attn     ="<<cx.str()<<" ="<<ELog::endDiag;
	  ELog::EM<<"Exp Attn ="<<std::get<3>(tc)<<" ="<<ELog::endDiag;

	  return -cnt;
	}
      cnt++;
    }

  return 0;
}

int
testXML::testGroupContent()
  /*!
    Test the XMLnamespace getGroupContent.
    These test only weakly test the attribute/Data
    return of GroupContent.
    \retval -1 :: Failed
    \retval 0 :: success
  */
{
  ELog::RegMethod RegA("testXML","testGroupContent");

  std::ofstream cx("testXML.xml");
  std::string Key;
  std::vector<std::string> Att;

  cx<<"<Out>"<<std::endl;
  cx<<"<test f=\"54\"> Some text </test>";
  cx<<"<testA f=\"44\"> Some more text </testA>";
  cx<<"<testB/>";
  cx<<"<testC>More Data</testC>"<<std::endl;
  cx<<"<testD>Xtra Data</testD>";
  cx<<"</Out>"<<std::endl;
  cx.close();
  
  typedef std::tuple<int,std::string,std::string,std::string> RTYPE;
  std::vector<RTYPE> ResA;
  ResA.push_back(RTYPE(1,"Out","",""));
  ResA.push_back(RTYPE(1,"test","f=\"54\"::",""));
  ResA.push_back(RTYPE(2,"test","","Some text"));
  ResA.push_back(RTYPE(1,"testA","f=\"44\"::",""));
  ResA.push_back(RTYPE(2,"testA","","Some more text"));
  ResA.push_back(RTYPE(-1,"testB","",""));
  ResA.push_back(RTYPE(1,"testC","",""));
  ResA.push_back(RTYPE(2,"testC","","More Data"));
  ResA.push_back(RTYPE(1,"testD","",""));
  ResA.push_back(RTYPE(2,"testD","","Xtra Data"));
  ResA.push_back(RTYPE(2,"Out","",""));
  ResA.push_back(RTYPE(0,"Out","",""));


  XML::XMLload Lfile("testXML.xml");

  int cnt(1);
  for(const RTYPE& tc : ResA)
    {
      std::ostringstream cx;
      std::vector<std::string> Att;
      std::string Data;
      const int flag=XML::getGroupContent(Lfile,Key,Att,Data);
      copy(Att.begin(),Att.end(),
	   std::ostream_iterator<std::string>(cx,"::"));
      
      if (std::get<0>(tc)!=flag || std::get<1>(tc)!=Key ||
	  std::get<2>(tc)!=cx.str() || std::get<3>(tc)!=Data)
	{
	  ELog::EM<<"Failed Flag= "<<flag<<" key="<<Key<<ELog::endDiag;
	  ELog::EM<<"Att=    "<<cx.str()<<ELog::endDiag;
	  ELog::EM<<"Expect= "<<std::get<2>(tc)<<ELog::endDiag;
	  ELog::EM<<"Data         :"<<Data<<":"<<ELog::endDiag;
	  ELog::EM<<"Expected Data:"<<std::get<3>(tc)<<":"<<ELog::endDiag;
	  return -cnt;
	}
      cnt++;
    }
  return 0;
}

int
testXML::testBinSearch()
  /*!
    Test the binary number search. Adds a set of 
    objects to the string/int map. then XML::getNumber finds
    the next versionof the index availiable. It uses a binary search
    and to find the object. This is provided by the XML::getNumberIndex funcion.
    - Positive test to get count of A
    - Negative test to get count of B
    \retval 0 :: Success
    \retval -1 :: Failed on in-map find
    \retval -2 :: Failed on out-map find
  */
{
  ELog::RegMethod RegA("testXML","testBinSearch");
  typedef std::multimap<std::string,int> MapX;
  MapX MX;
  MX.insert(MapX::value_type("A_1",1));
  MX.insert(MapX::value_type("A_2",2));
  MX.insert(MapX::value_type("A_3",3));
  MX.insert(MapX::value_type("A_4",4));
  MX.insert(MapX::value_type("A_5",5));
  long int out = XML::getNumberIndex<std::string,int>(MX,"A_");

  if (out!=6)
    return -1;
  // Test of start
  out = XML::getNumberIndex(MX,std::string("B_"));
  if (out!=1)
    return -2;
  
  return 0;
  
}

int
testXML::testReadObject()
  /*!
    Tests the reading of an object
    \retval 0 :: success
    \retval -ve :: error with XMLcollect::readObject
  */
{
  ELog::RegMethod RegA("testXML","testReadObject");

  buildXML();
  std::string Part;
  populateString(Part);
  std::istringstream cx(Part);
  return 0;
//  return XOut.readObject(cx,"");
}

int
testXML::testSplitGroup()
  /*!
    Test of the regex-system and 
    expression to determine a stream into an outer group
    \retval 0 :: success
   */
{
  ELog::RegMethod RegA("testXML","testSplitGroup");
  
  std::string Part;
  std::string Key;
  std::vector<std::string> Attrib;
  std::string Body;
  populateString(Part);   // Part is now <....> etc
  std::ofstream cx("testXML.xml");
  cx<<Part<<std::endl;
  cx.close();
  XML::XMLload IFile("testXML.xml");
  if (!XML::splitGroup(IFile,Key,Attrib,Body) ||
      Key!="metadata_entry" ||
      !Attrib.empty() || Body.size()<50 )
    {
      ELog::EM<<"Key == "<<Key<<" == "<<ELog::endDiag;
      ELog::EM<<"Attrib == "<<Attrib.size()<<ELog::endDiag;
      ELog::EM<<"Body(size) == "<<Body.size()<<ELog::endDiag;
      return -1;
    }
  return 0;
}

int
testXML::testParent()
  /*!
    Detemine the parent of a given object
    \retval -1 :: failure on gamma 1
    \retval 0 :: success
  */
{
  ELog::RegMethod RegA("testXML","testParent");

  buildXML();
  XML::XMLobject* AR=XOut.findObj("Gamma",2);
  if (!AR)
    return -1;

  XML::XMLobject* Parent=AR->getParent();
  if (!Parent)
    return -2;
  if (Parent->getKey()!="Beta3")
    {
      ELog::EM<<"Parent == "<<Parent->getKey()<<ELog::endTrace;
      return -3;
    }
  
  return 0;
}

int
testXML::testFileName()
  /*!
    Test the production of a filename from a component
    \return 0 :: success / -ve on error
  */
{
  ELog::RegMethod RegA("testXML","testFileName");

  buildXML();
  XML::XMLobject* AR=XOut.findObj("Gamma",2);
  if (!AR)
    return -1;
  std::string FName = AR->getCurrentFile(2);
  if (FName!="Beta3_Gamma_r2")
    {
      ELog::EM<<"File == "<<FName<<" =="<<ELog::endTrace;
      return -2;
    }
  return 0;
}


int
testXML::testCutString()
  /*!
    Test of the XMLnamespace CutString
    \return 0 :: success / -ve on failure
  */
{
  ELog::RegMethod RegA("testXML","testCutString");

  std::string AList = " file = \"test\" ref =\"Time\"";
  std::string Key;
  std::string Part;
  int flag=XML::splitAttribute(AList,Key,Part);
  if (flag!=1 && Key!="file" && Part!="test")
    {
      ELog::EM<<"Flag == "<<flag<<ELog::endTrace;
      ELog::EM<<"Key == "<<Key<<ELog::endTrace;
      ELog::EM<<"Part == "<<Part<<ELog::endTrace;
      return -1;
    }
  return 0;
}

int
testXML::testVectorWrite()
  /*!
    Test the writing of a vector component
    \retval 0 :: success
   */
{
  ELog::RegMethod RegA("testXML","testVectorWrite");
  buildXML();
  return 0;
}

int
testXML::testLoadSystem()
  /*!
    Writes to a file and tests the loading in of the XML format
    [Incomplete]
    \retval 0 :: success
   */
{
  ELog::RegMethod RegA("testXML","testLoadSystem");
  
  std::string In;
  populateString(In);
  std::ofstream TX("testXML.xml");
  TX<<In;
  TX.close();
  
  XMLcollect CO;
  CO.loadXML("testXML.xml");
  std::ostringstream cx; 
  CO.writeXML(cx);
  In="<?xmlversion=\"1.0\"encoding=\"ISO-8859-1\"?>"+In;
  if (StrFunc::removeSpace(cx.str())!=
      StrFunc::removeSpace(In))
    {
      ELog::EM<<"String == "<<StrFunc::removeSpace(In)<<ELog::endWarn;
      ELog::EM<<"Out == "<<StrFunc::removeSpace(cx.str())<<ELog::endWarn;
      return -1;
    }
  return 0;
}


int
testXML::testXMLiterator()
  /*!
    Test the XMLiterator object
    \retval 0 :: success
   */
{
  ELog::RegMethod RegA("testXML","testXMLiterator");

  std::string In;
  populateString2(In);
  std::ofstream TX("testXML.xml");
  TX<<In;
  TX.close();
  XMLcollect CO;
  CO.loadXML("testXML.xml");
  
  typedef std::tuple<int,std::string> RTYPE;
  std::vector<RTYPE> ResA;
  ResA.push_back(RTYPE(0,"metadata_entry"));
  ResA.push_back(RTYPE(1,"test"));
  ResA.push_back(RTYPE(2,"Beta1"));
  ResA.push_back(RTYPE(2,"Beta2"));
  ResA.push_back(RTYPE(2,"Beta3"));      // open group
  ResA.push_back(RTYPE(3,"InerGrp"));
  ResA.push_back(RTYPE(4,"Alpha1"));
  ResA.push_back(RTYPE(4,"Alpha2"));
  ResA.push_back(RTYPE(4,"Alpha3"));
  ResA.push_back(RTYPE(3,"Gamma"));  
  ResA.push_back(RTYPE(3,"Gamma"));
  ResA.push_back(RTYPE(3,"Gamma"));
  ResA.push_back(RTYPE(3,"Item"));

  std::vector<RTYPE>::const_iterator tc=ResA.begin();
  XMLiterator SK(CO.getCurrent());
  do 
    {
      if (*SK)
	{
	  if (std::get<1>(*tc)!=SK->getKey() || 
	      std::get<0>(*tc)!=SK.getLevel())
	    {
	      ELog::EM<<"Object key == "<<SK->getKey()
		      <<" at "<<SK.getLevel()<<ELog::endTrace;
	      return -1;
	    }
	  tc++;
	}
    } while (SK++);
  if (SK.getLevel()!=0)
    {
      ELog::EM<<"Failed on exit "<<SK.getLevel()<<ELog::endTrace;
      return -2;
    }  
  
  // TEST of negation
  SK.init();
  int cnt(0);
  do 
    {
      cnt++;
    } while (SK++ && cnt<8);

  
  std::vector<RTYPE>::const_reverse_iterator rc=ResA.rend();
  rc-=9;
  do 
    {
      if (*SK)
	{
	  if (std::get<1>(*rc)!=SK->getKey() || 
	      std::get<0>(*rc)!=SK.getLevel())
	    {
	      ELog::EM<<"Reverse Object key == "<<SK->getKey()
		      <<" at "<<SK.getLevel()<<ELog::endTrace;
	      ELog::EM<<"Expect key == "<<std::get<1>(*rc)<<" at "
		      <<std::get<0>(*rc)<<ELog::endTrace;
	      return -3;
	    }
	  rc++;
	}
    } while (SK--);

  return 0;
}

int
testXML::testPartLoad()
  /*!
    Objective is to load a single part.
    \retval 0 :: success
    \retval -1 :: failure
   */
{
  ELog::RegMethod RegA("testXML","testPartLoad");

  // Write file ::
  std::string In;
  populateString(In);
  std::ofstream TX("test.xml");
  TX<<In;
  TX.close();

  std::ostringstream cx;
  XMLcollect CO;

  std::string Out="<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?>\n"
    "<metadata_entry>"
    "<Beta3 units=\"uSec\">"
    "<Alpha1>50</Alpha1>"
    "<Alpha2>11</Alpha2>"
    "<Alpha3>21</Alpha3>"
    "<Gamma units=\"mSec^2\">16</Gamma>"
    "<Gamma>21</Gamma>"
    "<Gamma>31</Gamma>"
    "<Item units=\"mSec\">15</Item>"
    "</Beta3>"
    "</metadata_entry>" ;

  int flag=CO.loadXML("test.xml","Beta3");
  CO.writeXML(cx);
  if (flag || StrFunc::removeSpace(cx.str())!=StrFunc::removeSpace(Out))
    {
      ELog::EM<<"Flag == "<<flag<<ELog::endDiag;
      ELog::EM<<"Out == "<<cx.str()<<ELog::endDiag;
      return -1;
    }

  CO.clear();

  Out="<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?>"
    "<metadata_entry>"
    "<Gamma units=\"mSec^2\">16</Gamma>"
    "</metadata_entry>";

  flag=CO.loadXML("test.xml","Gamma");
  cx.str("");
  CO.writeXML(cx);
  if (flag || StrFunc::removeSpace(cx.str())!=StrFunc::removeSpace(Out))
    {
      ELog::EM<<"Flag == "<<flag<<ELog::endDiag;
      ELog::EM<<"Out == "<<cx.str()<<ELog::endDiag;
      return -2;
    }
  return 0;
}

int
testXML::testProcString()
  /*!
    Test the removal of an XML character like
    & etc.
    \return 0 :: success
   */
{
  ELog::RegMethod RegA("testXML","testProcString");

  typedef std::tuple<std::string,std::string> TTYPE;

  std::vector<TTYPE> Tests;
  Tests.push_back(TTYPE("Dougherty & White - ANU Canberra",
			"Dougherty &amp; White - ANU Canberra"));


  for(const TTYPE& tc : Tests)
    {
      const std::string PS=XML::procString(std::get<0>(tc));
      if (PS!=std::get<1>(tc))
	{
	  ELog::EM<<"Process string == "<<PS<<ELog::endTrace;
	  return -1;
	}
    }
  return 0;
}

int
testXML::testLoadInclude()
  /*!
    Test the building of a set of included objects
    \retval 0 :: success
  */
{
  ELog::RegMethod RegA("testXML","testLoadInclude");

  UnitTest::Top A(54);
  std::ofstream TX("testXML.xml");

  XMLcollect CO;
  CO.addGrp("Top");
  A.procXML(CO);
  CO.closeGrp(); 
  CO.writeXML(TX);
  
  CO.clear();
  CO.loadXML("testXML.xml");

  std::ofstream OX("testXMLout.xml");
  CO.writeXML(OX);
  
  return 0;
}

int
testXML::testCombineGrid()
  /*!
    Test of the Combination call combineGrid
    from the XMLgridSupport module
    \retval 0 :: success
   */
{
  XMLcollect CO;
  CO.addGrp("Spectra");
  // SET the vectors
  std::vector<double> T(10);
  for(size_t i=0;i<10;i++)
    T[i]=static_cast<double>(i);
  std::vector<double> Y(10);
  std::vector<double> Z(10);
  fill(Y.begin(),Y.end(),5.5);
  fill(Z.begin(),Z.end(),8.5);

  // SET the grid:
  for(int i=0;i<10;i++)
    {
      std::ostringstream cx;
      cx<<"grid"<<i;
      XMLgrid<std::vector,double,std::allocator<double> >* Gptr=
	new XMLgrid<std::vector,double,std::allocator<double> >(CO.getCurrent(),cx.str());
      Gptr->setComp(0,T);
      Gptr->setComp(1,Y);
      Gptr->setComp(2,Z);
      CO.getCurrent()->addManagedObj(Gptr);
    }
  CO.closeGrp();

  std::ostringstream cExpect;
  cExpect<<"<GridCluster iStart=\"0\" iEnd=\"4\">\n";
  for(int i=0;i<10;i++)
    cExpect<<" "<<i<<" 5.5 8.5 5.5 8.5 5.5 8.5 5.5 8.5 5.5 8.5 \n";
  cExpect<<" </GridCluster>\n";

  combineGrid(CO,"Spectra/grid.*",5);

  std::ostringstream cx;
  XMLobject* GPT=CO.findObj("GridCluster");
  GPT->writeXML(cx);
  const std::string Out=StrFunc::stripMultSpc(cx.str());
  if (Out!=cExpect.str())
    {
      ELog::EM<<Out<<ELog::endTrace;
      ELog::EM<<cExpect.str()<<ELog::endDiag;
      size_t index;
      for(index=0;index<Out.size() && 
	    Out[index]==cExpect.str()[index];index++) ;
      ELog::EM<<"Mismatch at character == "<<index
	      <<" :"<<static_cast<int>(Out[index])
	      <<" ++ "<<static_cast<int>(cExpect.str()[index])
	      <<ELog::endTrace;
      return -1;
    }


  CO.writeXML(cx);


  
  return 0;
}

int
testXML::testCombineDeepGrid()
  /*!
    Test of the Combination call combineDeepGrid
    from the TimeData module
    \retval 0 :: success
  */
{
  ELog::RegMethod RegA("testXML","testCombineDeepGrid");

  XMLcollect CO;
  CO.addGrp("Spectra");
  // SET the vectors
  std::vector<double> T(10);
  for(size_t i=0;i<10;i++)
    T[i]=static_cast<double>(i);
  std::vector<double> Y(10);
  std::vector<double> Z(10);
  fill(Y.begin(),Y.end(),5.5);
  fill(Z.begin(),Z.end(),8.5);

  // SET the grid:
  for(int i=0;i<10;i++)
    {
      std::ostringstream cx;
      cx<<"grid"<<i;
      XMLgrid<std::vector,double,std::allocator<double> >* Gptr=
	new XMLgrid<std::vector,double,std::allocator<double> >(CO.getCurrent(),cx.str());
      Gptr->setComp(0,T);
      Gptr->setComp(1,Y);
      Gptr->setComp(2,Z);
      CO.getCurrent()->addManagedObj(Gptr);
    }
  CO.closeGrp();

  combineDeepGrid<std::vector,double,std::allocator<double> >
    (CO,std::string("Spectra/grid.*"),5);

  //
  // Checking of result:
  //
  std::ostringstream cExpect;
  cExpect<<"<GridCluster iStart=\"5\" iEnd=\"9\">\n";
  for(int i=0;i<10;i++)
    cExpect<<" "<<i<<" 5.5 8.5 5.5 8.5 5.5 8.5 5.5 8.5 5.5 8.5 \n";
  cExpect<<" </GridCluster>\n";

  std::ostringstream cx;
  XMLobject* GPT=CO.findObj("GridCluster",1);
  GPT->writeXML(cx);

  const std::string Out=StrFunc::stripMultSpc(cx.str());
  if (Out!=cExpect.str())
    {
      ELog::EM<<Out<<ELog::endTrace;
      ELog::EM<<cExpect.str()<<ELog::endDiag;
      size_t index;
      for(index=0;index<Out.size() && 
	    Out[index]==cExpect.str()[index];index++) ;
      ELog::EM<<"Mismatch at character == "<<index
	      <<" :"<<static_cast<int>(Out[index])
	      <<" ++ "<<static_cast<int>(cExpect.str()[index])
	      <<ELog::endTrace;
      return -1;
    }
  
  return 0;
}

int
testXML::testDeleteObj()
  /*!
    Test the deletion operator
    \retval -1 :: Failed to delete
    \retval 0 :: success
   */
{
  ELog::RegMethod RegA("testXML","testDeleteObj");

  buildXML();
  std::string Out="<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?>\n"
    "<metadata_entry>"
    "<test>"
    "<Beta1/>"
    "</test>"
    "</metadata_entry>";

  if (!XOut.deleteObj(XOut.findObj("Beta2")))
    {
      ELog::EM<<"DELETE Failed on beta2"<<ELog::endDiag;
      return -1;
    }
  if (!XOut.deleteObj(XOut.findObj("Beta3")))
    {
      ELog::EM<<"DELETE Failed on Beta3"<<ELog::endDiag;
      return -2;
    }
  std::ostringstream cx;
  XOut.writeXML(cx);
  const std::string Result=StrFunc::removeSpace(cx.str());
  if (Result!=StrFunc::removeSpace(Out))
    {
      ELog::EM<<"Result == "<<cx.str()<<ELog::endDiag;
      ELog::EM<<"Expect == "<<Out<<ELog::endDiag;
      return -3;
    }
  return 0;
    
}

int
testXML::testDataBlock()
  /*!
    Test of the DataBlock write/read module
    \retval 0 :: success
   */
{
  ELog::RegMethod RegA("testXML","testDataBlock");

  std::string DBout=
    StrFunc::stripMultSpc("<DB index_0=\"3\" index_1=\"4\">\n"
			  " 1 1 1 1 1 2 3 4 1 3 \n"
			  " 5 7 \n"
			  " </DB>\n");

  XMLcollect CO;
  CO.addGrp("Spectra");
  // SET the multi_array
  typedef boost::multi_array<double,2> mapArray;
  mapArray TX(boost::extents[3][4]);
  
  for(int i=0;i<3;i++)
    for(int j=0;j<4;j++)
      TX[i][j]=1.0+i*j;

  XMLdatablock<double,2>* DB=
    new XMLdatablock<double,2>(CO.getCurrent(),"DB");
  DB->setUnManagedComp(&TX);
  CO.getCurrent()->addManagedObj(DB);
  CO.closeGrp();

  std::ostringstream cx;  
  DB->writeXML(cx);

  if (StrFunc::stripMultSpc(cx.str())
      !=StrFunc::stripMultSpc(DBout))
    {
      std::string Actual=StrFunc::stripMultSpc(cx.str());
      ELog::EM<<"DBOut  == "<<DBout<<ELog::endDiag;
      ELog::EM<<"Actual == "<<Actual<<ELog::endDiag;
      size_t index;
      for(index=0;index<DBout.size() && 
	    DBout[index]==Actual[index];index++) ;
      ELog::EM<<"Mismatch at character == "<<index
	      <<" :"<<static_cast<int>(DBout[index])
	      <<" ++ "<<static_cast<int>(Actual[index])
	      <<ELog::endTrace;

      return -1;
    }
  
  return 0;
}
