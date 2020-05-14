/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   LinacInc/L2SPFsegment3.h
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
#ifndef tdcSystem_L2SPFsegment3_h
#define tdcSystem_L2SPFsegment3_h

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
  class FlatPipe;
  
  /*!
    \class L2SPFsegment3
    \version 1.0
    \author S. Ansell
    \date April 2020
    \brief Second segment in the TDC from the linac
  */

class L2SPFsegment3 :
  public TDCsegment
{
 private:

  /// Bellows
  std::shared_ptr<constructSystem::Bellows> bellowA;

  // first pipe in dipole
  std::shared_ptr<tdcSystem::FlatPipe> flatA;   


  void buildObjects(Simulation&);
  void createLinks();
  
 public:
  
  L2SPFsegment3(const std::string&);
  L2SPFsegment3(const L2SPFsegment3&);
  L2SPFsegment3& operator=(const L2SPFsegment3&);
  ~L2SPFsegment3();

  
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
