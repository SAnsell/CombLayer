/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   LinacInc/Segment10.h
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
#ifndef tdcSystem_Segment10_h
#define tdcSystem_Segment10_h

namespace constructSystem
{
  class VacuumPipe;
  class Bellows;
  class portItem;
  class BlankTube;
  class PipeTube;
  class GateValveCube;
}


namespace tdcSystem
{
  class LQuadF;
  class CorrectorMag;
  class InjectionHall;

  /*!
    \class Segment10
    \version 1.0
    \author S. Ansell
    \date May 2020
    \brief Seventh segment
  */

class Segment10 :
  public TDCsegment
{
 private:

  const InjectionHall* IHall;      ///< Storage for injection hall if used.
  
  /// connection pipe
  std::shared_ptr<constructSystem::VacuumPipe> pipeA;   

  /// first bellow [actually ceramic gap]
  std::shared_ptr<constructSystem::Bellows> bellowA;   

  /// Gate valve holder
  std::shared_ptr<constructSystem::GateValveCube> gateValve;   
  
  /// ion pump [rotated]
  std::shared_ptr<constructSystem::BlankTube> pumpA;   

  /// connection pipe
  std::shared_ptr<constructSystem::VacuumPipe> pipeB;   

  /// first bellow [actually ceramic gap]
  std::shared_ptr<constructSystem::Bellows> bellowB;   

  /// quad pipe
  std::shared_ptr<constructSystem::VacuumPipe> pipeC;   

  /// Quad
  std::shared_ptr<tdcSystem::LQuadF> QuadA;

  std::shared_ptr<tdcSystem::CorrectorMag> cMagVertA;   ///< corrector mag


  double wallRadius;       ///< Hole radius in wall

  void constructHole(Simulation&);
  
  void populate(const FuncDataBase&);
  void createSurfaces();
  void buildObjects(Simulation&);
  void createLinks();
  
 public:
  
  Segment10(const std::string&);
  Segment10(const Segment10&);
  Segment10& operator=(const Segment10&);
  ~Segment10();

  using FixedComp::createAll;
  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
			 const long int);

};

}

#endif
