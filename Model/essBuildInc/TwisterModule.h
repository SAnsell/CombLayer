/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   essBuildInc/TwisterModule.h
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
#ifndef essSystem_TwisterModule_h
#define essSystem_TwisterModule_h

class Simulation;

namespace essSystem
{
/*!
  \class TwisterModule
  \author K. Batkov
  \version 1.0
  \date March 2016
  \brief Moderator twister plug
*/

class TwisterModule : public attachSystem::ContainedComp,
  public attachSystem::FixedComp,
  public attachSystem::CellMap
{
 private:

  const int tIndex;               ///< Index of surface offset
  int cellIndex;                  ///< Cell index

  int engActive;                  ///< Engineering active flag

  double xStep;                   ///< X step
  double yStep;                   ///< Y step
  double zStep;                   ///< Z step
  double xyAngle;                 ///< XY Angle
  double zAngle;                  ///< Z Angle

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

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  TwisterModule(const std::string&);
  TwisterModule(const TwisterModule&);
  TwisterModule& operator=(const TwisterModule&);
  virtual TwisterModule* clone() const;
  virtual ~TwisterModule();

  void createAll(Simulation&,const attachSystem::FixedComp&);
};

}

#endif
 
