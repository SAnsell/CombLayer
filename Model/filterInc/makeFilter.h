/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   photonInc/makeFilter.h
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
#ifndef photonSystem_makeFilter_h
#define photonSystem_makeFilter_h

namespace constructSystem
{
  class SupplyPipe;
}

/*!
  \namespace photonSystem
  \brief General Laser moderator system
  \version 1.0
  \date January 2015
  \author S. Ansell
*/

namespace photonSystem
{
  class CylLayer;
  class CylContainer;
  class TubeMod;
  class B4CCollimator;
  class EQDetector;
}

namespace filterSystem
{
  /*!
    \class makeFilter
    \version 1.0
    \author S. Ansell
    \date January 2015
    \brief General constructor for the photon system
  */
  
class makeFilter
{
 private:

  std::shared_ptr<photonSystem::CylLayer> SiPrimary;
  std::shared_ptr<photonSystem::CylLayer> SiSecond;
  std::shared_ptr<photonSystem::CylLayer> Lead;
  std::shared_ptr<photonSystem::CylLayer> SiThird;    
    

 public:
  
  makeFilter();
  makeFilter(const makeFilter&);
  makeFilter& operator=(const makeFilter&);
  ~makeFilter();
  
  void build(Simulation&,const mainSystem::inputParam&);

};

}

#endif
