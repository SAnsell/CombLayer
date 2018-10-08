/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/TSMainBuilding.h
 *
 * Copyright (c) 2016 by Konstantin Batkov
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
#ifndef essSystem_TSMainBuilding_h
#define essSystem_TSMainBuilding_h

class Simulation;

namespace essSystem
{

/*!
  \class TSMainBuilding
  \version 1.0
  \author Konstantin Batkov
  \date DATE
  \brief Target station main building
*/

class TSMainBuilding : public attachSystem::ContainedComp,
  public attachSystem::FixedComp,public attachSystem::CellMap
{
 private:
   
  int engActive;                ///< Engineering active flag

  double xStep;                   ///< X step
  double yStep;                   ///< Y step
  double zStep;                   ///< Z step
  double xyAngle;                 ///< XY Angle
  double zAngle;                  ///< Z Angle

  double length;                ///< Total length including void
  double width;                 ///< Width
  double height;                ///< height
  double depth;                 ///< depth

  int mainMat;                   ///< main material
  int roomMat;                   ///< room material  
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&);
  
  void createSurfaces();
  void createLinks();
  void createObjects(Simulation&);

  
 public:

  TSMainBuilding(const std::string&);
  TSMainBuilding(const TSMainBuilding&);
  TSMainBuilding& operator=(const TSMainBuilding&);
  virtual ~TSMainBuilding();
  
  void createAll(Simulation&,const attachSystem::FixedComp&);

};

}

#endif
 

