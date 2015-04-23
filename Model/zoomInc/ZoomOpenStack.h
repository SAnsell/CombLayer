/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   zoomInc/ZoomOpenStack.h
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
#ifndef zoomSystem_ZoomOpenStack_h
#define zoomSystem_ZoomOpenStack_h

class Simulation;
class varBlock;

namespace zoomSystem
{
  class ZoomChopper;

/*!
  \class ZoomOpenStack
  \version 1.0
  \author S. Ansell
  \date January 2015
  \brief Zoom Open Stack unit

  Big open stack to represent the new stack system 
*/

class ZoomOpenStack :  public attachSystem::ContainedComp,
  public attachSystem::TwinComp
{
 private:
  
  const int stackIndex;         ///< Index of surface offset
  int cellIndex;                ///< Cell index

  size_t nItem;                 ///< Number of items
  size_t posIndex;              ///< Position index

  double xStep;                ///< X offset
  double yStep;                ///< Y offset
  double zStep;                ///< Z offset
  
  double width;                 ///< Total width of inner void
  double height;                ///< Total height of inner void
  double length;                ///< Total length of inner void
  
  double wallThick;             ///< Outer  wall thickness
  double windowThick;           ///< Window thickness

  
  double guideOffset;           ///< Step from front/back
  double guideSep;              ///< Cent-Cent guide separation
  double guideWidth;            ///< Guide Width
  double guideHeight;           ///< Guide height
  double guideThick;            ///< Guide thickness
  int guideMat;                 ///< Guide material
  
  int wallMat;               ///< Wall material
  int windowMat;             ///< Window material
  

  int voidCell;                ///< Cell to insert to.

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::TwinComp&);
  void createSurfaces();
  void createObjects(Simulation&);

 public:

  ZoomOpenStack(const std::string&);
  ZoomOpenStack(const ZoomOpenStack&);
  ZoomOpenStack& operator=(const ZoomOpenStack&);
  virtual ~ZoomOpenStack();

  void createAll(Simulation&,const int,
		 const attachSystem::TwinComp&);

};

}

#endif
 
