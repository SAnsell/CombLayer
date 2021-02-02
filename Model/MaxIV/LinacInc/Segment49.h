/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   LinacInc/Segment49.h
 *
 * Copyright (c) 2004-2020 by Konstantin Batkov
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
#ifndef tdcSystem_Segment49_h
#define tdcSystem_Segment49_h

namespace tdcSystem
{
  class LocalShielding;

  /*!
    \class Segment49
    \version 1.0
    \author K. Batkov
    \date July 2020
    \brief SPF segment 49
  */

class Segment49 :
  public TDCsegment
{
 private:

  const InjectionHall* IHall; ///< Storage for injection hall if used

  std::shared_ptr<xraySystem::CylGateValve> gateA;    // #1
  std::shared_ptr<constructSystem::VacuumPipe> pipeA; // #2
  std::shared_ptr<tdcSystem::LocalShielding> shieldA; // local shielding
  std::shared_ptr<constructSystem::VacuumPipe> pipeB; // #3
  std::shared_ptr<xraySystem::CylGateValve> gateB;    // #4

  double wallRadius; // radius of the IHall penetration

  void constructHole(Simulation&);

  void populate(const FuncDataBase&);
  void createSurfaces();
  void buildObjects(Simulation&);
  void createLinks();

 public:

  Segment49(const std::string&);
  Segment49(const Segment49&);
  Segment49& operator=(const Segment49&);
  ~Segment49();

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
