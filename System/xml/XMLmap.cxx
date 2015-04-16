/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   xml/XMLmap.cxx
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
#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>
#include <vector>
#include <map>
#include <string>

#include "Exception.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "support.h"
#include "XMLload.h"
#include "XMLnamespace.h"
#include "XMLattribute.h"
#include "XMLobject.h"
#include "XMLgroup.h"
#include "XMLmap.h"

namespace XML
{

// --------------------------------------------------------
//                   XMLmap
// --------------------------------------------------------

template<template<typename K,typename T,typename Comp,typename Alloc> 
        class V,typename K,typename T,typename Comp,typename Alloc> 
XMLmap<V,K,T,Comp,Alloc>::XMLmap(XMLobject* B,const std::string& Kval) :
  XMLobject(B,Kval)
  /*!
    Constructor with junk key (value is NOT set)
    \param B :: XMLobject to used as parent
    \param Kval :: key
  */
{}

template<template<typename K,typename T,typename Comp,typename Alloc> 
        class V,typename K,typename T,typename Comp,typename Alloc> 
XMLmap<V,K,T,Comp,Alloc>::XMLmap(XMLobject* B,const std::string& Kval,
				 const GType& M) :
  XMLobject(B,Kval),Grid(M)
  /*!
    Constructor with key and object
    \param B :: XMLobject to used as parent
    \param Kval :: keyname for the object
    \param M :: Grid of values to add to M
  */
{
  setEmpty(Grid.empty());
}

template<template<typename K,typename T,typename Comp,typename Alloc> 
        class V,typename K,typename T,typename Comp,typename Alloc> 
XMLmap<V,K,T,Comp,Alloc>::XMLmap(const XMLmap<V,K,T,Comp,Alloc>& A) :
  XMLobject(A),Grid(A.Grid)
  /*!
    Standard copy constructor
    \param A :: XMLmap to copy
  */
{
}

template<template<typename K,typename T,typename Comp,typename Alloc> 
        class V,typename K,typename T,typename Comp,typename Alloc> 
XMLmap<V,K,T,Comp,Alloc>&
XMLmap<V,K,T,Comp,Alloc>::operator=(const XMLmap<V,K,T,Comp,Alloc>& A) 
  /*!
    Standard assignment operator
    \param A :: Object to copy
    \return *this
  */
{
  if (this!=&A)
    {
      XMLobject::operator=(A);
      Grid=A.Grid;
    }
  return *this;
}


template<template<typename K,typename T,typename Comp,typename Alloc> 
        class V,typename K,typename T,typename Comp,typename Alloc> 
XMLmap<V,K,T,Comp,Alloc>*
XMLmap<V,K,T,Comp,Alloc>::clone() const
  /*!
    The clone function
    \return new copy of this
  */
{
  return new XMLmap<V,K,T,Comp,Alloc>(*this);
}

template<template<typename K,typename T,typename Comp,typename Alloc> 
        class V,typename K,typename T,typename Comp,typename Alloc> 
XMLmap<V,K,T,Comp,Alloc>::~XMLmap()
  /*!
    Standard destructor
  */
{}

template<template<typename K,typename T,typename Comp,typename Alloc> 
        class V,typename K,typename T,typename Comp,typename Alloc> 
void
XMLmap<V,K,T,Comp,Alloc>::setComp(const GType& MO)
  /*!
    Set Component in Grid.
    \param MO :: container object to add
   */
{
  Grid=MO;
  this->setEmpty(0);
  return;
}

template<template<typename K,typename T,typename Comp,typename Alloc> 
        class V,typename K,typename T,typename Comp,typename Alloc> 
void
XMLmap<V,K,T,Comp,Alloc>::writeXML(std::ostream& OX) const
  /*!
    Write out the object
    \param OX :: output stream
  */
{
  writeDepth(OX);
  std::string::size_type pos=Key.find('%');
  const std::string KeyOut=Key.substr(0,pos);
  if (this->isEmpty())
    {
      OX<<"<"<<KeyOut<<Attr<<"/>"<<std::endl;
      return;
    }
  
  // Valued write
  OX<<"<"<<KeyOut<<Attr<<">"<<std::endl;
  typename GType::const_iterator gc;
  for(gc=Grid.begin();gc!=Grid.end();gc++)
    {
      writeDepth(OX);
      OX<<"  "<<gc->first<<" "<<gc->second<<std::endl;
    }
  writeDepth(OX);
  OX<<"</"<<KeyOut<<">"<<std::endl;
  return;
}

  
} // NAMESPACE XML

/// \cond TEMPLATE

template class XML::XMLmap<std::map,std::string,double>;
template class XML::XMLmap<std::map,std::string,int>;

/// \endcond TEMPLATE
