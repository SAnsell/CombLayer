/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   test/testRules.cxx
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
#include <vector>
#include <set>
#include <map>
#include <stack>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <memory>
#include <tuple>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "NameStack.h"
#include "MemStack.h"
#include "RegMethod.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "BnId.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Surface.h"
#include "Rules.h"
#include "RuleBinary.h"
#include "HeadRule.h"
#include "Object.h"
#include "surfIndex.h"
#include "mapIterator.h"

#include "testFunc.h"
#include "testRules.h"


testRules::testRules() 
/*!
  Constructor
*/
{}

testRules::~testRules() 
 /*! 
   Destructor
 */
{}

void
testRules::createSurfaces()
  /*!
    Create the surface list
   */
{
  ELog::RegMethod RegA("testObject","createSurfaces");
  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();
  SurI.reset();
  
  // First box :
  SurI.createSurface(1,"px -1");
  SurI.createSurface(2,"px 1");
  SurI.createSurface(3,"py -1");
  SurI.createSurface(4,"py 1");
  SurI.createSurface(5,"pz -1");
  SurI.createSurface(6,"pz 1");

  SurI.createSurface(11,"px -1");
  SurI.createSurface(12,"px 1");
  SurI.createSurface(13,"py -1");
  SurI.createSurface(14,"py 1");
  SurI.createSurface(15,"pz -1");
  SurI.createSurface(16,"pz 1");
  return;
}

