/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geomInc/Vec2D.h
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#ifndef Geometry_Vec2D_h
#define Geometry_Vec2D_h

namespace Geometry 
{

  
template<typename T> class Matrix;

/*!
  \class Vec2D
  \brief Basic 2D point class
  \version 1.1
  \date January 2016
  \author S. Ansell

  Simple Vec2D function based on x,y and providing
  simple dot and cross products
 */

class Vec2D
{
 protected:
  
  double x;        ///< X-Coordinates
  double y;        ///< Y-Coordinates
  
 public:
  
  Vec2D();
  Vec2D(const double,const double);
  explicit Vec2D(const double*);
  Vec2D(const Matrix<double>&);
  Vec2D(const Vec2D&);
  virtual ~Vec2D();

  const double& X() const { return x; }   ///< Accessor function (X)
  const double& Y() const { return y; }   ///< Accessor function (Y)

  Vec2D& operator=(const Vec2D&);
  Vec2D& operator()(const double,const double);


  double& operator[](const size_t);
  double operator[](const size_t) const;

  
  bool operator<(const Vec2D&) const;
  bool operator>(const Vec2D&) const;
  
  Vec2D& operator*=(const double);
  Vec2D& operator/=(const double);
  Vec2D& operator+=(const Vec2D&);
  Vec2D& operator-=(const Vec2D&);

  Vec2D operator*(const double) const;     // Scale factor
  Vec2D operator/(const double) const;     // Scale factor
  Vec2D operator+(const Vec2D&) const;
  Vec2D operator-(const Vec2D&) const;
  Vec2D operator-() const;
 
  bool operator==(const Vec2D&) const;
  bool operator!=(const Vec2D&) const;
  void rotate(const double);
  void rotate(const Vec2D&,const double);
  void rotate(const Matrix<double>&);

  double Distance(const Vec2D&) const;    ///< Calculate scale distance
  double makeUnit();                      ///< Convert into unit vector
  Geometry::Vec2D unit() const; 
  Geometry::Vec2D component(const Geometry::Vec2D&) const; 
  Geometry::Vec2D cutComponent(const Geometry::Vec2D&) const;
  ///< Calculate the volmue of a cube X*Y
  double volume() const { return fabs(x*y); }

  double dotProd(const Vec2D&) const;
  double abs() const;    

  Vec2D& boundaryCube(const Vec2D&,const Vec2D&); 
  int forwardBase(const Vec2D&,const Vec2D&,const Vec2D&);   
  int reBase(const Vec2D&,const Vec2D&);   
  int masterDir(const double= Geometry::zeroTol) const;               
  size_t principleDir() const;
  int nullVector(const double= Geometry::zeroTol) const;             
  int coLinear(const Vec2D&,const Vec2D&) const;
  Vec2D crossNormal() const; 

  void read(std::istream&);
  void write(std::ostream&) const;
};


std::ostream& operator<<(std::ostream&,const Vec2D&);
std::istream& operator>>(std::istream&,Vec2D&);

}   // NAMESPACE Geometry

#endif

