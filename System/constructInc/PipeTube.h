/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/PipeTube.h
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
#ifndef constructSystem_PipeTube_h
#define constructSystem_PipeTube_h

class Simulation;

namespace constructSystem
{
  
/*!
  \class PipeTube
  \version 1.0
  \author S. Ansell
  \date January 2018
  \brief PipeTube unit  
*/

class PipeTube :
  public attachSystem::FixedOffset,
  public attachSystem::ContainedSpace,
  public attachSystem::CellMap,
  public attachSystem::FrontBackCut
{
 private:

  double radius;              ///< radius of main tube
  double wallThick;           ///< wall thickness of main tube
  double length;              ///< Main length
  
  double flangeARadius;        ///< Joining Flange radius
  double flangeALength;        ///< Joining Flange length
  double flangeBRadius;        ///< Joining Flange radius
  double flangeBLength;        ///< Joining Flange length

  int voidMat;                ///< void material
  int wallMat;                ///< Fe material layer

  std::set<int> portCells;               ///< Extra cells for the port
  std::vector<Geometry::Vec3D> PCentre;  ///< Centre points [relative to origin]
  std::vector<Geometry::Vec3D> PAxis;    ///< Port centre Axis
  std::vector<portItem> Ports;           ///< Vector of ports FixedComp
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  void createPorts(Simulation&);
  
 public:

  PipeTube(const std::string&);
  PipeTube(const PipeTube&);
  PipeTube& operator=(const PipeTube&);
  virtual ~PipeTube();

  void splitVoidPorts(Simulation&,const std::string&,const int,const int);
  void addInsertPortCells(const int);
  void intersectPorts(Simulation&,const size_t,const size_t) const;
  const portItem& getPort(const size_t) const;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
