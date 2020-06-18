/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   LinacInc/Segment13.h
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
#ifndef tdcSystem_Segment13_h
#define tdcSystem_Segment13_h

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
  class BPM;
  class LQuadF;
  class LSexupole;
  class CorrectorMag;
  class YagUnit;
  class YagScreen;

  /*!
    \class Segment13
    \version 1.0
    \author S. Ansell
    \date June 2020
    \brief Last L2SPF segment
  */

class Segment13 :
  public TDCsegment
{
 private:

  // first pipe 
  std::shared_ptr<constructSystem::VacuumPipe> pipeA;   

  /// corrector mag
  std::shared_ptr<tdcSystem::CorrectorMag> cMagHorA;

  /// BPM
  std::shared_ptr<tdcSystem::BPM> bpm;    

  // main magnetic pipe
  std::shared_ptr<constructSystem::VacuumPipe> pipeB;   
  
  /// Quad begining QSQ 
  std::shared_ptr<tdcSystem::LQuadF> QuadA;

  /// sexupole
  std::shared_ptr<tdcSystem::LSexupole> SexuA; 

  /// Quad endng QSQ 
  std::shared_ptr<tdcSystem::LQuadF> QuadB;
  
  /// yag station [verical]
  std::shared_ptr<tdcSystem::YagUnit> yagUnit;

  /// yag screen
  std::shared_ptr<tdcSystem::YagScreen> yagScreen;

  /// exit pipe + corrector mag
  std::shared_ptr<constructSystem::VacuumPipe> pipeC;   

  /// corrector mag
  std::shared_ptr<tdcSystem::CorrectorMag> cMagVerC; 

  void buildObjects(Simulation&);
  void createLinks();
  
 public:
  
  Segment13(const std::string&);
  Segment13(const Segment13&);
  Segment13& operator=(const Segment13&);
  ~Segment13();

  using FixedComp::createAll;
  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
