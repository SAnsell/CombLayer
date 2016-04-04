/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/RingSeal.h
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
#ifndef constructSystem_RingSeal_h
#define constructSystem_RingSeal_h

class Simulation;

namespace constructSystem
{
  
/*!
  \class RingSeal
  \version 1.0
  \author S. Ansell
  \date April 2016
  \brief RingSeal unit  
  
  Produces a divided ring seal in multiple objects by 
  fractional division.
  Ring axis along Y axis. X/Z parallel to ring.
*/

class RingSeal :
  public attachSystem::FixedOffset,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap
{
 private:
  
  const int ringIndex;         ///< Index of surface offset
  int cellIndex;                ///< Cell index  

  size_t NSection;      ///< Divide into sections
  size_t NTrack;        ///< Track number [for find cells]
  double radius;        ///< main innner radius
  double deltaRad;      ///< main innner radius
  double thick;         ///< main inner thickness
  
  int mat;             ///< Port Seal material
      
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  void generateInsert(Simulation&);
  
 public:

  RingSeal(const std::string&);
  RingSeal(const RingSeal&);
  RingSeal& operator=(const RingSeal&);
  virtual ~RingSeal();

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
