/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   photonInc/VacuumVessel.h
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#ifndef photonSystem_VacuumVessel_h
#define photonSystem_VacuumVessel_h

class Simulation;

namespace constructSystem
{
  class RingSeal;
  class RingFlange;
}

namespace photonSystem
{
  
/*!
  \class VacuumVessel
  \author S. Ansell
  \version 1.0
  \date October 2016
  \brief Specialized for a vacuum vessel

  Cylindrical with semi-spherical front/back Doors
*/

class VacuumVessel : public attachSystem::ContainedComp,
  public attachSystem::FixedOffset,
  public attachSystem::CellMap
{
 private:

  const int vacIndex;         ///< Index of surface offset
  int cellIndex;                ///< Cell index

  double wallThick;            ///< Outer thickenss
    
  double frontLength;          ///< length of front part
  double radius;               ///< radius [inneer]

  double backLength;             ///< length of front part
  double width;               ///< width of back volume
  double height;              ///< height of back volume
  
  double doorRadius;            ///< Door out step
  double doorEdge;            ///< Door out step
  double doorThick;            ///< Door out step

  int voidMat;                 ///< inner material
  int doorMat;                     ///< Outer material
  int mat;                     ///< Outer material

  /// central port
  std::shared_ptr<constructSystem::RingFlange> CentPort;
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  void buildPorts(Simulation&);
  
 public:

  VacuumVessel(const std::string&);
  VacuumVessel(const VacuumVessel&);
  VacuumVessel& operator=(const VacuumVessel&);
  virtual ~VacuumVessel();
  
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);
};

}

#endif
 
