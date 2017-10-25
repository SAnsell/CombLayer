/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   supportInc/polySupport.h
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
#ifndef polySupport_h
#define polySupport_h


/// Calculate the value of a quadratic at x
double quad(const double,const double,const double,const double);

size_t 
solveQuadratic(const double*,std::pair<std::complex<double>,
	       std::complex<double> >&);

size_t 
solveQuadratic(const double&,const double&,const double&,
	       std::pair<std::complex<double>,
	       std::complex<double> >&);

size_t 
solveQuadratic(const double&,const double&,const double&,
	       std::complex<double>&,std::complex<double>&);

size_t 
solveQuadratic(const std::vector<double>&,std::pair<std::complex<double>,
	       std::complex<double> >&);

/// Solve a Cubic equation

size_t 
solveCubic(const std::vector<double>&,std::complex<double>&,
	   std::complex<double>&,std::complex<double>&);

size_t 
solveCubic(const double*,std::complex<double>&,
	   std::complex<double>&,std::complex<double>&);

size_t 
solveCubic(const double,const double,const double,
	   const double,std::complex<double>&,
	   std::complex<double>&,std::complex<double>&);



#endif

