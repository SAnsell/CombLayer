/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   support/mathSupport.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *s
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
#include <map>
#include <iterator>
#include <algorithm>
#include <numeric>
#include <functional>

#include "Exception.h"
#include "doubleErr.h"
#include "MapRange.h"
#include "mathSupport.h"

/*! 
  \file mathSupport.cxx 
*/

double
mathFunc::logFromLinear(const double A,const double B,const size_t N,
                        const size_t index)
  /*!
    Calculate the log step in a range A-B
    \param A :: Low range 
    \param B :: High range 
    \param N :: Number of steps [not checked]
    \param index :: value at step size [index between 0 - N] 
    \return value at log(Index)
  */
{
  const double step(static_cast<double>(index)*
		    log(fabs((B-A)/A))/static_cast<double>(N));
  return (A>B) ? B*exp(step) : A*exp(step);
}
  

size_t
lowBitIndex(const size_t& x)
/*!
  Returns the lowest bit
  HIGHLY specific to GCC
  \param x :: number to test
  \return one plus the index of the least significant 1-bit of x, 
  or if x is zero, returns zero.
*/
{
  return static_cast<size_t>(__builtin_ffsl(static_cast<long>(x)));
}

size_t
lowBitIndex(const unsigned int& x)
/*!
  Returns the lowest bit
  HIGHLY specific to GCC
  \param x :: number to test
  \return one plus the index of the least significant 1-bit of x, 
  or if x is zero, returns zero.
*/
{
  return static_cast<size_t>(__builtin_ffs(static_cast<int>(x)));
}

size_t
countBits(const unsigned int& u)
  /*!
    Clever trick from MIT. 
    Note: 3 bits are 4a+2b+c. Shift right 2a+b and double shift a
    Add all together == a+b+c  . number of bits set
    \param u :: Number to test
    \return number of bits
  */
{
  // This is a nice way of getting (a0+a1+a2) in little groups of three
  const unsigned int uCount = u
    - ((u >> 1) & 033333333333)
    - ((u >> 2) & 011111111111);
  return static_cast<size_t>(((uCount + (uCount >> 3))
			      & 030707070707) % 63);
}

size_t
countBits(const size_t& u)
  /*!
    Clever trick from MIT to obtain number of bits 
    \param u :: Number to test
    \return number of bits
  */
{
  // This is a nice way of getting (a0+a1+a2) in little groups of three
  size_t uCount = u
    - ((u >> 1) & 0333333333333333333333UL)
    - ((u >> 2) & 0111111111111111111111UL);
  return
    ((uCount + (uCount >> 3))
     & 0307070707070707070707UL) % 63;

}

long int
factorial(const int N)
/*!
  Ugly function to get fractorial of N
  \param N :: Number to calculate N! 
  \return N!
*/
{
  long int out(1);
  for(int i=2;i<=N;i++)
    out*=i;
  return out;
}

double
factorialDB(const int N)
/*!
  Ugly function to get fractorial of N
  \param N :: Number to calculate N! 
  \return N!
*/
{
  double out(1);
  for(int i=2;i<=N;i++)
    out*=i;
  return out;
}

double
fibonacci(const int N)
/*!
  Determine the fibonacci number 
  \param N :: Index number
  \return the fibonacci number 
*/
{
  const double sqr5=sqrt(5.0);
  const double phi=(1.0+sqr5)/2.0;
  return (pow(phi,N) - pow((1.0-phi),N))/sqr5;
}

double
bernoulliNumber(const int N)
/*!
  Ugly function to get the bernoulli numbers
  \param N :: Index value
  \return bernolli number : not finished
*/
{
  if (N==0) return 1;
  if (N==1) return -0.5;
  return 0.0;
}


