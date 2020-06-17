/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   LinacInc/Segment1.h
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
#ifndef tdcSystem_Segment1_h
#define tdcSystem_Segment1_h

namespace constructSystem
{
  class VacuumPipe;
  class Bellows;
  class portItem;
  class BlankTube;
  class PipeTube;
}

namespace tdcSystem
{
  class LQuadF;
  class CorrectorMag;
  /*!
    \class Segment1
    \version 1.0
    \author S. Ansell
    \date April 2020
    \brief First segment in the TCD from the linac
  */

class Segment1 :
  public TDCsegment
{
 private:

  std::shared_ptr<constructSystem::VacuumPipe> pipeA;   ///< start pipe
  std::shared_ptr<constructSystem::Bellows> bellowA;    ///< first bellow
  ///< connect with two corrector magnets
  std::shared_ptr<constructSystem::VacuumPipe> pipeB;
  std::shared_ptr<tdcSystem::CorrectorMag> cMagHorrA;   ///< corrector mag
  std::shared_ptr<tdcSystem::CorrectorMag> cMagVertA;   ///< corrector mag
  /// exit pipe from corrector magnet
  std::shared_ptr<constructSystem::VacuumPipe> pipeC;
  /// pipe from  corrector block+Quad
  std::shared_ptr<constructSystem::VacuumPipe> pipeD;
  std::shared_ptr<tdcSystem::CorrectorMag> cMagHorrB;   ///< corrector mag
  std::shared_ptr<tdcSystem::CorrectorMag> cMagVertB;   ///< corrector mag
  std::shared_ptr<tdcSystem::LQuadF> QuadA;

  /// THIS is the double pipe
  std::shared_ptr<constructSystem::VacuumPipe> pipeE;

  std::shared_ptr<constructSystem::VacuumPipe> pipeF;   ///< corrector mag pipe
  std::shared_ptr<tdcSystem::CorrectorMag> cMagHorrC;   ///< corrector mag
  std::shared_ptr<tdcSystem::CorrectorMag> cMagVertC;   ///< corrector mag
  /// ion pump [rotated]
  std::shared_ptr<constructSystem::BlankTube> pumpA;   
  
  void buildObjects(Simulation&);
  void createLinks();
  
 public:
  
  Segment1(const std::string&);
  Segment1(const Segment1&);
  Segment1& operator=(const Segment1&);
  ~Segment1();

  
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
