/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   src/Triple.cxx
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#include <string>
#include <cmath>
#include <vector>

#include "Exception.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Triple.h"

template<typename T>
Triple<T>::Triple() 
  /*!
    Standard Constructor
  */
{}
  
template<typename T>
Triple<T>::Triple(const Triple<T>& A) :
  first(A.first),second(A.second),third(A.third)
  /*!
    Standard Copy Constructor
    \param A :: Triple Item to copy
  */
{}

template<typename T>
Triple<T>::Triple(const T& A,const T& B,const T& C) :
  first(A),second(B),third(C)
  /*!
    Constructor from a 3 value input 
    \param A :: first item
    \param B :: second item
    \param C :: third item
  */
{}

template<typename T>
Triple<T>&
Triple<T>::operator=(const Triple<T>& A)
  /*!
    Standard Assignment Constructor
    \param A :: Triple Item to copy
    \return *this
  */
{
  if (this!=&A)
    {
      first=A.first;
      second=A.second;
      third=A.third;
    }
  return *this;
}

template<typename T>
Triple<T>::~Triple()
  /*!
    Standard Destructor
  */
{}

template<typename T>
Triple<T>&
Triple<T>::operator()(const T& a,const T& b,const T& c)
  /*! 
    Assignment operator
    \param a :: First item
    \param b :: Second item
    \param c :: Third item
    \return Triple(a,b,c)
  */
{
  first=a;
  second=b;
  third=c;
  return *this;
}


template<typename T>
bool
Triple<T>::operator==(const Triple<T>& A) const
  /*! 
    Operator== all components must be equal
    \param A :: Triple to compare
    \return True/false
  */
{
  return  (first!=A.first || second!=A.second || 
	   third!=A.third) ? 0 : 1;
}

template<typename T>
bool
Triple<T>::operator!=(const Triple<T>& A) const
  /*! 
    Operator!= any component is not equal
    \param A :: Other object to compare
    \return this!=A
  */
{
  return  (first==A.first && second==A.second &&
	   third==A.third) ? 0 : 1;
}

template<>
bool
Triple<Geometry::Vec3D>::operator<(const Triple<Geometry::Vec3D>& A) const
  /*! 
    Operator< takes first to last precidence.
    Special case for Vec3D based on the volume of the triple
    \param A :: Triple to compare
    \return this < A
  */
{
  const double Avol=A.first.dotProd(A.second*A.third);
  const double Tvol=first.dotProd(second*third);
  return (Tvol<Avol);
}

template<typename T>
bool
Triple<T>::operator<(const Triple<T>& A) const
  /*! 
    Operator< takes first to last precidence.
    \param A :: Triple to compare
    \return this < A
  */
{
  if (first>A.first)
    return 0;
  if (first<A.first)
    return 1;
  if (second>A.second)
    return 0;
  if (second<A.second)
    return 1;
  if (third>=A.third)
    return 0;
  return 1;
}

template<typename T>
bool
Triple<T>::operator>(const Triple<T>& A) const
  /*! 
    Operator> takes first to last precidence.
    Uses operator<  to obtain value.
    Note it does not uses 1-(A<this)
    \param A :: Triple to compare
    \return this > A
  */
{
  return A.operator<(*this);
}

template<typename T>
T&
Triple<T>::operator[](const size_t A)
  /*!
    Accessor Reference Function
    \param A :: Index to item to get 0-2
    \return Reference Item[A]
  */
{
  switch (A)
    {
    case 0:
      return first;
    case 1:
      return second;
    case 2:
      return third;
    }
  throw ColErr::IndexError<size_t>(A,3,"Triple::operator[]");
}

template<typename T>
const T&
Triple<T>::operator[](const size_t A) const
  /*!
    Accessor Value Function
    \param A :: Index to item to get 0-2
    \return Item[A]
  */
{
  switch (A)
    {
    case 0:
      return first;
    case 1:
      return second;
    case 2:
      return third;
    }
  throw ColErr::IndexError<size_t>(A,3,"Triple::operator[] const");
}


/* 
------------------------------------------------------------------
    Different Triple 
------------------------------------------------------------------   
*/

