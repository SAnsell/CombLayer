/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   include/Triple.h
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
#ifndef Triple_h
#define Triple_h

/*!
  \class Triple
  \brief Triple of three identical types
  \author S. Ansell
  \date April 2005
  \version 1.0
  
  Class maintians a type first/second/third triple
  similar to std::pair except all are identical
*/

template<typename T>
class Triple
{
  public:
  
  T first;       ///< First item
  T second;      ///< Second item
  T third;       ///< Third item

  Triple();
  Triple(const Triple<T>&);
  Triple(const T&,const T&,const T&);
  Triple<T>& operator=(const Triple<T>&);
  ~Triple();

  const T& operator[](const size_t) const;
  T& operator[](const size_t);
  Triple<T>& operator()(const T&,const T&,const T&);

  bool operator<(const Triple<T>&) const;
  bool operator>(const Triple<T>&) const;
  bool operator==(const Triple<T>&) const;
  bool operator!=(const Triple<T>&) const;

};


/*!
  \class DTriple
  \brief Triple of three different things
  \author S. Ansell
  \date April 2005
  \version 1.0
  
  Class maintians a different type first/second/third triple
  All are of a different type
*/

template<typename F,typename S,typename T>
class DTriple 
{
  public:
  
  F first;         ///< First item
  S second;        ///< Second item
  T third;         ///< Third item

  DTriple();
  DTriple(const DTriple<F,S,T>&);
  DTriple(const F&,const S&,const T&);
  DTriple<F,S,T>& operator=(const DTriple<F,S,T>&);
  ~DTriple();

  DTriple<F,S,T>& operator()(const F&,const S&,const T&);

  bool operator<(const DTriple<F,S,T>&) const;
  bool operator>(const DTriple<F,S,T>&) const;
  bool operator==(const DTriple<F,S,T>&) const;
  bool operator!=(const DTriple<F,S,T>&) const;

};

#endif
