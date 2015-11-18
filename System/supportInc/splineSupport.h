/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   supportInc/splineSupport.h
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
#ifndef splineSupport_h
#define splineSupport_h

namespace mathSupport
{

/*!
  \struct SplinePt
  \version 1.0
  \author S. Ansell
  \brief Very simple quad component support for spline
  \date November 2015
*/


template<typename T>
struct SplinePt
{
  T A;      ///< t^3 coefficient
  T B;      ///< t^2 coefficient
  T C;      ///< t coefficient
  T D;      ///< const coefficient
};

template<typename T>
void calcSpline(const std::vector<T>&,const std::vector<T>&,
		typename std::vector<SplinePt<T>>&);

template<typename T>
T evalSpline(const std::vector<T>&,
	     const typename std::vector<SplinePt<T> >&,
	     const T&);

  
}


#endif

