/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   supportInc/InputControl.h
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
#ifndef InputControl_h
#define InputControl_h

/*!
  \namespace InputControl
  \version 1.0
  \author S. Ansell
  \brief Convert Main into useful stuff
  \date June 2007
*/


namespace InputControl
{

int mainVector(const int,char**,std::vector<std::string>&);
std::string getFileName(std::vector<std::string>&);

/// Process the argv to find -options
int flagExtract(std::vector<std::string>&,const std::string&);

/// Process the argv to find -options
int flagExtract(std::vector<std::string>&,const std::string&,
		const std::string&);


template<typename T>
int flagVExtract(std::vector<std::string>&,const std::string&,T&);

template<typename T>
int flagVExtract(std::vector<std::string>&,const std::string&,
		const std::string&,T&);

template<typename T,typename U>
int flagVExtract(std::vector<std::string>&,const std::string&,
		 const std::string&,T&,U&);

template<typename T,typename U,typename V>
int flagVExtract(std::vector<std::string>&,const std::string&,
		const std::string&,T&,U&,V&);
        

int findFlag(const std::vector<std::string>&,const std::string&);

/// Process the argv to find -options
int processMainInput(const int,char**,
		     std::vector<int>&,std::vector<std::string>&);

/// Process the argv to find -options
int processMainInput(const int,char**,
		     std::vector<std::string>&,std::vector<std::string>&);


}


#endif
