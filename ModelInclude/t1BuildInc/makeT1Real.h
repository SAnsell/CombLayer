/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   t1BuildInc/makeT1Real.h
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
#ifndef t1System_makeT1Real_h
#define t1System_makeT1Real_h

namespace constructSystem
{
  class TargetBase;
}

namespace TMRSystem
{
  class TS2target;
}  


namespace moderatorSystem
{
  class FlightLine;
}

namespace shutterSystem
{
  class t1BulkShield;
  class t1CylVessel;
  class t1Vessel;
  class MonoPlug;
}

/*!
  \namespace ts1System
  \brief General T1 upgrade/main build
  \version 1.0
  \date November 2011
  \author S. Ansell
*/

namespace ts1System
{

// class t1PlateTarget;
class PressVessel;
class PlateTarget;
class WaterDividers;
class t1Reflector;
class Lh2Moderator;
class H2Moderator;
class CH4Moderator;
class MerlinModerator;
class WaterPipe;
class H2Pipe;
class CH4Pipe;
class BeamWindow;

  /*!
    \class makeT1Real
    \version 1.0
    \author S. Ansell
    \date July 2012
    \brief General constructor for the TS1 system
  */

class makeT1Real
{
 private:


  /// Target object
  std::shared_ptr<constructSystem::TargetBase> TarObj;  
  std::shared_ptr<t1Reflector> RefObj;               ///< Reflector 
  std::shared_ptr<H2Moderator> Lh2ModObj;            ///< LH2 Moderator 
  std::shared_ptr<CH4Moderator> CH4ModObj;           ///< CH4 liquid
  std::shared_ptr<MerlinModerator> MerlinMod;        ///< Merlin
  std::shared_ptr<MerlinModerator> WaterModObj;       ///< Water Moderator
  std::shared_ptr<WaterPipe> WaterPipeObj;           ///< Water pipe
  std::shared_ptr<WaterPipe> MPipeObj;               ///< Merlin pipe
  std::shared_ptr<H2Pipe> H2PipeObj;                 ///< H2 pipe
  std::shared_ptr<CH4Pipe> CH4PipeObj;                 ///< CH4 pipe    

  std::shared_ptr<shutterSystem::t1CylVessel> VoidObj;   ///< Void vessel
  std::shared_ptr<shutterSystem::t1BulkShield> BulkObj;  ///< Bulk shield
  std::shared_ptr<shutterSystem::MonoPlug> MonoTopObj;  ///< Top mono plug
  std::shared_ptr<shutterSystem::MonoPlug> MonoBaseObj; ///< Base mono plug

  /// Array of flightlines

  std::shared_ptr<moderatorSystem::FlightLine> H2FL; 
  std::shared_ptr<moderatorSystem::FlightLine> CH4NorthFL;
  std::shared_ptr<moderatorSystem::FlightLine> CH4SouthFL; 
  std::shared_ptr<moderatorSystem::FlightLine> MerlinFL; 
  std::shared_ptr<moderatorSystem::FlightLine> WaterNorthFL; 
  std::shared_ptr<moderatorSystem::FlightLine> WaterSouthFL; 

  void flightLines(Simulation*);

  std::string buildTarget(Simulation&,const std::string&,const int);
  
 public:
  
  makeT1Real();
  makeT1Real(const makeT1Real&);
  makeT1Real& operator=(const makeT1Real&);
  ~makeT1Real();
  
  void build(Simulation*,const mainSystem::inputParam&);

};

}

#endif
