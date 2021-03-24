/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   formaxInc/formaxExptLine.h
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
 * along with this program. If not, see <http://www.gnu.org/licenses/>. 
 *
 ****************************************************************************/
#ifndef xraySystem_formaxExptLine_h
#define xraySystem_formaxExptLine_h

namespace insertSystem
{
  class insertPlate;
  class insertSphere;
}

namespace tdcSystem
{
  class SixPortTube;
  class YagScreen;
}

namespace constructSystem
{
  class VacuumPipe;
  class Bellows;
  class portItem;
  class PortTube;
  class PipeTube;
  class GateValveCylinder;
}

namespace xraySystem
{
  class CylGateValve;
  class MonoBox;
  class FourPortTube;
  class BoxJaws;
  class ViewScreenTube;
  class CLRTube;
  class ConnectorTube;
  class formaxDetectorTube;

  
    
  /*!
    \class formaxExptLine
    \version 1.0
    \author S. Ansell
    \date January 2019
    \brief Constructor for the formax optics components
  */

class formaxExptLine :
  public attachSystem::CopiedComp,
  public attachSystem::ContainedComp,
  public attachSystem::FixedRotate,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap
{
 private:

  /// string for pre-insertion into mastercell:0
  std::shared_ptr<attachSystem::ContainedGroup> preInsert;
  /// construction space for main object
  attachSystem::BlockZone buildZone;
  int outerMat;                         ///< outermaterial if used

  /// Shared point to use for last component:
  std::shared_ptr<attachSystem::FixedComp> lastComp;

  /// bellow to collimator
  std::shared_ptr<constructSystem::Bellows> bellowA;
  /// filterbox
  std::shared_ptr<xraySystem::MonoBox> filterBoxA;
  /// bellow to collimator
  std::shared_ptr<constructSystem::Bellows> bellowB;
  /// cross way
  std::shared_ptr<xraySystem::FourPortTube> crossA;
  /// bellow to collimator
  std::shared_ptr<constructSystem::Bellows> bellowC;
  /// jaws
  std::shared_ptr<xraySystem::BoxJaws> jawBox;
  /// Pipe from jaws
  std::shared_ptr<xraySystem::ConnectorTube> connectA;
  // diff pump
  std::shared_ptr<xraySystem::CLRTube> clrTubeA;
  /// Pipe from diff
  std::shared_ptr<xraySystem::ConnectorTube> connectB;
  /// Pipe to gauge system [segment 12]
  std::shared_ptr<constructSystem::VacuumPipe> pipeA;
  /// Six port gauget etc port
  std::shared_ptr<tdcSystem::SixPortTube> sixPortA;
  /// Cylinder gate valve [with square top]
  std::shared_ptr<constructSystem::GateValveCylinder> cylGateA;
  /// Pipe from gauge system 
  std::shared_ptr<constructSystem::VacuumPipe> pipeB;
  /// bellow to collimator
  std::shared_ptr<constructSystem::Bellows> bellowD;  
  /// Six port gauget etc port
  std::shared_ptr<tdcSystem::SixPortTube> sixPortB;
  /// Pipe from gauge system 
  std::shared_ptr<constructSystem::VacuumPipe> pipeC;
  /// Cylinder gate valve [with square top]
  std::shared_ptr<constructSystem::GateValveCylinder> cylGateB;
  /// Six port gauget etc port
  std::shared_ptr<tdcSystem::SixPortTube> sixPortC;
  /// Short Pipe from gauge system 
  std::shared_ptr<constructSystem::VacuumPipe> pipeD;
  // Segment 13 :
  /// Pipe from gate value 
  std::shared_ptr<xraySystem::ConnectorTube> connectC;
  // second CLR tube
  std::shared_ptr<xraySystem::CLRTube> clrTubeB;
  /// Pipe from diff
  std::shared_ptr<xraySystem::ConnectorTube> connectD;
  /// view screen
  std::shared_ptr<constructSystem::PipeTube> viewTube;
  /// bellow to collimator
  std::shared_ptr<constructSystem::Bellows> bellowE;
  /// cross way
  std::shared_ptr<xraySystem::FourPortTube> crossB;
  /// Adjustable pipe
  std::shared_ptr<constructSystem::VacuumPipe> adjustPipe;
  /// End pipe on adjustable
  std::shared_ptr<constructSystem::VacuumPipe> pipeE;
  /// sample jaws
  std::shared_ptr<xraySystem::BoxJaws> jawBoxB;
  /// Pipe to short nose
  std::shared_ptr<xraySystem::ConnectorTube> connectE;
  /// Narrow end pipe
  std::shared_ptr<constructSystem::VacuumPipe> endPipe;
  /// Narrow end pipe
  std::shared_ptr<insertSystem::insertSphere> sample;

  double outerLeft;    ///< Left Width for cut rectangle
  double outerRight;   ///< Right width for cut rectangle
  double outerTop;     ///< Top lift for cut rectangle

  void constructViewScreen(Simulation&,
			   const attachSystem::FixedComp&,
			   const std::string&); 
  
  void populate(const FuncDataBase&);
  void createSurfaces();
  void buildObjects(Simulation&);
  void createLinks();
  
 public:
  
  formaxExptLine(const std::string&);
  formaxExptLine(const formaxExptLine&);
  formaxExptLine& operator=(const formaxExptLine&);
  ~formaxExptLine();

  /// Assignment to outer void
  void setOuterMat(const int M) { outerMat=M; }
  /// Assignment to extra for first volume
  void setPreInsert
    (const std::shared_ptr<attachSystem::ContainedGroup>& A) { preInsert=A; }

  void insertSample(Simulation&,const int) const;
  
  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
