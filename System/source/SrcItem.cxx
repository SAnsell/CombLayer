/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   src/SrcItem.cxx
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
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cmath>
#include <string>
#include <vector>
#include <map>
#include <boost/format.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "masterWrite.h"
#include "SrcItem.h"

namespace SDef
{

template<typename OutType>
SrcItem<OutType>::SrcItem(const std::string& K) : 
  SrcBase(),KeyType(K),active(0),dtype(9999),
  Data()
  /*!
    Constructor
    - note dtype must hold a value : but in this
    case it will give an error in the writing and 
    9999 is way beyond the allowed range.
    \param K :: Keyname
  */
{}

template<typename OutType>
SrcItem<OutType>::SrcItem(const SrcItem& A) :
  SrcBase(A),KeyType(A.KeyType),depKey(A.depKey),
  active(A.active),dtype(A.dtype),
  Data(A.Data)
  /*!
    Copy Constructor
    \param A :: SrcItem to copy
  */
{}

template<typename OutType>
SrcItem<OutType>&
SrcItem<OutType>::operator=(const SrcItem& A)
  /*!
    Copy Constructor
    \param A :: SrcItem to copy
    \return 
  */
{
  if (this!=&A)
    {
      SrcBase::operator=(A);
      depKey=A.depKey;
      active=A.active;
      dtype=A.dtype;
      Data=A.Data;
    }
  return *this;
}

template<typename OutType>
SrcItem<OutType>*
SrcItem<OutType>::clone() const
  /*!
    Virtual copy constructor
    \return SrcItem object
  */
{
  return new SrcItem<OutType>(*this);
}

template<typename OutType>
SrcItem<OutType>::~SrcItem()
  /*! 
    Destructor
  */
{}

template<typename OutType>
void
SrcItem<OutType>::setDataType(const int I)
  /*!
    Set a data item dependence
    \param I :: Index of data item
  */
{
  ELog::RegMethod RegA("StrItem","setDataType");
  if (!I)
    ELog::EM<<"Setting dtype = 0 in a non-data object"<<ELog::endErr;
  active=1;
  dtype=I;
  return;
}

template<typename OutType>
void
SrcItem<OutType>::setDepType(const std::string& dK,const int I)
  /*!
    Set a data item dependence
    \param dK :: Dependent key
    \param I :: Index of data item
  */
{
  ELog::RegMethod RegA("StrItem","setDepType");
  active=1;
  depKey=dK;
  dtype=(I>0) ? -I : I;   // negative always
  if (dtype==0) 
    ELog::EM<<"Setting dtype = 0 in a non-data object"<<ELog::endErr;

  return;
}

template<typename OutType>
void
SrcItem<OutType>::setValue(const OutType& V)
  /*!
    Convert the Item to a Value outtype 
    Sets active and cancels the dPtr
    \param V :: Value to set
  */
{
  Data=V;
  active=1;
  dtype=0;
  return;
}


template<typename OutType>
std::string
SrcItem<OutType>::getString() const
  /*!
    Write out the item to a string
    \return String
  */
{
  if (!active) return "";

  masterWrite& MW=masterWrite::Instance();
 
  std::ostringstream cx;
  cx<<KeyType<<"=";
  // d type
  if (dtype>0)
    cx<<"d"<<dtype<<" ";
  else if (dtype<0)
    cx<<"f"<<depKey<<"=d"<<-dtype<<" ";
  else
    cx<<MW.Num(Data)<<" ";
  
  return cx.str();
}

///\cond TEMPLATE

template class SrcItem<double>;
template class SrcItem<int>;
template class SrcItem<Geometry::Vec3D>;

///\endcond TEMPLATE

}  // NAMESPACE SDef
