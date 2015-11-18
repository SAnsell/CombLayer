/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   support/splineSupport.cxx
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
#include <iostream>
#include <fstream>
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
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "splineSupport.h"

/*! 
  \file splineSupport.cxx 
*/


namespace mathSupport
{
  
template<typename T>
void
calcSpline(const std::vector<T>& X,
	   const std::vector<T>& Y, 
	   std::vector<SplinePt<T>>& Out) 
  /*!
    Compute a spline, requires X/Y to be equal 
    and at least 2 points
    \param X :: X input list [sorted]
    \param Y :: Y input list 
    \param Out [output] 
   */
{
  ELog::RegMethod RegA("splineSupport[F]","calcSpline");
  
  if (X.size()<2)
    throw ColErr::SizeError<size_t>(X.size(),2,"X.size");
  if (Y.size()!=X.size())
    throw ColErr::MisMatch<size_t>(X.size(),Y.size(),"X/Y.size");
  
  const size_t N(X.size()-1);
  std::vector<T> w(N);
  std::vector<T> h(N);
  std::vector<T> ftt(N+1);
  
  for (size_t i=0;i<N;i++)
    {
      w[i] = (X[i+1]-X[i]);
      h[i] = (Y[i+1]-Y[i])/w[i];
    }
  
  ftt[0]=0;
  for (size_t i=0;i<N-1; i++)
    ftt[i+1] = 3*(h[i+1]-h[i])/(w[i+1]+w[i]);
  ftt[N]=0;
  
  for(size_t i=0;i<N;i++)
    {
      SplinePt<T> Val;
      Val.A = (ftt[i+1]-ftt[i])/(6.0*w[i]);
      Val.B = ftt[i]/2;
      Val.C = h[i]-w[i]*(ftt[i+1]+2*ftt[i])/6.0;
      Val.D = Y[i];
      Out.push_back(Val);
    }
  return;
}

template<typename T>
T
evalSpline(const std::vector<T>& XP,
	   const std::vector<SplinePt<T>>& SPvec,
	   const T& X)
  /*!
    Evaluate a spline point at a given X value
    \param XP :: X coordinates	 				       
    \param SPvec :: Vector of spline values [A-D]
    \param X :: X coordinate to calculate
    \return spline value
   */
{
  size_t index(0);
  if (X>XP.front())
    {
      typename std::vector<T>::const_iterator vc=
	std::lower_bound(XP.begin(),XP.end(),X);
      index=(vc-XP.begin());
      index--;
    }
  const T t=X-XP[index];
  const SplinePt<T>& SP(SPvec[index]);
  return SP.A*t*t*t+SP.B*t*t+SP.C*t+SP.D;
}




///\cond TEMPLATE

template double evalSpline(const std::vector<double>&,
			   const std::vector<SplinePt<double>>&,
			   const double&);
template void calcSpline(const std::vector<double>&,
			 const std::vector<double>&, 
			 std::vector<SplinePt<double>>&);

///\endcond TEMPLATE

} // NAMESPACE mathSupport
