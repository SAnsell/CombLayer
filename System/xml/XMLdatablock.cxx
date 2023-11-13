/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   xml/XMLdatablock.cxx
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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
#include <functional>
#include <iterator>

#include "dataSlice.h"
#include "multiData.h"
#include "XMLattribute.h"
#include "XMLobject.h"
#include "XMLdatablock.h"

#include "doubleErr.h"

namespace XML
{

// --------------------------------------------------------
//                   XMLdatablock
// --------------------------------------------------------

template<typename T>
XMLdatablock<T>::XMLdatablock(XMLobject* B,const std::string& K) :
  XMLobject(B,K),managed(0),contLine(10),Data(0)
  /*!
    Constructor with junk key (value is NOT set)
    \param B :: XMLobject to used as parent
    \param K :: key
  */
{}

template<typename T>
XMLdatablock<T>::XMLdatablock(const XMLdatablock<T>& A) :
  XMLobject(A),managed(A.managed),contLine(A.contLine),
  Data((!A.managed || !A.Data) ? A.Data : new mapArray(*A.Data))
  /*!
    Standard copy constructor
    \param A :: XMLdatablock to copy
  */
{}

template<typename T>
XMLdatablock<T>&
XMLdatablock<T>::operator=(const XMLdatablock<T>& A) 
  /*!
    Standard assignment operator
    \param A :: Object to copy
    \return *this
  */
{
  if (this!=&A)
    {
      XMLobject::operator=(A);
      contLine=A.contLine;

      if (managed)
	delete Data;
      if (A.managed)
	Data=new mapArray(*A.Data);
      else
	Data=A.Data;
      managed=A.managed;
    }
  return *this;
}

template<typename T>
XMLdatablock<T>*
XMLdatablock<T>::clone() const
  /*!
    The clone function
    \return new copy of this
  */
{
  return new XMLdatablock<T>(*this);
}

template<typename T>
XMLdatablock<T>::~XMLdatablock()
  /*!
    Standard destructor
  */
{
  if (managed)
    delete Data;
}

template<typename T>
void
XMLdatablock<T>::setManagedComp(const mapArray* VO)
  /*!
    Set Component into XMLdatablock. This function
    manages (deletes the stored memory) and thus
    takes a copy of the pointer
    \param VO :: container object to add
   */
{
  if (managed)
    delete Data;
  
  Data=(VO) ? new mapArray(*VO) : 0;
  managed=1;

  if (!Data || !Data->size())
    this->setEmpty();
  else
    this->setEmpty(0);
  return;
}

template<typename T>
void
XMLdatablock<T>::setUnManagedComp(const mapArray* VO)
  /*!
    Set Component into XMLdatablock
    \param VO :: container object to add
   */
{
  if (managed)
    delete Data;
  Data=VO;
  managed=0;
  if (!Data || !Data->size())
    this->setEmpty();
  else
    this->setEmpty(0);

  return;
}

template<typename T>
void
XMLdatablock<T>::setSizeAttributes() 
  /*!
    Sets the size attributes
  */
{
  if (!Data) return;

  const std::vector<size_t>& shape=Data->shape();
  for(size_t i=0;i<shape.size();i++)
    {
      Attr.setAttribute("Index_"+std::to_string(i),
			std::to_string(shape[i]));
    }
  return;
}

template<typename T>
void
XMLdatablock<T>::writeXML(std::ostream& OX) const
  /*!
    Write out the object
    \param OX :: output stream
  */
{
  writeDepth(OX);
  OX<<"<"<<Key<<Attr;
  if (Data)
    {
      const std::vector<size_t>& shape=Data->shape();  
      for(int i=0;i<shape.size();i++)
	OX<<" index_"<<i<<"=\""<<shape[i]<<"\"";
    }
  if (isEmpty())
    {
      OX<<"\\>"<<std::endl;
      return;
    }
  OX<<">"<<std::endl;

  if (Data)
    {
      writeDepth(OX,2);
      const std::vector<T>& dVec=
	Data->getVector();
      int cnt(0);
      for(const T& V : dVec)
	{
	  if (cnt && !(cnt % contLine))
	    {
	      OX<<std::endl;
	      writeDepth(OX,2);
	    }
	  OX<<V<<" ";
	  cnt++;
	}  
      OX<<std::endl;
    }
  writeCloseXML(OX);
  return;
}

  
} // NAMESPACE XML

/// \cond TEMPLATE

template class XML::XMLdatablock<double>;
template class XML::XMLdatablock<DError::doubleErr>;

/// \endcond TEMPLATE
