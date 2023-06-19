/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   formaxInc/MLMSupportWheel.h
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
#ifndef xraySystem_MLMSupportWheel_h
#define xraySystem_MLMSupportWheel_h

class Simulation;

namespace xraySystem
{

/*!
  \class MLMSupportWheel
  \author S. Ansell
  \version 1.0
  \date October 2019
  \brief Double Mirror Mono arrangement
*/

class MLMSupportWheel :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  double wheelRadius;          ///< main wheel radius
  double wheelOuterRadius;     ///< full outer reaius
  double wheelHubRadius;       ///< Hub radius
  double wheelHeight;          ///< Thickness of wheel

  size_t nSpokes;              ///< nummber of slots
  double spokeThick;           ///< spoke thickness
  double spokeCornerRadius;    ///< nice radius in corner
  double spokeCornerGap;       ///< gap at thin points
   
  int mat;                     ///< main material
  int voidMat;                 ///< Void / vacuum material
  
  // Functions:

  void populate(const FuncDataBase&) override;
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  MLMSupportWheel(const std::string&);
  MLMSupportWheel(const MLMSupportWheel&);
  MLMSupportWheel& operator=(const MLMSupportWheel&);
  ~MLMSupportWheel() override;

  using FixedComp::createAll;
  void createAll(Simulation&,
		 const attachSystem::FixedComp&,
		 const long int) override;
  
};

}

#endif
 
