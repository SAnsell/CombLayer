/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/FullBunker.h
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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
#ifndef essSystem_Bunker_h
#define essSystem_Bunker_h

class Simulation;

namespace essSystem
{

  class BunkerMainWall;
  class BunkerRoof;
  class BunkerWall;
  class BunkerInsert;
  class Chicane;  
/*!
  \class FullBunker
  \version 1.0
  \author S. Ansell
  \date October 2023
  \brief Bulk around Reflector

  The full bunker is built from the proton beam direction as +Y
  Z is vertical and X as either left or right (set by angles)
*/

class FullBunker : 
  public attachSystem::FixedComp,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:
       
  double longAngle;              ///< Extent of left angle [-ve is -X]
  double midAngle;               ///< Extent of mid wall
  double smallAngle;             ///< Extent of lower angle

  // MAIN WALL
  size_t nSectors;               ///< Number of sector divisions
  std::vector<double> sectPhase; ///< sector angles

  size_t nSegment;               ///< Number of sections in a segment
  std::vector<double> segDivide; ///< Segment divider

  size_t nVert;                  ///< Number of vertical divisions
  std::vector<double> vertFrac;  ///< Vertical fraction

  size_t nLayers;                ///< number of outgoing layers
  std::vector<double> wallFrac;  ///< thicknesss (fractions)

  double smallRadius;            ///< inner radius [calculated]
  double largeRadius;            ///< inner radius [calculated]
  double floorDepth;             ///< Floor depth
  double wallHeight;             ///< Wall height (allows roof to intercolate)
  double roofHeight;             ///< Roof height


  double wallThick;              ///< Wall thickness
  double sideThick;              ///< Side thickness
  double roofThick;              ///< Roof thickness
  double floorThick;             ///< Floor thickness

  int voidMat;                   ///< void material 
  int wallMat;                   ///< wall material
  int roofMat;                   ///< roof material
  
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int) override;

  void createSurfaces();
  void createLinks();
  void createObjects(Simulation&);
  void layerProcess(Simulation&);

 public:

  FullBunker(const std::string&);
  FullBunker(const FullBunker&);
  FullBunker& operator=(const FullBunker&);
  ~FullBunker() override;


  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int) override;


  
};

}

#endif
 

