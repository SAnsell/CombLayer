/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   softimaxInc/M1Frame.h
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
#ifndef xraySystem_M1Frame_h
#define xraySystem_M1Frame_h

class Simulation;

namespace xraySystem
{
  
/*!
  \class M1Frame
  \author S. Ansell
  \version 1.0
  \date September 2023
  \brief Outer Main support structure for mirror
*/

class M1Frame :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedComp,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap,
  public attachSystem::SurfMap,
  public attachSystem::PointMap
{
 private:

  // top edge supports:

  double bladeIRadius;              ///< Outer radius
  double bladeORadius;              ///< Outer radius
  double bladeThick;               ///< Thickness of one blade
  double bladeTopAngle;            ///< Angle to stop the top part of circle
  double bladeBaseAngle;           ///< Angle to stop the base part of cube
  double bladeBaseWidth;           ///< Full width of base
  double bladeBaseHeight;          ///< Height to down cut
  
  int supportMat;           ///< Base material
  int voidMat;              ///< outer pipe material

  // Functions:

  void populate(const FuncDataBase&) override;
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  M1Frame(const std::string&);
  M1Frame(const M1Frame&);
  M1Frame& operator=(const M1Frame&);
  ~M1Frame() override;

  using FixedComp::createAll;
  void createAll(Simulation&,
		 const attachSystem::FixedComp&,
		 const long int) override;
  
};

}

#endif
 
