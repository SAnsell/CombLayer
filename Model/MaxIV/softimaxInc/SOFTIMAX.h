/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
  * File:   softimaxInc/SOFTIMAX.h
  *
  * Copyright (c) 2004-2019 by Konstantin Batkov
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
#ifndef xraySystem_SOFTIMAX_h
#define xraySystem_SOFTIMAX_h

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
  class JawFlange;
}



/*!
  \namespace xraySystem
  \brief General xray optics system
  \version 1.0
  \date September 2019
  \author K. Batkov
*/

namespace xraySystem
{
  class balderOpticsHutch;
  class ExperimentalHutch;
  class softimaxFrontEnd;
  class FrontEndCave;
  class ConnectZone;
  class PipeShield;
  
  /*!
    \class SOFTIMAX
    \version 1.0
    \author S. Ansell
    \date January 2018
    \brief General constructor for the xray system
  */

  class SOFTIMAX : public R3Beamline
  {
  private:
  /// the components in the front end
  std::shared_ptr<softimaxFrontEnd> frontBeam;

  public:
  
    SOFTIMAX(const std::string&);
    SOFTIMAX(const SOFTIMAX&);
    SOFTIMAX& operator=(const SOFTIMAX&);
    virtual ~SOFTIMAX();

    virtual void build(Simulation&,const attachSystem::FixedComp&,
		       const long int);

  };

}

#endif
