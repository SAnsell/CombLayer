/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/formaxInc/formaxDetectorTube.h
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#ifndef xraySystem_formaxDetectorTube_h
#define xraySystem_formaxDetectorTube_h

class Simulation;

namespace constructSystem
{
  class PipeTube;
  class portItem;
  class FlangeDome;
}

namespace xraySystem
{

  class AreaDetector;
  class MonoBeamStop;

/*!
  \class formaxDetectorTube
  \version 1.0
  \author Stuart Ansell
  \date March 2021
  \brief Formax Submarine
*/

class formaxDetectorTube :
    public attachSystem::ContainedComp,
    public attachSystem::FixedRotate,
    public attachSystem::CellMap,
    public attachSystem::SurfMap,
    public attachSystem::ExternalCut
{
 private:

  /// internal blockzone for detctor tube componnets
  attachSystem::BlockZone buildZone;

  /// Shared point to use for last component:
  std::shared_ptr<attachSystem::FixedComp> lastComp;

  
  double outerRadius; ///< Radius of bounding volume
  double outerLength; ///< Length of bounding volume
  int outerMat;       ///< Surround matieral
  
  /// main tube vacuum segments 
  std::array<std::shared_ptr<constructSystem::PipeTube>,8> mainTube;
  std::shared_ptr<constructSystem::FlangeDome> frontDome;
  std::shared_ptr<constructSystem::FlangeDome> backDome;

  std::shared_ptr<xraySystem::MonoBeamStop> monoBeamStop;
  std::shared_ptr<xraySystem::AreaDetector> waxs;

  
  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  formaxDetectorTube(const std::string&);
  formaxDetectorTube(const formaxDetectorTube&);
  formaxDetectorTube& operator=(const formaxDetectorTube&);
  virtual ~formaxDetectorTube();

  /// set delay
  void createPorts(Simulation&);

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif


