/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   xmlInc/XMLnamespace.h
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
#ifndef XMLnamespace_h
#define XMLnamespace_h

/*!
  \namespace XML
  \version 1.0
  \author S. Ansell
  \date December 2006
  \brief Holds the XML namespace objects
*/

namespace XML
{

class XMLobject;

 std::vector<std::string> getUnits(std::string);
int getGroupContent(XMLload&,std::string&,std::vector<std::string>&,std::string&);
int getNextGroup(XMLload&,std::string&,std::vector<std::string>&); 
int collectBuffer(XMLload&,std::string&); 
int splitComp(XMLload&,std::string&,std::string&);
int splitGroup(XMLload&,std::string&,
	       std::vector<std::string>&,std::string&); 
int splitObjGroup(XMLobject*,std::vector<std::string>&,
		std::vector<std::string>&); 
int getFilePlace(XMLload&,const std::string&);
int splitLine(XMLload&,std::string&,std::string&);
int findGroup(XMLload&,const std::string&,const int =0);
int matchSubGrp(XMLload&,const std::string&,const std::string&);
int splitAttribute(std::string&,std::string&,std::string&);



template<typename K,typename D>
long int getNumberIndex(const std::multimap<K,D>&, const K&);

long int cutString(std::string&,std::string&);
std::vector<std::string> getParts(std::string);
std::pair<int,std::string> procKey(const std::string&);
std::string procString(const std::string&);
int matchPath(const std::string&,const std::string&);
int procGroupString(const std::string&,std::string&,std::vector<std::string>&);
 
}  // NAMESPACE XML

#endif
