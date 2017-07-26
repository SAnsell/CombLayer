/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geometry/Vec3D.cxx
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
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <cmath>
#include <vector>
#include <string>
#include <array>
#include <algorithm>

#include "Exception.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"

std::ostream& 
Geometry::operator<<(std::ostream& OX,const Geometry::Vec3D& A)
  /*!
    Calls Vec3D method write to output class
    \param OX :: Output stream
    \param A :: Vec3D to write
    \return Current state of stream
  */
{
  A.write(OX);
  return OX;
}

std::istream& 
Geometry::operator>>(std::istream& IX,Geometry::Vec3D& A)
  /*!
    Calls Vec3D method read to input class
    \param IX :: Input stream
    \param A :: Vec3D to read
    \return Current state of stream
  */
{
  A.read(IX);
  return IX;
}

namespace Geometry
{


Vec3D::Vec3D():
  x(0.0),y(0.0),z(0.0)
  /*!
    Origin Constructor
  */
{}

Vec3D::Vec3D(const double X,const double Y,const double Z) :
  x(X),y(Y),z(Z)
  /*!
    Constructor at Positon
    \param X :: x-coord
    \param Y :: y-coord
    \param Z :: z-coord
  */
{}

Vec3D::Vec3D(const double* xyz) :
  x(xyz[0]),y(xyz[1]),z(xyz[2])
  /*!
    Constructor from array
    \param xyz :: 3 Vec3D array
  */
{}

Vec3D::Vec3D(const Vec3D& A) :
  x(A.x),y(A.y),z(A.z)
  /*!
    Copy constructor
    \param A :: Vec3D to copy
  */
{}

Vec3D&
Vec3D::operator=(const Vec3D& A)
  /*!
    Assignment operator
    \param A :: Vec3D to copy
    \return *this
  */
{
  if (this!=&A)
    {
      x=A.x;
      y=A.y;
      z=A.z;
    }
  return *this;
}

Vec3D::~Vec3D()
  /*!
    Standard Destructor
  */
{}

Vec3D&
Vec3D::operator()(const double a,const double b,const double c) 
  /*!
    Operator() casts a set of double to a Vec3D
    \param a :: x-cord
    \param b :: y-cord
    \param c :: z-cord
    \return New point
  */
{
  x=a;
  y=b;
  z=c;
  return *this;
}

template<typename T>
Vec3D
Vec3D::operator()(const Matrix<T>& A) const
  /*!
    Operator() casts Matrix to a point
    The matrix needs to be 3x1 or 1x3
    \param A :: Matrix Item to be case
    \return Vec3D of matrix
  */
{
  std::pair<size_t,size_t> Asize=A.size();
  if (Asize.first*Asize.second<1)
    return Vec3D(0,0,0);
  Vec3D Out;
  if (Asize.first>Asize.second) 
    {
      for(size_t i=0;i<3;i++)
	Out[i]=(i<Asize.first) ? A[i][0] : 0.0;
    }
  else
    {
      for(size_t i=0;i<3;i++)
	Out[i]=(i<Asize.second) ? A[0][i] : 0.0;
    }
  return Out;
}

template<typename IT>
double&
Vec3D::operator[](const IT A)
  /*!
    Operator [] isolates component based on 
    index A 
    \param A :: Index Item
    \throw IndexError :: A out of range
    \return reference to coordinate
  */
{
  switch (A)
    {
    case 0:
      return x;
    case 1:
      return y;
    case 2:
      return z;
    }
  throw ColErr::IndexError<IT>(A,2,"Vec3D::operator[]");
}

template<typename IT>
double
Vec3D::operator[](const IT A) const
  /*!
    Operator [] isolates component based on 
    index A
    \param A :: Index number (0-2) 
    \return value of the coordinate (z on error)
  */
{
  switch (A)
    {
    case 0:
      return x;
    case 1:
      return y;
    case 2:
      return z;
    }
  throw ColErr::IndexError<IT>(A,2,"Vec3D::operator[] const");
}

bool
Vec3D::operator==(const Vec3D& A) const
  /*!
    Equality operator within tolerance
    \param A :: Vec3D to compare
    \return A==this
  */
{
  return (&A==this || Distance(A)<=zeroTol) ? 1 : 0;
}

bool
Vec3D::operator!=(const Vec3D& A) const
  /*!
    Not Equal operator within tolerance
    \param A :: Vec3D to compare
    \return A!=this
  */
{
  return (this->operator==(A)) ? 0 : 1;
}

bool
Vec3D::operator<(const Vec3D& A) const
  /*!
    Less than operator based on size
    \param A :: Vec3D to compare
    \return this<A
  */
{
  const double FV(this->abs()-A.abs());
  return (FV<zeroTol) ? 0 : 1;
}

bool
Vec3D::operator>(const Vec3D& A) const
  /*!
    Less than operator based on size
    \param A :: Vec3D to compare
    \return this<A
  */
{
  const double FV(A.abs()-this->abs());
  return (FV<zeroTol) ? 0 : 1;
}

Vec3D
Vec3D::operator*(const Vec3D& A) const
  /*!
    Cross product of this*A
    \param A :: Vec3D to take cross product from
    \returns cross product
  */  
{
  Vec3D X;
  X.x=y*A.z-z*A.y;
  X.y=z*A.x-x*A.z;
  X.z=x*A.y-y*A.x;
  return X;
}

template<typename T>
Vec3D
Vec3D::operator*(const Matrix<T>& A) const
  /*!
    Impliments a rotation 
    \param A :: Matrix to rotate by
    \returns Vec3D rotated by Matrix
  */
  
{
  Vec3D X(*this);
  X.rotate(A);
  return X;
}

Vec3D
Vec3D::operator*(const double V) const
  /*!
    Simple multiplication of this/Value 
    \param V :: scalar to multiply each component 
    \return Vec3D scaled by value
  */
{
  Vec3D X(*this);
  X*=V;
  return X;
}


Vec3D
Vec3D::operator/(const double V) const
  /*!
    Simple division of this/V 
    \param V :: Value to divide by.
    \returns Vec3D / Value
  */
{
  Vec3D X(*this);
  X/=V;
  return X;
}

Vec3D
Vec3D::operator+(const Vec3D& A) const
  /*! 
    Simple Vec3D addition
    \param A :: Vec3D to add
    \returns Vec3D + A
  */
{
  Vec3D X(*this);
  X+=A;
  return X;
}

Vec3D
Vec3D::operator-(const Vec3D& A) const
  /*!
    Simple Vec3D subtraction
    \param A : Vec3D to subtract
    \returns Vec3D - A
  */
{
  Vec3D X(*this);
  X-=A;
  return X;
}

Vec3D
Vec3D::operator-() const
  /*!
    Simple negation of the Vec3D
    \return -Vec3D
  */
{
  Vec3D X(*this);
  X *= -1.0;
  return X;
}

Vec3D&
Vec3D::operator*=(const Vec3D& A)
  /*!
    Cross produce of this*A
    \param A :: Vec3D 
    \return this X A
  */
{
  *this = this->operator*(A);
  return *this;
}

template<typename T> 
Vec3D&
Vec3D::operator*=(const Matrix<T>& A)
  /*!
    Rotate this by matrix A
    \param A :: Rotation Matrix (3x3)
    \return this after Rot
  */
{
  rotate(A);
  return *this;
}

Vec3D&
Vec3D::operator*=(const double V)
  /*!
    Vec3D multication by a value
    \param V :: Multiplication value
    \return *this * V
  */
{
  x*=V;
  y*=V;
  z*=V;
  return *this;
}

Vec3D&
Vec3D::operator/=(const double V)
  /*!
    Vec3D division by a value 
    (contains simple test for zero)
    \param V :: Value operator
    \return *this/V
  */
{
  if (fabs(V)>1e-250)
    {
      x/=V;
      y/=V;
      z/=V;
    }
  return *this;
}

Vec3D&
Vec3D::operator+=(const Vec3D& A)
  /*!
    Vec3D self addition 
    \param A :: Vec3D to add
    \return *this + A
  */
{
  x+=A.x;
  y+=A.y;
  z+=A.z;
  return *this;
}

Vec3D&
Vec3D::operator-=(const Vec3D& A)
  /*!
    Vec3D self subtraction 
    \param A :: Vec3D to subtract
    \return (this - A)
  */
{
  x-=A.x;
  y-=A.y;
  z-=A.z;
  return *this;
}

int 
Vec3D::masterDir(const double Tol) const
  /*! 
     Calculates the index of the primary direction (if there is one)
     \param Tol :: Tolerance accepted
     \retval range -3,-2,-1 1,2,3  if the vector
     is orientaged within Tol on the x,y,z direction (the sign
     indecates the direction to the +ve side )
     \retval 0 :: No master direction
  */
{
  // Calc max dist
  double max=x*x; 
  double other=max;
  double u2=y*y;
  int idx=(x>0) ? 1 : -1;
  if (u2>max)
    {
      max=u2;
      idx=(y>0) ? 2 : -2;
    }
  other+=u2;
  u2=z*z;
  if (u2>max)
    {
      max=u2;
      idx=(z>0) ? 3 : -3;
    }
  other+=u2;
  other-=max;
  if ((other/max)>Tol)    //doesn't have master direction
    return 0;
  return idx;
}

size_t 
Vec3D::principleDir() const
  /*! 
     Calculates the index of the primary direction
     \retval range 0,1,2 :: direction of maximum 
  */
{
  size_t maxI=0;
  double maxV=0.0;
  for(size_t i=0;i<3;i++)
    if (std::abs(this->operator[](i))>maxV)
      {
	maxV=std::abs(this->operator[](i));
	maxI=i;
      }
  return maxI;
}

double
Vec3D::Distance(const Vec3D& A) const
  /*! 
    Determine the distance bwtween points
    \param A :: Vec3D 
    \return :: the distance between A and this 
  */
{
  return sqrt((A.x-x)*(A.x-x)+
	      (A.y-y)*(A.y-y)+
	      (A.z-z)*(A.z-z));
}

double
Vec3D::makeUnit()
  /*!
    Make the vector a unit vector 
    \return :: the old magnitude 
  */
{
  const double Sz(abs());
  if (Sz>zeroTol)
    {
      x/=Sz;
      y/=Sz;
      z/=Sz;
    }
  return Sz;
}

Geometry::Vec3D
Vec3D::unit() const
  /*!
    Make the vector a unit vector 
    \return :: the vector as a unit form
  */
{
  Geometry::Vec3D Out(*this);
  Out.makeUnit();
  return Out;
}

Geometry::Vec3D
Vec3D::component(const Geometry::Vec3D& A) const
  /*!
    Make a component vector along the direction
    of A.
    \param A :: Vector to give principle direction 
    \return :: the vector as a unit form
  */
{
  const Geometry::Vec3D N=A.unit();    // Need to use A in unit form
  return N*N.dotProd(*this);
}

Geometry::Vec3D
Vec3D::cutComponent(const Geometry::Vec3D& A) const
  /*!
    Remove the component of A from the vector
    \param A :: Vector to give principle direction 
    \return :: the vector as a unit form
  */
{
  
  Geometry::Vec3D Out(*this);
  const Geometry::Vec3D N=A.unit();    // Need to use A in unit form
  Out -= N*Out.dotProd(N);
  return Out;
}

double
Vec3D::dotProd(const Vec3D& A) const
  /*!
    Calculate the dot product.
    \param A :: vector to take product from
    \returns this.A
  */
{
  return A.x*x+A.y*y+A.z*z;
}

int 
Vec3D::nullVector(const double Tol) const
  /*! 
    Checks the size of the vector
    \param Tol :: size of the biggest zero vector allowed.
    \retval 1 : the vector squared components
    magnitude are less than Tol 
    \retval 0 :: Vector bigger than Tol
  */
{
  return ((x*x+y*y+z*z)>Tol) ? 0 :1;
}


double
Vec3D::abs() const
  /*!
    Calculate the magnatude of the point
    \returns \f$ | this | \f$ 
  */
{
  return sqrt(x*x+y*y+z*z);
}

template<typename T>
void
Vec3D::rotate(const Matrix<T>& A)
  /*!
    Rotate a point by a matrix 
    \param A :: Rotation matrix (needs to be 3x3)
  */
{
  Matrix<T> Pv(3,1);
  Pv[0][0]=x;
  Pv[1][0]=y;
  Pv[2][0]=z;
  Matrix<T> Po=A*Pv;
  x=Po[0][0];
  y=Po[1][0];
  z=Po[2][0];
  return;
}

void
Vec3D::rotate(const Vec3D& Origin,const Vec3D& Axis,const double theta)
  /*!
    Executes an arbitory rotation about an Axis, Origin and 
    for an angle
    \param Origin :: Origin point to do rotation
    \param Axis :: Axis value to rotate around [needs to be unit]
    \param theta :: Angle in radians
  */
{
   x -= Origin.x;
   y -= Origin.y;
   z -= Origin.z;
   this->rotate(Axis,theta);
   x += Origin.x;
   y += Origin.y;
   z += Origin.z;
   return;
}

void
Vec3D::rotate(const Vec3D& Axis,const double theta)
  /*!
    Executes an arbitory rotation about an Axis and 
    for an angle
    \param Axis :: Axis value to rotate around [needs to be unit]
    \param theta :: Angle in radians
  */
{
   const double costheta = cos(theta);
   const double sintheta = sin(theta);
   Vec3D Q;       // output point
   Q.x += (costheta + (1 - costheta) * Axis.x * Axis.x) * x;
   Q.x += ((1 - costheta) * Axis.x * Axis.y - Axis.z * sintheta) * y;
   Q.x += ((1 - costheta) * Axis.x * Axis.z + Axis.y * sintheta) * z;

   Q.y += ((1 - costheta) * Axis.x * Axis.y + Axis.z * sintheta) * x;
   Q.y += (costheta + (1 - costheta) * Axis.y * Axis.y) * y;
   Q.y += ((1 - costheta) * Axis.y * Axis.z - Axis.x * sintheta) * z;

   Q.z += ((1 - costheta) * Axis.x * Axis.z - Axis.y * sintheta) * x;
   Q.z += ((1 - costheta) * Axis.y * Axis.z + Axis.x * sintheta) * y;
   Q.z += (costheta + (1 - costheta) * Axis.z * Axis.z) * z;

   x=Q.x;
   y=Q.y;
   z=Q.z;
   return;
}

Vec3D&
Vec3D::boundaryCube(const Vec3D& LP,const Vec3D& HP) 
  /*!
    Boundary limit point to +/- GP::BoxWidth
    \param LP :: Low corner value
    \param HP :: High corner value
    \return boundary limited point.
    \todo ADD SOME CHECKS!!!
  */
{
  for(int i=0;i<3;i++)
    {
      double& V=this->operator[](i);
      const double Range=HP[i]-LP[i];
      while (V>HP[i])
	V -= Range;
      while (V < LP[i])
	V += Range;
    }
  return *this;
}


int
Vec3D::reBase(const Vec3D& A,const Vec3D& B,const Vec3D& C) 
  /*! 
     Re-express this point components of A,B,C.
     Assuming that A,B,C are form an basis set (which
     does not have to be othonormal).
     \f[
     \left( a_0 b_0 c_0 \right) \left( \alpha \right) = \left( X \right)
     \f]
     \param A :: Unit vector in basis
     \param B :: Unit vector in basis
     \param C :: Unit vector in basis
     \retval -1 :: The points do not form a basis set.
     \retval 0  :: Vec3D has successfully been re-expressed.
  */
{
  Matrix<double> T(3,3);
  for(size_t i=0;i<3;i++)
    {
      T[i][0]=A[i];
      T[i][1]=B[i];
      T[i][2]=C[i];
    }
  const double det=T.Invert();
  if (fabs(det)<1e-13)       // failed
    return -1;
  rotate(T);
  return 0;
}


int
Vec3D::forwardBase(const Vec3D& A,const Vec3D& B,const Vec3D& C) 
  /*! 
     Re-express this point components of A,B,C.
     Assuming that A,B,C are form an basis set (which
     does not have to be othonormal.
     \f[
     \left( a_0 b_0 c_0 \right) \left( \alpha \right) = \left( X \right)
     \f]
     \param A :: Unit vector in basis
     \param B :: Unit vector in basis
     \param C :: Unit vector in basis
     \retval -1 :: The points do not form a basis set.
     \retval 0  :: Vec3D has successfully been re-expressed.
  */
{
  Matrix<double> T(3,3);
  for(size_t i=0;i<3;i++)
    {
      T[i][0]=A[i];
      T[i][1]=B[i];
      T[i][2]=C[i];
    }
  rotate(T);
  return 0;
}

int
Vec3D::coLinear(const Vec3D& Bv,const Vec3D& Cv) const
  /*!
    Determines if this,B,C are collinear (returns 1 if true)
    \param Bv :: Vector to test
    \param Cv :: Vector to test
    \returns 0 is no colinear and 1 if they are (within Ptolerance)
  */
{
  const Vec3D& Av=*this;
  const Vec3D Tmp((Bv-Av)*(Cv-Av));
  return (Tmp.abs()>zeroTol) ? 0 : 1;
}

Vec3D
Vec3D::crossNormal() const
  /*!
    Calculate the normal to the vector without reference to another
    \return Normal vector
   */
{
  Geometry::Vec3D N(y,z,x);
  
  return (*this * N).unit(); 
}

void
Vec3D::read(std::istream& IX)
  /*!
    Read data from a stream.
    \param IX :: Input stream
  */
{
  IX>>x;
  if (!IX.good())  // Failed on first read! -- check for Vec3D
    {
      std::string Name;
      IX.clear();
      std::getline(IX,Name,')');
      std::ios::off_type LNum=
	static_cast<std::ios::off_type>(Name.size());
      if (Name.substr(0,5)=="Vec3D" )
	{
	  Name+=")";
	  std::string::size_type pos(5);
	  while(Name[pos]!='(')
	    pos++;
	  Name.erase(0,pos+1);
	  std::istringstream cx(Name);
	  this->read(cx);
	  if (cx.good())
	    return;
	}
      IX.seekg(LNum,std::ios::cur);
      IX.setstate(std::ios::failbit);

      return;
    }
    
  if (IX.peek()==',')
    IX.get();
  IX>>y;
  if (IX.peek()==',')
    IX.get();
  IX>>z;

  return;
}

void
Vec3D::write(std::ostream& OX) const
  /*!
    Write out the point values
    \param OX :: Output stream
  */
{
  OX<<x<<" "<<y<<" "<<z;
  return;
}

/// \cond TEMPLATE

template Vec3D& 
Vec3D::operator*=(const Geometry::Matrix<double>&);
template Vec3D 
Vec3D::operator*(const Geometry::Matrix<double>&) const;

template double& Vec3D::operator[](const int);
template double Vec3D::operator[](const int) const;
template double& Vec3D::operator[](const size_t);
template double Vec3D::operator[](const size_t) const;

/// \endcond TEMPLATE

}  // NAMESPACE Geometry
