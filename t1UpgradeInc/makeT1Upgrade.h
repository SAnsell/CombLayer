/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   t1UpgradeInc/makeT1Upgrade.h
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
#ifndef t1System_makeT1Upgrade_h
#define t1System_makeT1Upgrade_h

namespace constructSystem
{
  class ModBase;
  class GroupOrigin;
  class TargetBase;
}

namespace TMRSystem
{
  class TS2target;
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
  class ModBase;
  class H2Section;
  class CH4Section;
  class CH4Layser;
  class HPreMod;
  class CH4Mod;
  class CH4PreMod;
  class CH4PreModBase;
  class CylMod;
  class CylPreSimple;

  /*!
    \class makeT1Upgrade
    \version 1.0
    \author S. Ansell
    \date July 2012
    \brief General constructor for the TS1 system
  */

class makeT1Upgrade
{
 private:

  boost::shared_ptr<constructSystem::TargetBase> TarObj;   ///< Target
  boost::shared_ptr<ts1System::BeamWindow> BWindowObj;     ///< Beam window
  boost::shared_ptr<CylReflector> RefObj;                  ///< Relfector
  boost::shared_ptr<moderatorSystem::TriangleMod> TriMod;  ///< Water moderator
  /// General offset
  boost::shared_ptr<constructSystem::GroupOrigin> ColdCentObj; 

  boost::shared_ptr<constructSystem::ModBase> H2Mod;       ///< H2 moderator
  boost::shared_ptr<ts1System::HPreMod> H2PMod;            ///< H2 premoderator
  boost::shared_ptr<ts1System::CylPreSimple> H2PCylMod;    ///< H2 premoderator
  boost::shared_ptr<constructSystem::ModBase> CH4Mod;      ///< CH4 moderator
  boost::shared_ptr<ts1System::CH4PreModBase> CH4PMod;     ///< CH4 premoderator

  boost::shared_ptr<shutterSystem::t1CylVessel> VoidObj;   ///< Void vessel
  boost::shared_ptr<shutterSystem::t1BulkShield> BulkObj;  ///< Bulk shield
  boost::shared_ptr<shutterSystem::MonoPlug> MonoTopObj;   ///< Top mono plug
  boost::shared_ptr<shutterSystem::MonoPlug> MonoBaseObj;  ///< Base mono plug

  // Array of flightlines
  
  // TriModerator flight line 
  boost::shared_ptr<moderatorSystem::FlightLine> TriFLA;
  boost::shared_ptr<moderatorSystem::FlightLine> TriFLB;
  boost::shared_ptr<moderatorSystem::FlightLine> H2FL;
  boost::shared_ptr<moderatorSystem::FlightLine> CH4FLA;
  boost::shared_ptr<moderatorSystem::FlightLine> CH4FLB;

  std::string buildTarget(Simulation&,const std::string&,const int);
  std::string buildCH4Pre(Simulation&,const std::string&);
  std::string buildCH4Mod(Simulation&,const attachSystem::FixedComp&,
			  const std::string&);
  std::string buildH2Mod(Simulation&,const attachSystem::FixedComp&,
			 const std::string&);

  void buildHelp(Simulation&);

 public:
  
  makeT1Upgrade();
  makeT1Upgrade(const makeT1Upgrade&);
  makeT1Upgrade& operator=(const makeT1Upgrade&);
  ~makeT1Upgrade();
  
  void build(Simulation*,const mainSystem::inputParam&);

};

}

#endif
