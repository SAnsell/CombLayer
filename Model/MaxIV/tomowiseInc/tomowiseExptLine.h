/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   tomowiseInc/tomowiseExptLine.h
 *
 * Copyright (c) 2004-2025 by Konstantin Batkov
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
#ifndef xraySystem_tomowiseExptLine_h
#define xraySystem_tomowiseExptLine_h

namespace insertSystem
{
  class insertPlate;
  class insertSphere;
}

namespace constructSystem
{
  class VacuumPipe;
  class Bellows;
  class portItem;
  class PortTube;
  class PipeTube;
  class GateValveCylinder;
  class FlangePlate;
}

namespace xraySystem
{
  class CylGateValve;
  class MonoBox;
  class BoxJaws;
  class ViewScreenTube;
  class CRLTube;
  class ConnectorTube;
  class RoundMonoShutter;




  /*!
    \class tomowiseExptLine
    \version 1.0
    \author S. Ansell
    \date January 2019
    \brief Constructor for the tomowise optics components
  */

class tomowiseExptLine :
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

  std::string exptType;        ///< experiment type [bypass/diffract/sample]

  /// Shared point to use for last component:
  std::shared_ptr<attachSystem::FixedComp> lastComp;

  /// first gate valve
  std::shared_ptr<xraySystem::CylGateValve> gateTubeA;
  /// bellow to collimator
  std::shared_ptr<constructSystem::Bellows> bellowA;
  /// view screen tube
  std::shared_ptr<xraySystem::ViewScreenTube> viewTube;
  /// view screen
  std::shared_ptr<xraySystem::CooledScreen> cooledScreen;
  /// connector to collimator
  std::shared_ptr<constructSystem::VacuumPipe> pipeA;
  /// jaws
  std::shared_ptr<xraySystem::HPJaws> hpJaws;
  /// connector from collimator
  std::shared_ptr<constructSystem::VacuumPipe> pipeB;
  /// second gate valve
  std::shared_ptr<xraySystem::CylGateValve> gateTubeB;
  /// connector to CRL
  std::shared_ptr<constructSystem::VacuumPipe> pipeC;
  /// Addaptor to CRL tube
  std::shared_ptr<constructSystem::VacuumPipe> crlPipeA;
  /// First CRL System
  std::shared_ptr<xraySystem::CRLTube> crlTubeA;
  /// Mid  CRL pipe
  std::shared_ptr<constructSystem::VacuumPipe> crlPipeB;
  /// Second CRL System
  std::shared_ptr<xraySystem::CRLTube> crlTubeB;
  /// End  CRL pipe [note skip C as not in expt design]
  std::shared_ptr<constructSystem::VacuumPipe> crlPipeD;
  /// Narrow end pipe
  std::shared_ptr<constructSystem::VacuumPipe> endPipe;
  /// Mirror box
  std::shared_ptr<constructSystem::VacuumBox> mirrorBoxA;
  /// Mirror front
  std::shared_ptr<xraySystem::Mirror> mirrorFrontA;
  /// Mirror back
  std::shared_ptr<xraySystem::Mirror> mirrorBackA;

  /// The main mono shutter
  std::shared_ptr<xraySystem::RoundMonoShutter> monoShutterB;

  //DIFFRACT:
  /// Tube to diffraction
  std::shared_ptr<constructSystem::VacuumPipe> diffractTube;

  //BYPASS:
  /// Tube to diffraction
  std::shared_ptr<constructSystem::VacuumPipe> byPassTube;

  /// SAMPLE
  /// End point on mirror box
  std::shared_ptr<constructSystem::FlangePlate> endWindow;
  /// Tube
  std::shared_ptr<constructSystem::VacuumPipe> sampleTube;
  /// Narrow end pipe
  std::shared_ptr<insertSystem::insertSphere> sample;



  double outerLeft;    ///< Left Width for cut rectangle
  double outerRight;   ///< Right width for cut rectangle
  double outerTop;     ///< Top lift for cut rectangle

  void constructCRL(Simulation&,
		    const attachSystem::FixedComp&,
		    const std::string&);
  void constructSampleStage(Simulation&,
			    const attachSystem::FixedComp&,
			    const std::string&);
  void constructDiffractionStage(Simulation&,
				 const attachSystem::FixedComp&,
				 const std::string&);
  void constructByPassStage(Simulation&);


  void populate(const FuncDataBase&) override;
  void createSurfaces();
  void buildObjects(Simulation&);
  void createLinks();

 public:

  tomowiseExptLine(const std::string&);
  tomowiseExptLine(const tomowiseExptLine&);
  tomowiseExptLine& operator=(const tomowiseExptLine&);
  ~tomowiseExptLine() override;

  /// Assignment to outer void
  void setOuterMat(const int M) { outerMat=M; }
  /// Assignment to extra for first volume
  void setPreInsert
    (const std::shared_ptr<attachSystem::ContainedGroup>& A) { preInsert=A; }

  void insertSample(Simulation&,const int) const;

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int) override;

};

}

#endif
