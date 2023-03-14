/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   softimaxInc/M1BackPlate.h
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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
#ifndef xraySystem_M1BackPlate_h
#define xraySystem_M1BackPlate_h

class Simulation;

namespace xraySystem
{

/*!
  \class M1BackPlate
  \author S. Ansell
  \version 1.0
  \date January 2018
  \brief Focusable mirror in mount
*/

class M1BackPlate :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedComp,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  double length;              ///< Length of support 
  double clearGap;            ///< Clearance gap to crystal
  double backThick;           ///< Thickness of back 
  double mainThick;           ///< Thickness of C-cups
  double extentThick;         ///< Thickness of extras
  double cupHeight;           ///< Height of C-cups 
  double topExtent;           ///< Length of top step
  double baseExtent;          ///< Length of top step

  int baseMat;                ///< Base material
  int voidMat;                ///< void material

  // Functions:

  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  M1BackPlate(const std::string&);
  M1BackPlate(const M1BackPlate&);
  M1BackPlate& operator=(const M1BackPlate&);
  virtual ~M1BackPlate();

  using FixedComp::createAll;
  void createAll(Simulation&,
		 const attachSystem::FixedComp&,
		 const long int);
  
};

}

#endif
 
