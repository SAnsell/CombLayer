/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   support/polySupport.cxx
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
#include <iostream>
#include <cmath>
#include <complex>
#include <vector>
#include <iterator>
#include <algorithm>
#include <numeric>
#include <functional>

#include "Exception.h"
#include "polySupport.h"

/*! 
  \file polySupport.cxx 
*/

template<typename T>
T
polyEval(const std::vector<T>& PCoeff,const T x)
  /*!
    Calcuate an arbitary polynominal
    \param PCoeff :: parameters (c0,c1...)
    \param x :: value
  */
{
  if (PCoeff.empty()) return 0.0;

  T xPower(1.0);
  T sum(0.0);
  for(const T& c : PCoeff)
    {
      sum+=c*xPower;
      xPower*=x;
    }
  return sum;
}

template<typename T>
T
quad(const T aa,const T bb,const T cc,const T x)
/*!
  Calcuate a quadratic
  \param aa :: x^2 value
  \param bb :: x value
  \param cc :: const
  \param x :: Value to calcute ate
  \return value of the quadratic give by
  \f[ ax^2+bx+c \f]
*/
{
  return aa*x*x+bb*x+cc;
}

template<typename T>
size_t 
solveRealQuadratic(const T& a,const T& b,const T& c,
		   T& AnsA,T& AnsB)
/*!
  Solves quadratic  \f[ Ax^2+Bx+C \f]. with only the real roots
  \param a :: x^2 coeff
  \param b :: x coeff
  \param c :: const coeff
  \param OutAns :: roots of the equation 
  \return number of unique and real solutions 
*/
{
  AnsA=0.0;
  AnsB=0.0;
  
  if (a==0.0)
    {
      if (b==0.0) return 0;
      AnsA= -c/b;
      AnsB=AnsA;
      return 1;
    }

  if (c==0.0)
    {
      AnsA=0.0;
      AnsB=b/a;     // a not zero
    }
  else
    {
      T D=b*b-4*a*c;
      if (D<0.0) return 0;  // no real roots
      D=std::sqrt(D);
      if (b>=0.0)
	{
	  AnsA= (-b-D)/(2.0*a);
	  AnsB= (2.0*c)/(-b-D);
	}
      else
	{
	  AnsA= (-b+D)/(2.0*a);
	  AnsB= (2.0*c)/(-b+D);
	}
    }
  if (AnsA>AnsB)
    std::swap(AnsA,AnsB);
  return (AnsA==AnsB) ? 1 : 2;
}

template<typename T>
size_t 
solveRealQuadratic(const std::vector<T>& Coeff,
		   T& AnsA,T& AnsB)
/*!
  Solves quadratic  \f[ Ax^2+Bx+C \f]. with only the real roots
  \param Coef :: iterator over all the coefficients in the order
  \f[ Ax^2+Bx+C \f].
  \param OutAns :: roots of the equation 
  \return number of unique and real solutions 
*/
{
  return solveRealQuadratic(Coeff[0],Coeff[1],Coeff[2],AnsA,AnsB);
}

template<typename T>
size_t 
solveQuadratic(const T& a,const T& b,const T& c,
	       std::complex<T>& AnsA,std::complex<T>& AnsB)
/*!
  Solves Complex Quadratic  \f[ Ax^2+Bx+C \f].
  \param a :: x^2 coeff
  \param b :: x coeff
  \param c :: const coeff
  \param AnsA :: complex roots of the equation 
  \param AnsB :: complex roots of the equation 
  \return number of unique solutions 
*/
{
  std::pair<std::complex<T>,std::complex<T> > OutAns;
  const size_t res=solveQuadratic(a,b,c,OutAns);
  AnsA=OutAns.first;
  AnsB=OutAns.second;
  return res;
}

template<typename T>
size_t 
solveQuadratic(const std::vector<T>& Coef,
	       std::pair<std::complex<T>,
			 std::complex<T> >& OutAns)
