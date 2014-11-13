/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   processInc/pairBase.h
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
#ifndef ModelSupport_pairBase_h
#define ModelSupport_pairBase_h

namespace ModelSupport
{

/*!
  \class pairBase
  \version 1.0
  \author S. Ansell
  \date September 2012
  \brief Splits a single surface : surface pair.

  This is an abstract class to allow any pair of surfaces to 
  be merged [if a specific merger routine has been written]
*/

class pairBase 
{
 private:

 public:
  
  /// Constructor
  pairBase() {}
  /// Copy constructor
  pairBase(const pairBase&) {}
  /// Assignement operator
  pairBase& operator=(const pairBase&) { return *this; }
  /// Destructor
  virtual ~pairBase() {}              

  ///\cond ABSTRACT
  virtual pairBase* clone() const =0;
  virtual int createSurface(const double,const int) =0;
  virtual std::string typeINFO() const =0;
  ///\endcond ABSTRACT
};

}

#endif
