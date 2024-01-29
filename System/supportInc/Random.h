/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   supportInc/Random.h
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

#ifndef Random_h
#define Random_h

/*!
  \namespace Random
  \brief Wrapper for C++ random functions
  \author Stuart Ansell
  \version 1.0
  \date August 2023

  Produces uniform distribution between 0 and 1 
*/

namespace Random
{
  
  static std::mt19937 gen(17823);
  static std::uniform_real_distribution<double> RNG(0.0,1.0);
  static std::normal_distribution<double> RNN(0.0,1.0);

  inline double rand() { return RNG(gen); }
  inline double randNorm(const double mean,const double sigma)
    { return sigma*RNN(gen)-mean; }
}

#endif
