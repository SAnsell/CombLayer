/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/HeatDump.h
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
#ifndef xraySystem_HeatDump_h
#define xraySystem_HeatDump_h

class Simulation;

namespace xraySystem
{

/*!
  \class HeatDump
  \author S. Ansell
  \version 1.0
  \date January 2018
  \brief Focasable mirror in mount
*/

class HeatDump :
  public attachSystem::ContainedComp,
  public attachSystem::FixedOffset,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap
{
 private:

  double radius;           ///< Radius 
  double height;           ///< width accross beam
  double width;            ///< width accross beam
  double thick;            ///< Thickness in normal direction to reflection

  double cutHeight;        ///< cut from base
  double cutAngle;        ///< Angle of cut
  double cutDepth;        ///< cut distance in Y

  int mat;                 ///< Base material

  // Functions:

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  std::vector<Geometry::Vec3D> calcEdgePoints() const;
  
 public:

  HeatDump(const std::string&);
  HeatDump(const HeatDump&);
  HeatDump& operator=(const HeatDump&);
  virtual ~HeatDump();

  void createAll(Simulation&,
		 const attachSystem::FixedComp&,
		 const long int);
  
};

}

#endif
 
