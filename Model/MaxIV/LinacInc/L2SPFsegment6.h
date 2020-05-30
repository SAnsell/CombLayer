/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   LinacInc/L2SPFsegment6.h
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
#ifndef tdcSystem_L2SPFsegment6_h
#define tdcSystem_L2SPFsegment6_h

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
  class Scrapper;
  class EBeamStop;
  
  /*!
    \class L2SPFsegment6
    \version 1.0
    \author S. Ansell
    \date April 2020
    \brief Second segment in the TDC from the linac
  */

class L2SPFsegment6 :
  public TDCsegment
{
 private:

  /// first pipe
  std::shared_ptr<constructSystem::VacuumPipe> pipeA;   
  /// seonc pipe
  std::shared_ptr<constructSystem::VacuumPipe> pipeB;   
  /// third pipe (falange change)
  std::shared_ptr<constructSystem::VacuumPipe> pipeC;   

  /// beam scrapper
  std::shared_ptr<tdcSystem::Scrapper> scrapper;   

  /// fourth pipe (flange change)
  std::shared_ptr<constructSystem::VacuumPipe> pipeD;   

  /// long bellow
  std::shared_ptr<constructSystem::Bellows> bellowA;   

  /// Electron beam stop
  std::shared_ptr<tdcSystem::EBeamStop> beamStop;   
  
  /// long bellow
  std::shared_ptr<constructSystem::Bellows> bellowB;   

  void buildObjects(Simulation&);
  void createLinks();
  
 public:
  
  L2SPFsegment6(const std::string&);
  L2SPFsegment6(const L2SPFsegment6&);
  L2SPFsegment6& operator=(const L2SPFsegment6&);
  ~L2SPFsegment6();


  using FixedComp::createAll;
  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
