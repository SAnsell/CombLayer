/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   test/testBoost.cxx
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
#include <fstream>
#include <sstream>
#include <cmath>
#include <vector>
#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <functional>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "RefControl.h"
#include "Triple.h"

#include "testFunc.h"
#include "testBoost.h"

std::ostream&
operator<<(std::ostream& OX,const Report& A)
  /*!
    Debug function for outputting infor
    \param OX :: output stream
    \param A :: ReportRef Object to write
    \return output stream status
   */
{
  A.write(OX);
  return OX;
}


// First the support class
int Report::cnt(0); 
int Report::Dcnt(0); 

Report::Report() :
  num(0)
  /*!
    Default constructor
  */
{
  cnt++;
}

Report::Report(const int A) :
  num(A)
  /*!
    Constructor with parameter
    \param A :: Value to set number.
  */
{
  cnt++;
}

Report::Report(const size_t A) :
  num(static_cast<int>(A))
  /*!
    Constructor with parameter
    \param A :: Value to set number.
  */
{
  cnt++;
}

Report::Report(const Report& A) :
  num(A.num)
  /*!
    Copy Constructor 
    \param A :: Report object to copy
  */
{
  cnt++;
}

Report&
Report::operator=(const Report& A)
  /*!
    Assignment operator=
    \param A :: Report object to copy
    \returns *this
  */
{
  if (this!=&A)
    {
      num=A.num;
    }
  return *this;
}

Report::~Report()
  /*!
    Destructor remove 
  */
{
  Dcnt--;
}

void
Report::write(std::ostream& OX) const
  /*!
    Output function to stream
    \param OX :: Output stream
  */
{
  OX<<num<<"(c/d) =="<<cnt<<":"<<Dcnt;
  return;
}

void
Report::setNum(const int A) 
  /*!
    Sets the number
    \param A :: number to set
  */
{
  num=A;
  return;
}

// REF ADDITON
ReportRef::ReportRef(const double& P) :
  Report(),Param(P)
  /*!
    Constructor
    \param P :: Parameter to Track
  */
{ }


ReportRef::ReportRef(const double& P,const int I) 
  : Report(I),Param(P)
  /*!
    Constructor
    \param P :: Parameter to Track
    \param I :: Index point
  */
{}

ReportRef::ReportRef(const ReportRef& A) 
  : Report(A),Param(A.Param) 
  /*!
    Copy Constructor
    \param A :: ReportRef to copy
  */
{
}

ReportRef&
ReportRef::operator=(const ReportRef& A) 
  /*!
    Assignment operator 
    \param A :: ReportRef to copy
    \return *this
  */
{ 
  if (this!=&A)
    {
      Report::operator=(A);
    }
  return *this;
}

ReportRef::~ReportRef()
  /*!
    Destructor
  */
{ }

void
ReportRef::write(std::ostream& OX) const
  /*!
    Debug output function
  */
{
  OX<<"P== "<<Param<<" -- ";
  Report::write(OX);
  return;
}



testBoost::testBoost()
  /*!
    Constructor
  */
{}

testBoost::~testBoost() 
  /*!
    Destructor
  */
{}

