/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   monte/BnId.cxx
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
#include <functional>
#include <iterator>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "BnId.h"


/// Setter form
int BnId::fullOut(0);

std::ostream&
operator<<(std::ostream& OX,const BnId& A) 
  /*!
    Output stream operator
    \param OX :: Output stream 
    \param A :: Object to write
    \return Stream 
   */
{
  A.write(OX);
  return OX;
}

BnId::BnId() :
  size(0),PI(1),Tnum(0),Znum(0)
  /*!
    Standard Constructor
  */
{}

BnId::BnId(const size_t TSize,const size_t X) :
  size(TSize),PI(1),Tnum(0),Znum(0),Tval(TSize)
  /*!
    Constructer that creates a true/false mapping
    without the  undetermined option
    \param TSize :: size of the vector Tval. (number of surfaces)
    \param X :: integer for of the binary representation 
  */
{
  size_t cnt=1;
  long int sum(0);
  for(size_t i=0;i<size;cnt*=2,i++)
    { 
      Tval[i] = (X & cnt) ? 1 : -1;
      sum+=Tval[i];
    }
  // counts number of true numbers
  Tnum=static_cast<size_t>((sum+static_cast<long>(size))/2);
  Znum=size-Tnum;
}

BnId::BnId(const BnId& A) :
  size(A.size),PI(A.PI),Tnum(A.Tnum),
  Znum(A.Znum),Tval(A.Tval),MinTerm(A.MinTerm)
  /*!
    Standard Copy Constructor
    \param A :: Object to copy
  */
{}

BnId&
BnId::operator=(const BnId& A) 
  /*!
    Assignment operator
    \param A :: object to copy
    \returns *this
  */
{
  if (this!=&A)
    {
      size=A.size;
      PI=A.PI;
      Tnum=A.Tnum;
      Znum=A.Znum;
      Tval=A.Tval;
      MinTerm=A.MinTerm;
    }
  return *this;
}

BnId::~BnId()
  /*!
    Destructor
   */
{}

int
BnId::operator==(const BnId& A) const
  /*!
    Tri-state return of the equality 
    \param A :: BnId object to compare
    \retval 1 == absolutely identical
    \retval 0 == not equal 
  */
{
  if (A.size!=size || A.Tnum!=Tnum
      || A.Znum!=Znum)
    return 0;
  std::vector<int>::const_iterator vc;
  std::vector<int>::const_iterator ac=A.Tval.begin();
  for(vc=Tval.begin();vc!=Tval.end();vc++,ac++)
    {
      if (ac==A.Tval.end())     // This should neve happen
	return 0;
      if (*vc!=*ac)
	return 0;
    }
  return 1;
}

int
BnId::equivalent(const BnId& A) const
  /*!
    Tri-state return of the equality 
    \param A :: BnId object to compare
    \retval 1 == absolutely identical
    \retval 2 == identical but via dc (don't care)
    \retval 0 == not equal 
  */
{
  if (A.size!=size)
    return 0;
  int retval=1;
  for(size_t i=0;i<size;i++)
    {
      if (Tval[i]*A.Tval[i]<0)          //true * false == -1.
        return 0;
      if (Tval[i]!=A.Tval[i]) 
	retval=2;
    }
  return retval;
}

int 
BnId::operator>(const BnId& A) const
  /*!
    Tri-state return of the ordering of number of true
    \param A :: BnId object to compare
    \returns !(this<A)
  */
{
  return !(A < *this);
}

