/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   LinacInc/L2SPFsegment10.h
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
#ifndef tdcSystem_L2SPFsegment10_h
#define tdcSystem_L2SPFsegment10_h

namespace constructSystem
{
  class VacuumPipe;
  class Bellows;
  class portItem;
  class BlankTube;
  class PipeTube;
}

/*!
  \namespace xraySystem
  \brief General xray optics system
  \version 1.0
  \date January 2018
  \author S. Ansell
*/

namespace tdcSystem
{
  class LQuadF;
  class CorrectorMag;
  class InjectionHall;

  /*!
    \class L2SPFsegment10
    \version 1.0
    \author S. Ansell
    \date May 2020
    \brief Seventh segment
  */

class L2SPFsegment10 :
  public TDCsegment
{
 private:

  const InjectionHall* IHall;      ///< Storage for injection hall if used.
  
  /// connection pipe
  std::shared_ptr<constructSystem::VacuumPipe> pipeA;   

  /// first bellow [actually ceramic gap]
  std::shared_ptr<constructSystem::Bellows> bellowA;   

  /// Gate valve holder
  std::shared_ptr<xraySystem::CylGateValve> gateValve;   
  
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
  
  L2SPFsegment10(const std::string&);
  L2SPFsegment10(const L2SPFsegment10&);
  L2SPFsegment10& operator=(const L2SPFsegment10&);
  ~L2SPFsegment10();

  using FixedComp::createAll;
  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
			 const long int);

};

}

#endif