int 
testBoost::applyTest(const int extra)
  /*!
    Applies all the tests and returns 
    the error number
    \param extra :: Index of test
    \returns -ve on error 0 on success.
  */
{
  ELog::RegMethod RegItem("testBoost","applyTest");
  TestFunc::regSector("testBoost");

  typedef int (testBoost::*testPtr)();
  testPtr TPtr[]=
    {
      &testBoost::testRepPtr,
      &testBoost::testRepRefPtr,
      &testBoost::testSharePtr,
      &testBoost::testSharePtrMap,
      &testBoost::testVecPtr,
      &testBoost::testVecRef
    };

  const std::string TestName[]=
    {
      "RepPtr",
      "RepRefPtr",
      "SharePtr",
      "SharePtrMap",
      "VecPtr",
      "VecRef"
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
testBoost::testVecPtr()
  /*!
    Test of refControl on a vector object!!!
    \returns 0 on succes and -ve on failure
  */
{
  ELog::RegMethod RegA("testBoost","testVecPtr");

  
  RefControl<std::vector<double> > Vitem;
  Vitem.access().push_back(67.0);
  Vitem.access().push_back(15.0);
  Vitem.access().push_back(20.0);
  Vitem.access().push_back(8.0);

  RefControl<std::vector<double> > VitemX;
  VitemX=Vitem;
  sort(VitemX.access().begin(),VitemX.access().end());
  // Check if sorted 
  for(size_t i=0;i<3;i++)
    {
      if ((*VitemX)[i]>(*VitemX)[i+1])
	{
	  ELog::EM<<"Failed on ordering"<<ELog::endCrit;
	  return -1;
	}
    }
  // Check if unsorted
  double sum(0.0);   
  for(size_t i=0;i<4;i++)
    sum+=static_cast<double>(i+1)*(*Vitem)[i];

  if (fabs(sum-189.0)>1e-4)
    {
      ELog::EM<<"Failed to keep arrays separate"<<ELog::endCrit;
      ELog::EM<<"Sum"<<sum<<ELog::endCrit;
      return -2;
    }

  return 0;
}

int
testBoost::testVecRef()
  /*!
    Test of refControl  on a vector object!!!
    \returns 0 on succes and -ve on failure
  */ 
{
  ELog::RegMethod RegA("testBoost","testVecRef");

  typedef RefControl<std::vector<double> > Rtype;
  std::vector<Rtype> Vitem;
  Vitem.resize(3);
  for(size_t i=0;i<3;i++)
    for(size_t j=0;j<5;j++)
      Vitem[i].access().push_back(static_cast<double>(j+100*i));

  std::vector<Rtype> Jitem;

  Jitem=Vitem;
  Vitem[2].access()[1]=999;

  for(size_t i=0;i<3;i++)
    for(size_t j=0;j<5;j++)
      {
	bool test=(fabs((*Vitem[i])[j]-(*Jitem[i])[j])<1e-4);
	if ((i==2 && j==1)==test)
	  {
	    ELog::EM<<"Failed on point "<<i<<" "<<j<<ELog::endCrit;
	    return -1;
	  }
      } 
  return 0;
}
  
int
testBoost::testRepPtr()
  /*!
    Test of simple RefControl on a vector
    \return 0 on success (currently never works)
  */
{
  ELog::RegMethod RegA("testBoost","testRepPtr");

  std::vector< RefControl<Report> > vec;
  for(size_t i=0;i<3;i++)
    {
      vec.push_back(RefControl<Report>());
      vec[i].access().setNum(static_cast<int>(i)+100);
    }
  RefControl<Report> X=vec[0];
  for(size_t i=0;i<3;i++)
    {
      if (vec[i]->getNum()!=100+static_cast<int>(i) || 
	  (*vec[i]).getNum()!=100+static_cast<int>(i))
	{
	  ELog::EM<<"Failed at "<<i
		  <<" V[]->Num =="<<vec[i]->getNum()
		  <<" (*V).Num =="<<(*vec[i]).getNum()
		  <<ELog::endCrit;
	  return -1;
	}
    }

  X.access().setNum(201);
  if (vec[0]->getNum()!=100 || X->getNum()!=201)
    {
      ELog::EM<<"Vector[0] =="<<(*vec[0]).getNum()<<ELog::endCrit;
      ELog::EM<<"X =="<<X->getNum()<<ELog::endCrit;  
      return -2;
    }

  return 0;
}

int
testBoost::testRepRefPtr()
  /*!
    Test of simple RefControl on a vector
    ReportRef holds a value (int) and a reference (PP) 
    -- all references are updated / but integers are separate
    \return 0 on success (currently never works)
  */
{
  ELog::RegMethod RegA("testBoost","testRepRefPtr");

  double PP(3.45);

  //
  // This
  std::vector<RefControl<ReportRef> > vec;
  for(size_t i=0;i<3;i++)
    {
      vec.push_back(RefControl<ReportRef>
		    ( ReportRef(PP,static_cast<int>(i)+100) ));
      PP+=1.0;
    }
  PP=8731.0;
  RefControl<ReportRef> X=vec[0];
  for(size_t i=0;i<3;i++)
    {
      if (vec[i]->getNum() != 100+static_cast<int>(i) ||
	  fabs(vec[i]->getParam()-PP)>1e-5)
	{
	  ELog::EM<<"Failed on vec["<<i<<"]"<<ELog::endTrace;
	  ELog::EM<<"Vec Num == "<<vec[i]->getNum()<<ELog::endTrace;
	  ELog::EM<<"Vec == "<<*vec[i]<<ELog::endTrace;
	  return -1;
	}
      PP+=8762.2;
    }


  X.access().setNum(201);
  if (vec[0]->getNum()!=100 || X->getNum()!=201)
    {
      ELog::EM<<"Vector[0] =="<<(*vec[0])<<ELog::endTrace;  
      ELog::EM<<"X         =="<<*X<<ELog::endTrace;  
    }

  // std::cout<<"Vector num =="<<vec[i].access().getNum()<<std::endl;
  // X.access().num=10;
  

  return 0;
}
  
int
testBoost::testSharePtr()
  /*!
    Apply a test of eset and copy
    \retval 0 :: success
  */
{
  ELog::RegMethod RegA("testBoost","testSharPtr");

  typedef std::shared_ptr<Report> RPtr;
  std::vector<RPtr> vec;
  for(int i=0;i<10;i++)
    vec.push_back(RPtr(new Report(i)));

  if (vec[3]->getNum()!=3)
    {
      ELog::EM<<"Vec[3] == "<<vec[3]<<ELog::endTrace;  
      return -1;
    }
  
  RPtr X=RPtr(new Report(28));

  X=vec[3];
  X.reset();
  if (vec[3]->getNum()!=3)
    {
      ELog::EM<<"Vec[3] == "<<*vec[3]<<ELog::endTrace;  
      return -2;
    }
  return 0;
}

int
testBoost::testSharePtrMap()
  /*!
    Apply a test to the share map pointer
    \retval 0 :: success
  */
{
  ELog::RegMethod RegA("testBoost","testSharePtrMap");

  typedef std::shared_ptr<Report> RPtr;
  typedef std::map<std::string,RPtr> tmapTYPE;
  tmapTYPE TMap;
  tmapTYPE CMap;


  TMap.emplace("A",std::make_shared<ReportRef>(11.1,1));
  TMap.emplace("B",std::make_shared<ReportRef>(12.2,2));
  TMap.emplace("C",std::make_shared<ReportRef>(13.3,3));
  TMap.emplace("D",std::make_shared<ReportRef>(14.4,4));


  CMap=TMap;
  TMap["C"]=std::make_shared<ReportRef>(21.0,21);
  const double CD=std::dynamic_pointer_cast<ReportRef>(TMap["C"])->getParam();
  const double TD = std::static_pointer_cast<ReportRef>(TMap["C"])->getParam();
  //  const double CD = std::static_pointer_cast<ReportRef>(CMap["C"])->getParam();
  if (TMap["C"]->getNum()!=21 ||
      CMap["C"]->getNum()!=3 ||
      std::abs(TD-21.0)>1e-4 ||  
      std::abs(CD-21.0)>1e-4)
    {
      ELog::EM<<"TMap == "<< *TMap["C"] <<ELog::endTrace;
      ELog::EM<<"CMap == "<< *CMap["C"]<<ELog::endTrace;
      ELog::EM<<"TMap[21] == "<< TMap["C"]->getNum()<<ELog::endTrace;
      ELog::EM<<"CMap[3] == "<< CMap["C"]->getNum()<<ELog::endTrace;
      ELog::EM<<"TD == "<< TD<<ELog::endTrace;
      ELog::EM<<"CD[13.3] == "<< CD<<ELog::endTrace;
      return -1;
    }

  return 0;
}