/*!
  Solves Complex Quadratic 
  \param Coef :: iterator over all the coefients in the order
  \f[ Ax^2+Bx+C \f].
  \param OutAns :: complex roots of the equation 
  \return number of unique solutions 
*/
{
  return solveQuadratic(Coef[0],Coef[1],Coef[2],OutAns);
			
}

template<typename T>
size_t 
solveQuadratic(const T* D,
	       std::pair<std::complex<T>,
			 std::complex<T> >& OutAns)
/*!
  Solves Complex Quadratic   \f[ Ax^2+Bx+C \f].
  \param D :: Parameter array (a,b,c)
  \param OutAns :: complex roots of the equation 
  \return number of unique solutions 
*/
{
  return solveQuadratic(D[0],D[1],D[2],OutAns);
}

template<typename T>
size_t 
solveQuadratic(const T& a,const T& b,const T& c,
	       std::pair<std::complex<T>,
	                 std::complex<T> >& OutAns)
/*!
  Solves Complex Quadratic  \f[ Ax^2+Bx+C \f].
  \param a :: x^2 coeff
  \param b :: x coeff
  \param c :: const coeff
  \param OutAns :: complex roots of the equation 
  \return number of unique solutions 
*/
{
  T cf;

  if (a==0.0)
    {
      if (b==0.0)
	{ 
	  OutAns.first=std::complex<T>(0.0,0.0);
	  OutAns.second=std::complex<T>(0.0,0.0);
	  return 0;
	}
      else
	{
	  OutAns.first=std::complex<T>(-c/b,0.0);
	  OutAns.second=OutAns.first;
	  return 1;
	}
    }
  cf=b*b-4*a*c;
  if (cf>=0)          /* Real Roots */
    {
      const T q=(b>=0) ? -0.5*(b+std::sqrt(cf)) :
	-0.5*(b-std::sqrt(cf));
      OutAns.first=std::complex<T>(q/a,0.0);
      OutAns.second=std::complex<T>(c/q,0.0);
      return (cf==0) ? 1 : 2;
    } 

  const std::complex<T> CQ(-0.5*b,
			   ((b>=0) ? -0.5*std::sqrt(-cf) :
			    0.5*std::sqrt(-cf)));
  OutAns.first= CQ/a;
  OutAns.second=c/CQ;
  return 2;
}

template<typename T>
size_t
solveCubic(const std::vector<T>& DVec,
	   std::complex<T>& AnsA,
	   std::complex<T>& AnsB,
	   std::complex<T>& AnsC)
/*!
  Solves Cubic equation
  Coef of eqn: \f[ Ax^3+Bx^2+Cx+D \f].
  \param DVec :: Parameters x^3 to const
  \param AnsA :: complex roots of the equation 
  \param AnsB :: complex roots of the equation 
  \param AnsC :: complex roots of the equation 
  \return number of unique solutions 
*/
{
  return solveCubic(DVec[0],DVec[1],DVec[2],DVec[3],AnsA,AnsB,AnsC);
}


template<typename T>
size_t
solveCubic(const T* D,
	   std::complex<T>& AnsA,
	   std::complex<T>& AnsB,
	   std::complex<T>& AnsC)
/*!
  Solves Cubic equation
  Iiterator over all the coefficients in the order
  \f[ Ax^3+Bx^2+Cx+D \f].
  \param D :: Parameters x^3 to const
  \param AnsA :: complex roots of the equation 
  \param AnsB :: complex roots of the equation 
  \param AnsC :: complex roots of the equation 
  \return number of unique solutions 
*/
{
  return solveCubic(D[0],D[1],D[2],D[3],AnsA,AnsB,AnsC);
}

