/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/Capillary.h
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
#ifndef saxsSystem_Capillary_h
#define saxsSystem_Capillary_h

class Simulation;

namespace saxsSystem
{
  
/*!
  \class Capillary
  \version 1.0
  \author S. Ansell
  \date March 2019
  \brief Capillary unit  
*/

class Capillary :
  public attachSystem::FixedOffset,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:
  
  double radius;                ///< inner radius 
  double length;                ///< total length

  double wallThick;             ///< wall thickness 
    
  int innerMat;                 ///< Inner material
  int wallMat;                  ///< Wall material 
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  
 public:

  Capillary(const std::string&);
  Capillary(const Capillary&);
  Capillary& operator=(const Capillary&);
  virtual ~Capillary();
  
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
