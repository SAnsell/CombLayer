/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   maxpeemInc/maxpeemOpticsHutch.h
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
#ifndef xraySystem_maxpeemOpticsHutch_h
#define xraySystem_maxpeemOpticsHutch_h

class Simulation;

namespace xraySystem
{
  class PortChicane;
/*!
  \class maxpeemOpticsHutch
  \version 1.0
  \author S. Ansell
  \date January 2018
  \brief maxpeemOpticsHutch unit  

  Built around the central beam axis
*/

class maxpeemOpticsHutch :
  public attachSystem::FixedOffset,
  public attachSystem::ContainedComp,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:
  
  double height;                ///< void depth
  double length;                ///< void length
  double outWidth;              ///< void out side width
  double ringShortWidth;        ///< void ring side width [short]
  double ringLongWidth;         ///< void ring side width [long]

  double shortLen;              ///< length to end of short len
  double fullLen;               ///< length to full width

  double innerSkin;            ///< Inner wall/roof skin
  double pbWallThick;           ///< Thickness of lead in walls
  double pbFrontThick;          ///< Thickness of lead in front plate
  double pbBackThick;           ///< Thickness of lead in back plate
  double pbRoofThick;           ///< Thickness of lead in Roof
  double outerSkin;            ///< Outer wall/roof skin

  double innerOutVoid;          ///< Extension for inner left void space
  double outerOutVoid;          ///< Extension for outer left void space 
  
  double inletXStep;            ///< Inlet XStep
  double inletZStep;            ///< Inlet ZStep  
  double inletRadius;           ///< Inlet radius

  double holeXStep;            ///< Hole XStep
  double holeZStep;            ///< Hole ZStep  
  double holeRadius;           ///< Hole radius

  int innerMat;                ///< Fe layer material for walls
  int pbMat;                  ///< pb layer material for walls 
  int outerMat;                ///< Conc layer material for ring walls

  /// Chicanes 
  std::vector<std::shared_ptr<PortChicane>> PChicane;  
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  void createChicane(Simulation&);

 public:

  maxpeemOpticsHutch(const std::string&);
  maxpeemOpticsHutch(const maxpeemOpticsHutch&);
  maxpeemOpticsHutch& operator=(const maxpeemOpticsHutch&);
  virtual ~maxpeemOpticsHutch();

  void createAll(Simulation&,
		 const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
