/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/makeESS.h
 *
 * Copyright (c) 2004-2017 by Stuart Ansell/Konstantin Batkov
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
  class BasicFlightLine;
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
  class FocusPoints;
  class WheelBase;
  class Wheel;
  class BilbaoWheel;
  class BeRef;
  class essMod;
  class ESSPipes;
  class ConicModerator;
  class CylPreMod;
  class IradCylinder;
  class BulkModule;
  class ShutterBay;
  class ProtonTube;
  class PreModWing;
  class GuideBay;
  class BeamMonitor;
  class DiskPreMod;
  class DiskLayerMod;
  class Bunker;
  class TwisterModule;
  class RoofPillars;
  class Curtain;
  class F5Collimator;
  class BunkerFeed;
  class WedgeFlightLine;
  class Chicane;

      
  /*!
    \class makeESS
    \version 1.0
    \author S. Ansell
    \date January 2013
    \brief Main moderator system for ESS
  */
  
class makeESS
{
 private:
  
  std::shared_ptr<WheelBase> Target;   ///< target object
  std::shared_ptr<BeRef> Reflector;    ///< reflector object
  std::shared_ptr<ProtonTube> PBeam;   ///< Proton Void
  std::shared_ptr<BeamMonitor> BMon;   ///< Beam Monitor

  // main moderator focus points
  std::shared_ptr<FocusPoints> topFocus;   ///< Top focus
  std::shared_ptr<FocusPoints> lowFocus;   ///< Lower focus

  // Butterfly
  /// Primary Lower Mod 
  std::shared_ptr<constructSystem::ModBase> LowMod;
  std::shared_ptr<DiskPreMod> LowPreMod;         ///< Lower mod 
  std::shared_ptr<DiskPreMod> LowCapMod;         ///< Upper mod

  std::shared_ptr<essSystem::WedgeFlightLine> LowAFL;  ///< Lower Mode FL
  std::shared_ptr<essSystem::WedgeFlightLine> LowBFL;  ///< Lower Mode FL
  
  // Butterly

  std::shared_ptr<constructSystem::ModBase> TopMod;   ///< Primary Upper Mod 
  std::shared_ptr<DiskLayerMod> TopPreMod;            ///< Top mod 
  std::shared_ptr<DiskLayerMod> TopCapMod;            ///< Lower mod

  std::shared_ptr<PreModWing> LowPreWingA; ///< Low premoderator wing
  std::shared_ptr<PreModWing> LowPreWingB; ///< Low premoderator wing
  std::shared_ptr<PreModWing> TopPreWingA; ///< Low premoderator wing
  std::shared_ptr<PreModWing> TopPreWingB; ///< Top premoderator wing

  std::shared_ptr<essSystem::WedgeFlightLine> TopAFL;  ///< Top Mod FL
  std::shared_ptr<essSystem::WedgeFlightLine> TopBFL;  ///< Top Mod FL

  std::unique_ptr<ESSPipes> ModPipes;       ///< Moderator pipes


  std::shared_ptr<BulkModule> Bulk;      ///< Main bulk module
  std::shared_ptr<TwisterModule> Twister; ///< Moderator twister module

  /// Shutterbay objects
  std::shared_ptr<ShutterBay> ShutterBayObj;  
  /// Array of Guidies
  std::vector<std::shared_ptr<GuideBay> > GBArray;  

  std::shared_ptr<Bunker> ABunker;  ///< Right bunker [A unit]
  std::shared_ptr<Bunker> BBunker;  ///< Left bunker [B unit]
  std::shared_ptr<Bunker> CBunker;  ///< Opposite Right bunker [A unit]
  std::shared_ptr<Bunker> DBunker;  ///< Opposite bunker [B unit]
  /// A bunker freed thorugh
  std::vector<std::shared_ptr<BunkerFeed> > bFeedArray;

  ///< Right bunker Pillars [A]
  std::shared_ptr<RoofPillars> ABunkerPillars;
  ///< Right bunker Pillars [B]
  std::shared_ptr<RoofPillars> BBunkerPillars; 
  std::shared_ptr<Curtain> TopCurtain;  ///< Conc-curtain

  /// collimators for F5 tallies
  std::vector<std::shared_ptr<F5Collimator>> F5array; 

  void buildFocusPoints(Simulation&);
  
  void createGuides(Simulation&);

  void buildLowButterfly(Simulation&);
  void buildLowerPipe(Simulation&,const std::string&);

  void buildIradComponent(Simulation&,const mainSystem::inputParam&);


  void buildTopButterfly(Simulation&);
  void buildToperPipe(Simulation&,const std::string&);

  void makeTarget(Simulation&,const std::string&);
  void makeBunker(Simulation&,const mainSystem::inputParam&);
  
  void makeBeamLine(Simulation&,
		    const mainSystem::inputParam&);

  void buildPillars(Simulation&);
  void buildBunkerFeedThrough(Simulation&,
			      const mainSystem::inputParam&);
  void buildBunkerChicane(Simulation&,
			  const mainSystem::inputParam&);
  void buildBunkerQuake(Simulation&,
			const mainSystem::inputParam&);
  void buildPreWings(Simulation&);
  void buildTwister(Simulation&);

  void buildF5Collimator(Simulation&,const size_t);

  void optionSummary(Simulation&);

 public:
  
  makeESS();
  makeESS(const makeESS&);
  makeESS& operator=(const makeESS&);
  ~makeESS();
  
  void build(Simulation&,const mainSystem::inputParam&);

};

}

#endif
