/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/BeamMount.h
*
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#ifndef xraySystem_BeamMount_h
#define xraySystem_BeamMount_h

class Simulation;

namespace xraySystem
{

/*!
  \class BeamMount
  \author S. Ansell
  \version 1.0
  \date January 2018
  \brief Focasable mirror in mount
*/

class BeamMount :
  public attachSystem::ContainedGroup,
  public attachSystem::FixedOffsetGroup,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap
{
 private:
  
  int blockFlag;           ///< build the block [1:centre / 2 low Edge]    
  bool upFlag;             ///< Up/down

  double outLift;         ///< Amount to lift [when raised]
  double beamLift;        ///< Amount to lift by in the beam
  
  double supportRadius;    ///< Radius of support  
  int supportMat;          ///< support material

    
  double height;           ///< height total 
  double width;            ///< width accross beam
  double length;           ///< Thickness in normal direction to reflection  
  int blockMat;            ///< Base material

  // Functions:

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int,
			const attachSystem::FixedComp&,const long int);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  std::vector<Geometry::Vec3D> calcEdgePoints() const;
  
 public:

  BeamMount(const std::string&);
  BeamMount(const BeamMount&);
  BeamMount& operator=(const BeamMount&);
  virtual ~BeamMount();

  void createAll(Simulation&,
		 const attachSystem::FixedComp&,const long int,
		 const attachSystem::FixedComp&,const long int);
  
};

}

#endif
 
