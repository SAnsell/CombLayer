/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   LinacInc/Segment5.h
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ****************************************************************************/
#ifndef tdcSystem_Segment5_h
#define tdcSystem_Segment5_h


namespace tdcSystem
{
  class BeamDivider;

  /*!
    \class Segment5
    \version 1.0
    \author S. Ansell
    \date April 2020
    \brief Second segment in the TDC from the linac
  */

class Segment5 :
  public TDCsegment
{
 private:

  /// first pipe
  std::shared_ptr<tdcSystem::FlatPipe> flatA;

  /// first  dipole
  std::shared_ptr<tdcSystem::DipoleDIBMag> dipoleA;

  /// first pipe
  std::shared_ptr<tdcSystem::BeamDivider> beamA;

 /// first pipe
  std::shared_ptr<tdcSystem::FlatPipe> flatB;

  /// second  dipole
  std::shared_ptr<tdcSystem::DipoleDIBMag> dipoleB;

  /// exit bellows
  std::shared_ptr<constructSystem::Bellows> bellowA;

  void buildObjects(Simulation&);
  void createLinks();

 public:

  Segment5(const std::string&);
  Segment5(const Segment5&);
  Segment5& operator=(const Segment5&);
  ~Segment5();


  using FixedComp::createAll;
  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
