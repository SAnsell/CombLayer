/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   monte/mapIterator.cxx
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
#include <string>
#include <sstream>
#include <cmath>
#include <vector>
#include <set>
#include <map>
#include <algorithm>

#include "mapIterator.h"

namespace MapSupport
{

mapIterator::mapIterator(const std::set<int>& Active) :
  N(Active.size() ? 2UL<<(Active.size()-1) : 1)
  /*!
    Constructor
    \param Active :: Active set of values
   */
{
  initSet(Active);
}

void
mapIterator::initSet(const std::set<int>& Active) 
  /*!
    Initial a set
    \param Active :: Active set of values
  */
{
  M.erase(M.begin(),M.end());
  keys.clear();
  index=0;

  std::set<int>::const_iterator sc;
  for(sc=Active.begin();sc!=Active.end();sc++)
    {
      const int asc=(*sc>0) ? *sc  : -*sc;
      keys.push_back(asc);
      M.insert(MTYPE::value_type(asc,-1));
    }
  std::sort(keys.begin(),keys.end());
  return;
}

size_t 
mapIterator::binaryToGray(const size_t I)
  /*!
    Simple way to convert a binary value to a gray
    \param I :: Input value
    \param size_t :: Otuptu greay
   */
{
  return (I >> 1) ^ I;
}

size_t 
mapIterator::nextGrayIndex() const
  /*!
    Calculate the next index in the key values that 
    is to change in a gray iteration
    \return key array position
   */
{
  const size_t LA=binaryToGray((index+1) % N);
  const size_t LB=binaryToGray(index);
  return static_cast<size_t>(__builtin_ffsl(static_cast<long>(LA ^ LB))-1);
}

bool
mapIterator::operator++() 
  /*!
    This is the simple ++ operator. It uses a Gray code to effect 
    the minimum number of look ups
    \return false on cycling round
   */
{
  const size_t I=nextGrayIndex();
  MTYPE::iterator mc=M.find(keys[I]);
  mc->second *= -1;
  index++;
  index%=N;
  return (index);
}

int
mapIterator::getItem(const int K) const
  /*!
    Get single item 
    \param K :: Key item
    \return status value
  */
{
  MTYPE::const_iterator mc=M.find(K);
  return (mc!=M.end()) ? mc->second : 0;
}

std::string
mapIterator::status() const
  /*!
    Simple representation of the status of the map
    \return the map as +/-
  */
{
  std::ostringstream cx;
  std::vector<int>::const_iterator vc;
  MTYPE::const_iterator mc;
  for(vc=keys.begin();vc!=keys.end();vc++)
    {
      mc=M.find(*vc);
      cx<<((mc->second>0) ? '+' : '-');
    }
  return cx.str();
}



}  // NAMESPACE mapSUpport

