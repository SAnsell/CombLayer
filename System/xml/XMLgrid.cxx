/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   xml/XMLgrid.cxx
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
#include "XMLgrid.h"

namespace XML
{

// --------------------------------------------------------
//                   XMLgrid
// --------------------------------------------------------

template<template<typename T,typename Alloc> class V,typename T,typename Alloc> 
XMLgrid<V,T,Alloc>::XMLgrid(XMLobject* B,const std::string& K) :
  XMLobject(B,K),size(0),contLine(10)
  /*!
    Constructor with junk key (value is NOT set)
    \param B :: XMLobject to used as parent
    \param K :: key
  */
{}

template<template<typename T,typename Alloc> class V,typename T,typename Alloc> 
XMLgrid<V,T,Alloc>::XMLgrid(const XMLgrid<V,T,Alloc>& A) :
  XMLobject(A),size(A.size),Grid(A.Grid),
  contLine(A.contLine)
  /*!
    Standard copy constructor
    \param A :: XMLgrid to copy
  */
{}

template<template<typename T,typename Alloc> class V,typename T,typename Alloc> 
XMLgrid<V,T,Alloc>&
XMLgrid<V,T,Alloc>::operator=(const XMLgrid<V,T,Alloc>& A) 
  /*!
    Standard assignment operator
    \param A :: Object to copy
    \return *this
  */
{
  if (this!=&A)
    {
      XMLobject::operator=(A);
      size=A.size;
      Grid=A.Grid;
      contLine=A.contLine;
    }
  return *this;
}


template<template<typename T,typename Alloc> class V,typename T,typename Alloc> 
XMLgrid<V,T,Alloc>*
XMLgrid<V,T,Alloc>::clone() const
  /*!
    The clone function
    \return new copy of this
  */
{
  return new XMLgrid<V,T,Alloc>(*this);
}

template<template<typename T,typename Alloc> class V,typename T,typename Alloc> 
XMLgrid<V,T,Alloc>::~XMLgrid()
  /*!
    Standard destructor
  */
{}

template<template<typename T,typename Alloc> class V,typename T,typename Alloc> 
void
XMLgrid<V,T,Alloc>::setComp(const size_t Index,const V<T,Alloc>& VO)
  /*!
    Set Component in Grid.
    \param Index :: Index number to add
    \param VO :: container object to add
   */
{
  if (Index>=size)
    {
      size=Index+1;
      Grid.resize(size);
    }
  if (!VO.empty())
    {
      Grid[Index]=VO;
      this->setEmpty(0);
    }
  return;
}

template<template<typename T,typename Alloc> class V,typename T,typename Alloc> 
V<T,Alloc>&
XMLgrid<V,T,Alloc>::getGVec(const size_t Index)
  /*!
    Get a particular container set from a grid.
    \param Index :: Index number to add
    \return Vector component 
   */
{
  if (Index>=Grid.size())
    throw ColErr::IndexError<size_t>(Index,Grid.size(),"XMLgrid::getGVec");
  return Grid[Index];
}

template<template<typename T,typename Alloc> class V,typename T,typename Alloc> 
const V<T,Alloc>&
XMLgrid<V,T,Alloc>::getGVec(const size_t Index) const
  /*!
    Get Component in Grid.
    \param Index :: Index number to add
    \return Vector component (const)
   */
{
  if (Index>=Grid.size())
    throw ColErr::IndexError<size_t>(Index,Grid.size(),"XMLgrid::getGVec");
  return Grid[Index];
}

template<template<typename T,typename Alloc> class V,typename T,typename Alloc> 
void
XMLgrid<V,T,Alloc>::writeXML(std::ostream& OX) const
  /*!
    Write out the object
    \param OX :: output stream
  */
{
  writeDepth(OX);
  std::string::size_type pos=Key.find('%');
  const std::string KeyOut=Key.substr(0,pos);
  if (this->isEmpty() || size==0 || Grid[0].empty())
    {
      OX<<"<"<<KeyOut<<Attr<<"/>"<<std::endl;
      return;
    }

  typename std::vector< V<T,Alloc> >::const_iterator gc;
  std::vector< typename V<T,Alloc>::const_iterator > IterVec;  // Vector of iterators
  std::vector< typename  V<T,Alloc>::const_iterator > IterEnd;  // Vector of iterators
  typename std::vector< typename V<T,Alloc>::const_iterator >::iterator ivc;  // Vector of iterators
  typename std::vector< typename V<T,Alloc>::const_iterator >::iterator evc;  // Vector of iterators
  size_t finished=Grid.size();     // Number of components that must be changed

  for(gc=Grid.begin();gc!=Grid.end();gc++)
    {
      IterVec.push_back(gc->begin());
      IterEnd.push_back(gc->end());
      if (gc->begin()==gc->end())
	finished--;
    }

  // EMPTY WRITE::
  if (!finished)
    {
      writeDepth(OX);
      OX<<"<"<<KeyOut<<Attr<<"/>"<<std::endl;
      return; 
    }

  // Valued write
  OX<<"<"<<KeyOut<<Attr<<">"<<std::endl;

  const size_t factor(IterVec.size());
  size_t cnt(0);
  while(finished)
    {
      if (cnt==0)
	writeDepth(OX);

      for(ivc=IterVec.begin(),evc=IterEnd.begin();
	  ivc!=IterVec.end();ivc++,evc++)
        {
	  if (*ivc != *evc)
	    {
	      OX<<*(*ivc)<<" ";
	      (*ivc)++;
	      if (*ivc == *evc)          // Note the extra test after ivc++
		finished--;
	      cnt++;
	    }
	  else
	    {
	      OX<<"    ";
	    }
	}
      if (cnt+factor>contLine)
        {
	  OX<<std::endl;
	  cnt=0;
	}
    }
  if (cnt)
    OX<<std::endl;

  writeDepth(OX);
  OX<<"</"<<KeyOut<<">"<<std::endl;
  return;
}

  
} // NAMESPACE XML

/// \cond TEMPLATE

template class XML::XMLgrid<std::vector,std::string,std::allocator<std::string> >;
template class XML::XMLgrid<std::vector,double,std::allocator<double> >;
template class XML::XMLgrid<std::vector,int,std::allocator<int> >;
template class XML::XMLgrid<std::vector,size_t,std::allocator<size_t> >;
template class XML::XMLgrid<std::vector,Geometry::Vec3D,std::allocator<Geometry::Vec3D> >;

/// \endcond TEMPLATE