int
BnId::operator<(const BnId& A) const
  /*!
    Tri-state return of the ordering of number of true states
    \param A :: BnId object to compare
    \returns Size<A.size, N of True<A.N of True,
       IntVal<A.IntVal
  */
{
  if (A.size!=size)
    return size<A.size;
  std::pair<int,int> cntA(0,0);   // count for A
  std::pair<int,int> cntT(0,0);   // count for this
  if (Znum!=A.Znum)
    return (Znum<A.Znum) ? 1 : 0;

  if (Tnum!=A.Tnum)
    return (Tnum<A.Tnum) ? 1 : 0;

  std::vector<int>::const_reverse_iterator tvc=Tval.rbegin();
  std::vector<int>::const_reverse_iterator avc=A.Tval.rbegin();
  while(tvc!=Tval.rend())
    {
      if (*tvc!=*avc)
	return *tvc<*avc;
      tvc++;
      avc++;
    }
  return 0;
}

int
BnId::operator[](const size_t A) const
  /*!
    Returns the particular rule value
    \param A :: array offset 0->size-1
    \returns -99 on err, Tval[A] normally
  */
{
  if (A>=size)
    throw ColErr::IndexError<size_t>(A,size,"BnId::operator[]"+
				     ELog::RegMethod::getFull());
  return Tval[A];
}

int 
BnId::operator++(int)
  /*!
    convertion to ++operator (prefix) 
    from operator++ (postfix)
    \retval 0 :: the function has looped (carry flag)
    \retval 1 :: no loop occored
   */
{
  return this->operator++();
}

int
BnId::operator++()
  /*!
    Addition operator
    Carrys out a rotational addition (effective binary
    addition. It ignores non-important flags (Tval[i]==0)
    \retval 0 :: the function has looped (carry flag)
    \retval 1 :: no loop occored
  */
{
  std::vector<int>::iterator vc;
  for(vc=Tval.begin();vc!=Tval.end() && (*vc)!=-1;vc++)
    {
      if (*vc==1)
	{
	  Tnum--;
	  *vc= -1;
	}
    }
  if (vc==Tval.end())
    return 0;
  // Normal exit
  *vc=1;
  Tnum++;
  return 1;
}

int 
BnId::operator--(const int)
  /*!
    Convertion to --operator (prefix) 
    from operator-- (postfix)
    \retval 0 :: the function has looped (carry flag)
    \retval 1 :: no loop occored
   */
{
  return this->operator--();
}

int
BnId::operator--()
  /*!
    Subtraction operator
    Carrys out a rotational subtraction (effective binary
    addition. It ignores non-important flags (Tval[i]==0)
    \retval 0 :: the function has looped (carry flag)
    \retval 1 :: no loop occored
  */
{
  std::vector<int>::iterator vc;
  for(vc=Tval.begin();vc!=Tval.end() && (*vc)!=1;vc++)
    if (*vc== -1)
      {
	*vc= 1;
	Tnum++;
      }
  if (vc==Tval.end())       // Loop took place
    return 0;

  *vc= -1;
  Tnum--;
  return 1;
}

void
BnId::setMinTerm(const int MT) 
  /*!
    Set the MinTerm to a single value
    \param MT :: Value to set
   */
{
  MinTerm.erase(MinTerm.begin(),MinTerm.end());
  MinTerm.insert(MT);
  return;
}
    

void
BnId::addMinTerm(const BnId& A) 
  /*!
    Add just the minTers together
    \param A :: BnId item to associate
   */
{
  MinTerm.insert(A.MinTerm.begin(),A.MinTerm.end());
  return;
}

int
BnId::hasMinTerm(const int Index) const
  /*!
    Determine in Index is in the set
    \param Index :: Index value
    \return True if index is found
   */
{
  std::set<int>::const_iterator sv=MinTerm.find(Index);
  return (sv==MinTerm.end()) ? 0 : 1;
}

void
BnId::setCounters()           
  /*!
    Sets the counters Tnum and Znum
  */
{
  std::vector<int>::const_iterator vc;
  Tnum=0;
  Znum=0;
  for(vc=Tval.begin();vc!=Tval.end();vc++)
    {
      if (*vc==1)
	Tnum++;
      else if (*vc==0)
	Znum++;
    }
  return;
}

