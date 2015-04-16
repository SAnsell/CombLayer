/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   support/ClebschGordan.cxx
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#include "mathSupport.h"
#include "ClebschGordan.h"

namespace mathSupport
{

double
ClebschGordan::C1(const int j,const int j1,const int j2,
		  const int m1,const int m2)
   /*!
     C1 calculate
     \param j :: Total
     \param j1 :: First      
     \param j2 :: second angular
     \param m1 :: First Change
     \param m2 :: Second change
   */
{
  const int klow=std::max(std::max(0,j2-j-m1),j1-j+m2);
  const int khigh=std::min(std::min(j1+j2-j,j2+m2),j1-m1);
  
  double res(0.0);
  for(int k=klow;k<=khigh;k++)
    {
      const int sign( (k % 2) ? -1 : 1); 
      res+=sign/ 
	( factorialDB(k)*factorialDB(j1+j2-j-k)*
	  factorialDB(j1-m1-k)*factorialDB(j2+m2-k)*
	  factorialDB(j-j2+m1+k) * factorialDB(j-j1-m2+k) );
    }
  return res;
}

double
ClebschGordan::C2(const int j,const int j1,const int j2)
   /*!
     C2 calculate
     \param j :: Total
     \param j1 :: First      
     \param j2 :: second angular
   */
{
  return factorialDB(-j+j1+j2)*
    factorialDB(j-j1+j2)* factorialDB(j+j1-j2)* (1+2*j) / 
    factorialDB(1+j+j1+j2);
}


double
ClebschGordan::C3(const int j,const int j1,const int j2,
		  const int m,const int m1,const int m2) 
   /*!
     C3 calculate
     \param j :: Total
     \param j1 :: First      
     \param j2 :: second angular
   */
{
  return factorialDB(j-m)*factorialDB(j+m)*
    factorialDB(j1-m1)*factorialDB(j1+m1)*
    factorialDB(j2-m2)*factorialDB(j2+m2);
}


double
ClebschGordan::calc(const int j,const int j1,const int j2,
			  const int m1,const int m2) 
   /*!
     ClebschGordan coefficient
     \param j :: Total
     \param j1 :: First      
     \param j2 :: second angular
   */
{
  const int m=m1+m2;
  const double T=C1(j,j1,j2,m1,m2);
  const double C2value=C2(j,j1,j2);
  const double C3value=C3(j,j1,j2,m,m1,m2);
  return T*sqrt( C2value*C3value);
}



}  // NAMESPACE 


