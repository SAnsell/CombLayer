/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   LinacInc/Segment30.h
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
#ifndef tdcSystem_Segment30_h
#define tdcSystem_Segment30_h

namespace tdcSystem
{
  /*!
    \class Segment30
    \version 1.0
    \author K. Batkov
    \date May 2020
    \brief TDC segment 17
  */

class Segment30 :
  public TDCsegment
{
 private:

  std::shared_ptr<constructSystem::PipeTube> gauge;     ///< #2 Vacuum gauge PTR18751
  std::shared_ptr<constructSystem::VacuumPipe> pipeA;   ///< #3 VC- Flanges 304L- Tube-316L
  std::shared_ptr<constructSystem::Bellows> bellow;     ///< #5 Bellows – 304L
  std::shared_ptr<constructSystem::BlankTube> ionPump;  ///< #4 Ion pump 75l cf63 1 port
  std::shared_ptr<constructSystem::VacuumPipe> pipeB;   ///< #7 VC- Flanges 304L- Tube-316L
  std::shared_ptr<tdcSystem::CorrectorMag> cMagV;       ///< #8 Corrector magnet type D - vertical

  void buildObjects(Simulation&);
  void createLinks();

 public:

  Segment30(const std::string&);
  Segment30(const Segment30&);
  Segment30& operator=(const Segment30&);
  ~Segment30();

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
