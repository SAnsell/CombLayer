/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   photonInc/TubeMod.h
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
#ifndef photonSystem_TubeMod_h
#define photonSystem_TubeMod_h

class Simulation;

namespace photonSystem
{

struct TUnit
{
  Geometry::Vec3D Offset; ///< Start offset
  double radius;          ///< Radius
  double height;          ///< Height
  int mat;                ///< Matieral    
};

  
/*!
  \class TubeMod
  \author S. Ansell
  \version 1.0
  \date Janurary 2015
  \brief Specialized for a layered cylinder Moderator
*/

class TubeMod : public attachSystem::ContainedComp,
   public attachSystem::FixedComp
{
 private:

  const int modIndex;         ///< Index of surface offset
  int cellIndex;                ///< Cell index

  double xStep;                 ///< X-Step
  double yStep;                 ///< Y-Step
  double zStep;                 ///< Z-Step
  double xyAngle;               ///< Angle rotation [deg]
  double zAngle;                ///< horizontal rotation [deg]

  double outerRadius;           ///< Outer volume
  double outerHeight;           ///< Outer height [from origin]
  int outerMat;                 ///< Outer material

  std::vector<TUnit> Tubes;  ///< Extra tubes
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  
 public:

  TubeMod(const std::string&);
  TubeMod(const TubeMod&);
  TubeMod& operator=(const TubeMod&);
  virtual ~TubeMod();
  virtual TubeMod* clone() const;
  
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);
};

}

#endif
 
