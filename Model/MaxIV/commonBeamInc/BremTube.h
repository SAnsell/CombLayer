/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/BremTube.h
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
#ifndef tdcSystem_BremTube_h
#define tdcSystem_BremTube_h

class Simulation;

namespace constructSystem
{
  class portItem;  
}

namespace xraySystem
{

  /*!
  \class BremTube
  \version 1.0
  \author S. Ansell
  \date May 2020

  \brief BremTube for Max-IV
  
  Fundermentally it is a 
*/

class BremTube :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedComp,
  public attachSystem::FrontBackCut,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  double frontRadius;           ///< Front radius
  double frontLength;           ///< Front length (from mid)
  double frontFlangeRadius;     ///< flange radius
  double frontFlangeLength;     ///< flange length
 
  double midRadius;             ///< mid radius
  double midLength;             ///< mid length (from centre)

  double tubeRadius;            ///< main radius
  double tubeHeight;            ///< tube  height
  double tubeDepth;             ///< main tube  depth
  double tubeFlangeRadius;      ///< tube flange radius
  double tubeFlangeLength;      ///< tubeFlangeLength

  double backRadius;           ///< Front radius
  double backLength;           ///< back length (from centre)
  double backFlangeRadius;     ///< Front length
  double backFlangeLength;     ///< Front length

  double wallThick;             ///< pipe thickness  
  double plateThick;            ///< Joining Flange radius

  // Front ports
  std::vector<Geometry::Vec3D> FCentre;  ///< Centre points [relative to origin]
  std::vector<Geometry::Vec3D> FAxis;    ///< Port centre Axis
  /// Vector of ports FixedComp
  std::vector<std::shared_ptr<constructSystem::portItem>> FPorts;     

  // Main ports
  std::vector<Geometry::Vec3D> MCentre;  ///< Centre points [relative to origin]
  std::vector<Geometry::Vec3D> MAxis;    ///< Port centre Axis
  /// Vector of ports FixedComp
  std::vector<std::shared_ptr<constructSystem::portItem>> MPorts;     

  int voidMat;                  ///< void material
  int wallMat;                  ///< main material
  int plateMat;                 ///< plate material

  void createFrontPorts(Simulation&);
  void createBasePorts(Simulation&);
  
  void populate(const FuncDataBase&);  
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  BremTube(const std::string&);
  BremTube(const std::string&,const std::string&);
  BremTube(const BremTube&);
  BremTube& operator=(const BremTube&);
  virtual ~BremTube();

  virtual void insertInCell(Simulation&,const int) const;
  virtual void insertInCell(Simulation&,const std::vector<int>&) const;
  virtual void insertInCell(MonteCarlo::Object&) const;

  using FixedComp::createAll;
  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
