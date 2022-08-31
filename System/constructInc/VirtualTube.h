/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/VirtualTube.h
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
  bool zNorm;                 ///< Apply Z tracking
  Geometry::Vec3D postZAxis;  ///< Alignment axis for Z after tracking
  double postYRotation;       ///< Post Y rotation if Y aligned to port.
  
  std::set<int> portCells;               ///< Extra cells for the port
  std::vector<Geometry::Vec3D> PCentre;  ///< Centre points [relative to origin]
  std::vector<Geometry::Vec3D> PAxis;    ///< Port centre Axis
  /// Vector of ports FixedComp
  std::vector<std::shared_ptr<portItem>> Ports;     

  virtual void applyPortRotation();

  std::string makeOuterVoid(Simulation&);
  
  virtual void populate(const FuncDataBase&);
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

  void setPortRotation(const size_t,const Geometry::Vec3D&,
		       const double =0.0);
  void setPortRotation(const size_t,const Geometry::Vec3D&,
		       const Geometry::Vec3D&);

  void addInsertPortCells(const int);
  void intersectPorts(Simulation&,const size_t,const size_t) const;
  void intersectVoidPorts(Simulation&,const size_t,const size_t) const;
  const portItem& getPort(const size_t) const;

  virtual void createPorts(Simulation&,MonteCarlo::Object*,
		   const HeadRule&,const HeadRule&);
  virtual void createPorts(Simulation&) =0;

  virtual void insertAllInCell(Simulation&,const int) const;
  virtual void insertAllInCell(Simulation&,const std::vector<int>&) const;
  virtual void insertMainInCell(Simulation&,const int) const;
  virtual void insertMainInCell(Simulation&,const std::vector<int>&) const;
  virtual void insertPortInCell(Simulation&,const size_t,const int) const;
  virtual void insertPortsInCell(Simulation&,const int) const;
  virtual void insertPortsInCell(Simulation&,
				const std::vector<std::set<int>>&) const;

  /// accessor to outer main radius
  double getOuterRadius() const { return radius+wallThick; }

  using FixedComp::createAll;
  virtual void createAll(Simulation&,
			 const attachSystem::FixedComp&,
			 const long int);

};

}

#endif
 