double 
invErf(const double P)
/*!
  Approximation to the inverse error function
  \param P :: point to calculate ierf(P) from
  \returns ierf(P)
*/
{
  const double a[]={ -0.5751703,-1.896513,-.5496261e-1 };
  const double b[]={ -0.1137730,-3.293474,-2.374996,-1.187515 }; 
  const double c[]={ -0.1146666,-0.1314774,-0.2368201,0.5073975e-1 };
  const double d[]={ -44.27977,21.98546,-7.586103 };
  const double e[]={ -0.5668422e-1,0.3937021,-0.3166501,0.6208963e-1 };
  const double f[]={ -6.266786,4.666263,-2.962883 };
  const double g[]={ 0.1851159E-3,-0.2028152E-2,-0.1498384,0.1078639e-1 };
  const double h[]={ 0.9952975E-1,0.5211733,-0.6888301e-1 };
  //  double  RINFM=1.7014E+38;
  if (P<-1 || P>1)
    return 0;

  const double Sigma=(P>0 ? 1 : -1);
  const double Z=fabs(P);
  double F;
  if (Z>0.85) 
    {
      const double A=1-Z;
      const double W=sqrt(-log(A+A*Z));
      if (W>=2.5) 
        {
	  if (W>=4.) 
	    {
	      const double wInv=1.0/W;
	      const double sn=((g[3]*wInv+g[2])*wInv+g[1])*wInv;
	      const double sd=((wInv+h[2])*wInv+h[1])*wInv+g[0];
	      F=W+W*(g[0]+sn/sd);
	    } 
	  else 
	    {
	      const double sn=((e[3]*W+e[2])*W+e[1])*W;
	      const double sd=((W+f[2])*W+f[1])*W+f[0];
	      F=W+W*(e[0]+sn/sd);
	    }
	} 
      else 
        {
	  const double sn=((c[3]*W+c[2])*W+c[1])*W;
	  const double sd=((W+d[2])*W+d[1])*W+d[0];
	  F=W+W*(c[0]+sn/sd);
	}
    } 
  else 
    {
      const double Z2=Z*Z;
      F=Z+Z*(b[0]+a[0]*Z2/(b[1]+Z2+a[1]/(b[2]+Z2+a[2]/(b[3]+Z2))));
    }
  return Sigma*F;
}

double 
normalDist(const double x)
/*!
  Calcuates the inverse normal distribution usign Halleys
  method.
  \param x :: value to calulate from (0>x>1)
  \returns 0 if x>1.0 or  x<0.0
  \retval normal distribute to machine pecision.
*/
{
  const double a[]={ -3.969683028665376e+01, 2.209460984245205e+02,
		     -2.759285104469687e+02, 1.383577518672690e+02,
		     -3.066479806614716e+01, 2.506628277459239e+00 };
  
  const double b[]= { -5.447609879822406e+01, 1.615858368580409e+02,
		      -1.556989798598866e+02, 6.680131188771972e+01,
		      -1.328068155288572e+01 };

  const double c[]= { -7.784894002430293e-03, -3.223964580411365e-01,
		      -2.400758277161838e+00, -2.549732539343734e+00,
		      4.374664141464968e+00, 2.938163982698783e+00 };

  const double d[] = { 7.784695709041462e-03, 3.224671290700398e-01,
		       2.445134137142996e+00, 3.754408661907416e+00 };

  const double pLow(0.02425);
  const double pHigh=(1.0-pLow);

  double xOut;
  
  if (x>=1.0 || x<=0.0)
    return 0.0;

  if (x<pLow)
    {
      const double q = sqrt(-2.0*log(x));
      xOut= (((((c[0]*q+c[1])*q+c[2])*q+c[3])*q+c[4])*q+c[5]) / ((((d[0]*q+d[1])*q+d[2])*q+d[3])*q+1);
    }
  else if (x>pHigh)
    {
      const double q = sqrt(-2.0*log(1-x));
      xOut= -(((((c[0]*q+c[1])*q+c[2])*q+c[3])*q+c[4])*q+c[5]) / ((((d[0]*q+d[1])*q+d[2])*q+d[3])*q+1);
    }
  else
    {
      const double q = x - 0.5;
      const double r = q*q;
      xOut= (((((a[0]*r+a[1])*r+a[2])*r+a[3])*r+a[4])*r+a[5])*q /
	(((((b[0]*r+b[1])*r+b[2])*r+b[3])*r+b[4])*r+1);
    }


  const double e = 0.5 * erfc(-xOut/sqrt(2)) - x;
  const double u = e * sqrt(2*M_PI) * exp(xOut*xOut/2);
  return xOut - u/(1 + xOut*u/2);
}

template<typename T>
T
norm(const std::vector<T>& Vec)
/*!
  Function to calculate the mean of a vector.
  \param Vec :: vector to use
  \returns \f$ \sqrt{V.V} \f$
*/
{
  T Sum(0);
  for(const T& A : Vec)
    Sum+= A*A;
  return sqrt(Sum);
}


