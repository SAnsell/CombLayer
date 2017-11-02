/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   xml/XMLvector.cxx
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
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <map>
#include <string>
#include <sstream>

#include "Exception.h"
#include "support.h"
#include "XMLload.h"
#include "XMLnamespace.h"
#include "XMLattribute.h"
#include "XMLobject.h"
#include "XMLgroup.h"
#include "XMLvector.h"

namespace XML
{

// --------------------------------------------------------
//                   XMLvector
// --------------------------------------------------------


template<template<typename T,typename Alloc> class V, typename T,typename Alloc>
XMLvector<V,T,Alloc>::XMLvector(XMLobject* B,const std::string& K) :
  XMLobject(B,K)
  /*!
    Constructor with junk key (value is NOT set)
    \param B :: Parent object
    \param K :: key
  */
{}

template<template<typename T,typename Alloc> class V, typename T,typename Alloc>
XMLvector<V,T,Alloc>::XMLvector(XMLobject* B,const std::string& K,
			  const V<T,Alloc>& Xvec,const V<T,Alloc>& Yvec) :
  XMLobject(B,K),X(Xvec),Y(Yvec)
  /*!
    Constructor with Key and Value
    \param B :: Parent object
    \param K :: key
    \param Xvec :: Vector to contain
    \param Yvec :: Vector to contain.
  */
{
  setEmpty(X.empty());
}

template<template<typename T,typename Alloc> 
         class V, typename T,typename Alloc>
XMLvector<V,T,Alloc>::XMLvector(const XMLvector<V,T,Alloc>& A) :
  XMLobject(A),X(A.X),Y(A.Y)
  /*!
    Standard copy constructor
    \param A :: XMLvector to copy
  */
{}

template<template<typename T,typename Alloc> 
         class V, typename T,typename Alloc>
XMLvector<V,T,Alloc>&
XMLvector<V,T,Alloc>::operator=(const XMLvector<V,T,Alloc>& A) 
  /*!
    Standard assignment operator
    \param A :: Object to copy
    \return *this
  */
{
  if (this!=&A)
    {
      XMLobject::operator=(A);
      X=A.X;
      Y=A.Y;
    }
  return *this;
}

template<template<typename T,typename Alloc> class V, typename T,
	 typename Alloc>
XMLvector<V,T,Alloc>*
XMLvector<V,T,Alloc>::clone() const
  /*!
    The clone function
    \return new copy of this
  */
{
  return new XMLvector<V,T,Alloc>(*this);
}

template<template<typename T,typename Alloc> class V, typename T,
	 typename Alloc>
XMLvector<V,T,Alloc>::~XMLvector()
  /*!
    Standard destructor
  */
{ }


template<template<typename T,typename Alloc> 
	 class V, typename T,typename Alloc>
void
XMLvector<V,T,Alloc>::writeXML(std::ostream& OX) const
  /*!
    Write out the object
    \param OX :: output stream
  */
{
  writeDepth(OX);
  std::string::size_type pos=Key.find('%');
  const std::string KeyOut=Key.substr(0,pos);
  if (this->isEmpty() )
    {
      OX<<"<"<<KeyOut<<Attr<<"/>"<<std::endl;
      return;
    }
  OX<<"<"<<KeyOut<<Attr<<">"<<std::endl;
  typename V<T,Alloc>::const_iterator xc=X.begin();
  typename V<T,Alloc>::const_iterator yc;
  for(yc=Y.begin();yc!=Y.end();xc++,yc++)
    OX<<(*xc)<<" "<<(*yc)<<std::endl;
  OX<<"</"<<KeyOut<<">"<<std::endl;
  return;
}


  
}    // NAMESPACE

/// \cond TEMPLATE

template class XML::XMLvector<std::vector,double,std::allocator<double> >;

/// \endcond TEMPLATE
