/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/VacTank.h
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
#ifndef essSystem_VacTank_h
#define essSystem_VacTank_h

class Simulation;

namespace essSystem
{

/*!
  \class VacTank
  \version 1.0
  \author S. Ansell
  \date July 2015
  \brief VacTank [insert object]
*/

class VacTank : public attachSystem::ContainedComp,
  public attachSystem::FixedOffset,
  public attachSystem::CellMap
{
 private:
  
  const int tankIndex;          ///< Index of surface offset
  int cellIndex;                ///< Cell index

  double radius;                ///< Radius
  double length;                ///< length [void]
  double frontThick;            ///< Side thickness
  double sideThick;             ///< Side thickness
  double backThick;             ///< Side thickness
  double noseLen;               ///< Nose cone length

  double windowThick;           ///< Window thickness
  double windowRadius;          ///< Window radius
  double windowInsetLen;        ///< Length fof insert window

  int wallMat;                  ///< Material for walls
  int windowMat;                ///< Material for window

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
  void createLinks();
  void createObjects(Simulation&);

 public:

  VacTank(const std::string&);
  VacTank(const VacTank&);
  VacTank& operator=(const VacTank&);
  ~VacTank();

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
