/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   singleItem/CryoMagnetBase.h
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#ifndef simpleCaveSystem_brickShape_h
#define simpleCaveSystem_brickShape_h

class Simulation;

namespace simpleCaveSystem
{
  
/*!
  \class CryoMagnetBase
  \author S. Ansell
  \version 1.0
  \date Janurary 2015
  \brief Sample in a cylindrical sample holder
*/

class brickShape :  public attachSystem::FixedOffset,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  //  const int layerIndex;         ///< Index of surface offset
  //  int cellIndex;                ///< Cell index
  double height;  // height of the bricked wall
  double width; // width of the bricked wall
  double offsetV;
  double offsetH;
  double brickHeight; // height of the brick shape
  double brickWidth; // width of the brick
  double brickThick; // brick thickness
  double brickGapWidthD; //gaps between brick layers around the brick to simulate
  double brickGapWidthH; //horizontal gaps between brick layers
  double brickGapWidthV; //vertical gaps between the brick to simulate
  int brickMat; // Material of the bricks, e.g. concrete
  int surroundMat; // Material surrounding bricks, e.g. air

  size_t nBrickLayers;       ///< Layers in shielding plate
  std::vector<double> brickFracList;
  std::vector<int> brickMatList; 
  

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  brickShape(const std::string&);
  brickShape(const brickShape&);
  brickShape& operator=(const brickShape&);
  virtual ~brickShape();
  
  void createAll(Simulation&,
		 const attachSystem::FixedComp&,
		 const long int);
};

}

#endif
 
