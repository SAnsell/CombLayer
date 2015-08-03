/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   photonInc/makePhoton.h
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
#ifndef photonSystem_makePhoton_h
#define photonSystem_makePhoton_h

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
  
  /*!
    \class makePhoton
    \version 1.0
    \author S. Ansell
    \date January 2015
    \brief General constructor for the photon system
  */

class makePhoton
{
 private:

  std::shared_ptr<CylLayer> CatcherObj;        ///< Initial moderator
  std::shared_ptr<CylContainer> OuterObj;   ///< Surround object
  std::shared_ptr<CylLayer> PrimObj;        ///< Initial moderator
  std::shared_ptr<CylLayer> CarbonObj;      ///< Pre-Mod moderator (graphite)
  std::shared_ptr<TubeMod> ModObj;          ///< Mod moderator (D2O)
  std::shared_ptr<B4CCollimator> B4CObj;    ///< B4C collimator
  std::shared_ptr<EQDetector> ND2;         ///< Detector


 public:
  
  makePhoton();
  makePhoton(const makePhoton&);
  makePhoton& operator=(const makePhoton&);
  ~makePhoton();
  
  void build(Simulation*,const mainSystem::inputParam&);

};

}

#endif