template<typename T,typename U>
void
indexSort(const std::vector<T>& pVec,std::vector<U>& Index)
  /*!
    Function to take a vector and sort the vector 
    so as to produce an index.
    Leaves the vector unchanged.
    \param pVec :: Vector of un-sorted values
    \param Index :: Index out positions in pVec that would make it sorted
    \tparam U :: Indexable type
  */
{
  Index.resize(pVec.size());
  std::vector<typename std::pair<T,U> > PartList;
  PartList.resize(pVec.size());

  transform(pVec.begin(),pVec.end(),PartList.begin(),
	    mathSupport::PIndex<T>());
  sort(PartList.begin(),PartList.end());
  transform(PartList.begin(),PartList.end(),Index.begin(),
	    mathSupport::PSep<T>());
  
  return;
}

template<typename T> 
typename std::vector<T>::const_iterator
iteratorPos(const std::vector<T>& xArray,const T& Aim)
/*!
  Function finds the iterator position in xArray that 
  which correspond to the next value after aim
  \param xArray :: vector of points in array
  \param Aim :: Point to aim for
  \retval iterator in xArray if Aim>= first or Aim<=last
  \retval xArray.end() if Aim outside of xArray
*/
{
  if (Aim>xArray.back() || Aim<xArray[0])
    return xArray.end();

  typename std::vector<T>::const_iterator xV=
    lower_bound(xArray.begin(),xArray.end(),Aim);
  return xV;
}

template<typename T>
long int
indexPos(const std::vector<T>& xArray,const T& Aim)
/*!
  Detemine the point that matches an array.
  Such that  \f$ xArray[index] > Aim> xArray[index+1] \f$
  This also gives -1 and size-1
  \param xArray :: Array to search
  \param Aim :: Aim Point
  \return position in array  [ranged between: -1 and size-1  ] 
*/
{
  if (xArray.empty() || Aim<=xArray.front())
    return -1;
  if (Aim>=xArray.back())
    return static_cast<long int>(xArray.size())-1;

  typename std::vector<T>::const_iterator 
    xV=lower_bound(xArray.begin(),xArray.end(),Aim);
  return distance(xArray.begin(),xV)-1;
}

template<typename Xtype,typename Ytype>
Ytype
polInterp(const Xtype& Aim,const unsigned int Order,
	  const std::vector<Xtype>& Xpts,const std::vector<Ytype>& Ypts) 
/*!
  Impliments a polynominal fit on data
  from around Order values of Aim. Note that Xpts and ypts
  does not need to be the same type.
  \param Aim :: X value to interpolate to
  \param Order :: Polynominal order 
  \param Xpts :: X-array 
  \param Ypts :: Y-array 
  \return Interpolated point [ y(X) ]
*/
{
  typename std::vector<Xtype>::const_iterator xP=iteratorPos<Xtype>(Xpts,Aim);

  if (xP==Xpts.end())  // indicates out of range
    {
      return (Aim>Xpts.back()) ?
	Ypts[Xpts.size()-1] : Ypts[0];
    }
  const unsigned int pt=
    static_cast<unsigned int>(distance(Xpts.begin(),xP));
  const unsigned int Orange=(Order+1)/2;   

  const unsigned int stx((pt>Orange) ? pt-Orange : 0);
  const unsigned int etx(
     (pt+Orange>=static_cast<unsigned int>(Xpts.size())) ? 
     pt+Orange :  static_cast<unsigned int>(Xpts.size())-1);

  const unsigned int Oeff((etx-stx)>Order ? Order : etx-stx);
  return polFit<Xtype,Ytype>(Aim,Oeff,Xpts.begin()+stx,Ypts.begin()+stx);
}

template<typename Xtype,typename Ytype>
Ytype
polFit(const Xtype& Aim,const unsigned int Order,
       typename std::vector<Xtype>::const_iterator X,
       typename std::vector<Ytype>::const_iterator Y)
