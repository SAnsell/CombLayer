/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/InnerPort.h
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#ifndef constructSystem_InnerPort_h
#define constructSystem_InnerPort_h

class Simulation;

namespace constructSystem
{
  
/*!
  \class InnerPort
  \version 1.0
  \author S. Ansell
  \date April 2016
  \brief InnerPort unit  
  
  Produces a sealed inner port with bolts within a space
  Ring axis along Y axis. X/Z parallel to ring.
*/

class InnerPort :
  public attachSystem::FixedOffset,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap,
  public attachSystem::ExternalCut
{
 private:
  

  double width;        ///< width of void
  double height;       ///< height of void
  double length;       ///< thickness of void

  size_t nBolt;        ///< Number of bolts
  double boltStep;     ///< Step from inner surf
  double boltRadius;   ///< Radius of bolt
  int boltMat;         ///< bolt material

  double sealStep;     ///< Step from inner surf
  double sealThick;    ///< Thickness of seal
  int sealMat;         ///< seal material

  double window;       ///< window thickness
  int windowMat;       ///< window material 
  
  int mat;             ///< Port Seal material

  std::set<int> activeCells;   ///< Cells containing port
  bool calcIntersect(const Geometry::Vec3D&) const;
  
  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  void generateInsert();
  void createBolts(Simulation&);

 public:

  InnerPort(const std::string&);
  InnerPort(const InnerPort&);
  InnerPort& operator=(const InnerPort&);
  virtual ~InnerPort();

  void addInnerCell(const int);
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
