/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geometry/Vec2D.cxx
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
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <cmath>
#include <vector>
#include <string>
#include <algorithm>

#include "Exception.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Vec2D.h"


std::ostream& 
Geometry::operator<<(std::ostream& OX,const Geometry::Vec2D& A)
  /*!
    Calls Vec2D method write to output class
    \param OX :: Output stream
    \param A :: Vec2D to write
    \return Current state of stream
  */
{
  A.write(OX);
  return OX;
}

std::istream& 
Geometry::operator>>(std::istream& IX,Geometry::Vec2D& A)
  /*!
    Calls Vec2D method read to input class
    \param IX :: Input stream
    \param A :: Vec2D to read
    \return Current state of stream
  */
{
  A.read(IX);
  return IX;
}

namespace Geometry
{

Vec2D::Vec2D():
  x(0.0),y(0.0)
  /*!
    Origin Constructor
  */
{}

Vec2D::Vec2D(const double X,const double Y) :
  x(X),y(Y)
  /*!
    Constructor at Positon
    \param X :: x-coord
    \param Y :: y-coord
  */
{}

Vec2D::Vec2D(const double* xy) :
  x(xy[0]),y(xy[1])
  /*!
    Constructor from array
    \param xy :: 2 Vec2D array
  */
{}

Vec2D::Vec2D(const Matrix<double>& A)  :
  x(0.0),y(0.0)
  /*!
    Construct a Vec2D from a mateix
    The matrix needs to be 2x1 or 1x2
    \param A :: Matrix Item to be cast
  */
{
  std::pair<size_t,size_t> Asize=A.size();
  if (Asize.first>Asize.second) 
    {
      for(size_t i=0;i<2;i++)
	this->operator[](i)=(i<Asize.first) ? A[i][0] : 0.0;
    }
  else
    {
      for(size_t i=0;i<2;i++)
        this->operator[](i)=(i<Asize.second) ? A[i][0] : 0.0;
    }
}

Vec2D::Vec2D(const Vec2D& A) :
  x(A.x),y(A.y)
  /*!
    Copy constructor
    \param A :: Vec2D to copy
  */
{}

Vec2D&
Vec2D::operator=(const Vec2D& A)
  /*!
    Assignment operator
    \param A :: Vec2D to copy
    \return *this
  */
{
  if (this!=&A)
    {
      x=A.x;
      y=A.y;
    }
  return *this;
}

Vec2D::~Vec2D()
  /*!
    Standard Destructor
  */
{}

Vec2D&
Vec2D::operator()(const double a,const double b) 
  /*!
    Operator() casts a set of double to a Vec2D
    \param a :: x-cord
    \param b :: y-cord
    \return New point
  */
{
  x=a;
  y=b;
  return *this;
}


double&
Vec2D::operator[](const size_t A)
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
    }
  throw ColErr::IndexError<size_t>(A,1,"Vec2D::operator[]");
}

double
Vec2D::operator[](const size_t A) const
  /*!
    Operator [] isolates component based on 
    index A
    \param A :: Index number (0-1) 
    \return value of the coordinate
  */
{
  switch (A)
    {
    case 0:
      return x;
    case 1:
      return y;
    }
  throw ColErr::IndexError<size_t>(A,1,"Vec2D::operator[] const");
}

bool
Vec2D::operator==(const Vec2D& A) const
  /*!
    Equality operator within tolerance
    \param A :: Vec2D to compare
    \return A==this
  */
{
  return (&A==this || Distance(A)<=Geometry::zeroTol) ? 1 : 0;
}

bool
Vec2D::operator!=(const Vec2D& A) const
  /*!
    Not Equal operator within tolerance
    \param A :: Vec2D to compare
    \return A!=this
  */
{
  return (this->operator==(A)) ? 0 : 1;
}

bool
Vec2D::operator<(const Vec2D& A) const
  /*!
    Less than operator based on size
    \param A :: Vec2D to compare
    \return this<A
  */
{
  const double FV(this->abs()-A.abs());
  return (FV<Geometry::zeroTol) ? 0 : 1;
}

bool
Vec2D::operator>(const Vec2D& A) const
  /*!
    Less than operator based on size
    \param A :: Vec2D to compare
    \return this<A
  */
{
  const double FV(A.abs()-this->abs());
  return (FV<Geometry::zeroTol) ? 0 : 1;
}

Vec2D
Vec2D::operator*(const double V) const
  /*!
    Simple multiplication of this/Value 
    \param V :: scalar to multiply each component 
    \return Vec2D scaled by value
  */
{
  Vec2D X(*this);
  X*=V;
  return X;
}


