/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
  * File:   src/multiIndex.cxx
 *
 * Copyright (c) 2004-2025 by Stuart Ansell
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

#include "Exception.h"
#include "multiIndex.h"


std::ostream&
operator<<(std::ostream& OX,const multiIndex& A) 
  /*!
    Output stream assesor
    \param OX :: Output stream
    \param A :: multiIndex to writeout
    \return output stream
   */
{
  A.write(OX);
  return OX;
} 

multiIndex::multiIndex(const size_t nA,const size_t nB) :
  currentIndex(0),
  stride({nB,1,0}),
  Rmin({0,0}),
  Rmax({nA-1,nB-1}),
  RC({0,0})
  /*!
    Simple constructor with fixed 2d system
    \param nA :: First index
    \param nB :: Second index
  */
{}

multiIndex::multiIndex(const size_t nA,const size_t nB,
		       const size_t nC) :
  currentIndex(0),
  stride({nB*nC,nC,1,0}),
  Rmin({0,0,0}),Rmax({nA-1,nB-1,nC-1}),RC({0,0,0})
  /*!
    Simple constructor with fixed 3d system
    \param nA :: First index
    \param nB :: Second index
    \param nC :: Second index
  */
{}

multiIndex::multiIndex(const size_t axisIndex,
		       const size_t unitIndex,
		       const std::vector<size_t>& index) :
  currentIndex(0),
  stride(index.size()+1),
  Rmin(index.size()),
  Rmax(index),
  RC(index.size())
  /*!
    Simple constructor with size based on input vector
    Note that Rmax holds max valid value so input is +1 value
  */
{
  if (axisIndex>=index.size() ||
      unitIndex>=index[axisIndex])
    throw ColErr::ArrayError
      (axisIndex,unitIndex,index,"multiIndex dimension");

  stride[index.size()]=0;   // always valid
  size_t prod=1;
  for(size_t i=index.size();i>0;i--)
    {
      stride[i-1]=prod;
      prod*=index[i-1];
    }

  // remove one as must have a valid max value
  for(size_t& I : Rmax)
    I--;

  Rmin[axisIndex]=unitIndex;
  Rmax[axisIndex]=unitIndex;
  RC[axisIndex]=unitIndex;
  if (axisIndex==0)
    currentIndex=stride[0]*unitIndex;
}

bool
multiIndex::operator==(const multiIndex& A) const
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

bool
multiIndex::operator>(const multiIndex& A) const 
  /*! 
    Determines the precidence of the multiIndexs
    Operator works on the 0 to high index 
    \param A :: multiIndex to compare
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

bool
multiIndex::operator<(const multiIndex& A) const
  /*! 
    Determines the precidence of the multiIndexs
    Operator works on the 0 to high index 
    \param A :: multiIndex to compare
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

size_t
multiIndex::operator++(int)
  /*!
    Convertion to ++operator (prefix) 
    from operator++ (postfix)
    \param  :: Dummy argument
    \return number of steps forward [0 for return loop]
   */
{
  return this->operator++();
}

size_t
multiIndex::calcIndex() const
  /*!
    Calc the position index
    \return offset index value
   */
{
  size_t cIndex=RC[0]*stride[0];
  for(size_t i=1;i<RC.size();i++)
    cIndex+=RC[i]*stride[i];
  return cIndex;
}

size_t
multiIndex::operator++()
  /*!
    Carrys out a rotational addition.
    Objective is a rolling integer stream ie 1,2,3
    going to 1,2,N-1 and then 1,3,4 etc...
    \return Number os strep forward
  */
{
  if (RC.empty()) 
    return 0;
  
  size_t I;
  for(I=RC.size();I>0 && RC[I-1]==Rmax[I-1];I--) ;


  if (I) RC[I-1]++;

  if (I==RC.size())
    return ++currentIndex;
	
  
  for(;I<RC.size();I++)
    RC[I]=Rmin[I];

  currentIndex=calcIndex();

  return currentIndex;
}

void
multiIndex::setAxis(const size_t axisIndex,
		    const size_t unitIndex)
  /*!
    Set a fixed index value
  */
{
  if (axisIndex>=RC.size())
    throw ColErr::IndexError<size_t>
      (axisIndex,RC.size(),"Index out of range");
  //  Note :: need to unset previous Rmin ?

  RC[axisIndex]=unitIndex;
  Rmin[axisIndex]=unitIndex;
  Rmax[axisIndex]=unitIndex;

  currentIndex=calcIndex();
  return;
}

void
multiIndex::write(std::ostream& OX) const
{
  for(size_t i=0;i<RC.size();i++)
    OX<<"["<<Rmin[i]<<"-"<<Rmax[i]<<"]="<<RC[i]<<" ";
  
  return;
}
  
