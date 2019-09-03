/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   flexpesInc/FLEXPES.h
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#ifndef xraySystem_FLEXPES_h
#define xraySystem_FLEXPES_h

namespace constructSystem
{
  class SupplyPipe;
  class CrossPipe;
  class VacuumPipe;
  class Bellows;
  class LeadPipe;
  class VacuumBox;
  class portItem;
  class PortTube;
  class GateValveCube;
  class JawValveCube;
}

namespace insertSystem
{
  class insertPlate;
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
  class R1Ring;
  class flexpesFrontEnd;
  class flexpesOpticsHut;
  class flexpesOpticsBeamline;
  class ExperimentalHutch;
  class ExptBeamline;
  class OpticsBeamline;
  class ConnectZone;
  class PipeShield;
  class WallLead;
 
  /*!
    \class FLEXPES
    \version 1.0
    \author S. Ansell
    \date January 2018
    \brief General constructor for the xray system
  */

class FLEXPES :
  public xraySystem::R1Beamline
{
 private:

  std::shared_ptr<flexpesFrontEnd> frontBeam;    ///< in ring front end
  std::shared_ptr<insertSystem::insertPlate> shield;  ///< extra beam shield
  std::shared_ptr<insertSystem::insertPlate> shieldB;  ///< extra beam shield
  std::shared_ptr<WallLead> wallLead;            ///< lead in beam wall
  std::shared_ptr<flexpesOpticsHut> opticsHut;   ///< main optics hut
  /// Pipe joining frontend to optics hut
  std::shared_ptr<constructSystem::VacuumPipe> joinPipe;
  /// Main optics hutch componentsr 
  std::shared_ptr<flexpesOpticsBeamline> opticsBeam;

 public:
  
  FLEXPES(const std::string&);
  FLEXPES(const FLEXPES&);
  FLEXPES& operator=(const FLEXPES&);
  ~FLEXPES();

  /// set ring
  void setRing(std::shared_ptr<R1Ring> R) { r1Ring=R; }

  /// Accessor to stop point
  void setStopPoint(const std::string& SP)  { stopPoint=SP; }
  void build(Simulation&,const attachSystem::FixedComp&,
	     const long int);

};

}

#endif
