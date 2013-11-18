/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   src/NRange.cxx
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
#include <cmath>
#include <complex>
#include <vector>
#include <string>
#include <map>
#include <list>
#include <algorithm>
#include <iterator>
#include <functional>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "support.h"
#include "mathSupport.h"
#include "Triple.h"
#include "NRange.h"

std::ostream&
operator<<(std::ostream& OX,const NRange& NR)
  /*!
    Write NRange to a stream (used condenced
    format)
    \param OX :: output stream
    \param NR :: NRange unit
    \return OX in modified state
  */
{
  NR.write(OX);
  return OX;
}


NRange::NRange() 
  /*!
    Default constructor
  */
{}

NRange::NRange(const NRange& A) : 
  Items(A.Items)
  /*!
    Copy Constructor
    \param A :: NRange object to copy
  */
{ }

NRange::NRange(const std::string& N)
  /*!
    Constructor based on a simple string
    \param N :: String to pass
  */
{ 
  const int errN=processString(N);
  if(errN)
    throw ColErr::InvalidLine(N,"NRange::constructor");
}

NRange&
NRange::operator=(const NRange& A)
  /*!
    Assignment operator=
    \param A :: Object to copy
    \return *this
  */
{
  if(&A!=this)
    {
      Items=A.Items;
    }
  return *this;
}

NRange::~NRange()
  /*!
    Destructor
  */
{}

double
NRange::operator[](const int Index) const
  /*!
    Gets an individual value
    \param Index :: Index value 
    \return values
  */
{
  ELog::RegMethod RegA("NRange","operator[]");

  if (Index<0)
    throw ColErr::IndexError<int>(Index,0,RegA.getBase());
  
  int cnt(-1);
  std::list<NRunit>::const_iterator vc;
  double value(0.0);   // default start point
  for(vc=Items.begin();cnt!=Index && vc!=Items.end();vc++)
    {
      if (vc->first==0)                         /// Static
        {
	  value=vc->third;
	  cnt++;
	}
      else if (vc->first==1)                     /// repeat 
        {
	  if (Index<=(vc->second+cnt))
	    return vc->third;
	  cnt+=vc->second;
	  value=vc->third;
	}
      else if (vc->first==2)                     // interval
        {
	  if ( Index<=(cnt+vc->second))
	    {
	      const double part(vc->third/(vc->second+1));
	      return value+part*Index-cnt;
	    }
	  cnt+=vc->second;
	  value=vc->third;
	}
      else if (vc->first==3)                    // /log interval
        {
	  if ( Index<=(cnt+vc->second))
	    {
	      const double lA(log(value));
	      const double lB(log(vc->third));
	      const double part((lB-lA)/(vc->second+1));
	      return exp(lA+part*(Index-cnt));  
	    }
	  value=vc->third;
	  cnt+=vc->second;
	}
    }
  if (cnt!=Index)
    throw ColErr::IndexError<int>(Index,cnt,RegA.getBase());
  return value;
}


NRange&
NRange::operator+=(const NRange& A)
  /*!
    Add two ranges (append A to the back of this)
    \param A :: Object to add
    \return *this
  */
{
  Items.insert(Items.end(),A.Items.begin(),A.Items.end());
  return *this;
}
 
int
NRange::identVal(const double& T,const double& VA,const double& VB) 
  /*!
    Determine if the values are equal within the tolerance
    \param T :: Tolerance
    \param VA :: Value 1
    \param VB :: Mid value
    \return 1 if identical
  */
{
  ELog::RegMethod RegA("NRange","identVal");
  // Zero test:
  if (fabs(VA)<T*T && fabs(VB)<T*T)
    return 1;
  return (fabs(VA-VB)/(fabs(VA)+fabs(VB))<T);
}


int
NRange::intervalVal(const double& T,const double& VA,
		    const double& VB,const double& VC) 
  /*!
    Determine if the values are equal within the tolerance
    \param T :: Tolerance
    \param VA :: Value 1
    \param VB :: Mid value
    \param VC :: End
    \return 1 if a valid interval
  */
{  
  return identVal(T,VB,(VA+VC)/2.0);
}