/*!
  Function to carry out interpolation
  \param Aim :: aim point
  \param Order :: value of the interator
  \param X :: X iterator point (at start not middle)
  \param Y :: Y iterator point corresponding to X
  \return Polynominal fit point
*/
{
  
  Xtype testDiff,diff;

  std::vector<Ytype> C(Order);      //  C and D Var
  std::vector<Ytype> D(Order);      // 

  size_t ns(0);
  
  diff=static_cast<Xtype>(fabs(Aim-X[0]));
  C[0]=Y[0];
  D[0]=Y[0];
  for(size_t i=1;i<Order;i++)
    {
      const long int li(static_cast<long int>(i));
      testDiff=static_cast<Xtype>(fabs(Aim-X[li]));
      if (diff>testDiff)
        {
          ns=i;
          diff=testDiff;
        }
      C[i]=Y[li];
      D[i]=Y[li];
    }

  Ytype out=Y[static_cast<long int>(ns)];

  Ytype den;
  Ytype w;
  Ytype outSigma;
  Xtype ho,hp;           // intermediate variables 

  for(size_t m=1;m<Order;m++)
    {
      for(size_t i=0;i<Order-m;i++)
        {
          ho=X[static_cast<long int>(i)]-Aim;
          hp=X[static_cast<long int>(i+m)]-Aim;
          w=C[i+1]-D[i];
          /*      den=ho-hp;  -- test !=0.0 */
          den=w/static_cast<Ytype>(ho-hp);
          D[i]=static_cast<Ytype>(hp)*den;
          C[i]=static_cast<Ytype>(ho)*den;
        }
      outSigma= (2*ns < (Order-m)) ? C[ns] : D[--ns];
      out+=outSigma;
    }
  return out;
}

template<typename T>
T
intQuadratic(const typename std::vector<T>::const_iterator& Xpts,
	     const typename std::vector<T>::const_iterator& Ypts) 
/*!
  This function carries out a quadratic polynominal integration
  for the three points Xpts,Ypts. It assumes that Xpts are random.
  It is designed to be used in 
  \param Xpts :: Iterator to 3pts
  \param Ypts :: Iterator to 3pts
  \return Quadratic integration 
*/
{
  const T C =  Ypts[0];
  const T x1= Xpts[0];
  const T x2= Xpts[1]-x1;
  const T x3= Xpts[2]-x1;
  const T y2= Ypts[1]-C;
  const T y3= Ypts[2]-C;

  const T FracD=x2*x3*x3-x2*x2*x3;
  const T B= (x3*x3*y2-x2*x2*y3)/FracD;
  const T A= -(x3*y2-x2*y3)/FracD;

  return x3*(C+x3*B/2.0+x3*x3*A/3.0);
}

template<typename T>
T
derivQuadratic(const typename std::vector<T>::const_iterator& Xpts,
	       const typename std::vector<T>::const_iterator& Ypts) 
/*!
  This function carries out a quadratic polynominal differentuation
  for the three points Xpts,Ypts. It assumes that Xpts are random.
  \param Xpts :: Iterator for three X-points
  \param Ypts :: Iterator for three Y-points
  \return derivative at midpoint
*/
{
  const T C = Ypts[0];
  const T x1= Xpts[0];
  const T x2= Xpts[1]-x1;
  const T x3= Xpts[2]-x1;
  const T y2= Ypts[1]-C;
  const T y3= Ypts[2]-C;

  const T FracD=x2*x3*x3-x2*x2*x3;
  const T B= (x3*x3*y2-x2*x2*y3)/FracD;
  const T A= -(x3*y2-x2*y3)/FracD;

  return 2.0*A*x2+B;
}

template<typename T>
T
d2dxQuadratic(const typename std::vector<T>::const_iterator& Xpts,
	      const typename std::vector<T>::const_iterator& Ypts) 
/*!
  This function carries out a quadratic polynominal differentuation
  for the three points Xpts,Ypts. It assumes that Xpts are random.
  \param Xpts :: XPoints
  \param Ypts :: YPoints 
  \return dy/dx at Xpts[1]
*/
{
  const T C = Ypts[0];
  const T x1= Xpts[0];
  const T x2= Xpts[1]-x1;
  const T x3= Xpts[2]-x1;
  const T y2= Ypts[1]-C;
  const T y3= Ypts[2]-C;

  const T FracD=x2*x3*x3-x2*x2*x3;
  //  const T B= (x3*x3*y2-x2*x2*y3)/FracD;
  const T A= -(x3*y2-x2*y3)/FracD;

  return A*static_cast<T>(2.0);
}



