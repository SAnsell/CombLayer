/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/DomeConnector.h
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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
#ifndef constructSystem_DomeConnector_h
#define constructSystem_DomeConnector_h

class Simulation;

namespace constructSystem
{
  
/*!
  \class DomeConnector
  \version 1.0
  \author S. Ansell
  \date March 2021
  \brief DomeConnector for capping ports/vac-tubes etc
*/

class DomeConnector :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap,
  public attachSystem::SurfMap,
  public attachSystem::ExternalCut
{
 private:

  double curveRadius;           ///< inner Radius of dome (extent)
  double innerRadius;           ///< inner Radius of straight section
  double curveStep;             ///< Step from flat start plane
  double joinStep;              ///< length of join dome-flat plane dist
  double flatLen;               ///< direct length

  double plateThick;            ///< Thickness of plate
  double flangeRadius;          ///< Flange radius
  double flangeLen;             ///< Flange Length

  int wallMat;                  ///< Main material  
  int voidMat;                  ///< inner (Void) material

  size_t portRotateIndex;       ///< port to rotate (index+1)
  Geometry::Vec3D postZAxis;    ///< Sexondary Z axis if PortRotate
  
  constructSystem::portSet PSet;        ///< Port set
  
  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  void createPorts(Simulation&);

  void correctPortIntersect();
  
 public:

  DomeConnector(const std::string&);
  DomeConnector(const DomeConnector&);
  DomeConnector& operator=(const DomeConnector&);
  virtual ~DomeConnector();

  const portItem& getPort(const size_t) const;

  virtual void insertInCell(MonteCarlo::Object&) const;
  virtual void insertInCell(Simulation&,const int) const;

  /// accessor to port rotate
  void setPortRotate(const size_t I) { portRotateIndex=I; }
  void setPostZAxis(Geometry::Vec3D A) { postZAxis=std::move(A); }
  
  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
