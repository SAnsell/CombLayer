/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   cuBlockInc/makeCu.h
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#ifndef cuSystem_makeCu_h
#define cuSystem_makeCu_h


/*!
  \namespace cuSystem
  \brief General Cu block model
  \version 1.0
  \date December 2012
  \author S. Ansell
*/

namespace cuSystem
{
  class CuCollet;

  /*!
    \class makeCu
    \version 1.0
    \author S. Ansell
    \date December 2012
    \brief General constructor for cu object
  */

class makeCu
{
 private:
  
  std::shared_ptr<CuCollet> Target;   // target object
  
 public:
  
  makeCu();
  makeCu(const makeCu&);
  makeCu& operator=(const makeCu&);
  ~makeCu();
  
  void build(Simulation*,const mainSystem::inputParam&);

};

}

#endif
