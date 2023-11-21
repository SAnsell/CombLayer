/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonBeamInc/bladerforkHoles.h
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
#ifndef xraySystem_forkHoles_h
#define xraySystem_forkHoles_h

class Simulation;

namespace ModelSupport
{
  class surfRegister;
}
 
namespace xraySystem
{
  class PortChicane;
/*!
  \class forkHoles
  \version 1.0
  \author S. Ansell
  \date December 2021
  \brief forkHoles unit 

  Built around the central beam axis with out 
  side wall on the ring side.
*/

class forkHoles 
{
 private:

  const std::string masterName; ///< Name of controlling FC
  size_t nForks;                ///< number of holes
  std::string forkWall;         ///< Wall for forklift
  double forkXStep;             ///< Step across beamline for forklift hole
  double forkYStep;             ///< Step down beamline for forklift hole
  double forkLength;            ///< length of units
  double forkHeight;            ///< height of unit
  std::vector<double> fZStep;   ///< step of units

  HeadRule forkWallBack;
  HeadRule forkWallRing;
  HeadRule forkWallOuter;
  
 public:

  forkHoles(std::string );
  forkHoles(const forkHoles&);
  forkHoles& operator=(const forkHoles&);
  ~forkHoles();

  
  void populate(const FuncDataBase&);
  void createSurfaces(ModelSupport::surfRegister&,
		      const attachSystem::FixedComp&,
		      const int);

  /// accessor to size
  size_t getSize() const { return nForks; }
  const HeadRule& getOuterCut() const { return forkWallOuter; }
  const HeadRule& getBackCut() const { return forkWallBack; }
  const HeadRule& getRingCut() const { return forkWallRing; }

  /// active fork
  bool isActive(const std::string& testKey) const
  { return testKey==forkWall; }

};

}

#endif
