/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essConstructInc/SingleChopper.h
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
#ifndef constructSystem_SingleChopper_h
#define constructSystem_SingleChopper_h

class Simulation;

namespace constructSystem
{
  class boltRing;
  class RingSeal;
  class InnerPort;
  class Motor;
  
/*!
  \class SingleChopper
  \version 1.0
  \author S. Ansell
  \date March 2016
  \brief SingleChopper unit  
  
  This piece aligns away from the chopper axis. Using
  the chopper origin [bearing position]
*/

class SingleChopper :
  public attachSystem::FixedOffsetGroup,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
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
  
  int boltMat;                  ///< Bolt material
  int wallMat;                  ///< Wall material layer

  std::shared_ptr<Motor> motor;            ///< Motor 
  std::shared_ptr<boltRing> frontFlange;   ///< Front flange
  std::shared_ptr<boltRing> backFlange;    ///< Back flange
  std::shared_ptr<RingSeal> RS;            ///< ringseal for main system
  std::shared_ptr<InnerPort> IPA;          ///< inner port
  std::shared_ptr<InnerPort> IPB;          ///< inner port
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

  void createMotor(Simulation&);
  
 public:

  SingleChopper(const std::string&);
  SingleChopper(const SingleChopper&);
  SingleChopper& operator=(const SingleChopper&);
  virtual ~SingleChopper();

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

  void insertAxle(Simulation&,const attachSystem::CellMap&) const;
};

}

#endif
 
