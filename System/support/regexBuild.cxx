/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   support/regexBuild.cxx
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
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <algorithm>
#include <functional>
#ifndef NO_REGEX
#include <regex>
#endif 

#include "support.h"
#include "regexSupport.h"
#include "regexBuild.h"

namespace StrFunc
{


int
findPattern(std::istream& IX,const std::string& ReExpression,std::string& Out)
  /*!
    Helper function to convert string to expression
    \param IX :: Input stream
    \param ReExpression :: Regular expression as string
    \param Out :: Match segment
    \return true if match found
   */
{
  std::regex RE(ReExpression);
  return findPattern(IX,RE,Out);
}

int
StrLook(const std::string& Text,const std::string& ReExpression)
  /*!
    Find the match in regular expression and return 1 if good match 
    \param Text :: string to match
    \param ReExpression :: string of regular expression
    \returns 0 on failure and 1 on success
  */
{
  std::regex Re(ReExpression);
  return StrLook(Text,Re);
}


template<typename T>
int
StrComp(const std::string& Text,const std::string& ReExpression,
	T& Aout,const size_t compNum) 
  /*!
    Find the match in regular expression and places number in Aout 
    \param Text :: string to search
    \param ReExpression :: regular expression to use (as string)
    \param Aout :: Place to put Unit found
    \param compNum :: item to extract [0:N-1]
    \returns 0 on failure and 1 on success
  */
{
  std::regex RE(ReExpression);
  return StrComp<T>(Text,RE,Aout,compNum);
}
  
std::vector<std::string> 
StrParts(const std::string& Sdx,const std::string& ReExpression )
  /*! 
    Find the match, return the disected items.
    Note it is complementary to support.h StrParts(Sdx)
    \param Sdx :: Input string (note implicit copy since altered)
    \param ReExpression :: Regular expression for separator component
    \returns vector of string components
  */
{
  std::regex RE(ReExpression);
  return StrParts(Sdx,RE);
}   

/// \cond TEMPLATE 

template int StrComp(const std::string&,const std::string&,std::string&,
		     const size_t);
template int StrComp(const std::string&,const std::string&,double&,
		     const size_t);
template int StrComp(const std::string&,const std::string&,int&,
		     const size_t);

/// \endcond TEMPLATE 

}  // NAMESPACE StrFunc