Vec2D
Vec2D::operator/(const double V) const
  /*!
    Simple division of this/V 
    \param V :: Value to divide by.
    \returns Vec2D / Value
  */
{
  Vec2D X(*this);
  X/=V;
  return X;
}

Vec2D
Vec2D::operator+(const Vec2D& A) const
  /*! 
    Simple Vec2D addition
    \param A :: Vec2D to add
    \returns Vec2D + A
  */
{
  Vec2D X(*this);
  X+=A;
  return X;
}

Vec2D
Vec2D::operator-(const Vec2D& A) const
  /*!
    Simple Vec2D subtraction
    \param A : Vec2D to subtract
    \returns Vec2D - A
  */
{
  Vec2D X(*this);
  X-=A;
  return X;
}

Vec2D
Vec2D::operator-() const
  /*!
    Simple negation of the Vec2D
    \return -Vec2D
  */
{
  Vec2D X(*this);
  X *= -1.0;
  return X;
}

Vec2D&
Vec2D::operator*=(const double V)
  /*!
    Vec2D multication by a value
    \param V :: Multiplication value
    \return *this * V
  */
{
  x*=V;
  y*=V;
  return *this;
}

Vec2D&
Vec2D::operator/=(const double V)
  /*!
    Vec2D division by a value 
    (contains simple test for zero)
    \param V :: Value operator
    \return *this/V
  */
{
  if (std::abs(V)>1e-250)
    {
      x/=V;
      y/=V;
    }
  return *this;
}

Vec2D&
Vec2D::operator+=(const Vec2D& A)
  /*!
    Vec2D self addition 
    \param A :: Vec2D to add
    \return *this + A
  */
{
  x+=A.x;
  y+=A.y;
  return *this;
}

Vec2D&
Vec2D::operator-=(const Vec2D& A)
  /*!
    Vec2D self subtraction 
    \param A :: Vec2D to subtract
    \return (this - A)
  */
{
  x-=A.x;
  y-=A.y;
  return *this;
}

int 
Vec2D::masterDir(const double Tol) const
  /*! 
     Calculates the index of the primary direction (if there is one)
     \param Tol :: Tolerance accepted
     \retval range -2,-1 1,2 if the vector
     is orientaged within Tol on the x,y direction (the sign
     indicates the direction to the +ve side )
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
  other-=max;
  if ((other/max)>Tol)    //doesn't have master direction
    return 0;
  return idx;
}

size_t 
Vec2D::principleDir() const
  /*! 
     Calculates the index of the primary direction
     \retval range 0,1 :: direction of maximum 
  */
{
  return (std::abs(x)-std::abs(y)>0.0) ? 0 : 1;  
}

double
Vec2D::Distance(const Vec2D& A) const
  /*! 
    Determine the distance bwtween points
    \param A :: Vec2D 
    \return :: the distance between A and this 
  */
{
  return sqrt((A.x-x)*(A.x-x)+
	      (A.y-y)*(A.y-y));

}

double
Vec2D::makeUnit()
  /*!
    Make the vector a unit vector 
    \return :: the old magnitude 
  */
{
  const double Sz(abs());
  if (Sz>Geometry::zeroTol)
    {
      x/=Sz;
      y/=Sz;
    }
  return Sz;
}

Geometry::Vec2D
Vec2D::unit() const
  /*!
    Make the vector a unit vector 
    \return :: the vector as a unit form
  */
{
  Geometry::Vec2D Out(*this);
  Out.makeUnit();
  return Out;
}

Geometry::Vec2D
Vec2D::component(const Geometry::Vec2D& A) const
  /*!
    Make a component vector along the direction
    of A.
    \param A :: Vector to give principle direction 
    \return :: the vector as a unit form
  */
{
  const Geometry::Vec2D N=A.unit();    // Need to use A in unit form
  return N*N.dotProd(*this);
}

Geometry::Vec2D
Vec2D::cutComponent(const Geometry::Vec2D& A) const
  /*!
    Remove the component of A from the vector
    \param A :: Vector to give principle direction 
    \return :: the vector as a unit form
  */
{
  
  Geometry::Vec2D Out(*this);
  const Geometry::Vec2D N=A.unit();    // Need to use A in unit form
  Out -= N*Out.dotProd(N);
  return Out;
}

double
Vec2D::dotProd(const Vec2D& A) const
  /*!
    Calculate the dot product.
    \param A :: vector to take product from
    \returns this.A
  */
{
  return A.x*x+A.y*y;
}

