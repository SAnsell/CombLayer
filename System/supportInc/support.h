/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   supportInc/support.h
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
#ifndef StrFunc_support_h
#define StrFunc_support_h
/*!
  \class reverse
  Template system to reverse interator.
  Note that if can be used in range for loops e.g.
  for(const int I : reverse(vecI))
 */

template<typename T>
class reverse
{
private:
  T& container;
public:
  explicit reverse(T& C) : container(C) {}
  auto begin() const { return std::rbegin(container); }
  auto end() const { return std::rend(container); }
};

template <typename T> inline constexpr
T signum(T x, std::false_type)
{
  return T(T(0) < x);
}

template <typename T> inline constexpr
T signum(T x, std::true_type)
{
  return T((T(0) < x) - (x < T(0)));
}

template <typename T> inline constexpr
T signum(T x)
{
  return signum(x, std::is_signed<T>());
}

/*!
  \namespace StrFunc
  \brief Holds support functions for strings
  \author S. Ansell
  \date February 2006
  \version 1.0
*/

namespace StrFunc
{
  
/// determine if a character group exists in a string
int confirmStr(const std::string&,const std::string&);
/// Get a word from a string
int extractWord(std::string&,const std::string&,const size_t=4);

/// strip ALL spaces
std::string removeAllSpace(const std::string&);
/// Strip out excessive spaces
std::string stripMultSpc(const std::string&);
/// find quoted blocks (section for strings)
int quoteBlock(std::string&,std::string&);

std::string getDelimUnit(const std::string&,const std::string&,
			  std::string&);
bool splitUnit(const std::string&,std::string&,std::string&,const std::string&);

/// strip pre-spaces
std::string frontBlock(const std::string&);
/// strip pre/post spaces
std::string removeOuterSpace(const std::string&);
/// strip pre/post + returns + double spaces:
std::string singleLine(const std::string&);
/// Find comment
long int hasComment(const std::string&);
long int hasComment(const std::string&,const std::string&,const std::string&);
/// strip trialing comments
int stripComment(std::string&);
std::string stripNotNumber(std::string);

void upperString(std::string&);
void lowerString(std::string&);
std::string toUpperString(const std::string&);
std::string toLowerString(const std::string&);

bool hasStringEnd(const std::string&,const std::string&);
int isEmpty(const std::string&);
/// Get a line 
std::string getAllLine(std::istream&,const int= 256);
/// Get a line and strip comments 
std::string getLine(std::istream&,const int= 256);
/// Get a part of line
int getPartLine(std::istream&,std::string&,std::string&,const size_t= 256);

template<typename T> int fortRead(std::string&,const size_t,T&);

template<typename T> size_t convPartNum(const std::string&,T&);

/// Convert a string into a number
template<typename T> int convert(const std::string&,T&);

template<typename T> 
int setValues(const std::string&,const std::vector<int>&,
	      std::vector<T>&);

int sectionBracket(std::string&,std::string&);
template<typename T> int sectPartNum(std::string&,T&);
template<typename T> int section(std::string&,T&);
/// Convert and cut a string for MCNPX
template<typename T> int sectionMCNPX(std::string&,T&);
template<typename T> int sectionCINDER(std::string&,T&);
template<typename T> int itemize(std::string&,std::string&,T&);

template<typename T>
bool convertNameWithIndex(std::string&,T&);
  
// Write file in standard MCNPX input form
void writeControl(const std::string&,std::ostream&,
		  const size_t,const int);

template<typename T> void writeLine(std::ostream&,const T&,
				     size_t&,const size_t);


template<template<typename T,typename Alloc> class V,typename T,typename Alloc> 
bool removeItem(V<T,Alloc>&,const T&);

size_t countUnits(const std::string&,const char =' ');
std::vector<std::string> StrParts(std::string);

std::string
splitFront(const std::string&,const char delim);
std::string
splitBack(const std::string&,const char delim);
std::string
cutFront(std::string&,const char delim);
std::pair<std::string,std::string>
splitPair(const std::string&,const char delim);
std::vector<std::string>
splitParts(const std::string&,const char delim);

std::vector<std::string>
splitNSParts(const std::string&,const char delim);

std::vector<std::string>
StrSeparate(const std::string&,const std::string&);

template<template<typename T,typename Alloc> class V,typename T,typename Alloc> 
int sliceVector(V<T,Alloc>&,const T&,const T&);

template<typename T>
int sectionRange(std::string&,std::vector<T>&);
template<typename T>
int sectionCount(const size_t,std::string&,std::vector<T>&);

template<typename T>
std::set<T> getRangedSet(std::string&);

  
/// Convert a VAX number to x86 little eindien
float getVAXnum(const float);

}  // NAMESPACE StrFunc

#endif