int
NRange::logIntVal(const double& T,const double& VA,
		  const double& VB,const double& VC) 
  /*!
    Determine if the values are in a log range within the tolerance
    \param T :: Tolerance
    \param VA :: Value 1
    \param VB :: Mid value
    \param VC :: End
    \return 1 if a valid interval
  */
{
  const double T3(T*T*T);

  if (VA<=T3 || VB<=T3 || VC<=T3 || VC<=VA)
    return 0;
  if ((VC-VA)/(VA+VC)<T) return 0;

  return identVal(T,log(VB),(log(VA)+log(VC))/2.0);
}

int 
NRange::processString(const std::string& N)
  /*!
    Process a string of the type 
      - 3.4 5.6 
      - 3.4 10i 5.6 
      - 3.4 10log 5.6 
     
    It assumes that there was not a last value
    The string is stripped of the first values
    
    \param N :: String 

    \retval 0 :: success
    \retval -1 :: string empty
    \retval -2 :: Unable to process first nubmer
  */
{
  if(N.empty())
    return -1;
  // Work with part of the strin first

  std::string MS(N);
  
  transform(MS.begin(),MS.end(),MS.begin(),std::ptr_fun(&tolower));

  // From now on in we have a string type (definately)
  // check if the next number is a number or doesn't
  // exist.
  double Value;
  std::string Comp;
  int index,flag;
  int prevFlag=-3;
  do
    {
      flag=StrFunc::section(MS,Value);
      if (!flag) 
	flag= -1*StrFunc::section(MS,Comp);

      if (flag>0)
        {
	  Items.push_back(NRunit(0,0,Value));	
	}
      else if (flag<0)
        {
	  if (prevFlag<0)  /// Error with 10log 5log instanity
	    return -3;

	  if (!StrFunc::sectPartNum(Comp,index))
	    index=1;
	  if (Comp=="log")
	    Items.push_back(NRunit(3,index,Value));
	  else if (Comp=="i")
	    Items.push_back(NRunit(2,index,Value));
	  else if (Comp=="r")
	    Items.push_back(NRunit(1,index,Value));
	  else
	    return -3;
	}
      prevFlag=flag;
    }
  while (flag);

  // Fancy bit to get the range into the Items table
  std::list<NRunit>::iterator vc;
  for(vc=Items.begin();vc!=Items.end();vc++)
    {
      if (vc->first>1)
        {
	  NRunit& X=(*vc);
	  vc++;
	  if (vc==Items.end())  /// Malformed string
	    return -4;
	  X.third= vc->third- X.third;
	}
    }
  return 0;
}

template<>
void
NRange::setVector(const std::vector<double>& Vec)
  /*!
    Takes the vector and writes it into the NRange.
    \param Vec :: Vector use
  */
{
  ELog::RegMethod RegA("NRange","setVector");

  Items.erase(Items.begin(),Items.end());
  
  std::vector<double>::const_iterator vc;
  for(vc=Vec.begin();vc!=Vec.end();vc++)
    Items.push_back(NRunit(0,0,*vc));	 
  
  double TValue(mathFunc::minDifference(Vec,1e-15)/10.0);
  if (TValue>1e-6) TValue=1e-6;
  condense(TValue);
  return;
}

template<>
void
NRange::setVector(const std::vector<int>& Vec)
  /*!
    Takes the vector and writes it into the NRange.
    \param Vec :: Vector use
  */
{
  ELog::RegMethod RegA("NRange","setVector");

  Items.erase(Items.begin(),Items.end());
  
  std::vector<int>::const_iterator vc;
  for(vc=Vec.begin();vc!=Vec.end();vc++)
    Items.push_back(NRunit(0,0,*vc));	 
  
  condense(1e-3);
  return;
}

