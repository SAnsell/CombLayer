/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   essBuildInc/TwisterModule.h
 *
 * Copyright (c) 2004-2023 by Stuart Ansell/Konstantin Batkov
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
#ifndef essSystem_TwisterModule_h
#define essSystem_TwisterModule_h

class Simulation;

namespace essSystem
{
/*!
  \class TwisterModule
  \author Stuart Ansell / Konstantin Batkov
  \version 1.0
  \date March 2016
  \brief Moderator twister plug
*/

class TwisterModule : 
  public attachSystem::FixedOffset,
  public attachSystem::ContainedGroup,
  public attachSystem::CellMap
{
 private:

  double shaftRadius;             ///< Shaft radius
  double shaftHeight;             ///< Shaft height
  double shaftWallThick;          ///< Shaft wall thickness
  double shaftBearingRadius;              ///< shaft bearing radius
  double shaftBearingHeight;              ///< shaft bearing height
  double shaftBearingWallThick;      ///< shaft bearing side wall thick
  
  double plugFrameRadius;         ///< plug frame outer radius
  double plugFrameHeight;         ///< plug frame height
  double plugFrameDepth;          ///< plug frame depth
  double plugFrameAngle;          ///< plug frame opening angle
  double plugFrameWallThick;      ///< plug frame wall thickness

  int plugFrameMat;               ///< plug frame material
  int plugFrameWallMat;           ///< plug frame wall material
  int shaftMat;                   ///< shaft material
  int shaftWallMat;               ///< shaft wall material

  // Functions:

  void populate(const FuncDataBase&) override;
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  TwisterModule(const std::string&);
  TwisterModule(const TwisterModule&);
  TwisterModule& operator=(const TwisterModule&);
  virtual TwisterModule* clone() const;
  ~TwisterModule() override;

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int) override;
};

}

#endif
 
