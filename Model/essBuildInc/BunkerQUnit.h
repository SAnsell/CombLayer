/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/BunkerQUnit.h
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
#ifndef essSystem_BunkerQUnit_h
#define essSystem_BunkerQUnit_h

class Simulation;

namespace essSystem
{
  class Bunker;
  class BunkerMainWall;
  class BunkerInsert;
  
/*!
  \class BunkerQUnit
  \version 1.0
  \author S. Ansell
  \date March 2017
  \brief Single connected unit for the bunker quake line
*/

class BunkerQUnit : public attachSystem::ContainedComp,
  public attachSystem::FixedOffset,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  const int cutIndex;             ///< Surface index
  int cellIndex;                  ///< Cell index
  
  double xGap;                    ///< x stuff
  double zGap;                    ///< Needs to be surface

  std::vector<int> PFlag;             ///< absolute/ZRelative/XYZ
  std::vector<Geometry::Vec3D> cPts;  ///< Centre points of cut
  std::vector<double> Radii;          ///< Curve radii [-ve for plane]
  std::vector<int> yFlag;             ///< Y flags   [0 for plane]
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int,const long int);
  void modifyPoints();
  void createObjects(Simulation&);
  
 public:

  BunkerQUnit(const std::string&);
  BunkerQUnit(const BunkerQUnit&);
  BunkerQUnit& operator=(const BunkerQUnit&);
  virtual ~BunkerQUnit();
  
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int,const long int);

};

}

#endif
 