void
NRange::addComp(const double V)
  /*!
    Adds a component. Condensation needs 
    to be done by the user later.
    \param V :: New value
   */
{
  Items.push_back(NRunit(0,0,V));
  return;
}

void
NRange::condense(const double Tol)
  /*!
    Determine intervals 
    \param Tol :: Tolerance value
  */
{
  ELog::RegMethod RegA("NRange","condence");

  std::vector<double> Values;
  writeVector(Values);
  if (Values.size()<2)
    return;
  
  std::list<NRunit> Out;
  // Extra value here a guard item
  std::vector<int> type(Values.size()+1,0);
  for(size_t i=1;i<Values.size();i++)
    {
      // Basic repeat:
      if (identVal(Tol,Values[i],Values[i-1]))
	type[i]=1;
      else if (i>1 && intervalVal(Tol,Values[i-2],Values[i-1],Values[i]))
	{
	  type[i-1]=2;
	}
      else if (i>1 && logIntVal(Tol/100.0,Values[i-2],Values[i-1],Values[i]))
	{
	  type[i-1]=3;
	}
    }
  size_t cnt(0);
  while(cnt<Values.size())
    {
      size_t repCnt=1;
      if (type[cnt]==0)
	Out.push_back(NRunit(0,0,Values[cnt]));
      else if (type[cnt])
	{
	  // count repeats/interval/
	  for(repCnt=1;type[cnt+repCnt]==type[cnt];repCnt++) ;
	  if (repCnt>1)
	    Out.push_back(NRunit(type[cnt],
				 static_cast<int>(repCnt),
				 Values[cnt]));
	  else
	    Out.push_back(NRunit(0,0,Values[cnt]));
	}
      cnt+=repCnt;
    }

  Items=Out;
  return;
}

int
NRange::count() const
  /*!
    Determine the number of points in a range
    \return Number of data points in range
  */
{
  int cnt(0);
  std::list<NRunit>::const_iterator vc;
  for(vc=Items.begin();vc!=Items.end();vc++)
    cnt+=(!vc->first) ? 1 : vc->second;

  return cnt;
}

int
NRange::writeVector(std::vector<double>& Vec) const
  /*!
    Sets the values into a vector
    \param Vec :: Vector to set
    \return Number of componnets
    \todo Finish
  */
{
  ELog::RegMethod RegA("NRange","writeVector");

  Vec.clear();
  double start(0.0);
  std::list<NRunit>::const_iterator vc;
  for(vc=Items.begin();vc!=Items.end();vc++)
    {
      if (vc->first==0)                         /// Static
        {
	  Vec.push_back(vc->third);
	  start=vc->third;
	}
      else if (vc->first==1)                     /// repeat 
        {
	  for(int i=0;i<vc->second;i++)
	    Vec.push_back(vc->third);
	  start=vc->third;
	}
      else if (vc->first==2)                     // interval
        {
	  const double part(vc->third/(vc->second+1));
	  for(int i=0;i<vc->second;i++)
	    {
	      start+=part;
	      Vec.push_back(start);
	    }
	}
      else if (vc->first==3)                    // /log
        {
	  const double lA=log(start);
	  const double lB=log(vc->third);
	  const double part((lB-lA)/(vc->second+1));
	  for(int i=1;i<=vc->second;i++)
	    Vec.push_back(exp(lA+i*part));
	  start=vc->third;
	}
    }
  return static_cast<int>(Vec.size());
}


void
NRange::write(std::ostream& OX) const
  /*!
    Write out the Range to a stream
    \param OX :: string stream to write out
  */
{
  static std::string AV[3]={"r","i","log"};

  std::list<NRunit>::const_iterator vc;
  for(vc=Items.begin();vc!=Items.end();vc++)
    {
      if (vc->first)
        {
	  if (vc->second>0)
	    OX<<vc->second;
	  OX<<AV[vc->first-1]<<" ";
	}
      else
	OX<<vc->third<<" ";
    }
  return;
}

