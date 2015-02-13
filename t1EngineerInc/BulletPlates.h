/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   t1BuildInc/BulletPlates.h
 *
 * Copyright (c) 2004-2015 by Stuart Ansell/Goran Skoro
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
#ifndef ts1System_BulletPlates_h
#define ts1System_BulletPlates_h

class Simulation;

namespace ts1System
{

/*!
  \class BulletPlates
  \version 1.0
  \author S. Ansell
  \date February 2015
  \brief W/Ta plates 
*/

class BulletPlates : public attachSystem::ContainedComp,
    public attachSystem::FixedComp
{
 private:
  
  const int ptIndex;            ///< Index of surface offset
  int cellIndex;                ///< Cell index

  double xStep;                 ///< X position
  double yStep;                 ///< Y position
  double zStep;
  double xyAngle;
  double zAngle;
  
  size_t nBlock;                ///< nBlock
  std::vector<double> tBlock;   ///< Vector of block thickness
  std::vector<std::string> blockType;   ///< Vector of block types

  double radius;                ///< Inner W radius
  double taThick;               ///< Ta thickness

  int taMat;                    ///< Ta material
  int wMat;                     ///< W material [def]
  int waterMat;                 ///< Water material

  void populate(const FuncDataBase&);  
  void createUnitVector(const attachSystem::FixedComp&);
  void createSurfaces();
  void createLinks();
  void createObjects(Simulation&);

 public:

  BulletPlates(const std::string&);
  BulletPlates(const BulletPlates&);
  BulletPlates& operator=(const BulletPlates&);
  ~BulletPlates();

  /// acessor to main radius
  double getMainRadius() const { return radius+taThick; }
  void createAll(Simulation&,const attachSystem::FixedComp&);

};

}

#endif
 
