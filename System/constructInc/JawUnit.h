/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/JawUnit.h
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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
#ifndef constructSystem_JawUnit_h
#define constructSystem_JawUnit_h

class Simulation;

namespace constructSystem
{
  
/*!
  \class JawUnit
  \version 1.0
  \author S. Ansell
  \date January 2015
  \brief JawUnit unit  
*/

class JawUnit :
    public attachSystem::FixedRotate,
    public attachSystem::ContainedComp,
    public attachSystem::CellMap
{
 private:
    
  double zOpen;                 ///< Z-opening
  double zOffset;                 ///< Z-opening
  double zThick;                ///< Blade thickness [beam]
  double zHeight;               ///< Z-cross distance [vert]
  double zWidth;                ///< Z-length [away from beam]
  
  double xOpen;                 ///< X-opening
  double xOffset;               ///< X-opening
  double xThick;                ///< X-thickness
  double xHeight;               ///< X-cross distance [height]
  double xWidth;                ///< X-length [away from beam]

  double jawGap;                ///< Gap of the jaws

  int zJawMat;                  ///< Z material
  int xJawMat;                  ///< X material
  
  void populate(const FuncDataBase&) override;
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  
 public:

  JawUnit(const std::string&);
  JawUnit(const JawUnit&);
  JawUnit& operator=(const JawUnit&);
  ~JawUnit() override;

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int) override;

};

}

#endif
 
