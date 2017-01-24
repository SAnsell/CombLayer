/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/TwinChopper.h
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#ifndef constructSystem_TwinChopper_h
#define constructSystem_TwinChopper_h

class Simulation;

namespace constructSystem
{
  class RingSeal;
  class InnerPort;
  
/*!
  \class TwinChopper
  \version 1.0
  \author S. Ansell
  \date January 2017
  \brief TwinChopper unit  
  
  This piece aligns away from the chopper axis. Using
  the chopper origin [bearing position]
*/

class TwinChopper :
  public attachSystem::FixedOffsetGroup,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap
{
 private:
  
  const int houseIndex;         ///< Index of surface offset
  int cellIndex;                ///< Cell index  

  Geometry::Vec3D lowCentre;      ///< Low centre
  Geometry::Vec3D topCentre;      ///< Top centre 
  
  double stepHeight;            ///< height from chopper rotor centre
  double length;                ///< total thickness
  double mainRadius;            ///< main innner radius
  double innerRadius;           ///< inner radius 
  double innerTopStep;          ///< Step of the top disk
  double innerLowStep;          ///< Step of the lower disk 
  double innerVoid;             ///< main inner thickness

  
  double portRadius;           ///< Port radius
  double portOuter;            ///< Port flange [outer radius
  double portStep;             ///< Port step [unused]
  double portWindow;           ///< Port window thickness
  
  size_t portNBolt;            ///< Number of port bolts
  double portBoltRad;          ///< Bolt radius
  double portBoltAngOff;       ///< Angle to start relative to 12:00
  double portSeal;             ///< Port seal
  int portSealMat;             ///< Port Seal material
  int portWindowMat;           ///< Window material
  
  double motorARadius;           ///< motor port radius
  double motorAOuter;            ///< Extrernal radius of motor port
  double motorAStep;             ///< motor flange step
  size_t motorANBolt;            ///< number of motor bolts  
  double motorABoltRad;          ///< Bolt radius
  double motorABoltAngOff;       ///< angle relative to 12:00
  double motorASeal;             ///< Motor seal Thinkness
  int motorASealMat;             ///< Motor Seal material
  int motorAMat;                 ///< Motor material
  
  double motorBRadius;           ///< motor port radius
  double motorBOuter;            ///< Extrernal radius of motor port
  double motorBStep;             ///< motor flange step
  size_t motorBNBolt;            ///< number of motor bolts  
  double motorBBoltRad;          ///< Bolt radius
  double motorBBoltAngOff;       ///< angle relative to 12:00
  double motorBSeal;             ///< Motor seal Thinkness
  int motorBSealMat;             ///< Motor Seal material
  int motorBMat;                 ///< Motor material  

  int boltMat;                  ///< Bolt material
  int wallMat;                  ///< Wall material layer

  std::shared_ptr<RingSeal> RS;   ///< ringseal for main system
  std::shared_ptr<InnerPort> IPA; ///< inner port
  std::shared_ptr<InnerPort> IPB; ///< inner port
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

  void createRing(Simulation&,const int,const Geometry::Vec3D&,
		  const std::string&,const std::string&,
		  const double,const size_t,const double,
		  const double,
		  const std::string&,const int);
  
 public:

  TwinChopper(const std::string&);
  TwinChopper(const TwinChopper&);
  TwinChopper& operator=(const TwinChopper&);
  virtual ~TwinChopper();

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
