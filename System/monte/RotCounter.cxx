/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   monte/RotCounter.cxx
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
#include <algorithm>
#include <iterator>
#include <vector>

#include "RotCounter.h"

template<typename T>
std::ostream&
operator<<(std::ostream& OX,const RotaryCounter<T>& A) 
  /*!
    Output stream assesor
    \param OX :: Output stream
    \param A :: RotaryCounter to writeout
    \return output stream
   */
{
  A.write(OX);
  return OX;
} 

template<typename T>
RotaryCounter<T>::RotaryCounter(const size_t S,const T& N) :
  Rmax(N),RC(S)
  /*!
    Simple constructor with fixed size and number.
    Fills RC with a flat 0->N number list
    \param S :: Size  (number of components)
    \param N :: Max number to get to
  */
{
  for(size_t i=0;i<S;i++)
    RC[i]=static_cast<T>(i);
}

template<typename T>
RotaryCounter<T>::RotaryCounter(const RotaryCounter<T>& A) :
  Rmax(A.Rmax),RC(A.RC)
  /*!
    Standard copy constructor
    \param A :: Object to copy
  */ 
{ }

template<typename T>
RotaryCounter<T>&
RotaryCounter<T>::operator=(const RotaryCounter<T>& A)
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

template<typename T>
RotaryCounter<T>::~RotaryCounter()
  /*!
    Standard Destructor
  */
{}

template<typename T>
bool
RotaryCounter<T>::operator==(const RotaryCounter<T>& A) const
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

template<typename T>
bool
RotaryCounter<T>::operator>(const RotaryCounter<T>& A) const 
  /*! 
    Determines the precidence of the RotaryCounters
    Operator works on the 0 to high index 
    \param A :: RotaryCounter to compare
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

template<typename T>
bool
RotaryCounter<T>::operator<(const RotaryCounter<T>& A) const
  /*! 
    Determines the precidence of the RotaryCounters
    Operator works on the 0 to high index 
    \param A :: RotaryCounter to compare
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

template<typename T>
bool
RotaryCounter<T>::operator++(int)
  /*!
    Convertion to ++operator (prefix) 
    from operator++ (postfix)
    \param  :: Dummy argument
    \return ++operator
   */
{
  return this->operator++();
}

template<typename T>
bool
RotaryCounter<T>::operator++()
  /*!
    Carrys out a rotational addition.
    Objective is a rolling integer stream ie 1,2,3
    going to 1,2,N-1 and then 1,3,4 etc...
    \retval 1 :: the function has looped (carry flag)
    \retval 0 :: no loop occored
  */
{
  if (RC.empty()) 
    return 0;
						 
  T Npart=Rmax-1;
  size_t I;
  for(I=RC.size();I && RC[I-1]==Npart;I--,Npart--) ;
  
  if (!I)
    {
      for(size_t i=0;i<RC.size();i++)
        RC[i]=static_cast<T>(i);
      return 1;
    }
  // I > 0 here: 
  RC[I-1]++;
  for(;I<RC.size();I++)
    RC[I]=RC[I-1]+1;

  return 0;
}

template<typename T>
bool
RotaryCounter<T>::operator--(int)
  /*!
    convertion to --operator (prefix) 
    from operator-- (postfix)
    \param  :: Dummy argument
    \return operator--
   */
{
  return this->operator--();
}

template<typename T>
bool
RotaryCounter<T>::operator--()
  /*!
    Carrys out a rotational addition.
    Objective is a rolling integer stream ie 1,2,3
    going to 1,2,N-1 and then 1,3,4 etc...
    \retval 1 :: the function has looped (carry flag)
    \retval 0 :: no loop occored
  */
{
  const size_t Size(RC.size());

  size_t Index;
  for(Index=RC.size()-1;Index>0 && RC[Index]==RC[Index-1]+1;Index--) ;
  // Loop case
  if(!Index && !RC[0])    
    {
      // In case of loop go to
      for(size_t i=0;i<RC.size();i++)
	RC[i]=Rmax-static_cast<T>(Size-i);
      return 1;
    }
  
  RC[Index]--;
  for(Index++;Index<Size;Index++)
    RC[Index]=Rmax-static_cast<T>(Size-Index);
  return 0;
}

template<typename T>
void
RotaryCounter<T>::write(std::ostream& OX) const
  /*!
    Write out object to a stream
    \param OX :: output stream
  */
{
  OX<<" ";
  copy(RC.begin(),RC.end()-1,std::ostream_iterator<int>(OX,":"));
  OX<<RC.back()<<" ";
  return;
}

template class RotaryCounter<size_t>;
template class RotaryCounter<int>;

template std::ostream& operator<<(std::ostream&,const RotaryCounter<size_t>&);
template std::ostream& operator<<(std::ostream&,const RotaryCounter<int>&);
