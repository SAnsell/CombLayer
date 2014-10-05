/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   delftInc/FlatModerator.h
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#ifndef delftSystem_FlatModerator_h
#define delftSystem_FlatModerator_h

class Simulation;

namespace delftSystem
{

/*!
  \class FlatModerator
  \version 1.0
  \author S. Ansell
  \date July 2012
  \brief FlatModerator [insert object]
*/

class FlatModerator : public virtualMod
{
 private:
  
  const int flatIndex;           ///< Index of surface offset
  int cellIndex;                ///< Cell index

  double xStep;             ///< X Step
  double yStep;             ///< YC Step
  double zStep;             ///< Z Step

  double backRad;           ///< Back radius
  double frontRad;          ///< Back radius
  double depth;             ///< Centre-Centre mid dist
  double length;            ///< inner apex - cut plane 
  double radius;            ///< Radius of minus wall
  double sideThick;         ///< Side thickness

  double wallThick;         ///< Wall thickenss
  
  double modTemp;           ///< Moderator temperature
  double gasTemp;           ///< Moderator temperature
  int modMat;               ///< Moderator material
  int gasMat;               ///< Moderator material
  int alMat;                ///< Al material

  int HCell;                ///< Main H2 cell
  
  void populate(const Simulation&);
  void createUnitVector(const attachSystem::SecondTrack&);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  FlatModerator(const std::string&);
  FlatModerator(const FlatModerator&);
  FlatModerator& operator=(const FlatModerator&);
  virtual FlatModerator* clone() const;
  virtual ~FlatModerator();

  /// Access to hydrogen region
  virtual int getMainBody() const { return HCell; }

  virtual void createAll(Simulation&,const attachSystem::TwinComp&);
  virtual void postCreateWork(Simulation&);
};

}

#endif
 
