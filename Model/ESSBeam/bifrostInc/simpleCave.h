/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   pipeInc/simpleCave.h
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
#ifndef simpleCaveSystem_simpleCave_h
#define simpleCaveSystem_simpleCave_h

class Simulation;

namespace simpleCaveSystem
{
/*!
  \class simpleCave
  \author S. Ansell
  \version 1.0
  \date May 2015
  \brief Simple pipe with layers

  Constructed from the front point
*/

class simpleCave : 
  public attachSystem::FixedOffsetGroup,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap
{
 private:
  
  // const int caveIndex;             ///< Index of surface offset
  // int cellIndex;                  ///< Cell index

  double innerLengthFront;            ///< distance front wall - center
  double innerLengthBack;            ///< distance center - back
  double innerHeight;            ///< distance center - ceiling
  double innerDepth;             ///< distance center - floor
  double innerWidthRight;   ///< distance center - right wall
  double innerWidthLeft;    ///< distance center - left wall
  double roofExtraThick;// additional plate above the roof
  double roofThick;
  //Hatch on top:
  /*  double hatchFront;
  double hatchBack;
  double hatchLeft;
  double hatchRight;
  double lockThick;
  double lockCapThick;
  int lockCapMat;
  int beamstopMat;
  int lockMat;
  size_t lockCapNLayers; //layers in the lock cap
  std::vector<double> capFracList;
  std::vector<int> capMatList; 
  double doglegWidth, doglegLength, doglegDepth;
  */
  double wallThick;       ///< innerHeight
   double floorThick;
   //  double cutRadius;

  int wallMat;              ///< wall material
  int roofMat;              ///< wall material
  int innerMat;     // Material inside the cave (air or void)

  int iLayerFlag;
  
  size_t nWallLayers;       ///< Layers in wall
  size_t nRoofLayers;       ///< Layers in wall
  size_t nFloorLayers;

  std::vector<double> wallFracList;
  std::vector<int> wallMatList; 
  std::vector<double> floorFracList;
  std::vector<int> floorMatList; 
  std::vector<double> roofFracList;
  std::vector<int> roofMatList; 
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  //  void layerProcess(Simulation&);

 public:
  void layerProcess(Simulation&);

  simpleCave(const std::string&);
  simpleCave(const simpleCave&);
  simpleCave& operator=(const simpleCave&);
  ~simpleCave();

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);
    
};

}

#endif
 
