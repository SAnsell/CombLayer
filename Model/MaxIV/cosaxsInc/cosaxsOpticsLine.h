/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   cosaxsInc/cosaxsOpticsLine.h
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#ifndef xraySystem_cosaxsOpticsLine_h
#define xraySystem_cosaxsOpticsLine_h

namespace insertSystem
{
  class insertPlate;
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
  class GateValve;
  class JawValve;
}



/*!
  \namespace xraySystem
  \brief General xray optics system
  \version 1.0
  \date January 2018
  \author S. Ansell
*/

namespace xraySystem
{
  class OpticsHutch;
  class BremColl;
  class FlangeMount;
  class Mirror;
  class MonoCrystals;
  class MonoBox;
    
  /*!
    \class cosaxsOpticsLine
    \version 1.0
    \author S. Ansell
    \date January 2018
    \brief General constructor for the xray system
  */

class cosaxsOpticsLine :
  public attachSystem::CopiedComp,
  public attachSystem::ContainedComp,
  public attachSystem::FixedOffset
{
 private:

  /// Shared point to use for last component:
  std::shared_ptr<attachSystem::FixedComp> lastComp;

  /// Inital bellow
  std::shared_ptr<constructSystem::Bellows> pipeInit;
  /// vauucm trigger system
  std::shared_ptr<constructSystem::CrossPipe> triggerPipe;
  /// first ion pump
  std::shared_ptr<constructSystem::CrossPipe> gaugeA;
  /// bellows after ion pump to filter
  std::shared_ptr<constructSystem::Bellows> bellowA;
  /// Vacuum pipe for collimator
  std::shared_ptr<xraySystem::BremColl> bremCollA;
  /// Filter tube
  std::shared_ptr<constructSystem::PortTube> filterBoxA;
  /// Filter stick [only one blade type -- fix]
  std::shared_ptr<xraySystem::FlangeMount> filterStick;
  /// First gate valve
  std::shared_ptr<constructSystem::GateValve> gateA;
  /// bellows after gateA ->view
  std::shared_ptr<constructSystem::Bellows> bellowB;
  
  /// diamon screen(?)
  std::shared_ptr<constructSystem::PipeTube> screenPipeA;
  /// View/something(?)
  std::shared_ptr<constructSystem::PipeTube> screenPipeB;
  /// Primary jaw (Box)
  std::shared_ptr<constructSystem::VacuumBox> primeJawBox;
  /// Bellow to gate on mono
  std::shared_ptr<constructSystem::Bellows> bellowC;
  /// First gate valve
  std::shared_ptr<constructSystem::GateValve> gateB;
  /// Mono box
  std::shared_ptr<xraySystem::MonoBox> monoBox;
  // ate
  std::shared_ptr<constructSystem::GateValve> gateC;
  /// Bellow to diagnositics
  std::shared_ptr<constructSystem::Bellows> bellowD;
  /// Diagnostic unit 1:
  std::shared_ptr<constructSystem::PortTube> diagBoxA;
  /// Bellow from diagnositics
  std::shared_ptr<constructSystem::Bellows> bellowE;
  // Gate fro first mirror
  std::shared_ptr<constructSystem::GateValve> gateD;

  /// Mirror box 
  std::shared_ptr<constructSystem::VacuumBox> mirrorA;
  // Gate fro first mirror
  std::shared_ptr<constructSystem::GateValve> gateE;
  /// Bellow to diagnositics
  std::shared_ptr<constructSystem::Bellows> bellowF;
  /// Diagnostic unit 1:
  std::shared_ptr<constructSystem::PortTube> diagBoxB;

  /// Diag Box B :: Jaw units
  std::array<std::shared_ptr<constructSystem::JawFlange>,2> jawComp;

  /// Bellow to mirror B
  std::shared_ptr<constructSystem::Bellows> bellowG;
  // Gate valve
  std::shared_ptr<constructSystem::GateValve> gateF;
  /// Mirror box B
  std::shared_ptr<constructSystem::VacuumBox> mirrorB;

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);
  void buildObjects(Simulation&);
  void createLinks();
  
 public:
  
  cosaxsOpticsLine(const std::string&);
  cosaxsOpticsLine(const cosaxsOpticsLine&);
  cosaxsOpticsLine& operator=(const cosaxsOpticsLine&);
  ~cosaxsOpticsLine();
  
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
