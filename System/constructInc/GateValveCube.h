/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/GateValveCube.h
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
#ifndef constructSystem_GateValveCube_h
#define constructSystem_GateValveCube_h

class Simulation;

namespace constructSystem
{
  
/*!
  \class GateValveCube
  \version 1.0
  \author S. Ansell
  \date January 2018
  \brief GateValveCube unit  
*/

class GateValveCube :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap,
  public attachSystem::SurfMap,
  public attachSystem::FrontBackCut
{
 private:
  
  double length;                ///< Void length
  double width;                 ///< Void width (full)
  double height;                ///< height 
  double depth;                 ///< depth
  
  double wallThick;             ///< Wall thickness
  double portARadius;            ///< Port inner radius (opening)
  double portAThick;             ///< Port outer ring
  double portALen;               ///< Forward step of port

  double portBRadius;            ///< Port inner radius (opening)
  double portBThick;             ///< Port outer ring
  double portBLen;               ///< Forward step of port 
 
  bool closed;                  ///< Shutter closed
  double bladeLift;             ///< Height of blade up
  double bladeThick;            ///< moving blade thickness
  double bladeRadius;           ///< moving blade radius
  
  int voidMat;                  ///< Void material
  int bladeMat;                 ///< blade material
  int wallMat;                  ///< Pipe material
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  
 public:

  GateValveCube(const std::string&);
  GateValveCube(const GateValveCube&);
  GateValveCube& operator=(const GateValveCube&);
  virtual ~GateValveCube();

  using FixedComp::createAll;
  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
