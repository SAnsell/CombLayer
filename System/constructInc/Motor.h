/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/Motor.h
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
#ifndef constructSystem_Motor_h
#define constructSystem_Motor_h

class Simulation;

namespace constructSystem
{
  class boltRing;
  
/*!
  \class Motor
  \version 1.0
  \author S. Ansell
  \date April 2016
  \brief Motor unit  
  
  Produces a sealed inner port with bolts within a space
  Ring axis along Y axis. X/Z parallel to ring.
*/

class Motor :
  public attachSystem::FixedOffset,
  public attachSystem::ContainedGroup,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:
  
  const int motorIndex;         ///< Index of surface offset
  int cellIndex;                ///< Cell index  

  int frontInner;                  ///< Front inner surf
  int backInner;                   ///< Back inner surf

  int revFlag;                  ///< Reverse motor flag
  double bodyLength;            ///< length out of port
  double plateThick;            ///< Main thickness
  
  double axleRadius;            ///< radius of centre axel
  double portInnerRadius;       ///< inner port radius
  double portOuterRadius;       ///< outer port radius
  double boltRadius;            ///< radius
  double bodyRadius;            ///< body radius

  size_t nBolt;                 ///< number of bolts
  double angOffset;             ///< Offset angle
  
  int boltMat;                  ///< material
  int bodyMat;                  ///< main motor material
  int axleMat;                  ///< axle material
  int plateMat;                 ///< plate material

  double yFront;                ///< Front y-step
  double yBack;                 ///< back y-step

  std::shared_ptr<boltRing> frontPlate;     ///< front flange
  std::shared_ptr<boltRing> backPlate;      ///< back flange

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  void createSurfaces();
  void createObjects(Simulation&);
  void createPlates(Simulation&);
  void createLinks();
  
 public:

  Motor(const std::string&);
  Motor(const Motor&);
  Motor& operator=(const Motor&);
  virtual ~Motor();

  /// set inner planes -- REALLY UGLY
  void setInnerPlanes(const int F,const int B)
  { frontInner=F; backInner=B; }

  /// set Ystep on inner planes -- REALLY UGLY
  void setYSteps(const double F,const double B)
    { yFront=F;yBack=B; }
  
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);
};

}

#endif
 
