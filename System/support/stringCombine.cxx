/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   support/stringCombine.cxx
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
#include <cstdio>
#include <cstring>
#include <fstream>
#include <sstream>
#include <cmath>
#include <complex>
#include <vector>
#include <list>
#include <string>
#include <iterator>


#include "Exception.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "doubleErr.h"
#include "stringCombine.h"
/*! 
  \file stringCombine.cxx 
*/

namespace  StrFunc
{

char
indexToAlpha(const size_t index)
  /*!
    Convert an index to A-Z / a-z 
    \param index :: index value
    \return [A-z] based on index '?' on error
  */
{
  static const char cmap[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz";
  return (index>=52) ? '?' : cmap[index];
}
    
  
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

template<template<typename T,typename A> class V,typename T,typename A> 
std::string
makeString(const V<T,A>& Vec)
  /*!
    Turn V into a string (using ostringstream)
    \param Vec :: Vec/List to convert
    \return String
  */
{
  std::ostringstream cx;
  std::copy(Vec.begin(),Vec.end(),std::ostream_iterator<T>(cx," "));
  return cx.str();
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

size_t
checkKey(const std::string& M,const std::string& X,
	 const std::string& Y,const std::string& Z)
  /*!
    Check match and return match point+1
    \param M :: Key to match
    \param X :: Key to check
    \param Y :: Key to check
    \param Z :: Key to check
    \return 0 on failure / 1,2,3 on success
  */
{
  if (M==X) return 1;
  if (M==Y) return 2;
  if (M==Z) return 3;
  return 0;
}

///\cond TEMPLATE
template std::string makeString(const int&);
template std::string makeString(const size_t&);
template std::string makeString(const double&);
template std::string makeString(const long int&);
template std::string makeString(const Geometry::Vec3D&);
template std::string makeString(const std::string&,const int&);
template std::string makeString(const std::string&,const double&);
template std::string makeString(const std::string&,const size_t&);
template std::string makeString(const std::string&,const long int&);

template std::string makeString(const std::vector<int>&);

  
template std::string makeString(const char*,const int&);
template std::string makeString(const char*,const size_t&);

///\endcond TEMPLATE

}  // NAMESPACE StrFunc
