/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/BremMonoColl.h
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#ifndef xraySystem_BremMonoColl_h
#define xraySystem_BremMonoColl_h

class Simulation;

namespace xraySystem
{
  
/*!
  \class BremMonoColl
  \version 1.0
  \author S. Ansell
  \date May 2018
  \brief Bremsstralung Collimator unit  
*/

class BremMonoColl :
  public attachSystem::FixedOffset,
  public attachSystem::ContainedGroup,
  public attachSystem::CellMap,
  public attachSystem::ExternalCut
{
 private:

  double radius;              ///< Main radius
  double length;              ///< Main length
  double gap;                 ///< Gap distance to start
  double wallThick;           ///< wall thickness of tube

  double flangeRadius;        ///< Joining Flange radius
  double flangeLength;        ///< Joining Flange length

  double holeXStep;            ///< X-offset of hole
  double holeZStep;            ///< Z-offset of hole
  double holeWidth;           ///< Front width of hole
  double holeHeight;          ///< Front height of hole

  double inXStep;            ///< X-offset of hole
  double inZStep;            ///< Z-offset of hole
  double inRadius;           ///< Front width of hole

  int voidMat;                ///< void material
  int innerMat;               ///< Tungsten material
  int wallMat;                ///< Fe material layer

  bool inFlag;                ///< Inlet origin set
  Geometry::Vec3D inOrg;      ///< In origin
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  
 public:

  BremMonoColl(const std::string&);
  BremMonoColl(const BremMonoColl&);
  BremMonoColl& operator=(const BremMonoColl&);
  virtual ~BremMonoColl();

  void setInOrg(const Geometry::Vec3D&);
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
