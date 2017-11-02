/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   funcBase/FValue.cxx
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
#include <sstream>
#include <string>
#include <cmath>
#include <climits>
#include <vector>
#include <map>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Code.h"
#include "FItem.h"
#include "varList.h"

//  ---------------------------------------
//                FVALUE
//  ---------------------------------------

template<typename T>
FValue<T>::FValue(varList* VA,const int I,const T& V) : FItem(VA,I),
  Value(V)
  /*!
    Constructor 
    \param VA :: VarList pointer
    \param I :: Index value
    \param V :: Value to set
  */
{}

template<typename T> 
FValue<T>::FValue(const FValue<T>& A) : 
  FItem(A),Value(A.Value)
  /*!
    Standard copy constructor
    \param A :: FValue object to copy
  */
{}

template<typename T> 
FValue<T>&
FValue<T>::operator=(const FValue<T>& A) 
  /*!
    Standard assignment operator
    \param A :: FValue object to copy
    \return *this
  */
{
  if (this!=&A)
    {
      FItem::operator=(A);
      Value=A.Value;
    }
  return *this;
}

template<typename T>
FValue<T>*
FValue<T>::clone() const
  /*!
    Clone class
    \return new Value(*this)
  */
{
  return new FValue<T>(*this);
}

template<typename T>
FValue<T>::~FValue()
  /*!
    Desturctor
  */
{}

// -----------------------------------------
//              SET VALUE
// -----------------------------------------


template<typename T>
void
FValue<T>::setValue(const Geometry::Vec3D&)
  /*!
    Sets the values 
  */
{
  ELog::RegMethod RegA("FValue","setValue<Vec3D>");
  throw ColErr::ExBase(0,"Incorrect Type"+RegA.getFull());  
  return;
}

template<typename T>
void
FValue<T>::setValue(const double& V)
  /*!
    Sets the values 
    \param V :: New value 
  */
{
  Value=static_cast<T>(V);
  return;
}

template<typename T>
void
FValue<T>::setValue(const int& V)
  /*!
    Sets the values 
    \param V :: New value 
  */
{
  Value=static_cast<T>(V);
  return;
}

template<typename T>
void
FValue<T>::setValue(const long int& V)
  /*!
    Sets the values 
    \param V :: New value 
  */
{
  Value=static_cast<T>(V);
  return;
}

template<typename T>
void
FValue<T>::setValue(const size_t& V)
  /*!
    Sets the values 
    \param V :: New value 
  */
{
  Value=static_cast<T>(V);
  return;
}

template<typename T>
void
FValue<T>::setValue(const std::string& V)
  /*!
    Sets the values 
    \param V :: New value 
  */
{
  ELog::RegMethod RegA("FValue","setValue");
  if (!StrFunc::convert(V,Value))
    throw ColErr::InvalidLine(V,RegA.getFull(),0);
  return;
}

template<>
void
FValue<std::string>::setValue(const double& V)
  /*!
    Sets the values 
    \param V :: New value 
  */
{
  std::stringstream cx;
  cx<<V;
  Value=cx.str();
  return;
}

template<>
void
FValue<std::string>::setValue(const int& V)
  /*!
    Sets the values 
    \param V :: New value 
  */
{
  std::stringstream cx;
  cx<<V;
  Value=cx.str();
  return;
}

template<>
void
FValue<std::string>::setValue(const long int& V)
  /*!
    Sets the values 
    \param V :: New value 
  */
{
  std::stringstream cx;
  cx<<V;
  Value=cx.str();
  return;
}

template<>
void
FValue<std::string>::setValue(const size_t& V)
  /*!
    Sets the values 
    \param V :: New value 
  */
{
  std::stringstream cx;
  cx<<V;
  Value=cx.str();
  return;
}

template<>
void
FValue<std::string>::setValue(const std::string& V)
  /*!
    Sets the values 
    \param V :: New value 
  */
{
  Value=V;
  return;
}

