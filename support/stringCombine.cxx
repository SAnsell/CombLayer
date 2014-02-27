/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   support/stringCombine.cxx
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
#include <iomanip>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <sstream>
#include <cmath>
#include <complex>
#include <vector>
#include <list>
#include <string>
#include <algorithm>
#include <functional>

#include "Exception.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "doubleErr.h"
#include "mathSupport.h"
#include "support.h"

/*! 
  \file stringCombine.cxx 
*/

namespace  StrFunc
{

template<typename T>
std::string
makeString(const T& V)
  /*!
    Turn V into a string (using ostringstream)
    \param V :: Object to convert
    \return String
  */
{
  std::ostringstream cx;
  cx<<V;
  return cx.str();
}

template<>
std::string
makeString(const std::string& V)
  /*!
    Turn V into a string (using ostringstream)
    \param V :: Object to convert
    \return String
  */
{
  return V;
}

template<typename T,typename U>
std::string
makeString(const T& A,const U& B)
  /*!
    Turn A+B into a string (using ostringstream)
    \param A :: Object to convert
    \param B :: Object to convert
    \return String
  */
{
  std::ostringstream cx;
  cx<<A<<B;
  return cx.str();
}

template<typename U>
std::string
makeString(const char* A,const U& B)
  /*!
    Turn A+B into a string (using ostringstream)
    \param A :: Object to convert
    \param B :: Object to convert
    \return String
  */
{
  std::ostringstream cx;
  cx<<A<<B;
  return cx.str();
}

///\cond TEMPLATE
template std::string makeString(const int&);
template std::string makeString(const size_t&);
template std::string makeString(const double&);
template std::string makeString(const long int&);
template std::string makeString(const std::string&,const int&);
template std::string makeString(const std::string&,const double&);
template std::string makeString(const std::string&,const size_t&);
template std::string makeString(const std::string&,const long int&);
template std::string makeString(const char*,const int&);
template std::string makeString(const char*,const size_t&);

///\endcond TEMPLATE

}  // NAMESPACE StrFunc
