/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   xmlInc/XMLvector.h
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
#ifndef XMLvector_h
#define XMLvector_h

namespace XML
{
  /*!
    \class XMLvector
    \version 1.0
    \author S. Ansell
    \date August 2007
    \brief Holds a vector pair
  */

template<template<typename T,typename Alloc> 
  class V,typename T,typename Alloc> 
class XMLvector : public XMLobject
{
 private:
  
  V<T,Alloc> X;            ///< X values
  V<T,Alloc> Y;            ///< Y values

 public:

  XMLvector(XMLobject*,const std::string&);
  XMLvector(XMLobject*,const std::string&,
	    const V<T,Alloc>&,const V<T,Alloc>&);
  XMLvector(const XMLvector<V,T,Alloc>&);
  XMLvector<V,T,Alloc>* clone() const;
  XMLvector<V,T,Alloc>& operator=(const XMLvector<V,T,Alloc>&);
  ~XMLvector();

  void setComp(const V<T,Alloc>&,const V<T,Alloc>&);    ///< Assumes copy setting
  virtual void writeXML(std::ostream&) const;     

  V<T,Alloc>& getX() { return X; }                  ///< Accessor to X
  V<T,Alloc>& getY() { return Y; }                  ///< Accessor to Y
  const V<T,Alloc>& getX() const { return X; }      ///< const Accessor to X
  const V<T,Alloc>& getY() const { return Y; }      ///< const Accessor to Y
  
};

}        // NAMESPACE XML

#endif
