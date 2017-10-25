/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   supportInc/stringCombine.h
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
#ifndef StrFunc_stringCombine_h
#define StrFunc_stringCombine_h

namespace StrFunc
{

  template<template<typename T,typename A> class V,typename T,typename A> 
    std::string makeString(const V<T,A>&);

  template<typename T> 
    std::string makeString(const T&);

  template<typename T,typename U> 
    std::string makeString(const T&,const U&);

  template<typename U> 
    std::string makeString(const char*,const U&);

  char indexToAlpha(const size_t);
  
  size_t checkKey(const std::string&,const std::string&,
		  const std::string&,const std::string&);
}  // NAMESPACE StrFunc

#endif