template<typename T>
long int
mathFunc::binSearch(const typename std::vector<T>::const_iterator& pVecB,
		    const typename std::vector<T>::const_iterator& pVecE,
		    const T& V)
/*!
  Determine a binary search of a component
  Returns 0 if below index and 
  \param pVecB :: start point in vector list
  \param pVecE :: end point in vector list
  \param V :: Item to search for
  \return position index
*/
{
  if (pVecB==pVecE) return 0;
  if (*pVecB>=V)
    return 0;
  if (*(pVecE-1)<=V)
    return distance(pVecB,pVecE)-1;
  
  typename std::vector<T>::const_iterator   
    vc=lower_bound(pVecB,pVecE,V,std::less<T>());

  return distance(pVecB,vc);
}

template<template<typename T,typename Alloc>  
	 class V,typename T,typename Alloc> 
T mathFunc::minDifference(const V<T,Alloc>& Vec,const T& tol)
/*!
  Find the minimum seperation between two components
  in a list/vector 
  \param Vec :: Vector to check [ordered]
  \param tol :: Tolerance value
  \return Gap [>Tol]
*/
{
  if (Vec.empty()) 
    return tol;

  typename V<T,Alloc>::const_iterator Avc;
  typename V<T,Alloc>::const_iterator Bvc;
  Avc=Vec.begin();
  Bvc=Vec.begin();

  // One entry return only tol: 
  Bvc++;
  if (Bvc==Vec.end()) return tol;
  // Multiple entries
  T diff(*Bvc - *Avc);  
  for(Avc++,Bvc++;Bvc!=Vec.end();Avc++,Bvc++)
    {
      T tmp=*Bvc - *Avc;
      if (tmp<diff && tmp>tol)
	std::swap(tmp,diff);
    }
  return diff;
}

template<typename T,typename U>
void
mathFunc::crossSort(std::vector<T>& pVec,std::vector<U>& Base)
/*!
  Function to take a vector and sort the vector and
  update Base in the equivalent manor.
  \param pVec :: Vector of variables to index
  \param Base :: 
*/
{
  if (pVec.size()!=Base.size())
    throw ColErr::MisMatch<size_t>(pVec.size(),Base.size(),
				   "mathFunc::crossSort");

  std::vector<typename std::pair<T,U> > Index(pVec.size());
  transform(pVec.begin(),pVec.end(),Base.begin(),
	    Index.begin(),mathSupport::PCombine<T,U>());
  sort(Index.begin(),Index.end());

  typename std::vector<std::pair<T,U> >::const_iterator vc;
  typename std::vector<T>::iterator ac=pVec.begin();
  typename std::vector<U>::iterator bc=Base.begin();
  for(vc=Index.begin();vc!=Index.end();vc++,ac++,bc++)
    {
      *ac = vc->first;
      *bc = vc->second;
    }
  return;
}

template<typename T>
std::vector<T>
mathFunc::filter(const std::vector<T>& YVec,const size_t N)
  /*!
    Filter the data with a centre average filter
    \param YVec :: Vector of points
    \param N :: number of points
  */
{

  const size_t NHalf(N/2);
  std::vector<T> Out(YVec.size());
  T Sum(0);

  T Cnt(0);
  for(size_t i=1;i<N/2 && i<=YVec.size();i++)
    {
      Sum+=YVec[i-1];
      Cnt++;
    }

  for(size_t i=0;i<YVec.size();i++)
    {
      if (i+NHalf<YVec.size())
	{
	  Cnt++;
	  Sum+=YVec[i+NHalf];
	}
      if (i>NHalf)
	{
	  Cnt--;
	  Sum-=YVec[i-NHalf];
	}
      Out[i]=Sum/Cnt;
    }
  return Out;
}
  
template<typename T>
T
mean(const std::vector<T>& Vec)
/*!
  Function to calculate the mean of a vector.
  \param Vec :: vector to use
  \returns \f$ \sqrt{V.V} \f$
*/
{

  T Sum(0);
  T Cnt(0);
  if (Vec.empty()) return Sum;

  for(const T& A : Vec)
    {
      Sum+= A;
      Cnt++;
    }
  return Sum/Cnt;
}

template<typename T>
void 
mathFunc::Order(T& A,T& B)
  /*!
    Function to order A,B
    \param A : set to min(A,B)
    \param B : set to max(A,B)
  */
{
  if (A>B)
    {
      T tmp=B;
      B=A;
      A=tmp;
    }
  return;
}


