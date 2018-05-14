 /********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t1UpgradeInc/makeT1Eng.h
 *
 * Copyright (c) 2004-2017 by Stuart Ansell/Goran Skoro
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
#ifndef t1System_makeT1Eng_h
#define t1System_makeT1Eng_h

namespace constructSystem
{
  class ModBase;
  class GroupOrigin;
  class SupplyPipe;
  class TargetBase;
}

namespace TMRSystem 
{
  class TS2target;
  class TS2ModifyTarget;
  class TS2FlatTarget;
}  

namespace moderatorSystem
{
  class TriangleMod;
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
  class BeamWindow;
  class CylReflector;
  class EngReflector;
  
  class ModBase;
  class H2Section;
  class CH4Section;
  class CH4Layser;
  class HPreMod;
  class CH4PreMod;
  class CH4PreFlat;
  class CH4PreModBase;
  class CylMod;
  class CylPreSimple;
  class InnerLayer;
  class SplitInner;

  class BulletPlates;
  class BulletVessel;
  class BulletTarget;
  class FishGillVessel;
  class FishGillTarget;
  
  /*!
    \class makeT1Eng
    \version 1.0
    \author S. Ansell
    \date July 2012
    \brief General constructor for the TS1 system
  */

class makeT1Eng
{
 private:

  std::shared_ptr<constructSystem::TargetBase> TarObj;   ///< Target
  std::shared_ptr<ts1System::BeamWindow> BWindowObj;     ///< Beam window
  std::shared_ptr<EngReflector> RefObj;                  ///< Relfector
  std::shared_ptr<SplitInner> TriMod;  ///< Water moderator
  /// General offset
  std::shared_ptr<constructSystem::GroupOrigin> ColdCentObj; 

  std::shared_ptr<InnerLayer> H2Mod;       ///< H2 moderator
//  std::shared_ptr<ts1System::HPreMod> H2PMod;            ///< H2 premoderator
  std::shared_ptr<CH4PreFlat> H2PMod;     ///< CH4 premoderator
  std::shared_ptr<InnerLayer> CH4Mod;      ///< CH4 moderator
  std::shared_ptr<CH4PreFlat> CH4PMod;     ///< CH4 premoderator

  std::shared_ptr<shutterSystem::t1CylVessel> VoidObj;   ///< Void vessel
  std::shared_ptr<shutterSystem::t1BulkShield> BulkObj;  ///< Bulk shield
  std::shared_ptr<shutterSystem::MonoPlug> MonoTopObj;   ///< Top mono plug
  std::shared_ptr<shutterSystem::MonoPlug> MonoBaseObj;  ///< Base mono plug

  // pipes:

  /// CH4 Pipe
  std::shared_ptr<constructSystem::SupplyPipe> CH4PipeObj;
  /// H2 Pipe
  std::shared_ptr<constructSystem::SupplyPipe> H2PipeObj;
  /// Water Pipe
  std::shared_ptr<constructSystem::SupplyPipe> WaterPipeObj;
  /// Water Return
  std::shared_ptr<constructSystem::SupplyPipe> WaterReturnObj;

  // Array of flightlines
  
  /// TriModerator flight line (Left)
  std::shared_ptr<moderatorSystem::FlightLine> TriFLA;
  /// TriModerator flight line (Right)
  std::shared_ptr<moderatorSystem::FlightLine> TriFLB;
  /// H2 flight line 
  std::shared_ptr<moderatorSystem::FlightLine> H2FL;
  /// CH4 methane
  std::shared_ptr<moderatorSystem::FlightLine> CH4FLA;
  /// CH4 methane
  std::shared_ptr<moderatorSystem::FlightLine> CH4FLB;

  void buildCH4Pipe(Simulation&);
  void buildH2Pipe(Simulation&);
  void buildWaterPipe(Simulation&);

 public:
  
  makeT1Eng();
  makeT1Eng(const makeT1Eng&);
  makeT1Eng& operator=(const makeT1Eng&);
  ~makeT1Eng();
  
  void build(Simulation&,const mainSystem::inputParam&);

};

}

#endif
