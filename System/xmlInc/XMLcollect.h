/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   xmlInc/XMLcollect.h
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
#ifndef XML_XMLcollect_h
#define XML_XMLcollect_h

namespace XML
{

  class XMLobject;

/*!
  \class XMLcollect 
  \brief XML holder for Spectrum information
  \author S. Ansell
  \version 1.0
  \date December 2006
  
  This class holds an XML schema of any 
  type but normally built against a class.
  It can write out a set of XML information.
  Its key component is Master, which holds the 
  main XML tree. Additionally WorkGrp is used
  as a placement pointer for extra speed

  \todo Need the output buffered : parital
*/

class XMLcollect 
{
 private:

  std::string depthKey;               ///< Current depth layer
  XMLgroup Master;                    ///< Master group
  XMLgroup* WorkGrp;                  ///< Working group [Never Null]
  XMLload InputFile;                  ///< Input loading
  
  std::string makeTimeString(const tm*) const;

 public:

  XMLcollect(); 
  XMLcollect(const XMLcollect&);
  XMLcollect& operator=(const XMLcollect&);
  ~XMLcollect();

  void clear();            

  void addGrp(const std::string&);
  int addNumGrp(const std::string&);
  template<typename T> long int addNumComp(const std::string&,const T&);
  
  // Non-file type One
  template<template<typename T,typename A> class V,typename T,typename A> 
  long int addNumComp(const std::string&,const V<T,A>&);
  // Non-file type
  template<template<typename T,typename A> class V,typename T,typename A> 
  long int addNumComp(const std::string&,const V<T,A>&,const V<T,A>&);

  template<template<typename T,typename A> class V,typename T,typename A> 
  long int addNumComp(const std::string&,const V<T,A>&,
		 const V<T,A>&,const V<T,A>&);
  // file type
  template<template<typename T,typename A> class V,typename T,typename A> 
  long int addNumComp(const std::string&,const std::string&,
		 const V<T,A>&,const V<T,A>&);

  template<template<typename T,typename A> class V,typename T,typename A> 
  long int addNumComp(const std::string&,const std::string&,
		 const V<T,A>&,const V<T,A>&,const V<T,A>&);

  template<typename T> int addComp(const std::string&,const T&);

  // One container
  template<template<typename T,typename A> class V,typename T,typename A> 
  int addComp(const std::string&,const V<T,A>&);

  // Two container
  template<template<typename T,typename A> class V,typename T,typename A> 
  int addComp(const std::string&,const V<T,A>&,const V<T,A>&);

  // Two container + file
  template<template<typename T,typename A> class V,typename T,typename A> 
  int addComp(const std::string&,const std::string&,
	      const V<T,A>&,const V<T,A>&);

  template<template<typename T,typename A> class V,typename T,typename A> 
  int addComp(const std::string&,const V<T,A>&,const V<T,A>&,const V<T,A>&);

  // Three Containers + file
  template<template<typename T,typename A> class V,typename T,typename A> 
  int addComp(const std::string&,const std::string&,
	      const V<T,A>&,const V<T,A>&,const V<T,A>&);

  // Four Containers
  template<template<typename T,typename A> class V,typename T,typename A> 
  int addComp(const std::string&,const V<T,A>&,
	      const V<T,A>&,const V<T,A>&,const V<T,A>&);

  // Attributes
  template<typename T>
  void addAttribute(const std::string&,const T&);
  template<typename T>
  void addAttribute(const std::string&,const std::string&,const T&);

  template<typename T>
  void addObjAttribute(const std::string&,const T&);

  // Add a comment
  void addComment(const std::string&);  

  // Get/Find object
  XMLobject* getObj(const std::string&,const int =0) const;
  XMLobject* findObj(const std::string&,const int =0);
  XMLgroup* findGroup(const std::string&,const int =0);
  /// Get current group
  XMLgroup* getCurrent() { return WorkGrp; }
  int setToKey(const std::string&,const int=0);

  /// get repeat count
  int getRepeatNumber() const { return WorkGrp->getRepNum(); }

  const XMLobject* findParent(const XMLobject*) const; 
  /// Get current object
  const XMLgroup* getCurrent() const { return WorkGrp; } 

  /// Get current object
  const XMLgroup* getMaster() const { return &Master; } 

  int deleteObj(XMLobject*);

  void closeGrp();
  int closeGrpWithCheck(const std::string&);

  void writeXML(std::ostream&) const;

  int loadXML(const std::string&);
  int loadXML(const std::string&,const std::string&);
  int loadXML(const std::string&,const std::string&,
	      const std::string&,const std::string&);
  int loadXML(const std::string&,const std::vector<std::string>&);


};

}   // NAMESPACE XML

#endif
