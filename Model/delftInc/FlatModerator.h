/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   delftInc/FlatModerator.h
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
  int cellIndex;                 ///< Cell index

  double frontRadius;       ///< inner(front) radius
  double backRadius;        ///< Back(outer) radius
  double frontRoundRadius;  ///< inner(front) radius
  double backRoundRadius;   ///< Back(outer) radius

  double frontWallThick;    ///< Wall thickenss of view side
  double backWallThick;     ///< Wall thickenss of core side

  double wingAngle;         ///< Angle of cone
  double viewExtent;        ///< Distance across join

  
  double depth;             ///< Sphere mid dist offset
  double length;            ///< inner apex - cut plane 
  double radius;            ///< Radius of minus wall
  double sideThick;         ///< Side thickness

  
  double modTemp;           ///< Moderator temperature
  double gasTemp;           ///< Moderator temperature
  int modMat;               ///< Moderator material
  int gasMat;               ///< Moderator material
  int alMat;                ///< Al material

  int HCell;                ///< Main H2 cell
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

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

  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
			 const long int);
  virtual void postCreateWork(Simulation&);
};

}

#endif
 
