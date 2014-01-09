/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   support/cycIterator.cxx
*
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#include <cmath>
#include <complex>
#include <vector>
#include <iterator>

#include "Exception.h"
#include "cycIterator.h"

namespace RMCbox
{
  class Atom;
}

template<typename T,typename Iterator> 
cycIterator<T,Iterator>::cycIterator(const Iterator& A,const Iterator& B) :
  loopIndex(0),beginIter(A),endIter(B),vc(A)
{}

template<typename T,typename Iterator> 
cycIterator<T,Iterator>::cycIterator(const cycIterator<T,Iterator>& A) : 
  loopIndex(A.loopIndex),beginIter(A.beginIter),
  endIter(A.endIter),vc(A.vc)
{}


template<typename T,typename Iterator> 
cycIterator<T,Iterator>::~cycIterator()
  /*!
    Destructor
  */
{}

template<typename T,typename Iterator> 
cycIterator<T,Iterator>
cycIterator<T,Iterator>::operator++(int) 
  /*!
    Simple addor
   */
{
  cycIterator<T,Iterator> out(*this);
  ++*this;
  return out;
}

template<typename T,typename Iterator> 
cycIterator<T,Iterator>&
cycIterator<T,Iterator>::operator++()
  /*!
    Simple addition operator [pre adder]
    \return iterator
  */
{
  vc++;
  if (vc==endIter)
    {
      loopIndex++;
      vc=beginIter;
    }
  return *this;
}

/// \cond TEMPLATE

template class cycIterator<RMCbox::Atom*,
			   std::vector<RMCbox::Atom*>::const_iterator>;
  // template class Matrix<int>;

/// \endcond TEMPLATE
