/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   test/testFunc.cxx
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#include <string>
#include <cmath>
#include <map>
#include <vector>


#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "testFunc.h"


TestFunc::TestFunc() :
  nTest(0),outStatus(7)
  /*!
    Basic Constructor
  */
{}

TestFunc::~TestFunc() 
  /*!
    Destructor
  */
{}

TestFunc&
TestFunc::Instance()
  /*!
    Static accessor to the instance of the singleton
    \return Effective this
   */
{
  static TestFunc A;
  return A;
}

void
TestFunc::regGroup(const std::string& G)
  /*!
    Set the group name using static accessor
    \param G :: Group name
  */
{
  TestFunc& A=TestFunc::Instance();
  A.setGroup(G);
  return;
}
void
TestFunc::regSector(const std::string& S)
  /*!
    Set the sector name using static accessor
    \param S :: Sector name
  */
{
  TestFunc& A=TestFunc::Instance();
  A.setSector(S);
  return;
}

void
TestFunc::regTest(const std::string& T)
  /*!
    Set the Test name using static accessor
    \param T :: Test name
  */
{
  TestFunc& A=TestFunc::Instance();
  A.setTest(T);
  return;
}

void
TestFunc::setGroup(const std::string& G)
  /*!
    Set the group name
    \param G :: Group name
  */
{
  TGroup=G;
  if ((outStatus & 7)==4)
    {
      reportTest(ELog::EM.Estream());
      ELog::EM<<ELog::endDiag;
    }
  return;
}

void
TestFunc::setSector(const std::string& S)
  /*!
    Set the sector name
    \param S :: Sector name
  */
{
  TSector=S;
  if ((outStatus & 3)==2)
    {
      ELog::EM<<"Out["<<S<<"] == "<<outStatus<<ELog::endDiag;
      reportTest(ELog::EM.Estream());
      ELog::EM<<ELog::endDiag;
    }
  
  return;
}

void
TestFunc::setTest(const std::string& T)
  /*!
    Set the test name
    \param T :: Test name
  */
{
  if (T!=TName)
    {
      nTest++;
      TName=T;
      if (outStatus & 1)
	{
	  reportTest(ELog::EM.Estream());
	  ELog::EM<<ELog::endDiag;
	}	  
    }
  return;
}

void
TestFunc::regStatus(const int A,const int B,const int C)
  /*!
    Register a status based on the scope of the section
    \param A :: Main index
    \param B :: Sub index
    \param C :: Extra index
  */
{
  outStatus  = (A<0) ? 0 : 4;
  outStatus |= (B<0) ? 0 : 2;
  outStatus |= (C<0) ? 0 : 1;
  return;
}

void
TestFunc::reportTest(std::ostream& OX) const
  /*!
    Given a function name write out a nice
    print out for it .
    \param OX :: output stream
  */
{
  OX<<"---------------------------------------------"<<std::endl;
  OX<<"Test["<<nTest<<"] "<<TGroup<<"::"
    <<TSector<<"=="<<TName<<std::endl;
  OX<<"---------------------------------------------"<<std::endl;
  return;
}

std::string
TestFunc::summary() const
  /*!
    Output a summary string
    \return Summary string
   */
{
  std::ostringstream cx;
  cx<<" completed : "<<nTest<<" tests";
  return cx.str();
}

void
TestFunc::writeList(std::ostream& OX,const size_t TSize,
		    const std::string TestName[]) const
  /*!
    Write out the list
    \param OX :: Output stream
    \param NS :: Number of entries
    \param TName :: String
   */
{
  reportTest(std::cout);
  std::ios::fmtflags flagIO=OX.setf(std::ios::left);
  for(size_t i=0;i<TSize;i++)
    {
      OX<<std::setw(30)<<TestName[i]<<"("<<i+1<<")"<<std::endl;
    }
  OX.flags(flagIO);
  return;
}

void
TestFunc::bracketTest(const std::string& Fname,std::ostream& OX)
  /*!
    Given a function name write out a nice
    print out for it .
    \param Fname :: output name to display 
    \param OX :: output stream
  */
{
  const long int sLen=(54-Fname.size())/2;
  
  OX<<"---------------------------------------------"<<std::endl;
  if (sLen>0)
    OX<<std::string(static_cast<size_t>(sLen),' ');
  OX<<Fname<<std::endl;
  OX<<"---------------------------------------------"<<std::endl;
  return;
}

void
TestFunc::writeTests(const std::vector<std::string>& TVec)
  /*!
    Write out the tests [pretty format]
    \param TVec :: Vector of tests
   */
{
  std::ios::fmtflags flagIO=std::cout.setf(std::ios::left);
  for(size_t i=0;i<TVec.size();i++)
    {
      std::cout<<std::setw(30)<<TVec[i]<<"("<<i+1<<")"<<std::endl;
    }
  std::cout.flags(flagIO);
  return;
}
