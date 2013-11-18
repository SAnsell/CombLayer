/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   essBuildInc/makeESS.h
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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


namespace moderatorSystem
{
  class FlightLine;
}

namespace ts1System 
{
  class ProtonVoid;
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
  class CylModerator;
  class SupplyPipe;
  class CylPreMod;
  class BulkModule;
  class ShutterBay;
  class GuideBay;

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
  boost::shared_ptr<BeRef> Reflector;   ///< reflector object
  boost::shared_ptr<ts1System::ProtonVoid> PBeam;  ///< Proton Void

  // ASSEMBLY 1:
  boost::shared_ptr<essMod> LowMod;  ///< Lower Mod
  boost::shared_ptr<essMod> LowModB;  ///< Lower Mod [if needed]
  boost::shared_ptr<moderatorSystem::FlightLine> LowAFL;  ///< Lower Mode FL
  boost::shared_ptr<moderatorSystem::FlightLine> LowBFL;  ///< Lower Mode FL
  boost::shared_ptr<CylPreMod> LowPre;  ///< Upper Mod (Pre)
  boost::shared_ptr<SupplyPipe> LowSupplyPipe;  ///< Lower supply 

  boost::shared_ptr<CylModerator> TopMod;   ///< Upper Mod
  boost::shared_ptr<moderatorSystem::FlightLine> TopAFL;  ///< Upper Mode FL
  boost::shared_ptr<moderatorSystem::FlightLine> TopBFL;  ///< Upper Mode FL
  boost::shared_ptr<CylPreMod> TopPre;  ///< Upper Mod (Pre)

  boost::shared_ptr<BulkModule> Bulk;      ///< Main bulk module
  boost::shared_ptr<moderatorSystem::FlightLine> BulkLowAFL;  ///< Lower Mode FL

  /// Shutterbay object
  boost::shared_ptr<ShutterBay> ShutterBayObj;  
  std::vector<boost::shared_ptr<GuideBay> > GBArray;  


  void topFlightLines(Simulation&);
  void lowFlightLines(Simulation&);
  void createGuides(Simulation&);
  void buildLowMod(Simulation&);
  void buildConicMod(Simulation&);
  void buildLayerMod(Simulation&);
  void makeTarget(Simulation&,const mainSystem::inputParam&);

 public:
  
  makeESS();
  makeESS(const makeESS&);
  makeESS& operator=(const makeESS&);
  ~makeESS();
  
  void build(Simulation*,const mainSystem::inputParam&);

};

}

#endif
