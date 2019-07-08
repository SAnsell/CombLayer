/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/BunkerInsert.h
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#ifndef essSystem_BunkerInsert_h
#define essSystem_BunkerInsert_h

class Simulation;

namespace essSystem
{

/*!
  \class BunkerInsert
  \version 1.0
  \author S. Ansell
  \date June 2015
  \brief Bulk around Reflector
*/

class BunkerInsert : public attachSystem::ContainedComp,
  public attachSystem::FixedOffset,public attachSystem::CellMap,
  public attachSystem::ExternalCut
{
 private:

  double backStep;               ///< Step back of dividing plane
  
  double height;                 ///< Height of insert 
  double width;                  ///< Width of insert 
  double topWall;                ///< Top Wall thickness 
  double lowWall;                ///< Low wall thickness
  double leftWall;               ///< Left wall thickness
  double rightWall;              ///< Right wall thickness

  int wallMat;                   ///< wall material
  int voidMat;                   ///< void material

  HeadRule outCut;               ///< Cut volume for items

  /// bunker object to construct insert in
  const attachSystem::FixedComp* bunkerObj;
  
  void populate(const FuncDataBase&);
  void createSurfaces();
  void createLinks();
  void createObjects(Simulation&,const std::string&);

  void addCalcPoint();
  
 public:

  BunkerInsert(const std::string&);
  BunkerInsert(const BunkerInsert&);
  BunkerInsert& operator=(const BunkerInsert&);
  virtual ~BunkerInsert();

  void setBunkerObject(const attachSystem::FixedComp& bunkerFC)
    { bunkerObj= &bunkerFC; }
  
  int objectCut(const std::vector<Geometry::Vec3D>&) const;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 

