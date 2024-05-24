/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   supportInc/polySupport.h
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
#ifndef polySupport_h
#define polySupport_h

template<typename T>
T 
polyEval(const std::vector<T>&,const T);
  
/// Calculate the value of a quadratic at x
template<typename T>
T quad(const T,const T,const T,const T);

template<typename T>
size_t 
solveRealQuadratic(const T&,const T&,const T&,T&,T&);

template<typename T>
size_t 
solveRealQuadratic(const std::vector<T>&,T&,T&);

template<typename T>
size_t 
solveQuadratic(const T*,std::pair<std::complex<T>,
	       std::complex<T> >&);

template<typename T>
size_t 
solveQuadratic(const T&,const T&,const T&,
	       std::pair<std::complex<T>,
	       std::complex<T> >&);

template<typename T>
size_t 
solveQuadratic(const T&,const T&,const T&,
	       std::complex<T>&,std::complex<T>&);

template<typename T>
size_t 
solveQuadratic(const std::vector<T>&,std::pair<std::complex<T>,
	       std::complex<T> >&);

/// Solve a Cubic equation

template<typename T>
size_t 
solveCubic(const std::vector<T>&,std::complex<T>&,
	   std::complex<T>&,std::complex<T>&);

template<typename T>
size_t 
solveCubic(const T*,std::complex<T>&,
	   std::complex<T>&,std::complex<T>&);

template<typename T>
size_t 
solveCubic(const T,const T,const T,
	   const T,std::complex<T>&,
	   std::complex<T>&,std::complex<T>&);

template<typename T>
size_t 
solveRealCubic(const T,const T,const T,
	       const T,T&,T&,T&);


#endif

