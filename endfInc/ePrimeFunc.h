/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   endfInc/ePrimeFunc.h
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
#ifndef ENDF_ePrimeFunc_h
#define ENDF_ePrimeFunc_h

namespace ENDF 
{

/*!
  \class ePrimeFunc
  \brief Allows integration of the ENDFmaterial ds/dOde
  \version 1.0
  \author S. Ansell
  \date June 2010
*/

class ePrimeFunc
{
  private:

  const double E;              ///< Energy
  const double mu;             ///< Mu for calculation
  const ENDFmaterial& A;       ///< Material object 

 public:

  /// Constructor
  ePrimeFunc(const ENDFmaterial& EM,const double energy,
	     const double M) : E(energy),mu(M),A(EM) {}

  // Output functional
  double operator()(const double& Eprime)
    {
      return A.dSdOdE(E,Eprime,mu);
    }
};

} // NAMESPACE ENDF

#endif
