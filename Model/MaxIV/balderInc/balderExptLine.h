/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   balderInc/balderExptLine.h
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#ifndef xraySystem_balderExptLine_h
#define xraySystem_balderExptLine_h

namespace insertSystem
{
  class insertPlate;
}

namespace constructSystem
{
  class SupplyPipe;
  class CrossPipe;
  class VacuumPipe;
  class Bellows;
  class VacuumBox;
  class portItem;
  class PortTube;
  class GateValveCube;
  class JawValveCube;
}



/*!
  \namespace xraySystem
  \brief General xray optics system
  \version 1.0
  \date January 2018
  \author S. Ansell
*/

namespace xraySystem
{
  class ExptHutch;
  class FlangeMount;
  class Mirror;
  class PipeShield;
    
  /*!
    \class balderExptLine
    \version 1.0
    \author S. Ansell
    \date January 2018
    \brief General constructor for the xray system
  */

class balderExptLine :
  public attachSystem::CopiedComp,
  public attachSystem::ContainedComp,
  public attachSystem::FixedOffset,
  public attachSystem::CellMap
{
 private:

  double sampleYStep;        ///< Distance for sample to move
  double sampleRadius;       ///< Spherical sample radius

  double beamStopYStep;          ///< Distance for beam to step
  double beamStopRadius;     ///< beamstop radius
  double beamStopThick;      ///< beamstop thick

  int sampleMat;             ///< Sample material
  int beamStopMat;           ///< beamStop Mat

  
  void populate(const FuncDataBase&);
  void createSurfaces();
  void buildObjects(Simulation&);
  void createLinks();
  
 public:
  
  balderExptLine(const std::string&);
  balderExptLine(const balderExptLine&);
  balderExptLine& operator=(const balderExptLine&);
  ~balderExptLine();
  
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
