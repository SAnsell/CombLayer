/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/Table.h
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
#ifndef xraySystem_Table_h
#define xraySystem_Table_h

class Simulation;

namespace xraySystem
{

/*!
  \class Table
  \author S. Ansell
  \version 1.0
  \date September 2021
  \brief Table system for placing in a blockZone
*/

class Table :
  public attachSystem::ContainedComp,
  public attachSystem::FixedRotate,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  double width;           ///< Width
  double length;          ///< Length
  double thick;           ///< Thickness

  double legSize;           ///< Square leg size
  double clearance;         ///< Hole clearace [%]
  

  int voidMat;              ///< Void material
  int plateMat;             ///< Top plate material
  int legMat;               ///< Base material

  std::vector<Geometry::Vec3D> holeCentre;   ///< hole centre
  std::vector<double> holeRadius;            ///< hole radius
  std::vector<HeadRule> holeExclude;         ///< hole exclude
  
  // Functions:

  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  Table(const std::string&);
  Table(const Table&);
  Table& operator=(const Table&);
  virtual ~Table();

  virtual void insertInCells(Simulation&,const std::vector<int>&);

  void addHole(const attachSystem::FixedComp&,
	       const std::string&,const double);
  void addHole(const attachSystem::FixedComp&,
	       const std::string&,const std::string&);

  
  using FixedComp::createAll;
  void createAll(Simulation&,
		 const attachSystem::FixedComp&,
		 const long int);
  
};

}

#endif
 
