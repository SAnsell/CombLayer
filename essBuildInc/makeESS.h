/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   essBuildInc/makeESS.h
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
#ifndef essSystem_makeESS_h
#define essSystem_makeESS_h


namespace beamlineSystem
{
  class beamlineConstructor;
}

namespace constructSystem
{
  class ModBase;
  class SupplyPipe;

}

namespace moderatorSystem
{
  class FlightLine;
}

/*!
  \namespace essSystem
  \brief General ESS stuff
  \version 1.0
  \date January 2013
  \author S. Ansell
*/

namespace essSystem
{
  class WheelBase;
  class SegWheel;
  class Wheel;
  class BeRef;
  class essMod;
  class ConicModerator;
  class CylPreMod;
  class BulkModule;
  class ShutterBay;
  class ProtonTube;
  class GuideBay;
  class BeamMonitor;

  /*!
    \class makeESS
    \version 1.0
    \author S. Ansell
    \date January 2013
    \brief WheeModerator for ESS 
  */
  
class makeESS
{
 private:
  
  boost::shared_ptr<WheelBase> Target;   ///< target object
  boost::shared_ptr<BeRef> Reflector;    ///< reflector object
  boost::shared_ptr<ProtonTube> PBeam;   ///< Proton Void
  boost::shared_ptr<BeamMonitor> BMon;   ///< Beam Monitor

  // ASSEMBLY 1:
  /// Primary Lower Mod
  boost::shared_ptr<constructSystem::ModBase> LowMod;  
  /// Secondary  Lower Mod [if needed]
  boost::shared_ptr<constructSystem::ModBase> LowModB; 
  boost::shared_ptr<moderatorSystem::FlightLine> LowAFL;  ///< Lower Mode FL
  boost::shared_ptr<moderatorSystem::FlightLine> LowBFL;  ///< Lower Mode FL
  boost::shared_ptr<CylPreMod> LowPre;          ///< Upper Mod (Pre)
  /// Lower supply 
  boost::shared_ptr<constructSystem::SupplyPipe> LowSupplyPipe; 
  boost::shared_ptr<constructSystem::SupplyPipe> LowReturnPipe;  ///< Lower supply 

  boost::shared_ptr<constructSystem::ModBase> TopMod;   ///< Upper Mod
  boost::shared_ptr<moderatorSystem::FlightLine> TopAFL;  ///< Upper Mode FL
  boost::shared_ptr<moderatorSystem::FlightLine> TopBFL;  ///< Upper Mode FL
  boost::shared_ptr<CylPreMod> TopPre;  ///< Upper Mod (Pre)

  boost::shared_ptr<BulkModule> Bulk;      ///< Main bulk module
  boost::shared_ptr<moderatorSystem::FlightLine> BulkLowAFL;  ///< Lower Mode FL

  /// Shutterbay objects
  boost::shared_ptr<ShutterBay> ShutterBayObj;  
  /// Array of Guidies
  std::vector<boost::shared_ptr<GuideBay> > GBArray;  

  /// Array of beamlines constructors:
  std::vector<boost::shared_ptr<beamlineSystem::beamlineConstructor> > 
    BLArray;  

  void topFlightLines(Simulation&);
  void lowFlightLines(Simulation&);
  void createGuides(Simulation&);

  void buildLowMod(Simulation&,const std::string&);
  void buildLowCylMod(Simulation&);
  void buildLowConicMod(Simulation&);
  void buildLayerMod(Simulation&);
  void buildTopCylMod(Simulation&);
  void buildLowerPipe(Simulation&,const std::string&);

  void makeTarget(Simulation&,const std::string&);

  void makeBeamLine(Simulation&,
		    const mainSystem::inputParam&);
  void optionSummary(Simulation&);

 public:
  
  makeESS();
  makeESS(const makeESS&);
  makeESS& operator=(const makeESS&);
  ~makeESS();
  
  void build(Simulation*,const mainSystem::inputParam&);

};

}

#endif
