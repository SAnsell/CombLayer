/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/TwinChopperU.h
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
#ifndef constructSystem_TwinChopperU_h
#define constructSystem_TwinChopperU_h

class Simulation;

namespace constructSystem
{
  class Motor;
  class boltRing;
  class RingSeal;
  class InnerPort;
  
/*!
  \class TwinChopperU
  \version 1.0
  \author S. Ansell
  \date January 2017
  \brief TwinChopperU unit  
  
  This piece aligns away from the chopper axis. Using
  the chopper origin [bearing position]
*/

class TwinChopperU :
  public attachSystem::FixedOffsetGroup,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:
  
  const int houseIndex;          ///< Index of surface offset
  int cellIndex;                 ///< Cell index  

  Geometry::Vec3D lowOutCent;    ///< Low centre of outer metal
  Geometry::Vec3D topOutCent;    ///< Top centre of outer metal
  
  double stepHeight;             ///< height from chopper rotor centre
  double length;                 ///< total thickness
  double mainRadius;             ///< main innner radius
  double innerRadius;            ///< inner radius 
  double innerTopStep;           ///< Step of the top disk
  double innerLowStep;           ///< Step of the lower disk 
  double innerVoid;              ///< main inner thickness
  
  size_t outerRingNBolt;         ///< Outer bolts in half ring
  size_t outerLineNBolt;         ///< Outer bolts in innerHeight
  double outerBoltStep;          ///< Bolt distance from outer edge
  double outerBoltRadius;        ///< Bolt radius
  int outerBoltMat;              ///< Outer Bolt material
  
  int boltMat;                   ///< Bolt material
  int wallMat;                   ///< Wall material layer

  std::shared_ptr<Motor> motorA;           ///< Motor A
  std::shared_ptr<Motor> motorB;           ///< Motor B
  std::shared_ptr<boltRing> frontFlange;   ///< Front flange
  std::shared_ptr<boltRing> backFlange;    ///< Back flange
  std::shared_ptr<InnerPort> IPA;          ///< inner port
  std::shared_ptr<InnerPort> IPB;          ///< inner port
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

  std::string motorFrontExclude() const;
  std::string motorBackExclude() const;

  void createOuterBolts(Simulation&,const int,const Geometry::Vec3D&,
			const std::string&,const std::string&,
			const double,const size_t,
			const double,const double,
			const double,const int,const int);

  void createLineBolts(Simulation&,const int,
		       const std::string&,const std::string&,
		       const std::string&,
		       const double,const size_t,
		       const double,const int,const int);


  void createMotor(Simulation&,const std::string&,
		   std::shared_ptr<Motor>&);
  
 public:

  TwinChopperU(const std::string&);
  TwinChopperU(const TwinChopperU&);
  TwinChopperU& operator=(const TwinChopperU&);
  virtual ~TwinChopperU();

  void insertAxle(Simulation&,const attachSystem::CellMap&,
		  const attachSystem::CellMap&) const;

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
