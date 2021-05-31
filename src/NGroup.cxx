/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   src/NGroup.cxx
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#include "OutputLog.h"

#include "support.h"

#include "NGroup.h"

namespace RangeUnit
{

template<typename T> 
std::ostream&
operator<<(std::ostream& OX,const NGroup<T>& NR)
  /*!
    Write NGroup to a stream (used condenced
    format)
    \param OX :: output stream
    \param NR :: NGroup unit
    \return OX in modified state
  */
{
  NR.write(OX);
  return OX;
}

template<typename T> 
std::ostream&
operator<<(std::ostream& OX,const RUnit<T>& RU)
  /*!
    Write RUnit to a stream (used condenced
    format)
    \param OX :: output stream
    \param RU :: RUnit
    \return OX in modified state
  */
{
  RU.write(OX);
  return OX;
}

template<typename T>
bool
identVal(const double tol,const T& VA,const T& VB) 
  /*!
    Determine if the values are equal within the tolerance
    \param T :: Tolerance
    \param VA :: Value 1
    \param VB :: Mid value
    \return 1 if identical
  */
{
  if constexpr (std::is_same<int,T>::value)
    {
      return VA==VB;
    }
  
  // Zero test:
  if (std::abs(VA)<tol*tol && std::abs(VB)<tol*tol)
    return 1;
  return (std::abs(VA-VB)/(std::abs(VA)+std::abs(VB))<tol);
}

template<typename T>
bool
intervalVal(const double tol,const T& VA,
		       const T& VB,const T& VC) 
  /*!
    Determine if the values are equal within the tolerance
    \param T :: Tolerance
    \param VA :: Value 1
    \param VB :: Mid value
    \param VC :: End
    \return 1 if a valid interval
  */
{
  return identVal<double>(tol,static_cast<double>(VB),
			  static_cast<double>(VA+VC)/2.0);
}


template<>
bool
logIntVal(const double tol,const double& VA,
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
  const double tol3(tol*tol*tol);

  if (VA<=tol3 || VB<=tol3 || VC<=tol3 || VC<=VA)
    return 0;
  if ((VC-VA)/(VA+VC)<tol) return 0;

  return identVal(tol,log(VB),(log(VA)+log(VC))/2.0);
}

template<>
bool
logIntVal(const double,const int&,
		       const int&,const int&) 
  /*!
    Determine if the values are in a log range within the tolerance
    \param tol :: Tolerance
    \param VA :: Value 1
    \param VB :: Mid value
    \param VC :: End
    \return 1 if a valid interval
  */
{
  return 0;
}
  
template<typename T>  
RSingle<T>::RSingle(const T& V) :
  RUnit<T>(),value(V)
  /*!
    Value constructor
    \param V :: single value
  */
{}

template<typename T>  
RSingle<T>::RSingle(const RSingle<T>& A) :
  RUnit<T>(A),
  value(A.value)
  /*!
    Copy constructor
    \param A :: single item
  */
{}

template<typename T>  
RSingle<T>&
RSingle<T>::operator=(const RSingle<T>& A)
  /*!
    Assignment operator
    \param A :: RSingle to copy
    \return *this
  */
{
  if (this!=&A)
    {
      RUnit<T>::operator=(A);
      value=A.value;
    }
  return *this;
}

template<typename T>  
void
RSingle<T>::writeVector(std::vector<T>& vItems) const
  /*!
    Write single item
   */
{
  vItems.push_back(value);
  return;
}

template<typename T>  
void
RSingle<T>::write(std::ostream& OX) const
  /*!
    Write single item
   */
{
  OX<<" "<<value;
  return;
}

// ---------------------------------------------------------
  
template<typename T>  
RRepeat<T>::RRepeat(const T& V,const int N) :
  RUnit<T>(),value(V),count(N)
  /*!
    Value constructor
    \param V :: single value
  */
{}

template<typename T>  
RRepeat<T>::RRepeat(const RRepeat<T>& A) :
  RUnit<T>(A),value(A.value),count(A.count)
  /*!
    Copy constructor
    \param A :: single item
  */
{}

template<typename T>  
RRepeat<T>&
RRepeat<T>::operator=(const RRepeat<T>& A)
{
  if (this!=&A)
    {
      RUnit<T>::operator=(A);
      value=A.value;
    }
  return *this;
}

template<typename T>  
void
RRepeat<T>::writeVector(std::vector<T>& vItems) const
  /*!
    Write repeated item to vector
   */
{
  if (count && RUnit<T>::isFirst)
    vItems.push_back(value);
  for(int i=0;i<count-1;i++)
    vItems.push_back(value);

  return;
}
  
template<typename T>  
void
RRepeat<T>::write(std::ostream& OX) const
  /*!
    Write repeat item
  */
{
  if (count && RUnit<T>::isFirst)
    OX<<" "<<value;
  if (count>2)
    OX<<" "<<count-1<<"r";
  else
    OX<<" "<<value;
  return;
}


// ---------------------------------------------------------
  
template<typename T>  
RInterval<T>::RInterval(const T& VA,const T& VB,const int N) :
  RUnit<T>(),aValue(VA),bValue(VB),count(N)
  /*!
    Value constructor
    \param VA :: first value
    \param VB :: last value
    \param count :: total number in intervale 
  */
{}

template<typename T>  
RInterval<T>::RInterval(const RInterval<T>& A) :
  RUnit<T>(A),aValue(A.aValue),bValue(A.bValue),
  count(A.count)
  /*!
    Copy constructor
    \param A :: interval item
  */
{}

template<typename T>  
RInterval<T>&
RInterval<T>::operator=(const RInterval<T>& A)
  /*!
    Assignement operator
  */
{
  if (this!=&A)
    {
      RUnit<T>::operator=(A);
      aValue=A.aValue;
      bValue=A.bValue;
      count=A.count;
    }
  return *this;
}

template<typename T>
T
RInterval<T>::getItem(const int index) const
  /*!
    Get the value at index 
    \param index :: Index value
  */
{
  ELog::RegMethod RegA("RInterval","getItem");

  if (index<static_cast<int>(index>=count))
    throw ColErr::IndexError<int>(index,count,"index");
  
  
  const T step=(bValue-aValue)/static_cast<T>(count-1);
  return aValue+index*step;
}
  
template<typename T>  
void
RInterval<T>::writeVector(std::vector<T>& vItems) const
  /*!
    Write repeated item to vector
   */
{
  if (count && RUnit<T>::isFirst)
    vItems.push_back(aValue);
  if (count>2)
    {
      const T step=(bValue-aValue)/static_cast<T>(count);
      T AV(aValue);
      for(int i=0;i<count-1;i++)
	{
	  AV+=step;
	  vItems.push_back(AV);
	}
    }
  vItems.push_back(bValue);  
  return;
}

template<typename T>  
void
RInterval<T>::write(std::ostream& OX) const
  /*!
    Write repeat item
   */
{
  if (count)
    {
      if (RUnit<T>::isFirst)
	OX<<" "<<aValue;
      if (count>2)
	OX<<" "<<count-1<<"i";
      OX<<" "<<bValue;
    }
  return;
}


/// ------------------------------------------------------------------

template<typename T>  
RLog<T>::RLog(const T& VA,const T& VB,const int N) :
  RUnit<T>(),aValue(VA),bValue(VB),count(N)
  /*!
    Value constructor
    \param VA :: first value
    \param VB :: last value
    \param count :: total number in intervale 
  */
{}

template<typename T>  
RLog<T>::RLog(const RLog<T>& A) :
  RUnit<T>(A),aValue(A.aValue),bValue(A.bValue),
  count(A.count)
  /*!
    Copy constructor
    \param A :: interval item
  */
{}

template<typename T>  
RLog<T>&
RLog<T>::operator=(const RLog<T>& A)
  /*!
    Assignement operator
  */
{
  if (this!=&A)
    {
      RUnit<T>::operator=(A);
      aValue=A.aValue;
      bValue=A.bValue;
      count=A.count;
    }
  return *this;
}

template<typename T>
T
RLog<T>::getItem(const int index) const
  /*!
    Get the value at index 
  */
{
  ELog::RegMethod RegA("RLog","getItem");

  if (index<static_cast<int>(index>=count))
    throw ColErr::IndexError<int>(index,count,"index");
  
  const double aLog=std::log(static_cast<double>(aValue));
  const double bLog=std::log(static_cast<double>(bValue));
  const double step=(bLog-aLog)/static_cast<double>(count);

  return static_cast<T>(std::exp(aLog+index*step));
}
  
template<typename T>  
void
RLog<T>::writeVector(std::vector<T>& vItems) const
  /*!
    Write repeated item to vector
    \param vItem :: Vector for output
   */
{
  if (count && RUnit<T>::isFirst)
    vItems.push_back(aValue);
  if (count>2)
    {
      const double aLog=std::log(static_cast<double>(aValue));
      const double bLog=std::log(static_cast<double>(bValue));
      const double step=(bLog-aLog)/static_cast<double>(count);
      double AV(aLog);
      for(int i=0;i<count-1;i++)
	{
	  AV+=step;
	  vItems.push_back(static_cast<T>(std::exp(AV)));
	}
    }
  vItems.push_back(bValue);  
  return;
}

template<typename T>  
void
RLog<T>::write(std::ostream& OX) const
  /*!
    Write repeat item
   */
{
  if (count)
    {
      if (RUnit<T>::isFirst)
	OX<<" "<<aValue;
      if (count>2)
	OX<<" "<<count-1<<"log";
      OX<<" "<<bValue;
    }
  return;
}



  
template<typename T>
NGroup<T>::NGroup() 
  /*!
    Default constructor
  */
{}

template<typename T>
NGroup<T>::NGroup(const NGroup<T>& A)
  /*!
    Copy Constructor
    \param A :: NGroup object to copy
  */
{
  for(const RUnit<T>* RPtr : A.Items)
    Items.push_back(RPtr->clone());
}

template<typename T>
NGroup<T>&
NGroup<T>::operator=(const NGroup<T>& A)
  /*!
    Assignment operator=
    \param A :: Object to copy
    \return *this
  */
{
  if(&A!=this)
    {
      clearItems();
      for(const RangeUnit::RUnit<T>* AUnit : A.Items)
	{
	  Items.push_back(AUnit->clone());
	}
    }
  return *this;
}

template<typename T>
NGroup<T>::~NGroup()
  /*!
    Destructor
  */
{
  clearItems();
}

template<typename T>
T
NGroup<T>::operator[](const int Index) const
  /*!
    Gets an individual value
    \param Index :: Index value 
    \return values
  */
{
  ELog::RegMethod RegA("NGroup","operator[]");

  if (Index<0)
    throw ColErr::IndexError<int>(Index,0,"Index");
  
  int cnt(0);
  typename std::list<RUnit<T>>::const_iterator vc;

  for(const RUnit<T>* unit : Items)
    {
      const int ISize=unit->getSize();
      if (cnt+ISize>Index)
	return unit->getItem(Index-cnt);
      cnt+=ISize;
    }
  throw ColErr::IndexError<int>(Index,cnt,"Index");
}

template<typename T>
NGroup<T>&
NGroup<T>::operator+=(const NGroup<T>& A)
  /*!
    Add two ranges (append A to the back of this)
    \param A :: Object to add
    \return *this
  */
{
  // add extra items on end :
  // could do a merge check (?)
  for(const RUnit<T>* unit : A.Items)
    Items.push_back(unit->clone());

  return *this;
}

template<typename T>
void
NGroup<T>::clearItems()
  /*!
    Remove items and clear list
   */
{
  for(RangeUnit::RUnit<T>* unit : Items)
    delete unit;
  Items.clear();
}
  
template<typename T>
int
NGroup<T>::count() const
  /*!
    Count total in system
    \return total count
  */
{
  int cnt(0);
  for(const RUnit<T>* unit : Items)
    {
      cnt+=unit->getSize();
    }
  return cnt;
}





template<typename T>
int 
NGroup<T>::processString(const std::string& N)
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
    \retval -2 :: Unable to process nubmer
  */
{
  if(N.empty())
    return -1;
  // Work with part of the string first

  std::string MS(N);
  
  transform(MS.begin(),MS.end(),MS.begin(),std::ptr_fun(&tolower));

  // From now on in we have a string type (definately)
  // check if the next number is a number or doesn't
  // exist.
  T AValue(0);
  T BValue(0);
  std::string Comp;
  int index;

  clearItems();

  bool tail(0);
  while(!StrFunc::isEmpty(MS))  // all done
    {
      while(StrFunc::section(MS,BValue))
	{
	  if (tail)
	    Items.push_back(new RSingle<T>(AValue));
	  tail=1;
	  AValue=BValue;
	}
      if (StrFunc::section(MS,Comp))
	{
	  if (StrFunc::sectPartNum(Comp,index))  // must be 10log etc.
	    {
	      if (tail)
		{
		  Items.push_back(new RSingle<T>(AValue));
		  tail=0;
		}
	      if (Comp=="log" && StrFunc::section(MS,BValue))
		{
		  Items.push_back(new RLog<T>(AValue,BValue,index+1));
		  AValue=BValue;
		}
	      else if (Comp=="i" && StrFunc::section(MS,BValue))
		{
		  Items.push_back(new RInterval<T>(AValue,BValue,index+1));
		  AValue=BValue;
		}
	      else if (Comp=="r")
		{
		  Items.push_back(new RRepeat<T>(AValue,index+1));
		}
	    }
	}
    } 
  if (tail)
    Items.push_back(new RSingle<T>(AValue));
  
  Items.front()->setFirstFlag(1);  
  return 0;
}



template<typename T>
void
NGroup<T>::writeVector(std::vector<T>& Vec) const
  /*!
    Takes the vector and writes it into the NGroup.
    \param Vec :: Vector use
  */
{
  ELog::RegMethod RegA("NGroup","writeVector");

  for(const RUnit<T>* RPtr  : Items)
    RPtr->writeVector(Vec);
  return;
}

template<typename T>
void
NGroup<T>::condense(const double Tol)
  /*!
    Determine intervals 
    \param Tol :: Tolerance value
  */
{
  ELog::RegMethod RegA("NGroup","condence");

  std::vector<T> Values;
  writeVector(Values);
  if (Values.size()>2)
    condense(Tol,Values);
  return;
}


template<typename T>
void
NGroup<T>::condense(const double Tol,
		    const std::vector<T>& Values)
  /*!
    Determine intervals 
    \param Tol :: Tolerance value
    \param Values :: Values to use
  */
{

  ELog::RegMethod RegA("NGroup","condence(vec)");

  clearItems();
  if (Values.size()<3)
    {
      for(const T& V : Values)
	Items.push_back(new RSingle<T>(V));
      return;
    }
  
  std::list<RUnit<T>*> OutList;

  // Extra value here a guard item
  std::vector<int> type(Values.size()+1,0);
  type[0]=0;
  for(size_t i=1;i<Values.size();i++)
    {
      // Basic repeat:
      if (identVal(Tol,Values[i],Values[i-1]))
	type[i]=1;
      else if (i>1 && intervalVal(Tol,Values[i-2],Values[i-1],Values[i]))
	{
	  ELog::EM<<"In value == "<<Values[i-2]<<ELog::endDiag;
	  type[i-1]=2;    // linear interval
	  type[i]=2;    // linear interval
	}
      else if (i>1 && logIntVal(Tol/100.0,Values[i-2],Values[i-1],Values[i]))
	{
	  type[i-1]=3;   // log interval
	  type[i]=3;   // log interval
	}
    }

  T AValue(Values[0]);
  size_t cnt(0);
  const size_t VSize(Values.size());
  while(cnt<VSize)
    {
      size_t repCnt=1;
      if (type[cnt]==0)
	{
	  OutList.push_back(new RSingle<T>(Values[cnt]));
	  AValue=Values[cnt];
	}
      else 
	{
	  // count repeats/interval/
	  for(repCnt=1;type[cnt+repCnt]==type[cnt];repCnt++) ;
	  if (type[cnt]==3)  // log
	    {
	      OutList.push_back
		(new RLog(AValue,Values[cnt+repCnt-1],
			  static_cast<int>(repCnt)));
	    }
	  if (type[cnt]==2)  // interval
	    {
	      OutList.push_back
		(new RInterval(AValue,Values[cnt+repCnt-1],
			       static_cast<int>(repCnt)));
	    }
	  if (type[cnt]==1)  // repeat
	    {
	      OutList.push_back
		(new RRepeat(AValue,static_cast<int>(repCnt+1)));
	    }
	  AValue=Values[cnt+repCnt-1];
	}
      cnt+=repCnt;
    }

  Items=std::move(OutList);
  return;
}

template<typename T>
void
NGroup<T>::write(std::ostream& OX) const
  /*!
    Write out the Range to a stream
    \param OX :: string stream to write out
  */
{
  for(const RUnit<T>* RPtr : Items)
    RPtr->write(OX);
  return;
}

///\cond TEMPLATE

template class RSingle<double>;
template class RInterval<double>;
template class RRepeat<double>;
template class RLog<double>;
template class NGroup<double>;

template class RSingle<int>;
template class RInterval<int>;
template class RRepeat<int>;
template class RLog<int>;
template class NGroup<int>;

template std::ostream&
operator<<(std::ostream&,const NGroup<double>&);

template std::ostream&
operator<<(std::ostream&,const NGroup<int>&);

///\endcond TEMPLATE


}  // NAMESPACE RangeUnit
		    
