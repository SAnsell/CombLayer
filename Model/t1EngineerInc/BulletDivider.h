/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t1EngineerInc/BulletDivider.h
 *
 * Copyright (c) 2004-2017 by Stuart Ansell/Goran Skoro
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
#ifndef ts1System_BulletDivider_h
#define ts1System_BulletDivider_h

class Simulation;

namespace ts1System
{

/*!
  \class BulletDivider
  \version 1.0
  \author S. Ansell/G. Skoro
  \date February 2015
  \brief Water Channel divider plates (curved)
*/

class BulletDivider : public attachSystem::ContainedComp,
    public attachSystem::FixedComp
{
 private:

  const int ID;                 ///< ID number
  const std::string baseName;   ///< Base name [no id]
  const int divIndex;           ///< Index of surface offset
  int cellIndex;                ///< Cell index

  const int divDirection;             ///< Divide direction
  double YStep;                 ///< Distance +/- from link point
  
  std::vector<double> radii;    ///< Cone/Cylinder radii
  std::vector<double> length;   ///< Cone/Cylinder length

  std::vector<Geometry::Vec3D> endPts;  ///< End points of vanes
  double wallThick;             ///< Wall thickness

  int wallMat;                  ///<  Wall material (Ta)xs
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&);
  void createSurfaces();
  void createObjects(Simulation&,
		     const attachSystem::FixedComp&,const long int,
		     const attachSystem::FixedComp&,const long int,
		     const long int);
  void createLinks();
  void setInnerRadius(const attachSystem::FixedComp&,const long int);
  void excludeInsertCells(Simulation& System);
  
 public:

  BulletDivider(const std::string&,const int,const int);
  BulletDivider(const BulletDivider&);
  BulletDivider& operator=(const BulletDivider&);
  ~BulletDivider();
  
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const attachSystem::FixedComp&);

};

}

#endif
 
