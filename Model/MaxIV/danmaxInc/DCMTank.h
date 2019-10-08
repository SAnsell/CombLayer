/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   danmaxInc/DCMTank.h
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
#ifndef xraySystem_DCMTank_h
#define xraySystem_DCMTank_h

class Simulation;

namespace xraySystem
{
  
/*!
  \class DCMTank
  \version 1.0
  \author S. Ansell
  \date July 2015
  \brief DCMTank unit  
*/

class DCMTank :
  public attachSystem::FixedOffset,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap,
  public attachSystem::ExternalCut
{
 private:

  const bool centreOrigin;    ///< Construct on the centre line
  double outerSize;           ///< Distance for outer void
  
  double voidRadius;          ///< void main radius
  double voidDepth;           ///< void depth to dome
  double voidHeight;          ///< void height to smooth lid jo
  
  double wallThick;           ///< Thick of side walls

  double baseThick;            ///< thickness of base
  double baseWidth;            ///< width of base
  double baseLength;           ///< length [beam direct] of base

  double topLift;              ///< Lift of top dome
  
  // inlet port
  double portAXStep;          ///< XStep of port
  double portAZStep;          ///< ZStep of port
  double portAWallThick;      ///< Flange wall thickness
  double portATubeLength;     ///< Port tube length
  double portATubeRadius;     ///< Port tube radius

  // exit port
  double portBXStep;          ///< XStep of port
  double portBZStep;          ///< ZStep of port
  double portBWallThick;      ///< Flange wall thickness
  double portBTubeLength;     ///< Port tube length
  double portBTubeRadius;     ///< Port tube radius

  double flangeAXStep;         ///< Step of flange [offset]
  double flangeAZStep;         ///< Step of flange [offset]
  double flangeBXStep;         ///< Step of flange [offset]
  double flangeBZStep;        ///< Step of flange [offset]
  double flangeARadius;        ///< Joining Flange radius
  double flangeALength;        ///< Joining Flange length
  double flangeBRadius;        ///< Joining Flange radius
  double flangeBLength;        ///< Joining Flange length

  std::set<int> portCells;               ///< Extra cells for the port
  std::vector<Geometry::Vec3D> PCentre;  ///< Centre points [relative to origin]
  std::vector<Geometry::Vec3D> PAxis;    ///< Port centre Axis
  /// Vector of ports FixedComp
  std::vector<constructSystem::portItem> Ports;  

  int voidMat;                  ///< void material
  int wallMat;                  ///< Fe material layer

  bool delayPortBuild;        ///< Delay port to manual construct
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  DCMTank(const std::string&);
  DCMTank(const DCMTank&);
  DCMTank& operator=(const DCMTank&);
  virtual ~DCMTank();

  /// Set a port delay
  void delayPorts() { delayPortBuild=1; }
  void createPorts(Simulation&);

  using FixedComp::createAll;
  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
