/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   poly/PolyVarOne.cxx
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
#include <cmath>
#include <complex>
#include <vector>
#include <map>
#include <algorithm>
#include <iterator>
#include <functional>
#include <boost/format.hpp>

#ifndef NO_GSL
#include <gsl/gsl_poly.h> 
#include <gsl/gsl_errno.h> 
#endif 

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "support.h"
#include "mathSupport.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "masterWrite.h"
#include "polySupport.h"
#include "PolyFunction.h"
#include "PolyVar.h"

namespace mathLevel
{

int gslflag(0);

void
polyHandler(const char*,const char*,int,int)
  /*!
    Handler for GSL errors
  */
{
  gslflag=1;
  return;
}


std::ostream& 
operator<<(std::ostream& OX,const PolyVar<1>& A)
  /*!
    External Friend :: outputs point to a stream 
    \param OX :: output stream
    \param A :: PolyFunction to write
    \returns The output stream (OX)
  */
{
  if (!A.write(OX))
    OX<<"0";
  return OX;
}

PolyVar<1>::PolyVar(const size_t iD) : 
  PolyFunction(),iDegree(iD),
  PCoeff(iDegree+1)
  /*!
    Constructor 
    \param iD :: degree
  */
{
  fill(PCoeff.begin(),PCoeff.end(),0.0);
}

PolyVar<1>::PolyVar(const size_t iD,const double E) : 
  PolyFunction(E),iDegree(iD),
  PCoeff(iDegree+1)
  /*!
    Constructor 
    \param iD :: degree
    \param E :: Accuracy
  */
{
  fill(PCoeff.begin(),PCoeff.end(),0.0);
}

PolyVar<1>::PolyVar(const std::vector<double>& Val) : 
  PolyFunction(),
  iDegree(Val.empty() ? 0 : Val.size()-1),
  PCoeff(iDegree+1)
  /*!
    Constructor 
    \param Val :: values low->high
  */
{
  if (Val.empty())
    PCoeff.front()=0.0;
  else
    PCoeff=Val;
}

PolyVar<1>::PolyVar(const size_t,const double B,const double A) : 
  PolyFunction(),
  iDegree(1),PCoeff(iDegree+1)
  /*!
    Constructor 
    \param B :: Value ( variable )
    \param A :: Value (constant)
  */
{  
  PCoeff[0]=A;
  PCoeff[1]=B;
}

PolyVar<1>::PolyVar(const size_t,const double C,
		    const double B,const double A) : 
  PolyFunction(),
  iDegree(2),PCoeff(iDegree+1)
  /*!
    Constructor 
    \param C :: Value ( variable^2 )
    \param B :: Value ( variable )
    \param A :: Value (constant)
  */
{
  PCoeff[0]=A;
  PCoeff[1]=B;
  PCoeff[2]=C;
}

PolyVar<1>::PolyVar(const PolyVar<1>& A)  :
  PolyFunction(A),iDegree(A.iDegree),PCoeff(A.PCoeff)
  /*! 
    Copy Constructor
    \param A :: PolyBase to copy
   */
{}

PolyVar<1>& 
PolyVar<1>::operator=(const PolyVar<1>& A)
  /*! 
    Assignment operator
    \param A :: PolyBase to copy
    \return *this
   */
{
  if (this!=&A)
    {
      PolyFunction::operator=(A);
      iDegree = A.iDegree;
      PCoeff=A.PCoeff;
    }
  return *this;
}

PolyVar<1>& 
PolyVar<1>::operator=(const double& V)
  /*! 
    Assignment operator
    \param V :: Double value to set
    \return *this
   */
{
  iDegree = 0;
  PCoeff.resize(1);
  PCoeff[0]=V;
  return *this;
}

PolyVar<1>::~PolyVar()
  /// Destructor
{}

void 
PolyVar<1>::setDegree(const size_t iD)
  /*!
    Set the degree value
    \param iD :: degree
   */
{
  iDegree = iD;
  PCoeff.resize(iDegree+1);
  return;
}

void
PolyVar<1>::zeroPoly()
  /*!
    Zeros each of the coefficients 
  */
{
  fill(PCoeff.begin(),PCoeff.end(),0.0);
  return;
}


size_t
PolyVar<1>::getDegree() const 
  /*!
    Accessor to degree size
    \return size 
  */
{
  return iDegree;
}

double
PolyVar<1>::subVariable(const size_t Index,const double& V) const
  /*!
    Substitute for a given index
    \param Index :: Variable index value, [therefore MUST be zero,
    as only variable is X]
    \param V :: Value to set
    \return polynominal evaluation
  */
{
  ELog::RegMethod RegA("PolyVar<1>","subVariable");
  if (Index)
    throw ColErr::IndexError<size_t>(Index,0,RegA.getBase());
  return operator()(V);
}

void
PolyVar<1>::setComp(const size_t Index,const double& V)
  /*!
    Set a component
    \param Index :: coefficient index
    \param V :: Value
    \throw IndeError on Index range check
   */
{
  if (Index>iDegree)
    throw ColErr::IndexError<size_t>(Index,iDegree+1,
				     "PolyVar<1>::setComp(double)");
  PCoeff[Index] = V;
  return;  
}

PolyVar<1>::operator const std::vector<double>& () const
  /*!
    Accessor to a vector component
    \return vector
  */
{
  return PCoeff;
}

PolyVar<1>::operator std::vector<double>& ()
  /*!
    Accessor to a vector component
    \return vector
  */
{
  return PCoeff;
}


double
PolyVar<1>::operator[](const size_t i) const
  /*!
    Accessor to component
    \param i :: index 
    \return Coeficient c_i
   */
{
  if (i>iDegree)
    throw ColErr::IndexError<size_t>(i,iDegree+1,"PolyBase::operator[] const");
  return PCoeff[i];
}

double& 
PolyVar<1>::operator[](const size_t i)
  /*!
    Accessor to component
    \param i :: index 
    \return Coeficient c_i
   */
{
  if (i>iDegree)
    throw ColErr::IndexError<size_t>(i,iDegree+1,"PolyBase::operator[]");
  return PCoeff[i];
}

double 
PolyVar<1>::operator()(const double X) const
  /*!
    Calculate polynomial at x
    \param X :: Value to calculate poly
    \return polyvalue(x)
  */
{
  double Result = PCoeff[iDegree];              
  for (size_t i=iDegree; i > 0;)
    {
      i--;
      Result *= X;
      Result += PCoeff[i];
    }
  return Result;
}

std::complex<double>
PolyVar<1>::evalPoly(const std::complex<double>& X) const
  /*!
    Calculate polynomial at x [complex]
    \param X :: Value to calculate poly [complex]
    \return polyvalue(x)
  */
{
  std::complex<double> Result = PCoeff[iDegree];              
  for (size_t i=iDegree; i > 0;)
    {
      i--;
      Result *= X;
      Result += PCoeff[i];
    }
  return Result;
}

double 
PolyVar<1>::operator()(const double* DArray) const
  /*!
    Calculate the value of the polynomial at a point
    \param DArray :: Values [x,y,z]
    \return Polynomial value
  */
{
  double X(1.0);
  double sum(0.0);
  const double xVal(DArray[0]);
  for(size_t i=0;i<=iDegree;i++)
    {
      sum+=PCoeff[i]*X;
      X*=xVal;
    }
  return sum;
}

double 
PolyVar<1>::operator()(const std::vector<double>& DArray) const
  /*!
    Calculate the value of the polynomial at a point
    \param DArray :: Values [x,y,z]
    \return results
  */
{
  if (DArray.empty())
    throw ColErr::IndexError<int>(0,1,"PolVar<1>::operator()");
  double X(1.0);
  double sum(0.0);
  const double xVal(DArray[0]);
  for(size_t i=0;i<=iDegree;i++)
    {
      sum+=PCoeff[i]*X;
      X*=xVal;
    }
  return sum;
}


bool
PolyVar<1>::operator<(const PolyVar<1>& A) const
  /*!
    PolyVar comparison based on number of coefficients 
    then degree (simpler)
    \param A :: PolyVar to compare
    \return this < A
   */
{
  return iDegree<A.iDegree;
}


PolyVar<1>& 
PolyVar<1>::operator+=(const PolyVar<1>& A)
  /*!
    Self addition value
    \param A :: PolyBase to add 
    \return *this+=A;
   */
{
  iDegree=(iDegree>A.iDegree) ? iDegree : A.iDegree;
  PCoeff.resize(iDegree+1);
  for(size_t i=0;i<=A.iDegree;i++)
    PCoeff[i]+=A.PCoeff[i];
  return *this;
}

PolyVar<1>& 
PolyVar<1>::operator-=(const PolyVar<1>& A)
  /*!
    Self addition value
    \param A :: PolyBase to add 
    \return *this-=A;
   */
{
  iDegree=(iDegree>A.iDegree) ? iDegree : A.iDegree;
  PCoeff.resize(iDegree+1);
  // Note iDegree>=A.iDegree
  for(size_t i=0;i<=A.iDegree;i++)
    PCoeff[i]-=A.PCoeff[i];
  return *this;
}

PolyVar<1>& 
PolyVar<1>::operator*=(const PolyVar<1>& A)
  /*!
    Self multiplication value
    \param A :: PolyBase to multiply
    \return *this*=A;
  */
{
  const size_t iD=iDegree+A.iDegree;
  std::vector<double> CX(iD+1,0.0);   // all set to zero
  for(size_t i=0;i<=iDegree;i++)
    for(size_t j=0;j<=A.iDegree;j++)
      CX[i+j]+=PCoeff[i]*A.PCoeff[j];

  PCoeff=CX;
  iDegree=iD;
  return *this;
}

PolyVar<1> 
PolyVar<1>::operator+(const PolyVar<1>& A) const
  /*!
    PolyBase addition
    \param A :: PolyBase addition
    \return (*this+A);
   */
{
  PolyVar<1> kSum(*this);
  return kSum+=A;
}

PolyVar<1>
PolyVar<1>::operator-(const PolyVar<1>& A) const
  /*!
    PolyBase subtraction
    \param A :: PolyBase to subtract
    \return (*this-A);
   */
{
  PolyVar<1> kSum(*this);
  return kSum-=A;
}

PolyVar<1> 
PolyVar<1>::operator*(const PolyVar<1>& A) const
  /*!
    PolyBase multiplication
    \param A :: PolyBase multiplication
    \return (*this*A);
   */
{
  PolyVar<1> kSum(*this);
  return kSum*=A;
}

PolyVar<1> 
PolyVar<1>::operator+(const double V) const
  /*!
    PolyBase addition by a value
    \param V :: Value to add
    \return (*this+V);
   */
{
  PolyVar<1> kSum(*this);
  return kSum+=V;
}

PolyVar<1> 
PolyVar<1>::operator-(const double V) const
  /*!
    PolyVar<1> substraction
    \param V :: Value to subtract
    \return (*this-V);
   */
{
  PolyVar<1> kSum(*this);
  return kSum-=V;
}

PolyVar<1> 
PolyVar<1>::operator*(const double V) const
  /*!
    PolyBase multiplication
    \param V :: Single value to scale polyVar by.
    \return (*this*V);
   */
{
  PolyVar<1> kSum(*this);
  return kSum*=V;
}

PolyVar<1> 
PolyVar<1>::operator/(const double V) const
  /*!
    PolyBase division
    \param V :: Single value to divide into polynomial
    \return (*this/V);
   */
{
  PolyVar<1> kSum(*this);
  return kSum/=V;
}

// ---------------- SCALARS --------------------------

PolyVar<1>&
PolyVar<1>::operator+=(const double V) 
  /*!
    PolyBase addition
    \param V :: Value to add
    \return (*this+V);
   */
{
  PCoeff[0]+=V;  // There is always zero component
  return *this;
}

PolyVar<1>&
PolyVar<1>::operator-=(const double V) 
  /*!
    PolyBase subtraction
    \param V :: Value to subtract
    \return (*this-V);
   */
{
  PCoeff[0]-=V;  // There is always zero component
  return *this;
}

PolyVar<1>&
PolyVar<1>::operator*=(const double V) 
  /*!
    PolyBase multiplication
    \param V :: Value to multipication
    \return (*this*V);
   */
{
  transform(PCoeff.begin(),PCoeff.end(),PCoeff.begin(),
	    std::bind2nd(std::multiplies<double>(),V));
  return *this;
}

PolyVar<1>&
PolyVar<1>::operator/=(const double V) 
  /*!
    PolyBase division scalar
    \param V :: Value to divide
    \return (*this/V);
   */
{
  transform(PCoeff.begin(),PCoeff.end(),PCoeff.begin(),
	    std::bind2nd(std::divides<double>(),V));
  return *this;
}

PolyVar<1> 
PolyVar<1>::operator-() const
  /*!
    PolyBase negation operator
    \return -(*this);
   */
{
  PolyVar<1> KOut(*this);
  KOut*= -1.0;
  return KOut;
}

int
PolyVar<1>::operator==(const PolyVar<1>& A) const
  /*!
    Determine if two polynomials are equal
    \param A :: Other polynomial to use
    \return 1 :: true 0 on false
  */
{
  size_t i;
  for(i=0;i<=A.iDegree && i<=iDegree;i++)
    if (std::abs(PCoeff[i]-A.PCoeff[i])>this->Eaccuracy)
      return 0;
  if (A.iDegree>iDegree)
    {
      for(;i<=A.iDegree;i++)
	if (std::abs(A.PCoeff[i])>this->Eaccuracy)
	  return 0;
    }
  else if (A.iDegree<iDegree)
    {
      for(;i<=iDegree;i++)
	if (std::abs(PCoeff[i])>this->Eaccuracy)
	  return 0;
    }
  return 1;
}
 
int
PolyVar<1>::operator!=(const PolyVar<1>& A) const
  /*!
    Determine if two polynomials are different
    \param A :: Other polynomial to use
    \return 1 is polynomial differ:  0 on false
  */
{
  return (this->operator==(A)) ? 0 : 1;
}

int
PolyVar<1>::operator==(const double& V) const
  /*!
    Determine if a polynomial and a value are equal
    \param V :: Single value to use
    \return 1 equal:  0 on false
  */
{
  if (std::abs(PCoeff[0]-V)>this->Eaccuracy)
    return 0;
  for(size_t i=1;i<=iDegree;i++)
    if (std::abs(PCoeff[i])>this->Eaccuracy)
      return 0;
  
  return 1;
}

int
PolyVar<1>::operator!=(const double& V) const
  /*!
    Determine if a value and polynomial are different
    \param V :: Single value to use
    \return 1 is polynomial differs:  0 on false
  */
{
  return (this->operator==(V)) ? 0 : 1;
}


PolyVar<1> 
PolyVar<1>::getDerivative() const
  /*!
    Take derivative
    \return dP / dx
  */
{
  PolyVar<1> KOut(*this);
  return KOut.derivative();
}

PolyVar<1>&
PolyVar<1>::derivative()
  /*!
    Take derivative of this polynomial
    \return dP / dx
  */
{
  if (iDegree<1)
    {
      PCoeff[0]=0.0;
      return *this;
    }

  for(size_t i=0;i<iDegree;i++)
    PCoeff[i]=PCoeff[i+1]*static_cast<double>(i+1);
  iDegree--;

  return *this;
}

PolyVar<1> 
PolyVar<1>::GetInversion() const
  /*!
    Inversion of the coefficients
    \return (Poly)^{-1}
   */
{
  PolyVar<1> InvPoly(iDegree);
  for (size_t i=0; i<=iDegree; i++)
    InvPoly.PCoeff[i] = PCoeff[iDegree-i];
  return InvPoly;
}

int
PolyVar<1>::expand(const PolyVar<1>& A)
  /*!
    Expand the polynominal to have at least one 
    term in X
    \param A :: Extra composite
    \return 0 on success and -ve on error
  */
{
  ELog::RegMethod RegA("PolyVar","expand",1);
  // special case : function contains all the variables
  if (this->isComplete())
    return 0;
  // Special case : The other function is complete
  if (A.isComplete())
    {
      this->operator=(A);
      return 0;
    }
  // No fancy compositions possible here
  return -1; 
}

void
PolyVar<1>::normalize()
  /*!
    Normalize the number 
  */
{
  const double Leading = PCoeff[iDegree];
  PCoeff[iDegree] = 1.0;   // avoid rounding issues
  for (size_t i=0;i<iDegree;i++)
    PCoeff[i] /= Leading;
}

void
PolyVar<1>::unitPrimary(const double epsilon)
  /*!
    Reduce the equation to have only a unit primary
    \param epsilon :: coeficient to use to decide if a values is zero
  */
{
  this->compress(epsilon);
  for(size_t i=0;i<iDegree;i++)
    PCoeff[i]/=PCoeff[iDegree];
  PCoeff[iDegree]=1.0;
  return;
}

void 
PolyVar<1>::compress(const double epsilon)
  /*!
    Process to remove coefficients of zero or near zero: 
    Reduce degree by eliminating all (nearly) zero leading coefficients
    \param epsilon :: coeficient to use to decide if a values is zero
  */
{
  const double eps((epsilon>0.0) ? epsilon : this->Eaccuracy);
  for (;iDegree>0 && std::abs(PCoeff[iDegree])<=eps;iDegree--) ;
  PCoeff.resize(iDegree+1);
  
  return;
}

void 
PolyVar<1>::divide(const PolyVar<1>& pD,PolyVar<1>& pQ, 
		 PolyVar<1>& pR,const double epsilon) const
  /*!
    Carry out polynomial division of this / pD  (Euclidean algorithm)
    If 'this' is P(t) and the divisor is D(t) with degree(P) >= degree(D),
    then P(t) = Q(t)*D(t)+R(t) where Q(t) is the quotient with
    degree(Q) = degree(P) - degree(D) and R(t) is the remainder with
    degree(R) < degree(D).  If this routine is called with
    degree(P) < degree(D), then Q = 0 and R = P are returned.  The value
    of epsilon is used as a threshold on the coefficients of the remainder
    polynomial.  If smaller, the coefficient is assumed to be zero.

    \param pD :: Polynominal to divide
    \param pQ :: Quotant
    \param pR :: Remainder
    \param epsilon :: Tolerance  [-ve to use master tolerance]
  */ 
{
  
  if (iDegree>=pD.iDegree)
    {
      const size_t iQuotDegree = iDegree - pD.iDegree;  
      pQ.setDegree(iQuotDegree);
      // temporary storage for the remainder
      pR = *this;
      
      // do the division 
      const double fInv = 1.0/pD[pD.iDegree];
      for (size_t iQ=iQuotDegree+1;iQ>0;)
        {
	  iQ--;
	  size_t iR = pD.iDegree + iQ;
	  pQ[iQ] = fInv*pR[iR];
	  for (iR--;iR>=iQ;iR--)
	    pR.PCoeff[iR] -= pQ[iQ]* pD[iR-iQ];
	}
      
      // calculate the correct degree for the remainder
      pR.compress(epsilon);
      return;
    } 

  pQ.setDegree(0);
  pQ[0] = 0.0;
  pR = *this;
  return;
}

std::vector<double> 
PolyVar<1>::realRoots(const double epsilon)
  /*!
    Get just the real roots
    \param epsilon :: tolerance factor (-ve to use default)
    \return vector of the real roots (if any)
  */
{
  ELog::RegMethod RegA("PolyVar<1>","realRoots");

  const double eps((epsilon>0.0) ? epsilon : Eaccuracy);
  std::vector<std::complex<double> > Croots=calcDurandKernerRoots(epsilon);
  std::vector<double> Out;
  std::vector<std::complex<double> >::const_iterator vc;
  for(vc=Croots.begin();vc!=Croots.end();vc++)
    {
      if (std::abs(vc->imag())<eps)
	Out.push_back(vc->real());
    }
  sort(Out.begin(),Out.end());
  Out.erase(
	    std::unique(Out.begin(),Out.end(),mathSupport::tolEqual(eps)),
	    Out.end());
  return Out;
}

int
PolyVar<1>::checkSmallPoly(std::vector<std::complex<double> >& Out) const
  /*!
    Check to see if the roots are from a small polynomial [factor 3 or less]
    and solve. If not return 0 and not solution
    \param Out :: Output of results if found
    \return  1 on success / 0 on fail
 */
{
  ELog::RegMethod RegA("PolyVar<1>","checkSmallPoly");

  // Zero State:
  if (iDegree<=0)
    return 1;

  Out.resize(iDegree);
  size_t N(1);
  switch (iDegree)
    {
    case 1:   // x+a_0 =0 
      Out[0]=std::complex<double>(-PCoeff[0]);
      break;
    case 2:  // x^2+a_1 x 
      N= ::solveQuadratic(PCoeff[2],PCoeff[1],PCoeff[0],
			  Out[0],Out[1]);
      break;
    case 3:  // x^3+a_2 x^2+ a_1 x+c=0
      N= ::solveCubic(PCoeff[3],PCoeff[2],PCoeff[1],PCoeff[0],
		      Out[0],Out[1],Out[2]);
      break;
    default:
      return 0;
    }
  Out.resize(N);
  return 1;
}
  
std::vector<std::complex<double> >
PolyVar<1>::calcDurandKernerRoots(const double epsilon)
  /*!
    Calculate all the roots of the polynominal.
    Uses the Durand-Kerner method. An approximation 
    method based on the stability of 
    \f[ f(x)=(x-P)(x-Q)(x-R) \dots \f]
    to each root iteration 
    \f[ x_1 := x_0 - \frac{f(x_0)}{(x_0-Q)(x_0-R)(...)} \f]
    \param epsilon :: tolerance factor (-ve to use default)
    \return roots (not sorted/uniqued)
  */
{
  ELog::RegMethod RegA("PolyVar<1>","calcDurandKernerRoots");
  unitPrimary(epsilon);
  const double eps((epsilon>0.0) ? epsilon : this->Eaccuracy);
  
  std::vector<std::complex<double> > cn;
  if (checkSmallPoly(cn))
    return cn;
  
  // cn has been correctly sized:
  // initialize cn coeffients [P,Q,R]
  cn[0]=std::complex<double>(0.4,0.9);
  for(size_t i=1;i<iDegree;i++)
    cn[i]=cn[0]*cn[i-1];

  const size_t maxIter(300);
  size_t flag(1);
  for(size_t iter=0;iter<maxIter && flag;iter++)
    {
      flag=0;
      for(size_t i=0;i<iDegree;i++)
	{
	  // calc division 
	  std::complex<double> D(1.0,0);
	  for(size_t j=0;j<iDegree;j++)
	    if (j!=i)
	      D*=(cn[i]-cn[j]);
	  const std::complex<double> modTerm(evalPoly(cn[i])/D);
	  flag += (sqrt(std::norm(modTerm))>eps/100.0) ? 1 : 0;
	  cn[i]=cn[i]- modTerm;
	}
    }
  return cn;
}

std::vector<std::complex<double> >
PolyVar<1>::calcRoots(const double epsilon)
  /*!
    Calculate all the roots of the polynominal.
    Uses the GSL which uses a Hessian reduction of the 
    characteristic compainion matrix.
    \f[ A= \left( -a_{m-1}/a_m -a_{m-2}/a_m ... -a_0/a_m \right) \f]
    where the matrix component below A is the Indenty.
    However, GSL requires that the input coefficient is a_m == 1,
    hence the call to this->compress().
    \param epsilon :: tolerance factor (-ve to use default)
    \return roots (not sorted/uniqued)
  */
{
  ELog::RegMethod RegA("PolyVar<1>","calcRoots");
  unitPrimary(epsilon);

  std::vector<std::complex<double> > Out;
  // Zero State:
  if (iDegree<=0)
    return Out;

  Out.resize(iDegree);
  // x+a_0 =0 
  if (iDegree==1)
    {
      Out[0]=std::complex<double>(-PCoeff[0]);
      return Out;
    }
  // x^2+a_1 x+c = 0
  if (iDegree==2)
    {
      const size_t N=
	::solveQuadratic(PCoeff[2],PCoeff[1],PCoeff[0],
			 Out[0],Out[1]);
      Out.resize(N);
      return Out;
    }

  // x^3+a_2 x^2+ a_1 x+c=0
  if (iDegree==3)
    {
      const size_t N=
	::solveCubic(PCoeff[3],PCoeff[2],PCoeff[1],PCoeff[0],
		     Out[0],Out[1],Out[2]);
      Out.resize(N);
      return Out;
    }

#ifndef NO_GSL 
  //
  // THERE IS A QUARTIC / QUINTIC Solution availiable but...
  // WS contains the the hessian matrix if required (eigenvalues/vectors)
  //
  gsl_set_error_handler(&polyHandler);
  gsl_poly_complex_workspace* WS
    (gsl_poly_complex_workspace_alloc(iDegree+1));
  double* RZ=new double[2*iDegree];
  double* PC=new double[iDegree+1];
  for(size_t i=0;i<iDegree+1;i++)
    PC[i]=PCoeff[i];

  gsl_poly_complex_solve(PC,iDegree+1, WS, RZ);
  if (gslflag)
    {
      ELog::EM<<"Failed on  :"<<*this<<ELog::endDebug;
      gslflag=0;
    }
  else 
    {
      for(size_t i=0;i<iDegree;i++)
	Out[i]=std::complex<double>(RZ[2*i],RZ[2*i+1]);
    }

  gsl_poly_complex_workspace_free (WS);
  gsl_set_error_handler(NULL);
  delete [] RZ;
  delete [] PC;

#endif

  return Out;
}

size_t
PolyVar<1>::getCount(const double eps) const
  /*!
    Determine if is zero
    \param eps :: tolerance factor
    \return Number of values greater than eps
  */
{
  size_t cnt(0);
  for(size_t i=0;i<=iDegree;i++)
    if (std::abs(PCoeff[i])>eps)
      cnt++;
  return cnt++;
}

size_t
PolyVar<1>::getVarFlag() const
  /*!
    Determines the variables in the equation
    and returns a flag of bits of each variable
    \return Flag of variable type
  */
{
  return (iDegree>0) ? 1 : 0;
}

size_t
PolyVar<1>::getVarCount() const
  /*!
    Determines the variables in the equation
    and returns a flag of bits of each variable
    \return Flag of variable count
  */
{
  return (iDegree>0) ? 1 : 0; 
}

int
PolyVar<1>::isComplete() const
  /*!
    Determines the all the variables are in the equaition
    \return 1 if complete / 0 if not
  */
{
  return (iDegree>0) ? 1 : 0;
}

int 
PolyVar<1>::isZero(const double eps) const
  /*!
    Determine if is zero
    \param eps :: tolerance value for a single component
    \return 1 if all components are effectively zero.
  */
{
  size_t i;
  for(i=0;i<=iDegree && std::abs(PCoeff[i])<eps;i++) ;
  return (i<=iDegree) ? 0 : 1;
}

int 
PolyVar<1>::isUnit(const double eps) const
  /*!
    Determine if is zero
    \param eps :: Epsilon value
    \retval 2 :: x unit set 
    \retval 1 :: +ve unit 
    \retval 0 :: Not unit
    \retval -1 :: -ve unit 
  */
{
  size_t i;
  for(i=iDegree;i>0 && std::abs(PCoeff[i])<eps;i--) ;
  if (i)
    return 0;
  if (std::abs(std::abs(PCoeff[0])-1.0)>eps)
    return 0;
  return (PCoeff[0]>0.0) ? 1 : -1;
}


int 
PolyVar<1>::isUnitary(const double eps) const
  /*!
    Determine if is zero
    \param eps :: Epsilon value
    \retval 2 :: x unit set 
    \retval 1 :: +ve unit 
    \retval 0 :: Not unit
    \retval -1 :: -ve unit 
  */
{
  size_t item(0);
  int flag(0);
  for(size_t i=iDegree;  i>0 && flag<2;)
    {
      i--;
      if (std::abs(PCoeff[i])>=eps)
        {
	  item=i;
	  flag++;
	}
    }
  if (flag==2 || flag==0) // all zeros are also NOT unit
    return 0;
  if (std::abs(std::abs(PCoeff[item])-1.0)>eps)
    return 0;
  const int sign( (PCoeff[item]>0.0) ? 1 : -1);
  return ((item==0) ? 1 : 2) * sign;
}

double
PolyVar<1>::substitute(const double& V) const
  /*!
    The value of x is set as V, then move all the 
    zy etc values to xy values.
    \param V :: Value for X
    \return new polynomial
   */
{
  return operator()(V);
}

int
PolyVar<1>::read(const std::string& Line)
  /*!
    Given a line of type 
    \f$ x^2+4.0x+3.0x \f$
    convert into a function:
    \param Line :: Line to process 
    \return 0 :: success
    \return -1 :: failure
  */
{
  const char Variable('x');
  std::string CLine=StrFunc::removeSpace(Line);
  setDegree(PolyFunction::getMaxSize(CLine,Variable));
  std::string::size_type pos=CLine.find(Variable);
  double cV;
  while (pos!=std::string::npos)
    {
      size_t compStart(pos);
      int sign(0);     // default is positive but not found
      // get preFunction 
       while(compStart>0)
        {
	  compStart--;
	  if (CLine[compStart]=='+' || CLine[compStart]=='-')
	    {
	      sign=(CLine[compStart]=='+') ? 1 : -1;
	      break;
	    } 
	}
      std::string Comp;      
      if (sign)
	Comp=CLine.substr(compStart+1,pos-1);
      else
        {
	  Comp=CLine.substr(compStart,pos-compStart);
	  sign=1;
	}
      
      // Find power
      size_t pV(1);
      CLine.erase(0,pos+1);
      if (!CLine.empty() && CLine[0]=='^')
        {
	  CLine.erase(0,1);
	  StrFunc::sectPartNum(CLine,pV);
	}
      
      if (!Comp.empty() && !StrFunc::convert(Comp,cV))
	throw ColErr::InvalidLine("PolVarOne::read",Line,0);

      if (Comp.empty())
	cV=1.0;
      PCoeff[pV]=sign*cV;
      // Find next value
      pos=CLine.find(Variable);
    }

  // Process variable at end:
  if (!CLine.empty() && StrFunc::section(CLine,cV))
    PCoeff[0]=cV;

  return 0;
}

int
PolyVar<1>::write(std::ostream& OX,const int prePlus) const
  /*!
    Basic write command
    \param OX :: output stream
    \param prePlus :: An initial +/- to be added
    \return 0 :: value ==0.0
  */
{
  masterWrite& MW=masterWrite::Instance();

  int nowrite(0);
  for(size_t i=iDegree+1;i>0;)
    {
      i--;
      if (std::abs(PCoeff[i])>this->Eaccuracy)
        {
	  if ((nowrite || prePlus) && PCoeff[i]>0.0)
	    OX<<"+";
	  else if (PCoeff[i]<0.0)
	    OX<<"-";
	  // Now write value:
	  if (!i || std::abs(std::abs(PCoeff[i])-1.0)>this->Eaccuracy)
	    OX<<MW.Num(std::abs(PCoeff[i]));
	  if (i)
	    {
	      OX<<"x";
	      if (i!=1)	OX<<"^"<<i;
	    }
	  nowrite=1;
	}
    }
  return nowrite;
}  

}  // NAMESPACE  mathLevel

