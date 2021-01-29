/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   bnctBuildInc/makeBNCT.h
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#ifndef bnctSystem_makeBNCT_h
#define bnctSystem_makeBNCT_h

/*!
  \namespace bnctSystem
  \brief General BNCT stuff
  \version 1.0
  \date September 2015
  \author S. Ansell
*/

namespace bnctSystem
{
  class DiscTarget;
  /*!
    \class makeBNCT
    \version 1.0
    \author S. Ansell
    \date December 2015
    \brief Top level builder for the whole BNCT model
  */
  
class makeBNCT
{
 private:

  std::shared_ptr<DiscTarget> targetObj;    ///< target object
  
 public:
  
  makeBNCT();
  makeBNCT(const makeBNCT&);
  makeBNCT& operator=(const makeBNCT&);
  ~makeBNCT();
  
  void build(Simulation*,const mainSystem::inputParam&);

};

}

#endif
