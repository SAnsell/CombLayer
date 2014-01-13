/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   epbBuildInc/makeEPB.h
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
#ifndef snsSystem_makeSNS_h
#define snsSystem_makeSNS_h

/*!
  \namespace snsSystem
  \brief General SNS stuff
  \version 1.0
  \date September 2013
  \author S. Ansell
*/

namespace snsSystem
{
  class targetOuter;
  class RefPlug;
  /*!
    \class makeSNS
    \version 1.0
    \author S. Ansell
    \date December 2013
    \brief Top level builder for the whole SNS model
  */
  
class makeSNS
{
 private:
  
  boost::shared_ptr<targetOuter> tarOuterObj;  ///< Front target section
  boost::shared_ptr<RefPlug> refObj;           ///< Reflector 

 public:
  
  makeSNS();
  makeSNS(const makeSNS&);
  makeSNS& operator=(const makeSNS&);
  ~makeSNS();
  
  void build(Simulation*,const mainSystem::inputParam&);

};

}

#endif
