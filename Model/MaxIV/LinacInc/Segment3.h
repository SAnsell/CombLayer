/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   LinacInc/Segment3.h
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
#ifndef tdcSystem_Segment3_h
#define tdcSystem_Segment3_h

namespace constructSystem
{
  class OffsetFlangePipe;
  class VacuumPipe;
  class Bellows;
  class portItem;
  class BlankTube;
  class PipeTube;
}

namespace xraySystem
{
  class CylGateValve;
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
  class CorrectorMag;
  class DipoleDIBMag;
  class FlatPipe;
  
  /*!
    \class Segment3
    \version 1.0
    \author S. Ansell
    \date April 2020
    \brief Second segment in the TDC from the linac
  */

class Segment3 :
  public TDCsegment
{
 private:

  /// Bellows
  std::shared_ptr<constructSystem::Bellows> bellowA;

  /// first pipe in dipole
  std::shared_ptr<tdcSystem::FlatPipe> flatA;   

  /// first  dipole
  std::shared_ptr<tdcSystem::DipoleDIBMag> dipoleA;   

  /// first pipe in dipole
  std::shared_ptr<constructSystem::OffsetFlangePipe> pipeA;   

  std::shared_ptr<tdcSystem::CorrectorMag> cMagHorA;    ///< corrector mag
  std::shared_ptr<tdcSystem::CorrectorMag> cMagVertA;   ///< corrector mag

  /// second pipe in dipole
  std::shared_ptr<tdcSystem::FlatPipe> flatB;   
  /// second  dipole
  std::shared_ptr<tdcSystem::DipoleDIBMag> dipoleB;   

  /// Final bellow [including bend]
  std::shared_ptr<constructSystem::Bellows> bellowB;

  void buildObjects(Simulation&);
  void createLinks();
  
 public:
  
  Segment3(const std::string&);
  Segment3(const Segment3&);
  Segment3& operator=(const Segment3&);
  ~Segment3();

  using FixedComp::createAll;
  virtual void createAll(Simulation&,
			 const attachSystem::FixedComp&,
			 const long int);

};

}

#endif