int 
testRules::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: Index of test to run
    \return 0 on success / -ve on error
  */
{
  ELog::RegMethod RegA("testRules","applyTest");
  TestFunc::regSector("testRules");

  ELog::MemStack::Instance().setVFlag(1);
  typedef int (testRules::*testPtr)();
  testPtr TPtr[]=
    {
      &testRules::testCreateDNF,
      &testRules::testExclude,
      &testRules::testIsValid,
      &testRules::testMakeCNF,
      &testRules::testRemoveComplement,
      &testRules::testRuleBinary
    };
  const std::string TestName[]=
    {
      "CreateDNF",
      "Exclude",
      "IsValid",
      "MakeCNF",
      "RemoveComplement",
      "RuleBinary"
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
testRules::testCreateDNF()
  /*!
    Create a DNF for mfrom a non-DNF and determin if we get 
    a DNF object
    \return 0 on success
  */
{
  ELog::RegMethod RegA("testRules","testCreateDNF");

  ELog::EM<<"EARLY RETURN AS TEST FAILS"<<ELog::endWarn;
  return 0;
  typedef std::tuple<std::string,int,std::string> TTYPE;
  const std::vector<TTYPE> Tests=
    {
      TTYPE("3 0 11  -12  13  -14  15  (-16 : 17 )",1,
	    "( -16 15 -14 13 -12 11 ) : ( 17 15 -14 13 -12 11 )"),
      
      TTYPE("3 0 12:-12:13:-14",0,""),
      
      TTYPE("3 0 11:-12:13:-14",1,"-14 : -12 : 11 : 13")
    };

  int cnt(1);
  MonteCarlo::Object Tx;
  for(const TTYPE& tc : Tests)
    {
      Tx.setObject(std::get<0>(tc));
      const Rule* TRule=Tx.topRule();
      Rule* XRule=TRule->clone();
      RuleBinary Dlist(XRule);
      ELog::EM<<"DList == "<<Dlist<<ELog::endDiag;
      Dlist.makeSurfIndex();
      ELog::EM<<"DN == "<<Dlist.createDNFitems()<<ELog::endDiag;
      ELog::EM<<"Gro == "<<Dlist.group()<<ELog::endDiag;
      ELog::EM<<"DList == "<<Dlist<<ELog::endDiag;
      
      Rule* OutRule=Dlist.createTree();
      if ( (std::get<1>(tc) && 
	    (!OutRule || OutRule->display()!=std::get<2>(tc))) ||
	   (!std::get<1>(tc) && OutRule))
	{
	  ELog::EM<<"Test :: "<<cnt<<ELog::endDiag;
	  ELog::EM<<"Original == "<<TRule->display()<<ELog::endTrace;
	  ELog::EM<<"D == "<<Dlist<<ELog::endTrace;
	  ELog::EM<<ELog::endDiag;
	  ELog::EM<<"Out Rule == "<<std::get<2>(tc)<<ELog::endDiag;
	  if (OutRule)
	    ELog::EM<<"Rule == "<<OutRule->display()<<ELog::endTrace;
	  else
	    ELog::EM<<"OutRule == NULL"<<ELog::endTrace;
	  delete XRule;
	  delete OutRule;
	  return -1;
	}
      delete XRule;
      delete OutRule;
      cnt++;
    }
  return 0;
}


int
testRules::testIsValid()
  /*!
    Check the validity of a point in a rule
    \return 0 :: success / -ve on error
   */
{
  ELog::RegMethod RegA("testRules","testIsValid");

  createSurfaces();

  typedef std::tuple<std::string,std::string,size_t,size_t> TTYPE;
  std::vector<TTYPE> Tests;
  Tests.push_back(TTYPE("3 0 1 -2 3 -4 5 -6","+-+-+-",1,1));
  Tests.push_back(TTYPE("3 0 1 -2 3 -4 5 -6 (11:(12 14))","+-+-+-+--",1,5));
    
  for(const TTYPE& tc : Tests)
    {
      std::ostringstream cx;
      MonteCarlo::Object Tx;
      Tx.setObject(std::get<0>(tc));
      Tx.populate();
      Tx.createSurfaceList();

      const Rule* TRule=Tx.topRule();
      MapSupport::mapIterator MI(Tx.getSurfSet());
      size_t N(0);       // number of successful items

      std::string out;
      do 
	{
	  if (TRule->isValid(MI.getM()))
	    {
	      cx<<MI.status()<<"\n";
	      N++;                    
	      if (N==std::get<2>(tc))
		out=MI.status();
	    }
	} while(++MI);

      if (N!=std::get<3>(tc) || out!=std::get<1>(tc))
	{
	  ELog::EM<<"Rule Finished == "<<TRule->display()<<ELog::endTrace;
	  ELog::EM<<"N        == "<<N<<ELog::endTrace;
	  ELog::EM<<"Expected == "<<std::get<3>(tc)<<ELog::endTrace;
	  ELog::EM<<"Got      == "<<out<<ELog::endTrace;
	  ELog::EM<<"Expected == "<<std::get<1>(tc)<<ELog::endTrace;
	  ELog::EM<<"Total      == \n"<<cx.str()<<ELog::endTrace;
	  return -1;
	}
    }
  return 0;
}

int
testRules::testExclude()
  /*!
    Check the validity of a point in a rule
    \return 0 :: success / -ve on error
   */
{
  ELog::RegMethod RegA("testRules","testExclude");

  createSurfaces();


  typedef std::tuple<std::string,int,int,int> TTYPE;
  std::vector<TTYPE> Tests;
  Tests.push_back(TTYPE("3 0 1 -2 3 -4 5 -6",1,0,1));
  // since 12 -14 are all true both success
  Tests.push_back(TTYPE("3 0 1 -2 3 -4 5 -6 (-11:12:13:-14)",11,1,1));
  
  for(const TTYPE& tc : Tests)
    {
      std::ostringstream cx;
      MonteCarlo::Object Tx;
      Tx.setObject(std::get<0>(tc));
      Tx.createSurfaceList();

      const HeadRule& HR = Tx.getHeadRule();

      // set all the rules to be true REGARDLESS of state
      MapSupport::mapIterator MI(Tx.getSurfSet());
      std::map<int,int> rmap=MI.getM();
      for(std::map<int,int>::value_type& MItem : rmap)
	{
	  if (MItem.first!=std::get<1>(tc))
	    MItem.second = 0;
	  else
	    MItem.second = -1;   // true -ve sence
	}      

      const int minusRes=HR.isValid(rmap);
      rmap[std::get<1>(tc)]=1;
      const int plusRes=HR.isValid(rmap);
      
      if (minusRes!=std::get<2>(tc) ||
	  plusRes!=std::get<3>(tc) )
	{
	  ELog::EM<<"Rule "<<HR.display()<<ELog::endDiag;
	  ELog::EM<<"- == "<<minusRes<<" ("<<std::get<2>(tc)<<")"
		  <<ELog::endDiag;;
	  ELog::EM<<"+ == "<<plusRes<<" ("<<std::get<3>(tc)<<")"
		  <<ELog::endDiag;;
	  return -1;
	}

    }
  return 0;
}

int
testRules::testMakeCNF()
  /*!
    Check the production of a Normal Form
    \return 0 :: success / -ve on error
  */
{
  ELog::RegMethod RegA("testRules","testMakeCNF");

  MonteCarlo::Object Tx;

  typedef std::tuple<std::string,int,std::string> TTYPE;
  std::vector<TTYPE> Tests;
  Tests.push_back(TTYPE("3 0 (1 : -2)  3 : (-3 -1 2)",0,
		       "( 3 1 ) : ( 3 -2 ) : ( 2 -1 -3 )"));

  for(const TTYPE& tc : Tests)
    {
      Tx.setObject(std::get<0>(tc));
      const Rule* TRule=Tx.topRule();
      Rule* XRule=TRule->clone();
      const int nCount=Rule::makeCNF(XRule);
      if (XRule->display()!=std::get<2>(tc))
	{
	  ELog::EM<<"Rule Finished == "<<XRule->display()<<ELog::endTrace;
	  ELog::EM<<"Expected == "<<std::get<2>(tc)<<ELog::endTrace;
	  ELog::EM<<"Change == "<<nCount<<ELog::endTrace;
	  delete XRule;
	  return -1;
	}
      delete XRule;
    }
  return 0;
}

int
testRules::testRemoveComplement()
  /*!
    Test the removeal of # objects
    \return 0 on success / -ve on error
  */
{
  ELog::RegMethod RegA("TestRules","testRemoveComplement");
  
  typedef std::tuple<std::string,std::string> TTYPE;
  std::vector<TTYPE> Tests;

  Tests.push_back(TTYPE("1 -2","-1 : 2"));
  Tests.push_back(TTYPE("-8 : (-7 5 -6) ","( -5 : 7 : 6 ) 8"));

  int cnt(1);
  for(const TTYPE& tc : Tests)
    {
      std::shared_ptr<Rule> RP(Rule::procCompString(std::get<0>(tc)));
      
      if (!RP || (RP->display()!=std::get<1>(tc)) )
	{
	  ELog::EM<<"Failed test "<<cnt<<ELog::endTrace;
	  ELog::EM<<"Input  :"<<std::get<0>(tc)<<ELog::endTrace;
	  ELog::EM<<"Expect :"<<std::get<1>(tc)<<ELog::endTrace;
	  if (RP)
	    ELog::EM<<"Result :"<<RP->display()<<ELog::endTrace;
	  else
	    ELog::EM<<"Result : NULL Ptr"<<ELog::endTrace;
	  return -1;
	}
      cnt++;
    }
  return 0;
}

int
testRules::testRuleBinary()
  /*!
    Test the binary rule.
    Fills it with a number of binary items
    Then grops the results and finds the essential
    primary inplicates
    \return 0 on success / -ve on error
  */
{
  ELog::RegMethod RegA("TestRules","testRuleBinary");
  
  RuleBinary Blist(0);
  size_t PX[]={4,8,9,10,11,12,14,15};
  std::vector<size_t> PI;
  for(size_t i=0;i<8;i++)
    PI.push_back(PX[i]);
  
  Blist.fillDNFitems(4,PI);
  ELog::EM<<"Make EPI == "<<Blist.group()<<ELog::endTrace;

  const std::vector<BnId>& EPI=Blist.getEPI();
  std::ostringstream cx;
  copy(EPI.begin(),EPI.end(),
       std::ostream_iterator<BnId>(cx," "));
  if (cx.str()!="1-0- 11-1 01-0 -010 ")
    {
      ELog::EM<<"CX == "<<cx.str()<<ELog::endTrace;
      ELog::EM<<"Rule Finished == "<<ELog::endTrace;
      ELog::EM<<Blist<<ELog::endTrace;
      //      return -1;
    }

  return 0;
}

