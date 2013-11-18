/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   old/support.h
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#ifndef support_h
#define support_h

/*!
  \namespace StrFunc
  \brief Holds support functions for strings
  \author S. Ansell
  \date February 2006
  \version 1.0
*/

namespace StrFunc
{
/// Process the argv to find -options
int processMainInput(const int,char**,
		     std::vector<int>&,std::vector<std::string>&);

/// determine if a character group exists in a string
int confirmStr(const std::string&,const char*);
/// Get a word from a string
int extractWord(std::string&,const std::string&,const int=4);

/// strip pre/post spaces
std::string fullBlock(const std::string&);
/// strip trialling comments
void stripComment(std::string&);
/// make lower
void lowerString(std::string&);
/// Determines if a string is only spaces
int isEmpty(const std::string&);
/// Get a line and strip comments 
std::string getLine(std::istream&,const int= 256);
/// get a part of a long line
int getPartLine(std::istream&,std::string&,std::string&,const int= 256);

template<typename T> int convPartNum(const std::string&,T&);

/// Convert a string into a number
template<typename T> int convert(const std::string&,T&);
/// Convert a char* into a number
template<typename T> int convert(const char*,T&);


/// Convert and cut a string
template<typename T> int sectPartNum(std::string&,T&);

/// Convert and cut a string
template<typename T> int section(std::string&,T&);
/// Convert and cut a char* 
template<typename T> int section(char*,T&);

/// Convert and cut a string for MCNPX
template<typename T> int sectionMCNPX(std::string&,T&);

/// Write file in standard MCNPX input form 
 void writeMCNPX(const std::string&,std::ostream&);
/// Write file in standard MCNPX input form 
 void writeMCNPXComment(const std::string&,std::ostream&);


/// Split string into spc deliminated components
std::vector<std::string> StrParts(std::string);

/// Write a set of vectors to a file
template<typename T> int writeFile(const std::string&,const T,const std::vector<T>&);
/// Write a set of vectors to a file
template<typename T> int writeFile(const std::string&,const std::vector<T>&,const std::vector<T>&);
/// Write a set of three vectors to a file
template<typename T> int writeFile(const std::string&,const std::vector<T>&,const std::vector<T>&,const std::vector<T>&);

/// Write a set of three vectors to a file
template<typename T> int writeStream(
  const std::string&,const std::string&,
  const std::vector<T>&,const std::vector<T>&,
  const std::vector<T>&,const int=0);

/// Write a set of three vectors to a file
template<typename T> int writeStream(
  const std::string&,const std::string&,
  const std::vector<T>&,const std::vector<T>&,const int=0);

/// Get a line into index points
template<typename T> 
int setValues(const std::string&,const int,T&,const int,T&,const int,T&);

/// Convert a VAX number to x86 little eindien
float getVAXnum(const float);

};

#endif

