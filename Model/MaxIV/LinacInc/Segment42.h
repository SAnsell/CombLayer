/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   LinacInc/Segment42.h
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
#ifndef tdcSystem_Segment42_h
#define tdcSystem_Segment42_h

namespace constructSystem
{
 class Bellows;
 class VacuumPipe;
}

namespace tdcSystem
{
  class UndulatorVacuum;
  class FMUndulator;
  

  /*!
    \class Segment42
    \version 1.0
    \author S. Ansell
    \date June 2020
    \brief Dividing segment in the TDC from the linac
  */

class Segment42 :
  public TDCsegment
{
 private:

  /// init bellows
  std::shared_ptr<tdcSystem::UndulatorVacuum> uVac;

  void buildObjects(Simulation&);
  void createLinks();

  void createSplitInnerZone(Simulation&);
  void constructVoid(Simulation&,const attachSystem::FixedComp&) const;
  
 public:

  Segment42(const std::string&);
  Segment42(const Segment42&);
  Segment42& operator=(const Segment42&);
  ~Segment42() override;


  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int) override;

};

}

#endif
