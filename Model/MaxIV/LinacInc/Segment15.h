/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   LinacInc/Segment15.h
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
#ifndef tdcSystem_Segment15_h
#define tdcSystem_Segment15_h

namespace tdcSystem
{
  class CrossWayTube;
  class CrossWayBlank;
  class YagScreen;
  class LocalShielding;

  /*!
    \class Segment15
    \version 1.1
    \author K. Batkov
    \date Feb 2021
    \brief TDC segment 15
  */

class Segment15 :
  public TDCsegment
{
 private:

  std::shared_ptr<constructSystem::VacuumPipe> pipeA;       ///< #1 - VC
  std::shared_ptr<tdcSystem::LocalShielding> shieldA; // local shielding
  std::shared_ptr<tdcSystem::CrossWayBlank> mirrorChamber;       ///< #2
  std::shared_ptr<tdcSystem::YagUnit> yagUnit;          ///<  Yag unit
  std::shared_ptr<tdcSystem::YagScreen> yagScreen;          ///< #3 Yag screen
  std::shared_ptr<constructSystem::VacuumPipe> pipeB;       ///< #5 - VC

  void buildObjects(Simulation&);
  void createLinks();

 public:

  Segment15(const std::string&);
  Segment15(const Segment15&);
  Segment15& operator=(const Segment15&);
  ~Segment15();

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
