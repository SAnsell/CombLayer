/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/RingSeal.h
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
  public attachSystem::FixedRotate,
  public attachSystem::ContainedComp,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap
{
 protected:
  
  size_t NSection;      ///< Divide into sections
  size_t NTrack;        ///< Track number [for find cells]
  double radius;        ///< main innner radius
  double deltaRad;      ///< main innner radius
  double deltaAngle;      ///< main innner radius
  double thick;         ///< main inner thickness
  
  int mat;              ///< Port Seal material

  bool standardInsert;          ///< If set do standard object insert
  int setFlag;                  ///< Structures set

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

  /// Normal object insert path
  void setStandardInsert() { standardInsert=1; }


  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
