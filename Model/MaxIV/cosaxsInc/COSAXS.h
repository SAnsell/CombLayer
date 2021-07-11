/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   cosaxsInc/COSAXS.h
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
#ifndef xraySystem_COSAXS_h
#define xraySystem_COSAXS_h

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
  class cosaxsFrontEnd;
  class cosaxsOpticsLine;
  class cosaxsExptLine;

  /*!
    \class COSAXS
    \version 1.0
    \author S. Ansell
    \date January 2018
    \brief General constructor for the xray system
  */

class COSAXS : public R3Beamline
{
 private:

  /// the components in the front end
  std::shared_ptr<cosaxsFrontEnd> frontBeam;

  /// lead in beam wall
  std::shared_ptr<WallLead> wallLead;

  /// Pipe joining frontend to optics hut
  std::shared_ptr<constructSystem::VacuumPipe> joinPipe;

  /// Optics hutch
  std::shared_ptr<OpticsHutch> opticsHut;
  /// Optics beamlines
  std::shared_ptr<cosaxsOpticsLine> opticsBeam;

  /// Pipe joining frontend to optics hut
  std::shared_ptr<constructSystem::VacuumPipe> joinPipeB;
  /// Screening shield
  std::shared_ptr<xraySystem::PipeShield> screenA;

  /// Main experimental hutch
  std::shared_ptr<ExperimentalHutch> exptHut;

  /// Experimental beamline
  std::shared_ptr<cosaxsExptLine> exptBeam;

 public:

  COSAXS(const std::string&);
  COSAXS(const COSAXS&);
  COSAXS& operator=(const COSAXS&);
  virtual ~COSAXS();

  virtual void build(Simulation&,const attachSystem::FixedComp&,
	     const long int);

};

}

#endif
