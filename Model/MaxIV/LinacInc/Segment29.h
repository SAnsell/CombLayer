/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   LinacInc/Segment29.h
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
#ifndef tdcSystem_Segment29_h
#define tdcSystem_Segment29_h

namespace constructSystem
{
  class Bellows;
  class VacuumPipe;
  class VoidUnit;
}

namespace tdcSystem
{
  class FlatPipe;
  class TriPipe;
  class SixPortTube;
  class MultiPipe;
  class DipoleDIBMag;
  class YagUnit;
  class YagScreen;
  class LBeamStop;
  class NBeamStop;
  class BeamWing;
  class LowBeamBox;
  class BeamBox;
  

  /*!
    \class Segment29
    \version 1.0
    \author S. Ansell
    \date June 2020
    \brief Dividing segment in the TDC from the linac
  */

class Segment29 :
  public TDCsegment
{
 private:

  std::unique_ptr<attachSystem::BlockZone> IZTop;        ///< Upper inner zone
  std::unique_ptr<attachSystem::BlockZone> IZMid;        ///< Flat inner zone

  /// Join pipe 
  std::shared_ptr<constructSystem::VacuumPipe> pipeAA;
  std::shared_ptr<constructSystem::VacuumPipe> pipeBA;

  /// init bellows
  std::shared_ptr<constructSystem::Bellows> bellowAA;
  std::shared_ptr<constructSystem::Bellows> bellowBA;

  /// Join pipe 
  std::shared_ptr<tdcSystem::YagUnit> yagUnitA;
  std::shared_ptr<tdcSystem::YagUnit> yagUnitB;

  /// exit bellows
  std::shared_ptr<tdcSystem::YagScreen> yagScreenA;
  std::shared_ptr<tdcSystem::YagScreen> yagScreenB;

  // End void for nice flat terminator 
  std::shared_ptr<constructSystem::VoidUnit> endVoid;
  
  /// beam-stops
  std::shared_ptr<tdcSystem::NBeamStop> beamStopA;
  std::shared_ptr<tdcSystem::NBeamStop> beamStopB;

  /// Extra wings [if required]
  std::shared_ptr<tdcSystem::BeamWing> beamWingA;
  std::shared_ptr<tdcSystem::BeamWing> beamWingB;

  /// Boxes round beamstops
  std::shared_ptr<tdcSystem::BeamBox> beamBoxA;
  std::shared_ptr<tdcSystem::LowBeamBox> beamBoxB;
  
  void buildObjects(Simulation&);
  void createLinks();

  void createSplitInnerZone();
  
 public:

  Segment29(const std::string&);
  Segment29(const Segment29&);
  Segment29& operator=(const Segment29&);
  ~Segment29();

  using FixedComp::createAll;
  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
