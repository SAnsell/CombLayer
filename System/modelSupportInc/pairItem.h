/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   processInc/pairItem.h
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
#ifndef ModelSupport_pairItem_h
#define ModelSupport_pairItem_h

namespace ModelSupport
{

/*!
  \class pairItem
  \version 1.0
  \author S. Ansell
  \date September 2012
  \brief Splits a single surface : surface pair.

  This is a holding class to allow any pair of surfaces to 
  be merged [if a specific merger routine has been written]
  \tparam T :: First surface type
  \tparam U :: Second surface type
  \tparam V :: Out surface type
*/

template<typename T,typename U>
class pairItem : public pairBase
{
 private:

  const T* surfA;      ///< Outer surface
  const U* surfB;      ///< Inner surface
  T* sOut;             ///< Split surface [control?]

  static const T* findSurfA(const int);
  static const U* findSurfB(const int);

 public:

  
  pairItem<T,U>(const T*,const U*);
  pairItem<T,U>(const int,const int);
  pairItem<T,U>(const pairItem<T,U>&);
  pairItem<T,U>& operator=(const pairItem<T,U>&);
  virtual pairItem<T,U>* clone() const;
  virtual ~pairItem() {}   ///< Simple destructor

  virtual int createSurface(const double,const int);
  virtual std::string typeINFO() const;
  
    
};

}

#endif