template<typename T>
void 
mathFunc::Swap(T& A,T& B)
  /*!
    Function to swap A,B
    \param A : set to make B
    \param B : set to make A
  */
{
  const T tmp=B;
  B=A;
  A=tmp;
  return;
}

double
mathFunc::logAdd(const double& A,const double& B)
  /*!
    Add up to value that are exp
    Equivilent of log(exp(A)+exp(B))
    \param A :: log value to add
    \param B :: log value to add
    \return log(exp(A)+exp(B))
  */
{
  return std::max(A,B) +
    std::log(1.0+exp(-std::abs(A-B)));
}

double
mathFunc::logSubtract(const double& A,const double& B)
  /*!
    Add up to value that are exp
    Equivilent of log(exp(A)+exp(B))
    \param A :: log value to add
    \param B :: log value to add
    \return log(exp(A)+exp(B))
  */
{
   if(A <= B)
     {
       throw ColErr::OrderError<double>
         (A,B,"logSubtract:B > A:");
     }
   return A+std::log(1.0+exp(-(B-A)));
 }

///\cond TEMPLATE 

namespace Geometry
{
  class Surface;
}
 

template std::complex<double> 
polFit(const double&,const unsigned int,
       std::vector<double>::const_iterator,
       std::vector<std::complex<double> >::const_iterator);

template 
double 
polFit(const double&,const unsigned int,
       std::vector<double>::const_iterator,
       std::vector<double>::const_iterator);

template 
std::complex<double> 
polInterp(const double&,const unsigned int,const std::vector<double>&,
	  const std::vector<std::complex<double> >&);

template 
std::complex<long double> 
polInterp(const double&,const unsigned int,const std::vector<double>&,
	  const std::vector<std::complex<long double> >&);

template 
double 
polInterp(const double&,const unsigned int,
	  const std::vector<double>&,
	  const std::vector<double>&);

template
float
polInterp(const double&,const unsigned int,const std::vector<double>&,
	  const std::vector<float>&);

template 
double
intQuadratic(const std::vector<double>::const_iterator&,
	     const std::vector<double>::const_iterator&);

template 
double
derivQuadratic(const std::vector<double>::const_iterator&,
	       const std::vector<double>::const_iterator&);


template class mathSupport::PIndex<double>;
template class mathSupport::PSep<double>;

template long int indexPos(const std::vector<double>&,const double&);
template long int indexPos(const std::vector<DError::doubleErr>&,
                           const DError::doubleErr&);

template void indexSort(const std::vector<double>&,std::vector<int>&);
template void indexSort(const std::vector<int>&,std::vector<int>&);
template void indexSort(const std::vector<std::string>&,std::vector<int>&);
template void indexSort(const std::vector<double>&,std::vector<size_t>&);
template void indexSort(const std::vector<int>&,std::vector<size_t>&);

//template void indexSort(const std::vector<MapSupport::Range<int>>&,
//                        std::vector<size_t>&);

template double norm(const std::vector<double>&);
template double mean(const std::vector<double>&);

//template void mathFunc::crossSort(std::vector<int>&,
//				  std::vector<RMCbox::Atom*>&);
template void mathFunc::crossSort(std::vector<int>&,std::vector<int>&);
template void mathFunc::crossSort(std::vector<int>&,std::vector<size_t>&);
template void mathFunc::crossSort(std::vector<int>&,std::vector<double>&);
template void mathFunc::crossSort(std::vector<double>&,std::vector<double>&);
template void mathFunc::crossSort(std::vector<float>&,std::vector<float>&);
template void mathFunc::crossSort(std::vector<double>&,std::vector<DError::doubleErr>&);
template void mathFunc::crossSort(std::vector<DError::doubleErr>&,
				  std::vector<DError::doubleErr>&);
 template void mathFunc::crossSort(std::vector<int>&,
				   std::vector<Geometry::Surface const*>&);

template void mathFunc::Order(int&,int&);
template void mathFunc::Order(double&,double&);
template void mathFunc::Swap(double&,double&);
template long int mathFunc::binSearch(
  const std::vector<double>::const_iterator&,
  const std::vector<double>::const_iterator&,
  const double&);

template double mathFunc::minDifference(const std::vector<double>&,
					const double&);

///\endcond TEMPLATE
