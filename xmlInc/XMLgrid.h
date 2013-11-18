/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   xmlInc/XMLgrid.h
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
#ifndef XMLgrid_h
#define XMLgrid_h

namespace XML
{

  /*!
    \class XMLgrid
    \version 1.0
    \author S. Ansell
    \date August 2007
    \brief The object is to hold a set of containers
  */

template<template<typename T,typename Alloc> class V,typename T,
         typename Alloc =std::allocator<T> > 
class XMLgrid : public XMLobject
{
 private:

  size_t size;                      ///< Current size
  std::vector< V<T,Alloc> > Grid;   ///< X values
  size_t contLine;                     ///< Number of components per line

 public:

  XMLgrid(XMLobject*,const std::string&);
  XMLgrid(const XMLgrid<V,T,Alloc>&);
  XMLgrid<V,T,Alloc>* clone() const;
  XMLgrid<V,T,Alloc>& operator=(const XMLgrid<V,T,Alloc>&);
  ~XMLgrid();

  size_t getSize() const { return (!empty) ? size : 0; }    ///< get size
  void setContLine(const size_t IFlag) { contLine=IFlag; }  ///< Set continue size 

  void setComp(const size_t,const V<T,Alloc>&);    ///< Assumes copy setting
  virtual void writeXML(std::ostream&) const;    

  V<T,Alloc>& getGVec(const size_t);
  const V<T,Alloc>& getGVec(const size_t) const;
  
};

}  // NAMESPACE XML

#endif
