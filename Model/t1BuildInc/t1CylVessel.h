/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t1BuildInc/t1CylVessel.h
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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
#ifndef ShutterSystem_t1CylVessel_h
#define ShutterSystem_t1CylVessel_h

class Simulation;

namespace shutterSystem
{

/*!
  \class t1CylVessel
  \author S. Ansell
  \version 1.0
  \date October 2012
  \brief Specialized for for the ts1 void vessel
*/

class t1CylVessel :
    public attachSystem::FixedRotate,
    public attachSystem::ContainedComp,
    public attachSystem::CellMap
{
 private:
  
  double radius;                  ///< Main cylinder radius
  double clearance;               ///< Void clearance
  double baseRadius;              ///< Base cap radius
  double topRadius;               ///< Top Cap radius
  double wallThick;               ///< Main wall thickness
  double height;                  ///< Main height [not including caps]

  int wallMat;                     ///< Wall material

  /// View port
  std::vector<constructSystem::Window> ports;

  // Functions:

  void populate(const FuncDataBase&) override;
  void createSurfaces();
  void createObjects(Simulation&);
  void createWindows(Simulation&);
  void createLinks();

 public:

  t1CylVessel(const std::string&);
  t1CylVessel(const t1CylVessel&);
  t1CylVessel& operator=(const t1CylVessel&);
  ~t1CylVessel() override;

  /// Access void cell
  //  int getVoidCell() const { return voidCell; }
  
  /// Outer radius
  double getOuterRadius() const 
    { return radius+wallThick+clearance; }
  void createStatus(const Simulation&,
		    const attachSystem::FixedComp&,
		    const long int);

  using FixedComp::createAll;
  void createAll(Simulation&,
		 const attachSystem::FixedComp&,
		 const long int) override;
  
};

}

#endif
 
