/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   LinacInc/L2SPFsegment11.h
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
#ifndef tdcSystem_L2SPFsegment11_h
#define tdcSystem_L2SPFsegment11_h

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
    \class L2SPFsegment11
    \version 1.0
    \author S. Ansell
    \date May 2020
    \brief Seventh segment
  */

class L2SPFsegment11 :
  public TDCsegment
{
 private:

  /// first bellow 
  std::shared_ptr<constructSystem::Bellows> bellowA;   

  /// Beam postion monitor
  std::shared_ptr<tdcSystem::BPM> bpm;
  /// first pipe
  std::shared_ptr<constructSystem::VacuumPipe> pipeA;   

  /// Quad 
  std::shared_ptr<tdcSystem::LQuadF> QuadA;

  /// ion pump [rotated]
  std::shared_ptr<tdcSystem::YagUnit> yagUnit;
  /// Yag screen for pump tube
  std::shared_ptr<tdcSystem::YagScreen> yagScreen;
  
  /// exit pipe
  std::shared_ptr<constructSystem::VacuumPipe> pipeB;   

  /// corrector mag
  std::shared_ptr<tdcSystem::CorrectorMag> cMagHorA;  

  void buildObjects(Simulation&);
  void createLinks();
  
 public:
  
  L2SPFsegment11(const std::string&);
  L2SPFsegment11(const L2SPFsegment11&);
  L2SPFsegment11& operator=(const L2SPFsegment11&);
  ~L2SPFsegment11();

  using FixedComp::createAll;
  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
