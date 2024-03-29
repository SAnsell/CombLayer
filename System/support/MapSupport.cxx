/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   support/MapSupport.cxx
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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
#include <map>

#include "MapSupport.h"

namespace MapSupport
{

template<typename T>
bool
iterateBinMap(typename std::map<T,int>& M,
	      const int falseState,
	      const int trueState)
  /*!
    Simple iteratator over a 0/1 state mat.
    The map (M) is assumed to have values of only 0 / 1
    The map is then updated as if it is a REVERSED binary
    number by added 1 to the map from. The carry is returned
    as the output flag (and the map is all 0). 
    \param M :: Map 
    \retval 0  :: next interation
    \retval 1  :: loop iteration
   */
{
  for(typename std::map<T,int>::value_type& mc : M)
    {
      if (mc.second==trueState)
	{
	  // zero and move to next
	  mc.second=falseState;  
	}
      else
	{
	  mc.second=trueState;
	  return 0;  // no more work to do
	}
    }
  return 1;
}

template<typename T>
bool
iterateBinMapLocked(typename std::map<T,int>& M,
		    const T& lockItem,
		    const int falseState,
		    const int trueState)
  /*!
    Simple iteratator over a 0/1 state mat.
    The map (M) is assumed to have values of only 0 / 1
    The map is then updated as if it is a REVERSED binary
    number by added 1 to the map from. The carry is returned
    as the output flag (and the map is all 0). 
    \param M :: Map
    \retval 0  :: next interation
    \retval 1  :: loop iteration
   */
{
  for(typename std::map<T,int>::value_type& mc : M)
    {
      if (mc.first!=lockItem)
	{
	  if (mc.second==trueState)
	    {
	      // zero and move to next
	      mc.second=falseState;  
	    }
	  else
	    {
	      mc.second=trueState;
	      return 0;  // no more work to do
	    }
	}
    }
  return 1;
}

template<typename TA,typename TB>
TB
findDefVal(const std::map<TA,TB>& map,
	   const TA& key,const TB& defVal)
  /*!
    Return a value from a map / or the def Value
    if the key is not found
    \param map :: map to search
    \param key :: key to search for
    \param defVal :: value to add
   */
{
  typename std::map<TA,TB>::const_iterator mc=
    map.find(key);
  return (mc!=map.end()) ? mc->second : defVal;
}
  


///\cond TEMPLATE  
template 
bool iterateBinMap(std::map<int,int>&,const int,const int);

template 
bool iterateBinMapLocked(std::map<int,int>&,const int&,
			 const int,const int);

template
double
findDefVal(const std::map<std::string,double>&,
	   const std::string&,const double&);

template
double
findDefVal(const std::map<int,double>&,
	   const int&,const double&);

  ///\endcond TEMPLATE  

  
} // NAMESPACE MapSupport
