/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   zoomInc/ZoomStack.h
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
#ifndef zoomSystem_ZoomStack_h
#define zoomSystem_ZoomStack_h

class Simulation;
class varBlock;

namespace zoomSystem
{
  class ZoomChopper;

/*!
  \class ZoomStack
  \version 1.0
  \author S. Ansell
  \date January 2010
  \brief Zoom Stack unit
  
  The guide is defined along a line that leads from the centre line
  of the shutter. The origin is the point that the shutter centre 
  line impacts the outer surface
*/

class ZoomStack :  public attachSystem::ContainedComp,
  public attachSystem::TwinComp
{
 private:
  
  const int stackIndex;         ///< Index of surface offset
  int cellIndex;                ///< Cell index

  size_t nItem;                   ///< Number of items
  size_t posIndex;                ///< Position index
  
  double length;                ///< Full length
  double height;                 ///< Full width

  std::vector<varBlock> Items;  ///< List of items
  int voidCell;                ///< Cell to insert to.

  void populate(const Simulation&);
  void createUnitVector(const attachSystem::TwinComp&);
  
  void createSurfaces();
  void createObjects(Simulation&);

 public:

  ZoomStack(const std::string&);
  ZoomStack(const ZoomStack&);
  ZoomStack& operator=(const ZoomStack&);
  virtual ~ZoomStack();

  void createAll(Simulation&,const attachSystem::TwinComp&);

  int exitWindow(const double,std::vector<int>&,
		 Geometry::Vec3D&) const;

};

}

#endif
 
