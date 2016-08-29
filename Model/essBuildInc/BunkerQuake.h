/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/BunkerQuake.h
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#ifndef essSystem_BunkerQuake_h
#define essSystem_BunkerQuake_h

class Simulation;

namespace essSystem
{
  class Bunker;
  class BunkerMainWall;
  class BunkerInsert;
  
/*!
  \class BunkerQuake
  \version 1.0
  \author S. Ansell
  \date April 2015
  \brief Bunker roof 
*/

class BunkerQuake : public attachSystem::ContainedComp,
  public attachSystem::FixedComp,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  const std::string baseName;     ///< Bunker base name

  double xGap;                          ///< x stuff
  double zGap;                          ///< Needs to be surface
  std::vector<Geometry::Vec3D> APoint;  ///< Centre points of cut
  std::vector<Geometry::Vec3D> BPoint;  ///< Centre points of cut

  

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

 public:

  BunkerQuake(const std::string&);
  BunkerQuake(const BunkerQuake&);
  BunkerQuake& operator=(const BunkerQuake&);
  virtual ~BunkerQuake();
  
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 

