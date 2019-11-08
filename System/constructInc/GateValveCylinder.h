/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/GateValveCylinder.h
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
#ifndef constructSystem_GateValveCylinder_h
#define constructSystem_GateValveCylinder_h

class Simulation;

namespace constructSystem
{
  
/*!
  \class GateValveCylinder
  \version 1.0
  \author S. Ansell
  \date January 2018
  \brief GateValveCylinder unit  
*/

class GateValveCylinder :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap,
  public attachSystem::SurfMap,
  public attachSystem::FrontBackCut
{
 private:
  
  double length;                ///< Void length
  double radius;                ///< Radius
  double liftWidth;             ///< width of lift region
  double liftHeight;            ///< width of lift region
  
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

  GateValveCylinder(const std::string&);
  GateValveCylinder(const GateValveCylinder&);
  GateValveCylinder& operator=(const GateValveCylinder&);
  virtual ~GateValveCylinder();

  using FixedComp::createAll;
  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
