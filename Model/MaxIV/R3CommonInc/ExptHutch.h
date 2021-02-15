/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   R3CommonInc/ExptHutch.h
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#ifndef xraySystem_ExptHutch_h
#define xraySystem_ExptHutch_h

class Simulation;

namespace xraySystem
{
  class PortChicane;
  
/*!
  \class ExptHutch
  \version 1.0
  \author S. Ansell
  \date January 2018
  \brief ExptHutch unit  

  Built around the central beam axis
*/

class ExptHutch :
  public attachSystem::FixedOffset,
  public attachSystem::ContainedComp,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:
  
  double height;                ///< void height to roof (from origin)
  double length;                ///< void out side width
  double ringWidth;             ///< void flat part to ring
  double outWidth;              ///< out side width

  double cornerAngle;           ///< angle at corner
  double cornerLength;          ///< length to corner

  double innerThick;            ///< Inner wall/roof skin
  double pbWallThick;           ///< Thickness of lead in walls
  double pbFrontThick;          ///< Thickness of lead in front plate
  double pbBackThick;           ///< Thickness of lead in back plate
  double pbRoofThick;           ///< Thickness of lead in Roof
  double outerThick;            ///< Outer wall/roof skin

  double innerOutVoid;          ///< Extension for inner void space
  double outerOutVoid;          ///< Extension for outer void space 
  double extension;
  
  int voidMat;                ///< Void material [air]
  int skinMat;                ///< Fe layer material for walls
  int pbMat;                  ///< pb layer material for walls

  /// Chicanes 
  std::vector<std::shared_ptr<PortChicane>> PChicane;  
  
  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

  void createChicane(Simulation&);

 public:

  ExptHutch(const std::string&);
  ExptHutch(const ExptHutch&);
  ExptHutch& operator=(const ExptHutch&);
  virtual ~ExptHutch();

  using FixedComp::createAll;
  virtual void createAll(Simulation&,
			 const attachSystem::FixedComp&,
			 const long int);

};

}

#endif
 
