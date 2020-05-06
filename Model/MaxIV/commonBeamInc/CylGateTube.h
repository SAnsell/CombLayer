/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/CylGateTube.h
 *
 * Copyright (c) 2004-2020 by Stuart Ansell 
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
#ifndef xraySystem_CylGateTube_h
#define xraySystem_CylGateTube_h

class Simulation;

namespace xraySystem
{
  
/*!
  \class CylGateTube
  \version 1.0
  \author S. Ansell
  \date January 2018
  \brief CylGateTube unit  
*/

class CylGateTube :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap,
  public attachSystem::SurfMap,
  public attachSystem::ExternalCut
{
 private:
  
  double radius;                ///< Radius
  double depth;                 ///< Void down depth
  double height;                ///< void lift height

  double wallThick;             ///< Wall thickness

  double portRadius;            ///< Port inner radius (opening)
  double portFlangeRadius;      ///< Port inner radius (opening)
  double portInner;             ///< Port inner step
  double portThick;             ///< Port outer step

  double topRadius;             ///< Top outer radius
  double topHoleRadius;         ///< Top hole radius
  double topThick;              ///< Top hole radius

  double liftHeight;            ///< Lift tube height
  double liftRadius;            ///< lift tube inner radius
  double liftThick;             ///< lift tube thickness
  double liftPlate;             ///< lift tube plate cover [if no motor]

  double driveRadius;           ///< drive radius
  
  bool closed;                  ///< Shutter closed

  double bladeLift;             ///< Height of blade up
  double bladeThick;            ///< moving blade thickness
  double bladeRadius;           ///< moving blade radius
  
  int voidMat;                  ///< Void material
  int bladeMat;                 ///< blade material
  int driveMat;                 ///< blade material
  int wallMat;                  ///< Pipe material
  
  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  
 public:

  CylGateTube(const std::string&);
  CylGateTube(const CylGateTube&);
  CylGateTube& operator=(const CylGateTube&);
  virtual ~CylGateTube();

  using FixedComp::createAll;
  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
