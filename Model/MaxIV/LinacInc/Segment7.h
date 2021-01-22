/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   LinacInc/Segment7.h
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
#ifndef tdcSystem_Segment7_h
#define tdcSystem_Segment7_h

namespace constructSystem
{
  class VacuumPipe;
  class Bellows;
  class portItem;
  class BlankTube;
  class PipeTube;
}

namespace xraySystem
{
  class CorrectorMag;
}

namespace tdcSystem
{
  class LQuadF;
  class StriplineBPM;

  /*!
    \class Segment7
    \version 1.0
    \author S. Ansell
    \date May 2020
    \brief Seventh segment
  */

class Segment7 :
  public TDCsegment
{
 private:

  /// first pipe
  std::shared_ptr<constructSystem::VacuumPipe> pipeA;   

  /// horizontal corrector mag
  std::shared_ptr<xraySystem::CorrectorMag> cMagHA;   
  /// first quad
  std::shared_ptr<tdcSystem::LQuadF> QuadA;
  /// bpm
  std::shared_ptr<tdcSystem::StriplineBPM> bpm;

  /// second pipe
  std::shared_ptr<constructSystem::VacuumPipe> pipeB;   

  /// vertical corrector mag
  std::shared_ptr<xraySystem::CorrectorMag> cMagVA;   
  
  void buildObjects(Simulation&);
  void createLinks();
  
 public:
  
  Segment7(const std::string&);
  Segment7(const Segment7&);
  Segment7& operator=(const Segment7&);
  ~Segment7();


  using FixedComp::createAll;
  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
