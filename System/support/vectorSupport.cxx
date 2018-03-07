/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   support/vectorSupport.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#include <iterator>
#include <iomanip>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <sstream>
#include <cctype>
#include <complex>
#include <vector>
#include <list>
#include <string>
#include <algorithm>
#include <functional>

#include "vectorSupport.h"

/*! 
  \file vectorSupport.cxx 
*/

namespace  stlFunc
{

template<typename T>
void
removeUnit(typename std::vector<T>& V,const T& aValue)
  /*!
    Remove a unit from a vector
    \param V :: Vector
    \param aValue :: value
  */
{
  V.erase(std::remove(V.begin(),V.end(),aValue),V.end());
  return;
}
    
/// \cond TEMPLATE 

template void
removeUnit(std::vector<int>&,const int&);

/// \endcond TEMPLATE 

}  // NAMESPACE stlFunc
