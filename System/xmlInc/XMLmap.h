/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   xmlInc/XMLmap.h
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
#ifndef XMLmap_h
#define XMLmap_h

namespace XML
{

  /*!
    \class XMLmap
    \version 1.0
    \author S. Ansell
    \date March 2008
    \brief The object is to hold a set of associative containers
    
   */

template<template<typename K,typename T,typename Comp,typename Alloc>
 class V,typename K,typename T,typename Comp = std::less<K>,
 typename Alloc = std::allocator<std::pair<const K,T> > >
class XMLmap : public XMLobject
{
 private:
   
  typedef V<K,T,Comp,Alloc> GType;   ///< values

  GType Grid;   ///< values

 public:

  XMLmap(XMLobject*,const std::string&);
  XMLmap(XMLobject*,const std::string&,const GType&);
  XMLmap(const XMLmap<V,K,T,Comp,Alloc>&);
  XMLmap<V,K,T,Comp,Alloc>* clone() const;
  XMLmap<V,K,T,Comp,Alloc>& operator=(const XMLmap<V,K,T,Comp,Alloc>&);
  ~XMLmap();

  /// get size
  size_t getSize() const { return (!empty) ? Grid.size() : 0; }  
   
  void setComp(const GType&);    ///< Assumes copy setting
    
  virtual void writeXML(std::ostream&) const;    
  
};

}  // NAMESPACE XML

#endif
