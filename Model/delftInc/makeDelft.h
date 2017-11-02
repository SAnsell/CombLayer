/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   delftInc/makeDelft.h
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#ifndef delftSystem_makeDelft_h
#define delftSystem_makeDelft_h

namespace attachSystem
{
  class FixedComp;
  class FixedOffset;
}
/*!
  \namespace delftSystem
  \brief Delft reactor build system
  \version 1.0
  \date June 2012
  \author S. Ansell
*/

namespace delftSystem
{
  class ReactorGrid;
  class BeamInsert;
  class BeamTube;
  class BeamTubeJoiner;
  class SwimingPool;
  class virtualMod;
  class FlatModerator;
  class SphereModerator;
  class H2Moderator;
  class H2Vac;
  class H2Groove;
  class BeSurround;
  class BeCube;
  class BeFullBlock;
  class Rabbit;
  class PressureVessel;
  class SpaceBlock;

  /*!
    \class makeDelft
    \version 1.0
    \author S. Ansell
    \date June 2012
    \brief General constructor for the delft system
  */
class makeDelft
{
 private:

  std::shared_ptr<ReactorGrid> GridPlate;    ///< Grid 
  std::shared_ptr<SwimingPool> Pool;         ///< Pool

  std::shared_ptr<BeamTubeJoiner> FlightA;         ///< Direct beamline 
  std::shared_ptr<BeamTube> FlightB;         ///< Close [top]
  std::shared_ptr<BeamTube> FlightC;         ///< Close [base]
  std::shared_ptr<BeamTube> FlightD;         ///< Opposite direct
  std::shared_ptr<BeamTube> FlightE;         ///< Opposite [top]
  std::shared_ptr<BeamTube> FlightF;         ///< Opposition [base]


  std::shared_ptr<FlatModerator> ColdMod;       ///< H2 Moderator
  std::shared_ptr<PressureVessel> ColdVac;      ///< Vacuum layer
  std::shared_ptr<PressureVessel> ColdPress;    ///< Pressure vessel


  // --  Inserts for flightline  --
  std::shared_ptr<BeamInsert> R2Insert;      ///< FlightA/R2 inset

  /// Reflector additional on tube
  std::shared_ptr<attachSystem::FixedOffset> R2Be; 

  /// Boxed units
  std::vector<std::shared_ptr<SpaceBlock> > SBox;  
  /// RABBITS
  std::vector<std::shared_ptr<Rabbit> > RSet;  

  void makeBlocks(Simulation&);
  void makeRabbit(Simulation&);

  void buildCore(Simulation&,const mainSystem::inputParam&);
  void buildFlight(Simulation&,const std::string&);
  void buildModerator(Simulation&,const attachSystem::FixedComp&,
		      const long int,const std::string&,
		      const std::string&);

 public:
  
  makeDelft();
  makeDelft(const makeDelft&);
  makeDelft& operator=(const makeDelft&);
  ~makeDelft();
  
  void build(Simulation&,const mainSystem::inputParam&);
  void setSource(Simulation&,const mainSystem::inputParam&);

};

}

#endif
