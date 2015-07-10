/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/VacuumBox.h
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#ifndef constructSystem_VacuumBox_h
#define constructSystem_VacuumBox_h

class Simulation;

namespace constructSystem
{
  
/*!
  \class VacuumBox
  \version 1.0
  \author S. Ansell
  \date July 2015
  \brief VacuumBox unit  
*/

class VacuumBox :
  public attachSystem::FixedOffset,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap
{
 private:
  
  const int vacIndex;           ///< Index of surface offset
  int cellIndex;                ///< Cell index  

  double voidHeight;            ///< void height [top only]
  double voidWidth;             ///< void width [total]
  double voidDepth;             ///< void depth [low only]
  double voidLength;            ///< void length [total]

  double feHeight;            ///< fe height [top only]
  double feDepth;             ///< fe depth [low only]
  double feWidth;             ///< fe width [total]
  double feFront;             ///< fe front 
  double feBack;              ///< fe front 

  double flangeRadius;        ///< Joining Flange radius
  double flangeLength;        ///< Joining Flange length
  double flangeWall;          ///< Joining Flange thickness
  
  int feMat;                  ///< Fe material layer
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  VacuumBox(const std::string&);
  VacuumBox(const VacuumBox&);
  VacuumBox& operator=(const VacuumBox&);
  virtual ~VacuumBox();

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
