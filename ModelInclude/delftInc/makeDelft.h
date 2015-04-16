/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   delftInc/makeDelft.h
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
  class SwimingPool;
  class virtualMod;
  class SphereModerator;
  class H2Moderator;
  class H2Vac;
  class H2Groove;
  class BeSurround;
  class BeCube;
  class BeFullBlock;
  class Rabbit;
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

  int vacReq;                            ///< Vac vessel required

  std::shared_ptr<ReactorGrid> GridPlate;    ///< Grid 
  std::shared_ptr<SwimingPool> Pool;         ///< Pool
  std::shared_ptr<BeamTube> FlightA;         ///< Direct beamline 
  std::shared_ptr<BeamTube> FlightB;         ///< Close [top]
  std::shared_ptr<BeamTube> FlightC;         ///< Close [base]
  std::shared_ptr<BeamTube> FlightD;         ///< Opposite direct
  std::shared_ptr<BeamTube> FlightE;         ///< Opposite [top]
  std::shared_ptr<BeamTube> FlightF;         ///< Opposition [base]
  std::shared_ptr<virtualMod> ColdMod;      ///< H2 Moderator
  /// H2 Moderator groove
  std::vector<std::shared_ptr<H2Groove> > ColdGroove;      
  /// H2 Moderator surround
  std::shared_ptr<H2Vac> CSurround;  

  // --  Inserts for flightline  --
  std::shared_ptr<BeamInsert> R2Insert;      ///< FlightA/R2 inset

  // Reflector additional
  std::shared_ptr<BeSurround> R2Be;      ///< R2 tube refle
  std::shared_ptr<BeCube> R2Cube;      ///< R2 tube refle
  std::shared_ptr<BeFullBlock> RFull;      ///< R Full block
  std::shared_ptr<BeFullBlock> LFull;      ///< L Full block


  /// Boxed units
  std::vector<std::shared_ptr<SpaceBlock> > SBox;  
  /// RABBITS
  std::vector<std::shared_ptr<Rabbit> > RSet;  

  static virtualMod* createColdMod(const std::string&);

  void variableObjects(const FuncDataBase&);
  void makeBlocks(Simulation&);
  void makeRabbit(Simulation&);

 public:
  
  makeDelft(const std::string&);
  makeDelft(const makeDelft&);
  makeDelft& operator=(const makeDelft&);
  ~makeDelft();
  
  void build(Simulation*,const mainSystem::inputParam&);
  void setSource(Simulation*,const mainSystem::inputParam&);

};

}

#endif
