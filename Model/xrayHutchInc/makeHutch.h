/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   xrayHutchInc/makeHutch.h
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
#ifndef xrayHutSystem_makeHutch_h
#define xrayHutSystem_makeHutch_h


/*!
  \namespace xrayHutSystem
  \brief simple xray source in container
  \version 1.0
  \date May 2022
  \author S. Ansell
*/

namespace xrayHutSystem
{
  class xrayHutch;
  class xrayTarget;
  /*!
    \class makeHutch
    \version 1.0
    \author S. Ansell
    \date May 2022
    \brief General manufacturing class
  */

class makeHutch
{
 private:


  std::shared_ptr<xrayHutch> hut;       ///< main hutch
  std::shared_ptr<xrayTarget> target;   ///< target

 public:
  
  makeHutch();
  makeHutch(const makeHutch&);
  makeHutch& operator=(const makeHutch&);
  ~makeHutch();
  
  void build(Simulation&,const mainSystem::inputParam&);

};

}

#endif
