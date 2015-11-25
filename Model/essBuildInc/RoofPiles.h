/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/RoofPiles.h
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#ifndef essSystem_RoofPiles_h
#define essSystem_RoofPiles_h

class Simulation;

namespace essSystem
{
  class RoofPilesMainWall;
  class RoofPilesInsert;
  
/*!
  \class RoofPiles
  \version 1.0
  \author S. Ansell
  \date April 2015
  \brief Bulk around Reflector
*/

class RoofPiles : public attachSystem::ContainedComp,
  public attachSystem::FixedComp,
  public attachSystem::CellMap
{
 private:
   
  const int rodIndex;           ///< Index of surface offset
  int cellIndex;                ///< Cell index

  /// Relative point for each centre
  std::vector<Geometry::Vec3D> CentPoint;

  double radius;                ///< Radius for each support
  int mat;                      ///< Matieral
  
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const attachSystem::FixedComp&,
			const long int,const bool);

  void createSurfaces();
  void createLinks();
  void createObjects(Simulation&,const attachSystem::FixedComp&,
		     const long int);
  
 public:

  RoofPiles(const std::string&);
  RoofPiles(const RoofPiles&);
  RoofPiles& operator=(const RoofPiles&);
  virtual ~RoofPiles();


  void createAll(Simulation&,

};

}

#endif
 

