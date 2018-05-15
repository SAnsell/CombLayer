/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   support/doubleErr.cxx
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
#include <cmath>
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>

#include "support.h"
#include "doubleErr.h"

/// GLOBAL FUNCTIONS 

namespace DError
{

std::ostream& 
operator<<(std::ostream& OX,const doubleErr& A)
  /*!
    Write out to a stream
    \param OX :: ostream to write 
    \param A :: Object to write
    \return Current state of stream
   */
{
  A.write(OX);
  return OX;
}

std::istream& 
operator>>(std::istream& IX,doubleErr& A)
  /*!
    Write out to a stream
    \param IX :: Input stream
    \param A :: Object to read
    \return Current state of stream
   */
{
  A.read(IX);
  return IX;
}

doubleErr
pow(const doubleErr& A,const doubleErr& B)
  /*!
    Calculates the Power of two numbers
    \param A :: Number to be raised to the power
    \param B :: Raising factor
    \returns pow(A,B)
   */
{
  doubleErr Out=A;
  return Out.pow(B);
}

doubleErr
sin(const doubleErr& A)
  /*!
    Calculates sin(A)
    \param A :: Number to use
    \returns sin(A)
   */
{
  doubleErr Out=A;
  return Out.sin();
}

doubleErr
cos(const doubleErr& A)
  /*!
    Calculates cos(A)
    \param A :: Number to use
    \returns cos(A)
   */
{
 doubleErr Out=A;
  return Out.cos();
}

doubleErr
tan(const doubleErr& A)
  /*!
    Calculates tan(A)
    \param A :: Number to use
    \returns tan(A)
   */
{
  doubleErr Out=A;
  return Out.tan();
}

doubleErr
asin(const doubleErr& A)
  /*!
    Calculates asin(A)
    \param A :: Number to use
    \returns asin(A)
   */
{
  doubleErr Out=A;
  return Out.asin();
}

doubleErr
acos(const doubleErr& A)
  /*!
    Calculates acos(A)
    \param A :: Number to use
    \returns acos(A)
   */
{
  doubleErr Out=A;
  return Out.acos();
}

doubleErr
atan(const doubleErr& A)
  /*!
    Calculates atan(A)
    \param A :: Number to use
    \returns atan(A)
   */
{
  doubleErr Out=A;
  return Out.atan();
}

doubleErr
exp(const doubleErr& A)
  /*!
    Calculates exp(A)
    \param A :: Number to use
    \returns exp(A)
   */
{
  doubleErr Out=A;
  return Out.exp();
}

doubleErr
log(const doubleErr& A)
  /*!
    Calculates log(A)
    \param A :: Number to use
    \returns log(A)
   */
{
  doubleErr Out=A;
  return Out.log();
}

/// CLASS OBJECTS

doubleErr::doubleErr() :
  Val(0.0),Err(0.0)
  /// Zero constructor
{}

doubleErr::doubleErr(const double V) :
  Val(V),Err(0.0)
  /*!
    Error zero constructor
    \param V :: Value
  */
{}

doubleErr::doubleErr(const double V,const double E) :
  Val(V),Err(E*E)
  /*!
    Error zero constructor
    \param V :: Value
    \param E :: Error value (sqrt form)
  */
{}

doubleErr::doubleErr(const doubleErr& A) :
  Val(A.Val),Err(A.Err)
  /*!
    Copy Constructor
    \param A :: doubleError
  */
{}

doubleErr&
doubleErr::operator=(const doubleErr& A)
  /*!
    Standard assignment operator 
    \param A :: doubleError
    \return *this
  */
{
  if (this!=&A)
    {
      Val=A.Val;
      Err=A.Err;
    }
  return *this;
}

doubleErr::~doubleErr()
  /// Destructor
{}

doubleErr&
doubleErr::operator+=(const doubleErr& A)
  /*!
    Addition:
    \param A :: doubleValue to add
    \return this+A
  */    
{
  Val+=A.Val;
  Err+=A.Err;
  return *this;
}

doubleErr&
doubleErr::operator-=(const doubleErr& A)
  /*!
    Subtraction:
    \param A :: Item to subtract
    \return this-A
  */    
{
  Val-=A.Val;
  Err+=A.Err;
  return *this;
}

doubleErr&
doubleErr::operator*=(const doubleErr& A)
  /*!
    Multiplication:
    \param  A :: item to multiply by
    \return this*A
  */    
{
  Err=Val*Val*A.Err+A.Val*A.Val*Err;
  Val*=A.Val;
  return *this;
}

doubleErr&
doubleErr::operator/=(const doubleErr& A)
  /*!
    Division:
    carries out 
    \f[ \frac{Err^2}{V^2} = \frac{Err_a^2}{a^2} + \frac{Err_b^2}{b^2} \f]
    \param A :: item to divide by
    \return this/=A;
  */    
{
  Err=Err+A.Err*Val*Val/(A.Val*A.Val);
  Val/=A.Val;
  Err/=(A.Val*A.Val);
  return *this;
}

// DOUBLE ADDITION

doubleErr&
doubleErr::operator+=(const double& D)
  /*!
    Addition:
    \param D :: double value to add
    \return this+A
  */    
{
  Val+=D;
  return *this;
}

doubleErr&
doubleErr::operator-=(const double& D)
  /*!
    Subtraction:
    \param D :: Item to subtract
    \return this-D
  */    
{
  Val-=D;
  return *this;
}

doubleErr&
doubleErr::operator*=(const double& D)
  /*!
    Multiplication:
    \param  D :: Item to multiply by
    \return this*D
  */    
{
  Val*=D;
  Err*=D*D;
  return *this;
}

doubleErr&
doubleErr::operator/=(const double& D)
  /*!
    Division:
    carries out 
    \f[ \frac{Err^2}{V^2} = \frac{Err_a^2}{a^2} + \frac{Err_b^2}{b^2} \f]
    \param D :: item to divide by
    \return this/=D;
  */    
{
  Val/=D;
  Err/=D*D;
  return *this;
}

doubleErr 
doubleErr::operator+(const doubleErr& A) const
  /*!
    Addition: Uses += operator
    \param A item to add
    \return this+A
  */    
{
  doubleErr Out(*this);
  Out+=A;
  return Out;
}

doubleErr 
doubleErr::operator-(const doubleErr& A) const
  /*!
    Subtraction: Uses -= operator
    \param A :: item to subtract
    \return this-A
  */    
{
  doubleErr Out(*this);
  Out-=A;
  return Out;
}

doubleErr 
doubleErr::operator*(const doubleErr& A) const
  /*!
    Multiplication: Uses *= operator
    \param A :: item to multiply by
    \return this*A
  */    
{
  doubleErr Out(*this);
  Out*=A;
  return Out;
}

doubleErr 
doubleErr::operator/(const doubleErr& A) const
  /*!
    Division: Uses /= operator
    \param A :: item to divde by
    \return this/A
  */    
{
  doubleErr Out(*this);
  Out/=A;
  return Out;
}

doubleErr 
doubleErr::operator+(const double& A) const
  /*!
    Addition: Uses += operator
    \param A item to add
    \return this+A
  */    
{
  doubleErr Out(*this);
  Out+=A;
  return Out;
}

doubleErr 
doubleErr::operator-(const double& A) const
  /*!
    Subtraction: Uses -= operator
    \param A :: item to subtract
    \return this-A
  */    
{
  doubleErr Out(*this);
  Out-=A;
  return Out;
}

doubleErr 
doubleErr::operator*(const double& A) const
  /*!
    Multiplication: Uses *= operator
    \param A :: item to multiply by
    \return this*A
  */    
{
  doubleErr Out(*this);
  Out*=A;
  return Out;
}

doubleErr 
doubleErr::operator/(const double& A) const
  /*!
    Division: Uses /= operator
    \param A :: item to divide by
    \return this/A
  */    
{
  doubleErr Out(*this);
  Out/=A;
  return Out;
}


doubleErr&
doubleErr::pow(const doubleErr& B) 
  /*!
    Calculates ans=this^n
    \f[ e2= a^{2b} \log^2 x \sigma_b + b^2 a^{2b-2} \sigma_a \f]
    Note the cluggy mechanism for dealing with -ve A
    is if (int(B)/2) 
    \param B :: raising power
    \return pow(this,B)
   */
{
  const double a=fabs(Val);
  const double V=::pow(a,B.Val);
  const double siga=B.Val*B.Val* ::pow(a,2*B.Val-2)*Err;
  Err=siga+V*V* ::log(a)* ::log(a)*B.Err;
  if (Val>0.0 || !(static_cast<int>(B.Val)/2))
    Val=V;
  else
    Val= -V;
  return *this;
}

doubleErr&
doubleErr::sin() 
  /*!
    Calculates sin(this)
    \f[ Err^2 = \cos^2(V) err^2 \f]
    \return *this
  */
{
  Val= ::sin(Val);
  Err*=1.0-Val*Val;
  return *this;
}


doubleErr&
doubleErr::cos() 
  /*!
    Calculates cos(this)
    \f[ Err^2 = \sin^2(V) err^2 \f]
    \return *this
  */
{
  Val= ::cos(Val);
  Err*=1.0-Val*Val;
  return *this;
}

doubleErr&
doubleErr::tan() 
  /*!
    Calculates tan(this)
    \f[ Err^2 = \sec^4(V) err^2 \f]
    \return *this
  */
{
  const double sec2=1.0/ ::cos(Val);
  Val= ::tan(Val);
  Err*=sec2*sec2;
  return *this;
}


doubleErr&
doubleErr::asin() 
  /*!
    Calculates asin(this)
    \f[ Err^2 = \frac{1}{1-x^2} err^2 \f]
    \return *this
  */
{
  Err/=1.0-Val*Val;
  Val= ::asin(Val);
  return *this;
}

doubleErr&
doubleErr::acos() 
  /*!
    Calculates acos(this)
    \f[ Err^2 = \frac{1}{1-x^2} err^2 \f]
    \return *this
  */
{
  Err/=1.0-Val*Val;
  Val= ::acos(Val);
  return *this;
}

doubleErr&
doubleErr::atan() 
  /*!
    Calculates acos(this)
    \f[ Err^2 = \frac{1}{1-x^2} err^2 \f]
    \return *this
  */
{
  Err/=::pow((1.0-Val*Val),2.0);
  Val= ::atan(Val);
  return *this;
}

doubleErr&
doubleErr::exp() 
  /*!
    Calculates exp(this)
    \f[ Err^2 = \exp(2x) err^2 \f]
    \return *this
  */
{
  Err*=::exp(2.0*Val);
  Val= ::exp(Val);
  return *this;
}


doubleErr&
doubleErr::log() 
  /*!
    Calculates log(this)
    \f[ Err^2 = \frac{1}{x^2} err^2 \f]
    \return *this
  */
{
  Err/=(Val*Val);
  Val= ::log(Val);
  return *this;
}


void
doubleErr::read(std::istream& IX)
  /*!
    Input an object from the stream
    Input options are :
    - double double
    - double (double)
    \param IX :: Input stream
  */
{
  double VA,VB;
  std::string ErrCmp;
  IX>>VA;
  IX>>ErrCmp;
  if (!IX.fail() && !ErrCmp.empty())
    {
      if (ErrCmp[0]=='(' && ErrCmp[ErrCmp.length()-1]==')')
        {
	  if (StrFunc::convert(ErrCmp.substr(1,ErrCmp.length()-2),VB))
	    {
	      Val=VA;
	      Err=VB*VB;
	    }
	}
      else if (StrFunc::convert(ErrCmp,VB))
        {
	  Val=VA;
	  Err=VB*VB;
	}
    }
  return;
}

void
doubleErr::write(std::ostream& OX) const
  /*!
    Write out data to a stream
    \param OX :: ostream to output
  */
{
  OX<<Val<<" ("<<sqrt(Err)<<")";
  return;
}

}   // NAMESPACE DError
