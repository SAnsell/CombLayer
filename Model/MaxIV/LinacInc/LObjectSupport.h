/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   LinacInc/LObjectSupport.h
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#ifndef constructSystem_LObjectSupport_h
#define constructSystem_LObjectSupport_h


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
  class GateValveCylinder;
  class JawValveCube;
  class JawFlange;

}

namespace tdcSystem
{
  class CorrectorMag;
  
int
pipeTerminate(Simulation&,
	      attachSystem::InnerZone& buildZone,
	      const std::shared_ptr<constructSystem::VacuumPipe>&);


int
correctorMagnetPair(Simulation&,
		    attachSystem::InnerZone& buildZone,
		    const std::shared_ptr<constructSystem::VacuumPipe>&,
		    const std::shared_ptr<tdcSystem::CorrectorMag>&,
		    const std::shared_ptr<tdcSystem::CorrectorMag>&);

template<typename magTYPE>
int
pipeMagUnit(Simulation&,
	    attachSystem::InnerZone& buildZone,
	    const std::shared_ptr<constructSystem::VacuumPipe>&,
	    const std::string&,
	    const std::shared_ptr<magTYPE>&);

}

#endif
