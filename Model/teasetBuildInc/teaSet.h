/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   teaSetInc/teaSet.h
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
#ifndef teaSetSystem_teaSet_h
#define teaSetSystem_teaSet_h

class Simulation;

namespace teaSetSystem
{
/*!
  \class teaSet
  \author S. Ansell
  \version 1.0
  \date May 2015
  \brief Simple teaSet with layers

  Constructed from the front point
*/

class teaSet : public attachSystem::ContainedComp,
  public attachSystem::FixedOffset,
  public attachSystem::CellMap
{
 private:
  
  const int tubeIndex;             ///< Index of surface offset
  int cellIndex;                  ///< Cell index

  double length;            ///< Total length
  double height;            ///< Total height 
  double width;             ///< Total width

  double innerHeight;       ///< innerHeight
  double innerWidth;        ///< innerWidth

  int wallMat;              ///< wall material

  size_t nWallLayers;       ///< Layers in wall
  std::vector<double> wallFracList;
  std::vector<int> wallMatList; 
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  void layerProcess(Simulation&);

 public:

  teaSet(const std::string&);
  teaSet(const teaSet&);
  teaSet& operator=(const teaSet&);
  ~teaSet();

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);
    
};

}

#endif
 
