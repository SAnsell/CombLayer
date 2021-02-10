/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   LinacInc/Segment34.h
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
#ifndef tdcSystem_Segment34_h
#define tdcSystem_Segment34_h

namespace tdcSystem
{
  class DipoleDIBMag;
  /*!
    \class Segment34
    \version 1.0
    \author K. Batkov
    \date June 2020
    \brief TDC segment 34 (copy of 32)

    Ideally we just use segment32 but this need specific debug stuff.
  */

class Segment34 :
  public TDCsegment
{
 private:

  std::shared_ptr<tdcSystem::FlatPipe> flatA;      ///< Pipe through dmA
  std::shared_ptr<tdcSystem::DipoleDIBMag> dipoleA;  ///< First dipole magnet

  /// Pipe between first two dipole magnets
  std::shared_ptr<constructSystem::VacuumPipe> pipeA;

  std::shared_ptr<tdcSystem::FlatPipe> flatB;     ///< Pipe through dmB
  std::shared_ptr<tdcSystem::DipoleDIBMag> dipoleB;   ///< Second dipole magnet

  std::shared_ptr<constructSystem::Bellows> bellow; ///< Bellow

  void buildObjects(Simulation&);
  void createLinks();

 public:

  Segment34(const std::string&);
  Segment34(const Segment34&);
  Segment34& operator=(const Segment34&);
  ~Segment34();

  void writePoints() const override;

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int) override;

};

}

#endif
