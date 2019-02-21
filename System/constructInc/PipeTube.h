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
  public attachSystem::ContainedGroup,
  public attachSystem::CellMap,
  public attachSystem::SurfMap,
  public attachSystem::FrontBackCut
{
 protected:

  double radius;              ///< radius of main tube
  double wallThick;           ///< wall thickness of main tube
  double length;              ///< Main length
  
  double flangeARadius;        ///< Joining Flange radius
  double flangeALength;        ///< Joining Flange length
  double flangeBRadius;        ///< Joining Flange radius
  double flangeBLength;        ///< Joining Flange length
  double flangeACap;           ///< Thickness of Flange cap if present
  double flangeBCap;           ///< Thickness of Flange cap if present
  
  int voidMat;                ///< void material
  int wallMat;                ///< Fe material layer
  int capMat;                 ///< flange cap material layer

  bool delayPortBuild;        ///< Delay port to manual construct
  size_t portConnectIndex;    ///< Port to connect to
  Geometry::Vec3D rotAxis;    ///< Rotation axis for port rotate

  std::set<int> portCells;               ///< Extra cells for the port
  std::vector<Geometry::Vec3D> PCentre;  ///< Centre points [relative to origin]
  std::vector<Geometry::Vec3D> PAxis;    ///< Port centre Axis
  std::vector<portItem> Ports;           ///< Vector of ports FixedComp

  virtual void applyPortRotation();
  Geometry::Vec3D calcCylinderDistance(const size_t) const;

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  
 public:

  PipeTube(const std::string&);
  PipeTube(const PipeTube&);
  PipeTube& operator=(const PipeTube&);
  virtual ~PipeTube();

  /// Set a port delay
  void delayPorts() { delayPortBuild=1; }
  int splitVoidPorts(Simulation&,const std::string&,
		     const int,const int);
  int splitVoidPorts(Simulation&,const std::string&,const int,
		     const int,const Geometry::Vec3D&);
  int splitVoidPorts(Simulation&,const std::string&,const int,
		     const int,const std::vector<size_t>&);

  void setPortRotation(const size_t,const Geometry::Vec3D&);

  void addInsertPortCells(const int);
  void intersectPorts(Simulation&,const size_t,const size_t) const;
  void intersectVoidPorts(Simulation&,const size_t,const size_t) const;
  const portItem& getPort(const size_t) const;

  void createPorts(Simulation&);

  virtual void insertAllInCell(Simulation&,const int);

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
