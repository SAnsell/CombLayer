/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   photonInc/ModContainer.h
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
#ifndef photonSystem_ModContainer_h
#define photonSystem_ModContainer_h

class Simulation;

namespace constructSystem
{
  class RingSeal;
  class RingFlange;
}

namespace photonSystem
{
  
/*!
  \class ModContainer
  \author S. Ansell
  \version 1.0
  \date Janurary 2015
  \brief Specialized for a layered cylinder Moderator
*/

class ModContainer :
    public attachSystem::ContainedComp,
    public attachSystem::FixedRotate,
    public attachSystem::CellMap
{
 private:


  double length;               ///< length of object
  double radius;               ///< radius [inneer]
  double thick;                ///< Outer thickenss

  int mat;                     ///< Outer material
  double temp;                 ///< temperature

  /// Front [flat] flange
  std::shared_ptr<constructSystem::RingFlange> FrontFlange;
  /// Back [with hole] flange
  std::shared_ptr<constructSystem::RingFlange> BackFlange;

  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

  void buildFlanges(Simulation&);
  
 public:

  ModContainer(const std::string&);
  ModContainer(const ModContainer&);
  ModContainer& operator=(const ModContainer&);
  virtual ~ModContainer();
  virtual ModContainer* clone() const;

  const attachSystem::FixedComp& getFrontFlange() const;
  const attachSystem::FixedComp& getBackFlange() const;


  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);
};

}

#endif
 
