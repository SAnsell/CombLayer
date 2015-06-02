/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/ODIN.h
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
#ifndef essSystem_ODIN_h
#define essSystem_ODIN_h

namespace constructSystem
{
  class Jaws;
}

namespace essSystem
{

  /*!
    \class ODIN
    \version 1.0
    \author S. Ansell
    \date January 2013
    \brief Main moderator system for ESS
  */
  
class ODIN
{
 private:

  std::shared_ptr<constructSystem::Jaws> CollA;

 public:
  
  ODIN();
  ODIN(const ODIN&);
  ODIN& operator=(const ODIN&);
  ~ODIN();
  
  void build(Simulation&);

};

}

#endif
