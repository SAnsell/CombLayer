/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   monteInc/AcompTools.h
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#ifndef MonteCarlo_AcompTools_h
#define MonteCarlo_AcompTools_h

namespace Geometry
{
  template<typename T> class MatrixBase;
  template<typename T> class Matrix;
}

namespace MonteCarlo 
{

/*!
  \namespace AcompTools
  \brief Holds tools for working with Acomp 
  \version 1.0
  \author S. Ansell
  \date April 2018
*/
  namespace AcompTools
    {
      struct unitsLessOrder
      {
        bool operator()(const int& A,const int& B) const
	  {
	    /*!
	      Process the sorting of units
	      \param A :: Unit to sort
	      \param B :: Unit to sort		       
	      \return A>B
	    */
	    const int aPlus=std::abs(A);
	    const int bPlus=std::abs(B);
	    if (aPlus!=bPlus) return aPlus<bPlus;
	    return (B<A);
	  }
      };
    }


}  // NAMESPACE MonteCarlo


#endif
