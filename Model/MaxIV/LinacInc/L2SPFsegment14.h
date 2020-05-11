/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   LinacInc/L2SPFsegment14.h
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
#ifndef tdcSystem_L2SPFsegment14_h
#define tdcSystem_L2SPFsegment14_h

namespace constructSystem
{
  class GateValveCube;
}

namespace tdcSystem
{
  class DipoleDIBMag;
  /*!
    \class L2SPFsegment14
    \version 1.0
    \author S. Ansell
    \date January 2018
    \brief General constructor for the xray system
  */

class L2SPFsegment14 :
  public TDCsegment
{
 private:

  std::shared_ptr<constructSystem::Bellows> bellowA;   ///< First bellow
  std::shared_ptr<constructSystem::VacuumPipe> pipeA;  ///< Pipe through dm1
  std::shared_ptr<tdcSystem::DipoleDIBMag> dm1;        ///< First dipole magnet
  std::shared_ptr<constructSystem::VacuumPipe> pipeB;  ///< Pipe between first two dipole magnets
  std::shared_ptr<constructSystem::VacuumPipe> pipeC;  ///< Pipe through dm2
  std::shared_ptr<tdcSystem::DipoleDIBMag> dm2;        ///< Second dipole magnet
  std::shared_ptr<constructSystem::GateValveCube> gateA;
  std::shared_ptr<constructSystem::Bellows> bellowB;   ///< Last bellow

  void buildObjects(Simulation&);
  void createLinks();

 public:

  L2SPFsegment14(const std::string&);
  L2SPFsegment14(const L2SPFsegment14&);
  L2SPFsegment14& operator=(const L2SPFsegment14&);
  ~L2SPFsegment14();

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif