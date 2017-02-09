/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   photonInc/makePhoton3.h
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
#ifndef photonSystem_makePhoton3_h
#define photonSystem_makePhoton3_h

namespace constructSystem
{
  class SupplyPipe;
  class insertPlate;
  class insertSphere;
  class RingFlange;
  class RingSeal;
      
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
  class He3Tubes;
  class TableSupport;
  class EQDetector;
  class ModContainer;
  class VacuumVessel;
  class TubeCollimator;
   
  /*!
    \class makePhoton3
    \version 1.0
    \author S. Ansell
    \date June 2016
    \brief General constructor for the photon system
  */

class makePhoton3
{
 private:

  std::shared_ptr<VacuumVessel> Chamber;        ///< Vacuum vesse
  std::shared_ptr<ModContainer> ModContObj;     ///< Container
  std::shared_ptr<CylLayer> ModObj;             ///< Moderator
  std::shared_ptr<TableSupport> BaseSupport;    ///< Support
  std::shared_ptr<He3Tubes> leftTubes;          ///< Detectors
  std::shared_ptr<He3Tubes> rightTubes;         ///< Detectors
  std::shared_ptr<TubeCollimator> leftColl;     ///< collimator

  void buildWings(Simulation&);
  
 public:
  
  makePhoton3();
  makePhoton3(const makePhoton3&);
  makePhoton3& operator=(const makePhoton3&);
  ~makePhoton3();
  
  void build(Simulation&,const mainSystem::inputParam&);

};

}

#endif
