/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   support/IndexCounter.cxx
 *
 * Copyright (c) 2004-2024 by Stuart Ansell
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
#include <algorithm>
#include <iterator>
#include <vector>

#include "IndexCounter.h"

template<typename T,bool UC>
std::ostream&
operator<<(std::ostream& OX,const IndexCounter<T,UC>& A) 
  /*!
    Output stream assesor
    \param OX :: Output stream
    \param A :: IndexCounter to writeout
    \return output stream
   */
{
  A.write(OX);
  return OX;
} 

template<typename T,bool UC>
template<typename InputIter>
IndexCounter<T,UC>::IndexCounter(InputIter iBeg,InputIter iEnd) :
  Rmax(iBeg,iEnd),RC(Rmax.size(),T(0))
  /*!
    Simple constructor for two interators
    \param iBeg :: First iterator
    \param iEnd :: Second iterator
  */
{
  for(T& i : Rmax)
    i--;
}

template<typename T,bool UC>
IndexCounter<T,UC>::IndexCounter(const size_t NP,const T& defValue) :
  Rmax(NP,defValue),RC(NP,0)
  /*!
    Simple constructor with fixed NP object
    \param NP :: number of objects
    \param defValue :: value (all same)
  */
{
  for(T& i : Rmax)
    i--;
}

template<typename T,bool UC>
IndexCounter<T,UC>::IndexCounter(const T& nA,const T& nB,
				 const T& nC) :
  Rmax({nA-1,nB-1,nC-1}),RC({0,0,0})
  /*!
    Simple constructor with fixed 3d system
    \param nA :: First index
    \param nB :: Second index
    \param nC :: Second index
  */
{}


template<typename T,bool UC>
IndexCounter<T,UC>::IndexCounter(std::vector<T> sizeArray) :
  Rmax(std::move(sizeArray)),RC(Rmax.size(),T(0))
  /*!
    Simple constructor with size based on input vector
    Note that Rmax holds max valid value so input is +1 value
  */
{
  for(T& I : Rmax)
    I--;  
}

template<typename T,bool UC>
IndexCounter<T,UC>::IndexCounter(const IndexCounter<T,UC>& A) :
  Rmax(A.Rmax),RC(A.RC)
  /*!
    Standard copy constructor
    \param A :: Object to copy
  */ 
{ }

template<typename T,bool UC>
IndexCounter<T,UC>::IndexCounter(IndexCounter<T,UC>&& A) :
  Rmax(std::move(A.Rmax)),RC(std::move(A.RC))
  /*!
    Standard move constructor
    \param A :: Object to copy
  */ 
{ }

template<typename T,bool UC>
IndexCounter<T,UC>&
IndexCounter<T,UC>::operator=(const IndexCounter<T,UC>& A)
  /*!
    Assignment operator
    \param A :: Object to copy
    \return *this
   */
{
  if (this!=&A)
    {
      Rmax=A.Rmax;
      RC=A.RC;
    }
  return *this;
}

template<typename T,bool UC>
IndexCounter<T,UC>&
IndexCounter<T,UC>::operator=(IndexCounter<T,UC>&& A)
  /*!
    Assignment operator with move
    \param A :: Object to move
    \return *this
   */
{
  if (this!=&A)
    {
      Rmax=std::move(A.Rmax);
      RC=std::move(A.RC);
    }
  return *this;
}

template<typename T,bool UC>
bool
IndexCounter<T,UC>::operator==(const IndexCounter<T,UC>& A) const
  /*!
    Chec to find if Counters identical in ALL respects
    \param A :: Counter to compare
    \retval 1 :: All things identical
    \retval 0 :: Something not the same
  */
{
  if (RC.size()!=A.RC.size())
    return 0;

  for(size_t i=0;i < RC.size();i++)
    if (RC[i]!=A.RC[i])
      return 0;
  return 1;
}

template<typename T,bool UC>
bool
IndexCounter<T,UC>::operator>(const IndexCounter<T,UC>& A) const 
  /*! 
    Determines the precidence of the IndexCounters
    Operator works on the 0 to high index 
    \param A :: IndexCounter to compare
    \return This > A
   */
{
  const size_t maxI(A.RC.size()>RC.size() ? RC.size() : A.RC.size());
  for(size_t i=0;i<maxI;i++)
    if (RC[i]!=A.RC[i])
      return RC[i]>A.RC[i];
  if (A.RC.size()!=RC.size())
    return RC.size()>A.RC.size();
  return 0;
}

