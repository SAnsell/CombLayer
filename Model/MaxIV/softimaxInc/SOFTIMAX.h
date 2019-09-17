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
  class VacuumPipe;
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
  class softimaxFrontEnd;
  class balderOpticsHutch;
  
  /*!
    \class SOFTIMAX
    \version 1.0
    \author K. Batkov
    \date September 2019
    \brief SoftiMAX beam line
  */

  class SOFTIMAX : public R3Beamline
  {
  private:
    /// the components in the front end
    std::shared_ptr<xraySystem::softimaxFrontEnd> frontBeam;
    /// lead in beam wall
    std::shared_ptr<WallLead> wallLead;
    /// Optics hutch
    std::shared_ptr<balderOpticsHutch> opticsHut;
    /// Pipe joining frontend to optics hut
    std::shared_ptr<constructSystem::VacuumPipe> joinPipe;

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
