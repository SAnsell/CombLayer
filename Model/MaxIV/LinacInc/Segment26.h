/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   LinacInc/Segment26.h
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
#ifndef tdcSystem_Segment26_h
#define tdcSystem_Segment26_h

namespace constructSystem
{
 class Bellows;
 class VacuumPipe;
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
  class LocalShielding;

  /*!
    \class Segment26
    \version 1.1
    \author S. Ansell
    \date Feb 2021
    \brief Dividing segment in the TDC from the linac
  */

class Segment26 :
  public TDCsegment
{
 private:

  std::unique_ptr<attachSystem::BlockZone> IZTop;        ///< Flat inner zone
  std::unique_ptr<attachSystem::BlockZone> IZMid;        ///< Mid inner zone
  std::unique_ptr<attachSystem::BlockZone> IZLower;      ///< Lower inner zone

  /// Join pipe
  std::shared_ptr<constructSystem::VacuumPipe> pipeAA;
  std::shared_ptr<constructSystem::VacuumPipe> pipeBA;
  std::shared_ptr<constructSystem::VacuumPipe> pipeCA;

  std::shared_ptr<tdcSystem::LocalShielding> shieldA; // local shielding

  /// Connect bellows
  std::shared_ptr<constructSystem::Bellows> bellowAA;
  std::shared_ptr<constructSystem::Bellows> bellowBA;
  std::shared_ptr<constructSystem::Bellows> bellowCA;


  /// Double yag screen
  std::shared_ptr<tdcSystem::YagUnit> yagUnitA;
  std::shared_ptr<tdcSystem::YagUnit> yagUnitB;

  std::shared_ptr<tdcSystem::YagScreen> yagScreenA;
  std::shared_ptr<tdcSystem::YagScreen> yagScreenB;

  /// Exit pipe
  std::shared_ptr<constructSystem::VacuumPipe> pipeAB;
  std::shared_ptr<constructSystem::VacuumPipe> pipeBB;
  std::shared_ptr<constructSystem::VacuumPipe> pipeCB;


  void buildObjects(Simulation&);
  void buildFrontSpacer(Simulation&);
  void createLinks();

  void createSplitInnerZone();

 public:

  Segment26(const std::string&);
  Segment26(const Segment26&);
  Segment26& operator=(const Segment26&);
  ~Segment26();

  virtual void insertPrevSegment(Simulation&,const TDCsegment*) const;

  using FixedComp::createAll;
  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
