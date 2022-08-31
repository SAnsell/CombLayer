/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   supportInc/MapSupport.h
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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
#ifndef MapSupport_h
#define MapSupport_h

/*!
  \namespace MapSupport
  \brief Holds stuff for manipulating maps
  \author S. Ansell
  \date 2/9/05
  \version 1.0

  Allows maps to be created with cloned objects
*/

namespace MapSupport
{

  template<typename TA,typename TB> 
  TB findDefVal(const std::map<TA,TB>&,const TA&,const TB&);

  template<typename T> 
  bool iterateBinMap(std::map<T,int>&,const int,const int);
  
  /*!
    \class valEqual
    \brief Functor using second value as equal
    \author S. Ansell
    \date 2/9/05
    \version 1.0

    Functor allows searching a map for second value
    equal.
  */
  template<typename KeyPart,typename NumPart>
  class valEqual
    {
      private:
      
      const NumPart value;   ///< Value to check against map
   
      public:

      /// Set Value to check
      valEqual(const NumPart& V) :
        value(V)
	{ }

      /// Equality operator vs Map second object
      bool
      operator()(const std::pair<KeyPart,NumPart>& A) const
	{
	  return A.second==value;
	}
    };
  
} // NAMESPACE MapSupport


#endif
