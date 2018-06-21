/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   teaSetInc/Mug.h
 *
 * Copyright (c) 2004-2018 by Konstantin Batkov
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
#ifndef teaSetSystem_Mug_h
#define teaSetSystem_Mug_h

class Simulation;

namespace teaSetSystem
{
/*!
  \class Mug
  \author K. Batkov
  \version 1.0
  \date June 2018
  \brief Example of simple geometry
*/

class Mug : public attachSystem::ContainedComp,
  public attachSystem::FixedOffset,
  public attachSystem::CellMap
{
 private:
  
  const int mugIndex;       ///< Index of surface offset
  int cellIndex;            ///< Cell index

  double radius;            ///< Mug inner radius
  double height;            ///< Mug height 
  double wallThick;         ///< Mug wall thick

  double handleRadius;      ///< Mug handle inner radius
  double handleOffset;      ///< Mug handle vertical offset

  int wallMat;              ///< wall material

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  Mug(const std::string&);
  Mug(const Mug&);
  Mug& operator=(const Mug&);
  ~Mug();

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);
    
};

}

#endif
 
