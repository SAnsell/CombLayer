/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/R3ChokeChamber.h
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
#ifndef xraySystem_R3ChokeChamber_h
#define xraySystem_R3ChokeChamber_h

class Simulation;

namespace xraySystem
{
  
/*!
  \class R3ChokeChamber
  \version 1.0
  \author S. Ansell
  \date January 2018
  \brief R3ChokeChamber unit  
*/

class R3ChokeChamber :
  public attachSystem::FixedOffset,
  public attachSystem::ContainedGroup,
  public attachSystem::CellMap,
  public attachSystem::SurfMap,
  public attachSystem::ExternalCut
{
 private:

  double radius;              ///< radius of main tube
  double wallThick;           ///< wall thickness of main tube
  double length;              ///< Main length
  double flangeRadius;        ///< Main Flange radius
  double flangeLength;        ///< Main Flange length

  double inletXStep;           ///< Inlex X Step
  double inletWidth;           ///< In pipe radius
  double inletHeight;          ///< In pipe height
  double inletLength;          ///< In pipe length [inc flange]
  double inletThick;           ///< In pipe wall thickness
  double flangeInletRadius;    ///< In Flange radius
  double flangeInletLength;    ///< In Flange length

  double electronXStep;          ///< Electron X step
  double electronXYAngle;        ///< Electron Angle 
  double electronRadius;         ///< Electron out radius
  double electronLength;         ///< Electron out lenght
  double electronThick;          ///< Electron pipe wall thickness
  double flangeElectronRadius;   ///< Electron Flange radius
  double flangeElectronLength;   ///< Electron Flange length
  
  double photonXStep;          ///< Photon X step
  double photonRadius;         ///< Photon out radius
  double photonLength;         ///< Photon out lenght
  double photonThick;          ///< Photon  wall thickness
  double flangePhotonRadius;   ///< Photon Flange radius
  double flangePhotonLength;   ///< Photon Flange length

  double sideRadius;           ///< Side out radius
  double sideLength;           ///< Side out lenght
  double sideThick;            ///< Side pipe wall thickness
  double flangeSideRadius;     ///< Side Flange radius
  double flangeSideLength;     ///< Side Flange length
  
  int voidMat;                ///< void material
  int wallMat;                ///< Wall material 
  int flangeMat;              ///< Flange material 

  bool epPairSet;                 ///< Setting of phot/elec Origin.
  Geometry::Vec3D photOrg;        ///< Photon origin
  Geometry::Vec3D elecOrg;        ///< Electron origin
  
  Geometry::Vec3D elecXAxis;      ///< Electron X-axis
  Geometry::Vec3D elecYAxis;      ///< Electron beam axis

  Geometry::Vec3D flangeOrg;      ///< flange origin

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  
 public:

  R3ChokeChamber(const std::string&);
  R3ChokeChamber(const R3ChokeChamber&);
  R3ChokeChamber& operator=(const R3ChokeChamber&);
  virtual ~R3ChokeChamber();

  void setEPOriginPair(const attachSystem::FixedComp&,
		       const long int,const long int);

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
