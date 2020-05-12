/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   LinacInc/L2SPFsegment15.h
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
#ifndef tdcSystem_L2SPFsegment15_h
#define tdcSystem_L2SPFsegment15_h

namespace tdcSystem
{
  class YagScreen;
  /*!
    \class L2SPFsegment15
    \version 1.0
    \author S. Ansell
    \date January 2018
    \brief General constructor for the xray system
  */

class L2SPFsegment15 :
  public TDCsegment
{
 private:

  std::shared_ptr<constructSystem::VacuumPipe> pipeA;       ///< #1 - VC
  std::shared_ptr<constructSystem::PipeTube> mirrorChamber; ///< #2 Mirror chamber
  std::shared_ptr<constructSystem::PipeTube> ionPump;       ///< #4 Ion pump 75l cf63
  std::shared_ptr<tdcSystem::YagScreen> yagScreen;          ///< #3 Yag screen
  std::shared_ptr<constructSystem::VacuumPipe> pipeB;       ///< #5 - VC

  void buildObjects(Simulation&);
  void createLinks();

 public:

  L2SPFsegment15(const std::string&);
  L2SPFsegment15(const L2SPFsegment15&);
  L2SPFsegment15& operator=(const L2SPFsegment15&);
  ~L2SPFsegment15();

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
