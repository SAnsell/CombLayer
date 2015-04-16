/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   supportInc/fileSupport.h
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
#ifndef fileSupport_h
#define fileSupport_h

namespace StrFunc
{

/// Write a set of containers to a file
template<template<typename T,typename A> class V,typename T,typename A> 
int writeFile(const std::string&,const T,const V<T,A>&);
template<template<typename T,typename A> class V,typename T,typename A> 
int writeFile(const std::string&,const V<T,A>&,const V<T,A>&);
template<template<typename T,typename A> class V,typename T,typename A> 
int writeFile(const std::string&,const V<T,A>&,const V<T,A>&,const V<T,A>&);

template<template<typename T,typename A> class V,typename T,typename A,
  template<typename Tx,typename Ax> class W,typename Tx,typename Ax> 
int writeFile(const std::string&,const V<T,A>&,const W<Tx,Ax>&);

template<template<typename T,typename A> class V,typename T,typename A> 
int writeColumn(const std::string&,const V<T,A>&,const V<T,A>&);

template<typename T> 
int writeStream(const std::string&,const std::string&,
		const std::vector<T>&,const std::vector<T>&,
		const std::vector<T>&,const int);

template<typename T,typename U> 
int writeStream(const std::string&,const std::string&,
		const std::vector<T>&,const std::vector<U>&,
		const int);

}  // NAMESPACE StrFunc

#endif

