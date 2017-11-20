/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   support/polySupport.cxx
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

double
quad(const double aa,const double bb,const double cc,const double x)
/*!
  Calcuate a quadratic
  \param aa :: x^2 value
  \param bb :: x^2 value
  \param cc :: const
  \param x :: Value to calcute ate
  \return value of the quadratic give by
  \f[ ax^2+bx+c \f]
*/
{
  return aa*x*x+bb*x+cc;
}

size_t 
solveQuadratic(const double& a,const double& b,const double& c,
	       std::complex<double>& AnsA,std::complex<double>& AnsB)
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
  std::pair<std::complex<double>,std::complex<double> > OutAns;
  const size_t res=solveQuadratic(a,b,c,OutAns);
  AnsA=OutAns.first;
  AnsB=OutAns.second;
  return res;
}

size_t 
solveQuadratic(const std::vector<double>& Coef,
	       std::pair<std::complex<double>,
			 std::complex<double> >& OutAns)
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

size_t 
solveQuadratic(const double* D,
	       std::pair<std::complex<double>,
			 std::complex<double> >& OutAns)
/*!
  Solves Complex Quadratic   \f[ Ax^2+Bx+C \f].
  \param D :: Parameter array (a,b,c)
  \param OutAns :: complex roots of the equation 
  \return number of unique solutions 
*/
{
  return solveQuadratic(D[0],D[1],D[2],OutAns);
}

size_t 
solveQuadratic(const double& a,const double& b,const double& c,
	       std::pair<std::complex<double>,
	                 std::complex<double> >& OutAns)
/*!
  Solves Complex Quadratic  \f[ Ax^2+Bx+C \f].
  \param a :: x^2 coeff
  \param b :: x coeff
  \param c :: const coeff
  \param OutAns :: complex roots of the equation 
  \return number of unique solutions 
*/
{
  double cf;

  if (a==0.0)
    {
      if (b==0.0)
	{ 
	  OutAns.first=std::complex<double>(0.0,0.0);
	  OutAns.second=std::complex<double>(0.0,0.0);
	  return 0;
	}
      else
	{
	  OutAns.first=std::complex<double>(-c/b,0.0);
	  OutAns.second=OutAns.first;
	  return 1;
	}
    }
  cf=b*b-4*a*c;
  if (cf>=0)          /* Real Roots */
    {
      const double q=(b>=0) ? -0.5*(b+sqrt(cf)) : -0.5*(b-sqrt(cf));
      OutAns.first=std::complex<double>(q/a,0.0);
      OutAns.second=std::complex<double>(c/q,0.0);
      return (cf==0) ? 1 : 2;
    } 

  const std::complex<double> CQ(-0.5*b,
			  ((b>=0) ? -0.5*sqrt(-cf) :
			   0.5*sqrt(-cf)));
  OutAns.first= CQ/a;
  OutAns.second=c/CQ;
  return 2;
}

size_t
solveCubic(const std::vector<double>& DVec,
	   std::complex<double>& AnsA,
	   std::complex<double>& AnsB,
	   std::complex<double>& AnsC)
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



size_t
solveCubic(const double* D,
	   std::complex<double>& AnsA,
	   std::complex<double>& AnsB,
	   std::complex<double>& AnsC)
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

size_t
solveCubic(const double A,const double B,
	   const double C,const double D,
	   std::complex<double>& AnsA,
	   std::complex<double>& AnsB,
	   std::complex<double>& AnsC) 
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
  double q,r;        // solution parameters 
  double s,t,termR,termI,discrim;
  double q3,r13;

  if (std::abs(A)<1e-30)
    return solveQuadratic(B,C,D,AnsA,AnsB);

  const double b = B/A;
  const double c = C/A;
  const double d = D/A;
  std::pair<std::complex<double>,std::complex<double> > SQ;
  
  q = (3.0*c - (b*b))/9.0;                   // -q
  r = -27.0*d + b*(9.0*c - 2.0*b*b);       // -r 
  r /= 54.0;
 
  discrim = q*q*q + r*r;           // r^2-qq^3 
  // The first root is always real. 
  termR = (b/3.0);

  if (discrim > 1e-13)  // one root real, two are complex 
    { 
      s = r + sqrt(discrim);
      s = ((s < 0) ? -pow(-s, (1.0/3.0)) : pow(s, (1.0/3.0)));
      t = r - sqrt(discrim);
      t = ((t < 0) ? -pow(-t, (1.0/3.0)) : pow(t, (1.0/3.0)));
      AnsA=std::complex<double>(-termR+s+t,0.0);
      // second real point.
      termR += (s + t)/2.0;
      termI = sqrt(3.0)*(-t + s)/2;
      AnsB=std::complex<double>(-termR,termI);
      AnsC=std::complex<double>(-termR,-termI);

      return 3;
    }

  // The remaining options are all real 

  if (discrim< -1e-13) // All roots real and different 
    {
      q = -q;
      q3 = q*q*q;
      q3 = acos(-r/sqrt(q3));
      r13 = -2.0*sqrt(q);
      AnsA=std::complex<double>(-termR + r13*cos(q3/3.0),0.0);
      AnsB=std::complex<double>(-termR + r13*cos((q3 + 2.0*M_PI)/3.0),0.0);
      AnsC=std::complex<double>(-termR + r13*cos((q3 - 2.0*M_PI)/3.0),0.0);
	    
      return 3;
    }

  // Only option left is that all roots are real and unequal 
  // (to get here, q*q*q=r*r)

  r13 = ((r < 0) ? -pow(-r,(1.0/3.0)) : pow(r,(1.0/3.0)));
  AnsA=std::complex<double>(-termR+2.0*r13,0.0);
  AnsB=std::complex<double>(-(r13+termR),0.0);
  AnsC=std::complex<double>(-(r13+termR),0.0);
  return 2;
}