int 
Vec2D::nullVector(const double Tol) const
  /*! 
    Checks the size of the vector
    \param Tol :: size of the biggest zero vector allowed.
    \retval 1 : the vector squared components
    magnitude are less than Tol 
    \retval 0 :: Vector bigger than Tol
  */
{
  return ((x*x+y*y)>Tol) ? 0 :1;
}


double
Vec2D::abs() const
  /*!
    Calculate the magnatude of the point
    \returns \f$ | this | \f$ 
  */
{
  return sqrt(x*x+y*y);
}

void
Vec2D::rotate(const Matrix<double>& A)
  /*!
    Rotate a point by a matrix 
    \param A :: Rotation matrix (needs to be 3x3)
  */
{
  Matrix<double> Pv(2,1);
  Pv[0][0]=x;
  Pv[1][0]=y;
  Matrix<double> Po=A*Pv;
  x=Po[0][0];
  y=Po[1][0];
  return;
}

void
Vec2D::rotate(const Vec2D& Origin,const double theta)
  /*!
    Executes an arbitory rotation about an Axis, Origin and 
    for an angle
    \param Origin :: Origin point to do rotation
    \param theta :: Angle in radians
  */
{
  x -= Origin.x;
  y -= Origin.y;
  this->rotate(theta);
  x += Origin.x;
  y += Origin.y;
  return;
}

void
Vec2D::rotate(const double theta)
  /*!
    Executes an arbitory rotation about the origin
    for an angle
    \param theta :: Angle in radians
  */
{
   const double costheta = cos(theta);
   const double sintheta = sin(theta);
   Vec2D Q(*this);       // output point
   x = Q.x*costheta - Q.y*sintheta;
   y = Q.x*sintheta + Q.y*costheta;
   return;
}

Vec2D&
Vec2D::boundaryCube(const Vec2D& LP,const Vec2D& HP) 
  /*!
    Boundary limit point to +/- GP::BoxWidth
    \param LP :: Low corner value
    \param HP :: High corner value
    \return boundary limited point.
    \todo ADD SOME CHECKS!!!
  */
{
  for(size_t i=0;i<2;i++)
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
Vec2D::reBase(const Vec2D& A,const Vec2D& B) 
  /*! 
     Re-express this point components of A,B
     Assuming that A,B,C are form an basis set (which
     does not have to be othonormal).
     \f[
     \left( a_0 b_0 c_0 \right) \left( \alpha \right) = \left( X \right)
     \f]
     \param A :: Unit vector in basis
     \param B :: Unit vector in basis
     \retval -1 :: The points do not form a basis set.
     \retval 0  :: Vec2D has successfully been re-expressed.
  */
{
  Matrix<double> T(2,2);
  for(size_t i=0;i<2;i++)
    {
      T[i][0]=A[i];
      T[i][1]=B[i];
    }
  const double det=T.Invert();
  if (fabs(det)<1e-13)       // failed
    return -1;
  rotate(T);
  return 0;
}



int
Vec2D::coLinear(const Vec2D& Bv,const Vec2D& Cv) const
  /*!
    Determines if this,B,C are collinear (returns 1 if true)
    \param Bv :: Vector to test
    \param Cv :: Vector to test
    \returns 0 is no colinear and 1 if they are (within zeroTol)
  */
{
  const Vec2D Bx = Bv - *this;
  const Vec2D Cx = Cv - *this;
  
  
  const double angle=atan2(Cx.y,Cx.x)-atan2(Bx.y,Bx.x);
  return (std::abs(angle)>Geometry::zeroTol) ? 0 : 1;
}

  
Vec2D
Vec2D::crossNormal() const
  /*!
    Calculate the normal to the vector without reference to another
    \return normal vector rotated by 90 degrees (clockwize to +ve Z)
   */
{
  Geometry::Vec2D N;
  N.x= -y;
  N.y= x;
  return N.unit();
}

void
Vec2D::read(std::istream& IX)
  /*!
    Read data from a stream.
    \param IX :: Input stream
  */
{
  IX>>x;
  if (!IX.good())  // Failed on first read! -- check for Vec2D
    {
      std::string Name;
      IX.clear();
      std::getline(IX,Name,')');
      std::ios::off_type LNum=
	static_cast<std::ios::off_type>(Name.size());
      if (Name.substr(0,5)=="Vec2D" )
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

  return;
}

void
Vec2D::write(std::ostream& OX) const
  /*!
    Write out the point values
    \param OX :: Output stream
  */
{
  OX<<x<<" "<<y<<" ";
  return;
}

}  // NAMESPACE Geometry
