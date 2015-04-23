/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   processInc/pairFactory.h
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
#ifndef ModelSupport_pairFactory_h
#define ModelSupport_pairFactory_h

namespace ModelSupport
{

  class pairBase;

/*!
  \class pairFactory
  \version 1.0
  \author S. Ansell
  \date October 2012
  \brief Splits a single surface : surface pair.
*/

class pairFactory 
{
 private:

  template<typename T,typename U>
  static pairBase* getPair(const T*,const Geometry::Surface*);

  template<typename T>
  static pairBase* getFirstItem(const Geometry::Surface*,
				const Geometry::Surface*);

 public:
  
  static pairBase* 
    createPair(const Geometry::Surface*,
	       const Geometry::Surface*);
};


}

#endif
