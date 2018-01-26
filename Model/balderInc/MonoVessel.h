/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   balderInc/MonoVessel.h
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
#ifndef xraySystem_MonoVessel_h
#define xraySystem_MonoVessel_h

class Simulation;

namespace xraySystem
{
  
/*!
  \class MonoVessel
  \version 1.0
  \author S. Ansell
  \date July 2016
  \brief MonoVessel unit  

  The vacuum system of the mono
*/

class MonoVessel :
  public attachSystem::FixedOffset,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:
  
  const int monoIndex;          ///< Index of surface offset
  int cellIndex;                ///< Cell index  

  double radius;                 ///< void radius
  double ringWidth;              ///< void width to ring side
  double outWidth;               ///< void width to door


  double wallThick;             ///< steel wall (round)
  double doorThick;             ///< door side thickness
  double backThick;             ///< back side thickness

  double doorFlangeLen;         ///< Flange length on door side
  double doorFlangeRad;         ///< Flange extra radius on door side

  double ringFlangeLen;         ///< Flange length on ring side
  double ringFlangeRad;         ///< Flange extra radius on ring side


  double inPortZStep;           ///< Drop of inport
  double inPortRadius;          ///< Incomming port size
  double inPortLen;             ///< Incomming port length
  double inPortThick;           ///< Incomming port thickness    
  double inPortFlangeRad;       ///< Incomming port flange rad
  double inPortFlangeLen;       ///< Incomming port flange lenght

  double outPortZStep;           ///< Raize of outport
  double outPortRadius;          ///< OutGoing port size
  double outPortLen;             ///< OutGoing port length
  double outPortThick;           ///< OutGoing port thickness    
  double outPortFlangeRad;       ///< OutGoing port flange rad
  double outPortFlangeLen;       ///< OutGoing port flange lenght

  int voidMat;                   ///< Void material
  int wallMat;                   ///< Wall material

  Geometry::Vec3D inPortPt;      ///< Intersection point with void cylinder
  Geometry::Vec3D outPortPt;     ///< Intersection point with void cylinder
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  MonoVessel(const std::string&);
  MonoVessel(const MonoVessel&);
  MonoVessel& operator=(const MonoVessel&);
  virtual ~MonoVessel();

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
