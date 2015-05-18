/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   pipeInc/pipeTube.h
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
#ifndef pipeSystem_pipeTube_h
#define pipeSystem_pipeTube_h

class Simulation;

namespace pipeSystem
{
/*!
  \class pipeTube
  \author S. Ansell
  \version 1.0
  \date May 2015
  \brief Simple pipe with layers

  Constructed from the front point
*/

class pipeTube : public attachSystem::ContainedComp,
  public attachSystem::FixedComp,
  public attachSystem::CellMap
{
 private:
  
  const int tubeIndex;             ///< Index of surface offset
  int cellIndex;                  ///< Cell index

  double xStep;             ///< X Step
  double yStep;             ///< Y Step
  double zStep;             ///< Z Step
  double xyAngle;           ///< xy rotation angle
  double zAngle;            ///< z rotation angle

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
  void createUnitVector(const attachSystem::FixedComp&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  void layerProcess(Simulation&);

 public:

  pipeTube(const std::string&);
  pipeTube(const pipeTube&);
  pipeTube& operator=(const pipeTube&);
  ~pipeTube();

  void createAll(Simulation&,const attachSystem::FixedComp&);
    
};

}

#endif
 
