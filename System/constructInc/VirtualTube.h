/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/VirtualTube.h
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#ifndef constructSystem_VirtualTube_h
#define constructSystem_VirtualTube_h

class Simulation;

namespace constructSystem
{
  class portItem;  
/*!
  \class VirtualTube
  \version 1.0
  \author S. Ansell
  \date January 2018
  \brief VirtualTube unit  
*/

class VirtualTube :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedGroup,
  public attachSystem::CellMap,
  public attachSystem::SurfMap,
  public attachSystem::FrontBackCut
{
 protected:

  double radius;              ///< radius of main tube
  double wallThick;           ///< wall thickness of main tube
  double length;              ///< Main length
    
  int voidMat;                ///< void material
  int wallMat;                ///< Fe material layer
  int capMat;                 ///< flange cap material layer

  bool outerVoid;             ///< Is outer void needed

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

  std::string makeOuterVoid(Simulation&);
  
  virtual void populate(const FuncDataBase&);
  virtual void createUnitVector(const attachSystem::FixedComp&,const long int);
  virtual void createSurfaces() =0;
  virtual void createObjects(Simulation&) =0;
  virtual void createLinks() =0;
  
 public:

  VirtualTube(const std::string&);
  VirtualTube(const VirtualTube&);
  VirtualTube& operator=(const VirtualTube&);
  virtual ~VirtualTube();

  /// Set the outer covering volume
  void setOuterVoid() { outerVoid=1; }
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
  virtual void insertPortInCell(Simulation&,const int);
  virtual void insertPortInCell(Simulation&,
				const std::vector<std::set<int>>&);

  using FixedComp::createAll;
  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
			 const long int);

};

}

#endif
 