template<typename T>
size_t
solveCubic(const T A,const T B,
	   const T C,const T D,
	   std::complex<T>& AnsA,
	   std::complex<T>& AnsB,
	   std::complex<T>& AnsC) 
  /*!
    Solves Cubic equation of type
    \f$ ax^3+bx^2+cx+d=0 \f$
    \param A :: x^3 value
    \param B :: x^2 value
    \param C :: x value
    \param D :: const value
    \param AnsA :: complex roots of the equation 
    \param AnsB :: complex roots of the equation 
    \param AnsC :: complex roots of the equation 
    \return number of unique solutions 
  */
{
  T q,r;        // solution parameters 
  T s,t,termR,termI,discrim;
  T q3,r13;

  if (std::abs(A)<1e-30)
    return solveQuadratic(B,C,D,AnsA,AnsB);

  const T b = B/A;
  const T c = C/A;
  const T d = D/A;
  std::pair<std::complex<T>,std::complex<T> > SQ;
  
  q = (3.0*c - (b*b))/9.0;                   // -q
  r = -27.0*d + b*(9.0*c - 2.0*b*b);       // -r 
  r /= 54.0;
 
  discrim = q*q*q + r*r;           // r^2-qq^3 
  // The first root is always real. 
  termR = (b/3.0);

  if (discrim > 1e-13)  // one root real, two are complex 
    { 
      s = r + std::sqrt(discrim);
      s = ((s < 0) ? -std::pow(-s, (1.0/3.0)) : std::pow(s, (1.0/3.0)));
      t = r - std::sqrt(discrim);
      t = ((t < 0) ? -std::pow(-t, (1.0/3.0)) : std::pow(t, (1.0/3.0)));
      AnsA=std::complex<T>(-termR+s+t,0.0);
      // second real point.
      termR += (s + t)/2.0;
      termI = std::sqrt(3.0)*(-t + s)/2;
      AnsB=std::complex<T>(-termR,termI);
      AnsC=std::complex<T>(-termR,-termI);

      return 3;
    }

  // The remaining options are all real 

  if (discrim< -1e-13) // All roots real and different 
    {
      q = -q;
      q3 = q*q*q;
      q3 = std::acos(-r/std::sqrt(q3))/3.0;
      r13 = -2.0*std::sqrt(q);
      constexpr T tpi=2.0*M_PI/3.0;
      AnsA=std::complex<T>(-termR + r13*std::cos(q3),0.0);
      AnsB=std::complex<T>(-termR + r13*std::cos(q3 + tpi),0.0);
      AnsC=std::complex<T>(-termR + r13*std::cos(q3 - tpi),0.0);
	    
      return 3;
    }

  // Only option left is that all roots are real and unequal 
  // (to get here, q*q*q=r*r)

  r13 = ((r < 0) ? -std::pow(-r,(1.0/3.0)) : std::pow(r,(1.0/3.0)));
  AnsA=std::complex<T>(-termR+2.0*r13,0.0);
  AnsB=std::complex<T>(-(r13+termR),0.0);
  AnsC=std::complex<T>(-(r13+termR),0.0);
  return 2;
}

template<typename T>
size_t
solveRealCubic(const T A,const T B,
	       const T C,const T D,
	       T& AnsA,T& AnsB,T& AnsC) 
