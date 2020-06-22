/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   LinacInc/Segment27.h
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
#ifndef tdcSystem_Segment27_h
#define tdcSystem_Segment27_h

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
  

  /*!
    \class Segment27
    \version 1.0
    \author S. Ansell
    \date June 2020
    \brief Dividing segment in the TDC from the linac
  */

class Segment27 :
  public TDCsegment
{
 private:

  std::unique_ptr<attachSystem::InnerZone> IZTop;        ///< Upper inner zone
  std::unique_ptr<attachSystem::InnerZone> IZFlat;       ///< Flat inner zone
  std::unique_ptr<attachSystem::InnerZone> IZLower;      ///< Lower inner zone
  
  /// init bellows
  std::shared_ptr<constructSystem::Bellows> bellowAA;
  std::shared_ptr<constructSystem::Bellows> bellowBA;
  std::shared_ptr<constructSystem::Bellows> bellowCA;

  /// Join pipe 
  std::shared_ptr<constructSystem::VacuumPipe> pipeAA;
  std::shared_ptr<constructSystem::VacuumPipe> pipeBA;
  std::shared_ptr<constructSystem::VacuumPipe> pipeCA;

  /// connect bellows
  std::shared_ptr<constructSystem::Bellows> bellowAB;
  std::shared_ptr<constructSystem::Bellows> bellowBB;
  std::shared_ptr<constructSystem::Bellows> bellowCB;

  /// Double yag screen
  std::shared_ptr<tdcSystem::YagUnit> yagUnitA;
  std::shared_ptr<tdcSystem::YagUnit> yagUnitB;
  std::shared_ptr<tdcSystem::YagUnit> yagUnitC;

  std::shared_ptr<tdcSystem::YagScreen> yagScreenA;
  std::shared_ptr<tdcSystem::YagScreen> yagScreenB;
  std::shared_ptr<tdcSystem::YagScreen> yagScreenC;

  /// exit bellows
  std::shared_ptr<constructSystem::Bellows> bellowAC;
  std::shared_ptr<constructSystem::Bellows> bellowBC;
  std::shared_ptr<constructSystem::Bellows> bellowCC;

  void buildObjects(Simulation&);
  void createLinks();

  void createSplitInnerZone(Simulation&);
  
 public:

  Segment27(const std::string&);
  Segment27(const Segment27&);
  Segment27& operator=(const Segment27&);
  ~Segment27();


  using FixedComp::createAll;
  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