template<typename T,bool UC>
bool
IndexCounter<T,UC>::operator<(const IndexCounter<T,UC>& A) const
  /*! 
    Determines the precidence of the IndexCounters
    Operator works on the 0 to high index 
    \param A :: IndexCounter to compare
    \return This < A
   */
{
  const size_t maxI(A.RC.size()>RC.size() ? RC.size() : A.RC.size());
  for(size_t i=0;i<maxI;i++)
    {
      if (RC[i]!=A.RC[i])
	return RC[i]<A.RC[i];
    }
  // Then test size 
  if (A.RC.size()!=RC.size())
    return RC.size()<A.RC.size();
  // Ok everything is equal
  return 0;
}

template<typename T,bool UC>
bool
IndexCounter<T,UC>::operator++(int)
  /*!
    Convertion to ++operator (prefix) 
    from operator++ (postfix)
    \param  :: Dummy argument
    \return ++operator
   */
{
  return this->operator++();
}

template<typename T,bool UC>
bool
IndexCounter<T,UC>::operator++()
  /*!
    Carrys out a rotational addition.
    Objective is a rolling integer stream ie 1,2,3
    going to 1,2,N-1 and then 1,3,4 etc...
    \retval 1 :: the function has looped (carry flag)
    \retval 0 :: no loop occored
  */
{
  if (RC.empty()) 
    return 1;
  size_t I;
  for(I=RC.size();I>0 && RC[I-1]==Rmax[I-1];I--) ;
  
  if (!I)
    {
      for(size_t i=0;i<RC.size();i++)
        RC[i]=T(0);
      return 1;
    }
  // I > 0 here: 
  RC[I-1]++;
  if constexpr (!UC)
    {
      for(;I<RC.size();I++)
	RC[I]=T(0);
    }
  else
    {
      T& tmp=RC[I-1];
      for(;I<RC.size();I++)
	RC[I]=tmp;
    }

  return 0;
}

template<typename T,bool UC>
bool
IndexCounter<T,UC>::operator--(int)
  /*!
    convertion to --operator (prefix) 
    from operator-- (postfix)
    \param  :: Dummy argument
    \return operator--
   */
{
  return this->operator--();
}

template<typename T,bool UC>
bool
IndexCounter<T,UC>::operator--()
  /*!
    Carrys out a rotational addition.
    Objective is a rolling integer stream ie 1,2,3
    going to 1,2,N-1 and then 1,3,4 etc...
    \retval 1 :: the function has looped (carry flag)
    \retval 0 :: no loop occored
  */
{
  size_t I;
  for(I=RC.size();I>0 && RC[I-1]==T(0);I--) ;

  // Loop case
  if(!I)    
    {
      RC=Rmax;
      return 1;
    }

  // I > 0 here:
  RC[I-1]--;
  for(;I<RC.size();I++)
    RC[I]=Rmax[I];
  return 0;
}

template<typename T,bool UC>
void
IndexCounter<T,UC>::write(std::ostream& OX) const
  /*!
    Write out object to a stream
    \param OX :: output stream
  */
{

  for(const T& v : RC)
    OX<<"["<<v<<"]";
  return;
}


///\cond TEMPLATE
template class IndexCounter<size_t,0>;
template class IndexCounter<size_t,1>;
template class IndexCounter<int,0>;
template class IndexCounter<int,1>;

template IndexCounter<size_t,0>::IndexCounter
(std::vector<size_t>::const_iterator,std::vector<size_t>::const_iterator);
template IndexCounter<size_t,1>::IndexCounter
(std::vector<size_t>::const_iterator,std::vector<size_t>::const_iterator);
template IndexCounter<int,0>::IndexCounter
(std::vector<int>::const_iterator,std::vector<int>::const_iterator);
template IndexCounter<int,1>::IndexCounter
(std::vector<int>::const_iterator,std::vector<int>::const_iterator);

template std::ostream& operator<<(std::ostream&,const IndexCounter<size_t>&);
template std::ostream& operator<<(std::ostream&,const IndexCounter<int,0>&);
template std::ostream& operator<<(std::ostream&,const IndexCounter<size_t,1>&);
template std::ostream& operator<<(std::ostream&,const IndexCounter<int,1>&);

///\endcond TEMPLATE
