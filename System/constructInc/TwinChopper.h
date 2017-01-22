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

  Geometry::Vec3D BY;       ///< Old reorientation
  
  double height;            ///< height from chopper rotor centre
  double width;             ///< width [across]
  double depth;             ///< depth 
  double length;            ///< length [total]
  double shortHeight;       ///< half corner (for diamond)
  double shortWidth;        ///< half corner (for diamond)
  
  double mainRadius;        ///< main innner radius
  double mainThick;         ///< main inner thickness

  double motorRadius;           ///< motor port radius
  double motorOuter;           ///< Extrernal radius of motor port
  double motorStep;             ///< motor flange step
  
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
  
  size_t motorNBolt;            ///< number of motor bolts  
  double motorBoltRad;          ///< Bolt radius
  double motorBoltAngOff;       ///< angle relative to 12:00
  double motorSeal;             ///< Motor seal
  int motorSealMat;             ///< Motor Seal material
  
  int motorMat;                 ///< Motor material
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
 