/*!
    Solves Cubic equation of type and gives only real term
    \f$ ax^3+bx^2+cx+d=0 \f$
    \param A :: x^3 value
    \param B :: x^2 value
    \param C :: x value
    \param D :: const value
    \param AnsA :: real roots of the equation 
    \param AnsB :: real roots of the equation 
    \param AnsC :: real roots of the equation 
    \return number of unique (real) solutions 
  */
{
  T q,r;        // solution parameters 
  T s,t,termR,discrim;
  T r13;
  
  if (std::abs(A)<1e-30)
    return solveRealQuadratic(B,C,D,AnsA,AnsB);

  const T b = B/A;
  const T c = C/A;
  const T d = D/A;

  
  q = (3.0*c - (b*b))/9.0;                   // -q
  r = -27.0*d + b*(9.0*c - 2.0*b*b);       // -r 
  r /= 54.0;

  discrim = q*q*q + r*r;           // qq^3 - r^2
  // The first root is always real. 
  termR = (b/3.0);

  if (discrim > 1e-13)  // one root real, two are complex 
    { 
      s = r + std::sqrt(discrim);
      s = ((s < 0) ? -std::pow(-s, (1.0/3.0)) : std::pow(s, (1.0/3.0)));
      t = r - std::sqrt(discrim);
      t = ((t < 0) ? -std::pow(-t, (1.0/3.0)) : std::pow(t, (1.0/3.0)));
      AnsA=-termR+s+t;
      return 1;
    }

  // The remaining options are all real 

  if (discrim< -1e-13) // All roots real and different  (q<0)
    {
      constexpr T tpi=2.0*M_PI/3.0;
      q=-q;
      T q3=q*q*q;
      q3 = std::acos(-r/std::sqrt(q3))/3.0;
      r13 = -2.0*std::sqrt(q);
      AnsA=-termR + r13*std::cos(q3);
      AnsB=-termR + r13*std::cos(q3 + tpi);
      AnsC=-termR + r13*std::cos(q3 - tpi);
      return 3;
    }

  // Only option left is that all roots are real and two are equal
  // (to get here, q*q*q=r*r)
  // which two ?

  r13 = ((r < 0) ? -std::pow(-r,(1.0/3.0)) : std::pow(r,(1.0/3.0)));
  AnsA=-termR+2.0*r13;
  AnsB=-(r13+termR);
  AnsC=-(r13+termR);   
  return 3;
}

///\cond TEMPLATE

template double polyEval(const std::vector<double>&,const double);
double quad(const double,const double,const double,const double);
template size_t 
solveRealQuadratic(const double&,const double&,const double&,double&,double&);
template size_t 
solveRealQuadratic(const std::vector<double>&,double&,double&);
template size_t 
solveQuadratic(const double*,std::pair<std::complex<double>,
	       std::complex<double> >&);
template size_t 
solveQuadratic(const double&,const double&,const double&,
	       std::pair<std::complex<double>,
	       std::complex<double> >&);
template size_t 
solveQuadratic(const double&,const double&,const double&,
	       std::complex<double>&,std::complex<double>&);
template size_t 
solveQuadratic(const std::vector<double>&,std::pair<std::complex<double>,
	       std::complex<double> >&);

template size_t 
solveCubic(const std::vector<double>&,std::complex<double>&,
	   std::complex<double>&,std::complex<double>&);

template size_t 
solveCubic(const double*,std::complex<double>&,
	   std::complex<double>&,std::complex<double>&);

template size_t 
solveCubic(const double,const double,const double,
	   const double,std::complex<double>&,
	   std::complex<double>&,std::complex<double>&);

template size_t solveRealCubic(const double,const double,const double,
			       const double,double&,double&,double&);

// LONG values
template long double polyEval
(const std::vector<long double>&,const long double);

long double quad(const long double,const long double,
		 const long double,const long double);
template size_t 
solveRealQuadratic(const long double&,const long double&,
		   const long double&,long double&,long double&);

template size_t 
solveRealQuadratic(const std::vector<long double>&,
		   long double&,long double&);
template size_t 
solveQuadratic(const long double*,
	       std::pair<std::complex<long double>,
	       std::complex<long double> >&);
template size_t 
solveQuadratic(const long double&,
	       const long double&,const long double&,
	       std::pair<std::complex<long double>,
	       std::complex<long double> >&);
template size_t 
solveQuadratic(const long double&,const long double&,const long double&,
	       std::complex<long double>&,std::complex<long double>&);
template size_t 
solveQuadratic(const std::vector<long double>&,
	       std::pair<std::complex<long double>,
	       std::complex<long double> >&);

template size_t 
solveCubic(const std::vector<long double>&,std::complex<long double>&,
	   std::complex<long double>&,std::complex<long double>&);

template size_t 
solveCubic(const long double*,std::complex<long double>&,
	   std::complex<long double>&,std::complex<long double>&);

template size_t 
solveCubic(const long double,const long double,const long double,
	   const long double,std::complex<long double>&,
	   std::complex<long double>&,std::complex<long double>&);

template size_t solveRealCubic(const long double,const long double,
			       const long double,const long double,
			       long double&,long double&,long double&);

///\endcond
