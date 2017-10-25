/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   supportInc/regexBuild.h
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
#ifndef StrFunc_regexBuild_h 
#define StrFunc_regexBuild_h

namespace StrFunc
{

int findPattern(std::istream&,const std::string&,std::string&);
template<typename T> int findComp(std::istream&,const std::string&,T&);
int StrLook(const std::string&,const std::string&);  
 
template<typename T>
int StrComp(const std::string&,const std::string&,T&,const size_t =0); 

std::vector<std::string>
StrParts(const std::string&,const std::string&); 


} // NAMESPACE StrFunc

#endif
