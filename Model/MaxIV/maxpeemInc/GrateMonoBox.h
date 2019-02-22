/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   maxpeemInc/GrateMonoBox.h
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
#ifndef xraySystem_GrateMonoBox_h
#define xraySystem_GrateMonoBox_h

class Simulation;

namespace xraySystem
{
  
/*!
  \class GrateMonoBox
  \version 1.0
  \author S. Ansell
  \date July 2015
  \brief GrateMonoBox unit  
*/

class GrateMonoBox :
  public attachSystem::FixedOffset,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap,
  public attachSystem::FrontBackCut
{
 private:

  const bool centreOrigin;      ///< Construct on the centre line

  double voidHeight;            ///< void height [flat section]
  double voidDepth;             ///< void depth [flat section]
  double voidWidth;             ///< void width [total]
  double voidLength;            ///< void forward [total]
  double voidRadius;            ///< void radius [low Z only]

  double overHangExtent;        ///< Overhang distance
  double overHangDepth;         ///< Overhang distance
  
  double wallThick;             ///< Thick of side walls
  double radiusThick;           ///< radius thickness
  double roofThick;             ///< roof thickness

  double portAXStep;          ///< XStep of port
  double portAZStep;          ///< ZStep of port
  double portAWallThick;      ///< Flange wall thickness
  double portATubeLength;     ///< Port tube length
  double portATubeRadius;     ///< Port tube radius

  double portBXStep;          ///< XStep of port
  double portBZStep;          ///< ZStep of port
  double portBWallThick;      ///< Flange wall thickness
  double portBTubeLength;     ///< Port tube length
  double portBTubeRadius;     ///< Port tube radius

  double flangeARadius;        ///< Joining Flange radius
  double flangeALength;        ///< Joining Flange length
  double flangeBRadius;        ///< Joining Flange radius
  double flangeBLength;        ///< Joining Flange length

  std::set<int> portCells;               ///< Extra cells for the port
  std::vector<Geometry::Vec3D> PCentre;  ///< Centre points [relative to origin]
  std::vector<Geometry::Vec3D> PAxis;    ///< Port centre Axis
  /// Vector of ports FixedComp
  std::vector<constructSystem::portItem> Ports;  

  int voidMat;                ///< void material
  int wallMat;                  ///< Fe material layer
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  GrateMonoBox(const std::string&);
  GrateMonoBox(const GrateMonoBox&);
  GrateMonoBox& operator=(const GrateMonoBox&);
  virtual ~GrateMonoBox();

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
