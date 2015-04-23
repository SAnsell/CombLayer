/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   ts3ModelInc/makeTS3.h
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#ifndef ts3System_makeTS3_h
#define ts3System_makeTS3_h

namespace constructSystem
{
  class CylMod;
  class WallCut;
}

namespace ts3System
{
  /*!
    \class makeTS3
    \version 1.0
    \author S. Ansell
    \date March 2015
    \brief main builder for TS3 model
  */

class makeTS3
{
 private:

 
  std::shared_ptr<constructSystem::CylMod> CentObj;   ///< centre object
  std::shared_ptr<constructSystem::WallCut> WCut;   ///< centre object

 public:
  
  makeTS3();
  makeTS3(const makeTS3&);
  makeTS3& operator=(const makeTS3&);
  ~makeTS3();
  
  void build(Simulation&,const mainSystem::inputParam&);
  
};

}

#endif
