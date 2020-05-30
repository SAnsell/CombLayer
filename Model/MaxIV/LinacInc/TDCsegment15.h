/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   LinacInc/TDCsegment15.h
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
#ifndef tdcSystem_TDCsegment15_h
#define tdcSystem_TDCsegment15_h

namespace tdcSystem
{
  class YagScreen;
  /*!
    \class TDCsegment15
    \version 1.0
    \author K. Batkov
    \date May 2020
    \brief TDC segment 15
  */

class TDCsegment15 :
  public TDCsegment
{
 private:

  std::shared_ptr<constructSystem::VacuumPipe> pipeA;       ///< #1 - VC
  std::shared_ptr<constructSystem::PipeTube> mirrorChamber; ///< #2 Mirror chamber
  std::shared_ptr<constructSystem::BlankTube> ionPump;       ///< #4 Ion pump 75l cf63
  std::shared_ptr<tdcSystem::YagScreen> yagScreen;          ///< #3 Yag screen
  std::shared_ptr<constructSystem::VacuumPipe> pipeB;       ///< #5 - VC

  void buildObjects(Simulation&);
  void createLinks();

 public:

  TDCsegment15(const std::string&);
  TDCsegment15(const TDCsegment15&);
  TDCsegment15& operator=(const TDCsegment15&);
  ~TDCsegment15();

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
