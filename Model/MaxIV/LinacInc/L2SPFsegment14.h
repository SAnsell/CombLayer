/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   LinacInc/L2SPFsegment14.h
 *
 * Copyright (c) 2004-2020 by Stuart Ansell / Konstantin Batkov
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
#ifndef tdcSystem_L2SPFsegment14_h
#define tdcSystem_L2SPFsegment14_h

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
  class LQuad;
  class CorrectorMag;
  /*!
    \class L2SPFsegment14
    \version 1.0
    \author S. Ansell
    \date January 2018
    \brief General constructor for the xray system
  */

class L2SPFsegment14 :
  public attachSystem::FixedOffset,
  public attachSystem::ContainedComp,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap
{
 private:

  /// System for building a divided inner
  attachSystem::InnerZone buildZone;

  double outerLeft;         ///< left for inner void
  double outerRight;        ///< right for inner void
  double outerHeight;       ///< lift from origin for inner void


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
  std::shared_ptr<tdcSystem::LQuad> QuadA;

  /// THIS is the double pipe
  std::shared_ptr<constructSystem::VacuumPipe> pipeE;

  std::shared_ptr<constructSystem::VacuumPipe> pipeF;   ///< corrector mag pipe
  std::shared_ptr<tdcSystem::CorrectorMag> cMagHorrC;   ///< corrector mag
  std::shared_ptr<tdcSystem::CorrectorMag> cMagVertC;   ///< corrector mag
  /// ion pump [rotated]
  std::shared_ptr<constructSystem::BlankTube> pumpA;

  void populate(const FuncDataBase&);
  void createSurfaces();
  void buildObjects(Simulation&);
  void createLinks();

 public:

  L2SPFsegment14(const std::string&);
  L2SPFsegment14(const L2SPFsegment14&);
  L2SPFsegment14& operator=(const L2SPFsegment14&);
  ~L2SPFsegment14();


  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
