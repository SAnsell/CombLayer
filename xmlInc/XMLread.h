/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   xmlInc/XMLread.h
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
#ifndef XMLread_h
#define XMLread_h

namespace XML
{

/*!
  \class XMLread
  \brief Hold an XML input before specilization
  \author S. Ansell
  \date May 2007
  \version 1.0
  
  This holds the input in a vector of strings

*/

class XMLread : public XMLobject
{
 private:

  std::string Comp;         ///< list of read components

 public:

  
  XMLread(XMLobject*,const std::string&);
  XMLread(XMLobject*,const std::string&,
	  const std::vector<std::string>&);
  XMLread(const XMLread&);
  virtual XMLread* clone() const;       ///< virtual constructor
  XMLread& operator=(const XMLread&);     
  virtual ~XMLread();

  void addLine(const std::string&);          
  void setObject(const std::vector<std::string>&);  
  void setObject(const std::string&);

  /// Access to string
  const std::string& getString() const { return Comp; }
  /// Access to string
  std::string& getString() { return Comp; }
 
  // Conversions (using StrFunc
  template<template<typename T,typename A> class V,typename T,typename A> 
  int convertToContainer(V<T,A>&) const;

  // Containers with forgotten junk!
  template<template<typename T,typename A> class V,typename T,typename A> 
  int convertToContainer(const int,V<T,A>&,V<T,A>&) const;

  template<typename T,int Size>
  int convertToArray(boost::multi_array<T,Size>&) const;

  template<typename T> 
  int convertToObject(T&) const;
  
  virtual int isEmpty() const { return Comp.empty(); }

  void writeXML(std::ostream&) const;

};

}  // NAMESPACE XML

#endif
