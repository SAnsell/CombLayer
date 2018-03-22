/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   inputInc/inputSupport.h
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#ifndef mainSystem_inputSupport_h
#define mainSystem_inputSupport_h


namespace mainSystem
{

/// populate input type
typedef std::map<std::string,std::vector<std::string>> MITYPE;

bool
hasInput(const MITYPE&,const std::string&);
  
template<typename T>
T getInput(const MITYPE&,const std::string&,const size_t =0);
 
template<typename T>
T getDefInput(const std::map<std::string,std::vector<std::string>>&,
	      const std::string&,const size_t,const T&);

template<typename T>
bool findInput(const std::map<std::string,std::vector<std::string>>&,
	       const std::string&,const size_t,T&);

}  // NAMESPACE mainSystem

#endif
 