template<typename F,typename S,typename T>
DTriple<F,S,T>::DTriple() 
  /*!
    Standard Constructor
  */
{}
  
template<typename F,typename S,typename T>
DTriple<F,S,T>::DTriple(const DTriple<F,S,T>& A) :
  first(A.first),second(A.second),third(A.third)
  /*!
    Standard Copy Constructor
    \param A :: DTriple Item to copy
  */
{}

template<typename F,typename S,typename T>
DTriple<F,S,T>::DTriple(const F& A,const S& B,const T& C) :
  first(A),second(B),third(C)
  /*!
    Constructor from a 3 value input 
    \param A :: first item
    \param B :: second item
    \param C :: third item
  */
{}

template<typename F,typename S,typename T>
DTriple<F,S,T>&
DTriple<F,S,T>::operator=(const DTriple<F,S,T>& A)
  /*!
    Assignment from a 3 value input 
    \param A :: DTriple to copy from 
    \returns *this
  */
{
  if (this!=&A)
    {
      first=A.first;
      second=A.second;
      third=A.third;
    }
  return *this;
}

template<typename F,typename S,typename T>
DTriple<F,S,T>::~DTriple()
  /*!
    Standard Destructor
  */
{}

template<typename F,typename S,typename T>
DTriple<F,S,T>&
DTriple<F,S,T>::operator()(const F& a,const S& b,const T& c)
  /*! 
    Assignment operator
    \param a :: First item
    \param b :: Second item
    \param c :: Third item
    \return DTriple(a,b,c)
  */
{
  first=a;
  second=b;
  third=c;
  return *this;
}

template<typename F,typename S,typename T>
bool
DTriple<F,S,T>::operator==(const DTriple<F,S,T>& A) const
  /*! 
    Operator== all components must be equal
    \param A :: Object to compare
    \returns A==*this
  */
{
  return  (first!=A.first || second!=A.second || 
	   third!=A.third) ? 0 : 1;
}

template<typename F,typename S,typename T>
bool
DTriple<F,S,T>::operator!=(const DTriple<F,S,T>& A) const
  /*! 
    Operator!= any component is not equal
    \param A :: DTriple to compare
    \return true/false 
  */
{
  return  (first==A.first && second==A.second &&
	   third==A.third) ? 0 : 1;
}

template<typename F,typename S,typename T>
bool
DTriple<F,S,T>::operator<(const DTriple<F,S,T>& A) const
  /*! 
    Operator< takes first to last precidence.
    \param A :: Triple to compare
    \return this < A
  */
{
  if (first>A.first)
    return 0;
  if (first<A.first)
    return 1;
  if (second>A.second)
    return 0;
  if (second<A.second)
    return 1;
  return (third<A.third) ? 1 : 0;
}

template<typename F,typename S,typename T>
bool
DTriple<F,S,T>::operator>(const DTriple<F,S,T>& A) const
  /*! 
    Operator> takes first to last precidence.
    \param A :: Triple to compare
    \return this < A
  */
{
  return A.operator<(*this);
}

/// \cond TEMPLATE

class Rule;
namespace Geometry
{
  class Surface;
}

template class DTriple<Rule*,int,Rule*>;
template class DTriple<std::string,int,int>;
template class DTriple<std::string,std::string,double>;
template class DTriple<int,int,double>;
template class DTriple<int,double,int>;
template class DTriple<int,int,std::string>;
template class DTriple<int,std::string,double>;
template class DTriple<int,double,std::string>;
template class DTriple<double,size_t,Geometry::Vec3D>;
template class DTriple<double,int,Geometry::Vec3D>;
template class DTriple<double,Geometry::Vec3D,Geometry::Vec3D>;
template class DTriple<int,int,Geometry::Surface*>;
template class DTriple<std::string,Geometry::Vec3D,int>;
template class DTriple<std::string,Geometry::Vec3D,Geometry::Vec3D>;
template class Triple<int>;
template class Triple<long int>;
template class Triple<size_t>;
template class Triple<double>;
template class Triple<std::string>;
template class Triple<Geometry::Vec3D>;

/// \endcond TEMPLATE
