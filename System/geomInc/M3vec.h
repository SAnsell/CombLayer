/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geomInc/M3vec.h
 *
 * Copyright (c) 2004-2024 by Stuart Ansell
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
#ifndef Geometry_M3vec_h
#define Geometry_M3vec_h

namespace Geometry 
{

template<typename T> class M3;
 
/*!
  \class M3vec
  \brief Templated matrix support: 3D point class
  \version 1.0
  \date February 2024
  \author S. Ansell

  Simple M3vec function based on x,y,z and providing
  simple dot and cross products
 */

template<typename T>
class M3vec
{
 private:
  
  T x;        ///< X-Coordinates
  T y;        ///< Y-Coordinates
  T z;        ///< Z-Coordinates

 public:
  
  M3vec();
  M3vec(const T&,const T&,const T&);
  M3vec(const M3<T>&,const size_t);
  M3vec(const size_t,const M3<T>&);
  M3vec(const M3vec<T>&) =default;
  virtual ~M3vec() =default;

  M3vec<T>& operator=(const M3vec<T>&) =default;
  M3vec<T>& operator=(M3vec<T>&&) =default;

  void setM3(const size_t,M3<T>&) const;
  void setM3(M3<T>&,const size_t) const;
  

  M3vec<T>& operator*=(const T&);
  M3vec<T>& operator*=(const M3vec&);
  
  M3vec<T>& operator/=(const T&);

  M3vec<T> operator*(const T&) const;
  M3vec<T> operator*(const M3vec<T>&) const;

  M3vec<T> operator/(const T&) const;     // Scale factor

  M3vec<T> crossNormal() const;
  T dotProd(const M3vec<T>&) const;
  T abs() const;
  T makeUnit();

  void write(std::ostream&) const;
};

template<typename T>
  std::ostream&
  operator<<(std::ostream&,const M3vec<T>&);
 
}   // NAMESPACE Geometry

#endif