template<>
void
FValue<Geometry::Vec3D>::setValue(const double&)
  /*!
    Sets the value 
  */
{
  ELog::RegMethod RegA("FValue<Vec3D>","setValue(double)");
  throw ColErr::ExBase(0,"Incorrect Type");
  return;
}

template<>
void
FValue<Geometry::Vec3D>::setValue(const int&)
  /*!
    Sets the value 
  */
{
  ELog::RegMethod RegA("FValue<Vec3D>","setValue(int)");
  throw ColErr::ExBase(0,"Incorrect Type");
  return;
}

template<>
void
FValue<Geometry::Vec3D>::setValue(const long int&)
  /*!
    Sets the value 
  */
{
  ELog::RegMethod RegA("FValue<Vec3D>","setValue(int)");
  throw ColErr::ExBase(0,"Incorrect Type");
  return;
}

template<>
void
FValue<Geometry::Vec3D>::setValue(const size_t&)
  /*!
    Sets the value 
  */
{
  ELog::RegMethod RegA("FValue<Vec3D>","setValue(size_t)");
  throw ColErr::ExBase(0,"Incorrect Type"+RegA.getFull());
  return;
}


template<>
void
FValue<Geometry::Vec3D>::setValue(const std::string& V)
  /*!
    Sets the values 
    \param V :: New value 
  */
{
  ELog::RegMethod RegA("FValue<Vec3D>","setValue(string)");

  std::string X=V;
  Geometry::Vec3D A;
  if (!StrFunc::section(X,A[0]) ||
      !StrFunc::section(X,A[1]) ||
      !StrFunc::section(X,A[2]))
    throw ColErr::InvalidLine(V,RegA.getFull(),0);

  Value=A;
  return;
}

template<>
void
FValue<Geometry::Vec3D>::setValue(const Geometry::Vec3D& V)
  /*!
    Sets the value
    \param V :: New value 
  */
{
  Value=V;
  return;
}

// -----------------------------------------
//              GET VALUE
// -----------------------------------------


template<typename T>
int
FValue<T>::getValue(Geometry::Vec3D&) const
  /*!
    Sets the value
    \return 0 (never possible)
  */
{
  return 0;
}

template<typename T>
int
FValue<T>::getValue(double& V) const
  /*!
    Sets the values 
    \param V :: New value 
    \return 1 (always possible)
  */
{
  V=static_cast<double>(Value);
  const_cast<int&>(active)++;
  return 1;
}

template<typename T>
int
FValue<T>::getValue(int& V) const
  /*!
    Sets the values 
    \param V :: New value 
    \return 1 (always possible)
  */
{
  V=static_cast<int>(Value);
  const_cast<int&>(active)++;
  return 1;
}


template<typename T>
int
FValue<T>::getValue(long int& V) const
  /*!
    Sets the values 
    \param V :: New value 
    \return 1 (always possible)
  */
{
  V=static_cast<long int>(Value);
  const_cast<int&>(active)++;
  return 1;
}

template<typename T>
int
FValue<T>::getValue(size_t& V) const
  /*!
    Sets the values 
    \param V :: New value 
    \return 1 (always possible)
  */
{
  if (Value<0)
    V=ULONG_MAX;
  else
    V=static_cast<size_t>(Value);
  const_cast<int&>(active)++;
  return 1;
}

template<typename T>
int
FValue<T>::getValue(std::string& V) const
  /*!
    Puts the value into V
    \param V :: Output variable
    \return 0 if not possbile  (1 if valid)
  */
{
  std::stringstream cx;
  cx<<Value;
  V=cx.str();
  const_cast<int&>(active)++;
  return 1;
}

template<>
int
FValue<std::string>::getValue(Geometry::Vec3D& V) const
  /*!
    Puts the value into V
    \param V :: Output variable
    \return 0 if not possbile  (1 if valid)
  */
{
  ELog::RegMethod RegA("FValue","getValue(Vec3D)");

  if (!StrFunc::convert(Value,V))
    return 0;
  const_cast<int&>(active)++;
  return 1;
}

template<>
int
FValue<std::string>::getValue(double& V) const
  /*!
    Puts the value into V
    \param V :: Output variable
    \return 0 if not possbile  (1 if valid)
  */
{
  ELog::RegMethod RegA("FValue","getValue(double)");
  if (!StrFunc::convert(Value,V))
    return 0;
  const_cast<int&>(active)++;
  return 1;
}

template<>
int
FValue<std::string>::getValue(long int& V) const
  /*!
    Puts the value into V
    \param V :: Output variable
    \return 0 if not possbile  (1 if valid)
  */
{
  ELog::RegMethod RegA("FValue","getValue(long int )");
  if (!StrFunc::convert(Value,V))
    return 0;
  const_cast<int&>(active)++;
  return 1;
}

template<>
int
FValue<std::string>::getValue(size_t& V) const
  /*!
    Puts the value into V
    \param V :: Output variable
    \return 0 if not possbile  (1 if valid)
  */
{
  ELog::RegMethod RegA("FValue","getValue(size_t)");
  if (!StrFunc::convert(Value,V))
    return 0;
  const_cast<int&>(active)++;
  return 1;
}

template<>
int
FValue<std::string>::getValue(int& V) const
  /*!
    Puts the value into V
    \param V :: Output variable
    \return 0 if not possbile  (1 if valid)
  */
{
  ELog::RegMethod RegA("FValue","getValue(int)");
  if (!StrFunc::convert(Value,V))
    return 0;
  const_cast<int&>(active)++;
  return 1;
}


template<>
int
FValue<std::string>::getValue(std::string& V) const
  /*!
    Puts the value into V
    \param V :: Output variable
    \return 1 as convertable (always)
  */
{
  V=Value;
  const_cast<int&>(active)++;
  return 1;
}


template<>
int
FValue<Geometry::Vec3D>::getValue(Geometry::Vec3D& V) const
  /*!
    Puts the value into V
    \param V :: Output variable
    \return 1 as convertable (always)
  */
{
  V=Value;
  const_cast<int&>(active)++;
  return 1;
}

template<>
int 
FValue<Geometry::Vec3D>::getValue(double&) const
  /*!
    Puts the value into V
    \return  false [not possible]
  */
{
  return 0; 
}

template<>
int
FValue<Geometry::Vec3D>::getValue(long int&) const
  /*!
    Puts the value into V
    \return  false [not possible]
  */
{
  return 0;
}

template<>
int
FValue<Geometry::Vec3D>::getValue(size_t&) const
  /*!
    Puts the value into V
    \return  false [not possible]
  */
{
  return 0;
}

template<>
int
FValue<Geometry::Vec3D>::getValue(int&) const
  /*!
    Puts the value into V
    \return 0 as invalid
  */
{
  return 0;
}


template<>
int
FValue<Geometry::Vec3D>::getValue(std::string& V) const
  /*!
    Puts the value into V
    \param V :: Output variable
    \return 1 as convertable (always)
  */
{
  std::ostringstream cx;
  cx<<Value;
  V=cx.str();
  const_cast<int&>(active)++;
  return 1;
}

template<typename T>
void
FValue<T>::write(std::ostream& OX) const
  /*!
    Write out the variable
    \param OX :: Output stream
   */
{
  OX<<Value;
  return;
}

/// \cond simpleTemplate

template<>
std::string FValue<Geometry::Vec3D>::typeKey()  const
{  return "Geometry::Vec3D"; }

template<>
std::string FValue<double>::typeKey()  const
{  return "double"; }

template<>
std::string FValue<int>::typeKey()  const
{  return "int"; }

template<>
std::string FValue<size_t>::typeKey()  const
{  return "size_t"; }

template<>
std::string FValue<long int>::typeKey()  const
{  return "long int"; }

template<>
std::string FValue<std::string>::typeKey()  const
{  return "std::string"; }

/// \endcond simpleTemplate

/// \cond TEMPLATE

template class FValue<Geometry::Vec3D>;
template class FValue<double>;
template class FValue<int>;
template class FValue<long int>;
template class FValue<size_t>;
template class FValue<std::string>;

/// \endcond TEMPLATE
