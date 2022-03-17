/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/cosaxsInc/cosaxsTube.h
 *
 * Copyright (c) 2004-2021 by Konstantin Batkov
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
#ifndef xraySystem_cosaxsTube_h
#define xraySystem_cosaxsTube_h

class Simulation;

namespace constructSystem
{
  class FlangeDome;
  class PipeTube;
}

namespace xraySystem
{

/*!
  \class cosaxsTube
  \version 1.0
  \author Konstantin Batkov
  \date May 2019
  \brief CoSAXS Submarine
*/

  class cosaxsTubeNoseCone;
  class cosaxsTubeStartPlate;
  class cosaxsTubeWAXSDetector;
  class cosaxsTubeAirBox;
  class cosaxsTubeCable;
  class MonoBeamStop;

class cosaxsTube :
    public attachSystem::ContainedComp,
    public attachSystem::FixedRotate,
    public attachSystem::CellMap,
    public attachSystem::SurfMap,
    public attachSystem::FrontBackCut
{
 private:

  bool delayPortFlag;           ///< Delay building port
  double outerRadius;           ///< Radius of bounding volume
  double outerLength;           ///< Length of bounding volume
 
  int outerMat;                 ///< Outer material
  
  attachSystem::BlockZone buildZone;      ///< Outer BlockZone
  attachSystem::BlockZone tubeZone;       ///< Inner void BlockZone

  std::shared_ptr<xraySystem::cosaxsTubeNoseCone> noseCone;
  std::shared_ptr<constructSystem::GateValveCylinder> gateA;
  std::shared_ptr<xraySystem::cosaxsTubeStartPlate> startPlate;
  std::array<std::shared_ptr<constructSystem::PipeTube>, 8> seg;
  std::shared_ptr<constructSystem::FlangeDome> backPlate;

  // inner components:
  std::shared_ptr<xraySystem::MonoBeamStop> beamDump;
  std::shared_ptr<xraySystem::cosaxsTubeWAXSDetector> waxs;
  std::shared_ptr<xraySystem::cosaxsTubeAirBox> airBox;
  std::shared_ptr<xraySystem::cosaxsTubeCable>  cable;
  

  void createInnerObjects(Simulation&);
  
  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  cosaxsTube(const std::string&);
  cosaxsTube(const cosaxsTube&);
  cosaxsTube& operator=(const cosaxsTube&);
  virtual ~cosaxsTube();

  /// set delay
  void delayPorts() { delayPortFlag=1; }
  void createPorts(Simulation&);

  /// Assignment to outer void
  void setOuterMat(const int M) { outerMat=M; }

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,const long int);

};

}

#endif