size_t
BnId::intValue() const
  /*!
    Returns the lowest int value associated with 
    the string 
    \returns lowest bit in the BnId vector
  */
{
  size_t out(0);
  std::vector<int>::const_reverse_iterator vc;
  for(vc=Tval.rbegin();vc!=Tval.rend();vc++)
    {
      out<<=1;
      out+=((*vc)==1) ? 1 : 0;
    }
  return out;
}

void
BnId::mapState(const std::vector<int>& Index,
	       std::map<int,int>& Base) const
  /*!
    Sets the components within base with true/false
    \param Index :: vector of Literal/Surface numbers
    \param Base :: map to be filled
  */ 
{
  for(size_t i=0;i<Index.size();i++)
    Base[Index[i]]=(Tval[i]==1) ? 1 : 0;    

  return;
}



std::pair<int,BnId>
BnId::makeCombination(const BnId& A) const
  /*!
    Find if A and this can be differ by one
    1/-1 bit and make a 0 value for that bit
    \param A :: value to check
    Output 
      -  0 :: nothing to do.
      -  1 :: complement found
      - -1 :: items are too differnt
    \returns pair<status,BnId> of possible new PI
    BnId is valid only if status ==1
  */
{

  if (size!=A.size)   // sizes different
    return std::pair<int,BnId>(-1,BnId());

  // Zero unequal or 1 value to far apart
  if (Znum!=A.Znum || (Tnum-A.Tnum)*(Tnum-A.Tnum)>1)
    return std::pair<int,BnId>(-1,BnId());

  // no difference 
  if (Tnum==A.Tnum)
    return std::pair<int,BnId>(0,BnId());

  int flag(0);         // numb of diff
  std::pair<int,int> Tcnt(0,0);        // this counter 
  std::pair<int,int> Acnt(0,0);        // A counter 
  std::vector<int>::const_iterator tvc;
  std::vector<int>::const_iterator avc=A.Tval.begin();
  std::vector<int>::const_iterator chpt;   // change point
  for(tvc=Tval.begin();tvc!=Tval.end();tvc++,avc++)
    {
      if ((*avc * *tvc)<0)       // false/true
	{
	  if (flag)           // failed
	    return std::pair<int,BnId>(0,BnId());;
	  flag=1;  //inc change counter
	  chpt=tvc;
	}
      else if (*avc!= *tvc)       // failed on a 0 : value
	return std::pair<int,BnId>(0,BnId());;
    }
  // Good value
  if (flag)
    {
      BnId PIout(*this);
      PIout.Tval[static_cast<size_t>(chpt-Tval.begin())]=0;
      PIout.setCounters();
      PIout.addMinTerm(A);
      return std::pair<int,BnId>(1,PIout);
    }
  // Nothing to do.
  return std::pair<int,BnId>(0,BnId());
}

void
BnId::reverse() 
  /*!
    Reverset the bits.
    Transform 1 -> -1
  */
{
  transform(Tval.begin(),Tval.end(),Tval.begin(),
	    std::bind2nd(std::multiplies<int>(),-1));
  return;
}

std::string
BnId::display() const
  /*!
    Simple display to string
    0 == false, - == any, 1 == true
    \returns String value of true etc
   */
{
  std::string Out;
  std::vector<int>::const_reverse_iterator vc;
  std::ostringstream cx;
  for(vc=Tval.rbegin();vc!=Tval.rend();vc++)
    {
      if (*vc==0)
	Out+="-";
      else if (*vc==1)
	Out+="1";
      else
	Out+="0";
    }

  if (fullOut)
    {
      cx<<"[";
      copy(MinTerm.begin(),MinTerm.end(),
	   std::ostream_iterator<int>(cx,","));
      cx<<"]("<<Tnum<<":"<<Znum<<")";
    }
  return Out+cx.str();
}

void
BnId::write(std::ostream& OX) const
  /*!
    Write out to the stream
    \param OX :: Output stream
   */
{
  OX<<display();
  return;
}
