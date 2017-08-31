/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   supportInc/regexSupport.h
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
#ifndef regexSupport_h 
#define regexSupport_h
#ifndef NO_REGEX

namespace StrFunc
{

template<typename T> int findComp(std::istream&,const std::regex&,T&);

int findPattern(std::istream&,const std::regex&,std::string&);

template<typename T> int findComp(std::istream&,const std::string&,T&);
template<typename T> int findComp(std::istream&,const std::regex&,T&);
  
template<typename T>
int StrComp(const std::string&,const std::regex&,T&,const size_t =0);
 
int StrLook(const char*,const std::regex&);
int StrLook(const std::string&,const std::regex&);

/// Split  a line into component parts
std::vector<std::string> 
StrParts(std::string,const std::regex&);

/// Split  a line searched parts
template<typename T> int StrFullSplit(const std::string&,const std::regex&,std::vector<T>&);
template<typename T> int StrSingleSplit(const std::string&,const std::regex&,std::vector<T>&);

/// Cut out the searched section and returns component
template<typename T> 
int StrFullCut(std::string&,const std::regex&,T&,const long int= -1);
template<typename T> 
int StrFullCut(std::string&,const std::regex&,std::vector<T>&);

int StrRemove(std::string&,std::string&,const std::regex&);




} // NAMESPACE StrFunc

#endif  // NO_REGEX

#endif
