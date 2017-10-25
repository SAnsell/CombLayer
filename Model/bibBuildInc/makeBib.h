/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   bibBuildInc/makeBib.h
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
#ifndef bibSystem_makeBib_h
#define bibSystem_makeBib_h


namespace moderatorSystem
{
  class FlightLine;
}


/*!
  \namespace bibSystem
  \brief General ESS-Bilbau stuff
  \version 1.0
  \date April 2013
  \author S. Ansell
*/

namespace bibSystem
{
  class BilReflector;
  class RotorWheel;
  class ProtonPipe;
  class WaterMod;
  class ColdH2Mod;
  class FilterBox;
  class GuideBox;
  class NiGuide;
  class ConcreteWall;
  class GuideShield;
  /*!
    \class makeBib
    \version 1.0
    \author S. Ansell
    \date May 2013
    \brief Bilbao main build system
  */
  
class makeBib
{
 private:

  std::shared_ptr<RotorWheel> Rotor;             ///< Main wheel
  std::shared_ptr<ProtonPipe> ProtonObj;        ///< Main wheel
  std::shared_ptr<BilReflector> RefObj;          ///< Reflector
  std::shared_ptr<WaterMod> WatMod;              ///< Water moderator
  std::shared_ptr<ColdH2Mod> ColdMod;            ///< Cold-H2 moderator fordward
  std::shared_ptr<FilterBox> BeFilterForward;    ///< Berylium Filter 
  std::shared_ptr<FilterBox> BeFilterBackward;   ///< Berylium Filter


  std::shared_ptr<ColdH2Mod> ColdMod2;           ///< Second ColdH2 moderator

  /// Array to replace the horrible list:
  std::vector<std::shared_ptr<GuideBox> > GuideArray;   ///< Guides
  std::vector<std::shared_ptr<GuideShield> > ShieldArray;   ///< Protection around guides

  std::shared_ptr<ConcreteWall> CWall;        ///< Concreate wall

  void buildGuideArray(Simulation&,const int);
  void buildShieldArray(Simulation&);

  void flightLines(Simulation&);

 public:
  
  makeBib();
  makeBib(const makeBib&);
  makeBib& operator=(const makeBib&);
  ~makeBib();
  
  void build(Simulation&,const mainSystem::inputParam&);

};

}

#endif
