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
}

namespace tdcSystem
{
  class YagScreen;
}

namespace constructSystem
{
  class SupplyPipe;
  class CrossPipe;
  class VacuumPipe;
  class Bellows;
  class VacuumBox;
  class portItem;
  class PortTube;
  class PipeTube;
  class GateValveCube;
  class JawValveCube;
  class JawFlange;
}

namespace xraySystem
{
  class CylGateValve;
  class BremColl;
  class BeamPair;
  class BremMonoColl;
  class DCMTank;
  class FlangeMount;
  class FourBeam;
  class GaugeTube;
  class Mirror;
  class MLMono;
  class MonoBlockXstals;
  class ShutterUnit;
  class SquareFMask;
  class IonGauge;
  class TriggerTube;
  class HPJaws;
  class BoxJaws;
  class BremTube;
  class ViewScreenTube;
  class MonoShutter;
  class MonoBox;
  class FourPortTube;
  class DiffPumpXIADP03;
  
    
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
  int innerMat;                         ///< inner material if used

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
  std::shared_ptr<constructSystem::VacuumPipe> pipeA;
  // diff pump
  std::shared_ptr<xraySystem::DiffPumpXIADP03> diffPump;
  
  double outerLeft;    ///< Left Width for cut rectangle
  double outerRight;   ///< Right width for cut rectangle
  double outerTop;     ///< Top lift for cut rectangle

 
  void populate(const FuncDataBase&);
  void createSurfaces();
  void buildObjects(Simulation&);
  void createLinks();
  
 public:
  
  formaxExptLine(const std::string&);
  formaxExptLine(const formaxExptLine&);
  formaxExptLine& operator=(const formaxExptLine&);
  ~formaxExptLine();

  /// Assignment to inner void
  void setInnerMat(const int M) { innerMat=M; }
  /// Assignment to extra for first volume
  void setPreInsert
    (const std::shared_ptr<attachSystem::ContainedGroup>& A) { preInsert=A; }

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
