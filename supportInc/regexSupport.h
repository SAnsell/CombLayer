/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   supportInc/regexSupport.h
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
#ifndef regexSupport_h 
#define regexSupport_h
#ifndef NO_REGEX

namespace StrFunc
{
/// Find if a pattern matches
template<typename T> int StrComp(const std::string&,
				 const boost::regex&,T&,const int=0);

int StrLook(const char*,const boost::regex&);
int StrLook(const std::string&,const boost::regex&);

/// Split  a line into component parts
std::vector<std::string> 
StrParts(std::string,const boost::regex&);

/// Split  a line searched parts
template<typename T> int StrFullSplit(const std::string&,const boost::regex&,std::vector<T>&);
template<typename T> int StrSingleSplit(const std::string&,const boost::regex&,std::vector<T>&);

/// Cut out the searched section and returns component
template<typename T> 
int StrFullCut(std::string&,const boost::regex&,T&,const int= -1);
template<typename T> 
int StrFullCut(std::string&,const boost::regex&,std::vector<T>&);

/// Extract a section from a string
int StrRemove(std::string&,std::string&,const boost::regex&);

/// Find a compmonent in a Regex in a file
template<typename T> int findComp(std::istream&,const boost::regex&,T&);

/// Finds a pattern in a file
int findPattern(std::istream&,const boost::regex&,std::string&);

} // NAMESPACE StrFunc

#endif
#endif
