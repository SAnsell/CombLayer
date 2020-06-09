/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   LinacInc/L2SPFsegment12.h
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
#ifndef tdcSystem_L2SPFsegment12_h
#define tdcSystem_L2SPFsegment12_h

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
  class CeramicSep;
  class YagScreen;

  /*!
    \class L2SPFsegment12
    \version 1.0
    \author S. Ansell
    \date May 2020
    \brief Seventh segment
  */

class L2SPFsegment12 :
  public TDCsegment
{
 private:

  /// entrance bellows
  std::shared_ptr<constructSystem::Bellows> bellowA;

  /// first pipe
  std::shared_ptr<tdcSystem::FlatPipe> flatA;

  /// first  dipole
  std::shared_ptr<tdcSystem::DipoleDIBMag> dipoleA;

  /// first pipe
  std::shared_ptr<tdcSystem::BeamDivider> beamA;

  // LEFT DIVIDER:

  // bellow on left exit
  std::shared_ptr<constructSystem::Bellows> bellowLA;

  /// Ion pump
  std::shared_ptr<constructSystem::BlankTube> ionPumpLA;
  
  /// first pipe
  std::shared_ptr<constructSystem::VacuumPipe> pipeLA;

  // LAST Unit in left cell
  std::shared_ptr<constructSystem::Bellows> bellowLB;

  // RIGHT DIVIDER:
  
  /// first pipe
  std::shared_ptr<tdcSystem::FlatPipe> flatB;

  /// second  dipole
  std::shared_ptr<tdcSystem::DipoleDIBMag> dipoleB;

    /// second  dipole
  std::shared_ptr<constructSystem::Bellows> bellowRB;
  

  void buildObjects(Simulation&);
  void createLinks();
  
 public:
  
  L2SPFsegment12(const std::string&);
  L2SPFsegment12(const L2SPFsegment12&);
  L2SPFsegment12& operator=(const L2SPFsegment12&);
  ~L2SPFsegment12();

  using FixedComp::createAll;
  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
