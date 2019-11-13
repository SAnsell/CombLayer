/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   constructInc/BiPortTube.h
 *
 * Copyright (c) 2004-2019 by Stuart Ansell / Konstantin Batkov
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
#ifndef constructSystem_BiPortTube_h
#define constructSystem_BiPortTube_h

class Simulation;

namespace constructSystem
{

/*!
  \class BiPortTube
  \version 1.0
  \author S. Ansell / K. Batkov
  \date November 2019
  \brief BiPortTube unit (PipeTube with which can be connected to two
  branches of a beam line)
*/

class BiPortTube :
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
  double flangeACapThick;           ///< Thickness of Flange cap if present
  double flangeBCapThick;           ///< Thickness of Flange cap if present

  int voidMat;                ///< void material
  int wallMat;                ///< Fe material layer
  int capMat;                 ///< flange cap material layer

  bool delayPortBuild;        ///< Delay port to manual construct
  size_t portConnectIndex;    ///< Port to connect for new-origin
  Geometry::Vec3D rotAxis;    ///< Rotation axis for port rotate

  std::set<int> portCells;               ///< Extra cells for the port
  std::vector<Geometry::Vec3D> PCentre;  ///< Centre points [relative to origin]
  std::vector<Geometry::Vec3D> PAxis;    ///< Port centre Axis
  /// Vector of ports FixedComp
  std::vector<std::shared_ptr<portItem>> Ports;

  virtual void applyPortRotation();
  Geometry::Vec3D calcCylinderDistance(const size_t) const;

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  BiPortTube(const std::string&);
  BiPortTube(const BiPortTube&);
  BiPortTube& operator=(const BiPortTube&);
  virtual ~BiPortTube();

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
  virtual void insertAllInCell(Simulation&,const std::vector<int>&);
  virtual void insertMainInCell(Simulation&,const int);
  virtual void insertMainInCell(Simulation&,const std::vector<int>&);
  virtual void insertPortInCell(Simulation&,
				const std::vector<std::set<int>>&);

  using FixedComp::createAll;
  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
			 const long int);

};

}

#endif
