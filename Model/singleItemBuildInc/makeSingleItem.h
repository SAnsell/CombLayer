/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   singleItemInc/makeSingleItem.h
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#ifndef singleItemSystem_makeSINGLEITEM_h
#define singleItemSystem_makeSINGLEITEM_h

/*!
  \namespace singleItemSystem
  \brief General singleItem stuff
  \version 1.0
  \date March 2016
  \author S. Ansell
*/

namespace singleItemSystem
{
  
class makeSingleItem
{
 private:
  
 public:
  
  makeSingleItem();
  makeSingleItem(const makeSingleItem&);
  makeSingleItem& operator=(const makeSingleItem&);
  ~makeSingleItem();
  
  void build(Simulation&,const mainSystem::inputParam&);

};

}

#endif
